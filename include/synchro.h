#ifndef SYNCHRO_THREAD_INCLUDED
#define SYNCHRO_THREAD_INCLUDED
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>

typedef struct{
    pthread_mutex_t mutex;          //mutex used internally to protect the counters manipulations
    sem_t           turnstile1;     //first semaphore used during the synchronisation
    sem_t           turnstile2;     //second semaphore used during the synchronisation
    uint16_t        th_count;       //Amount of threads currently waiting in the barrier sas
    uint16_t        th_nb;          //Total amount of threads to synchronise in the barrier
}barrier_t;

int barrier_alloc(barrier_t* bar, const uint16_t nb);
int barrier_free(barrier_t* bar);
int barrier_sync(barrier_t* bar);

#endif