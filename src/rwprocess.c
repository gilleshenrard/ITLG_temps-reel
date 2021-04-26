/*
** readwrite.c
** Library regrouping functions which are specific to readers/writers threads
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 26/04/2021
*/
#include "rwprocess.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

/****************************************************************************************/
/*  I : readers/writers type used to synchronise readers                                */
/*      barrier type used to make sure all threads start at the same time               */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*      Priority value (lower means higher priority)                                    */
/*  P : Allocate memory for a reader and fill its fields                                */
/*  O : On success, a new reader/writer structure is returned                           */
/*      On error, NULL is returned and errno is set                                     */
/****************************************************************************************/
thrw_t* rwprocess_alloc(void* rw, barrier_t* bar, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value){
    thrw_t* reader = NULL;

    //attempt to allocate memory for the new reader/writer
    //  (allocation will be checked in assignment function)
    reader = calloc(1, sizeof(thrw_t));

    //fill its fields with the values received
    if(rwprocess_assign(reader, rw, bar, thnum, data, max, nice_value) < 0)
        return NULL;
    else
        return reader;
}

/****************************************************************************************/
/*  I : readers/writer type to fill                                                     */
/*      readers/writers type used to synchronise readers                                */
/*      barrier type used to make sure all threads start at the same time               */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*      Priority value (lower means higher priority)                                    */
/*  P : Fill the readwrite structure fields                                             */
/*  O : 0 if ok                                                                         */
/*     -1 if error, and errno is set                                                    */
/****************************************************************************************/
int rwprocess_assign(thrw_t* reader, void* rw, barrier_t* bar, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value){
    //check if the readwrite structure has been allocated
    if(!reader){
        errno = ENOMEM;
        return -1;
    }

    //populate the readwrite structure fields
    reader->rw = rw;
    reader->barrier = bar;
    reader->thNum = thnum;
    reader->data = data;
    reader->max = max;
    reader->nice_value = nice_value;
    reader->wait_min = 0;
    reader->wait_max = 0;
    reader->onPrint = NULL;
    reader->onCritical = NULL;
    reader->onRW = NULL;

    return 0;
}

/****************************************************************************************/
/*  I : readers/writer type to deallocate                                               */
/*  P : Deallocate the memory used by a reader/writer                                   */
/*  O : 0 if ok                                                                         */
/*     -1 if error, and errno is set                                                    */
/****************************************************************************************/
int rwprocess_free(thrw_t* reader){
    free(reader);
    reader = NULL;

    return 0;
}

/****************************************************************************************/
/*  I : readers/writer type of which print the information                              */
/*  P : Print the information held by a reader/writer (used to debug)                   */
/*  O : /                                                                               */
/****************************************************************************************/
void rwprocess_print(thrw_t* reader){
    if (reader && reader->onPrint){
        (*reader->onPrint)("rw=%p, thnum=%03u, data=%p, max=%u, nice=%2u, wait_min=%7u, wait_max=%7u",
                           reader->rw,
                           reader->thNum,
                           reader->data,
                           reader->max,
                           reader->nice_value,
                           reader->wait_min,
                           reader->wait_max);
    }
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro                                        */
/*  P : Wait for a random amount of time, then either increment (writers) or display    */
/*          (readers) the common data until the max value has been reached              */
/*  O : /                                                                               */
/****************************************************************************************/
void *thread_handler(void *reader){
    thrw_t* rd = (thrw_t*)reader;
    uint32_t t = 0;
    int ret = 0;

    //set the thread priority (between 0 and 20, lower has higher priority)
    if(rd->nice_value != NONICE){
        errno = 0;
        ret = nice(rd->nice_value);
        if(ret != rd->nice_value || errno != 0)
            rd->onPrint("thread %u : error on assigning nice (value : %d)", rd->thNum, ret);
    }
    //wait for all readers and writers to be at the barrier
    barrier_sync(rd->barrier, NULL, NULL);

    do{
        //generate a random number of us to wait, then wait said amount
        t = rd->wait_min + ((uint32_t)rand() % (rd->wait_max - rd->wait_min));
        usleep(t);

        //perform either the reader action, or the writer action
        (*rd->onRW)(rd->rw, rd->onCritical, rd);
    }while (*rd->data < rd->max);

    pthread_exit(NULL);
}

/****************************************************************************************/
/*  I : readers/writers type to display                                                 */
/*  P : Display the thread number and the data value                                    */
/*  O : /                                                                               */
/****************************************************************************************/
int displayData(void* reader){
    thrw_t* rd = (thrw_t*)reader;
    uint32_t t = 0;

    //if max not reached yet
    if(*rd->data < rd->max && rd->onPrint){
        //wait for a random amount of time (simulate a slow reading task)
        t = RPROCWAIT_MIN + ((uint32_t)rand() % (RPROCWAIT_MAX - RPROCWAIT_MIN));
        usleep(t);
        
        //display the data
        rd->onPrint("R-%03d-%02hd", rd->thNum, *((int*)rd->data));
    }

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type to display                                                 */
/*  P : Increment the data value and display its value                                  */
/*  O : /                                                                               */
/****************************************************************************************/
int updateData(void* writer){
    thrw_t* wr = (thrw_t*)writer;

    //update the data if not max yet
    if(*wr->data < wr->max && wr->onPrint)
        wr->onPrint("W-%03d-%02hd", wr->thNum, ++*wr->data);

    return 0;
}
