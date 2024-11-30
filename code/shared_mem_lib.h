#ifndef SHARED_MEM_LIB_H
#define SHARED_MEM_LIB_H

#include <stdint.h>
#include <stddef.h>
void* memset(void* ptr, int len, size_t val);
void* memcpy(void* __restrict dst, const void* __restrict src, size_t size);
uint32_t __bswapsi2(uint32_t u);
uint64_t __bswapdi2(uint64_t u);

#endif