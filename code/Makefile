CROSS_COMPILE = riscv64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
LDFLAGS = -T linker.ld  -L../hacl-star/dist/gcc-compatible 
CFLAGS =  -static  -fno-stack-protector  -march=rv64gc -mabi=lp64d -lc -lgcc  -g -o0 -ffreestanding


# Source files
C_SRCS =  sm.c lib.c getters.c delete.c memory.c main.c untrusted_lib.c shared_mem_lib.c
ASM_SRCS = boot.s  pmp.s untrusted_boot.s enclave1.s enclave2.s set_pmp.s untrusted_sm_calls.s


# .o files
C_OBJS = $(C_SRCS:.c=.o)
ASM_OBJS = $(ASM_SRCS:.s=.o)
OBJS =  $(C_OBJS) $(ASM_OBJS)


all: $(OBJS)

# Build HACL* static library
all:   boot


# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<  

# Compile assembly source files
%.o: %.s
	$(CC) $(CFLAGS) -o $@ -c $< 


clean:
	rm -f $(OBJS) boot


boot: $(OBJS)
	$(LD) $(LDFLAGS) -o boot shared_mem_lib.o hacl-hmac.o boot.o untrusted_boot.o  pmp.o sm.o lib.o enclave1.o enclave2.o getters.o delete.o memory.o main.o untrusted_lib.o set_pmp.o untrusted_sm_calls.o





