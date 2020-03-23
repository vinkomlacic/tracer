#include "pstate.h"
#include "t_error.h"
#include "procmem.h"
#include "log.h"
#include "ptrace_wrapper.h"

#include "inject_code.h"


extern void inject_breakpoint(pstate_t * const pstate, intptr_t const address) {
    uint8_t const interrupt[] = {0xCC};

    save_process_code(pstate, address, sizeof(interrupt));
    if (error_occurred()) return;

    inject_raw_code_to_process(pstate->pid, address, sizeof(interrupt), interrupt);
}


extern void inject_raw_code_to_process(pid_t pid, intptr_t address, size_t code_size, const uint8_t *code) {
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
    inject_raw_code_to_process(pid, current_address, sizeof(jump_instruction), jump_instruction);
    current_address += sizeof(jump_instruction);
    if (error_occurred()) return;

    proc_write_word(pid, current_address, function_address);
    current_address += sizeof(function_address);
    if (error_occurred()) return;

    inject_raw_code_to_process(pid, current_address, sizeof(end_instruction), end_instruction);
    DEBUG("Injected trampoline to %#lx at %#lx", function_address, address);
#ifdef DEBUG_ENABLE
    DEBUG("Looking up injected code in tracee to verify");
    for (size_t i = 0; i < 13; i++) {
        DEBUG("<%#lx>: %#x", address + i, proc_read_byte(pid, address + i));
    }
#endif
}


extern void inject_indirect_call_at(pstate_t * const pstate, intptr_t const address, intptr_t const function_address) {
    DEBUG("Setting up indirect call %%rax = %#lx", function_address);
    struct user_regs_struct regs = get_regs(pstate->pid);
    if (error_occurred()) return;

    regs.rax = (unsigned long long int) function_address;

    set_regs(pstate->pid, &regs);
    if (error_occurred()) return;

    DEBUG("Injecting indirect call to the process memory");
    uint8_t const indirect_call[] = {0xFF, 0xD0, 0xCC};
    save_process_code(pstate, address, sizeof(indirect_call));
    inject_raw_code_to_process(pstate->pid, address, sizeof(indirect_call), indirect_call);
}


extern void scrub_virus(pid_t const pid, intptr_t const start_address, size_t const size) {
    for (size_t i = 0; i < size; i++) {
        intptr_t address = start_address + i;
        proc_write_byte(pid, address, 0x00);
        if (error_occurred()) return;
    }
}
