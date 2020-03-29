#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "t_error.h"
#include "log.h"
#include "options_t.h"

#include "options.h"


static void initialize_default_options(options_t * options);
static void display_help(void);
static bool validate_options(options_t const * options);

/**
 * TODO: consider replacing getopt with argparse (getopt seems too simple)
 */
extern void parse_options(int const argc, char const * const argv[const], options_t * options) {
    if (options == NULL) {
        raise(T_ENULL_ARG, "options");
        return;
    }
    initialize_default_options(options);

    int c = 0;

    while ((c = getopt(argc, (char * const *) argv, "cb:p:e:h")) != -1) {
        DEBUG("-%c option encountered with value %s", c, optarg == NULL ? "" : optarg);
        switch (c) {
            case 'c':
                options->clean = true;
                break;

            case 'b':
                strncpy(options->binary_path, optarg, PATH_MAX - 1);
                break;

            case 'p':
                strncpy(options->process_name, optarg, PATH_MAX - 1);
                break;

            case 'e':
                strncpy(options->entry_function, optarg, FUNCTION_NAME_MAX - 1);
                break;

            case 'h':
                display_help();
                break;

            case '?':
                if (optopt == 'c' || optopt == 'b' || optopt == 'p' || optopt == 'e' || optopt == 'h') {
                    raise(T_ECLI_EMPTY, "%c", optopt);
                    return;
                }
                WARN("Unknown option '-%c'", optopt);

            default:
                ERROR("Unexpected exception while parsing command line. Aborting.");
                exit(EXIT_FAILURE);
        }

        if (error_occurred()) return;
    }

    if (validate_options(options) == false) {
        raise(T_ECLI_REQ, "One of the options is missing");
        display_help();
        return;
    }


}


static void initialize_default_options(options_t * const options) {
    options->clean = false;
    memset(options->binary_path, 0, PATH_MAX);
    memset(options->process_name, 0, PATH_MAX);
    memset(options->entry_function, 0, FUNCTION_NAME_MAX);
}


static void display_help(void) {
    printf("Usage: \n");
}


static bool validate_options(options_t const * const options) {
    bool validated = true;

    validated = validated && (strlen(options->binary_path) > 0);
    validated = validated && (strlen(options->process_name) > 0);
    validated = validated && (strlen(options->entry_function) > 0);

    return validated;
}