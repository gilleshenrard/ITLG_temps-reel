#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "runner.h"

#define NB_THREADS 5

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char* argv[])
{
	pthread_t thread[NB_THREADS];
	int i;
	int ret;
	
    //create all the runner threads
    for (i = 0; i < NB_THREADS; i++)
		if ((ret = pthread_create(&thread[i],
					  NULL, runner_handler, (void *)&i)) != 0) {
			fprintf(stderr, "%s", strerror(ret));
			exit(EXIT_FAILURE);
		}
		
    //wait for all the runner threads to finish
    for (i = 0; i < NB_THREADS; i++)
		pthread_join(thread[i], NULL);
	
    exit(EXIT_SUCCESS);
}


