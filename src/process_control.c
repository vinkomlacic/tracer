#include <stdint.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <signal.h>
#include <stdbool.h>

#include "t_error.h"
#include "procmem.h"
#include "log.h"
#include "process_info.h"
#include "process_control.h"


static void inject_indirect_call_at(intptr_t address, char const function_name[], int arg, pstate_t *pstate);
static void inject_code(pstate_t *pstate, unsigned code_size, uint8_t const code[]);


extern void pattach(pid_t const pid) {
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
        t_errno = T_EPTRACE;
        return;
    }


    DEBUG("PTRACE_ATTACH executed. Waiting for stop signal.");
    if (waitpid(pid, NULL, WSTOPPED) == -1) {
        t_errno = T_EWAIT;
    }
    DEBUG("Tracee process stopped. Tracee is attached.");
}


extern void pcontinue(pid_t const pid) {
    DEBUG("rip: %#llx", get_regs(pid).rip);
    if (ptrace(PTRACE_CONT, pid, 0, SIGCONT) == -1) {
        t_errno = T_EPTRACE;
    }
    DEBUG("SIGCONT sent");
}


extern void set_breakpoint(intptr_t const address, pstate_t * const pstate) {
    if (has_changes(pstate) == false) pstate->change_address = address;

    uint8_t const interrupt[] = {0xCC};
    inject_code(pstate, sizeof(interrupt), interrupt);
}


extern void wait_for_bp(pid_t const pid) {
    DEBUG("Waiting for breakpoint");
    if (waitpid(pid, NULL, 0) == -1) {
        t_errno = T_EWAIT;
    }
    DEBUG("Breakpoint caught or process died");
}


extern int call_function(pstate_t * const pstate, char const function_to_call[const], int const arg) {
    // TODO validate pstate
    // TODO extract address in a function
    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(address, function_to_call, arg, pstate);
    if (error_occurred()) return -1;

    intptr_t breakpoint_address = get_address_after_changes(pstate);
    set_breakpoint(breakpoint_address, pstate);
    if (error_occurred()) return -1;

#ifdef DEBUG_ENABLE
    uint8_t bytes[4] = {0};
    for (int i = 0; i < 4; i++) {
        bytes[i] = proc_read_byte(pstate->pid, pstate->change_address + i);
    }
    DEBUG("======== Changed memory = %#x %#x %#x %#x", bytes[0], bytes[1], bytes[2], bytes[3]);
#endif

    DEBUG("Executing indirect call and interrupt");
    pcontinue(pstate->pid);
    if (error_occurred()) return -1;

    wait_for_bp(pstate->pid);
    if (error_occurred()) return -1;

    int ret_value = (int) get_regs(pstate->pid).rax;
    if (error_occurred()) return -1;

    return ret_value;
}


extern void pdetach(pid_t const pid) {
    DEBUG("rip: %#llx", get_regs(pid).rip);
    if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) {
        t_errno = T_EPTRACE;
    }
    DEBUG("Process %d detached", pid);
}


static void inject_indirect_call_at(
        intptr_t const address,
        char const function_name[const],
        int const arg,
        pstate_t * const pstate
) {
    if (has_changes(pstate) == false) pstate->change_address = address;

    intptr_t const function_address = get_symbol_address_in_target(pstate->name, function_name);
    if (function_address == 0L && error_occurred()) return;
    DEBUG("Function address in %s: %#lx", pstate->name, function_address);

    DEBUG("Setting up indirect call %%rax = %#lx, %%rdi = %d", function_address, arg);
    struct user_regs_struct regs = get_regs(pstate->pid);
    regs.rax = (unsigned long long int) function_address;
    regs.rdi = (unsigned long long int) arg;
    if (ptrace(PTRACE_SETREGS, pstate->pid, NULL, &regs) == -1) {
        t_errno = T_EPTRACE;
        return;
    }

    DEBUG("Injecting indirect call to the process memory");
    uint8_t const indirect_call[] = {0xFF, 0xD0};
    inject_code(pstate, sizeof(indirect_call), indirect_call);
}


static void inject_code(pstate_t * const pstate, unsigned const code_size, uint8_t const code[const]) {
    TRACE("Saving old code to pstate");
    intptr_t start_address = get_address_after_changes(pstate);
    for (size_t i = 0; i < code_size; i++) {
        intptr_t address = start_address + i;
        uint8_t byte = proc_read_byte(pstate->pid, address);
        if (error_occurred()) return;

        proc_write_byte(pstate->pid, address, code[i]);
        if (error_occurred()) return;

        append_code_to_pstate(pstate, byte);
    }
    TRACE("Injected code to the process memory");
}