#include "win_api.h"
#include <windows.h>
#include <process.h>
#include <string.h>
#include <stdio.h>

int os_pipe(OsPipe fds) {
    HANDLE readPipe, writePipe;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        fprintf(stderr, "os_pipe: pipe creation failed, error: %lu\n", GetLastError());
        return -1;
    }
    fds[0] = _open_osfhandle((intptr_t)readPipe, _O_RDONLY);
    fds[1] = _open_osfhandle((intptr_t)writePipe, 0);
    if (fds[0] == -1 || fds[1] == -1) {
        fprintf(stderr, "os_pipe: file handle conversion failed, error: %lu\n", GetLastError());
        return -1;
    }
    return 0;
}

int os_close(int fd) {
    if (!CloseHandle((HANDLE)(intptr_t)fd)) {
        fprintf(stderr, "os_close: handle close failed, error: %lu\n", GetLastError());
        return -1;
    }
    return 0;
}

int os_dup2(int oldfd, int newfd) {
    HANDLE newHandle;
    if (!DuplicateHandle(GetCurrentProcess(), (HANDLE)(intptr_t)oldfd, 
                        GetCurrentProcess(), &newHandle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        fprintf(stderr, "os_dup2: handle duplication failed, error: %lu\n", GetLastError());
        return -1;
    }
    return (int)(intptr_t)newHandle;
}

ssize_t os_read(int fd, void *buf, size_t count) {
    DWORD bytesRead;
    if (!ReadFile((HANDLE)(intptr_t)fd, buf, (DWORD)count, &bytesRead, NULL)) {
        fprintf(stderr, "os_read: read operation failed, error: %lu\n", GetLastError());
        return -1;
    }
    return (ssize_t)bytesRead;
}

ssize_t os_write(int fd, const void *buf, size_t count) {
    DWORD bytesWritten;
    if (!WriteFile((HANDLE)(intptr_t)fd, buf, (DWORD)count, &bytesWritten, NULL)) {
        fprintf(stderr, "os_write: write operation failed, error: %lu\n", GetLastError());
        return -1;
    }
    return (ssize_t)bytesWritten;
}

pid_t os_waitpid(pid_t handle, int *exitCode) {
    HANDLE h = (HANDLE)(uintptr_t)handle;
    DWORD rc = WaitForSingleObject(h, INFINITE);
    if (rc == WAIT_FAILED) {
        fprintf(stderr, "os_waitpid: wait operation failed, error: %lu\n", GetLastError());
        return -1;
    }
    DWORD code = 0;
    if (GetExitCodeProcess(h, &code)) {
        if (exitCode) *exitCode = (int)code;
        return handle;
    }
    fprintf(stderr, "os_waitpid: failed to get exit code, error: %lu\n", GetLastError());
    return -1;
}

static int fork_count = 0;
static int is_child_process = 0;
static HANDLE child_handles[10];
static int child_count = 0;

pid_t os_fork(void) {
    char exe_path[MAX_PATH];
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) == 0) {
        fprintf(stderr, "os_fork: failed to get executable path, error: %lu\n", GetLastError());
        return -1;
    }
    
    char cmdline[MAX_PATH + 50];
    snprintf(cmdline, sizeof(cmdline), "%s --child --fork=%d", exe_path, fork_count);
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    if (!CreateProcessA(exe_path, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "os_fork: process creation failed, error: %lu\n", GetLastError());
        return -1;
    }
    if (child_count < 10) {
        child_handles[child_count] = pi.hProcess;
        child_count++;
    }
    
    CloseHandle(pi.hThread);
    
    fork_count++;
    
    return (pid_t)(intptr_t)pi.hProcess;
}

int os_exec(const char *path, const char *arg0) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    char cmdline[1024];
    snprintf(cmdline, sizeof(cmdline), "%s %s", path, arg0);
    
    if (!CreateProcessA(path, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "os_exec: process execution failed, error: %lu\n", GetLastError());
        return -1;
    }
    
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}

int os_is_child_process(void) {
    return is_child_process;
}

void os_init_child_process(void) {
    // Check command line arguments to see if we're a child process
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv != NULL) {
        for (int i = 1; i < argc; i++) {
            if (wcscmp(argv[i], L"--child") == 0) {
                is_child_process = 1;
                break;
            }
        }
        LocalFree(argv);
    }
}

int os_get_fork_number(void) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv != NULL) {
        for (int i = 1; i < argc; i++) {
            if (wcsncmp(argv[i], L"--fork=", 7) == 0) {
                int fork_num = _wtoi(argv[i] + 7);
                LocalFree(argv);
                return fork_num;
            }
        }
        LocalFree(argv);
    }
    return -1;
}

