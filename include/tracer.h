/**
 * Definitions of possible CLI options.
 */
#pragma once

#include <stddef.h>

#include "option_t.h"

typedef enum {TARGET} R_OPTION;
option_t g_required_options[] = {
  {"-t", "--target", "Attaches to a process with the name in the value."}
};
size_t g_required_options_len = 1;

typedef enum {SYMBOL} O_OPTION;
option_t g_optional_options[] = {
  {"-s", "--symbol", "Print the address of specified symbol before attaching to a process."}
};
size_t g_optional_options_len = 1;

