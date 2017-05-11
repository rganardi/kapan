CFLAGS +=-DHOME=\"${HOME}\" -Wall

kapan: kapan.o

kapan.o: kapan.c config.h

clean:
	rm -f *.o
