//const are seperate here this should never be included, types.h should be included instead!

#ifndef CONST_H
#define CONST_H

#include <stdint.h>
#include <stddef.h>

#define PMP_ENTRY_COUNT 16 // hardware depended!
#define SM_SECTION __attribute__((section(".sm")))
#define UNTRUSTED_SECTION __attribute__((section(".untrusted_text")))
#define MAX_SP_LIST 10
#define MAX_BUFFERS 40
#define MAX_ENCLAVES 10
#define MAX_ACTIVE_BUFFERS 7 // IMPORTANT should be adapted to number of pmp registers!!!!
#define MAX_ENCLAVE_BUFFER (MAX_ENCLAVES * MAX_ACTIVE_BUFFERS) //stay off
#define SPACE_LIST_MAX 100 //basically as much as ever needed
#define BUFFERS_START 0x80500000
#define END 0x80505000    //todel make dynamic
#define UART_BASE_ADDR 0x10000000
#define UART0_BASE UART_BASE_ADDR
#define REG(base, offset) ((*((volatile unsigned char *)(base + offset))))
#define UART0_DR    REG(UART0_BASE, 0x00)
#define UART0_FCR   REG(UART0_BASE, 0x02)
#define UART0_LSR   REG(UART0_BASE, 0x05)
#define UARTFCR_FFENA 0x01                // UART FIFO Control Register enable bit
#define UARTLSR_THRE 0x20                 // UART Line Status Register Transmit Hold Register Empty bit
#define UART0_FF_THR_EMPTY (UART0_LSR & UARTLSR_THRE)
#define MSG_LEN         13
#define KEY_LEN         64
#define MAC_LEN         32

#endif

// some sanity checks DO NOT CHANGE THESE!! even if the code still runs you are removing vital safety!
#if MAX_SP_LIST <= 0
#error "MAX_SP_LIST must at least be 1."
#endif

#if MAX_BUFFERS <= 0
#error "MAX_BUFFERS must at least be 1."
#endif

#if MAX_ENCLAVES <= 0
#error "MAX_ENCLAVES must at least be 1."
#endif

#if MAX_ACTIVE_BUFFERS <= 0
#error "MAX_ACTIVE_BUFFERS must at least be 1."
#endif

#if MAX_BUFFERS > SPACE_LIST_MAX+2
#error "MAX_BUFFERS must be at least 2 less then SPACE_LIST_MAX"
#endif

#if MAX_ACTIVE_BUFFERS > MAX_BUFFERS
#error "MAX_ACTIVE_BUFFERS cannot exceed MAX_BUFFERS"
#endif

#if MAX_ENCLAVES > MAX_BUFFERS
#error "MAX_ENCLAVES cannot exceed MAX_BUFFERS"
#endif

#if BUFFERS_START + 0x400 >= END
#error "BUFFERS_START must be at least 1KB larger then END"
#endif

#if PMP_ENTRY_COUNT < MAX_ACTIVE_BUFFERS*2 +1
#error "MAX_ACTIVE_BUFFERS needs to be less then the PMP_ENTRY_COUNT/2 -1."
#endif

#if PMP_ENTRY_COUNT > 64
#error "I'm sorry, Dave. I'm afraid I can't let you exceed 64 PMP entries."
#endif