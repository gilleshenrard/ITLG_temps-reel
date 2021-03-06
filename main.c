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

int init_rw(thrw_t** array, const uint16_t nbthreads, const uint16_t nbwriters, const uint16_t maximum);

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
int main(int argc, char *argv[]){
    thrw_t* rw_array = NULL;
    uint16_t nbthreads = 0, nbwriters = 0, maximum = 0;

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
	}

    //initialise the readers/writers structures
    if(init_rw(&rw_array, nbthreads, nbwriters, maximum) < 0){
        fprintf(stderr, "main: error while initialising readers/writers\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Array of readers/writers to initialise                          				*/
/*		Amount of readers/writers to create												*/
/*      Amount of writers amongst the threads                                           */
/*      Maximum value to reach by the readers/writers                                   */
/*  P : Create the array of readers/writers with the proper values  					*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_rw(thrw_t** array, const uint16_t nbthreads, const uint16_t nbwriters, const uint16_t maximum){
    return 0;
}