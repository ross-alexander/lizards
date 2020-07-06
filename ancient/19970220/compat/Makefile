DEBUG = -g -Wstrict-prototypes -Wmissing-prototypes
CFLAGS = $(DEBUG)

all: compat.o curses.o

compat.o: compat.c compat.h

curses.o: curses.c cio.h
