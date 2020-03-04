#include <stdio.h>
#include <stdlib.h>
#include "t_error.h"


t_errno_t t_errno = T_SUCCESS;


extern void t_perror(char const * const message) {
    switch (t_errno) {
        case T_SUCCESS:
            printf("%s: no error\n", message);
            break;

        case T_ERROR:
        case T_EPOPEN:
        case T_EPCLOSE:
        case T_EPREAD:
        case T_EOPEN:
        case T_ECLOSE:
        case T_EREAD:
        case T_EWRITE:
        case T_EPTRACE:
            perror(message);
            break;

        case T_EFGETS:
            printf("%s: fgets failed reading from a stream.\n", message);
            break;

        case T_EPRINTF:
            printf("%s: function from printf family failed printing to a stream.\n", message);
            break;

        case T_ECLI_ARGV:
            printf("%s: error reading command line arguments.\n", message);
            break;

        case T_EVALUE_NOT_FOUND:
            printf("%s: value not found in an array.\n", message);
            break;

        case T_EPROC_NOT_RUNNING:
            printf("%s: target process not running.\n", message);
            break;

        case T_ESYMBOL_NOT_FOUND:
            printf("%s: specified symbol could not found in the object file.\n", message);
            break;

        case T_ECLI_EMPTY:
            printf("%s: one of the option has no value.\n", message);
            break;

        case T_ECLI_REQ:
            printf("%s: required option is missing.\n", message);
            break;

        case T_ENULL_ARG:
            printf("%s: null argument passed to a function.\n", message);
            break;

        case T_EENCODING:
            printf("%s: encoding error while copying string.\n", message);
            break;

        case T_ESTR_TRUNC:
            printf("%s: string truncated while copying string.\n", message);
            break;

        case T_EWAIT:
            printf("%s: wait / waitpid / waitid error.\n", message);
            break;

        default:
          printf("%s: unrecognized error.\n", message);
    }
}


extern void check_for_error(void) {
    if (error_occurred()) {
        t_perror("tracer");
        exit(EXIT_FAILURE);
    }
}


extern bool error_occurred(void) {
    return t_errno != T_SUCCESS;
}