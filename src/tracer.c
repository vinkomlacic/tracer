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


#define FUN_COUNT 3
enum LIBC_FUNCTIONS {POSIX_MEMALIGN, MPROTECT, FREE};

static intptr_t inspect_target_binary(char const binary_path[], char const entry_function[]);
static void inspect_libc(char const binary_path[], intptr_t function_offset[]);
static pstate_t attach_to_process(char const process_name[], intptr_t entry_offset);
static void inspect_running_process(pid_t pid, intptr_t const function_offset[], intptr_t function_address[]);
static intptr_t inject_virus(pstate_t * pstate, intptr_t const function_address[], size_t *virus_size);
static void execute_virus(pstate_t *pstate, intptr_t virus_address, int argument);
static void cleanup(pstate_t * pstate, intptr_t const function_address[], intptr_t virus_address, size_t virus_size);
static void restore_process_state(pstate_t const * pstate);
static void replace_entry_function(pid_t pid, intptr_t entry_function, intptr_t virus_address);
static void detach_process(pstate_t const * pstate);


/*
 * Instrumentation functions will not be linked in the tracee where the virus is copied
 * which will segfault tracee. That is why it's best to turn off sanitizers for this function.
 */
__attribute__ ((no_sanitize("address", "undefined")))
int virus(int argument);


int main(int const argc, char * const argv[const]) {
    options_t options = {false, "", "", ""};
    parse_options(argc, argv, &options);
    check_for_error();

    intptr_t offset[FUN_COUNT] = {0};
    intptr_t address[FUN_COUNT] = {0};

    intptr_t entry_address_offset = inspect_target_binary(options.binary_path, options.entry_function);
    inspect_libc(options.binary_path, offset);

    pstate_t pstate = attach_to_process(options.process_name, entry_address_offset);
    inspect_running_process(pstate.pid, offset, address);

    size_t virus_size = 0;
    intptr_t virus_address = inject_virus(&pstate, address, &virus_size);

    if (options.clean){
        execute_virus(&pstate, virus_address, 5);
        cleanup(&pstate, address, virus_address, virus_size);
    }

    restore_process_state(&pstate);
    if (options.clean == false) replace_entry_function(pstate.pid, pstate.change_address, virus_address);
    detach_process(&pstate);

    return EXIT_SUCCESS;
}


static intptr_t inspect_target_binary(char const binary_path[const], char const entry_function[const]) {
    INFO("Inspecting binary: %s ...", binary_path);

    intptr_t const address = get_symbol_offset_in_binary(binary_path, entry_function, false);
    check_for_error();
    INFO("%s function found at %#lx", entry_function, address);

    return address;
}


static void inspect_libc(char const binary_path[const], intptr_t function_offset[const]) {
    INFO("Inspecting libc runtime library in binary %s ...", binary_path);

    char libc_path[PATH_MAX] = {0};
    get_libc_path(binary_path, libc_path);
    check_for_error();
    INFO("libc runtime library found at %s", libc_path);

    function_offset[POSIX_MEMALIGN] = get_symbol_offset_in_binary(libc_path, "posix_memalign", true);
    check_for_error();
    INFO("posix_memalign found at %#lx in libc", function_offset[POSIX_MEMALIGN]);

    function_offset[MPROTECT] = get_symbol_offset_in_binary(libc_path, "mprotect", true);
    check_for_error();
    INFO("mprotect found at %#lx in libc", function_offset[MPROTECT]);

    function_offset[FREE] = get_symbol_offset_in_binary(libc_path, "free", true);
    check_for_error();
    INFO("free found at %#lx in libc\n", function_offset[FREE]);
}


