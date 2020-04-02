#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pstate_t.h"


/**
 * Saves the process state (values of all registers) into the pstate_t struct.
 *
 * If pstate is NULL, T_EPSTATE_INVALID is raised.
 * If any other error occurs, t_error is set accordingly.
 */
extern void save_process_regs(pstate_t *pstate);


/**
 * Saves the code_size bytes of code currently in the process starting from the start_address.
 *
 * If pstate is NULL, T_EPSTATE_INVALID is raised.
 * If start_address is 0, T_EADDRESS is raised.
 * If any other error occurs, t_error is set accordingly.
 */
extern void save_process_code(pstate_t *pstate, intptr_t start_address, size_t code_size);


/**
 * Restore the process to the state specified in the argument.
 * This changes process registers and restores possible code changes (e.g. setting breakpoints or
 * other code injections).
 *
 * If pstate is NULL, T_EPSTATE_INVALID is raised.
 * If any other error occurs, t_error is set accordingly.
 */
extern void revert_to(pstate_t const * pstate);


/**
 * Returns the address after the changed code.
 *
 * If pstate is NULL, T_EPSTATE_INVALID is raised.
 */
extern intptr_t get_address_after_changes(pstate_t const * pstate);
