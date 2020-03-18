#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "t_error.h"
#include "log.h"
#include "options_t.h"

#include "options.h"


extern options_t parse_options(int const argc, char const * const argv[const]) {
    options_t options = {false};
    int c = 0;

    while ((c = getopt(argc, (char * const *) argv, "c")) != -1) {
        switch (c) {
            case 'c':
                TRACE("-c option encountered with value %s", optarg);
                options.clean = true;
                break;

            case '?':
                WARN("Option '-%c' is an unknown option or it requires an argument which was not provided", optopt);
                if (optopt == 't' || optopt == 's' || optopt == 'c') {
                    raise(T_ECLI_EMPTY, "%c", optopt);
                }
                return options;

            default:
                FATAL("Unexpected exception while parsing command line. Aborting.");
                exit(EXIT_FAILURE);
        }

        if (error_occurred()) return options;
    }

    return options;
}
