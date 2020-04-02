#pragma once

#include <unistd.h>
#include <sys/user.h>


/**
 * Attaches to the target, sends a SIGSTOP and waits until the target's state hasn't changed.
 *
 * If ptrace request fails, T_EPTRACE is raised and function returns.
 * If wait fails, T_EWAIT is raised and function returns.
 */
extern void pattach(pid_t pid);


/**
 * Continues the normal execution of the attached process (sends SIGCONT).
 * If ptrace request fails, T_EPTRACE is raised and function returns.
 */
extern void pcontinue(pid_t pid);


/**
 * Returns the registers from the currently running process.
 * If ptrace request fails, T_EPTRACE is raised and function returns.
 */
extern struct user_regs_struct get_regs(pid_t pid);


/**
 * Sets the registers on the currently running process.
 * If ptrace request fails, T_EPTRACE is raised and function returns.
 */
extern void set_regs(pid_t pid, struct user_regs_struct const * regs);


/**
 * Detaches from the target process.
 * If ptrace request fails, T_EPTRACE is raised and function returns.
 */
extern void pdetach(pid_t pid);
