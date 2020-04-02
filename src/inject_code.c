#include "pstate.h"
#include "t_error.h"
#include "procmem.h"
#include "log.h"
#include "ptrace_wrapper.h"

#include "inject_code.h"


extern void inject_breakpoint(pstate_t * const pstate, intptr_t const address) {
    if (pstate == NULL) {
        RAISE(T_ENULL_ARG, "pstate");
        return;
    }

    uint8_t const interrupt[] = {0xCC};

    save_process_code(pstate, address, sizeof(interrupt));
    if (error_occurred()) return;
    DEBUG("Saved original %lu bytes to pstate", sizeof(interrupt));

    proc_write_block(pstate->pid, address, sizeof(interrupt), interrupt);
    DEBUG("Breakpoint injected at %#lx", address);
}


extern void inject_indirect_call_at(pstate_t * const pstate, intptr_t const address, intptr_t const function_address) {
    struct user_regs_struct regs = get_regs(pstate->pid);
    if (error_occurred()) return;

    regs.rax = (unsigned long long int) function_address;

    set_regs(pstate->pid, &regs);
    if (error_occurred()) return;
    DEBUG("%%rax = %#lx", function_address);

    uint8_t const indirect_call[] = {0xFF, 0xD0, 0xCC};
    save_process_code(pstate, address, sizeof(indirect_call));
    if (error_occurred()) return;
    DEBUG("Saved original %lu bytes to pstate", sizeof(indirect_call));

    proc_write_block(pstate->pid, address, sizeof(indirect_call), indirect_call);
    if (error_occurred()) return;
    DEBUG("Code injected to process %d", pstate->pid);
}


extern void inject_trampoline(pid_t const pid, intptr_t const address, intptr_t const function_address) {
    uint8_t const jump_instruction[] = {0x48, 0xB8};
    uint8_t const end_instruction[] = {0xFF, 0xE0, 0xC3};

    intptr_t current_address = address;
    proc_write_block(pid, current_address, sizeof(jump_instruction), jump_instruction);
    current_address += (intptr_t) sizeof(jump_instruction);
    if (error_occurred()) return;

    proc_write_word(pid, current_address, function_address);
    current_address += (intptr_t) sizeof(function_address);
    if (error_occurred()) return;
    DEBUG("Injected jump instruction to %#lx at %#lx", function_address, address);

    proc_write_block(pid, current_address, sizeof(end_instruction), end_instruction);
    if (error_occurred()) return;
    DEBUG("Injected end instruction at %#lx", current_address);
}


extern void scrub_memory(pid_t pid, intptr_t start_address, size_t size) {
    for (size_t i = 0; i < size; i++) {
        intptr_t address = start_address + (intptr_t) i;
        proc_write_byte(pid, address, 0x00);
        if (error_occurred()) return;
    }
}
