/*
** runner.c
** Library regrouping functions which are specific to runners
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 16/02/2021
*/
#include "runner.h"

/****************************************************************************************/
/*  I : /                                                                               */
/*  P : Execute the behaviour of a runner                                               */
/*  O : /                                                                               */
/****************************************************************************************/
void *runner_handler(void *num)
{
	uint16_t thNum = *((int*)num);

	printf("Thread %hd starts the step 1\n", thNum);
	pthread_exit(NULL);
}