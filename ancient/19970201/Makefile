DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -ansi -pedantic

DEFINES = -DLZ_DEFAULT_DATA=\"DATA\"
CFLAGS = $(DEBUG) $(DEFINES) -Iliblizards -Icompat
CXXFLAGS = -I/usr/lib/g++-include $(CFLAGS)
TEMPLATE = template.o
FILES = lizmenu worldinit worldffro worldfill worldpop worldfix \
orderent \
tordbord terrmast bordshuf worldopen worldmast worldshut \
clanreport clanrank messapnd clanview \
psmapper psplypage pscenprn psprint psworldopen

LIBLIZARDS = liblizards/liblizards.a

COMPAT = compat/compat.o
CURSES = compat/curses.o -lncurses

all: $(FILES)

lizmenu: lizmenu.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)
	$(CC) $^ -o lizmenu

lizmenu.o: lizmenu.c compat/cio.h compat/compat.h

worldinit: worldinit.o $(LIBLIZARDS)

worldffro: worldffro.o $(LIBLIZARDS) $(CURSES)

worldfill: worldfill.o $(LIBLIZARDS)

worldfix: worldfix.o $(LIBLIZARDS)

worldpop: worldpop.o $(LIBLIZARDS) -lm

orderent: orderent.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)

tordbord: tordbord.o $(LIBLIZARDS)

txt2binord: txt2binord.o lzcc.o $(LIBLIZARDS)

terrmast: terrmast.o $(LIBLIZARDS)

bordshuf: bordshuf.o $(LIBLIZARDS)

boardshuffle: boardshuffle.o lzcc.o $(LIBLIZARDS)

worldopen: worldopen.o $(LIBLIZARDS)

psworldopen: psworldopen.o $(LIBLIZARDS)

worldmast: worldmast.o $(LIBLIZARDS)

worldshut: worldshut.o $(LIBLIZARDS)

clanreport: clanreport.o $(LIBLIZARDS)

clanrank: clanrank.o $(LIBLIZARDS)

messapnd: messapnd.o $(LIBLIZARDS)

clanview: clanview.o $(LIBLIZARDS)

psmapper: psmapper.o $(LIBLIZARDS)

psplypage: psplypage.o $(LIBLIZARDS)

pscenprn: pscenprn.o $(LIBLIZARDS)

psprint: psprint.o $(LIBLIZARDS)

$(LIBLIZARDS):
	cd liblizards
	make	
clean:
	rm -f *~ *.o *.a $(FILES)
