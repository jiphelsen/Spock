#include "const.h"
#include <stdint.h>
#include <stddef.h>

// all of the bellow are both for the SM and Untrusted the linker assures both get a copy

void*  memset(void* ptr, int len, size_t val)
{
  char *buf = (char*) ptr;
  int i = 0;
  for (i=0; i<len; i++)
        buf[i] = val;
  return ptr;
}

void*  memcpy(void*__restrict dst, const void*__restrict src, size_t size)
{
  char *out = (char*) dst;
  char *in = (char*) src;
  int i = 0;
  for (i=0; i<size; i++)
        out[i] = in[i];
  return dst;
}

/* https://github.com/llvm-mirror/compiler-rt/blob/master/lib/builtins/bswapsi2.c */
uint32_t  __bswapsi2(uint32_t u) {
  return ((((u)&0xff000000) >> 24) |
          (((u)&0x00ff0000) >> 8)  |
          (((u)&0x0000ff00) << 8)  |
          (((u)&0x000000ff) << 24));
}

/* https://github.com/llvm-mirror/compiler-rt/blob/master/lib/builtins/bswapdi2.c */
uint64_t  __bswapdi2(uint64_t u) {
  return (
      (((u)&0xff00000000000000ULL) >> 56) |
      (((u)&0x00ff000000000000ULL) >> 40) |
      (((u)&0x0000ff0000000000ULL) >> 24) |
      (((u)&0x000000ff00000000ULL) >> 8)  |
      (((u)&0x00000000ff000000ULL) << 8)  |
      (((u)&0x0000000000ff0000ULL) << 24) |
      (((u)&0x000000000000ff00ULL) << 40) |
      (((u)&0x00000000000000ffULL) << 56));
}
