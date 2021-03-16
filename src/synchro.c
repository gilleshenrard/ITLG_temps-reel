/*
** synchro.c
** Library regrouping threads synchronisation functions
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 15/03/2021
*/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

    //initialise the barrier turnstile 1 conditional variable
    if(pthread_cond_init(&(*bar)->cond_turnstile1, NULL) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&(*bar)->mutex);
        free(*bar);
        errno = errtmp;
        return -1;
    }

    //initialise the barrier turnstile 2 conditional variable
    if(pthread_cond_init(&(*bar)->cond_turnstile2, NULL) < 0){
        errtmp = errno;
        pthread_cond_destroy(&(*bar)->cond_turnstile1);
        pthread_mutex_destroy (&(*bar)->mutex);
        free(*bar);
        errno = errtmp;
        return -1;
    }

    //set the amount of threads (overall) to synchronise
    (*bar)->th_nb = nb;
    (*bar)->th_count = 0;
    (*bar)->flipflop = 1;   //turnstile 1 closed and 2 open by default

    return 0;
}

/****************************************************************************************/
/*  I : Barrier type to deallocate                                                      */
/*  P : Free the memory used by a Barrier type                                          */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int barrier_free(barrier_t* bar){
    if(bar){
        pthread_mutex_destroy (&bar->mutex);
        pthread_cond_destroy(&bar->cond_turnstile1);
        pthread_cond_destroy(&bar->cond_turnstile2);
        free(bar);
        bar = NULL;
    }

    return 0;
}

/****************************************************************************************/
/*  I : barrier structure to synchronise                                                */
/*      procedure to perform in the barrier critical section                            */
/*      argument to use in the barrier critical section                                 */
/*  P : Synchronise the current thread with others using the Barrier technique          */
/*  O : return value of the critical procedure                                          */
/****************************************************************************************/
int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg){
    int ret = 0;
    
    //wait for all the threads at the turnstile1
    pthread_mutex_lock(&bar->mutex);
        //increment the threads count in the barrier
        bar->th_count++;

        //if all threads arrived, unlock turnstile1
        //  and lock turnstile2
        if(bar->th_count >= bar->th_nb){
            pthread_cond_broadcast(&bar->cond_turnstile1);
            bar->flipflop = 0;
        }

        //synchronise all the threads at the turnstile1
        while(bar->flipflop)
            pthread_cond_wait(&bar->cond_turnstile1, &bar->mutex);
        pthread_cond_signal(&bar->cond_turnstile1);
    pthread_mutex_unlock(&bar->mutex);

    //perform the critical procedure
    if(doAction)
        ret = (*doAction)(action_arg);

    //wait for all the threads at the turnstile1
    pthread_mutex_lock(&bar->mutex);
        //decrement the threads count
        bar->th_count--;

        //if all threads are done with the critical section,
        //  lock turnstile1 (to be reused) and unlock turnstile2
        if(!bar->th_count){
            pthread_cond_signal(&bar->cond_turnstile2);
            bar->flipflop = 1;
        }

        //synchronise all the threads  at the turnstile2
        while(!bar->flipflop)
            pthread_cond_wait(&bar->cond_turnstile2, &bar->mutex);
        pthread_cond_signal(&bar->cond_turnstile2);
    pthread_mutex_unlock(&bar->mutex);
    
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

    //initialise the FIFO full conditional variable
    if(pthread_cond_init(&(*fifo)->cond_notfull, NULL) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&(*fifo)->mutex);
        free(*fifo);
        errno = errtmp;
        return -1;
    }

    //initialise the FIFO empty conditional variable
    if(pthread_cond_init(&(*fifo)->cond_notempty, NULL) < 0){
        errtmp = errno;
        pthread_cond_destroy(&(*fifo)->cond_notfull);
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
    (*fifo)->amount = amount;
    (*fifo)->item_sz = elemsz;
    (*fifo)->indexIn = 0;
    (*fifo)->indexOut = 0;
    (*fifo)->items = 0;
    (*fifo)->spaces = amount;

    return 0;
}

