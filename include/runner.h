#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "synchro.h"

typedef struct{
    uint16_t    threadNum;
    barrier_t*  barrier;
}runner_t;

void *runner_handler(void *num);
#endif