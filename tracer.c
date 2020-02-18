#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "process_info.h"
#include "t_error.h"


static void check_error(void);


int main(int argc, char *argv[static argc]) {
  if (arguments_are_invalid(argc, argv)) {
    puts("Error: invalid arguments");
    print_usage();
    exit(EXIT_FAILURE);
  }

  // const char *target = parse_arguments(TARGET_NAME, argc, argv);
  const char *target = parse_arguments(6, argc, argv);
  check_error();
  const char *function = parse_arguments(FUNCTION_NAME, argc, argv);
  check_error();
  const unsigned long function_address = get_symbol_address_in_target(target, function);
  check_error();

  printf("%s's address in %s = %lx\n", function, target, function_address);

  return EXIT_SUCCESS;
}


static void check_error(void) {
  if (t_errno != T_SUCCESS) {
    t_perror("Error");
    exit(EXIT_FAILURE);
  }
}