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
void *runner_handler(void *run)
{
	printf("inside a thread\n");
	runner_t* runner = (runner_t*)run;
	uint32_t waittime = 0;

	srand(time(NULL));
	waittime = 2001 + (rand_r((uint32_t*)(&runner->threadNum)) % 2999);

	printf("Thread N%hd starts the step 1 (%d us of sleep time)\n", runner->threadNum, waittime);
	usleep(waittime);
	pthread_exit(NULL);
}