#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "process_control.h"
#include "t_error.h"


static void construct_memory_path(char const memory_path[MEMORY_PATH_MAX], int const pid);


extern void pattach(int const pid) {
  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
    t_errno = T_EPTRACE;
    return;
  }

  if (waitpid(pid, NULL, WUNTRACED) == -1) {
    t_errno = T_EWAIT;
  }
}


extern void inject_interrupt_at(int const pid, unsigned long const address) {
  char memory_path[MEMORY_PATH_MAX] = {0};
  construct_memory_path(memory_path, pid);
  if (t_errno != T_SUCCESS) return;
  
  int fd = open(memory_path, O_WRONLY);
  if (fd == -1) {
    t_errno = T_EOPEN;
    return;
  }

  if (lseek(fd, address, SEEK_SET) == -1) {
    t_errno = T_EREAD;
    return;
  }

  uint8_t interrupt = 0xCC;
  if (write(fd, &interrupt, sizeof(interrupt)) == -1) {
    t_errno = T_EWRITE;
    return;
  }

  close(fd);
  if (fd == -1) {
    t_errno = T_EOPEN;
  }
}


extern void pcontinue(int const pid) {
  if (ptrace(PTRACE_CONT, pid, 0, SIGCONT) == -1) {
    t_errno = T_EPTRACE;
    return;
  }

  int wstatus = 0;
  if (waitpid(pid, &wstatus, WUNTRACED) == -1) {
    t_errno = T_EWAIT;
  }
}


extern void pdetach(int const pid) {
  if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
    t_errno = T_EPTRACE;
  }
}


static void construct_memory_path(char const memory_path[static MEMORY_PATH_MAX], int const pid) {
  int copied_characters = snprintf(memory_path, MEMORY_PATH_MAX, "/proc/%d/mem", pid);

  if (copied_characters < 0) {
    t_errno = T_EENCODING;
  } else if (copied_characters > (MEMORY_PATH_MAX-1)) {
    t_errno = T_ESTR_TRUNC;
  }
}
