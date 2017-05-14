KAPANDIR=$(HOME)/.kapan/

CFLAGS+=-DKAPANDIR=\"${KAPANDIR}\" -Wall -pedantic

kapan: kapan.o

kapan.o: kapan.c

install: kapan
	mkdir -p ${KAPANDIR}
	cp ./datemsk ${KAPANDIR}
	install -D kapan /usr/bin/kapan

uninstall:
	rm -f /usr/bin/kapan
	rm -rf ${KAPANDIR}
