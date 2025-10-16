#include "os_linux.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

ThreadHandle thread_handle_init() {
    pthread_t* handle = (pthread_t*)malloc(sizeof(pthread_t));
    return (ThreadHandle)handle;
}

void thread_handle_free(ThreadHandle handle) {
    free(handle);
}

int create_thread(ThreadHandle* thread, ThreadRoutine routine, void* arg) {
    if (!thread) return -1;
    
    ThreadHandle handle = thread_handle_init();
    if (!handle) return -1;
    
    int result = pthread_create(&THREAD_HANDLE_TO_PTHREAD(handle), NULL, routine, arg);
    if (result != 0) {
        thread_handle_free(handle);
        return -1;
    }
    
    *thread = handle;
    return 0;
}

int join_thread(ThreadHandle thread) {
    if (!thread) return -1;
    
    int result = pthread_join(THREAD_HANDLE_TO_PTHREAD(thread), NULL);
    thread_handle_free(thread);
    return result;
}

int init_mutex(void** mutex) {
    if (!mutex) return -1;
    
    pthread_mutex_t* mtx = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!mtx) return -1;
    
    int result = pthread_mutex_init(mtx, NULL);
    if (result != 0) {
        free(mtx);
        return -1;
    }
    
    *mutex = mtx;
    return 0;
}

int lock_mutex(void* mutex) {
    if (!mutex) return -1;
    return pthread_mutex_lock((pthread_mutex_t*)mutex);
}

int unlock_mutex(void* mutex) {
    if (!mutex) return -1;
    return pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

int destroy_mutex(void* mutex) {
    if (!mutex) return -1;
    
    int result = pthread_mutex_destroy((pthread_mutex_t*)mutex);
    free(mutex);
    return result;
}

int get_processor_count() {
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}
