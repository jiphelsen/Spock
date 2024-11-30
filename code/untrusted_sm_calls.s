.global u_SP_init
.global get_res
.global u_enclave_init_f 
.global u_enclave_init_r
.global u_attest_enclave
.global u_run_enclave
.global u_unprotect
.global continue #for testing only
.extern main
.extern _untrusted_sp_save

.section .untrusted_text, "awx"

#this soly serves to easy the untrusted calls to the sm it is not in any way required or trusted
# sp_init
u_SP_init:

li a7,1
j make_ecall

#================================

# add_enclave realloc allowed
u_enclave_init_r:
li a7,2
j make_ecall
#================================

# add_enclave realloc not allowed or fixed
u_enclave_init_f:
li a7,3
j make_ecall
#================================

# add_enclave
u_run_enclave:
li a7,4
j make_ecall
#================================

u_attest_enclave:
li a7,5
j make_ecall
#================================

# unprotect
u_unprotect:
li a7,20
j make_ecall
#================================

make_ecall:
    addi sp, sp, -112
    sd ra, 0(sp)
    sd s0, 8(sp)
    sd s1, 16(sp)
    sd s2, 24(sp)
    sd s3, 32(sp)
    sd s4, 40(sp)
    sd s5, 48(sp)
    sd s6, 56(sp)
    sd s7, 64(sp)
    sd s8, 72(sp)
    sd s9, 80(sp)
    sd s10, 88(sp)
    sd s11, 96(sp)
    sd t2, 104(sp)
    la  t1, _untrusted_sp_save
    sd   sp, (t1)

    la ra,continue
    ecall
    continue:
    
    la  sp, _untrusted_sp_save
    ld   sp, (sp)
    ld t2, 104(sp)
    ld s11, 96(sp)
    ld s10, 88(sp)
    ld s9, 80(sp)
    ld s8, 72(sp)
    ld s7, 64(sp)
    ld s6, 56(sp)
    ld s5, 48(sp)
    ld s4, 40(sp)
    ld s3, 32(sp)
    ld s2, 24(sp)
    ld s1, 16(sp)
    ld s0, 8(sp)
    ld ra, 0(sp)
    addi sp, sp, 112
    ret 

# not a call but fuck it still assembly
get_res:
    addi sp, sp, -8
    sd ra, 0(sp) 
    
    la t0, _resp_buffer_start
    add t0,t0,a0
    ld a0,(t0)
    

    ld ra, 0(sp)
    addi sp, sp, 8
    ret

debug: li a0, 0x78	
    li a1, 0x10000000
    sb a0, (a1) 
	j end



end: j end
