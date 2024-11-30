.section .enclave1, "ax"
.global enclave1
.extern enclave1_data
enclave1: 
    #lb a0, (a0) #this should give mcause 5
    mv t0,a0
    mv t1,a1 

    li a0,0x65 
	li a1, 0x10000000
	sb a0, (a1) 

    li a0,49
	sb a0, (a1) 

    li a0,32
	sb a0, (a1) 

    #calculating and writing to data section
    add a0,t0,t1
    la a3, enclave1_data
    sd a0,(a3)


    #response 
    # loading from data section
    la t1, enclave1_data
    sd a0,(t1)

    li a1,0
    li a2,0
    li a3,0
    li a4,0
    li a5,0
    li a6,0
    li a7,0

    ecall
