#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "process_info.h"
#include "t_error.h"
#include "options.h"
#include "options_t.h"


static void print_symbol_address_in_target(char const * const symbol, char const * const target);
static void check_for_error(void);


int main(int const argc, char * const argv[static argc]) {
  options_t options = parse_options(argc, argv);
  check_for_error();

  printf("Target = %s\n", options.target);

  if (option_is_present(&options, SYMBOL)) {
    printf("Symbol = %s\n", options.symbol);
    print_symbol_address_in_target(options.symbol, options.target);
  }

  int const pid = get_pid(options.target);
  check_for_error();
  printf("PID (%s) = %d\n", options.target, pid);

  ptrace(PTRACE_ATTACH, pid, NULL, NULL);
  printf("Target %s attached.\n", options.target);

  printf("Continue (Y/n): ");
  getchar(); getchar();

  ptrace(PTRACE_CONT, pid, NULL, SIGCONT);
  puts("Restarting the stopped process.");

  printf("Detach process(Y/n): ");
  getchar(); getchar();

  ptrace(PTRACE_DETACH, pid, NULL, NULL);
  puts("Detaching the target process.");

  return EXIT_SUCCESS;
}


static void print_symbol_address_in_target(char const * const symbol, char const * const target) {
  unsigned long const symbol_address = get_symbol_address_in_target(target, symbol);
  printf("Symbol: %s; address: 0x%lx\n", symbol, symbol_address);
}


static void check_for_error(void) {
  if (t_errno != T_SUCCESS) {
    t_perror("tracer");
    exit(EXIT_FAILURE);
  }
}