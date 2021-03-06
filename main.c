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
** Last modified : 06/03/2021
*/
#include <unistd.h>
#include <stdlib.h>
#include "readwrite.h"

int init_rw(thrw_t** array, const uint16_t nbthreads, void* data, const uint16_t maximum);
int free_rw(thrw_t* array, const uint16_t nbthreads);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char *argv[]){
    thrw_t* rw_array = NULL;
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

    //initialise the readers/writers structures
    if(init_rw(&rw_array, nbthreads, (void*)&data, maximum) < 0){
        fprintf(stderr, "main: error while initialising readers/writers\n");
        exit(EXIT_FAILURE);
    }

    //free memory used by all the readers/writers
    free_rw(rw_array, nbthreads);

    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Array of readers/writers to initialise                          				*/
/*		Amount of readers/writers to create												*/
/*      Data shared by the readers/writers                                              */
/*      Maximum value to reach by the readers/writers                                   */
/*  P : Create the array of readers/writers with the proper values  					*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_rw(thrw_t** array, const uint16_t nbthreads, void* data, const uint16_t maximum){
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

    //allocate the readers/writers array
    for(uint16_t i = 0 ; i < nbthreads ; i++)
        readwrite_assign(&(*array)[i], rw, i, data, maximum);

    return 0;
}

/****************************************************************************************/
/*  I : Array of readers/writers to free                                  				*/
/*      Amount of readers/writers in total                                              */
/*  P : Deallocate the memory used by the readers/writers array       					*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int free_rw(thrw_t* array, const uint16_t nbthreads){
    //deallocate the readwrite structure common to all readers/writers
    rw_free(array[0].rw);
    
    //deallocate the readers/writers array (all have just been assigned individually)
    free(array);
    array = NULL;

    return 0;
}