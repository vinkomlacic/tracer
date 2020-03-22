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
 * Sets breakpoint in the code.
 * pstate_t structure is modified to reflect the changes in the code so they could be restored later.
 *
 * Precondition: pstate mustn't be null
 */
extern void inject_breakpoint(pstate_t* pstate, intptr_t address);


extern void inject_code_to_process(pid_t pid, intptr_t address, size_t code_size, uint8_t const code[]);


extern void inject_trampoline(pid_t pid, intptr_t function_address, intptr_t address);


/**
 * Blocks the calling thread until the breakpoint is hit or the target process dies.
 */
extern void wait_for_breakpoint(pid_t pid);


extern int call_virus(pstate_t *pstate, intptr_t virus_address, int arg);


extern intptr_t call_posix_memalign(pstate_t *pstate, size_t alignment, size_t size);


extern void call_mprotect(pstate_t *pstate, intptr_t start_address, size_t length, int prot);


extern void call_free(pstate_t * pstate, intptr_t address);


extern void scrub_virus(pid_t pid, intptr_t start_address, size_t size);
