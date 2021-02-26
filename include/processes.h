#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include <stdio.h>
#include "synchro.h"
#include "ctype.h"

typedef struct{
    char*    filename;  //name of the text file to read 
    fifo_t*  fifo;      //fifo to which push the characters
}readproc_t;

typedef struct{
    fifo_t*  readfifo;  //fifo from which popping the characters read in the file
    fifo_t*  dispfifo;  //fifo to which pushing the characters to display
}calcproc_t;

typedef struct{
    fifo_t*  dispfifo;  //fifo to which pushing the characters to display
}dispproc_t;

void *readproc_handler(void *proc);
void *calcproc_handler(void *proc);
void *dispproc_handler(void *proc);
#endif