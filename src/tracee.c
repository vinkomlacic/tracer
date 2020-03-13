#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


static void f1(void);
static int f2(int arg) __attribute__ ((unused));

int main(void) {
    printf("Press ENTER to start");
    getchar();

    f1();

    return EXIT_SUCCESS;
}

static void f1(void) {
    printf("Press ENTER to exit...");
    getchar();
    printf("Errno: %s\n", strerror(errno));
}

static int f2(int const arg) {
    printf("Function f2 called with argument value %d\n", arg);
    printf("Press ENTER to return");
    getchar();
    printf("Returning %d\n", arg+1);

    return arg+1;
}
