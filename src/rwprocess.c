/*
** readwrite.c
** Library regrouping functions which are specific to readers/writers threads
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 16/03/2021
*/
#include "rwprocess.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

/****************************************************************************************/
/*  I : readers/writer type to allocate                                                 */
/*      readers/writers type used to synchronise readers                                */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*  P : Allocate memory for a reader and fill its fields                                */
/*  O : 0 if ok                                                                         */
/*     -1 if error, and errno is set                                                    */
/****************************************************************************************/
int rwprocess_alloc(thrw_t** reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max){
    *reader = calloc(1, sizeof(thrw_t));
    return rwprocess_assign(*reader, rw, thnum, data, max);
}

/****************************************************************************************/
/*  I : readers/writer type to allocate                                                 */
/*      readers/writers type used to synchronise readers                                */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*  P : Fill the readwrite structure fields                                             */
/*  O : 0 if ok                                                                         */
/*     -1 if error, and errno is set                                                    */
/****************************************************************************************/
int rwprocess_assign(thrw_t* reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max){
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
/*  P : Wait for up to 200us, then display the thread number and the data value,        */
/*          then stop when the data reached the max value                               */
/*  O : /                                                                               */
/****************************************************************************************/
void *reader_handler(void *reader){
    thrw_t* rd = (thrw_t*)reader;

    do{
        usleep(1 + (rand() % 200));
        rw_read(rd->rw, displayData, rd);
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

    if(rd->onPrint)
        rd->onPrint("Reader n°%d reads : %hd", rd->thNum, *((int*)rd->data));

    return 0;
}

/****************************************************************************************/
/*  I : readers/writers type used in the synchro                                        */
/*  P : Wait for 200 to 400us, then increment the data value (and display the result),  */
/*          then stop when the data reached the max value                               */
/*  O : /                                                                               */
/****************************************************************************************/
void *writer_handler(void *writer){
    thrw_t* wr = (thrw_t*)writer;

    do{
        usleep(201 + (rand() % 200));
        rw_write(wr->rw, updateData, wr);
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
        wr->onPrint("Writer n°%d writes : %hd", wr->thNum, ++*wr->data);

    return 0;
}
