.section .enclave2 , "ax"
.global enclave2
# a0 contains the test address
enclave2:

    #lb a0, (a0) #this should give mcause 5
    mv t0,a0
    mv t1,a1 

    li a0,0x65 
	li a1, 0x10000000
	sb a0, (a1) 

    li a0,50
	sb a0, (a1) 

    li a0,32
	sb a0, (a1) 
    
    #asking to be unprotected : 
    #li a7,20

    #response 
    sub a0,t0,t1
    li a1,0
    li a2,0
    li a3,0
    li a4,0
    li a5,0
    li a6,0
    li a7,0
    
    ecall
