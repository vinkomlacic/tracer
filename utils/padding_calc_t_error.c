#include <stdio.h>
#include <stdalign.h>

#include "../include/t_error_t.h"


int main(void) {
    t_error_t t_error;

    size_t size = sizeof(t_error_t);
    size_t alignment = alignof(t_error_t);
    printf("sizeof(t_error_t): %lu\n", size);
    printf("alignof(t_error_t): %lu\n", alignment);

    size_t line = sizeof(t_error.line);
    size_t code = sizeof(t_error.code);
    size_t filename = sizeof(t_error.filename);
    size_t message = sizeof(t_error.message);

    size_t sum = line + code + filename + message;
    printf("Sum of sizes of members: %lu\n", sum);

    size_t padding = size - sum;

    if (padding != 0) {
        printf("Try adding %lu bytes to the last member\n", padding);
    } else {
        printf("No padding\n");
    }
}