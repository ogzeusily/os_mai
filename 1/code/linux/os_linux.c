#include "os_linux.h"
#include <errno.h>
#include <stdio.h>

int os_pipe(OsPipe fds) {
    int result = pipe(fds);
    if (result == -1) {
        perror("os_pipe: pipe creation failed");
    }
    return result;
}

pid_t os_fork(void) {
    pid_t result = fork();
    if (result == -1) {
        perror("os_fork: process creation failed");
    }
    return result;
}

int os_close(int fd) {
    int result = close(fd);
    if (result == -1) {
        perror("os_close: file descriptor close failed");
    }
    return result;
}

int os_dup2(int oldfd, int newfd) {
    int result = dup2(oldfd, newfd);
    if (result == -1) {
        perror("os_dup2: file descriptor duplication failed");
    }
    return result;
}

ssize_t os_read(int fd, void *buf, size_t count) {
    ssize_t result = read(fd, buf, count);
    if (result == -1) {
        perror("os_read: read operation failed");
    }
    return result;
}

ssize_t os_write(int fd, const void *buf, size_t count) {
    ssize_t result = write(fd, buf, count);
    if (result == -1) {
        perror("os_write: write operation failed");
    }
    return result;
}

pid_t os_waitpid(pid_t pid, int *wstatus, int options) {
    pid_t result = waitpid(pid, wstatus, options);
    if (result == -1) {
        perror("os_waitpid: process wait failed");
    }
    return result;
}

int os_exec(const char *path, const char *arg0) {
    int result = execl(path, arg0, (char *)NULL);
    if (result == -1) {
        perror("os_exec: program execution failed");
    }
    return result;
}

int os_is_child_process(void) {
    return 0;
}

void os_init_child_process(void) {
}

int os_get_fork_number(void) {
    return -1;
}



