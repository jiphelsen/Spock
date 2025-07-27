#include <stdint.h>

#define UART_TX_FIFO 0x60000000 + 0x00000010

void put_char(char c) {
    volatile uint32_t* uart_fifo = (uint32_t*)UART_TX_FIFO;
    *uart_fifo = c;
}

void print(const char* str) {
    while (*str) put_char(*str++);
}

void app_main(void) {
    print("Hello, world!\n");
    while (1);
}
