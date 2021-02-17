/*
** main.c
** Creates runners threads, which will all loop around the following procedure :
** - sleep for a random amount of time
** - wait for a rendezvous at a barrier (Barrier multithreading problem)
** - exit their handler
** -------------------------------------------
** Made by Gilles Henrard
** Last modified : 17/02/2021
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "runner.h"

int init_runners(runner_t** array, pthread_t** threads, const uint16_t nbrun, const uint16_t nbturns);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t* threads;
	uint16_t i, nbrun, nblaps;
	int ret;
	runner_t* runners_array = NULL;

	//check if the amount of runners and laps to run have been provided as program arguments
	if(argc != 3){
		fprintf(stderr, "usage : bin/runners nb_runners nb_laps");
		exit(EXIT_FAILURE);
	}
	else
	{
		nbrun = atoi(argv[1]);
		nblaps = atoi(argv[2]);
	}

	//initialise the runners
	if (init_runners(&runners_array, &threads, nbrun, nblaps) < 0)
		exit(EXIT_FAILURE);

	//initialise time randimisation
	srand(time(NULL));
	
    //create all the runner threads
    for (i = 0; i < nbrun; i++){
		ret = pthread_create(&threads[i], NULL, runner_handler, (void*)(&runners_array[i]));
		if (ret){
			fprintf(stderr, "pthread_create : %s", strerror(ret));
			exit(EXIT_FAILURE);
		}
	}
		
    //wait for all the runner threads to finish
    for (i = 0; i < nbrun; i++)
		pthread_join(threads[i], NULL);
	
    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Address of an array of runners to initialise                                    */
/*		Amount of runners to initialise													*/
/*		Amount of turns all the runners must do											*/
/*  P : Create an array of runners and assign them their number and common barrier at	*/
/*			which synchronise with a rendezvous											*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_runners(runner_t** array, pthread_t** threads, const uint16_t nbrun, const uint16_t nbturns){
	barrier_t* bar_tmp = NULL;

	//allocate the barrier
	if(barrier_alloc(&bar_tmp, nbrun) <0 ){
		fprintf(stderr, "init_runners : %s\n", strerror(errno));
		return -1;
	}

	//allocate the array of runners
	*array = (runner_t*)calloc(nbrun, sizeof(runner_t));
	if(!*array){
		fprintf(stderr, "init_runners : error while allocating the runners\n");
		barrier_free(bar_tmp);
		return -1;
	}

	//allocate the threads
	*threads = (pthread_t*)calloc(nbrun, sizeof(pthread_t));
	if(!*threads){
		fprintf(stderr, "init_runners : error while allocating the threads\n");
		barrier_free(bar_tmp);
		free(*array);
		return -1;
	}

	//assign the runner number and barrier
	for (uint16_t i = 0 ; i < nbrun ; i++){
		(*array)[i].threadNum = i + 1;
		(*array)[i].barrier = bar_tmp;
		(*array)[i].nbTurns = nbturns;
	}

	return 0;
}
