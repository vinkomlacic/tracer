#include <stdio.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <signal.h>

#include "t_error.h"
#include "procmem.h"
#include "process_control.h"


extern void pattach(pid_t const pid) {
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
        t_errno = T_EPTRACE;
        return;
    }

    if (waitpid(pid, NULL, WSTOPPED) == -1) {
        t_errno = T_EWAIT;
    }
}


extern uint8_t inject_interrupt_at(pid_t const pid, intptr_t const address) {
    uint8_t const old_instruction = proc_read_byte(pid, address);
    if (old_instruction == 0 && error_occurred()) {
        return 0;
    }

    uint8_t const interrupt = 0xCC;
    proc_write_byte(pid, address, interrupt);
    if (error_occurred()) {
        return 0;
    }

    return old_instruction;
}


extern void pcontinue(pid_t const pid) {
    if (ptrace(PTRACE_CONT, pid, 0, SIGCONT) == -1) {
        t_errno = T_EPTRACE;
    }
}


extern void breakpoint_handler(pid_t const pid, intptr_t const address, uint8_t const old_instruction) {
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
        t_errno = T_EWAIT;
    }

    if (WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGTRAP) {
        proc_write_byte(pid, address, old_instruction);

        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, &regs, NULL);
        printf("%%rip: %llx\n", regs.rip);
    }
}


extern void pdetach(pid_t const pid) {
    if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) {
        t_errno = T_EPTRACE;
    }
}

