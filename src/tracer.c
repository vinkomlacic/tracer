#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "process_control.h"
#include "inject_code.h"
#include "process_info.h"
#include "ptrace_wrapper.h"
#include "t_error.h"
#include "pstate.h"
#include "options.h"
#include "procmem.h"
#include "log.h"


int virus(int argument) {
    return argument + 1;
}


static size_t const FUN_COUNT = 4;
enum FUNCTION {ENTRY_FUNCTION, POSIX_MEMALIGN, MPROTECT, FREE};

static void inspect_target_binary(char const binary_path[], char const entry_function[], intptr_t function_offset[]);
static pstate_t attach_to_process(char const process_name[]);
static void inspect_running_process(pid_t pid, intptr_t const function_offset[], intptr_t function_address[]);


int main(int const argc, char const * const argv[const]) {
    options_t options = {false, "", "", ""};
    parse_options(argc, argv, &options);
    check_for_error();

    INFO("Initializing pstate struct. Getting pid of the target...");
    pstate_t pstate = {.pid = get_pid(options.process_name)};
    check_for_error();
    INFO("PID found: %d", pstate.pid);

    INFO("Looking up %s in the targets symbol table...", options.entry_function);
    intptr_t const entry_function = get_symbol_address_in_target(pstate.pid, options.entry_function);
    check_for_error();
    INFO("Symbol %s found at address: %#lx", options.entry_function, entry_function);

    pattach(pstate.pid);
    check_for_error();
    INFO("Target \"%s\" attached. Target process stopped.", options.process_name);

    INFO("Changing process memory at %#lx", entry_function);
    inject_breakpoint(&pstate, entry_function);
    check_for_error();
    INFO("Breakpoint injected at %#lx <%s>", pstate.change_address, options.entry_function);

    INFO("Continuing execution");
    pcontinue(pstate.pid);
    check_for_error();

    INFO("Waiting for code to get to the breakpoint");
    wait_for_breakpoint(pstate.pid);
    check_for_error();
    INFO("Target process stopped. Target process at entry function. Saving registers");
    save_process_regs(&pstate);
    pstate.changed_regs.rip -= 1ULL;
    check_for_error();

    uint8_t code[MAX_CODE_LENGTH] = {0};
    size_t code_size = get_function_code(getpid(), (intptr_t) virus, code);
    check_for_error();

    int const alignment = getpagesize();
    intptr_t memory_address = call_posix_memalign(&pstate, alignment, code_size);
    check_for_error();
    INFO("posix_memalign(%d, %d) => %#lx", alignment, code_size, memory_address);

    call_mprotect(&pstate, memory_address, getpagesize(), PROT_READ|PROT_WRITE|PROT_EXEC);
    check_for_error();
    INFO("called mprotect(%#lx, %d, %d)", memory_address, getpagesize(), PROT_EXEC | PROT_WRITE | PROT_EXEC);

    inject_raw_code_to_process(pstate.pid, memory_address, code_size, code);
    check_for_error();

    if (options.clean) {
        int arg = 5;
        int ret_val = call_virus(&pstate, memory_address, arg);
        check_for_error();
        INFO("Function call: virus(%d) => %d", arg, ret_val);

        INFO("Deleting virus from memory");
        scrub_memory(pstate.pid, memory_address, code_size);

        INFO("Reverting permissions to original heap permissions on the memory region");
        call_mprotect(&pstate, memory_address, getpagesize(), PROT_READ|PROT_WRITE);
        check_for_error();

        INFO("Deallocated virus memory");
        call_free(&pstate, memory_address);
        check_for_error();
    }

#ifdef DEBUG_ENABLE
    DEBUG("Looking up injected code in tracee to verify");
    for (size_t i = 0; i < code_size; i++) {
        DEBUG("<%#lx>: %#x", memory_address + i, proc_read_byte(pstate.pid, memory_address + i));
    }
#endif

    INFO("Reverting process state before first change. Address of first change: %#lx", pstate.change_address);
    revert_to(&pstate);
    check_for_error();

    if (options.clean == false) {
        inject_trampoline(pstate.pid, entry_function, memory_address);
        check_for_error();
        INFO("Injected trampoline. Releasing %s", options.process_name);
    }

    INFO("Detaching the target process.");
    pdetach(pstate.pid);
    check_for_error();

    return EXIT_SUCCESS;
}


static void inspect_target_binary(char const binary_path[], char const entry_function[], intptr_t function_offset[]) {
    // TODO
}


static pstate_t attach_to_process(char const process_name[]) {
    // TODO
    pstate_t pstate = {.pid = 0};
    return pstate;
}


static void inspect_running_process(pid_t pid, intptr_t const function_offset[], intptr_t function_address[]) {
    // TODO
}
