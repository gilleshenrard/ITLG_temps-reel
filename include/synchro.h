#ifndef SYNCHRO_THREAD_INCLUDED
#define SYNCHRO_THREAD_INCLUDED
#include <inttypes.h>
#include <semaphore.h>

typedef struct{
    pthread_mutex_t mutex;          //mutex used internally to protect the counters manipulations
    sem_t           turnstile1;     //first semaphore used during the synchronisation
    sem_t           turnstile2;     //second semaphore used during the synchronisation
    uint16_t        th_count;       //Amount of threads currently waiting in the barrier sas
    uint16_t        th_nb;          //Total amount of threads to synchronise in the barrier
}barrier_t;

typedef struct{
    pthread_mutex_t mutex;          //mutex used internally to protect the counters manipulations
    pthread_cond_t  cond_notfull;   //conditional variable indicating the FIFO is not full anymore
    pthread_cond_t  cond_notempty;  //conditional variable indicating the FIFO is not empty anymore
    uint16_t        items;          //counter of items available the FIFO queue to pop
    uint16_t        spaces;         //counter of remaining free slots in the FIFO queue 
    uint16_t        amount;       //size of the buffer (amount of slots)
    uint16_t        item_sz;        //Size of an item
    uint16_t        indexIn;        //index of the next slot in which an item can be pushed
    uint16_t        indexOut;       //index of the next slot from which an item can be popped
    void*           buffer;         //address of the actual buffer
}fifo_t;

typedef struct{
    uint16_t        counter;        //counter to the amount of threads in the lightswitch
    pthread_mutex_t mutex;          //mutex allowing synchronisation of the lightswitch
}lightswitch_t;

typedef struct{
    lightswitch_t   readSwitch;     //lightswitch used to synchronise the readers (first locks, last unlocks)
    lightswitch_t   writeSwitch;    //lightswitch used to synchronise the writers (first locks, last unlocks)
    sem_t           noReaders;      //semaphore used to exclude readers
    sem_t           noWriters;      //semaphore used to exclude writers
}readwrite_t;

//barrier synchronisation functions
int barrier_alloc(barrier_t** bar, const uint16_t nb);
int barrier_free(barrier_t* bar);
int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);

//FIFO synchronisation functions
int fifo_alloc(fifo_t** fifo, const uint16_t elemsz, const uint16_t amount);
int fifo_free(fifo_t* fifo);
int fifo_push(fifo_t* fifo, void* elem);
void* fifo_pop(fifo_t* fifo);

//readers-writers synchronisation functions
int lightswitch_lock(lightswitch_t* light, sem_t* sem);
int lightswitch_unlock(lightswitch_t* light, sem_t* sem);
int rw_alloc(readwrite_t** rw);
int rw_free(readwrite_t* rw);
int rw_read(readwrite_t* rw, int (doAction)(void*), void* action_arg);
int rw_write(readwrite_t* rw, int (doAction)(void*), void* action_arg);
#endif