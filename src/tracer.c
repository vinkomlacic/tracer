#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "process_control.h"
#include "process_info.h"
#include "t_error.h"
#include "options.h"
#include "options_t.h"


static void check_for_error(void);


int main(int const argc, char * const argv[static argc]) {
  options_t options = parse_options(argc, argv);
  check_for_error();

  printf("\nLooking up %s in the targets symbol table...\n", options.symbol);
  unsigned long const symbol_address = get_symbol_address_in_target(options.target, options.symbol);
  check_for_error();
  printf("Symbol %s found at address: 0x%lx\n", options.symbol, symbol_address);

  int const pid = get_pid(options.target);
  check_for_error();
  printf("\nPID (%s) = %d\n", options.target, pid);

  puts("\nAttaching target to this process.");
  pattach(pid);
  check_for_error();
  printf("Target %s attached.\n", options.target);
  
  inject_interrupt_at(pid, symbol_address);
  check_for_error();
  printf("\nInjected interrupt in the process code.\n");

  printf("Continuing execution.\n");
  pcontinue(pid);
  check_for_error();

  printf("\nPress ENTER to continue the process");
  getchar();

  pdetach(pid);
  check_for_error();
  puts("\nDetaching the target process.");

  return EXIT_SUCCESS;
}


static void check_for_error(void) {
  if (t_errno != T_SUCCESS) {
    t_perror("tracer");
    exit(EXIT_FAILURE);
  }
}