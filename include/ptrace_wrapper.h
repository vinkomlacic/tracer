#pragma once

#include <unistd.h>
#include <sys/user.h>

/**
 * Attaches to the target, sends a SIGSTOP and waits until the target's state hasn't changed.
 */
extern void pattach(pid_t pid);


/**
 * Continues the normal execution of the attached process (sends SIGCONT).
 *
 * Precondition: process must be attached.
 */
extern void pcontinue(pid_t pid);


/**
 * Returns the registers from the currently running process.
 */
extern struct user_regs_struct get_regs(pid_t pid);


/**
 * Sets the registers on the currently running process.
 */
extern void set_regs(pid_t pid, struct user_regs_struct *regs);


/**
 * Detaches from the target process.
 *
 * Precondition: process must be attached.
 */
extern void pdetach(pid_t pid);