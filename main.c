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

int init_processes(void* processes[], const uint16_t* fifosize, const char* filename);
int launch_threads(pthread_t threads[], void* processes[]);
int free_processes(void* processes[]);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t threads[3];
	uint16_t fifosize = 0;
	int *thret = NULL;
	void* processes[3] = {NULL};
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
	init_processes(processes, &fifosize, filename);

	//launch the threads
	launch_threads(threads, processes);
		
    //wait for the file reading process to finish
	pthread_join(threads[0], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the file reading.\n");
		free_processes(processes);
		free(thret);
		exit(EXIT_FAILURE);
	}
		
    //wait for the calculation process to finish
	pthread_join(threads[1], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the characters calculation.\n");
		free_processes(processes);
		free(thret);
		exit(EXIT_FAILURE);
	}
		
    //wait for the display process to finish
	pthread_join(threads[2], (void**)&thret);
	if(thret){
		fprintf(stderr, "Error while processing the characters diplaying.\n");
		free_processes(processes);
		free(thret);
		exit(EXIT_FAILURE);
	}
	
	free_processes(processes);
	free(filename);
    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Threads to launch																*/
/*		processes to use in the threads													*/
/*  P : Launch all the threads with their assigned processes							*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int launch_threads(pthread_t threads[], void* processes[]){
	void* (*doHandler[3])(void *) = {readproc_handler, calcproc_handler, dispproc_handler};
	int ret = 0;

	for(int i=0 ; i<3 ; i++){
		ret = pthread_create(&threads[i], NULL, doHandler[i], processes[i]);
		if (ret){
			fprintf(stderr, "main : %s", strerror(ret));
			free_processes(processes);
			exit(EXIT_FAILURE);
		}
	}

	return 0;
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
int init_processes(void* processes[], const uint16_t* fifosize, const char* filename){
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
	processes[0] = calloc(1, sizeof(readproc_t));
	if(readproc_alloc((readproc_t**)&processes[0], readfifo, filename) < 0){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	} 

	//allocate the characters calculation process
	processes[1] = calloc(1, sizeof(calcproc_t));
	if(calcproc_alloc((calcproc_t**)&processes[1], readfifo, dispfifo) < 0){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	} 

	//allocate the characters printing
	processes[2] = calloc(1, sizeof(dispproc_t));
	if(dispproc_alloc((dispproc_t**)&processes[2], dispfifo) < 0){
		fprintf(stderr, "init_processes : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

/****************************************************************************************/
/*  I : Array of processes to deallocate												*/
/*  P : Deallocate process structures and all of their components						*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int free_processes(void* processes[]){
	//deallocate the file reading process
	fifo_free(((readproc_t*)processes[0])->fifo);
	free(((readproc_t*)processes[0])->filename);
	free(processes[0]);

	//deallocate the calculation process
	fifo_free(((calcproc_t*)processes[1])->dispfifo);
	free(processes[1]);

	//deallocate the display process structure
	free(processes[2]);

	return 0;
}
