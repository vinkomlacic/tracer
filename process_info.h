#pragma once

/**
 * Returns the address of a symbol in a currently running program.
 * 
 * If the process is not running or a symbol could not be found in the object file
 * 0UL is returned and the corresponing t_errno error code is set.
 * 
 * Also, if any other error occurs the same value is return and an error code is
 * set.
 */
extern unsigned long get_symbol_address_in_target(const char *target, const char *symbol);