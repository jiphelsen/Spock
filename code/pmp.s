	.global r_enclave
    .global r_untrusted
    .global zero_mem
    .global set_resp_byte
    .global get_resp_byte
	.global _trap_handler
    .global set_untrusted_pmp
	.extern _sm_start
	.extern _sm_end
	.extern _enclave_start
	.extern _enclave_end
	.extern _sm_ret
    .extern enclave2
    .extern _call_sp_safe
    .extern _untrusted_data_start
    .extern _untrusted_data_end
	.extern _end
    .extern main
    .extern sm_call
    .extern untrusted_boot
    .extern _untrusted_text_start
    .extern _untrusted_text_end
    .extern _untrusted_stack_top
    .extern _buffers_end
    .extern make_ecall
    .extern set_pmp_tor
    .extern _resp_buffer_start
    .extern _elapsed_time
	.section .sm, "awx"

# Arguments a0 = start, a1 = end
zero_mem:

    #this sanity check was only necesairy when freespace mangament wasnt implementd yet
    la a3,_buffers_end
    bge a3,a1,no_correction
    la a1,_buffers_end
    no_correction: 

    
    
    z_loop: 
    
    beq a0, a1, end_z_loop
        sb x0,(a0)
        addi a0,a0,1
    j z_loop
    end_z_loop: ret

#############################################################################################################################################
#############################################################################################################################################
#############################################################################################################################################
#############################################################################################################################################

# Arguments: a0 = enclave start, a1 = enclave end, 
#            a2 = data start, a3 = data end
#			 a4 = enclave id
#            a5 = perms
#			 a6 - a7 params

r_enclave:
    
    # setting the arugments like they used to be 
    # a0 = enclave start, a1 = enclave end, 
    #			 a2 = enclave id
    #            a3 = perms
    #            a4 = data start
    #            a5 = data end
    #			 a6 - a7 params
    mv t0,a2
    mv t1,a3
    mv t2,a4
    mv t3,a5

    mv a2,t2
    mv a3,t3
    mv a4,t0
    mv a5,t1


    # If the perms are larger then 7 something very bad is happening as rwx = 7
    # this should probably be handled as an access_violation
    li t0,8
    bge a3,t0,access_violation
    #we allow executing the untrusted software at rwx this is a bypass
    j end_bypass
    r_untrusted:
    # a0 = text_section_start, a1 = text_section_end, a2 = data_section_start, a3 = data_section_end 
    #saving a2 and a3
    
    li a2,-2     # setting id = -2 to make clear this isn't an enclave nor a not found exception!
    li a3,7    # setting text perms to 0b111 or rwx, this doesn't matter for untrusted

    la a0,untrusted_boot
    la a1,_untrusted_data_end
    # we just set another pmp entry after the pmp entries we get from r_enclave
    # and then set the arguments as if we are just running an enclave
    # this assumes the data section is after the text_section btw

    end_bypass:

    #saving args
    mv t4,a0
    mv t5,a1
    mv t6,a2
    #a3 is in t3 if data section
    #saving ra for after pmp calls 

    addi sp, sp, -8
    sd ra, 0(sp) 

    # for now opening up the uart : 
    la a0,0x20000000
	li a1,3         
    li a2,0
    call set_pmp_tor


    #protecting sm by default
    la a0, _sm_end
    li a1,0
    li a2,1
    call set_pmp_tor
    
    # protecting all before the enclave
	#a0 contains the start address already
    li a1,0
    li a2,2
    call set_pmp_tor
    
	## unprotecting the enclave
    mv a0,t5    # t5 = a1 = end address
    mv a1,a3    
    li a2,3
    call set_pmp_tor
    
    # post protection isn't required => pmp access fault
    # iF enclave id is -2 it is not in fact an enclave but the untrusted section
    li t0,-2
    #t6 = id
    #UNTRUSTED DATA SECTION PMP
    bne t6, t0,no_data_section  


    #la a0, _untrusted_data_end
    #li a1, 3
    #li a2, 5
    #call set_pmp_tor
    #j end_data_section_pmp
    data_section:
    #preventing enclaves from accecing untrusted
    li t0,0
    beq a4,t0,no_data_section
    beq a5,t0,no_data_section
    
    mv a0, a4
    li a1, 0
    li a2, 4
    call set_pmp_tor

    mv a0, a5
    li a1, 3
    li a2, 5
    call set_pmp_tor
    
    no_data_section:

    la a0, _end
    li a1, 0
    li a2, 4
    call set_pmp_tor

    #restoring ra
    ld ra, 0(sp)    
    addi sp, sp, 8      
    
    #restoring args
    mv a0,t4
    mv a1,t5
    mv a2,t6

    mv a1,a2 #id put in
    mv a2,a4
    #fixing user address
    csrw    mepc, a0


    # a0 : user_address ,a1: enclave_id (optional), a2: parameter
