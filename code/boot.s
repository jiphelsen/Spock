.globl _start
.extern sm_call
.extern set_pmp_tor
.extern _stack_top
.extern _sm_end
.section .sm, "ax"
_start:
    #intializing the stack and the heap
    la sp, _stack_top

    #setting call number to sm intitialization call
    li a0,0
    li a1,0 # this can never be changed is to indicate is not being called from enclave
    
    j sm_call
    end: j end
    