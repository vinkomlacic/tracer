#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>

#include "t_error.h"
#include "log.h"
#include "options_t.h"
#include "options.h"


static bool required_options_are_present(options_t const * options);
static int parse_int(char const string[]);


extern options_t parse_options(int const argc, char const * const argv[const]) {
    options_t options = {"", "", "", 0};
    int c = 0;

    while ((c = getopt(argc, (char * const *) argv, "t:s:c:a:")) != -1) {
        switch (c) {
            case 't':
                TRACE("-t option encountered with value %s", optarg);
                strncpy(options.target, optarg, sizeof(options.target));
                break;

            case 's':
                TRACE("-s option encountered with value %s", optarg);
                strncpy(options.symbol, optarg, sizeof(options.symbol));
                break;

            case 'c':
                TRACE("-c option encountered with value %s", optarg);
                strncpy(options.function_to_call, optarg, sizeof(options.function_to_call));
                break;

            case 'a':
                TRACE("-a option encountered with value %s", optarg);
                options.argument = parse_int(optarg);
                break;

            case '?':
                WARN("Option '-%c' is an unknown option or it requires an argument which was not provided", optopt);
                if (optopt == 't' || optopt == 's' || optopt == 'c') {
                    t_errno = T_ECLI_EMPTY;
                }
                return options;

            default:
                FATAL("Unexpected exception while parsing command line. Aborting.");
                exit(EXIT_FAILURE);
        }

        if (error_occurred()) return options;
    }

    if (!required_options_are_present(&options)) {
        t_errno = T_ECLI_REQ;
    }

    return options;
}


extern bool option_is_present(options_t const * const options, option_t const option) {
    if (options == NULL) {
        t_errno = T_ENULL_ARG;
        return false;
    }

    switch (option) {
        case TARGET:
            return options->target[0] != 0;

        case SYMBOL:
            return options->symbol[0] != 0;

        case FUNCTION_TO_CALL:
            return options->function_to_call[0] != 0;

        default:
            t_errno = T_ECLI_ARGV;
            return false;
    }
}


static bool required_options_are_present(options_t const * const options) {
    bool result = true;

    result &= option_is_present(options, TARGET);
    result &= option_is_present(options, SYMBOL);
    result &= option_is_present(options, FUNCTION_TO_CALL);

    return result;
}


static int parse_int(char const string[const]) {
    long integer = strtol(string, NULL, 10);

    if (errno != ERANGE && integer >= INT_MIN && integer <= INT_MAX) {
        return (int) integer;
    }

    t_errno = T_ECLI_ARGV;
    return 0;
}