#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "process_info.h"
#include "t_error.h"
#include "pread.h"


static int pgrep(const char *process_name);
static unsigned long get_symbol_offset(const char *process_name, const char *symbol);
static unsigned long get_process_base_address(const int pid);


extern unsigned long get_symbol_address_in_target(const char *target, const char *symbol) {
  int pid = pgrep(target);
  if (pid == -1) {
    t_errno = T_EPROC_NOT_RUNNING;
    return 0UL;
  }

  unsigned long base_address = get_process_base_address(pid);
  unsigned long offset = get_symbol_offset(target, symbol);
  if (offset == 0UL) {
    t_errno = T_ESYMBOL_NOT_FOUND;
    return 0UL;
  }

  return base_address + offset;
}


static int pgrep(const char *process_name) {
  char command[PATH_MAX] = {0};

  int printed_characters = snprintf(command, PATH_MAX, "pgrep %s", process_name);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
  }
  
  return pread_int(command);
}


static unsigned long get_symbol_offset(const char *process_name, const char *symbol) {
  char command[PATH_MAX] = {0};
  
  int printed_characters = snprintf(command, PATH_MAX, "nm %s | grep %s", process_name, symbol);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
    return 0UL;
  }

  return pread_unsigned_long(command);
}


static unsigned long get_process_base_address(const int pid) {
  char command[PATH_MAX] = {0};
  
  int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps", pid);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
    return 0UL;
  }

  return pread_unsigned_long(command);
}
