#include "os_linux.h"
#include <errno.h>

int pipe_(OsPipe fds) {
    return pipe(fds);
}

pid_t fork_(void) {
    return fork();
}

int close_(int fd) {
    return close(fd);
}

int dup2_(int oldfd, int newfd) {
    return dup2(oldfd, newfd);
}

ssize_t read_(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t write_(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

pid_t waitpid_(pid_t pid, int *wstatus, int options) {
    return waitpid(pid, wstatus, options);
}

int exec_(const char *path, const char *arg0) {
    return execl(path, arg0, (char *)NULL);
}

int execve_(const char *filename, char *const argv[], char *const envp[]) {
    return execve(filename, argv, envp);
}

int open_(const char *pathname, int flags, mode_t mode) {
    return open(pathname, flags, mode);
}

int mkfifo_(const char *pathname, mode_t mode) {
    return mkfifo(pathname, mode);
}


