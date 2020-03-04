#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#include "process_control.h"
#include "process_info.h"
#include "t_error.h"
#include "options.h"
#include "options_t.h"


int main(int const argc, char * const argv[]) {
    options_t options = parse_options(argc, argv);
    check_for_error();

    printf("\nLooking up %s in the targets symbol table...\n", options.symbol);
    intptr_t const symbol_address = get_symbol_address_in_target(options.target, options.symbol);
    check_for_error();
    printf("Symbol %s found at address: 0x%lx\n", options.symbol, symbol_address);

    int const pid = get_pid(options.target);
    check_for_error();
    printf("\nPID (%s) = %d\n", options.target, pid);

    puts("\nAttaching target to this process..");
    pattach(pid);
    check_for_error();
    printf("Target %s attached.\n", options.target);

    uint8_t old_instruction = inject_interrupt_at(pid, symbol_address);
    check_for_error();
    printf("\nInjected interrupt in the process code.\n");
    printf("Old instruction: %x\n", old_instruction);

    printf("Continuing execution.\n");
    pcontinue(pid);
    check_for_error();

    printf("Waiting for code to get to the breakpoint.\n");
    breakpoint_handler(pid, symbol_address, old_instruction);
    check_for_error();
    printf("\nBreakpoint caught.\n");

    printf("\nPress ENTER to continue the process");
    getchar();

    pdetach(pid);
    check_for_error();
    puts("\nDetaching the target process.");

    return EXIT_SUCCESS;
}