# kapan
small calendar program

WARNING: kapan is still under heavy development. Right now it works on my machine. Hopefully it will work on more machines soon :D

## installation
get the sources by running

	git clone https://github.com/rganardi/kapan.git

compile and install

	make
	make install

if you want to use an alternate default database (~/.kapandb), do

	CFLAGS+=-DKAPANDB\\\"/path/to/your/database\\\" make
	make install

note the double escaping.

## configuration
there isn't really much to configure. kapan reads the list of acceptable time formats from

	/etc/datemsk

if you want kapan to recognize more formats, add it to this file (see man 3 strptime, man 3 getdate).

## usage
run

	kapan --help

to see usage information


