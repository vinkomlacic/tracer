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

#include "process_control.h"


static void inject_indirect_call_at(pstate_t *pstate, intptr_t address, intptr_t function_address);
static void set_arguments(pid_t pid, size_t argc, unsigned long long const argv[]);
static unsigned long long get_return_value(pid_t pid);
static void execute_injections(pid_t pid);
static intptr_t allocate_stack_variable(pid_t pid);
static void deallocate_stack_variable(pid_t pid);


extern void inject_breakpoint(pstate_t * const pstate, intptr_t const address) {
    uint8_t const interrupt[] = {0xCC};

    save_process_code(pstate, address, sizeof(interrupt));
    if (error_occurred()) return;

    inject_code_to_process(pstate->pid, address, sizeof(interrupt), interrupt);
}


extern void inject_code_to_process(pid_t const pid, intptr_t const address, size_t const code_size, uint8_t const code[const]) {
    for (size_t i = 0; i < code_size; i++) {
        intptr_t current_address = address + i;
        proc_write_byte(pid, current_address, code[i]);
        if (error_occurred()) return;
    }
    TRACE("Injected code to the process memory");
}


extern void inject_trampoline(pid_t const pid, intptr_t const function_address, intptr_t const address) {
    uint8_t const jump_instruction[] = {0x48, 0xB8};
    uint8_t const end_instruction[] = {0xFF, 0xE0, 0xC3};
    DEBUG("First instruction that will be replaced: %#lx", proc_read_byte(pid, address));

    intptr_t current_address = address;
    inject_code_to_process(pid, current_address, sizeof(jump_instruction), jump_instruction);
    current_address += sizeof(jump_instruction);
    if (error_occurred()) return;

    proc_write_word(pid, current_address, function_address);
    current_address += sizeof(function_address);
    if (error_occurred()) return;

    inject_code_to_process(pid, current_address, sizeof(end_instruction), end_instruction);
    DEBUG("Injected trampoline to %#lx at %#lx", function_address, address);
#ifdef DEBUG_ENABLE
    DEBUG("Looking up injected code in tracee to verify");
    for (size_t i = 0; i < 13; i++) {
        DEBUG("<%#lx>: %#x", address + i, proc_read_byte(pid, address + i));
    }
#endif
}


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


extern intptr_t call_posix_memalign(pstate_t * const pstate, size_t const alignment, size_t const size) {
    intptr_t const function_address = get_symbol_address_in_libc(pstate->pid, "posix_memalign");
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


extern void call_mprotect(pstate_t * const pstate, intptr_t const start_address, size_t const length, int const prot) {
    intptr_t const function_address = get_mprotect_address(pstate->pid);
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
    intptr_t const function_address = get_symbol_address_in_libc(pstate->pid, "__libc_free@@GLIBC");
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


extern void scrub_virus(pid_t const pid, intptr_t const start_address, size_t const size) {
    for (size_t i = 0; i < size; i++) {
        intptr_t address = start_address + i;
        proc_write_byte(pid, address, 0x00);
        if (error_occurred()) return;
    }
}


static void inject_indirect_call_at(pstate_t * const pstate, intptr_t const address, intptr_t const function_address) {
    DEBUG("Setting up indirect call %%rax = %#lx", function_address);
    struct user_regs_struct regs = get_regs(pstate->pid);
    if (error_occurred()) return;

    regs.rax = (unsigned long long int) function_address;

    set_regs(pstate->pid, &regs);
    if (error_occurred()) return;

    DEBUG("Injecting indirect call to the process memory");
    uint8_t const indirect_call[] = {0xFF, 0xD0, 0xCC};
    save_process_code(pstate, address, sizeof(indirect_call));
    inject_code_to_process(pstate->pid, address, sizeof(indirect_call), indirect_call);
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


static unsigned long long get_return_value(pid_t const pid) {
    struct user_regs_struct regs = get_regs(pid);
    if (error_occurred()) return 0;

    return regs.rax;
}


static void execute_injections(pid_t const pid) {
    DEBUG("Executing injected code");
    pcontinue(pid);
    if (error_occurred()) return;

    wait_for_breakpoint(pid);
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