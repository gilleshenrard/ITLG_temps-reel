#ifndef RUNNER_THREAD_INCLUDED
#define RUNNER_THREAD_INCLUDED
#include "synchro.h"

typedef struct{
    char*    filename;  //name of the text file to read 
    fifo_t*  fifo;      //fifo to which push the characters
    void     (*onPrint)(char*, ...); //function used to print info on screen
}readproc_t;

typedef struct{
    fifo_t*  readfifo;  //fifo from which popping the characters read in the file
    fifo_t*  dispfifo;  //fifo to which pushing the characters to display
}calcproc_t;

typedef struct{
    fifo_t*  dispfifo;  //fifo from which popping the characters to display
}dispproc_t;

void *readproc_handler(void *proc);
readproc_t* readproc_alloc(fifo_t* readfifo, const char* filename);

void *calcproc_handler(void *proc);
calcproc_t* calcproc_alloc(fifo_t* readfifo, fifo_t* dispfifo);

void *dispproc_handler(void *proc);
dispproc_t* dispproc_alloc(fifo_t* dispfifo);
#endif