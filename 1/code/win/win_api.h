#pragma once

#include <stddef.h>
#include <stat.h>
#include <fcntl.h>
#include <io.h>
#include <basetsd.h>

typedef SSIZE_T ssize_t;
typedef int pid_t;


typedef int OsPipe[2];

int os_pipe(OsPipe fds);
int os_dup2(int oldfd, int newfd);
int os_close(int fd);
ssize_t os_read(int fd, void *buf, size_t count);
ssize_t os_write(int fd, const void *buf, size_t count);

pid_t os_fork(void);
pid_t os_waitpid(pid_t handle, int *exitCode);
int os_exec(const char *path, const char *arg0);
int os_is_child_process(void);
void os_init_child_process(void);
int os_get_fork_number(void);


