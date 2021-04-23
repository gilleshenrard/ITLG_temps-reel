/*
** main.c
** Creates readers/writers threads, which will respectively do the following :
**		- Writers will increment a common data until a specified maximum value is reached
**      - Readers will simply read the data and display it on the screen until max value is reached
**
**      All threads will wait for a random amount of us before doing their task
** -------------------------------------------
** Made by Gilles Henrard
** Last modified : 23/04/2021
*/
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "rwprocess.h"
#include "screen.h"

#define RWAIT_MIN 100000
#define RWAIT_MAX 400000
#define WWAIT_MIN 300000
#define WWAIT_MAX 1000000

enum sched_type {NONE, FIFO, NICE};

int check_args(int argc, char* argv[], uint16_t* nbth, uint16_t* nbwr, uint16_t* max, uint8_t* sched, uint8_t* nice_r, uint8_t* nice_w);
int init_rw(thrw_t** array, pthread_t** threads, const uint16_t nbthreads, void* data, const uint16_t maximum,
        const uint16_t nbwriters, const uint8_t nice_r, const uint8_t nice_w);
int threads_launch(pthread_t th_array[], thrw_t rw_array[], const uint16_t nbthreads, const uint8_t sched);
int threads_join(pthread_t threads[], thrw_t rw_array[], const uint16_t nbthreads);
int free_rw(thrw_t* arrays, pthread_t* threads);

/****************************************************************************************/
/*  I : Amount of arguments passed to the programs                          		    */
/*      List of program arguments                                                       */
/*  P : Main program function                                                           */
/*  O : 0 if no error                                                                   */
/*	    1 otherwise																		*/
/****************************************************************************************/
int main(int argc, char *argv[]){
    thrw_t* rw_array = NULL;
    pthread_t* th_array = NULL;
    uint16_t nbthreads = 0, nbwriters = 0, maximum = 0, data = 0;
    uint8_t nice_r = NONICE, nice_w = NONICE, sched = NONE;

    //check program arguments and recover values given
    if(check_args(argc, argv, &nbthreads, &nbwriters, &maximum, &sched, &nice_r, &nice_w) < 0){
        print_error("wrong amount or values set for program arguments");
		exit(EXIT_FAILURE);
    }

    //initialise time randimisation
	srand(time(NULL));

    //initialise the readers/writers structures
    if(init_rw(&rw_array, &th_array, nbthreads, (void*)&data, maximum, nbwriters, nice_r, nice_w) < 0){
        print_error("main: error while initialising readers/writers");
        exit(EXIT_FAILURE);
    }

    //allocate and launch the threads
    if(threads_launch(th_array, rw_array, nbthreads, sched) < 0){
        print_error("Error while launching threads");
        exit(EXIT_FAILURE);
    }

    //wait for all the threads to finish running
    threads_join(th_array, rw_array, nbthreads);

    //free memory used by all the readers/writers
    free_rw(rw_array, th_array);

    exit(EXIT_SUCCESS);
}

/****************************************************************************************/
/*  I : Amount of arguments passed to the programs                          		    */
/*      List of program arguments                                                       */
/*		Amount of threads to create     												*/
/*      Amount of writers amongst the threads                                           */
/*      Maximum value to reach by the readers/writers                                   */
/*      Method used to prioritise threads (none, nice(), FIFO scheduler, ...)           */
/*      Nice score to assign to readers                                                 */
/*      Nice score to assign to writers                                                 */
/*  P : Check if the program arguments are correct and transform them into values       */
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int check_args(int argc, char* argv[], uint16_t* nbth, uint16_t* nbwr, uint16_t* max, uint8_t* sched, uint8_t* nice_r, uint8_t* nice_w){
    //check if the fifo size and file name have been provided in the program arguments
	if(argc != 5 && argc != 7){
        print_error("usage : ");
        print_error("bin/readerswriters nbthreads nbwriters maximum none");
        print_error("bin/readerswriters nbthreads nbwriters maximum fifo");
        print_error("bin/readerswriters nbthreads nbwriters maximum nice nice_readers nice_writers");
		return -1;
	}
	else
	{
        //assign the parameters
		*nbth = atoi(argv[1]);
		*nbwr = atoi(argv[2]);
		*max = atoi(argv[3]);

        //retrieve the threads priorisation method
        if(!strcmp(argv[4], "none"))
            *sched = NONE;
        else if(!strcmp(argv[4], "nice"))
            *sched = NICE;
        else if(!strcmp(argv[4], "fifo"))
            *sched = FIFO;
        else{
            print_error("usage : Unknown priorisation method : %s", argv[4]);
            return -1;
        }

        //check if the minimum amount of writers is correct
        if(*nbwr < 1 || *nbth < 2){
            print_error("usage : There must be at least 1 writers and 2 threads");
		    return -1;
        }

        //check if the amount of writers is correct
        if(*nbwr >= *nbth){
            print_error("usage : The amount of writers must be inferior to the amount of threads");
            return -1;
        }

        //check if amount of threads is set with FIFO scheduling
        //  (priorities are incremental at each thread, so no more than 19)
        if(*sched == FIFO && *nbth >= 20){
            print_error("usage : With the FIFO scheduling method, no more than 20 threads can be created");
            return -1;
        }

        //if nice scores have been given as arguments, assign them and check their values
        if(argc == 7){
            *nice_r = atoi(argv[5]);
            *nice_w = atoi(argv[6]);

            //score can't be negative due to uint8_t nature
            if(*nice_r > 20 || *nice_w > 20){
                print_error("usage : The nice numbers must be between 0 and 20 included");
		        return -1;
            }
        }
	}

    return 0;
}

