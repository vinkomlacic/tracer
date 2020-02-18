#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

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

  const char *target = parse_arguments(TARGET_NAME, argc, argv);
  check_error();
  const char *function = parse_arguments(FUNCTION_NAME, argc, argv);
  check_error();
  const unsigned long function_address = get_symbol_address_in_target(target, function);
  check_error();

  printf("%s's address in %s = %lx\n", function, target, function_address);

  const int pid = get_pid(target);
  printf("PID (%s) = %d\n", target, pid);

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


static void check_error(void) {
  if (t_errno != T_SUCCESS) {
    t_perror("Error");
    exit(EXIT_FAILURE);
  }
}