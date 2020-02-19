#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "cli.h"
#include "process_info.h"
#include "t_error.h"
#include "tracer.h"


static void check_for_error(void);


int main(int const argc, char const * const argv[static argc]) {
  if (arguments_are_invalid(argc, argv)) {
    puts("tracer: invalid arguments");
    print_usage();
    exit(EXIT_FAILURE);
  }

  int const target_index = get_option_value(g_required_options[TARGET], argc, argv);
  check_for_error();
  char const * const target = argv[target_index];
  int const symbol_index = get_option_value(g_optional_options[SYMBOL], argc, argv);
  check_for_error();
  char const * const symbol = symbol_index == -1 ? "" : argv[symbol_index];

  printf("Target = %s\n", target);
  printf("Symbol = %s\n", symbol);

  int const pid = get_pid(target);
  check_for_error();
  printf("PID (%s) = %d\n", target, pid);

  if (strcmp(symbol, "") != 0) {
    unsigned long const symbol_address = get_symbol_address_in_target(target, symbol);
    printf("Symbol: %s; Address: %lx\n", symbol, symbol_address);
  }

  ptrace(PTRACE_ATTACH, pid, NULL, NULL);
  printf("Target %s attached.\n", target);
  
  ptrace(PTRACE_CONT, pid, NULL, SIGSTOP);
  puts("Stopping the target.");

  printf("Continue (Y/n): ");
  getchar();

  ptrace(PTRACE_CONT, pid, NULL, SIGCONT);
  puts("Restarting the stopped process.");

  ptrace(PTRACE_DETACH, pid, NULL, NULL);
  puts("Detaching the target process.");

  return EXIT_SUCCESS;
}


static void check_for_error(void) {
  if (t_errno != T_SUCCESS) {
    t_perror("tracer");
    exit(EXIT_FAILURE);
  }
}