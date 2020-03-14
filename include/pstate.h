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


/**
 * Restore the process to the state specified in the argument.
 * This changes process registers and restores possible code changes (e.g. setting breakpoints or
 * other code injections).
 */
extern void revert_to(pstate_t const *pstate);


/**
 * Adds code to changed code array in pstate_t struct.
 * Accordingly updates changed code length in the struct.
 *
 * This is the preferred way of updating code in the struct.
 */
extern void append_code_to_pstate(pstate_t *pstate, uint8_t code);


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


/**
 * Returns the registers from the currently running process.
 */
extern struct user_regs_struct get_regs(pid_t pid);