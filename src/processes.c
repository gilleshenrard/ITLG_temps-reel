/*
** processes.c
** Library regrouping functions which are specific to processes
**      (as described in the assignment)
** ----------------------------------------------------
** Made by Gilles Henrard
** Last modified : 26/02/2021
*/
#include "processes.h"

/****************************************************************************************/
/*  I : process structure to use                                                        */
/*  P : Execute the file reading process :                                              */
/*			- read a text file character by character                                   */
/*          - push each character into the first FIFO queue                             */  
/*  O : NULL if ok                                                                      */
/*     -1 (as a void pointer) if error                                                  */
/****************************************************************************************/
void *readproc_handler(void *proc){
    readproc_t* process = (readproc_t*)proc;
    FILE* pfile = NULL;
    int* ret = NULL;
    char buf = '0';

    fprintf(stdout, "Reading file %s\n", process->filename);

    //open the file to read
    pfile = fopen(process->filename, "r");
    if(!pfile){
        fprintf(stderr, "readproc_handler : %s\n", strerror(errno));
        ret = (int*)malloc(sizeof(int));
        *ret = -1;
        pthread_exit(ret);
    }
    
    //read the file character by character and push all the characters read in
    //  the FIFO queue
    do{
        buf = (char)fgetc(pfile);
        fifo_push(process->fifo, (void*)(&buf));
    }while(buf != EOF);
    fclose(pfile);
	
	pthread_exit(ret);
}

/****************************************************************************************/
/*  I : process structure to use                                                        */
/*  P : Execute the file reading process :                                              */
/*			- read a text file character by character                                   */
/*          - push each character into the first FIFO queue                             */  
/*  O : NULL                                                                            */
/****************************************************************************************/
void *calcproc_handler(void *proc){
    calcproc_t* process = (calcproc_t*)proc;
    char* buf = NULL, output = '0';

    //pop one character at a time and push its upper case in the second FIFO
    //  until reaching EOF
    do{
        buf = fifo_pop(process->readfifo);
        output = toupper(*buf);
        free(buf);
        fifo_push(process->dispfifo, &output);
        fprintf(stdout, "%c\n", output);
    }while(tolower(output) != EOF);

	pthread_exit(NULL);
}