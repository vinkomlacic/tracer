#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdnoreturn.h>

#define TRUE 1

noreturn void signal_handler(int);
void f1(void);

int main(void) {
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    puts("Unable to attach SIGINT handler.");
    exit(EXIT_FAILURE);
  }

  puts("Started the infinite loop program. To exit use CTRL-C.");
  printf("Address of the function f1: %lx\n\n", (unsigned long) f1);

  while (TRUE) {
    f1();
  }

  return EXIT_SUCCESS;
}

noreturn void signal_handler(const int signo) {
  if (signo == SIGINT) {
    puts("\nSIGINT received.");
  }

  puts("Terminating program...");
  exit(EXIT_SUCCESS);
}

void f1(void) {
  puts("Looping...");
  sleep(2);
}