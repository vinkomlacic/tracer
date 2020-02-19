/**
 * TODO: use a third-party lib instead of this one
 */

#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "option_t.h"

/*
 * It is required to define the following globals outside of 
 * this module otherwise the code will fail on linking.
 */
extern option_t g_required_options[];
extern size_t g_required_options_len;
extern option_t g_optional_options[];
extern size_t g_optional_options_len;

/**
 * Validates the command line arguments.
 * Validation checks for presence of required arguments as for the correctness
 * of optional arguments.
 *
 * If arguments are invalid returns true, otherwise false is returned.
 */
extern bool arguments_are_invalid(int const argc, char const * const argv[]);

/**
 * Prints the usage of the program.
 */
extern void print_usage(void);

/**
 * Parses argument list and returns the index of the option value in the argv.
 *
 *
 * If an unknown option was provided t_errno is set to T_ECLI_ARGV.
 *
 * If option is not present and the option is required, t_errno is set to
 * T_ECLI_ARGV and -1 is returned.
 *
 * If option is not present and it is optional -1 is returned and t_errno is not set.
 *
 * If option is present but it contains no value, t_errno is set to T_ECLI_EMPTY
 * and -1 is returned.
 *
 * On success returns the value of the command line option.
 */
extern int get_option_value(option_t const option, int const argc, char const * const argv[]);
