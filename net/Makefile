objs := arp.o tftp.o net.o

all : $(objs)

%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o		
	
