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
/*  I : /                                                                               */
/*  P : Execute the behaviour of a runner :                                             */
/*			wait for a random timespan between 2000 and 5000 us							*/  
/*  O : /                                                                               */
/****************************************************************************************/
void *runner_handler(void *num)
{
	uint16_t thNum = *((uint16_t*)num);
	uint32_t waittime = 0, tmp = (uint32_t)thNum;

	srand(time(NULL));
	waittime = 2001 + (rand_r(&tmp) % 2999);

	printf("Thread N%hd starts the step 1 (%d us of sleep time)\n", thNum, waittime);
	usleep(waittime);
	pthread_exit(NULL);
}