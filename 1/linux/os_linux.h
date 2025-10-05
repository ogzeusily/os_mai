#pragma once

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stddef.h>




typedef int OsPipe[2];

int pipe_(OsPipe fds);
pid_t fork_(void);
int close_(int fd);
int dup2_(int oldfd, int newfd);
ssize_t read_(int fd, void *buf, size_t count);
ssize_t write_(int fd, const void *buf, size_t count);
pid_t waitpid_(pid_t pid, int *wstatus, int options);
int exec_(const char *path, const char *arg0);
int execve_(const char *filename, char *const argv[], char *const envp[]);
int open_(const char *pathname, int flags, mode_t mode);
int mkfifo_(const char *pathname, mode_t mode);
