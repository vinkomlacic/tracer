#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

#include "t_error.h"
#include "pread.h"
#include "process_info.h"


static intptr_t get_symbol_offset(char const * process_name, char const * symbol);
static intptr_t get_process_base_address(pid_t pid, char const * target);


extern intptr_t get_symbol_address_in_target(char const * const target, char const * const symbol) {
    pid_t const pid = get_pid(target);
    if (pid == -1) {
        t_errno = T_EPROC_NOT_RUNNING;
        return 0;
    }

    intptr_t const base_address = get_process_base_address(pid, target);
    intptr_t const offset = get_symbol_offset(target, symbol);
    if (offset == 0) {
        t_errno = T_ESYMBOL_NOT_FOUND;
        return 0;
    }

    return base_address + offset;
}


extern pid_t get_pid(char const * const process_name) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "pgrep %s", process_name);
    if (printed_characters < 0) {
      t_errno = T_EPRINTF;
    }

    return pread_int(command);
}


static intptr_t get_symbol_offset(char const * const process_name, char const * const symbol) {
    char command[PATH_MAX] = {0};

    int const printed_characters = snprintf(command, PATH_MAX, "nm %s | grep %s", process_name, symbol);
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return 0;
    }

    return pread_word(command);
}


static intptr_t get_process_base_address(pid_t const pid, char const * target) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.*%s", pid, target);
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return 0;
    }

    return pread_word(command);
}