static pstate_t attach_to_process(char const process_name[const], intptr_t const entry_offset) {
    INFO("Attaching to process %s ...", process_name);

    pstate_t pstate = {.pid = get_pid(process_name)};
    check_for_error();
    INFO("Found %s pid: %d", process_name, pstate.pid);

    intptr_t const entry_base = get_process_base_address(pstate.pid);
    intptr_t const entry_point = entry_base + entry_offset;
    check_for_error();
    INFO("Entry point: %#lx", entry_point);

    pattach(pstate.pid);
    check_for_error();
    INFO("Target \"%s\" attached. Target process stopped", process_name);

    inject_breakpoint(&pstate, entry_point);
    check_for_error();
    INFO("Breakpoint injected at %#lx", entry_point);

    pcontinue(pstate.pid);
    check_for_error();
    INFO("Continuing execution");

    wait_for_breakpoint(pstate.pid);
    check_for_error();
    INFO("Breakpoint caught");

    save_process_regs(&pstate);
    pstate.changed_regs.rip -= 1ULL;
    check_for_error();
    INFO("Process state saved\n");

    return pstate;
}


static void inspect_running_process(pid_t const pid, intptr_t const function_offset[const], intptr_t function_address[const]) {
    INFO("Inspecting running process %d", pid);

    intptr_t libc_base = locate_libc_in(pid);
    check_for_error();
    INFO("Located libc in process %d at %#lx", pid, libc_base);

    for (size_t i = 0; i < FUN_COUNT; i++) {
        function_address[i] = libc_base + function_offset[i];
    }
    INFO("Function addresses calculated\n");
}


static intptr_t inject_virus(pstate_t * const pstate, intptr_t const function_address[const], size_t * const virus_size) {
    INFO("Injecting virus to %d ...", pstate->pid);

    uint8_t code[MAX_CODE_LENGTH] = {0};
    size_t code_size = get_function_code(getpid(), (intptr_t) virus, code);
    check_for_error();
    INFO("Virus code retrieved (%lu bytes)", code_size);

    int const alignment = getpagesize();
    intptr_t block_address = call_posix_memalign(pstate, function_address[POSIX_MEMALIGN], alignment, code_size);
    check_for_error();
    INFO("Allocated %lu bytes of memory with %d alignment at %#lx", code_size, alignment, block_address);

    call_mprotect(pstate, function_address[MPROTECT], block_address, (size_t) alignment, PROT_READ | PROT_WRITE | PROT_EXEC);
    check_for_error();
    INFO("Set new permissions on the allocated block (rwx)");

    proc_write_block(pstate->pid, block_address, code_size, code);
    check_for_error();
    INFO("Virus code successfully injected in the allocated memory\n");

    *virus_size = code_size;
    return block_address;
}


static void execute_virus(pstate_t * const pstate, intptr_t const virus_address, int const argument) {
    int ret_val = call_virus(pstate, virus_address, argument);
    check_for_error();
    INFO("Calling virus with %d, returned %d\n", argument, ret_val);
}


static void cleanup(
        pstate_t * const pstate, intptr_t const function_address[const],
        intptr_t const virus_address, size_t const virus_size
) {
    INFO("Scrubbing and cleaning up allocated memory at %#lx", virus_address);

    scrub_memory(pstate->pid, virus_address, virus_size);
    check_for_error();
    INFO("Memory block set to 0 (%lu bytes)", virus_size);

    call_mprotect(pstate, function_address[MPROTECT], virus_address, (size_t) getpagesize(), PROT_READ|PROT_WRITE);
    check_for_error();
    INFO("Block's permissions set to the original permissions of the heap (rw-)");

    call_free(pstate, function_address[FREE], virus_address);
    check_for_error();
    INFO("Virus deallocated\n");
}


static void restore_process_state(pstate_t const * const pstate) {
    revert_to(pstate);
    check_for_error();
    INFO("Process registers and code restored (%lu bytes at %#lx)\n", pstate->changed_code_len, pstate->change_address);
}


static void replace_entry_function(pid_t const pid, intptr_t const entry_function, intptr_t const virus_address) {
    inject_trampoline(pid, entry_function, virus_address);
    check_for_error();
    INFO("Entry function <%#lx> replaced with virus <%#lx>\n", entry_function, virus_address);
}


static void detach_process(pstate_t const *  const pstate) {
    pdetach(pstate->pid);
    check_for_error();
    INFO("Process %d detached\n", pstate->pid);
}


int virus(int const argument) {
    return argument + 1;
}
