#include <sys/ptrace.h>
#include <string.h>

#include "log.h"
#include "t_error.h"
#include "procmem.h"
#include "process_info.h"
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


extern void revert_to(pstate_t const * const pstate) {
    if (pstate == NULL) {
        t_errno = T_ENULL_ARG;
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
        t_errno = T_EPTRACE;
        return;
    }

    DEBUG("%s process' state restored", pstate->name);
}


extern void append_code_to_pstate(pstate_t * const pstate, uint8_t const code) {
    if (pstate != NULL) {
        pstate->changed_code[pstate->changed_code_len] = code;
        pstate->changed_code_len++;
    }
}


extern intptr_t get_address_after_changes(pstate_t const * const pstate) {
    if (pstate == NULL) {
        t_errno = T_ENULL_ARG;
        return 0;
    }

    return pstate->change_address + pstate->changed_code_len;
}


extern bool has_changes(pstate_t const * const pstate) {
    if (pstate == NULL) {
        t_errno = T_ENULL_ARG;
        return true;
    }

    return ((pstate->changed_code_len) == 0) ? false : true;
}


extern struct user_regs_struct get_regs(pid_t const pid) {
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        t_errno = T_EPTRACE;
    }

    return regs;
}
