#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pstate_t.h"


/**
 * Saves the process state (values of all registers) into the pstate_t struct.
 *
 * Precondition: pstate must not be null.
 *
 * If pstate is null, T_EPSTATE_INVALID is raised.
 */
extern void save_process_regs(pstate_t *pstate);


/**
 * Saves the code_size bytes of code currently in the process starting from the start_address.
 *
 * Precondition: pstate must not be null, start_address must not be zero
 *
 * If pstate is null, T_EPSTATE_INVALID is raised.
 * If start_address is 0,
 */
extern void save_process_code(pstate_t *pstate, intptr_t start_address, size_t code_size);


/**
 * Restore the process to the state specified in the argument.
 * This changes process registers and restores possible code changes (e.g. setting breakpoints or
 * other code injections).
 *
 * Precondition: pstate must not be null
 *
 * If pstate is null, T_EPSTATE_INVALID is raised.
 */
extern void revert_to(pstate_t const * pstate);


/**
 * Returns the address after the changes.
 * If there were no changes to the process, 0 is returned.
 *
 * Precondition: pstate must not be null.
 *
 * If pstate is null, T_EPSTATE_INVALID is raised.
 */
extern intptr_t get_address_after_changes(pstate_t const * pstate);
