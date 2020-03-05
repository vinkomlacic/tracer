#include <stdio.h>
#include <stdlib.h>

#include "process_control.h"
#include "process_info.h"
#include "t_error.h"
#include "options.h"
#include "options_t.h"

#include "log.h"


int main(int const argc, char * const argv[]) {
    options_t options = parse_options(argc, argv);
    check_for_error();

    INFO("Looking up %s in the targets symbol table...", options.symbol);
    intptr_t const symbol_address = get_symbol_address_in_target(options.target, options.symbol);
    check_for_error();
    INFO("Symbol %s found at address: %#lx", options.symbol, symbol_address);

    int const pid = get_pid(options.target);
    check_for_error();
    DEBUG("PID (%s) = %d", options.target, pid);

    INFO("Attaching target to this process..");
    pattach(pid);
    check_for_error();
    INFO("Target %s attached.\n", options.target);

    uint8_t old_instruction = inject_interrupt_at(pid, symbol_address);
    check_for_error();
    INFO("Injected interrupt in the process code.");

    INFO("Continuing execution.\n");
    pcontinue(pid);
    check_for_error();

    INFO("Waiting for code to get to the breakpoint.");
    breakpoint_handler(pid, symbol_address, old_instruction);
    check_for_error();
    INFO("Breakpoint caught.\n");

    printf("Press ENTER to detach the process");
    getchar();

    INFO("Detaching the target process.");
    pdetach(pid);
    check_for_error();

    return EXIT_SUCCESS;
}