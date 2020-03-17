#pragma once

/**
 * Error definitions and handy functions for the Tracer program.
 * Essentially a wrapper for errno.h which extends its functionality to
 * conform to custom needs and error messages in the program.
 * 
 * Use this in the Tracer program instead of directly reading errno or calling perror().
 */

#include <stdbool.h>


typedef enum {
  T_SUCCESS = 0,

  /**
  * Generic errors: consult errno to see detailed error code
  */
  T_ERROR = -1,
  T_EPOPEN = -2,                  // error opening pipe
  T_EPCLOSE = -3,                 // error closing pipe
  T_EPREAD = -4,                  // error reading from pipe

  /** 
  * One of the functions from the printf family of function encountered a problem.
  * It is not defined if functions from this family would set the errno variable
  * so it's treated specially in this program.
  */
  T_EPRINTF = -5,
  T_EFGETS = -6,
  T_ECLI_ARGV = -7,               // error reading command line arguments
  T_EVALUE_NOT_FOUND = -8,
  T_EPROC_NOT_RUNNING = -9,       // target process not running
  T_ESYMBOL_NOT_FOUND = -10,      // symbol not found
  T_ECLI_EMPTY = -11,             // option is missing an argument
  T_ECLI_REQ = -12,               // required options is missing
  T_ENULL_ARG = -13,              // null argument passed to function
  T_EENCODING = -14,              // encoding error
  T_ESTR_TRUNC = -15,             // string truncated
  T_EOPEN = -16,                  // error opening file
  T_ECLOSE = -17,                 // error closing file
  T_EREAD = -18,                  // generic read error
  T_EWRITE = -19,                 // generic write error
  T_EPTRACE = -20,                // ptrace error
  T_EWAIT = -21,                  // wait / waitpid / waitid error
  T_FUNC_TOO_BIG = -22,           // function code is too big
} t_errno_t;


/**
 * Global variable used to check if an error has been thrown.
 */
extern t_errno_t t_errno;


/**
 * Prints the error message of currently set t_errno variable.
 * Error is outputted to stderr stream.
 */
extern void t_perror(char const message[]);

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