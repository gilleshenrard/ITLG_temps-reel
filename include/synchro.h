#ifndef SYNCHRO_THREAD_INCLUDED
#define SYNCHRO_THREAD_INCLUDED
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct{
    pthread_mutex_t mutex;          //mutex used internally to protect the counters manipulations
    sem_t           turnstile1;     //first semaphore used during the synchronisation
    sem_t           turnstile2;     //second semaphore used during the synchronisation
    uint16_t        th_count;       //Amount of threads currently waiting in the barrier sas
    uint16_t        th_nb;          //Total amount of threads to synchronise in the barrier
}barrier_t;

typedef struct{
    pthread_mutex_t mutex;          //mutex used internally to protect the counters manipulations
    sem_t           items;          //semaphore counting how many items are available the FIFO queue to pop
    sem_t           spaces;         //semaphore counting the remaining free slots in the FIFO queue 
    uint16_t        nb_items;       //size of the buffer (amount of slots)
    uint16_t        item_sz;        //Size of an item
    uint16_t        indexIn;        //index of the next slot in which an item can be pushed
    uint16_t        indexOut;       //index of the next slot from which an item can be popped
    void*           buffer;         //address of the actual buffer
}fifo_t;

//barrier synchronisation functions
int barrier_alloc(barrier_t** bar, const uint16_t nb);
int barrier_free(barrier_t* bar);
int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);

//FIFO synchronisation functions
int fifo_alloc(fifo_t** fifo, const uint16_t elemsz, const uint16_t amount);
int fifo_free(fifo_t* fifo);
int fifo_push(fifo_t* fifo, void* elem);
void* fifo_pop(fifo_t* fifo);
#endif