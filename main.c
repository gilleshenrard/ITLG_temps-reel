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

int init_processes(readproc_t** readproc, calcproc_t** calcproc, const uint16_t* fifosize, const char* filename);
int free_processes(readproc_t* readproc, calcproc_t* calcproc);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t threads[2];
	uint16_t fifosize = 0;
	int ret = 0, *thret = NULL;
	readproc_t* readproc = NULL;
	calcproc_t* calcproc = NULL;
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
	init_processes(&readproc, &calcproc, &fifosize, filename);

    //create the file reading procedure thread
	ret = pthread_create(&threads[0], NULL, readproc_handler, readproc);
	if (ret){
		fprintf(stderr, "main : %s", strerror(ret));
		free_processes(readproc, calcproc);
		exit(EXIT_FAILURE);
	}

    //create the calculation procedure thread
	ret = pthread_create(&threads[1], NULL, calcproc_handler, calcproc);
	if (ret){
		fprintf(stderr, "main : %s", strerror(ret));
		free_processes(readproc, calcproc);
		exit(EXIT_FAILURE);
	}
		
    //wait for the file reading process to finish
	pthread_join(threads[0], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the file reading.\n");
		free_processes(readproc, calcproc);
		free(thret);
		exit(EXIT_FAILURE);
	}
		
    //wait for the calculation process to finish
	pthread_join(threads[1], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the characters calculation.\n");
		free_processes(readproc, calcproc);
		free(thret);
		exit(EXIT_FAILURE);
	}
	
	free_processes(readproc, calcproc);
	free(filename);
    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Address of the file reading process structure to initialise                     */
/*		Address of the calculation process structure to initialise						*/
/*		Amount of slots all the FIFO queues hold										*/
/*  P : Create all the processes and assign them their FIFO queues to use				*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_processes(readproc_t** readproc, calcproc_t** calcproc, const uint16_t* fifosize, const char* filename){
	fifo_t* readfifo = NULL;
	fifo_t* dispfifo = NULL;

	//allocate the FIFO queue holding the characters read
	if(fifo_alloc(&readfifo, sizeof(char), *fifosize) <0 ){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	}

	//allocate the FIFO queue holding the characters read
	if(fifo_alloc(&dispfifo, sizeof(char), *fifosize) <0 ){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	}

	//allocate the file reading process structure
	*readproc = (readproc_t*)calloc(1, sizeof(readproc_t));
	if(!*readproc){
		fprintf(stderr, "init_processes : error while allocating the file reading process structure\n");
		return -1;
	}

	//allocate the calculation process structure
	*calcproc = (calcproc_t*)calloc(1, sizeof(calcproc_t));
	if(!*calcproc){
		fprintf(stderr, "init_processes : error while allocating the calculation process structure\n");
		return -1;
	}

	//assign the file name and reading FIFO to the file reading process
	(*readproc)->filename = calloc(1, strlen(filename) + 1);
	strcpy((*readproc)->filename, filename);
	(*readproc)->fifo = readfifo;

	//assign both FIFO queues to be used in the calculation process
	(*calcproc)->readfifo = readfifo;
	(*calcproc)->dispfifo = dispfifo;

	return 0;
}

/****************************************************************************************/
/*  I : File reading process to deallocate												*/
/*		Calculation process to deallocate												*/
/*  P : Deallocate process structures and all of their components						*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int free_processes(readproc_t* readproc, calcproc_t* calcproc){
	if(!readproc)
		return 0;
	
	//deallocate the file reading process
	fifo_free(readproc->fifo);
	free(readproc->filename);
	free(readproc);

	//deallocate the calculation process
	fifo_free(calcproc->dispfifo);
	free(calcproc);

	return 0;
}
