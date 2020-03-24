#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "t_error.h"
#include "procmem.h"
#include "process_info.h"
#include "ptrace_wrapper.h"

#include "pstate.h"


extern void save_process_regs(pstate_t * const pstate) {
    if (pstate == NULL) {
        raise(T_EPSTATE_INVALID, "pstate is null");
        return;
    }

    struct user_regs_struct regs = get_regs(pstate->pid);
    if (error_occurred()) return;
    pstate->changed_regs = regs;
}


extern void save_process_code(pstate_t * const pstate, intptr_t const start_address, size_t const code_size) {
    if (pstate == NULL) {
        raise(T_EPSTATE_INVALID, "pstate is null");
        return;
    } else if (start_address == 0) {
        raise(T_EADDRESS, "%#lx", start_address);
        return;
    }

    if (pstate->changed_code_len == 0) pstate->change_address = start_address;

    DEBUG("Copying %lu bytes of process code from %#lx", code_size, start_address);
    for (size_t i = 0; i < code_size; i++) {
        pstate->changed_code[pstate->changed_code_len] = proc_read_byte(pstate->pid, start_address + i);
        pstate->changed_code_len++;
        if (error_occurred()) return;
    }
}


extern void revert_to(pstate_t const * const pstate) {
    if (pstate == NULL) {
        raise(T_EPSTATE_INVALID, "pstate is null");
        return;
    }

    DEBUG("Restoring %lu bytes in code at %#lx", pstate->changed_code_len, pstate->change_address);
    for (size_t i = 0; i < pstate->changed_code_len; i++) {
        proc_write_byte(pstate->pid, pstate->change_address + i, pstate->changed_code[i]);
        if (error_occurred()) return;
    }

    DEBUG("Restoring old registers");
    set_regs(pstate->pid, (struct user_regs_struct * const) &pstate->changed_regs);
    if (error_occurred()) return;

    DEBUG("%d process' state restored", pstate->pid);
}


extern intptr_t get_address_after_changes(pstate_t const * const pstate) {
    if (pstate == NULL) {
        raise(T_EPSTATE_INVALID, "pstate is null");
        return 0;
    }

    return pstate->change_address + pstate->changed_code_len;
}
