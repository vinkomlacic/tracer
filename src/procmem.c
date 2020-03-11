#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "t_error.h"
#include "log.h"
#include "procmem.h"


static void construct_memory_path(char memory_path[static MEMORY_PATH_MAX], pid_t pid);


extern uint8_t proc_read_byte(pid_t const pid, intptr_t const address) {
    char memory_path[MEMORY_PATH_MAX] = {0};
    construct_memory_path(memory_path, pid);
    TRACE("Memory path constructed: %s", memory_path);
    if (error_occurred()) {
        return 0;
    }

    int const fd = open(memory_path, O_RDONLY);
    if (fd == -1) {
        t_errno = T_EOPEN;
        return 0;
    }
    TRACE("Opening process file success");


    if (lseek(fd, address, SEEK_SET) == -1) {
        t_errno = T_EREAD;
        return 0;
    }

    uint8_t result = 0;
    if (read(fd, &result, sizeof(result)) == -1) {
        t_errno = T_EREAD;
        return 0;
    }

    if (close(fd) == -1) {
        t_errno = T_ECLOSE;
        return 0;
    }
    TRACE("Read %#x from process %d at %#lx", result, pid, address);

    return result;
}


extern void proc_write_byte(pid_t const pid, intptr_t const address, uint8_t const value) {
    char memory_path[MEMORY_PATH_MAX] = {0};
    construct_memory_path(memory_path, pid);
    TRACE("Memory path constructed: %s", memory_path);
    if (error_occurred()) {
        return;
    }

    int const fd = open(memory_path, O_WRONLY);
    if (fd == -1) {
        t_errno = T_EOPEN;
        return;
    }
    TRACE("Opening process file success");

    if (lseek(fd, address, SEEK_SET) == -1) {
        t_errno = T_EWRITE;
        return;
    }

    if (write(fd, &value, sizeof(value)) == -1) {
        t_errno = T_EWRITE;
        return;
    }

    if (close(fd) == -1) {
        t_errno = T_ECLOSE;
    }
    TRACE("Successfully written %#x to %d at %#lx", value, pid, address);
}


static void construct_memory_path(char memory_path[const static MEMORY_PATH_MAX], pid_t const pid) {
    int const copied_characters = snprintf(memory_path, MEMORY_PATH_MAX, MEMORY_PATH_FORMAT, pid);

    if (copied_characters < 0) {
        t_errno = T_EENCODING;
    } else if (copied_characters > (MEMORY_PATH_MAX-1)) {
        t_errno = T_ESTR_TRUNC;
    }
}
