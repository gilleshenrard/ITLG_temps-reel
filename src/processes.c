/*
** processes.c
** Library regrouping functions which are specific to processes
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 17/03/2021
*/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include "processes.h"

/****************************************************************************************/
/*  I : process structure to use                                                        */
/*  P : Execute the file reading process :                                              */
/*			- read a text file character by character                                   */
/*          - push each character into the first FIFO queue                             */  
/*  O : NULL if ok                                                                      */
/*     -1 (as a void pointer) if error                                                  */
/****************************************************************************************/
void *readproc_handler(void *proc){
    readproc_t* process = (readproc_t*)proc;
    FILE* pfile = NULL;
    char* ret = NULL;
    uint16_t len = 0;
    char buf = '0';
    uint32_t waittime = 0;

    //indicate to the user that the reading process is starting
    if(process->onPrint)
        process->onPrint("Reading file %s", process->filename);

    //open the file to read
    pfile = fopen(process->filename, "r");
    if(!pfile){
        //push the EOF character to stop the other processes
        buf = EOF;
        fifo_push(process->fifo, (void*)(&buf));
        
        //make the thread return an error message
        len = strlen("readproc_handler : ") + strlen(strerror(errno));
        ret = (char*)calloc(len + 1, sizeof(char));
        sprintf(ret, "readproc_handler : %s", strerror(errno));

        pthread_exit(ret);
    }
    
    //read the file character by character and push all the characters read in
    //  the FIFO queue
    do{
        //sleep between 0 and 300 us
        waittime = rand() % 300;
		usleep(waittime);

        //read a character from the file and output it in the FIFO
        buf = (char)fgetc(pfile);
        fifo_push(process->fifo, (void*)(&buf));
    }while(buf != EOF);
    fclose(pfile);
	
	pthread_exit(ret);
}

/****************************************************************************************/
/*  I : process structure to allocate                                                   */
/*      FIFO used to push characters read                                               */
/*      name of the file to read                                                        */
/*  P : Allocate the process structure and all of its components                        */
/*  O : On success, a new file reading process is returned                              */
/*      On error, NULL is returned and ERRNO is set                                     */
/****************************************************************************************/
readproc_t* readproc_alloc(fifo_t* readfifo, const char* filename){
    readproc_t* readproc = NULL;

    //allocate the file reading process structure
	readproc = (readproc_t*)calloc(1, sizeof(readproc_t));
	if(!readproc){
        errno = ENOMEM;
		return NULL;
	}

	//assign the file name and reading FIFO to the file reading process
	readproc->filename = calloc(1, strlen(filename) + 1);
	if(!readproc->filename){
        free(readproc);
        errno = ENOMEM;
		return NULL;
	}
	strcpy(readproc->filename, filename);
	readproc->fifo = readfifo;
    readproc->onPrint = NULL;

    return readproc;
}

/****************************************************************************************/
/*  I : process structure to use                                                        */
/*  P : Execute the characters calculation process :                                    */
/*          - pop each character from the first FIFO queue                              */
/*          - push them as upper case in the second FIFO queue                          */  
/*  O : NULL                                                                            */
/****************************************************************************************/
void *calcproc_handler(void *proc){
    calcproc_t* process = (calcproc_t*)proc;
    char* buf = NULL, output = '0';
    uint32_t waittime = 0;

    //pop one character at a time and push its upper case in the second FIFO
    //  until reaching EOF
    do{
        //sleep between 300 and 600 us
        waittime = 300 + (rand() % 300);
		usleep(waittime);

        //pop a character from the 1st fifo and push its lower case to the 2nd fifo
        buf = fifo_pop(process->readfifo);
        output = toupper(*buf);
        free(buf);
        fifo_push(process->dispfifo, &output);
    }while(tolower(output) != EOF);

	pthread_exit(NULL);
}

/****************************************************************************************/
/*  I : process structure to allocate                                                   */
/*      FIFO used to pop characters read                                                */
/*      FIFO used to push characters set to upper case                                  */
/*  P : Allocate the process structure and all of its components                        */
/*  O : On success, a new character calculation process is returned                     */
/*      On error, NULL is returned and ERRNO is set                                     */
/****************************************************************************************/
calcproc_t* calcproc_alloc(fifo_t* readfifo, fifo_t* dispfifo){
    calcproc_t* calcproc = NULL;

    //allocate the calculation process structure
	calcproc = (calcproc_t*)calloc(1, sizeof(calcproc_t));
	if(!calcproc){
        errno = ENOMEM;
		return NULL;
	}

	//assign both FIFO queues to be used in the calculation process
	calcproc->readfifo = readfifo;
	calcproc->dispfifo = dispfifo;

    return calcproc;
}

/****************************************************************************************/
/*  I : process structure to use                                                        */
/*  P : Execute the characters displaying process :                                     */
/*          - pop each character from the FIFO queue                                    */
/*          - print them on the display                                                 */  
/*  O : NULL                                                                            */
/****************************************************************************************/
void *dispproc_handler(void *proc){
    dispproc_t* process = (dispproc_t*)proc;
    char* buf = NULL, output = '0';
    uint32_t waittime = 0;

    //pop one character at a time and display it
    //  until reaching EOF
    do{
        //sleep between 600 and 900 us
        waittime = 600 + (rand() % 300);
		usleep(waittime);

        //pop a character from the second FIFO and print it on the screen
        buf = fifo_pop(process->dispfifo);
        output = *buf;
        free(buf);
        if (tolower(output) != EOF){
            fprintf(stdout, "%c", output);
            fflush(stdout);
        }
    }while(tolower(output) != EOF);
    fprintf(stdout, "\n");

	pthread_exit(NULL);
}

/****************************************************************************************/
/*  I : process structure to allocate                                                   */
/*      FIFO used to pop characters set to upper case and display                       */
/*  P : Allocate the process structure and all of its components                        */
/*  O : On success, a new character displaying process is returned                      */
/*      On error, NULL is returned and ERRNO is set                                     */
/****************************************************************************************/
dispproc_t* dispproc_alloc(fifo_t* dispfifo){
    dispproc_t* dispproc = NULL;

    //allocate the calculation process structure
	dispproc = (dispproc_t*)calloc(1, sizeof(dispproc_t));
	if(!dispproc){
        errno = ENOMEM;
		return NULL;
	}

	//assign the display FIFO to the display process
	dispproc->dispfifo = dispfifo;

    return dispproc;
}