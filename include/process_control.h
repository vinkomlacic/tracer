/**
 * Wrapper for ptrace function. Also contains other process control related
 * utility functions.
 * 
 * All functions are synchrounous, which means that after sending a signal they don't
 * return until after the target received it.
 */
#pragma once

#define MEMORY_PATH_MAX 16

/**
 * Attaches to the target, sends a SIGSTOP and waits until the target's state hasn't changed.
 */
extern void pattach(int const pid);

/**
 * Injects interrupt in a running process' code.
 *
 * Precondition: process must be attached.
 */
extern void inject_interrupt_at(int const pid, unsigned long const address);

/**
 * Continues the normal execution of the attached process until the next breakpoint
 * or until the program exits normally.
 *
 * Precondition: process must be attached.
 * TODO: consider returning the value of the stop signal
 */
extern void pcontinue(int const pid);


/**
 * Detaches from the target process.
 *
 * Precondition: process must be attached.
 */
extern void pdetach(int const pid);
