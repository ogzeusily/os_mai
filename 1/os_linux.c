#include "os_api.h"
#include <errno.h>

int os_pipe(OsPipe fds) {
    return pipe(fds);
}

pid_t os_fork(void) {
    return fork();
}

int os_close(int fd) {
    return close(fd);
}

int os_dup2(int oldfd, int newfd) {
    return dup2(oldfd, newfd);
}

ssize_t os_read(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t os_write(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

pid_t os_waitpid(pid_t pid, int *wstatus, int options) {
    return waitpid(pid, wstatus, options);
}

int os_exec(const char *path, const char *arg0) {
    return execl(path, arg0, (char *)NULL);
}

int os_execve(const char *filename, char *const argv[], char *const envp[]) {
    return execve(filename, argv, envp);
}

void os_exit(int status) {
    _exit(status);
}

int os_open(const char *pathname, int flags, mode_t mode) {
    return open(pathname, flags, mode);
}

int os_mkfifo(const char *pathname, mode_t mode) {
    return mkfifo(pathname, mode);
}