/****************************************************************************************/
/*  I : FIFO type to deallocate                                                         */
/*  P : Free the memory used by a FIFO type                                             */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int fifo_free(fifo_t* fifo){
    if(fifo){
        pthread_cond_destroy(&fifo->cond_notfull);
        pthread_cond_destroy(&fifo->cond_notempty);
        pthread_mutex_destroy(&fifo->mutex);
        free(fifo);
    }

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
    //lock the mutex for the current thread
    pthread_mutex_lock(&fifo->mutex);
    
        //wait for a free slot
        while(!fifo->spaces)
            pthread_cond_wait(&fifo->cond_notfull, &fifo->mutex);

        //insert the new element + increment the input index + rollback to 0 if end of circ. buffer
        memcpy(fifo->buffer + (fifo->indexIn * fifo->item_sz), elem, fifo->item_sz);
        fifo->indexIn++;
        fifo->indexIn %= fifo->amount; 

        //increment the amount of items in the FIFO queue and decrement the amount of free space
        fifo->items++;
        fifo->spaces--;

    //notify all the threads that the mutex is free and unlock it
    pthread_cond_signal(&fifo->cond_notempty);
    pthread_mutex_unlock(&fifo->mutex);

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

    //lock the mutex for the current thread
    pthread_mutex_lock(&fifo->mutex);

        //wait for an item in the FIFO queue
        while(!fifo->items)
            pthread_cond_wait(&fifo->cond_notempty, &fifo->mutex);

        //get the address of the element to pop + increment the output index
        //  + rollback to 0 if end of circ. buffer
        memcpy(ret, fifo->buffer + (fifo->indexOut * fifo->item_sz), fifo->item_sz);
        fifo->indexOut++;
        fifo->indexOut %= fifo->amount;

        //increment the amount of spaces available and decrement the amount of items in the FIFO
        fifo->items--;
        fifo->spaces++;

    //notify all the threads that the mutex is free and unlock it
    pthread_cond_signal(&fifo->cond_notfull);
    pthread_mutex_unlock(&fifo->mutex);

    return ret;
}

/****************************************************************************************/
/*  I : lightswitch to lock                                                             */
/*      condition variable used to lock threads                                         */
/*      flag used in addition to the condition variable as a security                   */
/*  P : Lock the lightswitch, allowing either the readers to block the writers while    */
/*          they're working, or the opposite                                            */
/*  O : 0 if ok                                                                         */
/*     -1 if error                                                                      */
/****************************************************************************************/
int lightswitch_lock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag){
    int lock_acquired = 0;

    //make sure parameters are not NULL
    if(!light || !cond || !flag)
        return -1;

    lock_acquired = pthread_mutex_trylock(&light->mutex);   //if mutex not already locked, lock it
        light->counter++;                                   //increment the counter

        if(light->counter == 1){                            //lock the condition when the first thread
            *flag = 0;                                      //  reaches the switch
            while(!flag)
                pthread_cond_wait(cond, &light->mutex);
        }

    if(!lock_acquired)                                      //if mutex locked with trylock, unlock it
        pthread_mutex_unlock(&light->mutex);
    
    return 0;
}

