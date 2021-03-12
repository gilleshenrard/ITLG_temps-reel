#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include "synchro.h"

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
int readproc_alloc(readproc_t** readproc, fifo_t* readfifo, const char* filename);

void *calcproc_handler(void *proc);
int calcproc_alloc(calcproc_t** calcproc, fifo_t* readfifo, fifo_t* dispfifo);

void *dispproc_handler(void *proc);
int dispproc_alloc(dispproc_t** dispproc, fifo_t* dispfifo);
#endif