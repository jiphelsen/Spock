CROSS_COMPILE = riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -march=rv32imc -mabi=ilp32 -nostdlib -Wall -O0 -ffreestanding
LDFLAGS = -T linker.ld

C_SRCS = main.c
ASM_SRCS = start.S

C_OBJS = $(C_SRCS:.c=.o)
ASM_OBJS = $(ASM_SRCS:.S=.o)
OBJS = $(C_OBJS) $(ASM_OBJS)

TARGET_ELF = esptest.elf
TARGET_BIN = esptest.bin

all: $(TARGET_BIN)

#use gcc linking!!!
$(TARGET_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.bin
