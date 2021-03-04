#include "readwrite.h"

/****************************************************************************************/
/*  I : readers type to allocate                                                        */
/*      readers/writers type used to synchronise readers                                */
/*      Thread number                                                                   */
/*      Pointer to the data shared between threads                                      */
/*      Max value the data can take                                                     */
/*  P : Allocate memory for a reader and fill its fields                                */
/*  O : 0 if ok                                                                         */
/*     -1 if error, and errno is set                                                    */
/****************************************************************************************/
int reader_alloc(thrw_t** reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max){
    //allocate the memory for the reader structure
    *reader = calloc(1, sizeof(thrw_t));
    if(!*reader){
        errno = ENOMEM;
        return -1;
    }

    //populate its fields
    (*reader)->rw = rw;
    (*reader)->thNum = thnum;
    (*reader)->data = data;
    (*reader)->max = max;

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
        rw_read(rd->rw, displayData, &rd);
    }while (*rd->data <= rd->max);

    pthread_exit(NULL);
}

/****************************************************************************************/
/*  I : readers/writers type to display                                                 */
/*  P : Display the thread number and the data value                                    */
/*  O : /                                                                               */
/****************************************************************************************/
int displayData(void* reader){
    thrw_t* rd = (thrw_t*)reader;

    fprintf(stdout, "Reader n°%d reads : %d", rd->thNum, *((int*)rd->data));

    return 0;
}