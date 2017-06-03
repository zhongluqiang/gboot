objs := button.o interrupt.o led.o mmu.o nand.o uart.o dma.o dm9000.o lcd.o image.o

all : $(objs)

%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o		
	
