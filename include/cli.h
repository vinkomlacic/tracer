#pragma once

#define TRUE 1
#define FALSE 0

// Includes program name
#define REQUIRED_ARGUMENT_COUNT 5 

#define TARGET_NAME_OPTION "-t"
#define FUNCTION_NAME_OPTION "-f"

typedef enum option_t {TARGET_NAME, FUNCTION_NAME} option_t;

/**
 * Validates the command line arguments.
 * If arguments are valid returns 0, otherwise 1 is returned.
 */
extern int arguments_are_invalid(const int argc, char *argv[]);

/**
 * Prints the usage of the program.
 */
extern void print_usage(void);

/**
 * Parses argument list and returns the value of the value of the specified option.
 *
 * If there was a problem parsing argument list t_errno is set to T_EVALUE_NOT_FOUND.
 * If an unknown option was provided t_errno is set to T_ECLI_ARGV.
 *
 * On success returns the value of the command line option.
 * Otherwise, returns NULL and sets the t_errno global variable.
 */
extern char * parse_arguments(option_t option, const int argc, char *argv[]);