# Generated automatically from Makefile.in by configure.
MV = mv
YACC = bison -y
LIBLEX = -lfl

DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic -Werror

DEFINES = -DLZ_DEFAULT_DATA=\"DATA\"  -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DHAVE_GETOPT_H=1 -DHAVE_ERRNO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC_H=1 -DHAVE_STRING_H=1 
CFLAGS = $(DEBUG) $(DEFINES) -Iliblizards -Icompat

TEMPLATE = template.o

FILES = worldlisp worldinit worldffro worldturn runturn pscenprn worlddump

LIBLIZARDS = liblizards/liblizards.a
LIBTURN = libturn/libturn.a

CURSES = compat/curses.o
CURSESLIB = -lncurses

all: $(LIBLIZARDS) $(CURSES) $(FILES)

$(LIBLIZARDS):
	make -C liblizards

$(LIBTURN):
	make -C libturn

$(CURSES):
	make -C compat

lizmenu: lizmenu.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

lizmenu.o: lizmenu.c compat/cio.h

worldinit: worldfill.o worldpop.o customstart.o worldfix.o worldinit.o $(LIBLIZARDS) -lm

worldffro: worldffro.o $(LIBLIZARDS) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

worldfill: worldfill.o $(LIBLIZARDS)

worldfix: worldfix.o $(LIBLIZARDS)

worldpop: worldpop.o $(LIBLIZARDS) -lm

orderent: orderent.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

worldturn: worldturn.o $(LIBTURN) $(LIBLIZARDS)

customstart: customstart.o $(LIBLIZARDS) -lm

jdump: jdump.o $(LIBLIZARDS)

runturn: runturn.o $(LIBLIZARDS)

pscenprn: pscenprn.o $(LIBLIZARDS)

worlddump: worlddump.o $(LIBLIZARDS)

worldlisp: worldlisp.o $(LIBLIZARDS)

clean:
	rm -f *~ *.o *.a *.log $(FILES)
	cd liblizards; make clean
	cd libturn; make clean
	cd decode; make clean
