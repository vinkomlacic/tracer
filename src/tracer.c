#include <stdlib.h>
#include <string.h>

#include "process_control.h"
#include "process_info.h"
#include "t_error.h"
#include "pstate_t.h"

#include "log.h"


int main(int const argc, char const * const argv[const]) {
    char const target[] = "tracee";
    char const symbol[] = "f1";
    char const function_to_call[] = "getpagesize";
    char const lib[] = "libc";
    int argument = 0;
    int expected_output = getpagesize();

    INFO("Looking up %s in the targets symbol table...", symbol);
    intptr_t const entry_function = get_symbol_address_in_target(target, symbol);
    check_for_error();
    INFO("Symbol %s found at address: %#lx", symbol, entry_function);

    INFO("Initializing pstate struct. Getting pid of the target...");
    pstate_t pstate = create_pstate();
    strncpy(pstate.name, target, sizeof(pstate.name) - 1);
    pstate.pid = get_pid(target);
    check_for_error();
    INFO("PID found: %d", pstate.pid);

    pattach(pstate.pid);
    check_for_error();
    INFO("Target \"%s\" attached. Target process stopped.", target);

    INFO("Changing process memory at %#lx", entry_function);
    set_breakpoint(entry_function, &pstate);
    check_for_error();
    INFO("Breakpoint injected at %#lx <%s>", pstate.change_address, symbol);

    INFO("Continuing execution");
    pcontinue(pstate.pid);
    check_for_error();

    INFO("Waiting for code to get to the breakpoint");
    wait_for_bp(pstate.pid);
    check_for_error();
    INFO("Target process stopped. Target process at entry function. Saving registers");
    save_process_regs(&pstate);
    pstate.changed_regs.rip -= 1ULL;
    check_for_error();

    int ret_val = call_function_in_lib(&pstate, function_to_call, lib, argument);
    check_for_error();
    INFO("Function call: %s(%d) => %d", function_to_call, argument, ret_val);
    INFO("Expected output: %d", expected_output);

    INFO("Reverting process state before first change. Address of first change: %#lx", pstate.change_address);
    revert_to(&pstate);
    check_for_error();

    INFO("Detaching the target process.");
    pdetach(pstate.pid);
    check_for_error();

    return EXIT_SUCCESS;
}