CFLAGS=-D_XOPEN_SOURCE=600 -std=c99 `pkg-config --cflags cairo gtk+-2.0`
LDLIBS=`pkg-config --libs cairo gtk+-2.0`

all: cairo final.ps svg.ps ps.svg

cairo: cairo.o

cairo.o: cairo.c symbols.c symbols.h

symbols.c: original.ps ps2cairo.pl
	./ps2cairo.pl < $< > $@

final.ps: defs.ps original.ps show.ps
	cat $^ > $@

svg.ps: ps2svg.ps final.ps
	cat $^ > $@

ps.svg: svg.ps
	gs -sDEVICE=bbox -dNOPAUSE -dBATCH svg.ps
