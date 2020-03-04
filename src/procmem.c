#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "procmem.h"
#include "t_error.h"


static void construct_memory_path(char memory_path[static MEMORY_PATH_MAX], pid_t pid);


extern uint8_t proc_read_byte(pid_t const pid, intptr_t const address) {
    char memory_path[MEMORY_PATH_MAX] = {0};
    construct_memory_path(memory_path, pid);
    if (error_occurred()) {
        return 0;
    }

    int const fd = open(memory_path, O_RDONLY);
    if (fd == -1) {
        t_errno = T_EOPEN;
        return 0;
    }


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

    return result;
}


extern void proc_write_byte(pid_t const pid, intptr_t const address, uint8_t const value) {
    char memory_path[MEMORY_PATH_MAX] = {0};
    construct_memory_path(memory_path, pid);
    if (error_occurred()) {
        return;
    }

    int const fd = open(memory_path, O_WRONLY);
    if (fd == -1) {
        t_errno = T_EOPEN;
        return;
    }

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
}


static void construct_memory_path(char memory_path[static MEMORY_PATH_MAX], pid_t const pid) {
    int copied_characters = snprintf(memory_path, MEMORY_PATH_MAX, MEMORY_PATH_FORMAT, pid);

    if (copied_characters < 0) {
        t_errno = T_EENCODING;
    } else if (copied_characters > (MEMORY_PATH_MAX-1)) {
        t_errno = T_ESTR_TRUNC;
    }
}
