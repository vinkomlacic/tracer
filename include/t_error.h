#pragma once

/**
 * Error definitions and handy functions for the Tracer program.
 * Essentially a wrapper for errno.h which extends its functionality to
 * conform to custom needs and error messages in the program.
 * 
 * Use this in the Tracer program instead of directly reading errno or calling perror().
 * TODO: add support for providing contextual error messages
 */


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
  T_ECLI_EMPTY = -11
} t_errno_t;


/**
 * Global variable used to check if an error has been thrown.
 */
extern t_errno_t t_errno;


/**
 * Prints the error message of currently set t_errno variable.
 * Message argument is printed before the error message followed by a colon.
 */
extern void t_perror(char const * const message);
