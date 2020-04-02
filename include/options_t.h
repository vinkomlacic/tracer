/**
 * Definitions of types containing possible CLI options.
 */
#pragma once

#include <stdbool.h>
#include <limits.h>

#define FUNCTION_NAME_MAX (256)

/**
 * Container for options gotten from the CLI arguments.
 */
typedef struct {
  bool clean;
  char binary_path[PATH_MAX];
  char process_name[PATH_MAX];
  char entry_function[FUNCTION_NAME_MAX];
} options_t;
