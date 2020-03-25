/**
 * Contains functions for parsing options from the command line
 */

#pragma once

#include <stdbool.h>

#include "options_t.h"


/**
 * TODO: correct docs
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
extern void parse_options(int argc, char const * const argv[], options_t * options);
