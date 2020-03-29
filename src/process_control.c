#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "t_error.h"
#include "procmem.h"
#include "log.h"
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
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
        raise(T_EWAIT, "%d", pid);
        return;
    }

    if (WSTOPSIG(wstatus) != SIGTRAP) {
        raise(T_EUNEXPECTED_STOP, "%d", pid);
        return;
    }
    DEBUG("SIGTRAP caught in %d", pid);
}


extern int call_virus(pstate_t * const pstate, intptr_t const virus_address, int const arg) {
    unsigned long long int const argv[] = {(unsigned long long int) arg};
    size_t const argc = 3;
    set_arguments(pstate->pid, argc, argv);
    if (error_occurred()) return -1;

    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, address, virus_address);
    if (error_occurred()) return -1;
    DEBUG("Indirect call to %#lx injected at %#lx", virus_address, address);

    execute_injections(pstate->pid);
    if (error_occurred()) return -1;
    DEBUG("Injected virus code executed");

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
        raise(T_EARG_INVAL, "argc not in [1, 6]: %lu", argc);
        return;
    }

    unsigned long long int * const arg_regs[] = {&regs.rdi, &regs.rsi, &regs.rdx, &regs.rcx, &regs.r8, &regs.r9};
    for (size_t i = 0; i < argc; i++) {
        *arg_regs[i] = argv[i];
        TRACE("Argument set for function call:  [#%lu] = %llu", i, argv[i]);
    }

    set_regs(pid, &regs);
}


static void execute_injections(pid_t const pid) {
    pcontinue(pid);
    if (error_occurred()) return;
    TRACE("SIGCONT sent to %d", pid);

    wait_for_breakpoint(pid);
}


static unsigned long long get_return_value(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return 0;

    return regs.rax;
}


extern intptr_t call_posix_memalign(
        pstate_t * const pstate, intptr_t const posix_memalign_address,
        int const alignment, size_t const size
) {
    intptr_t stack_variable = allocate_stack_variable(pstate->pid);
    if (error_occurred()) return -1;
    DEBUG("Stack variable allocated, address: %#lx", stack_variable);

    unsigned long long int const argv[] = {(unsigned long long int) stack_variable, (unsigned) alignment, size};
    size_t const argc = 3;
    set_arguments(pstate->pid, argc, argv);

    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, address, posix_memalign_address);
    if (error_occurred()) return -1;
    DEBUG("Injected indirect call to %#lx at %#lx", posix_memalign_address, address);

    execute_injections(pstate->pid);
    if (error_occurred()) return -1;

    int return_value = (int) get_return_value(pstate->pid);
    if (return_value != 0) {
        raise(T_ECALL_FAIL, "posix_memalign returned %d", return_value);
        return -1;
    }
    DEBUG("Function posix_memalign returned successfully");

    intptr_t block_address = proc_read_word(pstate->pid, stack_variable);
    if (error_occurred()) return -1;
    DEBUG("Allocated block address retrieved from local variable: %#lx", block_address);

    deallocate_stack_variable(pstate->pid);
    if (error_occurred()) return -1;
    DEBUG("Local variable deallocated and stack restored");

    return block_address;
}


static intptr_t allocate_stack_variable(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return -1;

    regs.rsp -= 8ULL;
    TRACE("%%rsp decremented by 8, new value: %#llx", regs.rsp);

    set_regs(pid, &regs);
    if (error_occurred()) return -1;

    return (intptr_t) regs.rsp;
}


static void deallocate_stack_variable(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return;

    regs.rsp += 8ULL;
    TRACE("%%rsp incremented by 8, new value: %#llx", regs.rsp);

    set_regs(pid, &regs);
}


extern void call_mprotect(
        pstate_t * const pstate, intptr_t const mprotect_address,
        intptr_t const start_address, size_t const length, int const prot
) {
    unsigned long long int const argv[] = {(unsigned long long int) start_address, length, (unsigned) prot};
    size_t const argc = 3;
    set_arguments(pstate->pid, argc, argv);
    if (error_occurred()) return;

    intptr_t address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, address, mprotect_address);
    if (error_occurred()) return;
    DEBUG("Injected indirect call to %#lx at %#lx", mprotect_address, address);

    execute_injections(pstate->pid);
    if (error_occurred()) return;

    int return_value = (int) get_return_value(pstate->pid);
    if (error_occurred()) return;

    if (return_value != 0) {
        raise(T_ECALL_FAIL, "mprotect returned %d", return_value);
        return;
    }
    DEBUG("mprotect returned successfully");
}


extern void call_free(pstate_t * const pstate, intptr_t const free_address, intptr_t const address) {
    unsigned long long int const argv[] = {(unsigned long long int) address};
    size_t const argc = 1;
    set_arguments(pstate->pid, argc, argv);
    if (error_occurred()) return;

    intptr_t const indirect_call_address = get_address_after_changes(pstate);
    inject_indirect_call_at(pstate, indirect_call_address, free_address);
    if (error_occurred()) return;
    DEBUG("Injected indirect call to %#lx at %#lx", free_address, address);

    execute_injections(pstate->pid);
    if (error_occurred()) return;
    DEBUG("free returned successfully");
}
