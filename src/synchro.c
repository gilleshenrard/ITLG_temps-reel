/*
** synchro.c
** Library regrouping threads synchronisation functions
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 26/02/2021
*/
#include "synchro.h"

/****************************************************************************************/
/*  I : Barrier type to allocate                                                        */
/*      Amount of threads to synchronise                                                */
/*  P : Allocate a Barrier type in memory (no value initialisation)                     */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise and errno is set                                                    */
/****************************************************************************************/
int barrier_alloc(barrier_t** bar, const uint16_t nb){
    uint16_t errtmp = 0;
    
    //check if the number of threads to sync > 0
    if(!nb){
        errno = EINVAL;
        return -1;
    }
    
    //allocate space for the structure
    if(*bar)
        free(*bar);
        
    //allocate space for the structure
    *bar = calloc(1, sizeof(barrier_t));
    if (!*bar){
        errno = ENOMEM;
        return -1;
    }
    
    //initialise the internal mutex
    if(pthread_mutex_init(& (*bar)->mutex, NULL) < 0){
        errtmp = errno;
        free(*bar);
        errno = errtmp;
        return -1;
    }

    //initialise the first semaphore used
    if(sem_init(&(*bar)->turnstile1, 0, 0) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&(*bar)->mutex);
        free(*bar);
        errno = errtmp;
        return -1;
    }

    //initialise the second semaphore used
    if(sem_init(&(*bar)->turnstile2, 0, 1) < 0){
        errtmp = errno;
        sem_destroy (&(*bar)->turnstile1);
        pthread_mutex_destroy (&(*bar)->mutex);
        free(*bar);
        errno = errtmp;
        return -1;
    }

    //set the amount of threads (overall) to synchronise
    (*bar)->th_nb = nb;

    return 0;
}

/****************************************************************************************/
/*  I : Barrier type to deallocate                                                      */
/*  P : Free the memory used by a Barrier type                                          */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int barrier_free(barrier_t* bar){
    pthread_mutex_destroy (&bar->mutex);
    sem_destroy (&bar->turnstile1);
    sem_destroy (&bar->turnstile2);
    free(bar);

    return 0;
}

/****************************************************************************************/
/*  I : barrier structure to synchronise                                                */
/*      procedure to perform in the barrier critical section                            */
/*      argument to use in the barrier critical section                                 */
/*  P : Synchronise the current thread with others using the Barrier technique          */
/*  O : return value of the critical procedure                                          */
/****************************************************************************************/
#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg){
    int ret = 0;
    
    //update the threads count in the barrier
    pthread_mutex_lock(&bar->mutex);
        bar->th_count++;
        if(bar->th_count == bar->th_nb){
            sem_wait(&bar->turnstile2);
            sem_post(&bar->turnstile1);
        }
    pthread_mutex_unlock(&bar->mutex);

    //synchronise all the threads in a first rendezvous
    sem_wait(&bar->turnstile1);
    sem_post(&bar->turnstile1);

    //perform the critical procedure
    if(doAction)
        ret = (*doAction)(action_arg);

    //cleanup the threads count
    pthread_mutex_lock(&bar->mutex);
        bar->th_count--;
        if(!bar->th_count){
            sem_wait(&bar->turnstile1);
            sem_post(&bar->turnstile2);
        }
    pthread_mutex_unlock(&bar->mutex);

    //synchronise all the threads in a second rendezvous
    sem_wait(&bar->turnstile2);
    sem_post(&bar->turnstile2);
    
    return ret;
}

