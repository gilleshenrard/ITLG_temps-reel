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
/*  P : Allocate a Barrier type in memory (no value initialisation)                     */
/*  O : 0 if ok                                                                         */
/*     -1 otherwise                                                                     */
/****************************************************************************************/
int barrier_alloc(barrier_t* bar){
    bar = calloc(1, sizeof(barrier_t));
    if (bar){
        pthread_mutex_init(& bar->mutex, NULL);
        sem_init (&bar->turnstile1, 0, 1);
        sem_init (&bar->turnstile2, 0, 1);
    }
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
int barrier_sync(){
    return 0;
}