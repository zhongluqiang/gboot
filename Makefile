OBJS := start.o mem.o main.o mmu.o led.o button.o interrupt.o nand.o uart.o 
OBJS += lib/lib.o

INCLUDE := $(PWD)/include

CFLAGS := -I$(INCLUDE)
CFLAGS += -fno-builtin    #不使用内建函数
export CFLAGS

gboot.bin:gboot.elf
	arm-linux-objcopy -O binary gboot.elf gboot.bin
	
gboot.elf:$(OBJS)
	arm-linux-ld -Tgboot.lds -o gboot.elf $^
	
%.o:%.S
	arm-linux-gcc -g -c $<
	
%.o:%.c
	arm-linux-gcc  $(CFLAGS) -g -c $<
	
lib/lib.o:
	make -C lib all
	
clean:
	rm -fr *.o *.elf *.bin
	make -C lib clean