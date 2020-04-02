#pragma once

#include <stdint.h>

#define MAX_CODE_LENGTH (1024) /* Maximum supported length of code in a function */
#define LIBC_NAME "libc"


/**
 * Returns process base address. Reads /proc/${pid}/maps.
 *
 * If preparing the command string fails, T_EPRINTF is raised and function returns 0.
 * If any other error is encountered, t_error code is set accordingly and 0 is returned.
 */
extern intptr_t get_process_base_address(pid_t pid);


/**
 * Returns the symbol offset in binary. If binary is a shared object (e.g. shared library) you need
 * to set shared_object to true.
 *
 * If binary_path or symbol is NULL, T_ENULL_ARG is raised and function returns 0.
 * If preparing the command string fails, T_EPRINTF is raised and function returns 0.
 * If symbol could not be found for some reason, T_ESYMBOL_NOT_FOUND is raised and function returns 0.
 */
extern intptr_t get_symbol_offset_in_binary(char const binary_path[], char const symbol[], bool shared_object);


/**
 * Returns the base address of runtime libc in process specified by pid.
 *
 * If preparing the command fails, T_EPRINTF is raised and function returns 0.
 * If any other error occurs, t_error is set accordingly and function return 0 immediately.
 */
extern intptr_t locate_libc_in(pid_t pid);


/**
 * Binary is inspected to see what exact libc is used and retrieve its path.
 *
 * If binary_path or path are null, T_ENULL_ARG is raised and function returns.
 * If preparing the command fails, T_EPRINTF is raised and function returns.
 * If any other error occurs, t_error is set accordingly and function return 0 immediately.
 */
extern void get_libc_path(char const binary_path[], char path[]);


/**
 * Running process with pid is inspected to get function code at the specified address.
 * The code is read until the return instruction or the function size reaches MAX_CODE_LENGTH.
 *
 * If error occurs, function sets t_error code and returns -1 immediately.
 *
 * Possible error codes:
 * T_ENULL_ARG - if code_output is NULL
 * T_EFUNC_TOO_BIG - if function byte size exceeds MAX_CODE_LENGTH
 *
 * If any other error occurs, t_error is set accordingly and function return 0 immediately.
 */
extern size_t get_function_code(pid_t pid, intptr_t start_address, uint8_t code_output[]);


/**
 * Returns the PID of a running process.
 *
 * If error occurs, function sets t_error code and returns -1 immediately.
 *
 * Possible error codes:
 * T_NULL_ARG - if process_name is NULL
 * T_EPRINTF - if preparing the command fails
 * T_EPROC_NOT_RUNNING - if process is not running
 */
extern pid_t get_pid(char const process_name[]);
