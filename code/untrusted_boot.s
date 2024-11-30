.globl untrusted_boot
.extern main
.extern _untrusted_stack_top
.section .untrusted_text, "ax"
untrusted_boot: 
    la sp, _untrusted_stack_top

    call main

    ecall
