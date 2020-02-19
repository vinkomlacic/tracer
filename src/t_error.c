#include <stdio.h>
#include <errno.h>
#include "t_error.h"


t_errno_t t_errno = T_SUCCESS;


extern void t_perror(const char *message) {
  switch (t_errno) {
    case T_SUCCESS:
    printf("%s: no error\n", message);
    break;

    case T_ERROR:
    case T_EPOPEN:
    case T_EPCLOSE:
    case T_EPREAD:
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

    default:
    printf("%s: unrecognized error.\n", message);
  }
}
