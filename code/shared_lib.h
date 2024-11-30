#include "const.h"
#ifndef S_LIB
#define S_LIB
#define SHARED_LIB(section_name,section_prefix) void __attribute__((section(section_name))) section_prefix##write_uart(char c) { \
        volatile char* uart = (volatile char*)UART_BASE_ADDR; \
        *uart = c; \
    } \
    void __attribute__((section(section_name))) section_prefix##print(char* str) { \
        for (int i = 0; str[i] != '\0'; ++i) { \
            section_prefix##write_uart(str[i]); \
        } \
        section_prefix##write_uart('\0'); \
    } \
    void __attribute__((section(section_name))) section_prefix##test() {\
        section_prefix##write_uart('f');\
    }\
    void __attribute__((section(section_name)))  section_prefix##print_int(int num){\
    if(num<0){ \
        return;\
    }\
    if(num == 0){\
        section_prefix##write_uart(48);\
        return;\
    }\
    int i = 0;\
    int p = 1;\
    int prev_p = 1;\
    while (!((num==0)&&prev_p==1))\
    { \
        for(p=1;p*10 <= num;p = p*10);\
        int copy_p =p;\
        if(prev_p>p*10){for(;prev_p>p*10;p*=10){section_prefix##write_uart(48);}}\
        prev_p = copy_p;\
        p = copy_p;\
        int c=0;\
        for(;num>=p;num = num-p){c++;}\
        section_prefix##write_uart(c+48);\
    }\
    section_prefix##print(" ");\
}\
void __attribute__((section(section_name))) section_prefix##print_hex_digit(unsigned char digit) {\
    if (digit < 10) {\
        section_prefix##write_uart('0' + digit);\
    } else {\
        section_prefix##write_uart('A' + (digit - 10));\
    }\
}\
void __attribute__((section(section_name))) section_prefix##print_address(void * address) {\
    unsigned long addr = (unsigned long)address;\
    section_prefix##write_uart('0');\
    section_prefix##write_uart('x');\
    for (int i = (sizeof(unsigned long) * 2) - 1; i >= 0; --i) {\
        unsigned char digit = (addr >> (i * 4)) & 0xF;\
        section_prefix##print_hex_digit(digit);\
    }\
    section_prefix##print(" ");\
}

#endif


