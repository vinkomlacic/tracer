/**
 * Utility functions used for reading from a pipe.
 */
#pragma once

#include <stdint.h>

#define BUFFER_LENGTH (1024)


/**
 * Reads a word from a pipe specified by a shell command.
 * In the command, word needs to be output in hex otherwise T_EPREAD is reported.
 *
 * In case an error occurs t_error is set and the function immediately return 0.
 *
 * Possible t_error codes are as follows.
 * T_ENULL_ARG - if command argument is null
 * T_EPOPEN - if there was an error opening the pipe
 * T_EFGETS - if there was an error reading from the pipe
 * T_EPREAD - if there was an error with the output format
 * T_EPCLOSE - if there was an error closing the pipe
 */
extern intptr_t pread_word(char const command[static 1]);


/**
 * Reads an int from a pipe specified by a shell command.
 *
 * In case an error occurs t_error is set and the function immediately return 0.
 *
 * Possible t_error codes are as follows.
 * T_ENULL_ARG - if command argument is null
 * T_EPOPEN - if there was an error opening the pipe
 * T_EFGETS - if there was an error reading from the pipe
 * T_EPREAD - if there was an error with the output format
 * T_EPCLOSE - if there was an error closing the pipe
 */
extern int pread_int(char const command[static 1]);


/**
 * Reads a line from a pipe specified by a shell command.
 * Newline is not included in the output.
 *
 * In case an error occurs t_error is set and the function immediately return 0.
 *
 * Possible t_error codes are as follows.
 * T_ENULL_ARG - if command argument is null
 * T_EPOPEN - if there was an error opening the pipe
 * T_EFGETS - if there was an error reading from the pipe
 * T_EPCLOSE - if there was an error closing the pipe
 */
extern void pread_raw_line(char const command[static 1], char output[static BUFFER_LENGTH]);