/****************************************************************************************/
/*  I : lightswitch containing the counter to decrement                                 */
/*      condition variable used to unlock threads                                       */
/*      flag used in addition to the condition variable as a security                   */
/*  P : Unlock the lightswitch, to release the readers/writers which had been locked    */
/*  O : 0 if ok                                                                         */
/*     -1 if error                                                                      */
/****************************************************************************************/
int lightswitch_unlock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag){
    int lock_acquired = 0;

    //make sure parameters are not NULL
    if(!light || !cond || !flag)
        return -1;

    lock_acquired = pthread_mutex_trylock(&light->mutex);   //if mutex not already locked, lock it
        light->counter--;                                   //decrement the counter

        if(!light->counter){                                //unlock the condition when the last thread
            *flag = 1;                                      //  reaches the switch
            pthread_cond_signal(cond);
        }

    if(!lock_acquired)                                      //if mutex locked with trylock, unlock it
        pthread_mutex_unlock(&light->mutex);
    
    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type to allocate                                                */
/*  P : Allocate a readers/writers type in memory                                       */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise and errno is set                                                    */
/****************************************************************************************/
int rw_alloc(readwrite_t** rw){
    int errtmp = 0;

    //allocate the memory for the rw structure
    *rw = calloc(1, sizeof(readwrite_t));
    if(!*rw){
        errno = ENOMEM;
        return -1;
    }

    //initialise the internal mutex for the readers
    if(pthread_mutex_init(& (*rw)->readSwitch.mutex, NULL) < 0){
        errtmp = errno;
        free(*rw);
        errno = errtmp;
        return -1;
    }

    //initialise the internal mutex for the writers
    if(pthread_mutex_init(& (*rw)->writeSwitch.mutex, NULL) < 0){
        errtmp = errno;
        pthread_mutex_destroy (& (*rw)->readSwitch.mutex);
        free(*rw);
        errno = errtmp;
        return -1;
    }

    //initialise the FIFO full conditional variable
    if(pthread_cond_init(&(*rw)->cond_noReaders, NULL) < 0){
        errtmp = errno;
        pthread_mutex_destroy (& (*rw)->writeSwitch.mutex);
        pthread_mutex_destroy (& (*rw)->readSwitch.mutex);
        free(*rw);
        errno = errtmp;
        return -1;
    }

    //initialise the FIFO full conditional variable
    if(pthread_cond_init(&(*rw)->cond_noWriters, NULL) < 0){
        errtmp = errno;
        rw_free(*rw);
        errno = errtmp;
        return -1;
    }

    //initialise the lightswitches counters
    (*rw)->readSwitch.counter = 0;
    (*rw)->writeSwitch.counter = 0;
    (*rw)->noReaders = 1;
    (*rw)->noWriters = 1;

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type to deallocate                                              */
/*  P : Free the memory used by a readers/writers type                                  */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int rw_free(readwrite_t* rw){
    if(rw){
        pthread_cond_destroy(&rw->cond_noReaders);
        pthread_cond_destroy(&rw->cond_noWriters);
        pthread_mutex_destroy (&rw->writeSwitch.mutex);
        pthread_mutex_destroy (&rw->readSwitch.mutex);
        free(rw);
    }

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro                                        */
/*      critical action for the readers to perform                                      */
/*      argument used in the critical action                                            */
/*  P : Lock the data for the current readers to perform an action (will be interrupt.  */
/*          when a writer shows up (they have the priority))                            */
/*  O : return code of the action for the readers to perform                            */
/****************************************************************************************/
int rw_read(readwrite_t* rw, int (doAction)(void*), void* action_arg){
    int ret = 0;
    
    //make the first reader lock the switch to forbid any writer to perform
    //  an action
    pthread_mutex_lock(&rw->readSwitch.mutex);                                      //reader takes the read mutex
        while (!rw->noReaders)                                                      //reader waits for read cond to be true
            pthread_cond_wait(&rw->cond_noReaders, &rw->readSwitch.mutex);
        lightswitch_lock(&rw->readSwitch, &rw->cond_noWriters, &rw->noWriters);     //first reader locks writers
        pthread_cond_signal(&rw->cond_noReaders);                                   //reader releases the next reader

            //perform a reader's action
            if(doAction)
                ret = (*doAction)(action_arg);
        
        //make the last reader unlock the switch and allow writers to act
        lightswitch_unlock(&rw->readSwitch, &rw->cond_noWriters, &rw->noWriters);   //last reader unlocks writers
    pthread_mutex_unlock(&rw->readSwitch.mutex);                                    //reader releases readers mutex

    return ret;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro                                        */
/*      critical action for the readers to perform                                      */
/*      argument used in the critical action                                            */
/*  P : Lock the data for the current writers to perform an action (will block          */
/*          the readers because higher priority)                                        */
/*  O : return code of the action for the writers to perform                            */
/****************************************************************************************/
int rw_write(readwrite_t* rw, int (doAction)(void*), void* action_arg){
    int ret = 0;

    //make the first writer lock the switch to forbid any reader to perform
    //  an action
    pthread_mutex_lock(&rw->writeSwitch.mutex);                                     //writer takes the write mutex
        lightswitch_lock(&rw->writeSwitch, &rw->cond_noReaders, &rw->noReaders);    //first writer locks readers
            while (!rw->noWriters){
                pthread_cond_wait(&rw->cond_noWriters, &rw->writeSwitch.mutex);     //writer waits for write cond to be true
            }

                //perform a writers's action
                if(doAction)
                    ret = (*doAction)(action_arg);

            //make the last writer unlock the switch and allow readers to act
            pthread_cond_signal(&rw->cond_noWriters);                               //writer releases the next writer
        lightswitch_unlock(&rw->writeSwitch, &rw->cond_noReaders, &rw->noReaders);  //last writer unlocks readers
    pthread_mutex_unlock(&rw->writeSwitch.mutex);                                   //writer releases the write mutex

    return ret;
}
