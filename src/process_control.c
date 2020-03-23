#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "t_error.h"
#include "procmem.h"
#include "log.h"
#include "process_info.h"
#include "pstate.h"
#include "ptrace_wrapper.h"
#include "inject_code.h"

#include "process_control.h"


static void set_arguments(pid_t pid, size_t argc, unsigned long long const argv[]);
static unsigned long long get_return_value(pid_t pid);
static void execute_injections(pid_t pid);
static intptr_t allocate_stack_variable(pid_t pid);
static void deallocate_stack_variable(pid_t pid);


extern void wait_for_breakpoint(pid_t const pid) {
    DEBUG("Waiting for breakpoint");
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
        raise(T_EWAIT, "%d", pid);
    }

    if (WSTOPSIG(wstatus) != SIGTRAP) {
        // TODO put a detailed error code here
        raise(T_ERROR, "process %d stopped unexpectedly", pid);
    }
    DEBUG("Breakpoint caught");
}


extern int call_virus(pstate_t * const pstate, intptr_t const virus_address, int const arg) {
    unsigned long long int const argv[] = {arg};
    size_t const argc = 3;
    set_arguments(pstate->pid, argc, argv);
    if (error_occurred()) return -1;

    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, address, virus_address);
    if (error_occurred()) return -1;

#ifdef DEBUG_ENABLE
    uint8_t bytes[4] = {0};
    for (int i = 0; i < 4; i++) {
        bytes[i] = proc_read_byte(pstate->pid, pstate->change_address + i);
    }
    DEBUG("======== Changed memory = %#x %#x %#x %#x", bytes[0], bytes[1], bytes[2], bytes[3]);
#endif

    execute_injections(pstate->pid);
    if (error_occurred()) return -1;

    return (int) get_return_value(pstate->pid);
}


/**
 * Supports up to 6 arguments. More than 6 arguments means they have to be put on the stack.
 * This could be put in another function if needed.
 */
static void set_arguments(pid_t const pid, size_t const argc, unsigned long long const argv[const static 1]) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return;
    if (argc > 6 || argc == 0) {
        // TODO add detailed error code
        raise(T_ERROR, "argc not in [1, 6]: %lu", argc);
    }

    unsigned long long int * const arg_regs[] = {&regs.rdi, &regs.rsi, &regs.rdx, &regs.rcx, &regs.r8, &regs.r9};
    for (size_t i = 0; i < argc; i++) {
        *arg_regs[i] = argv[i];
    }

    set_regs(pid, &regs);
}


static void execute_injections(pid_t const pid) {
    DEBUG("Executing injected code");
    pcontinue(pid);
    if (error_occurred()) return;

    wait_for_breakpoint(pid);
}


static unsigned long long get_return_value(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return 0;

    return regs.rax;
}


extern intptr_t call_posix_memalign(pstate_t * const pstate, size_t const alignment, size_t const size) {
    intptr_t const function_address = get_symbol_address_in_libc(pstate->pid, POSIX_MEMALIGN_GLIBC_NAME);
    if (function_address == 0L && error_occurred()) return -1;
    DEBUG("Function address in %s: %#lx", pstate->name, function_address);

    intptr_t stack_variable = allocate_stack_variable(pstate->pid);
    if (error_occurred()) return -1;

    unsigned long long int const argv[] = {stack_variable, alignment, size};
    size_t const argc = 3;
    set_arguments(pstate->pid, argc, argv);

    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, address, function_address);
    if (error_occurred()) return -1;

#ifdef DEBUG_ENABLE
    uint8_t bytes[4] = {0};
    for (int i = 0; i < 4; i++) {
        bytes[i] = proc_read_byte(pstate->pid, pstate->change_address + i);
    }
    DEBUG("======== Changed memory = %#x %#x %#x %#x", bytes[0], bytes[1], bytes[2], bytes[3]);
#endif

    execute_injections(pstate->pid);
    if (error_occurred()) return -1;

    int return_value = (int) get_return_value(pstate->pid);
    if (return_value != 0) {
        // TODO place more detailed error
        raise(T_ERROR, "function call unsuccessful (returned %d)", return_value);
        return -1;
    }
    DEBUG("Function posix_memalign returned successfully");

    intptr_t ret_value = proc_read_word(pstate->pid, stack_variable);
    if (error_occurred()) return -1;

    DEBUG("Restoring stack");
    deallocate_stack_variable(pstate->pid);

    return ret_value;
}


static intptr_t allocate_stack_variable(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return -1;

    regs.rsp -= 8ULL;

    set_regs(pid, &regs);
    if (error_occurred()) return -1;

    return regs.rsp;
}


static void deallocate_stack_variable(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return;

    regs.rsp += 8ULL;

    set_regs(pid, &regs);
}


extern void call_mprotect(pstate_t * const pstate, intptr_t const start_address, size_t const length, int const prot) {
    intptr_t const function_address = get_symbol_address_in_libc(pstate->pid, MPROTECT_GLIBC_NAME);
    if (function_address == 0L && error_occurred()) return;
    DEBUG("Function address in %s: %#lx", pstate->name, function_address);

    unsigned long long int const argv[] = {start_address, length, prot};
    size_t const argc = 3;
    set_arguments(pstate->pid, argc, argv);
    if (error_occurred()) return;

    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, address, function_address);
    if (error_occurred()) return;

#ifdef DEBUG_ENABLE
        uint8_t bytes[4] = {0};
    for (int i = 0; i < 4; i++) {
        bytes[i] = proc_read_byte(pstate->pid, pstate->change_address + i);
    }
    DEBUG("======== Changed memory = %#x %#x %#x %#x", bytes[0], bytes[1], bytes[2], bytes[3]);
#endif

    execute_injections(pstate->pid);
    if (error_occurred()) return;

    int return_value = (int) get_return_value(pstate->pid);
    DEBUG("Return value = %d", return_value);
    if (return_value != 0) {
        // TODO put detailed error code
        raise(T_ERROR, "calling mprotect failed (returned %d)", return_value);
        return;
    }
    DEBUG("mprotect returned successfully");
}


extern void call_free(pstate_t * const pstate, intptr_t const address) {
    intptr_t const function_address = get_symbol_address_in_libc(pstate->pid, FREE_GLIBC_NAME);
    if (function_address == 0L && error_occurred()) return;
    DEBUG("Function address in %s: %#lx", pstate->name, function_address);

    unsigned long long int const argv[] = {address};
    size_t const argc = 1;
    set_arguments(pstate->pid, argc, argv);
    if (error_occurred()) return;

    intptr_t const indirect_call_address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, indirect_call_address, function_address);
    if (error_occurred()) return;

#ifdef DEBUG_ENABLE
        uint8_t bytes[4] = {0};
    for (int i = 0; i < 4; i++) {
        bytes[i] = proc_read_byte(pstate->pid, pstate->change_address + i);
    }
    DEBUG("======== Changed memory = %#x %#x %#x %#x", bytes[0], bytes[1], bytes[2], bytes[3]);
#endif

    execute_injections(pstate->pid);
}
