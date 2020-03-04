/**
 * Wrapper for ptrace function. Also contains other process control related
 * utility functions.
 * 
 * All functions are synchronous, which means that after sending a signal they don't
 * return until after the target received it.
 */
#pragma once

#include <stdint.h>


/**
 * Attaches to the target, sends a SIGSTOP and waits until the target's state hasn't changed.
 */
extern void pattach(pid_t pid);


/**
 * Injects interrupt in a running process' code.
 *
 * Precondition: process must be attached.
 * Returns the value of the current byte in the place where the interrupt
 * will be.
 * In case an error occurs, the t_errno value will be set and 0 will be returned.
 */
extern uint8_t inject_interrupt_at(pid_t pid, intptr_t address);


/**
 * Continues the normal execution of the attached process (sends SIGCONT)
 *
 * Precondition: process must be attached.
 */
extern void pcontinue(pid_t pid);


/**
 * Waits for SIGTRAP. When the SIGTRAP occurs replaces the interrupt signal previously
 * placed at the address with the instruction that was originally at that memory location
 * and reverts the instruction pointer by one instruction.
 */
extern void breakpoint_handler(pid_t pid, intptr_t address, uint8_t old_instruction);


/**
 * Detaches from the target process.
 *
 * Precondition: process must be attached.
 */
extern void pdetach(pid_t pid);
