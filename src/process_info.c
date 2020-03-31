#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "t_error.h"
#include "pread.h"
#include "log.h"
#include "procmem.h"

#include "process_info.h"

static bool file_exists(char const * filename);


extern intptr_t get_process_base_address(pid_t const pid) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.*`pgrep -n %d`", pid, pid);
    if (printed_characters < 0) {
        RAISE(T_EPRINTF, "snprintf failed");
        return 0;
    }

    intptr_t const base_address = pread_word(command);
    if (error_occurred()) return 0;
    DEBUG("Process %d base address: %#lx", pid, base_address);

    return base_address;
}


extern intptr_t get_symbol_offset_in_binary(char const binary_path[const], char const symbol[const], bool const shared_object) {
    char command[PATH_MAX] = {0};

    if (file_exists(binary_path) == false) {
        RAISE(T_EPATH_INVALID, "%s", binary_path);
        return 0;
    }

    int printed_characters = 0;
    if (shared_object) {
        printed_characters = snprintf(command, PATH_MAX, "nm -D %s | grep -w %s", binary_path, symbol);
    } else {
        printed_characters = snprintf(command, PATH_MAX, "nm %s | grep -w %s", binary_path, symbol);
    }

    if (printed_characters < 0) {
        RAISE(T_EPRINTF, "snprintf failed");
        return 0;
    }

    intptr_t const offset = pread_word(command);
    if (error_occurred()) {
        RAISE(T_ESYMBOL_NOT_FOUND, "%s", symbol);
        return 0;
    }

    return offset;
}


extern intptr_t locate_libc_in(pid_t const pid) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "cat /proc/%d/maps | grep r.*%s", pid, LIBC_NAME);
    if (printed_characters < 0) {
        RAISE(T_EPRINTF, "snprintf failed");
        return 0;
    }

    intptr_t const libc_address = pread_word(command);
    if (error_occurred()) return 0;

    return libc_address;
}


extern void get_libc_path(char const binary_path[const], char path[const]) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "ldd %s | grep -o -e \"/.*%s.* \"", binary_path, LIBC_NAME);
    if (printed_characters < 0) {
        RAISE(T_EPRINTF, "snprintf failed");
        return;
    }

    pread_raw_line(command, path);
    path[strlen(path) - 1] = '\0'; // remove empty character in the end
}


extern size_t get_function_code(pid_t const pid, intptr_t const start_address, uint8_t code_output[]) {
    uint8_t const retq_instruction = 0xC3;
    size_t i = 0;
    for (; i < MAX_CODE_LENGTH; i++) {
        uint8_t const byte = proc_read_byte(pid, start_address + (intptr_t) i);
        if (error_occurred()) return 0;

        code_output[i] = byte;

        if (byte == retq_instruction) {
            i++;
            break;
        }
        if ((i + 1) == MAX_CODE_LENGTH) {
            RAISE(T_EFUNC_TOO_BIG, "function at %#lx", start_address);
            return 0;
        }
    }
    DEBUG("Read %lu bytes from %d", i, pid);

    return i;
}


extern pid_t get_pid(char const process_name[const]) {
    char command[PATH_MAX] = {0};

    int printed_characters = snprintf(command, PATH_MAX, "pgrep %s", process_name);
    if (printed_characters < 0) {
        RAISE(T_EPRINTF, "snprintf failed");
    }

    pid_t pid =  pread_int(command);
    if (error_occurred()) {
        RAISE(T_EPROC_NOT_RUNNING, "%s", process_name);
    }

    return pid;
}


static bool file_exists(char const * const filename){
    struct stat file;
    int res = stat(filename, &file);
    if (res == -1) {
        return false;
    }

    return true;
}
