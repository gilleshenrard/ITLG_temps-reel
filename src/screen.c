/*
** screen.c
** Library regrouping screen-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 16/03/2021
*/
#include "screen.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

/************************************************************************/
/*  I : buffer for the final message to display                         */
/*      format string of the message                                    */
/*      additional parameters of the message                            */
/*  P : formats the final message to be displayed                       */
/*  O : /                                                               */
/************************************************************************/
void format_output(char* final_msg, char* format, va_list* arg)
{
    time_t timer = {0};
    char buffer[SZLINE] = {0};
    struct tm* tm_info = {0};

    //get current time, and format it in the buffer
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", tm_info);
    strcat(buffer, " -> ");
    strcat(buffer, format);

    vsprintf(final_msg, buffer, *arg);
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message in green                                   */
/*  O : /                                                               */
/************************************************************************/
void print_success(char* msg, ...)
{
    char final_msg[SZLINE] = {0};
    va_list arg;

    va_start(arg, msg);

    format_output(final_msg, msg, &arg);
    fprintf(stdout, "\033[%d;%dm%s\033[0m\n", NORMAL, GREEN, final_msg);

    va_end(arg);
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message in bold red                                */
/*  O : /                                                               */
/************************************************************************/
void print_error(char* msg, ...)
{
    char final_msg[SZLINE] = {0};
    va_list arg;

    va_start(arg, msg);

    format_output(final_msg, msg, &arg);
    fprintf(stderr, "\033[%d;%dm%s\033[0m\n", BOLD, RED, final_msg);

    va_end(arg);
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message without any colour change                  */
/*  O : /                                                               */
/************************************************************************/
void print_neutral(char* msg, ...)
{
    char final_msg[SZLINE] = {0};
    va_list arg;

    va_start(arg, msg);

    format_output(final_msg, msg, &arg);
    fprintf(stdout, "\033[%d;%dm%s\033[0m\n", NORMAL, RESET, final_msg);

    va_end(arg);
}
