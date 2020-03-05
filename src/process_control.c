#include <stdio.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <signal.h>

#include "t_error.h"
#include "procmem.h"
#include "log.h"
#include "process_control.h"


extern void pattach(pid_t const pid) {
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
        t_errno = T_EPTRACE;
        return;
    }


    DEBUG("PTRACE_ATTACH executed. Waiting for stop signal.");
    if (waitpid(pid, NULL, WSTOPPED) == -1) {
        t_errno = T_EWAIT;
    }
    DEBUG("Tracee process stopped. Tracee is attached.");
}


extern uint8_t inject_interrupt_at(pid_t const pid, intptr_t const address) {
    uint8_t const old_instruction = proc_read_byte(pid, address);
    if (old_instruction == 0 && error_occurred()) {
        return 0;
    }
    DEBUG("Old instruction: %#x", old_instruction);

    uint8_t const interrupt = 0xCC;
    proc_write_byte(pid, address, interrupt);
    if (error_occurred()) {
        return 0;
    }
    DEBUG("Wrote interrupt instruction at: %#lx", address);

    return old_instruction;
}


extern void pcontinue(pid_t const pid) {
    if (ptrace(PTRACE_CONT, pid, 0, SIGCONT) == -1) {
        t_errno = T_EPTRACE;
    }
    DEBUG("SIGCONT sent");
}


extern void breakpoint_handler(pid_t const pid, intptr_t const address, uint8_t const old_instruction) {
    int wstatus = 0;
    DEBUG("Waiting for interrupt in the target process");
    if (waitpid(pid, &wstatus, 0) == -1) {
        t_errno = T_EWAIT;
    }
    DEBUG("Target process interrupted");

    if (WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGTRAP) {
        DEBUG("Writing old instruction where the interrupt used to be");
        proc_write_byte(pid, address, old_instruction);


        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        DEBUG("Reverting %%rip to point to the address of the old instruction (%#x)", address);
        regs.rip = (unsigned long long int) address;
        ptrace(PTRACE_SETREGS, pid, NULL, &regs);
    }
}


extern void pdetach(pid_t const pid) {
    if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) {
        t_errno = T_EPTRACE;
    }
    DEBUG("Process %d detached", pid);
}

