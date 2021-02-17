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

#define NB_THREADS 5

int init_runners(runner_t** array, const uint16_t nb);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t thread[NB_THREADS];
	uint16_t i;
	int ret;
	runner_t* runners_array = NULL;

	//initialise the runners
	if (init_runners(&runners_array, NB_THREADS) < 0)
		exit(EXIT_FAILURE);
	
    //create all the runner threads
    for (i = 0; i < NB_THREADS; i++){
		ret = pthread_create(&thread[i], NULL, runner_handler, (void*)(&runners_array[i]));
		if (ret){
			fprintf(stderr, "pthread_create : %s", strerror(ret));
			exit(EXIT_FAILURE);
		}
	}
		
    //wait for all the runner threads to finish
    for (i = 0; i < NB_THREADS; i++)
		pthread_join(thread[i], NULL);
	
    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Address of an array of runners to initialise                                    */
/*		Amount of runners to initialise													*/
/*  P : Create an array of runners and assign them their number and common barrier at	*/
/*			which synchronise with a rendezvous											*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_runners(runner_t** array, const uint16_t nb){
	barrier_t* bar_tmp = NULL;

	//allocate the barrier
	if(barrier_alloc(bar_tmp, nb) <0 ){
		fprintf(stderr, "init_runners : %s\n", strerror(errno));
		return -1;
	}

	//allocate the array of runners
	*array = (runner_t*)calloc(nb, sizeof(runner_t));
	if(!*array){
		fprintf(stderr, "init_runners : error while allocating the runners\n");
		barrier_free(bar_tmp);
		return -1;
	}

	//assign the runner number and barrier
	for (uint16_t i = 0 ; i < nb ; i++){
		(*array)[i].threadNum = i + 1;
		(*array)[i].barrier = bar_tmp;
	}

	return 0;
}
