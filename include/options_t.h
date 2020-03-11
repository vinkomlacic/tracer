/**
 * Definitions of types containing possible CLI options.
 */
#pragma once

#define TARGET_MAX (256)
#define SYMBOL_MAX (256)

typedef struct {
  /* Required options */
  char target[TARGET_MAX];
  char symbol[SYMBOL_MAX];
  char function_to_call[SYMBOL_MAX];
  int argument;
} options_t;
