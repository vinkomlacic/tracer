#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

#include "t_error.h"
#include "pread.h"
#include "log.h"
#include "procmem.h"

#include "process_info.h"


static intptr_t get_symbol_offset(char const process_name[], char const symbol[]);
static intptr_t get_symbol_offset_in_lib(char const lib_path[], char const symbol[]);
static intptr_t get_mprotect_offset(char const lib_path[]);
static intptr_t get_lib_base_address(pid_t pid, char const lib[]);
static void get_lib_path(pid_t pid, char const lib[], char path[]);


extern intptr_t get_symbol_address_in_target(char const target[const], char const symbol[const]) {
    pid_t const pid = get_pid(target);
    if (pid == -1) {
        t_errno = T_EPROC_NOT_RUNNING;
        return 0;
    }
    DEBUG("pid: %d", pid);

    intptr_t const base_address = get_lib_base_address(pid, target);
    DEBUG("base_address: %#lx", base_address);

    intptr_t const offset = get_symbol_offset(target, symbol);
    if (offset == 0) {
        t_errno = T_ESYMBOL_NOT_FOUND;
        return 0;
    }
    DEBUG("offset: %#x", offset);

    return base_address + offset;
}


extern intptr_t get_symbol_address_in_lib(char const target[const], char const lib[const], char const symbol[const]) {
    pid_t const pid = get_pid(target);
    if (pid == -1) {
        t_errno = T_EPROC_NOT_RUNNING;
        return 0;
    }
    DEBUG("pid: %d", pid);

    intptr_t const base_address = get_lib_base_address(pid, lib);
    DEBUG("base_address: %#lx", base_address);

    char lib_path[PATH_MAX] = {0};
    get_lib_path(pid, lib, lib_path);
    if (error_occurred()) return 0;

    intptr_t const offset = get_symbol_offset_in_lib(lib_path, symbol);
    if (offset == 0) {
        t_errno = T_ESYMBOL_NOT_FOUND;
        return 0;
    }
    DEBUG("offset: %#x", offset);

    return base_address + offset;
}


extern intptr_t get_mprotect_address(char const target[const]) {
    pid_t const pid = get_pid(target);
    if (pid == -1) {
        t_errno = T_EPROC_NOT_RUNNING;
        return 0;
    }
    DEBUG("pid: %d", pid);

    intptr_t const base_address = get_lib_base_address(pid, "libc");
    DEBUG("base_address: %#lx", base_address);

    char lib_path[PATH_MAX] = {0};
    get_lib_path(pid, "libc", lib_path);
    if (error_occurred()) return 0;

    intptr_t const offset = get_mprotect_offset(lib_path);
    if (offset == 0) {
        t_errno = T_ESYMBOL_NOT_FOUND;
        return 0;
    }
    DEBUG("offset: %#x", offset);

    return base_address + offset;
}


extern size_t get_function_code(char const target[const], intptr_t const start_address, uint8_t code_output[]) {
    pid_t const pid = get_pid(target);
    if (pid == -1) {
        t_errno = T_EPROC_NOT_RUNNING;
        return 0;
    }
    DEBUG("pid: %d", pid);

    uint8_t const retq_instruction = 0xC3;
    size_t i = 0;
    for (; i < MAX_CODE_LENGTH; i++) {
        uint8_t const byte = proc_read_byte(pid, start_address + i);
        if (error_occurred()) return 0;

        code_output[i] = byte;

        if (byte == retq_instruction) {
            i++;
            break;
        }
        if ((i + 1) == MAX_CODE_LENGTH) {
            t_errno = T_ERROR;
            return 0;
        }
    }
    DEBUG("Read %d bytes from %s", i, target);

    return i;
}


extern pid_t get_pid(char const process_name[const]) {
    char command[PATH_MAX] = {0};

    DEBUG("Getting PID => pgrep %s", process_name);
    int printed_characters = snprintf(command, PATH_MAX, "pgrep %s", process_name);
    if (printed_characters < 0) {
      t_errno = T_EPRINTF;
    }

    return pread_int(command);
}


static intptr_t get_symbol_offset(char const process_name[const], char const symbol[const]) {
    char command[PATH_MAX] = {0};

    DEBUG("Getting symbol offset => nm %s | grep %s", process_name, symbol);
    int const printed_characters = snprintf(command, PATH_MAX, "nm %s | grep %s", process_name, symbol);
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return 0;
    }

    return pread_word(command);
}


static intptr_t get_symbol_offset_in_lib(char const lib_path[const], char const symbol[const]) {
    char command[PATH_MAX] = {0};

    DEBUG("Getting symbol offset => objdump -d %s | grep -e \"<.*%s.*>:\"", lib_path, symbol);
    int const printed_characters = snprintf(command, PATH_MAX, "objdump -d %s | grep -e \"<.*%s.*>:\"", lib_path, symbol);
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return 0;
    }

    return pread_word(command);
}


static intptr_t get_mprotect_offset(char const lib_path[const]) {
    char command[PATH_MAX] = {0};

    DEBUG("Getting symbol offset => objdump -d %s | grep -e \"<.*mprotect.*>:\" | grep -v -e \"<.*pkey_mprotect.*>:\"", lib_path);
    int const printed_characters = snprintf(
            command,
            PATH_MAX,
            "objdump -d %s | grep -e \"<.*mprotect.*>:\" | grep -v -e \"<.*pkey_mprotect.*>:\" ",
            lib_path
    );
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return 0;
    }

    return pread_word(command);
}


static intptr_t get_lib_base_address(pid_t const pid, char const lib[const]) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.*%s", pid, lib);
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return 0;
    }
    DEBUG("Getting library %s base address => %s", lib, command);

    return pread_word(command);
}


static void get_lib_path(pid_t const pid, char const lib[const], char path[const]) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep -o -e \"/.*%s.*$\"", pid, lib);
    if (printed_characters < 0) {
        t_errno = T_EPRINTF;
        return;
    }
    DEBUG("Getting library %s path => %s", lib, command);

    pread_raw_line(command, path);
    path[strlen(path) - 1] = '\0'; // delete the newline at the end
}
