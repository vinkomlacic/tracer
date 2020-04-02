#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "t_error.h"
#include "log.h"
#include "options_t.h"
#include "con_color.h"

#include "options.h"


static void initialize_default_options(options_t * options);
static void display_help(void);
static void validate_options(options_t const * options);

// TODO: consider replacing getopt with argparse
extern void parse_options(int const argc, char * const argv[const], options_t * const options) {
    if (options == NULL) {
        RAISE(T_ENULL_ARG, "options");
        return;
    }
    if (argv == NULL) {
        RAISE(T_ENULL_ARG, "argv");
        return;
    }
    initialize_default_options(options);

    int c = 0;

    while ((c = getopt(argc, argv, "cb:p:e:h")) != -1) {
        DEBUG("-%c option encountered with value %s", c, optarg == NULL ? "" : optarg);
        switch (c) {
            case 'c':
                options->clean = true;
                break;

            case 'b':
                strncpy(options->binary_path, optarg, PATH_MAX - 1);
                if (strlen(optarg) > PATH_MAX) {
                    WARN("Option -b value truncated (length > %d). This could produce unexpected results.", PATH_MAX);
                }
                break;

            case 'p':
                strncpy(options->process_name, optarg, PATH_MAX - 1);
                if (strlen(optarg) > PATH_MAX) {
                    WARN("Option -p value truncated (length > %d). This could produce unexpected results.", PATH_MAX);
                }
                break;

            case 'e':
                strncpy(options->entry_function, optarg, FUNCTION_NAME_MAX - 1);
                if (strlen(optarg) > FUNCTION_NAME_MAX) {
                    WARN("Option -e value truncated (length > %d). This could produce unexpected results.", FUNCTION_NAME_MAX);
                }
                break;

            case 'h':
                display_help();
                exit(EXIT_SUCCESS);

            case '?':
                if (optopt == 'c' || optopt == 'b' || optopt == 'p' || optopt == 'e' || optopt == 'h') {
                    RAISE(T_ECLI_EMPTY, "%c", optopt);
                    return;
                }
                WARN("Unknown option '-%c'", optopt);
                break;

            default:
                ERROR("Unexpected exception while parsing command line. Aborting.");
                exit(EXIT_FAILURE);
        }

        if (error_occurred()) return;
    }

    validate_options(options);
}


static void initialize_default_options(options_t * const options) {
    options->clean = false;
    memset(options->binary_path, 0, PATH_MAX);
    memset(options->process_name, 0, PATH_MAX);
    memset(options->entry_function, 0, FUNCTION_NAME_MAX);
}


static void display_help(void) {
    set_console_color(WHITE, true);
    printf("\nUsage: tracer -p <process_name> -b <path_to_binary> -e <entry_point> -c <cleanup>\n");
    reset_console_color();

    printf("\nProgram takes control of the specified process at the specified entry point and\n");
    printf("creates a virus which is then injected in the tracee process.\n");

    printf("\nOptions:\n\n");
    printf("\t-p\tName of the process\n");
    printf("\t-b\tPath to the running process binary\n");
    printf("\t-e\tEntry point in the process where the tracer will hook up (e.g. function f1)\n");
    printf("\t-c\tCleanup option (optional) - inject and execute the virus and leave no trace\n");
    printf("\t  \tIf you don't specify cleanup option program will replace entry function with\n");
    printf("\t  \tthe virus and detach which means every time program executes the entry function\n");
    printf("\t  \tthe virus will be executed.\n");
}


static void validate_options(options_t const * const options) {
    if (strlen(options->binary_path) <= 0) {
        RAISE(T_ECLI_REQ, "-b");
    } else if (strlen(options->process_name) <= 0) {
        RAISE(T_ECLI_REQ, "-p");
    } else if (strlen(options->entry_function) <= 0) {
        RAISE(T_ECLI_REQ, "-e");
    }
}
