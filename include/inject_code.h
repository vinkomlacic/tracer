#pragma once

#include <unistd.h>
#include <stdint.h>

#include "pstate_t.h"

/**
 * Sets breakpoint in the code at the specified address in memory.
 * pstate_t structure is modified to reflect the changes in the code so they could be restored later.
 */
extern void inject_breakpoint(pstate_t *pstate, intptr_t address);


/**
 * Injects code_size bytes into a running process with specified pid at the specified address.
 *
 * Precondition: address must not be zero.
 *
 * If address is zero T_EADDRESS is raised.
 */
extern void inject_raw_code_to_process(pid_t pid, intptr_t address, size_t code_size, uint8_t const code[static 1]);


/**
 * Sets up an indirect call in a running process. Changes the code and sets up the rax register.
 *
 * Precondition: address and function_address must not be zero.
 *
 * If addresses are zero, T_EADDRESS is raised.
 */
extern void inject_indirect_call_at(pstate_t *pstate, intptr_t address, intptr_t function_address);


/**
 * Injects trampoline at address which calls a function at function_address.
 *
 * Precondition: address and function_address must not be zero.
 *
 * If addresses are zero, T_EADDRESS is raised.
 */
extern void inject_trampoline(pid_t pid, intptr_t address, intptr_t function_address);


/**
 * Sets size bytes in process memory to 0 at start_address.
 *
 * Precondition: start_address must not be zero.
 *
 * If start_address is zero, T_EADDRESS is raised.
 */
extern void scrub_memory(pid_t pid, intptr_t start_address, size_t size);
