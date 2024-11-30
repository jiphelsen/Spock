#include <stdint.h>
#include <stddef.h>

#ifndef MEM_H
#define MEM_H
void* find_and_reserve_mem(SM* sm, int size);
void* reserve_mem(SM* sm, void * start,void * end);
int free_mem(SM* sm, void * start,void * end);
void mem_copy(void* dest, const void* src, int size, void* data_address,int relocatable);
void*  memset(void* ptr, int len, size_t val);
void*  memcpy(void*__restrict dst, const void*__restrict src, size_t size);
uint32_t  __bswapsi2(uint32_t u);
uint64_t  __bswapdi2(uint64_t u);
#endif