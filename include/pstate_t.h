#pragma once

#include <sys/user.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#define CODE_TO_RESTORE_MAX 36

/**
 * Definition of a struct which holds process state.
 * PID and process name should be set only once - upon creation of the struct.
 * They are the indicator if the struct is initialized.
 */
typedef struct PSTATE {
    struct user_regs_struct changed_regs;
    intptr_t change_address;
    size_t changed_code_len;
    pid_t const pid;
    uint8_t changed_code[CODE_TO_RESTORE_MAX];
} pstate_t;