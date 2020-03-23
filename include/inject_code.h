#pragma once

#include <unistd.h>
#include <stdint.h>

#include "pstate_t.h"

/**
 * Sets breakpoint in the code.
 * pstate_t structure is modified to reflect the changes in the code so they could be restored later.
 *
 * Precondition: pstate mustn't be null
 */
extern void inject_breakpoint(pstate_t* pstate, intptr_t address);


extern void inject_raw_code_to_process(pid_t pid, intptr_t address, size_t code_size, const uint8_t *code);


extern void inject_indirect_call_at(pstate_t *pstate, intptr_t address, intptr_t function_address);


extern void inject_trampoline(pid_t pid, intptr_t function_address, intptr_t address);


extern void scrub_virus(pid_t pid, intptr_t start_address, size_t size);