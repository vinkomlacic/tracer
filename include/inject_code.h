#pragma once

#include <unistd.h>
#include <stdint.h>

#include "pstate_t.h"

/**
 * Sets breakpoint in the code at the specified address in memory.
 * pstate_t structure is modified to reflect the changes in the code so they could be restored later.
 *
 * If pstate is null, T_ENULL_ARG is raised.
 * In case of any other error caused by dependent functions, t_error is set
 * and the function immediately returns.
 */
extern void inject_breakpoint(pstate_t *pstate, intptr_t address);


/**
 * Sets up an indirect call in a running process. Changes the code and sets up the rax register.
 *
 * In case of any error caused by dependent functions, t_error is set
 * and the function immediately returns.
 */
extern void inject_indirect_call_at(pstate_t *pstate, intptr_t address, intptr_t function_address);


/**
 * Injects trampoline at address which calls a function at function_address.
 *
 * In case of any error caused by dependent functions, t_error is set
 * and the function immediately returns.
 */
extern void inject_trampoline(pid_t pid, intptr_t address, intptr_t function_address);


/**
 * Sets size bytes in process memory to 0 at start_address.
 *
 * In case of any error caused by dependent functions, t_error is set
 * and the function immediately returns.
 */
extern void scrub_memory(pid_t pid, intptr_t start_address, size_t size);
