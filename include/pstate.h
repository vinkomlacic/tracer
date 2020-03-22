#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pstate_t.h"

/**
 * Creates new pstate_t struct and initializes its values.
 * @return
 */
extern pstate_t create_pstate(void);


/**
 * Saves the process state (values of all registers) into the pstate_t struct.
 */
extern void save_process_regs(pstate_t *pstate);


extern void save_process_code(pstate_t *pstate, intptr_t start_address, size_t code_size);


/**
 * Restore the process to the state specified in the argument.
 * This changes process registers and restores possible code changes (e.g. setting breakpoints or
 * other code injections).
 */
extern void revert_to(pstate_t const *pstate);


/**
 * Returns the address after the changes.
 * If there were no changes to the process, 0 is returned.
 * If the pstate is null t_errno is set and 0 is returned.
 *
 * Precondition: pstate must not be null.
 */
extern intptr_t get_address_after_changes(pstate_t const * pstate);


/**
 * Returns true if the pstate struct has stored any changes so far.
 *
 * Precondition: pstate must not be null.
 */
extern bool has_changes(pstate_t const * pstate);
