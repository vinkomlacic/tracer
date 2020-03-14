#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "t_error.h"

t_errno_t t_errno = T_SUCCESS;


extern void t_perror(char const message[const]) {
    switch (t_errno) {
        case T_SUCCESS:
            ERROR("No error");
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
            ERROR("%s: %s", message, strerror(errno));
            break;

        case T_EFGETS:
            ERROR("%s: fgets failed reading from a stream.", message);
            break;

        case T_EPRINTF:
            ERROR("%s: function from printf family failed printing to a stream.", message);
            break;

        case T_ECLI_ARGV:
            ERROR("%s: error reading command line arguments.", message);
            break;

        case T_EVALUE_NOT_FOUND:
            ERROR("%s: value not found in an array.", message);
            break;

        case T_EPROC_NOT_RUNNING:
            ERROR("%s: target process not running.", message);
            break;

        case T_ESYMBOL_NOT_FOUND:
            ERROR("%s: specified symbol could not found in the object file.", message);
            break;

        case T_ECLI_EMPTY:
            ERROR("%s: one of the options has no value.", message);
            break;

        case T_ECLI_REQ:
            ERROR("%s: required option is missing.", message);
            break;

        case T_ENULL_ARG:
            ERROR("%s: null argument passed to a function.", message);
            break;

        case T_EENCODING:
            ERROR("%s: encoding error while copying string.", message);
            break;

        case T_ESTR_TRUNC:
            ERROR("%s: string truncated while copying string.", message);
            break;

        case T_EWAIT:
            ERROR("%s: wait / waitpid / waitid error.", message);
            break;

        default:
          ERROR("%s: unrecognized error.", message);
    }
}


extern void check_for_error(void) {
    if (error_occurred()) {
        t_perror("caused by");
        exit(EXIT_FAILURE);
    }
}


extern bool error_occurred(void) {
    return t_errno != T_SUCCESS;
}