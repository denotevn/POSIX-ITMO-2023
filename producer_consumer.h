#pragma once
# include <pthread.h>

typedef struct producer_args_tag {
    int *number_ptr;
    int n;
    bool *completed;
    bool *consumer_started;
    bool *new_number;
} producer_args;

typedef struct consumer_args_tag {
    int *number_ptr;
    unsigned short int ms;
    bool debug;
    int psum;
    int tid;
    bool *completed;
    bool *consumer_started;
    bool *new_number;
} consumer_args;

typedef struct interrupter_args_tag {
    pthread_t *consumers;
    int n;
    bool *completed;
    bool *consumer_started;
} interrupter_args;

// the declaration of run threads can be changed as you like
int run_threads(int N, int ms, bool debug);
void* producer_routine(void* arg);
void* consumer_routine(void* arg);
void* consumer_interrupter_routine(void* arg); 
int get_tid(int id=0);