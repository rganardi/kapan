CFLAGS +=-DHOME=\"${HOME}\" -Wall -pedantic

kapan: kapan.o

kapan.o: kapan.c config.h

clean:
	rm -f *.o
