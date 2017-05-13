CFLAGS +=-DHOME=\"${HOME}\" -Wall -std=c99 -pedantic

kapan: kapan.o

kapan.o: kapan.c config.h

clean:
	rm -f *.o
