/*
** main.c
** Creates processes threads, which will respectively do the following :
**		- file reading process : read a file one character at a time, and feed them to a FIFO queue
**		- characters processing : take each character from the reading FIFO, make them upper case,
**			then feed them to a second FIFO queue
**		- displaying process : take each character from the second FIFO, and display them one at a time
**			on screen
** -------------------------------------------
** Made by Gilles Henrard
** Last modified : 26/02/2021
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "processes.h"

int init_processes(readproc_t** readproc, const uint16_t* fifosize, const char* filename);
int free_processes(readproc_t* readproc);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t threads;
	uint16_t fifosize = 0;
	int ret = 0, *thret = NULL;
	readproc_t* readproc = NULL;
	char* filename = NULL;

	//check if the fifo size and file name have been provided in the program arguments
	if(argc != 3){
		fprintf(stderr, "usage : bin/prodcons fifo_size filename\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		fifosize = atoi(argv[1]);
		filename = (char*)calloc(strlen(argv[2]) + 1, sizeof(char));
		if(!filename){
			fprintf(stderr, "main : error while allocating the file name buffer\n");
			exit(EXIT_FAILURE);
		}
		strcpy(filename, argv[2]);
	}

	//initialise the processes and the FIFO queues they use
	init_processes(&readproc, &fifosize, filename);

    //create the file reading procedure thread
	ret = pthread_create(&threads, NULL, readproc_handler, readproc);
	if (ret){
		fprintf(stderr, "main : %s", strerror(ret));
		free_processes(readproc);
		exit(EXIT_FAILURE);
	}
		
    //wait for all the runner threads to finish
	pthread_join(threads, (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the file reading.\n");
		free_processes(readproc);
		exit(EXIT_FAILURE);
	}
	free(thret);
	
	free(filename);
    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Address of the file reading process structure to initialise                     */
/*		Amount of slots all the FIFO queues hold										*/
/*  P : Create all the processes and assign them their FIFO queues to use				*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_processes(readproc_t** readproc, const uint16_t* fifosize, const char* filename){
	fifo_t* readfifo = NULL;

	//allocate the FIFO queue holding the characters read
	if(fifo_alloc(&readfifo, sizeof(char), *fifosize) <0 ){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	}

	//allocate the file reading process structure
	*readproc = (readproc_t*)calloc(1, sizeof(readproc_t));
	if(!*readproc){
		fprintf(stderr, "init_processes : error while allocating the file reading process structure\n");
		return -1;
	}

	//assign the file name and reading FIFO to the file reading process
	(*readproc)->filename = calloc(1, strlen(filename) + 1);
	strcpy((*readproc)->filename, filename);
	(*readproc)->fifo = readfifo;

	return 0;
}

/****************************************************************************************/
/*  I : File reading process to deallocate												*/
/*  P : Deallocate process structures and all of their components						*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int free_processes(readproc_t* readproc){
	if(!readproc)
		return 0;
	
	//deallocate the file reading process
	fifo_free(readproc->fifo);
	free(readproc->filename);
	free(readproc);

	return 0;
}
