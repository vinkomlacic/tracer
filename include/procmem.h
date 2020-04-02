/**
 * Contains utility functions for reading and writing into process memory.
 */
#pragma once

#include <stdint.h>
#include <sys/types.h>

#define MEMORY_PATH_FORMAT "/proc/%d/mem"
#define MEMORY_PATH_MAX (22)

/**
 * Returns one byte from the specified address in the process. Reads /proc/${pid}/mem.
 *
 * In case error occurs, sets t_error code and returns 0 immediately.
 */
extern uint8_t proc_read_byte(pid_t pid, intptr_t address);


/**
 * Returns one word from the specified address in the process. Reads /proc/${pid}/mem.
 *
 * If reading from the file fails, raises T_EREAD and returns 0.
 * In case any other error occurs, t_error code is set and function returns 0.
 */
extern intptr_t proc_read_word(pid_t pid, intptr_t address);


/**
 * Writes a byte-sized value at the specified address in the process.
 * Writes to /proc/${pid}/mem file.
 *
 * In case an error occurs t_error code value is set and function returns.
 */
extern void proc_write_byte(pid_t pid, intptr_t address, uint8_t value);


/**
 * Writes a word-sized value at the specified address in the process.
 * Writes to /proc/${pid}/mem file.
 *
 * If there was an error writing to the address, T_EWRITE is raised and function returns.
 * In case of any other error, t_error code is set and function returns immediately.
 */
extern void proc_write_word(pid_t pid, intptr_t address, intptr_t word);


/**
 * Reads a memory block of arbitrary size from the process at the specified address.
 * Reads /proc/${pid}/mem file.
 *
 * If error occurs, t_error code is set and function returns immediately.
 *
 * Possible error codes:
 * T_EADDRESS - if address is 0
 * T_ENULL_ARG - if output is NULL
 * T_EREAD - if there was an error reading from the file
 *
 * In case of any other errors, t_error is set accordingly.
 */
extern void proc_read_block(pid_t pid, intptr_t start_address, size_t block_size, uint8_t output[static 1]);


/**
 * Writes a block of memory of block_size size to the process at specified address.
 * Writes to /proc/${pid}/mem.
 *
 * If error occurs, t_error code is set and function returns immediately.
 *
 * Possible error codes:
 * T_EADDRESS - if address is 0
 * T_ENULL_ARG - if code is NULL
 * T_EWRITE - if there was an error writing to the file
 *
 * In case of any other errors, t_error code is set accordingly.
 */
extern void proc_write_block(pid_t pid, intptr_t start_address, size_t block_size, uint8_t const code[static 1]);
