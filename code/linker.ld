ENTRY(_start)
MEMORY {
    SM (rwx) : ORIGIN = 0x80000000, LENGTH = 5M
    BUFFER (rwx) : ORIGIN = 0x80000000 + 5M, LENGTH = 1M
    UNTRUSTED (rwx) : ORIGIN = 0x80000000 + 6M, LENGTH = 6M
}

SECTIONS {
    . = 0x80000000; /* Start address of the program see xv6 */
    .sm : ALIGN(4) {
        _sm_start = .;
         *(.sm*) 
        . =  ALIGN(4);
        . = . +0x200000;
        _stack_top = .;
        . = . +0x4;
        _call_sp_safe = .;
        _elapsed_time = . +0x50;
        _sm_end = .+0x100;
         *(.text.hacl-hmac)
         *(.text.shared_mem_lib)


    } > SM AT > SM
    . = ALIGN (CONSTANT (COMMONPAGESIZE));

    .encalve1_space : ALIGN(4) { /* legacy kick out! only buffres_Startr*/
        _buffers_start = .;
        . = ALIGN(4);
        . = . +0x2000;
        enclave1_data = .;
        . = . +0x20;
    } > BUFFER AT > BUFFER


    .enclave2_space : ALIGN(4) { /* legacy kick out! only buffer end*/
        _enclave2_start = .;
        . = _enclave2_start + 0x1000; 
        
        . = ALIGN(4);
        enclave2_data = .;
        . = . +0x20;
        _buffers_end = .;
    } > BUFFER AT > BUFFER
    . = ALIGN (CONSTANT (COMMONPAGESIZE));

    .untrusted_space : ALIGN(4) {
        _untrusted_text_start = .;
        . = ALIGN(4);
        *(.untrusted_text*)
        . = ALIGN(4);
        *(.text.hacl-hmac)
        *(.text.shared_mem_lib)
        . = ALIGN(4);
        . = . + 0x400000;
        _untrusted_text_end = .  ;
        _untrusted_data_start = .;
        . = .+0x10;
        *(.enclave1*)
        *(.enclave2*)
        _untrusted_stack_top = _untrusted_data_start + 0x100000;
        _untrusted_sp_save = _untrusted_stack_top + 0x100;
        _resp_buffer_start = _untrusted_sp_save + 0x10;
        _resp_buffer_end = _resp_buffer_start + 0x80;
        _untrusted_data_end = _resp_buffer_end +0x1000;
    } > UNTRUSTED AT > UNTRUSTED

    

    _end = .;
}
