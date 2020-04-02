#pragma once

#include <sys/user.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#define CODE_TO_RESTORE_MAX 36

/**
 * Definition of a struct which holds a subset of the process state.
 * In the context of this program all that is needed is the register values and the possible changes
 * to the code that could be done during injecting code.
 */
typedef struct PSTATE {
    struct user_regs_struct changed_regs;
    intptr_t change_address;
    size_t changed_code_len;
    pid_t const pid;
    uint8_t changed_code[CODE_TO_RESTORE_MAX];
} pstate_t;
