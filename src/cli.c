#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cli.h"
#include "t_error.h"
#include "option_t.h"


static bool required_arguments_are_provided(int const argc, char const * const argv[static argc]);
static bool strings_are_equal(char const * const string_1, char const * const string_2);
static bool option_is_in_argv(
  option_t const option,
  size_t const argc,
  char const * const argv[static argc]
);
static bool option_is_required(option_t const option);
static size_t get_option_index_in_argv(
  option_t const option,
  int const argc,
  char const * const argv[static argc]
);
static void print_option(option_t const option);


extern bool arguments_are_invalid(int const argc, char const * const argv[static argc]) {
  if (!required_arguments_are_provided(argc, argv)) {
    return true;
  }

  return false;
}


extern void print_usage(void) {
  puts("\t\t\t--- Tracer version 1.0.0 ---");
  puts("Program attached to the specifiedd running process and controls it.\n");
  
  puts("Required arguments:");
  for (size_t i = 0; i < g_required_options_len; i++) {
    print_option(g_required_options[i]);
  }

  puts("\nOptional arguments:");
  for (size_t i = 0; i < g_optional_options_len; i++) {
    print_option(g_optional_options[i]);
  }
  puts("");
}


static void print_option(option_t const option) {
  printf("\t%s, %s\n\t\t\t\t%s\n", option.short_name, option.long_name, option.help);
}


extern int get_option_value(
  option_t const option,
  int const argc,
  char const * const argv[static argc]
) {
  if (!option_is_in_argv(option, argc, argv)) {
    if (option_is_required(option)) {
      t_errno = T_ECLI_ARGV;
    }
    return -1;
  }

  size_t const option_index = get_option_index_in_argv(option, argc, argv);

  if (argv[option_index + 1][0] == '-') {
    t_errno = T_ECLI_EMPTY;
    return -1;
  }

  return option_index + 1;
}


static bool required_arguments_are_provided(int const argc, char const * const argv[static argc]) {
  for (size_t i = 0; i < g_required_options_len; i++) {
    if (!option_is_in_argv(g_required_options[i], argc, argv)) {
      return false;
    }

    size_t const option_index_in_argv = get_option_index_in_argv(g_required_options[i], argc, argv);
    
    if (argv[option_index_in_argv + 1][0] == '-') {
      return false;
    }
  }

  return true;
}


static bool strings_are_equal(char const *string_1, char const *string_2) {
  if (strcmp(string_1, string_2) == 0) {
    return true;
  }

  return false;
}


static bool option_is_in_argv(
  option_t const option,
  size_t const argc,
  char const * const argv[static argc]
) {
  for (size_t i = 0; i < argc; i++) {
    if (
      strings_are_equal(argv[i], option.short_name) ||
      strings_are_equal(argv[i], option.long_name)
    ) {
      return true;
    }
  }

  return false;
}


static bool option_is_required(option_t const option) {
  for (size_t i = 0; i < g_required_options_len; i++) {
    if (strings_are_equal(g_required_options[i].short_name, option.short_name)) {
      return true;
    }
  }

  return false;
}


static size_t get_option_index_in_argv(
  option_t const option,
  int const argc,
  char const * const argv[static argc]
) {
  for (int i = 0; i < argc; i++) {
    if (
      strings_are_equal(argv[i], option.short_name) ||
      strings_are_equal(argv[i], option.long_name)
    ) {
      return i;
    }
  }
  
  t_errno = T_EVALUE_NOT_FOUND;
  return -1;
}
