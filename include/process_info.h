#pragma once

#include <stdint.h>

#define MAX_CODE_LENGTH 1024
#define LIBC_NAME "libc"


extern intptr_t get_process_base_address(pid_t pid);


extern intptr_t get_symbol_offset_in_binary(char const binary_path[], char const symbol[], bool shared_object);


extern intptr_t locate_libc_in(pid_t pid);


extern void get_libc_path(char const binary_path[], char path[]);


extern size_t get_function_code(pid_t pid, intptr_t start_address, uint8_t code_output[]);


/**
 * Returns the PID of a running process.
 *
 * If the process PID could not be retrieved for some reason t_errno is set
 * and -1 is returned by the function.
 */
extern pid_t get_pid(char const process_name[]);
