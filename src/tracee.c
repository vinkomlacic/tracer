#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


static void f1(void);
static int f2(int arg) __attribute__ ((unused));

int main(void) {
    printf("Press ENTER to start\n");
    getchar();
    int c = 0;

    do {
        printf("Press ENTER to call f1, q for call and exit\n");
        c = getchar();

        f1();
    } while (c != 'q');

    return EXIT_SUCCESS;
}

static void f1(void) {
    printf("f1: press ENTER...\n");
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
