/*
** synchro.c
** Library regrouping threads synchronisation functions
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 16/02/2021
*/
#include "synchro.h"

/****************************************************************************************/
/*  I : Barrier type to allocate                                                        */
/*      Amount of threads to synchronise                                                */
/*  P : Allocate a Barrier type in memory (no value initialisation)                     */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise and errno is set                                                    */
/****************************************************************************************/
int barrier_alloc(barrier_t* bar, const uint16_t nb){
    uint16_t errtmp = 0;
    
    //allocate space for the structure
    bar = calloc(1, sizeof(barrier_t));
    if (!bar){
        errno = ENOMEM;
        return -1;
    }
    
    //initialise the internal mutex
    if(pthread_mutex_init(& bar->mutex, NULL) < 0){
        errtmp = errno;
        free(bar);
        errno = errtmp;
        return -1;
    }

    //initialise the first semaphore used
    if(sem_init(&bar->turnstile1, 0, 0) < 0){
        errtmp = errno;
        pthread_mutex_destroy (&bar->mutex);
        free(bar);
        errno = errtmp;
        return -1;
    }

    //initialise the second semaphore used
    if(sem_init(&bar->turnstile2, 0, 1) < 0){
        errtmp = errno;
        sem_destroy (&bar->turnstile1);
        pthread_mutex_destroy (&bar->mutex);
        free(bar);
        errno = errtmp;
        return -1;
    }

    //set the amount of threads (overall) to synchronise
    bar->th_nb = nb;

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
/*  I : /                                                                               */
/*  P : Synchronise the current thread with others using the Barrier technique          */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int barrier_sync(barrier_t* bar){
    return 0;
}