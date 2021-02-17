#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "synchro.h"

typedef struct{
    uint16_t    threadNum;
    uint16_t    nbTurns;
    barrier_t*  barrier;
}runner_t;

void *runner_handler(void *num);
int print_barrier(void* run);
#endif