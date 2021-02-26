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

int init_processes(readproc_t** readproc, calcproc_t** calcproc, dispproc_t** dispproc, const uint16_t* fifosize, const char* filename);
int free_processes(readproc_t* readproc, calcproc_t* calcproc, dispproc_t* dispproc);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t threads[3];
	uint16_t fifosize = 0;
	int ret = 0, *thret = NULL;
	readproc_t* readproc = NULL;
	calcproc_t* calcproc = NULL;
	dispproc_t* dispproc = NULL;
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
	init_processes(&readproc, &calcproc, &dispproc, &fifosize, filename);

    //create the file reading procedure thread
	ret = pthread_create(&threads[0], NULL, readproc_handler, readproc);
	if (ret){
		fprintf(stderr, "main : %s", strerror(ret));
		free_processes(readproc, calcproc, dispproc);
		exit(EXIT_FAILURE);
	}

    //create the calculation procedure thread
	ret = pthread_create(&threads[1], NULL, calcproc_handler, calcproc);
	if (ret){
		fprintf(stderr, "main : %s", strerror(ret));
		free_processes(readproc, calcproc, dispproc);
		exit(EXIT_FAILURE);
	}

    //create the display procedure thread
	ret = pthread_create(&threads[2], NULL, dispproc_handler, dispproc);
	if (ret){
		fprintf(stderr, "main : %s", strerror(ret));
		free_processes(readproc, calcproc, dispproc);
		exit(EXIT_FAILURE);
	}
		
    //wait for the file reading process to finish
	pthread_join(threads[0], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the file reading.\n");
		free_processes(readproc, calcproc, dispproc);
		free(thret);
		exit(EXIT_FAILURE);
	}
		
    //wait for the calculation process to finish
	pthread_join(threads[1], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the characters calculation.\n");
		free_processes(readproc, calcproc, dispproc);
		free(thret);
		exit(EXIT_FAILURE);
	}
		
    //wait for the display process to finish
	pthread_join(threads[2], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the characters diplaying.\n");
		free_processes(readproc, calcproc, dispproc);
		free(thret);
		exit(EXIT_FAILURE);
	}
	
	free_processes(readproc, calcproc, dispproc);
	free(filename);
    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Address of the file reading process structure to initialise                     */
/*		Address of the calculation process structure to initialise						*/
/*		Address of the display process structure to initialise							*/
/*		Amount of slots all the FIFO queues hold										*/
/*  P : Create all the processes and assign them their FIFO queues to use				*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_processes(readproc_t** readproc, calcproc_t** calcproc, dispproc_t** dispproc, const uint16_t* fifosize, const char* filename){
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

	//allocate the file reading process
	if(readproc_alloc(readproc, readfifo, filename) < 0){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	} 

	//allocate the characters calculation process
	if(calcproc_alloc(calcproc, readfifo, dispfifo) < 0){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	} 

	//allocate the characters printing
	if(dispproc_alloc(dispproc, dispfifo) < 0){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

/****************************************************************************************/
/*  I : File reading process to deallocate												*/
/*		Calculation process to deallocate												*/
/*		Display process to deallocate													*/
/*  P : Deallocate process structures and all of their components						*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int free_processes(readproc_t* readproc, calcproc_t* calcproc, dispproc_t* dispproc){
	if(!readproc)
		return 0;
	
	//deallocate the file reading process
	fifo_free(readproc->fifo);
	free(readproc->filename);
	free(readproc);

	//deallocate the calculation process
	fifo_free(calcproc->dispfifo);
	free(calcproc);

	//deallocate the display process structure
	free(dispproc);

	return 0;
}
