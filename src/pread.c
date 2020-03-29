#include <stdio.h>
#include <stdarg.h>

#include "t_error.h"
#include "log.h"

#include "pread.h"


__attribute__ ((format(scanf, 2, 3)))
static void pread_item(char const command[], char const format[], ...);


extern intptr_t pread_word(char const command[const]) {
  intptr_t output = 0L;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
  pread_item(command, "%lx", &output);
#pragma GCC diagnostic pop

  if (error_occurred()) {
    return 0L;
  }

  return output;
}


extern int pread_int(char const command[const]) {
  int output = 0;
  pread_item(command, "%d", &output);
  if (error_occurred()) {
    return -1;
  }

  return output;
}


extern void pread_raw_line(char const command[const], char output[static BUFFER_LENGTH]) {
    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        raise(T_EPOPEN, "%s", command);
        return;
    }

    if (fgets(output, BUFFER_LENGTH, pipe) == NULL) {
        raise(T_EFGETS, "%s", command);
        return;
    }
    output[strlen(output) - 1] = '\0'; // remove newline

    if (pclose(pipe) == -1) {
        raise(T_EPCLOSE, "%s", command);
    }

    TRACE("Called: %s", command);
    TRACE("Output: %s", output);
}


static void pread_item(char const command[const], char const format[const], ...) {
    char buffer[BUFFER_LENGTH] = {0};

    pread_raw_line(command, buffer);
    if (error_occurred()) return;

    va_list arguments;
    va_start(arguments, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    // Warning is disabled because the format is checked when passed to this function
    if (sscanf(buffer, format, va_arg(arguments, void const *)) != 1) {
#pragma GCC diagnostic pop
        raise(T_EPREAD, "format %s", format);
        return;
    }
    va_end(arguments);
}
