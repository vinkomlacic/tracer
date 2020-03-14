#pragma once

#include <sys/user.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#define PROCESS_NAME_MAX 256
#define CODE_TO_RESTORE_MAX 34

/**
 * Definition of a struct which holds process state.
 */
typedef struct PSTATE {
    struct user_regs_struct changed_regs;
    intptr_t change_address;
    pid_t pid;
    unsigned short changed_code_len;
    char name[PROCESS_NAME_MAX];
    uint8_t changed_code[CODE_TO_RESTORE_MAX];
} pstate_t;