return_to_u_mode:

    #saving ra if running untrusted
    mv t6,ra

    #setting trap
    la      t1, m_trap
    csrw    mtvec, t1
    #switch
	li      t1, 0x1800
    csrc    mstatus, t2
    # dont need to delegate
    li      t1, 0x100
    csrs    medeleg, t1

    #saving enclave_id
    mv t2,a1

	#saving regs
	addi sp, sp, -128
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
    sd t0, 104(sp)
    sd t1, 112(sp)
    sd t2, 120(sp)

	# saving stack pointer 
    la  a0, _sm_end
    sd  sp, (a0)
    
    # setting parameters
    mv a0,a6
    mv a1,a7

	#clearing regs 
	# we dont clear a0 for now because it contains the test param
	# it never would contain any infomration leak anyways as it is a r_enclave param before that
	# x0 dont need to be cleared
    li  x1, 0    # ra
	# sp dont need to be cleared does not contain any info is already saved and otherwise breaks mret
	# note that if the enclave sets sp to 0 or whatever number, nothing happens as it is saved
    li  x3, 0    # gp
    li  x4, 0    # tp
    li  x5, 0    # t0
    li  x6, 0    # t1
    li  x7, 0    # t2
    li  x8, 0    # s0
    li  x9, 0    # s1
    #li  x10, 0   # a0 passing args
    #li  x11, 40   # a1
    #li  x12, 0   # a2
    #li  x13, 0   # a3
    #li  x14, 0   # a4
    #li  x15, 0   # a5
    #li  x16, 0   # a6
    li  x17, 0   # a7 syscall
    li  x18, 0   # s2
    li  x19, 0   # s3
    li  x20, 0   # s4
    li  x21, 0   # s5
    li  x22, 0   # s6
    li  x23, 0   # s7
    li  x24, 0   # s8  
    li  x25, 0   # s9
    li  x26, 0   # s10
    li  x27, 0   # s11
    li  x28, 0   # t3
    li  x29, 0   # t4
    li  x30, 0   # t5
    li  x31, 0   # t6 
    mret

#############################################################################################################################################
#############################################################################################################################################
#############################################################################################################################################
#############################################################################################################################################

m_trap: 
    #saving response
    la t0, _resp_buffer_start
    sd a0, 0(t0)
    sd a1, 8(t0)
    sd a2, 16(t0)
    sd a3, 24(t0)
    sd a4, 32(t0)
    sd a5, 40(t0)
    sd a6, 48(t0)
    # a7 doesn't need to be saved, it is 0 cause no call is made

	# see ABI 
    # t6 is the return address
    mv t6,ra
    #setting sp
    la  sp, _sm_end
    ld   sp, (sp)
    
	#restoring regs
    ld ra, 0(sp)
    ld s0, 8(sp)
    ld s1, 16(sp)
    ld s2, 24(sp)
    ld s3, 32(sp)
    ld s4, 40(sp)
    ld s5, 48(sp)
    ld s6, 56(sp)
    ld s7, 64(sp)
    ld s8, 72(sp)
    ld s9, 80(sp)
    ld s10, 88(sp)
    ld s11, 96(sp)
    ld t0, 104(sp)
    ld t1, 112(sp)
    ld t2, 120(sp)
    addi sp, sp, 128


	# getting mcause
    csrr    t1, mcause
    
	li  t0, 5
	beq t1, t0, access_violation 
    
    
	return_to_sm:
        #t2 contains enclave id
        #see ABI
        #here t regs need to be used in a regs the args are stored
        mv t1,a7
        mv t3,t6 # saving the address to go to
        mv t4,a1
        mv t5,a0
        # here I prepare the args 
        mv a0,a7
        li a1,0
        # iF the id is -2 it is untrusted not enclave
        li t6,-2
        beq t2,t6,not_called_from_enclave
        li a1,1 #indicating it is indeed called from enclave
        not_called_from_enclave:

        #moving all args two regsright
        mv a7,a5
        mv a6,a4
        mv a5,a3
        mv a4,a2
        mv a3,t4
        mv a2,t5
        
        li t0,1
        beq t0,a1,enclave_call
        #saving regs
        li t2,-2
	    addi sp, sp, -128
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
        sd t0, 104(sp)
        sd t1, 112(sp)
        sd t2, 120(sp)
        la  t0, _sm_end
        sd  sp, (t0)

        li t0,0
        bne a0,t0,make_sm_call
        j end_enclave_call

        enclave_call:
        li t0,0
        bne a0,t0,make_enclave_sm_call
        end_enclave_call:
        # returning from enclave

		# indicating nothing needs to happen 
		li a0,-1
        
        ret

