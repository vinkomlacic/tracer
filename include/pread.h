/**
 * Utility functions used for reading from a pipe.
 */
#pragma once

#include <stdint.h>

#define BUFFER_LENGTH (1024)


/**
 * Reads a word from a pipe specified by a command.
 * 
 * If there was a problem opening, closing or reading from a pipe 0UL is returned
 * and a corresponding t_errno value is set.
 */
extern intptr_t pread_word(char const command[]);


/**
 * Reads an integer from a pipe specified by a command.
 * 
 * If there was a problem opening, closing or reading from a pipe -1 is returned
 * and a corresponding t_errno value is set.
 */
extern int pread_int(const char command[]);


extern void pread_raw_line(const char command[], char output[static BUFFER_LENGTH]);
