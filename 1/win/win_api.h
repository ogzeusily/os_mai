#pragma once

#include <stddef.h>
#include <stat.h>
#include <fcntl.h>
#include <io.h>
#include <basetsd.h>

typedef SSIZE_T ssize_t;
typedef int pid_t;


typedef int Pipe[2];

int create_pipe_(Pipe fds);
int dup2_(int oldfd, int newfd);
int close_(int fd);
ssize_t read_(int fd, void *buf, size_t count);
ssize_t write_(int fd, const void *buf, size_t count);

pid_t create_process_(const char *path, const char *cmdline);
pid_t wait_process_(pid_t handle, int *exitCode);
void exit_process_(int status);

int open_file_(const char *pathname, int flags);
int create_file_(const char *pathname, int flags);
int create_named_pipe_(const char *name);
int set_named_pipe_state_(const char *name);
int close_handle_(void *handle);


