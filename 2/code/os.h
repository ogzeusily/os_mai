#ifndef OS_H
#define OS_H

#include <stddef.h>

typedef struct {
    int current_round;     
    int player1_score;     
    int player2_score;     
    int rounds_total;      
    int experiments_count; 
    int max_threads;       
} GameState;

typedef struct {
    int experiment_id;    
    GameState* game_state;
    double* results;      
} ThreadData;

typedef struct {
    double sequential_time;
    double parallel_time;
    double speedup;
    double efficiency;
    double amdahl_speedup;
    double gustafson_speedup;
    double cpu_utilization;
    double io_wait_estimation;
    int thread_count;
    int processor_count;
} Metrics;


typedef void* ThreadHandle;
typedef void* ThreadRoutine;

int create_thread(ThreadHandle* thread, ThreadRoutine routine, void* arg);

int join_thread(ThreadHandle thread);

int init_mutex(void** mutex);

int lock_mutex(void* mutex);

int unlock_mutex(void* mutex);

int destroy_mutex(void* mutex);

int get_processor_count();

#endif
