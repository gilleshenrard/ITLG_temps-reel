/*
** runner.c
** Library regrouping functions which are specific to runners
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 17/02/2021
*/
#include "runner.h"

/****************************************************************************************/
/*  I : runner who is executing the procedure                                           */
/*  P : Execute the behaviour of a runner :                                             */
/*			- wait for a random timespan between 2000 and 5000 us						*/  
/*  O : /                                                                               */
/****************************************************************************************/
/*	WARNING : srand() must already have been called beforehand							*/
/****************************************************************************************/
void *runner_handler(void *run)
{
	runner_t* runner = (runner_t*)run;
	uint32_t waittime = 0;

	//sleep for a random amount of usec
	printf("Thread N%hd starts the step 1\n", runner->threadNum);
	waittime = 2000 + (rand() % 3000);
	usleep(waittime);
	
	//reach the barrier
	barrier_sync(runner->barrier);
	
	pthread_exit(NULL);
}