#ifdef WIN_32
#include "win_api.h"
#include <dows.h>
#include <process.h>
#include <string.h>

int create_pipe_(Pipe fds) {
    HANDLE readPipe, writePipe;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        return -1;
    }
    fds[0] = _open_osfhandle((intptr_t)readPipe, _O_RDONLY);
    fds[1] = _open_osfhandle((intptr_t)writePipe, 0);
    return (fds[0] == -1 || fds[1] == -1) ? -1 : 0;
}

int close_(int fd) {
    return CloseHandle(fd);
}

int dup2_(int oldfd, int newfd) {
    return DuplicateHandle(oldfd, newfd);
}

ssize_t read_(int fd, void *buf, size_t count) {
    return ReadFile(fd, buf, (unsigned int)count);
}

ssize_t write_(int fd, const void *buf, size_t count) {
    return WriteFile(fd, buf, (unsigned int)count);
}

pid_t wait_process_(pid_t handle, int *exitCode) {
    HANDLE h = (HANDLE)(uintptr_t)handle;
    DWORD rc = WaitForSingleObject(h, INFINITE);
    if (rc == WAIT_FAILED) return -1;
    DWORD code = 0;
    if (GetExitCodeProcess(h, &code)) {
        if (exitCode) *exitCode = (int)code;
        return handle;
    }
    return -1;
}

pid_t create_process_(const char *path, const char *cmdline) {
    STARTUPINFOA si; PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    LPSTR mutableCmd = NULL;
    if (cmdline && *cmdline) {
        size_t n = strlen(cmdline) + 1;
        mutableCmd = (LPSTR)malloc(n);
        if (!mutableCmd) return -1;
        memcpy(mutableCmd, cmdline, n);
    }
    if (!CreateProcessA(path, mutableCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        if (mutableCmd) free(mutableCmd);
        return -1;
    }
    if (mutableCmd) free(mutableCmd);
    CloseHandle(pi.hThread);
    return (int)(uintptr_t)pi.hProcess;
}

void exit_process_(int status) {
    ExitProcess((UINT)status);
}

int open_file_(const char *pathname, int flags) {
    return CreateFileA(pathname, flags, 0, NULL, OPEN_EXISTING, 0, NULL);
}

int create_named_pipe_(const char *name) {
    HANDLE h = CreateNamedPipeA(name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 4096, 4096, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return -1;
    CloseHandle(h);
    return 0;
}

int set_named_pipe_state_(const char *name) {
    HANDLE h = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return -1;
    DWORD mode = PIPE_READMODE_BYTE | PIPE_WAIT;
    BOOL ok = SetNamedPipeHandleState(h, &mode, NULL, NULL);
    CloseHandle(h);
    return ok ? 0 : -1;
}

int close_handle_(void *handle) {
    return CloseHandle((HANDLE)handle) ? 0 : -1;
}
#endif


