/*
 * After the process has been attached, these functions can be used to control
 * how the attached process (i.e. tracee) will be executed.
 */
#pragma once

#include <stdint.h>
#include <sys/user.h>

#include "pstate_t.h"


/**
 * Blocks the calling thread until the breakpoint is hit or the target process dies.
 *
 * If wait-family function used fails, T_EWAIT is raised.
 * If process is stopped not because of the trap but because of something else, T_UNEXPECTED_STOP
 * raised.
 */
extern void wait_for_breakpoint(pid_t pid);


/**
 * Calls virus function that has been injected somewhere in the attached process.
 * The function is of prototype int virus(int).
 *
 * If pstate is NULL, T_ENULL_ARG is raised and function returns -1.
 * In case of any other error, t_error code is set accordingly and function immediately returns -1.
 */
extern int call_virus(pstate_t *pstate, intptr_t virus_address, int arg);


/**
 * Calls posix_memalign function from libc. Passes alignment and size arguments to the function.
 * The function returns address of the allocated block.
 * During the function call, a variable is allocated, used and deallocated in the tracee's stack.
 *
 * If pstate is NULL, T_ENULL_ARG is raised and function returns -1.
 * In case of any other error, t_error code is set accordingly and function immediately returns -1.
 */
extern intptr_t call_posix_memalign(pstate_t *pstate, intptr_t posix_memalign_address, int alignment, size_t size);


/**
 * Calls mprotect with start_address, length and prot arguments.
 *
 * If pstate is NULL, T_ENULL_ARG is raised and function returns -1.
 * If function is successfully executed in the tracee but for some reason fails, T_ECALL_FAIL is raised.
 * In case of any other error, t_error code is set accordingly and function immediately returns.
 */
extern void call_mprotect(pstate_t *pstate, intptr_t mprotect_address, intptr_t start_address, size_t length, int prot);


/**
 * Calls free with the pointer to deallocate.
 *
 * If pstate is NULL, T_ENULL_ARG is raised and function returns;
 * In case of any other error, t_error code is set accordingly and function immediately returns.
 */
extern void call_free(pstate_t * pstate, intptr_t free_address, intptr_t address);
