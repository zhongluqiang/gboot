OBJS := start.o mem.o main.o
OBJS += lib/lib.o
OBJS += dev/dev.o

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
	make -C dev all
	
clean:
	rm -fr *.o *.elf *.bin
	make -C lib clean
	make -C dev clean