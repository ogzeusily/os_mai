#pragma once

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stddef.h>


typedef int OsPipe[2];

int os_pipe(OsPipe fds);
pid_t os_fork(void);
int os_close(int fd);
int os_dup2(int oldfd, int newfd);
ssize_t os_read(int fd, void *buf, size_t count);
ssize_t os_write(int fd, const void *buf, size_t count);
pid_t os_waitpid(pid_t pid, int *wstatus, int options);
int os_exec(const char *path, const char *arg0);
int os_execve(const char *filename, char *const argv[], char *const envp[]);
void os_exit(int status);
int os_open(const char *pathname, int flags, mode_t mode);
int os_mkfifo(const char *pathname, mode_t mode);


