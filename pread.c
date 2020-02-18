#include <stdio.h>

#include "pread.h"
#include "t_error.h"


static void pread_item(const char *command, const char *format_specifier, void *output);


extern unsigned long pread_unsigned_long(const char *command) {
  unsigned long output = 0UL;
  pread_item(command, "%lx", &output);
  if (t_errno) {
    return 0UL;
  }

  return output;
}


extern int pread_int(const char *command) {
  int output = 0;
  pread_item(command, "%d", &output);
  if (t_errno) {
    return -1;
  }

  return output;
}


static void pread_item(const char *command, const char *format_specifier, void *output) {
  FILE *pipe = popen(command, "r");
  if (pipe == NULL) {
    t_errno = T_EPOPEN;
  }

  char buffer[BUFFER_LENGTH] = {0};
  if (fgets(buffer, BUFFER_LENGTH, pipe) == NULL) {
    t_errno = T_EFGETS;
    return;
  }

  if (sscanf(buffer, format_specifier, output) != 1) {
    t_errno = T_EPREAD;
    return;
  }

  if (pclose(pipe) == -1) {
    t_errno = T_EPCLOSE;
  }
}