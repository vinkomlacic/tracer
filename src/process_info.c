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


static intptr_t get_symbol_offset(pid_t pid, char const symbol[]);
static intptr_t get_symbol_offset_in_lib(char const lib_path[], char const symbol[]);
static intptr_t get_mprotect_offset(char const lib_path[]);
static intptr_t get_libc_base_address(pid_t pid);
static intptr_t get_process_base_address(pid_t pid);
static void get_libc_path(pid_t pid, char path[]);


extern intptr_t get_symbol_address_in_target(pid_t const pid, char const symbol[const]) {
    intptr_t const base_address = get_process_base_address(pid);
    DEBUG("base_address: %#lx", base_address);

    intptr_t const offset = get_symbol_offset(pid, symbol);
    if (offset == 0) {
        raise(T_ESYMBOL_NOT_FOUND, "symbol: %s", symbol);
        return 0;
    }
    DEBUG("offset: %#x", offset);

    return base_address + offset;
}


extern intptr_t get_symbol_address_in_libc(pid_t const pid, const char *symbol) {
    intptr_t const base_address = get_libc_base_address(pid);
    DEBUG("base_address: %#lx", base_address);

    char lib_path[PATH_MAX] = {0};
    get_libc_path(pid, lib_path);
    if (error_occurred()) return 0;

    intptr_t const offset = get_symbol_offset_in_lib(lib_path, symbol);
    if (offset == 0) {
        raise(T_ESYMBOL_NOT_FOUND, "symbol: %s", symbol);
        return 0;
    }
    DEBUG("offset: %#x", offset);

    return base_address + offset;
}


extern intptr_t get_mprotect_address(pid_t const pid) {
    intptr_t const base_address = get_libc_base_address(pid);
    DEBUG("base_address: %#lx", base_address);

    char lib_path[PATH_MAX] = {0};
    get_libc_path(pid, lib_path);
    if (error_occurred()) return 0;

    intptr_t const offset = get_mprotect_offset(lib_path);
    if (offset == 0) {
        raise(T_ESYMBOL_NOT_FOUND, "symbol: mprotect");
        return 0;
    }
    DEBUG("offset: %#x", offset);

    return base_address + offset;
}


extern size_t get_function_code(pid_t const pid, intptr_t const start_address, uint8_t code_output[]) {
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
            raise(T_EFUNC_TOO_BIG, "function at %#lx", start_address);
            return 0;
        }
    }
    DEBUG("Read %d bytes from %d", i, pid);

    return i;
}


extern pid_t get_pid(char const process_name[const]) {
    char command[PATH_MAX] = {0};

    DEBUG("Getting PID => pgrep %s", process_name);
    int printed_characters = snprintf(command, PATH_MAX, "pgrep %s", process_name);
    if (printed_characters < 0) {
        raise(T_EPRINTF, "snprintf failed");
    }

    pid_t pid =  pread_int(command);
    if (error_occurred()) {
        raise(T_EPROC_NOT_RUNNING, "%s", process_name);
    }

    return pid;
}


static intptr_t get_symbol_offset(pid_t const pid, char const symbol[const]) {
    char command[PATH_MAX] = {0};

    int const printed_characters = snprintf(command, PATH_MAX, "nm `cat /proc/%d/comm` | grep %s", pid, symbol);
    if (printed_characters < 0) {
        raise(T_EPRINTF, "snprintf failed");
        return 0;
    }
    DEBUG("Getting symbol offset => %s", command);

    return pread_word(command);
}


static intptr_t get_symbol_offset_in_lib(char const lib_path[const], char const symbol[const]) {
    char command[PATH_MAX] = {0};

    DEBUG("Getting symbol offset => objdump -d %s | grep -e \"<.*%s.*>:\"", lib_path, symbol);
    int const printed_characters = snprintf(command, PATH_MAX, "objdump -d %s | grep -e \"<.*%s.*>:\"", lib_path, symbol);
    if (printed_characters < 0) {
        raise(T_EPRINTF, "snprintf failed");
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
        raise(T_EPRINTF, "snprintf failed");
        return 0;
    }

    return pread_word(command);
}


static intptr_t get_libc_base_address(pid_t const pid) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.*%s", pid, LIBC_NAME);
    if (printed_characters < 0) {
        raise(T_EPRINTF, "snprintf failed");
        return 0;
    }
    DEBUG("Getting libc base address => %s", command);

    return pread_word(command);
}


static intptr_t get_process_base_address(pid_t const pid) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.*`pgrep -n %d`", pid, pid);
    if (printed_characters < 0) {
        raise(T_EPRINTF, "snprintf failed");
        return 0;
    }
    DEBUG("Getting process base address => %s", command);

    return pread_word(command);
}


static void get_libc_path(pid_t const pid, char path[const]) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep -o -e \"/.*%s.*$\"", pid, LIBC_NAME);
    if (printed_characters < 0) {
        raise(T_EPRINTF, "snprintf failed");
        return;
    }
    DEBUG("Getting libc path => %s", command);

    pread_raw_line(command, path);
    path[strlen(path) - 1] = '\0'; // deletes the newline at the end
}
