/**
 * Utility functions used for reading from a pipe.
 */
#pragma once

#define BUFFER_LENGTH 1024


/**
 * Reads an unsigned long integer from a pipe specified by a command.
 * 
 * If there was a problem opening, closing or reading from a pipe 0UL is returned
 * and a corresponding t_errno value is set.
 */
extern unsigned long pread_unsigned_long(const char *command);


/**
 * Reads an integer from a pipe specified by a command.
 * 
 * If there was a problem opening, closing or reading from a pipe -1 is returned
 * and a corresponding t_errno value is set.
 */
extern int pread_int(const char *command);