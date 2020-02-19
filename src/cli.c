#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "t_error.h"

static int strings_are_equal(const char *string_1, const char *string_2);
static int strings_are_different(const char *string_1, const char *string_2);
static int get_index_of_string_by_value(const char *value, const int length, char *array[]);


extern int arguments_are_invalid(const int argc, char *argv[static argc]) {
  if (argc != REQUIRED_ARGUMENT_COUNT) {
    return TRUE;
  }

  for (int i = 1; i < argc; i += 2) {
    if (
      strings_are_different(argv[i], TARGET_NAME_OPTION) &&
      strings_are_different(argv[i], FUNCTION_NAME_OPTION)
    ) {
      return TRUE;
    }
  }

  return FALSE;
}


extern void print_usage(void) {
  puts("\t\t\t--- Tracer version 1.0.0 ---");
  puts("Program outputs the address of the provided function in the currently running binary.");
  puts("\nUsage: tracer -t <target_binary_name> -f <function_name>\n");
}


extern char * parse_arguments(option_t option, const int argc, char *argv[static argc]) {
  int argument_value_index = 0;

  switch (option) {
    case TARGET_NAME:
    argument_value_index = get_index_of_string_by_value(TARGET_NAME_OPTION, argc, argv) + 1;
    break;
    case FUNCTION_NAME:
    argument_value_index = get_index_of_string_by_value(FUNCTION_NAME_OPTION, argc, argv) + 1;
    break;
    default:
      t_errno = T_ECLI_ARGV;
      return NULL;
  }

  if (argument_value_index == -1) {
    return NULL;
  }

  return argv[argument_value_index];
}


static int strings_are_equal(const char *string_1, const char *string_2) {
  return !strings_are_different(string_1, string_2);
}


static int strings_are_different(const char *string_1, const char *string_2) {
  if (strcmp(string_1, string_2) == 0) {
    return FALSE;
  }

  return TRUE;
}


static int get_index_of_string_by_value(
  const char *value, const int length, char *array[static length]) {
  for (int i = 0; i < length; ++i) {
    if (strings_are_equal(value, array[i])) {
      return i;
    }
  }
  
  t_errno = T_EVALUE_NOT_FOUND;
  return -1;
}