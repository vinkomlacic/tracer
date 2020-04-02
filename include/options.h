/**
 * Contains functions for parsing options from the command line
 */

#pragma once

#include <stdbool.h>

#include "options_t.h"


/**
 * Parses the argv array and returns a struct containing all values provided.
 * Note: Unknown options are ignored and a warning is issued, however this will probably result in
 * an error further down the road.
 *
 * If an error occurs, t_error is set and the function will return the options_t struct which
 * will contain default values.
 *
 * Possible t_error codes:
 * T_ENULL_ARG - options or argv is null
 * T_ECLI_EMPTY - option is missing an argument
 * T_ECLI_REQ - required option is missing
 *
 * On success, populated options_t struct is returned.
 */
extern void parse_options(int argc, char * const argv[], options_t * options);
