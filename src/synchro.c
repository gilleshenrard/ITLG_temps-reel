/*
** synchro.c
** Library regrouping threads synchronisation functions
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 26/03/2021
*/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "synchro.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  BARRIER PROBLEM FUNCTIONS                                                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/****************************************************************************************/
/*  I : Amount of threads to synchronise                                                */
/*  P : Allocate a Barrier type in memory (no value initialisation)                     */
/*  O : If success, a new barrier is returned                                           */
/*      On error, NULL is returned and errno is set                                     */
/****************************************************************************************/
barrier_t* barrier_alloc(const uint16_t nb){
    pthread_mutexattr_t mut_attr = {0};
    barrier_t* bar = NULL;
    uint16_t errtmp = 0;
    
    //check if the number of threads to sync > 0
    if(!nb){
        errno = EINVAL;
        return NULL;
    }
        
    //allocate space for the structure
    bar = calloc(1, sizeof(barrier_t));
    if (!bar){
        errno = ENOMEM;
        return NULL;
    }

    //initialise the attributes set to the mutex for
    //      priority inheritance
    errtmp = pthread_mutexattr_init(&mut_attr);
    if(errtmp){
        free(bar);
        errno = errtmp;
        return NULL;
    }

    //set the priority inheritance protocol to the mutex
    errtmp = pthread_mutexattr_setprotocol(&mut_attr, PTHREAD_PRIO_INHERIT);
    if(errtmp){
        free(bar);
        errno = errtmp;
        return NULL;
    }
    
    //initialise the internal mutex
    if(pthread_mutex_init(&bar->mutex, &mut_attr) < 0){
        errtmp = errno;
        free(bar);
        errno = errtmp;
        return NULL;
    }

    //initialise the barrier turnstile 1 conditional variable
    if(pthread_cond_init(&bar->cond_turnstile1, NULL) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&bar->mutex);
        free(bar);
        errno = errtmp;
        return NULL;
    }

    //initialise the barrier turnstile 2 conditional variable
    if(pthread_cond_init(&bar->cond_turnstile2, NULL) < 0){
        errtmp = errno;
        pthread_cond_destroy(&bar->cond_turnstile1);
        pthread_mutex_destroy (&bar->mutex);
        free(bar);
        errno = errtmp;
        return NULL;
    }

    //set the amount of threads (overall) to synchronise
    bar->th_nb = nb;
    bar->th_count = 0;
    bar->flipflop = 1;   //turnstile 1 closed and 2 open by default

    return bar;
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



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FIFO PROBLEM FUNCTIONS                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/****************************************************************************************/
/*  I : Size of a slot in the FIFO                                                      */
/*      Amount of slots the FIFO can hold                                               */
/*  P : Allocate a FIFO type in memory (no value initialisation)                        */
/*  O : On success, a new FIFO queue is returned                                        */
/*      On error, NULL is returned and errno is set                                     */
/****************************************************************************************/
fifo_t* fifo_alloc(const uint16_t elemsz, const uint16_t amount){
    pthread_mutexattr_t mut_attr = {0};
    fifo_t* fifo = NULL;
    uint16_t errtmp = 0;

    //check if the size and amount are > 0
    if(!elemsz || !amount){
        errno = EINVAL;
        return NULL;
    }

    fifo = calloc(1, sizeof(fifo_t));
    if (!fifo){
        errno = ENOMEM;
        return NULL;
    }

    //initialise the attributes set to the mutex for
    //      priority inheritance
    errtmp = pthread_mutexattr_init(&mut_attr);
    if(errtmp){
        free(fifo);
        errno = errtmp;
        return NULL;
    }

    //set the priority inheritance protocol to the mutex
    errtmp = pthread_mutexattr_setprotocol(&mut_attr, PTHREAD_PRIO_INHERIT);
    if(errtmp){
        free(fifo);
        errno = errtmp;
        return NULL;
    }
    
    //initialise the internal mutex
    if(pthread_mutex_init(&fifo->mutex, &mut_attr) < 0){
        errtmp = errno;
        free(fifo);
        errno = errtmp;
        return NULL;
    }

    //initialise the FIFO full conditional variable
    if(pthread_cond_init(&fifo->cond_notfull, NULL) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&fifo->mutex);
        free(fifo);
        errno = errtmp;
        return NULL;
    }

    //initialise the FIFO empty conditional variable
    if(pthread_cond_init(&fifo->cond_notempty, NULL) < 0){
        errtmp = errno;
        pthread_cond_destroy(&fifo->cond_notfull);
        pthread_mutex_destroy (&fifo->mutex);
        free(fifo);
        errno = errtmp;
        return NULL;
    }

    //allocate space for the FIFO buffer
    fifo->buffer = calloc(amount, elemsz);
    if (!fifo){
        fifo_free(fifo);
        errno = ENOMEM;
        return NULL;
    }

    //set the amount of slots available in the FIFO, as well as
    //  the input and ouput indexes
    fifo->amount = amount;
    fifo->item_sz = elemsz;
    fifo->indexIn = 0;
    fifo->indexOut = 0;
    fifo->items = 0;
    fifo->spaces = amount;

    return fifo;
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



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  READERS/WRITERS PROBLEM FUNCTIONS                                                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/****************************************************************************************/
/*  I : lightswitch to lock                                                             */
/*      mutex variable used to lock threads                                             */
/*  P : Lock the lightswitch, allowing either the readers to block the writers while    */
/*          they're working, or the opposite                                            */
/*  O : 0 if ok                                                                         */
/*     -1 if error (or error code returned by pthread functions)                        */
/****************************************************************************************/
int lightswitch_lock(lightswitch_t* light, pthread_mutex_t* mutex){
    int err = 0;

    if(!light || !mutex)                        //make sure parameters are not NULL
        return -1;

    err = pthread_mutex_lock(&light->mutex);    //current thread locks the switch mutex
    if(err){return err;}

        if(++light->counter == 1){               //first thread locks the external mutex
            err = pthread_mutex_lock(mutex);
            if(err){
                pthread_mutex_unlock(&light->mutex);
                return err;
            }
        }

    err = pthread_mutex_unlock(&light->mutex);  //current thread releases the switch mutex
    if(err){return err;}
    
    return 0;
}

