#include <sys/ptrace.h>
#include <string.h>

#include "log.h"
#include "t_error.h"
#include "procmem.h"
#include "process_info.h"
#include "ptrace_wrapper.h"

#include "pstate.h"


extern pstate_t create_pstate(void) {
    pstate_t pstate = {
            .pid = 0,
            .name = "",
            .change_address = 0,
            .changed_code_len = 0,
            .changed_code = {0}
    };

    return pstate;
}


extern void save_process_regs(pstate_t *pstate) {
    if (pstate->pid == 0) {
        WARN("pstate pid uninitialized. Returning from save_process_regs");
        return;
    }

    struct user_regs_struct regs = get_regs(pstate->pid);
    if (error_occurred()) return;
    pstate->changed_regs = regs;
}


extern void save_process_code(pstate_t * const pstate, intptr_t const start_address, size_t const code_size) {
    if (pstate == NULL) {
        raise(T_ENULL_ARG, "pstate is null");
        return;
    }

    if (pstate->changed_code_len == 0) pstate->change_address = start_address;

    DEBUG("Saving process code");
    for (size_t i = 0; i < code_size; i++) {
        pstate->changed_code[pstate->changed_code_len] = proc_read_byte(pstate->pid, start_address + i);
        pstate->changed_code_len++;
        if (error_occurred()) return;
    }
}


extern void revert_to(pstate_t const * const pstate) {
    if (pstate == NULL) {
        raise(T_ENULL_ARG, "pstate arg is null");
        return;
    }

    DEBUG("Restoring instructions in code");
    for (size_t i = 0; i < pstate->changed_code_len; i++) {
        proc_write_byte(pstate->pid, pstate->change_address + i, pstate->changed_code[i]);
        if (error_occurred()) {
            return;
        }
    }

    DEBUG("Restoring old registers");
    if (ptrace(PTRACE_SETREGS, pstate->pid, NULL, &(pstate->changed_regs)) == -1) {
        raise(T_EPTRACE, "setting regs");
        return;
    }

    DEBUG("%s process' state restored", pstate->name);
}


extern intptr_t get_address_after_changes(pstate_t const * const pstate) {
    if (pstate == NULL) {
        raise(T_ENULL_ARG, "pstate arg is null");
        return 0;
    }

    return pstate->change_address + pstate->changed_code_len;
}
