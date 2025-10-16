#pragma once

#include "../os.h"
#include <pthread.h>

#define THREAD_HANDLE_TO_PTHREAD(handle) (*((pthread_t*)handle))

ThreadHandle thread_handle_init();

void thread_handle_free(ThreadHandle handle);

