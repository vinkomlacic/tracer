#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "process_info.h"
#include "t_error.h"
#include "pread.h"


static uint64_t get_symbol_offset(char const * const process_name, char const * const symbol);
static uint64_t long get_process_base_address(int const pid, char const * const target);


extern uint64_t get_symbol_address_in_target(char const * const target, char const * const symbol) {
  int const pid = get_pid(target);
  if (pid == -1) {
    t_errno = T_EPROC_NOT_RUNNING;
    return 0UL;
  }

  uint64_t const base_address = get_process_base_address(pid);
  uint64_t const offset = get_symbol_offset(target, symbol);
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


static uint64_t get_symbol_offset(char const * const process_name, char const * const symbol) {
  char command[PATH_MAX] = {0};
  
  int const printed_characters = snprintf(command, PATH_MAX, "nm %s | grep %s", process_name, symbol);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
    return 0UL;
  }

  return pread_word(command);
}


static uint64_t get_process_base_address(int const pid, char const * target) {
  char command[PATH_MAX] = {0};
  
  int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.x.*%s", pid, target);
  if (printed_characters < 0) {
    t_errno = T_EPRINTF;
    return 0UL;
  }
  
  return pread_word(command);
}
