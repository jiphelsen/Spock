#include "const.h"
#include "untrusted_lib.h"
#include "enclave.h"

#include <stdint.h>
#include <stddef.h>
#include "shared_mem_lib.h"
#include "hacl-c/Hacl_HMAC_SHA2_256.h"

extern int u_SP_init();
extern int u_enclave_init_r(void* text_start, int text_sz, int data_sz,int SP_id);
extern int u_enclave_init_f(void* copy_start,void* text_start, int text_sz,void* data_start, int data_sz,int SP_id);
extern int u_run_enclave(int id,int nb1,int nb2);
extern void u_attest_enclave(int id);
extern int u_unprotect(int id);
extern void enclave1(int*test);
extern void enclave2(int*test);
extern int get_res();
extern char enclave1_data;
extern char enclave2_data;

void UNTRUSTED_SECTION uart_putc(char c) {
  while (!UART0_FF_THR_EMPTY);            // Wait until the FIFO holding register is empty
  UART0_DR = c;                           // Write character to transmitter register
}

void  UNTRUSTED_SECTION uart_puts(const char *str) {
  while (*str) {                          // Loop until value at string pointer is zero
    uart_putc(*str++);                    // Write the character and increment pointer
  }
}

void UNTRUSTED_SECTION dump_hex(char *str, uint8_t* buf, size_t length) {
    uart_puts(str);
    uart_puts(": ");
    for (int i = 0; i < length; i++)
    {
        uint8_t byte = buf[i];
        uart_putc((byte >> 4) < 10 ? (byte >> 4) + '0' : (byte >> 4) - 10 + 'A');
        uart_putc((byte & 0x0F) < 10 ? (byte & 0x0F) + '0' : (byte & 0x0F) - 10 + 'A');
    }
    uart_puts("\n");
}

//UNTRUSTED "OS"
void UNTRUSTED_SECTION main(){


    int SP_id = u_SP_init();
    u_print("The sp id is : ");
    u_print_int(SP_id);

    void* data_addr1 = (void*)&enclave1_data;

    // for now attest will always return 0x4242
    int MAC_correct = 0x4242;
    // this will fail because attest is hard coded to 0x4242
    int MAC_false = 454;
    int enclave_id1 = u_enclave_init_f(enclave1,(void *)( BUFFERS_START + 0x10), 0x1000,(void *)( BUFFERS_START + 0x2000),0x20 ,1);
    int nb1 = 2;
    int nb2 = 40;
    u_run_enclave(enclave_id1,nb1,nb2);
    u_print("hey!");
    
    int res = get_res(0);
    u_print("response  should be 42 : ");
    u_print_int(res);
    u_print(".");

    /*if(!attest_enclave(enclave_id1,0x4242)){
        print("the enclave is not what you think it is! (terrible screams) \n\r");
    }*/

    void* data_addr2 = (void*)&enclave2_data;
    int enclave_id2 = u_enclave_init_r(enclave2 ,0x1000,0,1);
    nb1 = 1234642;
    nb2 = 321;
    u_run_enclave(enclave_id2,nb1,nb2);
    res = get_res(0);
    u_print("response should be 1234321 : ");
    u_print_int(res);
    u_print(".");

    u_attest_enclave(enclave_id1);

    u_print("result : ");
    for(int i =0;i<7;i++){
        u_print_int(get_res(i*8));
    }
    u_print("\n\r.");

    u_unprotect(enclave_id2);
    
    u_print(" finished");
    while (1);
    return;
    
}
