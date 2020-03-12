/**
 * Wrapper for ptrace function. Also contains other process control related
 * utility functions.
 * 
 * All functions are synchronous, which means that after sending a signal they don't
 * return until after the target received it.
 */
#pragma once

#include <stdint.h>
#include <sys/user.h>

#include "pstate_t.h"


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
 * Sets breakpoint in the code.
 * pstate_t structure is modified to reflect the changes in the code so they could be restored later.
 *
 * Precondition: pstate mustn't be null
 */
extern void set_breakpoint(intptr_t address, pstate_t *pstate);


/**
 * Blocks the calling thread until the breakpoint is hit or the target process dies.
 */
extern void wait_for_bp(pid_t pid);


/**
 * Calls a function in the process specified with the pid and returns the function return value.
 * Argument arg is passed to the function.
 *
 * Only functions corresponding to int (*) (int) signature are implemented so far.
 */
extern int call_function(pstate_t *pstate, char const function_to_call[], int arg);


extern int call_function_in_lib(pstate_t *pstate, char const function_to_call[], char const lib[], int arg);


extern intptr_t call_posix_memalign(pstate_t *pstate, size_t alignment, size_t size);


extern void call_mprotect(pstate_t *pstate, intptr_t start_address, size_t length, int prot);


extern size_t inject_virus(pid_t pid, intptr_t start_address, size_t size, uint8_t const code[]);


/**
 * Detaches from the target process.
 *
 * Precondition: process must be attached.
 */
extern void pdetach(pid_t pid);
