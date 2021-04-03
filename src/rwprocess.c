/*
** readwrite.c
** Library regrouping functions which are specific to readers/writers threads
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 03/04/2021
*/
#include "rwprocess.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

/****************************************************************************************/
/*  I : readers/writers type used to synchronise readers                                */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*      Priority value (lower means higher priority)                                    */
/*  P : Allocate memory for a reader and fill its fields                                */
/*  O : On success, a new reader/writer structure is returned                           */
/*      On error, NULL is returned and errno is set                                     */
/****************************************************************************************/
thrw_t* rwprocess_alloc(readwrite_ns_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value){
    thrw_t* reader = NULL;

    //attempt to allocate memory for the new reader/writer
    //  (allocation will be checked in assignment function)
    reader = calloc(1, sizeof(thrw_t));

    //fill its fields with the values received
    if(rwprocess_assign(reader, rw, thnum, data, max, nice_value) < 0)
        return NULL;
    else
        return reader;
}

/****************************************************************************************/
/*  I : readers/writer type to fill                                                     */
/*      readers/writers type used to synchronise readers                                */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*      Priority value (lower means higher priority)                                    */
/*  P : Fill the readwrite structure fields                                             */
/*  O : 0 if ok                                                                         */
/*     -1 if error, and errno is set                                                    */
/****************************************************************************************/
int rwprocess_assign(thrw_t* reader, readwrite_ns_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value){
    //check if the readwrite structure has been allocated
    if(!reader){
        errno = ENOMEM;
        return -1;
    }

    //populate the readwrite structure fields
    reader->rw = rw;
    reader->thNum = thnum;
    reader->data = data;
    reader->max = max;
    reader->onPrint = NULL;
    reader->nice_value = nice_value;

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
/*  I : readers/writers type used in the synchro                                        */
/*  P : Wait between 300 and 500ms, then display the thread number and the data value,   */
/*          then stop when the data reached the max value                               */
/*  O : /                                                                               */
/****************************************************************************************/
void *reader_handler(void *reader){
    thrw_t* rd = (thrw_t*)reader;
    uint32_t t = 0;

    nice(rd->nice_value);

    do{
        getLongRand(&t, 100000, 400000);
        usleep(t);
        rwnostarve_read(rd->rw, displayData, rd);
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

    if(*rd->data < rd->max && rd->onPrint)
        rd->onPrint("Reader n°%03d reads : %hd", rd->thNum, *((int*)rd->data));

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro                                        */
/*  P : Wait between 500 and 700us, then increment the data value (and display the      */
/*          result), then stop when the data reached the max value                      */
/*  O : /                                                                               */
/****************************************************************************************/
void *writer_handler(void *writer){
    thrw_t* wr = (thrw_t*)writer;
    uint32_t t = 0;

    nice(wr->nice_value);

    do{
        getLongRand(&t, 300000, 1000000);
        usleep(t);
        rwnostarve_write(wr->rw, updateData, wr);
    }while (*wr->data < wr->max);

    pthread_exit(NULL);
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
        wr->onPrint("Writer n°%03d writes : %hd", wr->thNum, ++*wr->data);

    return 0;
}

/****************************************************************************************/
/*  I : buffer in which the newly generated number will be stored                       */
/*      min value of the rand number                                                    */
/*      max value of the rand number                                                    */
/*  P : Generate a 32 bits unsigned random number between min and max                   */
/*  O : /                                                                               */
/****************************************************************************************/
int getLongRand(uint32_t* buf, const uint32_t min, const uint32_t max){
    *buf = ((uint32_t)rand())<<16;      //generate the two MSB bytes (two random bytes shifted left)
    *buf += (uint32_t)rand();           //generate the two LSB bytes (two random bytes)
    *buf = min + (*buf % (max-min));    //crop the number generated so if fits between min and max

    return 0;
}
