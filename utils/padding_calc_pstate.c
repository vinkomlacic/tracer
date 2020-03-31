#include <stdio.h>
#include <stdalign.h>

#include "../include/pstate_t.h"


int main(void) {
    pstate_t pstate;

    size_t size = sizeof(pstate_t);
    size_t alignment = alignof(pstate_t);
    printf("sizeof(pstate_t): %lu\n", size);
    printf("alignof(pstate_t): %lu\n", alignment);

    size_t registers_struct = sizeof(pstate.changed_regs);
    size_t change_address = sizeof(pstate.change_address);
    size_t changed_code_len = sizeof(pstate.changed_code_len);
    size_t pid = sizeof(pstate.pid);
    size_t changed_code = sizeof(pstate.changed_code);

    size_t sum = registers_struct + change_address + changed_code_len + pid + changed_code;
    printf("Sum of sizes of members: %lu\n", sum);

    size_t padding = size - sum;

    if (padding != 0) {
        printf("Try adding %lu bytes to the last member\n", padding);
    } else {
        printf("No padding\n");
    }
}