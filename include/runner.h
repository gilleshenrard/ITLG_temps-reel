#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include "synchro.h"

typedef struct{
    uint16_t    threadNum;              //ID of the thread
    uint16_t    nbTurns;                //amount of laps to run
    barrier_t*  barrier;                //barrier used to sync all the runners
    void        (*onPrint)(char*, ...); //function used to print info on screen
}runner_t;

void *runner_handler(void *num);
int print_barrier(void* run);
#endif