/****************************************************************************************/
/*  I : lightswitch containing the counter to decrement                                 */
/*      mutex used to unlock threads                                                    */
/*  P : Unlock the lightswitch, to release the readers/writers which had been locked    */
/*  O : 0 if ok                                                                         */
/*     -1 if error (or error code returned by pthread functions)                        */
/****************************************************************************************/
int lightswitch_unlock(lightswitch_t* light, pthread_mutex_t* mutex){
    int err = 0;

    //make sure parameters are not NULL
    if(!light || !mutex)
        return -1;

    err = pthread_mutex_lock(&light->mutex);          //current thread locks the switch mutex
    if(err){return err;}
        
        if(!--light->counter){                        //last thread unlocks the external mutex
            err = pthread_mutex_unlock(mutex);
            if(err){
                pthread_mutex_unlock(&light->mutex);
                return err;
            }
        }
    
    err = pthread_mutex_unlock(&light->mutex);        //current thread releases the switch mutex
    if(err){return err;}
    
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  READERS/WRITERS PROBLEM FUNCTIONS (WRITERS PRIORITY)                                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/****************************************************************************************/
/*  I : /                                                                               */
/*  P : Allocate a readers/writers type in memory                                       */
/*  O : On success, a new reader/writer type is returned                                */
/*      On error, NULL is returned and errno is set                                     */
/****************************************************************************************/
readwrite_pr_t* rwprior_alloc(){
    pthread_mutexattr_t mut_attr = {0};
    readwrite_pr_t* rw = NULL;
    int errtmp = 0;

    //allocate the memory for the rw structure
    rw = calloc(1, sizeof(readwrite_pr_t));
    if(!rw){
        errno = ENOMEM;
        return NULL;
    }

    //initialise the attributes set to the mutex for
    //      priority inheritance
    errtmp = pthread_mutexattr_init(&mut_attr);
    if(errtmp){
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //set the priority inheritance protocol to the mutex
    errtmp = pthread_mutexattr_setprotocol(&mut_attr, PTHREAD_PRIO_INHERIT);
    if(errtmp){
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the internal mutex which blocks the writers
    if(pthread_mutex_init(&rw->readSwitch.mutex, &mut_attr) < 0){
        errtmp = errno;
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the internal mutex which blocks the readers
    if(pthread_mutex_init(&rw->writeSwitch.mutex, &mut_attr) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&rw->readSwitch.mutex);
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the readers blocked conditional variable
    if(pthread_mutex_init(&rw->noReaders, &mut_attr) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&rw->writeSwitch.mutex);
        pthread_mutex_destroy (&rw->readSwitch.mutex);
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the writers blocked conditional variable
    if(pthread_mutex_init(&rw->noWriters, &mut_attr) < 0){
        errtmp = errno;
        rwprior_free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the lightswitches counters and conditional values
    rw->readSwitch.counter = 0;
    rw->writeSwitch.counter = 0;

    return rw;
}

/****************************************************************************************/
/*  I : readers/writers type to deallocate                                              */
/*  P : Free the memory used by a readers/writers type                                  */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int rwprior_free(readwrite_pr_t* rw){
    if(rw){
        pthread_mutex_destroy(&rw->noReaders);
        pthread_mutex_destroy(&rw->noWriters);
        pthread_mutex_destroy(&rw->writeSwitch.mutex);
        pthread_mutex_destroy(&rw->readSwitch.mutex);
        free(rw);
        rw = NULL;
    }

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro (as void for compatibility with        */
/*          nostarve)                                                                   */
/*      critical action for the readers to perform                                      */
/*      argument used in the critical action                                            */
/*  P : Lock the data for the current readers to perform an action (will be interrupt.  */
/*          when a writer shows up (they have the priority))                            */
/*  O : return code of the action for the readers to perform                            */
/****************************************************************************************/
int rwprior_read(void* rdwt, int (doAction)(void*), void* action_arg){
    readwrite_pr_t* rw = (readwrite_pr_t*)rdwt;
    int ret = 0;

    pthread_mutex_lock(&rw->noReaders);                         //current reader waits for the readers mutex to be available
        lightswitch_lock(&rw->readSwitch, &rw->noWriters);      //first reader locks writers
    pthread_mutex_unlock(&rw->noReaders);                       //current reader releases the next reader

        //perform a reader's action
        if(doAction)
            ret = (*doAction)(action_arg);

    lightswitch_unlock(&rw->readSwitch, &rw->noWriters);        //last reader unlocks writers

    return ret;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro (as void for compatibility with        */
/*          nostarve)                                                                   */
/*      critical action for the readers to perform                                      */
/*      argument used in the critical action                                            */
/*  P : Lock the data for the current writers to perform an action (will block          */
/*          the readers because higher priority)                                        */
/*  O : return code of the action for the writers to perform                            */
/****************************************************************************************/
int rwprior_write(void* rdwt, int (doAction)(void*), void* action_arg){
    readwrite_pr_t* rw = (readwrite_pr_t*)rdwt;
    int ret = 0;

    lightswitch_lock(&rw->writeSwitch, &rw->noReaders);     //first writer locks readers
        pthread_mutex_lock(&rw->noWriters);                 //current writer waits for writers mutex to be available

        //perform a writers's action
        if(doAction)
            ret = (*doAction)(action_arg);

        pthread_mutex_unlock(&rw->noWriters);               //current writer releases the next writer
    lightswitch_unlock(&rw->writeSwitch, &rw->noReaders);   //last writer unlocks readers

    return ret;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  READERS/WRITERS PROBLEM FUNCTIONS (NO-STARVE WRITERS)                                                               //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/****************************************************************************************/
/*  I : /                                                                               */
/*  P : Allocate a readers/writers type in memory                                       */
/*  O : On success, a new reader/writer type is returned                                */
/*      On error, NULL is returned and errno is set                                     */
/****************************************************************************************/
readwrite_ns_t* rwnostarve_alloc(){
    pthread_mutexattr_t mut_attr = {0};
    readwrite_ns_t* rw = NULL;
    int errtmp = 0;

    //allocate the memory for the rw structure
    rw = calloc(1, sizeof(readwrite_ns_t));
    if(!rw){
        errno = ENOMEM;
        return NULL;
    }

    //initialise the attributes set to the mutex for
    //      priority inheritance
    errtmp = pthread_mutexattr_init(&mut_attr);
    if(errtmp){
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //set the priority inheritance protocol to the mutex
    errtmp = pthread_mutexattr_setprotocol(&mut_attr, PTHREAD_PRIO_INHERIT);
    if(errtmp){
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the internal mutex which blocks the writers
    if(pthread_mutex_init(&rw->readSwitch.mutex, &mut_attr) < 0){
        errtmp = errno;
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the turnstile mutex
    if(pthread_mutex_init(&rw->turnstile, &mut_attr) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&rw->readSwitch.mutex);
        free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the readers room empty mutex
    if(pthread_mutex_init(&rw->roomEmpty, &mut_attr) < 0){
        errtmp = errno;
        rwnostarve_free(rw);
        errno = errtmp;
        return NULL;
    }

    //initialise the lightswitch counter
    rw->readSwitch.counter = 0;

    return rw;
}

/****************************************************************************************/
/*  I : readers/writers type to deallocate                                              */
/*  P : Free the memory used by a readers/writers type                                  */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int rwnostarve_free(readwrite_ns_t* rw){
    if(rw){
        pthread_mutex_destroy(&rw->turnstile);
        pthread_mutex_destroy(&rw->roomEmpty);
        pthread_mutex_destroy (&rw->readSwitch.mutex);
        free(rw);
        rw = NULL;
    }

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro (as void for compatibility with prior) */
/*      critical action for the readers to perform                                      */
/*      argument used in the critical action                                            */
/*  P : Lock the data for the current readers to perform an action (will lock the       */
/*          writers outside)                                                            */
/*  O : return code of the action for the readers to perform (or pthread functions      */
/*          error codes)                                                                */
/****************************************************************************************/
int rwnostarve_read(void* rdwt, int (doAction)(void*), void* action_arg){
    readwrite_ns_t* rw = (readwrite_ns_t*)rdwt;
    int ret = 0, err = 0;

    err = pthread_mutex_lock(&rw->turnstile);               //current reader waits for the turnstile to be available
    if(err){return err;}
    
    err = pthread_mutex_unlock(&rw->turnstile);             //current reader releases the turnstile for the next thread
    if(err){return err;}

    err = lightswitch_lock(&rw->readSwitch, &rw->roomEmpty);      //first reader locks all the writers
    if(err){return err;}
        
        if(doAction)                                        //current reader performs the critical section
            ret = (*doAction)(action_arg);
    
    err = lightswitch_unlock(&rw->readSwitch, &rw->roomEmpty);    //last reader unlocks the writers
    if(err){return err;}

    return ret;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro (as void for compatibility with prior) */
/*      critical action for the readers to perform                                      */
/*      argument used in the critical action                                            */
/*  P : Lock the data for the current writers to perform an action, but wait for all    */
/*          the readers currently reading to be done                                    */
/*  O : return code of the action for the writers to perform                            */
/****************************************************************************************/
int rwnostarve_write(void* rdwt, int (doAction)(void*), void* action_arg){
    readwrite_ns_t* rw = (readwrite_ns_t*)rdwt;
    int ret = 0, err = 0;

    err = pthread_mutex_lock(&rw->turnstile);                     //current writer waits for the turnstile to be available
    if(err){return err;}
    err = pthread_mutex_lock(&rw->roomEmpty);                     //current writer waits for all the current readers to be done
    if(err){return err;}

    if(doAction)                                            //current writer performs the critical action
        ret = (*doAction)(action_arg);

    err = pthread_mutex_unlock(&rw->turnstile);                   //current writer releases the turnstile
    if(err){return err;}
    err = pthread_mutex_unlock(&rw->roomEmpty);                   //current writer releases the next writer
    if(err){return err;}

    return ret;
}