/****************************************************************************************/
/*  I : FIFO type to allocate                                                           */
/*      Size of a slot in the FIFO                                                      */
/*      Amount of slots the FIFO can hold                                               */
/*  P : Allocate a FIFO type in memory (no value initialisation)                        */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise and errno is set                                                    */
/****************************************************************************************/
int fifo_alloc(fifo_t** fifo, const uint16_t elemsz, const uint16_t amount){
    uint16_t errtmp = 0;

    //check if the size and amount are > 0
    if(!elemsz || !amount){
        errno = EINVAL;
        return -1;
    }
    
    //allocate space for the structure
    if(*fifo)
        free(*fifo);

    *fifo = calloc(1, sizeof(fifo_t));
    if (!*fifo){
        errno = ENOMEM;
        return -1;
    }
    
    //initialise the internal mutex
    if(pthread_mutex_init(& (*fifo)->mutex, NULL) < 0){
        errtmp = errno;
        free(*fifo);
        errno = errtmp;
        return -1;
    }

    //initialise the items semaphore to 0
    if(sem_init(&(*fifo)->items, 0, 0) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&(*fifo)->mutex);
        free(*fifo);
        errno = errtmp;
        return -1;
    }

    //initialise the spaces semaphore to the amount of slots
    if(sem_init(&(*fifo)->spaces, 0, amount) < 0){
        errtmp = errno;
        sem_destroy (&(*fifo)->items);
        pthread_mutex_destroy (&(*fifo)->mutex);
        free(*fifo);
        errno = errtmp;
        return -1;
    }

    //allocate space for the FIFO buffer
    (*fifo)->buffer = calloc(amount, elemsz);
    if (!*fifo){
        fifo_free(*fifo);
        errno = ENOMEM;
        return -1;
    }

    //set the amount of slots available in the FIFO, as well as
    //  the input and ouput indexes
    (*fifo)->nb_items = amount;
    (*fifo)->item_sz = elemsz;
    (*fifo)->indexIn = 0;
    (*fifo)->indexOut = 0;

    return 0;
}

/****************************************************************************************/
/*  I : FIFO type to deallocate                                                         */
/*  P : Free the memory used by a FIFO type                                             */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int fifo_free(fifo_t* fifo){
    sem_destroy (&fifo->spaces);
    sem_destroy (&fifo->items);
    pthread_mutex_destroy (&fifo->mutex);
    free(fifo);

    return 0;
}

/****************************************************************************************/
/*  I : FIFO to which push the element                                                  */
/*      Element to push in the FIFO                                                     */
/*  P : Wait for a free slot in the FIFO, then push an element into the next slot       */
/*          available                                                                   */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int fifo_push(fifo_t* fifo, void* elem){
    //wait for a free slot
    sem_wait(&fifo->spaces);

    //insert the new element + increment the input index + rollback to 0 if end of circ. buffer
    pthread_mutex_lock(&fifo->mutex);
        memcpy(fifo->buffer + (fifo->indexIn * fifo->item_sz), elem, fifo->item_sz);
        fifo->indexIn++;
        fifo->indexIn %= fifo->nb_items; 
    pthread_mutex_unlock(&fifo->mutex);

    //increment the amount of items in the FIFO queue
    sem_post(&fifo->items);

    return 0;
}

/****************************************************************************************/
/*  I : FIFO to which push the element                                                  */
/*  P : Wait for an item in the FIFO queue, then pop it when available                  */
/*  O : address of the popped element if ok                                             */
/*      NULL otherwise                                                                  */
/****************************************************************************************/
void* fifo_pop(fifo_t* fifo){
    void* ret = NULL;

    //allocate space for the element to pop (MUST BE DEALLOCATED AFTER USE)
    ret = calloc(1, fifo->item_sz);
    if(!ret)
        return NULL;

    //wait for an item in the FIFO queue
    sem_wait(&fifo->items);

    //get the address of the element to pop + increment the output index
    //  + rollback to 0 if end of circ. buffer
    pthread_mutex_lock(&fifo->mutex);
        memcpy(ret, fifo->buffer + (fifo->indexOut * fifo->item_sz), fifo->item_sz);
        fifo->indexOut++;
        fifo->indexOut %= fifo->nb_items; 
    pthread_mutex_unlock(&fifo->mutex);

    //increment the amount of items in the FIFO queue
    sem_post(&fifo->spaces);

    return ret;
}