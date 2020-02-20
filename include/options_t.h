/**
 * Definitions of types containing possible CLI options.
 */
#pragma once

#define TARGET_MAX 256
#define SYMBOL_MAX 256

typedef struct {
  /* Required options */
  char target[TARGET_MAX];

  /* Optional options */
  char symbol[SYMBOL_MAX];
} options_t;
