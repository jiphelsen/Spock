#include "types.h"
#ifndef LIB_H
#define LIB_H
void write_uart(char c);
void print(char* str);
void print_int(int i);
void print_address(void* addr);
void print_space(SM * sm);
void print_mem(void * start, void * end, char * str);
#endif