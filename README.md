# kapan
small calendar program

WARNING: kapan is still under heavy development. Right now it works on my machine. Hopefully it will work on more machines soon :D

## installation
get the sources by running

	git clone https://github.com/rganardi/kapan.git

tell kapan where to store its settings and database by changing the variable KAPANDIR in Makefile. The default directory is ~/.kapan

compile and install

	make
	make install

## configuration
There isn't really much to configure. kapan reads the list of acceptable time formats in KAPANDIR/datemsk. If you want kapan to recognize more formats, add it to this file (see man 3 strptime, man 3 getdate).

## usage
run

	kapan --help

to see usage information


