#include <stdio.h>
#include <stdlib.h>


static void f1(void);
static void f2(void) __attribute__ ((unused));

int main(void) {
  f1();

  return EXIT_SUCCESS;
}

static void f1(void) {
  printf("Press ENTER to exit...");
  getchar();
}

static void f2(void) {}
