#ifndef SCREENTH_H_INCLUDED
#define SCREENTH_H_INCLUDED
#include <stdarg.h>

#define RESET   0
#define RED     31
#define GREEN   32
#define YELLOW  33
#define BLUE    34
#define MAGENTA 35
#define CYAN    36

#define NORMAL  0
#define BOLD    1

#define SZLINE  128

void format_output(char* final_msg, char* format, va_list* arg);
void print_success(char* msg, ...);
void print_error(char* msg, ...);
void print_neutral(char* msg, ...);
#endif