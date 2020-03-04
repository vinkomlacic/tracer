/**
 * Contains functions for parsing options from the command line
 */

#pragma once

#include <stdbool.h>

#include "options_t.h"


typedef enum {TARGET, SYMBOL} option_t;


/**
 * Parses the argv array and returns a struct containing all values provided.
 * The function heavily relies on the getopt GNU library.
 *
 * Note: Non option arguments are ignored.
 *
 * If an error occurs, t_errno is set and the function will return the options_t struct which
 * will contain parsed options until the moment error occurred.
 *
 * Possible t_errno values:
 *    T_ECLI_EMPTY  - option is missing an argument
 *    T_ECLI_ARGV   - unrecognized option is provided
 *    T_ECLI_REQ    - required option not found
 *
 * On success, populated options_t struct is returned.
 */
extern options_t parse_options(int argc, char * const argv[]);


/**
 * Checks if the option is present in the options_t struct.
 *
 * If an unknown option passed to this function means it has not been addressed
 * by the function ergo, implementation fault.
 * The user provides an enum type which means an unexpected value is impossible
 * to pass. Nevertheless, in this case t_errno is set to T_ECLI_ARGV and false is
 * returned.
 */
extern bool option_is_present(options_t const * options, option_t option);
