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

/* Function names in the libc library - convenient for efficient grepping */
#define POSIX_MEMALIGN_GLIBC_NAME "posix_memalign@@GLIBC"
#define MPROTECT_GLIBC_NAME "__mprotect@@GLIBC"
#define FREE_GLIBC_NAME "__libc_free@@GLIBC"


/**
 * Blocks the calling thread until the breakpoint is hit or the target process dies.
 */
extern void wait_for_breakpoint(pid_t pid);


extern int call_virus(pstate_t *pstate, intptr_t virus_address, int arg);


extern intptr_t call_posix_memalign(pstate_t *pstate, size_t alignment, size_t size);


extern void call_mprotect(pstate_t *pstate, intptr_t start_address, size_t length, int prot);


extern void call_free(pstate_t * pstate, intptr_t address);
