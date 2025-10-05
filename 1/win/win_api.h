#pragma once

#include <stddef.h>
#include <stat.h>
#include <fcntl.h>
#include <io.h>
#include <basetsd.h>

typedef SSIZE_T ssize_t;
typedef int pid_t;


typedef int Pipe[2];

int _create_pipe(Pipe fds);
int _dup2(int oldfd, int newfd);
int _close(int fd);
ssize_t _read(int fd, void *buf, size_t count);
ssize_t _write(int fd, const void *buf, size_t count);

pid_t _create_process(const char *path, const char *cmdline);
pid_t _wait_process(pid_t handle, int *exitCode);
void _exit_process(int status);

int _open_file(const char *pathname, int flags);
int _create_file(const char *pathname, int flags);
int _create_named_pipe(const char *name);
int _set_named_pipe_state(const char *name);
int _close_handle(void *handle);


