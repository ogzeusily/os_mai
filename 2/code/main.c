#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "os.h"


int roll_dice() {
    return (rand() % 6 + 1) + (rand() % 6 + 1);
}

void* igra(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    GameState* state = data->game_state;
    
    int local_p1_score = state->player1_score;
    int local_p2_score = state->player2_score;
    
    for (int i = state->current_round; i < state->rounds_total; i++) {
        local_p1_score += roll_dice();
        local_p2_score += roll_dice();
    }
    
    data->results[data->experiment_id] = (local_p1_score > local_p2_score) ? 1.0 : 
                                       (local_p1_score < local_p2_score) ? 2.0 : 1.5;
    
    return NULL;
}

double sequential_version(GameState* state, double* results) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < state->experiments_count; i++) {
        ThreadData data = {
            .experiment_id = i,
            .game_state = state,
            .results = results
        };
        igra(&data);
    }
    
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}

double parallel_version(GameState* state, double* results, int num_threads) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    ThreadHandle* threads = (ThreadHandle*)malloc(num_threads * sizeof(ThreadHandle));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    
    if (!threads || !thread_data) {
        if (threads) free(threads);
        if (thread_data) free(thread_data);
        return -1;
    }
    
    for (int i = 0; i < state->experiments_count; i += num_threads) {
        int current_threads = (i + num_threads <= state->experiments_count) ? 
                            num_threads : (state->experiments_count - i);
        
        for (int j = 0; j < current_threads; j++) {
            thread_data[j].experiment_id = i + j;
            thread_data[j].game_state = state;
            thread_data[j].results = results;
            
            if (create_thread(&threads[j], igra, &thread_data[j]) != 0) {
                for (int k = 0; k < j; k++) {
                    join_thread(threads[k]);
                }
                free(threads);
                free(thread_data);
                return -1;
            }
        }
        
        for (int j = 0; j < current_threads; j++) {
            join_thread(threads[j]);
        }
    }
    
    free(threads);
    free(thread_data);
    
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}

// zakon amdala: Sp = 1 / ((1-p) + p/N)
double calculate_amdahl_speedup(double parallel_fraction, int processors) {
    if (parallel_fraction <= 0 || processors <= 0) return 1.0;
    return 1.0 / ((1.0 - parallel_fraction) + parallel_fraction / processors);
}

// zakon gustavsona-barsisa: Sp = N + (1-N)*s
double calculate_gustafson_speedup(int processors, double sequential_fraction) {
    return processors + (1 - processors) * sequential_fraction;
}

// modelirivanie mnogozadachnosti CP = 1 - p^n
double calculate_cpu_utilization(double io_wait_probability, int processes) {
    return 1.0 - pow(io_wait_probability, processes);
}

void calculate_all_metrics(Metrics* metrics, double seq_time, double par_time, 
                          int threads, int total_processors, double parallel_fraction_estimate) {
    metrics->sequential_time = seq_time;
    metrics->parallel_time = par_time;
    metrics->thread_count = threads;
    metrics->processor_count = total_processors;
    if (par_time > 0) {
        metrics->speedup = seq_time / par_time;
        metrics->efficiency = metrics->speedup / threads;
    } else {
        metrics->speedup = 0;
        metrics->efficiency = 0;
    }
    
    metrics->amdahl_speedup = calculate_amdahl_speedup(parallel_fraction_estimate, threads);
    
    double sequential_fraction = 1.0 - parallel_fraction_estimate;
    metrics->gustafson_speedup = calculate_gustafson_speedup(threads, sequential_fraction);
    
    double io_wait_probability = 0.1;  
    metrics->io_wait_estimation = io_wait_probability;
    metrics->cpu_utilization = calculate_cpu_utilization(io_wait_probability, threads);
}

void print_detailed_metrics(const Metrics* metrics) {
    printf("T1:          %.2f ms\n", metrics->sequential_time);
    printf("T%d:          %.2f ms\n", metrics->thread_count, metrics->parallel_time);
    printf("Speedup:     %.2f\n", metrics->speedup);
    printf("Efficiency:  %.2f%%\n", metrics->efficiency * 100);
    printf("Amdahl:      %.2f\n", metrics->amdahl_speedup);
    printf("Gustafson:   %.2f\n", metrics->gustafson_speedup);
    printf("CPU util:    %.2f%%\n", metrics->cpu_utilization * 100);
    
}

double estimate_parallel_fraction(double seq_time, double par_time, int threads) {
    if (seq_time <= 0 || par_time <= 0) return 0.9;
    double observed_speedup = seq_time / par_time;
    if (observed_speedup >= threads) return 1.0;
    double p = (1.0/observed_speedup - 1.0) / (1.0/threads - 1.0);
    return fmax(0.1, fmin(0.99, p));
}

int main(int argc, char* argv[]) {
    if (argc < 6 || argc > 7) {
        printf("Использование: %s <раунды> <текущий_раунд> <очки_игрок1> <очки_игрок2> <эксперименты> [потоки]\n", argv[0]);
        return 1;
    }
    
    srand(time(NULL));
    
    int num_threads = (argc == 7) ? atoi(argv[6]) : get_processor_count();
    if (num_threads <= 0) {
        num_threads = get_processor_count();
    }

    GameState state = {
        .rounds_total = atoi(argv[1]),
        .current_round = atoi(argv[2]),
        .player1_score = atoi(argv[3]),
        .player2_score = atoi(argv[4]),
        .experiments_count = atoi(argv[5]),
        .max_threads = num_threads
    };
    
    if (state.rounds_total <= 0 || state.current_round < 0 || 
        state.current_round > state.rounds_total || state.experiments_count <= 0) {
        return 1;
    }
    
    
    double* results_seq = (double*)malloc(state.experiments_count * sizeof(double));
    double* results_par = (double*)malloc(state.experiments_count * sizeof(double));
    
    if (!results_seq || !results_par) {
        free(results_seq);
        free(results_par);
        return 1;
    }
    
    Metrics metrics;
    
    printf("sequential...\n");
    double seq_time = sequential_version(&state, results_seq);
    
    printf("parallel...\n");
    double par_time = parallel_version(&state, results_par, num_threads);
    
    if (par_time < 0) {
        free(results_seq);
        free(results_par);
        return 1;
    }
    
    double parallel_fraction = estimate_parallel_fraction(seq_time, par_time, num_threads);
    
    calculate_all_metrics(&metrics, seq_time, par_time, num_threads, 
                         get_processor_count(), parallel_fraction);
    
    print_detailed_metrics(&metrics);
    
    int wins_p1 = 0, wins_p2 = 0, draws = 0;
    for (int i = 0; i < state.experiments_count; i++) {
        if (results_par[i] == 1.0) wins_p1++;
        else if (results_par[i] == 2.0) wins_p2++;
        else draws++;
    }
    
    printf("SHANCY 1 IGROKA: %.2f%%\n", (double)wins_p1 / state.experiments_count * 100);
    printf("SHANCY 2 IGROKA: %.2f%%\n", (double)wins_p2 / state.experiments_count * 100);
    printf("SHANCY NICHI:    %.2f%%\n", (double)draws / state.experiments_count * 100);
    
    free(results_seq);
    free(results_par);
    
    return 0;
}