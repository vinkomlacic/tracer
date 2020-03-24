#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <t_error_t.h>

#include "log.h"

#include "t_error.h"


t_error_t t_error = {0, T_SUCCESS, "", ""};


extern void t_perror() {
    char error_message[MAX_MESSAGE_SIZE] = {0};
    t_strerror(t_error.code, MAX_MESSAGE_SIZE-1, error_message);
    ERROR("at line %d of %s => %s: %s", t_error.line, t_error.filename, error_message, t_error.message);
}


extern void t_strerror(t_errno_t const error_code, size_t const string_length, char output[const static MAX_MESSAGE_SIZE]) {
    switch (error_code) {
        case T_SUCCESS:
            strncpy(output, "No error", string_length);
            break;

        case T_EPOPEN:
        case T_EPCLOSE:
        case T_EPREAD:
        case T_EOPEN:
        case T_ECLOSE:
        case T_EREAD:
        case T_EWRITE:
        case T_EPTRACE:
            strncpy(output, strerror(errno), string_length);
            break;

        case T_EFGETS:
            strncpy(output, "fgets failed reading from a stream", string_length);
            break;

        case T_EPRINTF:
            strncpy(output, "function from printf family failed printing to a stream", string_length);
            break;

        case T_ECLI_ARGV:
            strncpy(output, "error reading command line arguments", string_length);
            break;

        case T_EVALUE_NOT_FOUND:
            strncpy(output, "value not found in an array", string_length);
            break;

        case T_EPROC_NOT_RUNNING:
            strncpy(output, "target process not running", string_length);
            break;

        case T_ESYMBOL_NOT_FOUND:
            strncpy(output, "specified symbol could not found in the object file", string_length);
            break;

        case T_ECLI_EMPTY:
            strncpy(output, "one of the options has no value", string_length);
            break;

        case T_ECLI_REQ:
            strncpy(output, "required option is missing", string_length);
            break;

        case T_ENULL_ARG:
            strncpy(output, "null argument passed to a function", string_length);
            break;

        case T_EENCODING:
            strncpy(output, "encoding error while copying string", string_length);
            break;

        case T_ESTR_TRUNC:
            strncpy(output, "string truncated while copying string", string_length);
            break;

        case T_EWAIT:
            strncpy(output, "wait / waitpid / waitid error", string_length);
            break;

        case T_EFUNC_TOO_BIG:
            strncpy(output, "function code is too big", string_length);
            break;

        case T_EPSTATE_INVALID:
            strncpy(output, "illegal pstate", string_length);
            break;

        case T_EADDRESS:
            strncpy(output, "invalid address provided", string_length);
            break;

        case T_ERROR:
        default:
            strncpy(output, "unrecognized error", string_length);
    }
}


extern void check_for_error(void) {
    if (error_occurred()) {
        t_perror();
        exit(EXIT_FAILURE);
    }
}


extern bool error_occurred(void) {
    return t_error.code != T_SUCCESS;
}