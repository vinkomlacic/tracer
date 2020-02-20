#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdnoreturn.h>
#include <stdbool.h>

static noreturn void signal_handler(int);
static void f1(void);
static void f2(void) __attribute__ ((unused));

int main(void) {
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    puts("Unable to attach SIGINT handler.");
    exit(EXIT_FAILURE);
  }

  puts("Started the infinite loop program. To exit use CTRL-C.");
  printf("Address of the function f1: %lx\n\n", (unsigned long) f1);

  while (true) {
    f1();
  }

  return EXIT_SUCCESS;
}

static noreturn void signal_handler(const int signo) {
  if (signo == SIGINT) {
    puts("\nSIGINT received.");
  }

  puts("Terminating program...");
  exit(EXIT_SUCCESS);
}

static void f1(void) {
  puts("Looping...");

  struct timespec req = {2, 0L};
  struct timespec rem = {0 , 0L};

  // implementation note: nanosleep is used instead of sleep because the manual clearly 
  // specifies it doesn't interact with signals
  nanosleep(&req, &rem);
}

static void f2(void) {}
