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
#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
void *runner_handler(void *num)
{
	pthread_exit(NULL);
}