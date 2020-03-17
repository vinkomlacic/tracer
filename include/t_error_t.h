#pragma once

#include <linux/limits.h>

#include "t_errno_t.h"

#define MAX_MESSAGE_SIZE 160


typedef struct ERROR {
    unsigned line;
    t_errno_t code;
    char *filename;
    char message[MAX_MESSAGE_SIZE];
} t_error_t;

extern t_error_t t_error;
