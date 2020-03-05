#include <stdio.h>
#include <stdarg.h>

#include "t_error.h"
#include "log.h"
#include "pread.h"


__attribute__ ((format(scanf, 2, 3)))
static void pread_item(char const *command, char const *format, ...);



extern intptr_t pread_word(char const * const command) {
  intptr_t output = 0L;
  pread_item(command, "%lx", &output);
  if (error_occurred()) {
    return 0L;
  }
  TRACE("%s => %#lx", command, output);

  return output;
}


extern int pread_int(char const * const command) {
  int output = 0;
  pread_item(command, "%d", &output);
  if (error_occurred()) {
    return -1;
  }
  TRACE("%s => %d", command, output);

  return output;
}


static void pread_item(char const * const command, char const * const format, ...) {
    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        t_errno = T_EPOPEN;
    }

    char buffer[BUFFER_LENGTH] = {0};
    if (fgets(buffer, BUFFER_LENGTH, pipe) == NULL) {
        t_errno = T_EFGETS;
        return;
    }

    va_list arguments;
    va_start(arguments, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    // Warning is disabled because the format is checked when passed to this function
    if (sscanf(buffer, format, va_arg(arguments, void const *)) != 1) {
#pragma GCC diagnostic pop
        t_errno = T_EPREAD;
        return;
    }
    va_end(arguments);

    if (pclose(pipe) == -1) {
        t_errno = T_EPCLOSE;
    }
}
