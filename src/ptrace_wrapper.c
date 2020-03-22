#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "t_error.h"
#include "log.h"

#include "ptrace_wrapper.h"


extern void pattach(pid_t const pid) {
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
        raise(T_EPTRACE, "attaching %d", pid);
        return;
    }


    DEBUG("PTRACE_ATTACH executed. Waiting for stop signal.");
    if (waitpid(pid, NULL, WSTOPPED) == -1) {
        raise(T_EWAIT, "%d", pid);
    }
    DEBUG("Tracee process stopped. Tracee is attached.");
}


extern void pcontinue(pid_t const pid) {
    DEBUG("rip: %#llx", get_regs(pid).rip);
    if (ptrace(PTRACE_CONT, pid, 0, SIGCONT) == -1) {
        raise(T_EPTRACE, "sending SIGCONT to %d", pid);
    }
    DEBUG("SIGCONT sent");
}


extern struct user_regs_struct get_regs(pid_t const pid) {
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        raise(T_EPTRACE, "getting regs failed (pid: %d)", pid);
    }

    return regs;
}


extern void set_regs(pid_t const pid, struct user_regs_struct * const regs) {
    if (ptrace(PTRACE_SETREGS, pid, NULL, regs) == -1) {
        raise(T_EPTRACE, "setting regs failed (pid: %d)", pid);
    }
}


extern void pdetach(pid_t const pid) {
    DEBUG("rip: %#llx", get_regs(pid).rip);
    if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) {
        raise(T_EPTRACE, "detaching %d", pid);
    }
    DEBUG("Process %d detached", pid);
}