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

	for(uint16_t i = 0 ; i < runner->nbTurns ; i++){
		//sleep for a random amount of usec
		printf("Thread N%hd starts the step %hd\n", runner->threadNum, i + 1);
		waittime = 2000 + (rand() % 3000);
		usleep(waittime);
		
		//reach the barrier
		barrier_sync(runner->barrier, print_barrier, run);
	}
	
	pthread_exit(NULL);
}

/****************************************************************************************/
/*  I : runner who is reaching the barrier	                                            */
/*  P : Execute the behaviour of a runner :                                             */
/*			- wait for a random timespan between 2000 and 5000 us						*/  
/*  O : /                                                                               */
/****************************************************************************************/
int print_barrier(void* run){
	runner_t* runner = (runner_t*)run;

	printf("Thread N%hd reaches barrier\n", runner->threadNum);
	return 0;
}