/****************************************************************************************/
/*  I : Array of readers/writers to initialise                          				*/
/*      Array of threads to initialise                                                  */
/*		Amount of readers/writers to create												*/
/*      Data shared by the readers/writers                                              */
/*      Maximum value to reach by the readers/writers                                   */
/*      Amount of writers amongst the threads                                           */
/*      Nice score to assign to readers                                                 */
/*      Nice score to assign to writers                                                 */
/*  P : Create the array of readers/writers with the proper values + the array of thr.  */  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int init_rw(thrw_t** array, pthread_t** threads, const uint16_t nbthreads, void* data, const uint16_t maximum,
        const uint16_t nbwriters, const uint8_t nice_r, const uint8_t nice_w){
    readwrite_ns_t* rw = NULL;
    barrier_t* bar = NULL;
    uint16_t i = 0;

    //allocate a readwrite structure shared between all the threads
    rw = rwnostarve_alloc();
    if(!rw){
        print_error("init_rw : %s", strerror(errno));
        return -1;
    }

    //allocate a barrier structure shared between all the threads
    bar = barrier_alloc(nbthreads);
    if(!bar){
        rwnostarve_free(rw);
        print_error("init_rw : %s", strerror(ENOMEM));
        return -1;
    }

    //allocate memory for the whole readers/writers array
    *array = calloc(nbthreads, sizeof(thrw_t));
    if(!*array){
        rwnostarve_free(rw);
        barrier_free(bar);
        print_error("init_rw : %s", strerror(ENOMEM));
        return -1;
    }

    //allocate the pthread array
    *threads = calloc(nbthreads, sizeof(pthread_t));
    if(!*threads){
        rwnostarve_free(rw);
        free(*array);
        print_error("init_rw : %s", strerror(ENOMEM));
        return -1;
    }

    //assign the proper values and functions to the writers
    while(i < nbwriters){
        rwprocess_assign(&(*array)[i], rw, bar, i, data, maximum, nice_w);
        (*array)[i].wait_min = WWAIT_MIN;
        (*array)[i].wait_max = WWAIT_MAX;
        (*array)[i].onRW = rwnostarve_write;
        (*array)[i].onCritical = updateData;
        (*array)[i].onPrint = print_noformat;

        i++;
    }

    //assign the proper values and functions to the readers
    while(i < nbthreads){
        rwprocess_assign(&(*array)[i], rw, bar, i, data, maximum, nice_r);
        (*array)[i].wait_min = RWAIT_MIN;
        (*array)[i].wait_max = RWAIT_MAX;
        (*array)[i].onRW = rwnostarve_read;
        (*array)[i].onCritical = displayData;
        (*array)[i].onPrint = print_noformat;

        i++;
    }

    return 0;
}

/****************************************************************************************/
/*  I : Threads array to allocate and launch                              				*/
/*      Readers/writers array to use in the threads                                     */
/*      Total amount of threads                                                         */
/*      Scheduling method used to prioritise threads                                    */
/*  P : Allocate all the threads as readers or writers and launch them all  			*/  
/*  O : 0 if no error                                                                   */
/*	   -1 otherwise																		*/
/****************************************************************************************/
int threads_launch(pthread_t th_array[], thrw_t rw_array[], const uint16_t nbthreads, const uint8_t sched){
    uint16_t i = 0;
    int err = 0;
    pthread_attr_t *attr = NULL;
    struct sched_param param;

    //if scheduling policy set as FIFO
    if(sched == FIFO){
        //allocate a thread attributes structure
        attr = calloc(1, sizeof(pthread_attr_t));
        if(!attr){
            print_error("threads_launch : calloc : %s", strerror(ENOMEM));
            return -1;
        }

        //set its default values
        pthread_attr_init (attr);
        
        //set the scheduling policy as FIFO
        if ((err = pthread_attr_setschedpolicy(attr, SCHED_FIFO)) != 0) {
            print_error("threads_launch: setschedpolicy: %s", strerror(err));
            free(attr);
            return -1;
        }

        if ((err = pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED)) != 0) {
            print_error("threads_launch: setinheritsched: %s", strerror(err));
            free(attr);
            return -1;
        }
    }

    //launch all the threads
    while(i < nbthreads){
        //if FIFO scheduling policy 
        if(sched == FIFO){
            //set the thread priority (descendant for each new thread)
            param.sched_priority = 20 - i;

            //fixate parameters needed to the thread attributes
            if ((err = pthread_attr_setschedparam(attr, & param)) != 0) {
                print_error("threads_launch: setschedparam: %s", strerror(err));
                free(attr);
                return -1;
            }
        }

        //launch a thread
        err = pthread_create(&th_array[i], attr, thread_handler, (void*)&rw_array[i]);
		if (err){
            print_error("threads_launch : create: %s", strerror(err));
			free_rw(rw_array, th_array);
            free(attr);
			return -1;
		}
        i++;
    }

    free(attr);
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
            print_error("threads_join : %s", thret);
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
    rwnostarve_free(array[0].rw);
    
    //deallocate the readers/writers array (all have just been assigned individually)
    free(array);
    array = NULL;

    //deallocate the threads array
    free(threads);
    threads = NULL;

    return 0;
}