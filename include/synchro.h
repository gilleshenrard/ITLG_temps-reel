#ifndef SYNCHRO_THREAD_INCLUDED
#define SYNCHRO_THREAD_INCLUDED
#include <inttypes.h>
#include <semaphore.h>

typedef struct{
    pthread_mutex_t mutex;              //mutex used internally to protect the counters manipulations
    pthread_cond_t  cond_turnstile1;    //conditional variable indicating the turnstile 1 has been reached
    pthread_cond_t  cond_turnstile2;    //conditional variable indicating the turnstile 2 has been reached
    uint8_t         flipflop;           //variable managing turnstiles status (0 = 1 open, 2 closed ; 1 = 1 closed, 2 open)
    uint16_t        th_count;           //Amount of threads currently waiting in the barrier sas
    uint16_t        th_nb;              //Total amount of threads to synchronise in the barrier
}barrier_t;

typedef struct{
    pthread_mutex_t mutex;          //mutex used internally to protect the counters manipulations
    pthread_cond_t  cond_notfull;   //conditional variable indicating the FIFO is not full anymore
    pthread_cond_t  cond_notempty;  //conditional variable indicating the FIFO is not empty anymore
    uint16_t        items;          //counter of items available the FIFO queue to pop
    uint16_t        spaces;         //counter of remaining free slots in the FIFO queue 
    uint16_t        amount;         //size of the buffer (amount of slots)
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
    pthread_cond_t  cond_noReaders; //condition variable used to exclude readers
    pthread_cond_t  cond_noWriters; //condition variable used to exclude writers
    uint8_t         noReaders;      //flag indicating if the noReaders condition has been met
    uint8_t         noWriters;      //flag indicating if the noWriters condition has been met
}readwrite_pr_t;

//barrier synchronisation functions
barrier_t* barrier_alloc(const uint16_t nb);
int barrier_free(barrier_t* bar);
int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);

//FIFO synchronisation functions
fifo_t* fifo_alloc(const uint16_t elemsz, const uint16_t amount);
int fifo_free(fifo_t* fifo);
int fifo_push(fifo_t* fifo, void* elem);
void* fifo_pop(fifo_t* fifo);

//readers-writers synchronisation functions
int lightswitch_lock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag);
int lightswitch_unlock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag);
readwrite_pr_t* rwprior_alloc();
int rwprior_free(readwrite_pr_t* rw);
int rwprior_read(readwrite_pr_t* rw, int (doAction)(void*), void* action_arg);
int rwprior_write(readwrite_pr_t* rw, int (doAction)(void*), void* action_arg);
#endif