#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include "options_t.h"
#include "options.h"
#include "t_error.h"


static bool required_options_are_present(options_t const * options);


extern options_t parse_options(int const argc, char * const argv[]) {
    options_t options = {"", ""};
    int c = 0;

    while ((c = getopt(argc, argv, "t:s:")) != -1) {
        switch (c) {
            case 't':
                strncpy(options.target, optarg, sizeof(options.target));
                break;

            case 's':
                strncpy(options.symbol, optarg, sizeof(options.symbol));
                break;

            case '?':
                if (optopt == 't' || optopt == 's') {
                    t_errno = T_ECLI_EMPTY;
                } else {
                    t_errno = T_ECLI_ARGV;
                }
                return options;

            default:
                puts("Unexpected exception while parsing command line. Aborting.");
                exit(EXIT_FAILURE);
        }
    }

    if (!required_options_are_present(&options)) {
        t_errno = T_ECLI_REQ;
    }

    return options;
}


extern bool option_is_present(options_t const * const options, option_t option) {
    if (options == NULL) {
        t_errno = T_ENULL_ARG;
        return false;
    }

    switch (option) {
        case TARGET:
            return options->target[0] != 0;

        case SYMBOL:
            return options->symbol[0] != 0;

        default:
            t_errno = T_ECLI_ARGV;
            return false;
    }
}


static bool required_options_are_present(options_t const * const options) {
    bool result = true;

    result &= (options->target[0] != 0);
    result &= (options->symbol[0] != 0);

    return result;
}
