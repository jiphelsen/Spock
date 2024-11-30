.global set_pmp_tor
.section .sm, "awx"


# WARNING : THIS FUNCTION WRITES DESTRUCTIVELY IF (PMP NB + 1)%4 = 0
# the reason we do this is to easily reset regs, in our workflow pmp regs need to be reset when written to so it makes perfect sense
# chaning this could result in memory regions beeing unprotected that should be protected!
# WARNING : CAN ONLY BE CALLED FROM M-MODE
# a0 = address a1 = perms a2 = pmp_reg_nb
# the args are not secured this function is protected and dev so no idiot proofing required (technically)
set_pmp_tor:
    #protecting some regs
    addi sp, sp, -16    
    sd t0, 0(sp)         
    sd t1, 8(sp)                      
    
    #pmp address
    mv 		t1, a0
	srl 	t1, t1, 2
    
    beq a2, x0, set_pmpaddr0
    li t0, 1
    beq a2, t0, set_pmpaddr1
    li t0, 2
    beq a2, t0, set_pmpaddr2
    li t0, 3
    beq a2, t0, set_pmpaddr3
    li t0, 4
    beq a2, t0, set_pmpaddr4
    li t0, 5
    beq a2, t0, set_pmpaddr5
    li t0, 6
    beq a2, t0, set_pmpaddr6
    li t0, 7
    beq a2, t0, set_pmpaddr7
    li t0, 8
    beq a2, t0, set_pmpaddr8
    li t0, 9
    beq a2, t0, set_pmpaddr9
    li t0, 10
    beq a2, t0, set_pmpaddr10
    li t0, 11
    beq a2, t0, set_pmpaddr11
    li t0, 12
    beq a2, t0, set_pmpaddr12
    li t0, 13
    beq a2, t0, set_pmpaddr13
    li t0, 14
    beq a2, t0, set_pmpaddr14
    j set_pmpaddr15

set_pmpaddr0:
    csrw pmpaddr0, t1
    j end_write_addr

set_pmpaddr1:
    csrw pmpaddr1, t1
    j end_write_addr

set_pmpaddr2:
    csrw pmpaddr2, t1
    j end_write_addr

set_pmpaddr3:

    csrw pmpaddr3, t1
    j end_write_addr

set_pmpaddr4:
    csrw pmpaddr4, t1
    j end_write_addr

set_pmpaddr5:
    csrw pmpaddr5, t1
    j end_write_addr

set_pmpaddr6:
    csrw pmpaddr6, t1
    j end_write_addr

set_pmpaddr7:
    csrw pmpaddr7, t1
    j end_write_addr

set_pmpaddr8:
    csrw pmpaddr8, t1
    j end_write_addr

set_pmpaddr9:
    csrw pmpaddr9, t1
    j end_write_addr

set_pmpaddr10:
    csrw pmpaddr10, t1
    j end_write_addr

set_pmpaddr11:
    csrw pmpaddr11, t1
    j end_write_addr

set_pmpaddr12:
    csrw pmpaddr12, t1
    j end_write_addr

set_pmpaddr13:
    csrw pmpaddr13, t1
    j end_write_addr

set_pmpaddr14:
    csrw pmpaddr14, t1
    j end_write_addr

set_pmpaddr15:
    csrw pmpaddr15, t1
    j end_write_addr

end_write_addr:


    #pmp config 
    mv      t1, a1  
    addi    t1,t1,8 #setting TOR

    #loading the CSR address 
    
    beq a2, zero, set_pmpcfg0
    li t0, 1
    beq a2, t0, set_pmpcfg1
    li t0, 2
    beq a2, t0, set_pmpcfg2
    li t0, 3
    beq a2, t0, set_pmpcfg3
    li t0, 4
    beq a2, t0, set_pmpcfg4
    li t0, 5
    beq a2, t0, set_pmpcfg5
    li t0, 6
    beq a2, t0, set_pmpcfg6
    li t0, 7
    beq a2, t0, set_pmpcfg7
    li t0, 8
    beq a2, t0, set_pmpcfg8
    li t0, 9
    beq a2, t0, set_pmpcfg9
    li t0, 10
    beq a2, t0, set_pmpcfg10
    li t0, 11
    beq a2, t0, set_pmpcfg11
    li t0, 12
    beq a2, t0, set_pmpcfg12
    li t0, 13
    beq a2, t0, set_pmpcfg13
    li t0, 14
    beq a2, t0, set_pmpcfg14
    j set_pmpcfg15

set_pmpcfg0:
    csrw pmpcfg0, t1
    j end_write_cfg

set_pmpcfg1:
    slli t1, t1, 8
    csrs pmpcfg0, t1
    j end_write_cfg

set_pmpcfg2:
    slli t1, t1, 16
    csrs pmpcfg0, t1
    j end_write_cfg

set_pmpcfg3:
    slli t1, t1, 24
    csrs pmpcfg0, t1
    j end_write_cfg

set_pmpcfg4:
    csrw pmpcfg1, t1
    j end_write_cfg

set_pmpcfg5:
    slli t1, t1, 8
    csrs pmpcfg1, t1
    j end_write_cfg

set_pmpcfg6:
    slli t1, t1, 16
    csrs pmpcfg1, t1
    j end_write_cfg

set_pmpcfg7:
    slli t1, t1, 24
    csrs pmpcfg1, t1
    j end_write_cfg

set_pmpcfg8:
    csrw pmpcfg2, t1
    j end_write_cfg

set_pmpcfg9:
    slli t1, t1, 8
    csrs pmpcfg2, t1
    j end_write_cfg

set_pmpcfg10:
    slli t1, t1, 16
    csrs pmpcfg2, t1
    j end_write_cfg

set_pmpcfg11:
    slli t1, t1, 24
    csrs pmpcfg2, t1
    j end_write_cfg

set_pmpcfg12:
    csrw pmpcfg3, t1
    j end_write_cfg

set_pmpcfg13:
    slli t1, t1, 8
    csrs pmpcfg3, t1
    j end_write_cfg

set_pmpcfg14:
    slli t1, t1, 16
    csrs pmpcfg3, t1
    j end_write_cfg

set_pmpcfg15:
    slli t1, t1, 24
    csrs pmpcfg3, t1
    j end_write_cfg
end_write_cfg:


    #restoring regs 
    ld t0, 0(sp)         
    ld t1, 8(sp)                    
    addi sp, sp,16

    ret              


