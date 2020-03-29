#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "t_errno_t.h"
#include "t_error_t.h"


#define raise(ERROR_CODE, ...) t_error.code = ERROR_CODE; \
    t_error.line = __LINE__; t_error.filename = __BASE_FILE__; \
    snprintf(t_error.message, MAX_MESSAGE_SIZE, __VA_ARGS__)


/**
 * Prints the error message of currently set t_errno variable.
 * Error is outputted to stderr stream.
 */
extern void t_perror(void);


/**
 * Stores the string equivalent of the error code to the output string.
 */
extern void t_strerror(t_errno_t error_code, size_t string_length, char output[static MAX_MESSAGE_SIZE]);

/**
 * Checks the t_errno value and exits the process in case the value is anything other than success.
 * Also, error message detailing the error is printed to stderr.
 * In case there is no error, nothing happens.
 */
extern void check_for_error(void);


/**
 * Checks if an error occurred.
 * If error occurred returns true, otherwise false.
 */
extern bool error_occurred(void);