#pragma once

#include <stdint.h>

#define MAX_CODE_LENGTH 1024
#define LIBC_NAME "libc"

/**
 * Returns the address of a symbol in a currently running program.
 * 
 * If the process is not running or a symbol could not be found in the object file
 * 0UL is returned and the corresponding t_errno error code is set.
 * 
 * Also, if any other error occurs the same value is return and an error code is
 * set.
 */
extern intptr_t get_symbol_address_in_target(pid_t pid, char const symbol[]);


/**
 * Returns the address of a symbol in a library in a currently running program.
 *
 * If the process is not running or a symbol could not be found in the object file
 * 0UL is returned and t_errno code is set.
 */
extern intptr_t get_symbol_address_in_libc(pid_t pid, const char *symbol);


extern intptr_t get_mprotect_address(pid_t pid);


extern size_t get_function_code(pid_t pid, intptr_t start_address, uint8_t code_output[]);


/**
 * Returns the PID of a running process.
 *
 * If the process PID could not be retrieved for some reason t_errno is set
 * and -1 is returned by the function.
 */
extern pid_t get_pid(char const process_name[]);