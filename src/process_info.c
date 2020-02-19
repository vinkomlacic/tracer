#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "process_info.h"
#include "t_error.h"
#include "pread.h"


static unsigned long get_symbol_offset(char const * const process_name, char const * const symbol);
static unsigned long get_process_base_address(int const pid);


extern unsigned long get_symbol_address_in_target(char const * const target, char const * const symbol) {
  int const pid = get_pid(target);
  if (pid == -1) {
    t_errno = T_EPROC_NOT_RUNNING;
    return 0UL;
  }

  unsigned long const base_address = get_process_base_address(pid);
  unsigned long const offset = get_symbol_offset(target, symbol);
  if (offset == 0UL) {
    t_errno = T_ESYMBOL_NOT_FOUND;
    return 0UL;
  }

  return base_address + offset;
}


extern int get_pid(char const * const process_name) {
  char command[PATH_MAX] = {0};

  int printed_characters = snprintf(command, PATH_MAX, "pgrep %s", process_name);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
  }
  
  return pread_int(command);
}


static unsigned long get_symbol_offset(char const * const process_name, char const * const symbol) {
  char command[PATH_MAX] = {0};
  
  int const printed_characters = snprintf(command, PATH_MAX, "nm %s | grep %s", process_name, symbol);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
    return 0UL;
  }

  return pread_unsigned_long(command);
}


static unsigned long get_process_base_address(int const pid) {
  char command[PATH_MAX] = {0};
  
  int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps", pid);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
    return 0UL;
  }
  
  return pread_unsigned_long(command);
}