make_sm_call:
        addi sp, sp, -16
        sd ra, 0(sp) 
        sd t3, 8(sp)

        #setting args
        addi sp, sp, -96       

        sd a2, 8(sp)            
        sd a2, 0(sp)            

        sd a3, 24(sp)           
        sd a3, 16(sp)           

        sd a4, 40(sp)           
        sd a4, 32(sp)           

        sd a5, 56(sp)           
        sd a5, 48(sp)           


        sd a6, 72(sp)           
        sd a6, 64(sp)  

        sd a7 , 88(sp)           
        sd a7, 80(sp)           
        
        #saving the stack pointer because c can be rather hard to predict
        la  t1, _call_sp_safe
        sd   sp, (t1)
        
        li a1,-2 #not an enclave
        
        call sm_call
        
        li t2,-2
	    addi sp, sp, -128
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
        sd t0, 104(sp)
        sd t1, 112(sp)
        sd t2, 120(sp)
        la  t0, _sm_end
        sd  sp, (t0)

        #retrieving the stack pointer
        la  t1, _call_sp_safe
        ld   sp, (t1)
        
        #fixing the stack
        addi sp, sp, 96 
        
        ld ra, 0(sp)   
        ld t3, 8(sp)
        addi sp, sp, 16

        mv ra,t3
        li  x3, 0    # gp
        li  x4, 0    # tp
        li  x5, 0    # t0
        li  x6, 0    # t1
        li  x7, 0    # t2
        li  x8, 0    # s0
        li  x9, 0    # s1
        #li  x10, 0   # a0 passing args
        li  x11, 0   # a1
        li  x12, 0   # a2
        li  x13, 0   # a3
        li  x14, 0   # a4
        li  x15, 0   # a5
        li  x16, 0   # a6
        li  x17, 0   # a7 syscall
        li  x18, 0   # s2
        li  x19, 0   # s3
        li  x20, 0   # s4
        li  x21, 0   # s5
        li  x22, 0   # s6
        li  x23, 0   # s7
        li  x24, 0   # s8  
        li  x25, 0   # s9
        li  x26, 0   #  s10
        li  x27, 0   # s11
        li  x28, 0   # t3
        li  x29, 0   # t4
        li  x30, 0   # t5
        li  x31, 0   # t6  
        ret


make_enclave_sm_call: 
        li t0,20 
        beq t0,t1,unprotect


        j debug 
        unprotect:
        mv a0,t2
        ret

	j end	

set_untrusted_pmp:
    addi sp, sp, -8
    sd ra, 0(sp) 

    # for now opening up the uart : 
    la a0,0x20000000
	li a1,3         
    li a2,0
    call set_pmp_tor

    #protecting sm by default
    la a0, _sm_end
    li a1,0
    li a2,1
    call set_pmp_tor

    la a0,untrusted_boot
    li a1,0
    li a2,2
    call set_pmp_tor
	
    la a0, _untrusted_data_end
    li a1,7
    li a2,3
    call set_pmp_tor


    la a0, _end
    li a1, 0
    li a2, 4
    call set_pmp_tor

    ld ra, 0(sp)
    addi sp, sp, 8
    ret
j end


	trap_handler:
        addi a0, t1, 48 
		li a1, 0x10000000
		sb a0, (a1) #mcause
	j end

	access_violation:
		addi a0, t1, 48 
		li a1, 0x10000000
		sb a0, (a1) #mcause
		#indicating enclave needs to be unprotected
		mv a0,t2
		ret
	j end

# a0  ofsset
get_resp_byte:
    addi sp, sp, -8
    sd ra, 0(sp) 
    
    mv t1,a0
    la t0, _resp_buffer_start
    add t0,t0,t1
    ld a0,(t0)

    ld ra, 0(sp)
    addi sp, sp, 8
    ret

# a0 offset a1 to set
set_resp_byte: 
    addi sp, sp, -8
    sd ra, 0(sp) 
    
    mv t1,a0
    la t0, _resp_buffer_start
    add t0,t0,t1

    sd a1,(t0)

    ld ra, 0(sp)
    addi sp, sp, 8
    ret

debug: li a0, 0x79	
    li a1, 0x10000000
    sb a0, (a1) 
	j end

debug2:li a0, 0x7a	
    li a1, 0x10000000
    sb a0, (a1) 
	j end

end: j end


