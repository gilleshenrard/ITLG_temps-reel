/*
** main.c
** Creates readers/writers threads, which will respectively do the following :
**		- Writers will increment a common data until a specified maximum value is reached
**      - Readers will simply read the data and display it on the screen until max value is reached
**
**      All threads will wait for a random amount of us before doing their task
**      The writers will have the priority over the readers (no starve readers/writers synchronisation method)
** -------------------------------------------
** Made by Gilles Henrard
** Last modified : 14/03/2021
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "readwrite.h"

int init_rw(thrw_t** array, pthread_t** threads, const uint16_t nbthreads, void* data, const uint16_t maximum);
int threads_launch(pthread_t th_array[], thrw_t rw_array[], const uint16_t nbthreads, const uint16_t nbwriters);
int threads_join(pthread_t threads[], thrw_t rw_array[], const uint16_t nbthreads);
int free_rw(thrw_t* arrays, pthread_t* threads);

int main(int argc, char *argv[]){
    thrw_t* rw_array = NULL;
    pthread_t* th_array = NULL;
    uint16_t nbthreads = 0, nbwriters = 0, maximum = 0, data = 0;

    //check if the fifo size and file name have been provided in the program arguments
	if(argc != 4){
		fprintf(stderr, "usage : bin/readerswriters nbthreads nbwriters maximum\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		nbthreads = atoi(argv[1]);
		nbwriters = atoi(argv[2]);
		maximum = atoi(argv[3]);

        //check if the minimum amount of writers is correct
        if(nbwriters < 1 || nbthreads < 2){
            fprintf(stderr, "There must be at least 1 writers and 2 threads\n");
            exit(EXIT_FAILURE);
        }

        //check if the amount of writers is correct
        if(nbwriters >= nbthreads){
            fprintf(stderr, "The amount of writers must be inferior to the amount of threads\n");
            exit(EXIT_FAILURE);
        }
	}

    //initialise time randimisation
	srand(time(NULL));

    //initialise the readers/writers structures
    if(init_rw(&rw_array, &th_array, nbthreads, (void*)&data, maximum) < 0){
        fprintf(stderr, "main: error while initialising readers/writers\n");
        exit(EXIT_FAILURE);
    }

    //allocate and launch the threads
    threads_launch(th_array, rw_array, nbthreads, nbwriters);

    //wait for all the threads to finish running
    threads_join(th_array, rw_array, nbthreads);

    //free memory used by all the readers/writers
    free_rw(rw_array, th_array);

    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Array of readers/writers to initialise                          				*/
/*      Array of threads to initialise                                                  */
/*		Amount of readers/writers to create												*/
/*      Data shared by the readers/writers                                              */
/*      Maximum value to reach by the readers/writers                                   */
/*  P : Create the array of readers/writers with the proper values + the array of thr.  */  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_rw(thrw_t** array, pthread_t** threads, const uint16_t nbthreads, void* data, const uint16_t maximum){
    readwrite_t* rw = NULL;

    //allocate a readwrite structure shared between all the threads
    if(rw_alloc(&rw) < 0){
        fprintf(stderr, "init_rw : %s\n", strerror(errno));
        return -1;
    }

    //allocate memory for the whole readers/writers array
    *array = calloc(nbthreads, sizeof(thrw_t));
    if(!*array){
        rw_free(rw);
        fprintf(stderr, "init_rw : %s\n", strerror(ENOMEM));
        return -1;
    }

    //allocate the pthread array
    *threads = calloc(nbthreads, sizeof(pthread_t));
    if(!*threads){
        rw_free(rw);
        free(*array);
        fprintf(stderr, "init_rw : %s\n", strerror(ENOMEM));
        return -1;
    }

    //allocate the readers/writers array
    for(uint16_t i = 0 ; i < nbthreads ; i++)
        readwrite_assign(&(*array)[i], rw, i, data, maximum);

    return 0;
}

/****************************************************************************************/
/*  I : Threads array to allocate and launch                              				*/
/*      Readers/writers array to use in the threads                                     */
/*      Total amount of threads                                                         */
/*      Amount of writers in the threads                                                */
/*  P : Allocate all the threads as readers or writers and launch them all  			*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int threads_launch(pthread_t th_array[], thrw_t rw_array[], const uint16_t nbthreads, const uint16_t nbwriters){
    uint16_t i = 0;
    int ret = 0;

    //launch all the threads
    while(i < nbthreads){
        ret = pthread_create(&th_array[i], NULL, (i < nbwriters ? writer_handler : reader_handler), (void*)&rw_array[i]);
		if (ret){
			fprintf(stderr, "threads_launch : %s", strerror(ret));
			free_rw(rw_array, th_array);
			exit(EXIT_FAILURE);
		}
        i++;
    }

    return 0;
}

/****************************************************************************************/
/*  I : Threads to join																	*/
/*		readers/writers used in the threads												*/
/*      Amount of threads previousely created                                           */
/*  P : Wait for all the threads to finish running										*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int threads_join(pthread_t threads[], thrw_t rw_array[], const uint16_t nbthreads){
	char *thret = NULL;
	
	for (uint16_t i = 0 ; i < nbthreads ; i++)
	{
		pthread_join(threads[i], (void**)&thret);
		if(thret){
			fprintf(stderr, "%s\n", thret);
			free_rw(rw_array, threads);
			free(thret);
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

/****************************************************************************************/
/*  I : Array of readers/writers to free                                  				*/
/*      Array of threads to free                                                        */
/*  P : Deallocate the memory used by the readers/writers array and threads array		*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int free_rw(thrw_t* array, pthread_t* threads){
    //deallocate the readwrite structure common to all readers/writers
    rw_free(array[0].rw);
    
    //deallocate the readers/writers array (all have just been assigned individually)
    free(array);
    array = NULL;

    //deallocate the threads array
    free(threads);
    threads = NULL;

    return 0;
}