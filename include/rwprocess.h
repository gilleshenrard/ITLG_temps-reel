#ifndef READWRITE_THREAD_INCLUDED
#define READWRITE_THREAD_INCLUDED
#include "synchro.h"

typedef struct{
    uint16_t        thNum;                  //thread number
    uint16_t*       data;                   //data to be manipulated by either readers or writers
    uint16_t        max;                    //maximum value of the data
    readwrite_ns_t* rw;                     //readers/writers synchronisation structure
    void            (*onPrint)(char*, ...); //function used to print info on screen
    uint8_t         nice_value;             //priority value of the thread (lower means higher priority)
}thrw_t;

thrw_t* rwprocess_alloc(readwrite_ns_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value);
int rwprocess_assign(thrw_t* reader, readwrite_ns_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value);
int rwprocess_free(thrw_t* reader);
void *reader_handler(void *reader);
int displayData(void* reader);
void *writer_handler(void *writer);
int updateData(void* writer);
int getLongRand(uint32_t* buf, const uint32_t min, const uint32_t max);
#endif