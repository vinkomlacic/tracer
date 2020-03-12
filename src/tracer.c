#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "process_control.h"
#include "process_info.h"
#include "t_error.h"
#include "pstate_t.h"
#include "procmem.h"

#include "log.h"


int virus(int argument) {
    return 0;
}


int main(int const argc, char const * const argv[const]) {
    char const target[] = "tracee";
    char const symbol[] = "f1";

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

    uint8_t code[MAX_CODE_LENGTH] = {0};
    size_t code_size = get_function_code("tracer", (intptr_t) virus, code);
    check_for_error();

    size_t alignment = sizeof(unsigned long long);
    intptr_t memory_address = call_posix_memalign(&pstate, alignment, code_size);
    check_for_error();
    INFO("posix_memalign(%d, %d) => %#lx", alignment, code_size, memory_address);
    INFO("Address of the allocated memory is stored on top of the stack.");

    call_mprotect(&pstate, memory_address, code_size, PROT_EXEC);
    check_for_error();
    INFO("called mprotect(%#lx, %d, %d)", memory_address, code_size, PROT_EXEC);

    size_t injected_code = inject_virus(pstate.pid, memory_address, code_size, code);
    check_for_error();
    INFO("Injected %d bytes of the virus", injected_code);

#ifdef DEBUG_ENABLE
    DEBUG("Looking up injected code in tracee to verify");
    for (size_t i = 0; i < code_size; i++) {
        DEBUG("<%#lx>: %#x", memory_address, proc_read_byte(pstate.pid, memory_address + i));
    }
#endif

    INFO("Reverting process state before first change. Address of first change: %#lx", pstate.change_address);
    revert_to(&pstate);
    check_for_error();

    INFO("Detaching the target process.");
    pdetach(pstate.pid);
    check_for_error();

    return EXIT_SUCCESS;
}