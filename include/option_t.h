/**
 * Type definition for the option.
 */
#pragma once

#define OPTION_SHORT_MAX 3      // format: -[one_letter_option]
#define OPTION_LONG_MAX 256     // format: --[full_option]
#define OPTION_HELP_MAX 512     // help about the option displayed on usage

typedef struct {
  char short_name[OPTION_SHORT_MAX];
  char long_name[OPTION_LONG_MAX];
  char help[OPTION_HELP_MAX];
} option_t;
