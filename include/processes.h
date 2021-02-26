#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include <stdio.h>
#include "synchro.h"

typedef struct{
    char*    filename;  //name of the text file to read 
    fifo_t*  fifo;      //fifo to which push the characters
}readproc_t;

void *readproc_handler(void *proc);
#endif