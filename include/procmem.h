/**
 * Contains utility functions for reading and writing into process memory.
 */
#pragma once

#include <stdint.h>
#include <sys/types.h>

#define MEMORY_PATH_FORMAT "/proc/%d/mem"
#define MEMORY_PATH_MAX (22)

/**
 * Returns one byte from the specified address in the process.
 * In case an error occurs, always sets the value of t_errno and returns 0.
 */
extern uint8_t proc_read_byte(pid_t pid, intptr_t address);

/**
 * Writes a byte-sized value at the specified address in the process.
 * In case an error occurs t_errno value is set.
 */
extern void proc_write_byte(pid_t pid, intptr_t address, uint8_t value);