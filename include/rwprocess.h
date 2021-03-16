#ifndef READWRITE_THREAD_INCLUDED
#define READWRITE_THREAD_INCLUDED
#include "synchro.h"

typedef struct{
    uint16_t     thNum;                 //thread number
    uint16_t*    data;                  //data to be manipulated by either readers or writers
    uint16_t     max;                   //maximum value of the data
    readwrite_t* rw;                    //readers/writers synchronisation structure
    void        (*onPrint)(char*, ...); //function used to print info on screen
}thrw_t;

int rwprocess_alloc(thrw_t** reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max);
int rwprocess_assign(thrw_t* reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max);
int rwprocess_free(thrw_t* reader);
void *reader_handler(void *reader);
int displayData(void* reader);
void *writer_handler(void *writer);
int updateData(void* writer);
#endif