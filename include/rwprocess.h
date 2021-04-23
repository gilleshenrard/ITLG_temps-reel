#ifndef READWRITE_THREAD_INCLUDED
#define READWRITE_THREAD_INCLUDED
#include "synchro.h"

#define NONICE 255
#define RPROCWAIT_MIN 100000
#define RPROCWAIT_MAX 400000

typedef struct{
    uint16_t        thNum;                          //thread number
    uint16_t*       data;                           //data to be manipulated by either readers or writers
    uint16_t        max;                            //maximum value of the data
    barrier_t*      barrier;                        //barrier used to make sure all threads start at the same time
    readwrite_ns_t* rw;                             //readers/writers synchronisation structure
    uint8_t         nice_value;                     //priority value of the thread (lower means higher priority)
    uint32_t        wait_min;                       //minimum time to wait before reading/writing
    uint32_t        wait_max;                       //maximum time to wait before reading/writing
    void            (*onPrint)(char*, ...);         //function used to print info on screen
    int             (*onCritical)(void*);           //function used in the thread's critical section
    int             (*onRW)(readwrite_ns_t* rw,     //function used in the readers/writers synchronisation
                            int (doAction)(void*),
                            void* action_arg);
}thrw_t;

thrw_t* rwprocess_alloc(readwrite_ns_t* rw, barrier_t* bar, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value);
int rwprocess_assign(thrw_t* reader, readwrite_ns_t* rw, barrier_t* bar, const uint16_t thnum, uint16_t* data, const uint16_t max, uint8_t nice_value);
int rwprocess_free(thrw_t* reader);
void rwprocess_print(thrw_t* reader);
void *thread_handler(void *reader);
int displayData(void* reader);
int updateData(void* writer);
#endif