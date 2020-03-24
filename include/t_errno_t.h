#pragma once

/**
 * Error definitions and handy functions for the Tracer program.
 * Essentially a wrapper for errno.h which extends its functionality to
 * conform to custom needs and error messages in the program.
 * 
 * Use this in the Tracer program instead of directly reading errno or calling perror().
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
  T_EFUNC_TOO_BIG = -22,          // function code is too big
  T_EPSTATE_INVALID = -23,        // pstate struct is in illegal state
  T_EADDRESS = -24,               // address is invalid
  T_EUNKNOWN_COLOR = -25,         // unknown color
} t_errno_t;
