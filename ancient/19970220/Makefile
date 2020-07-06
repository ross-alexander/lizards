DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic

DEFINES = -DLZ_DEFAULT_DATA=\"DATA\"
CFLAGS = $(DEBUG) $(DEFINES) -Iliblizards -Icompat
TEMPLATE = template.o
FILES = worldinit worldffro worldfill worldpop worldfix \
tordbord terrmast bordshuffle worldopen worldmast worldshut \
clanreport clanrank messapnd clanview \
psmapper \
worlddump

LIBLIZARDS = liblizards/liblizards.a

COMPAT = compat/compat.o
CURSES = compat/curses.o
CURSESLIB = -lncurses

all: $(FILES)

lizmenu: lizmenu.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

lizmenu.o: lizmenu.c compat/cio.h compat/compat.h

worldinit: worldinit.o $(LIBLIZARDS)

worldffro: worldffro.o $(LIBLIZARDS) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

worldfill: worldfill.o $(LIBLIZARDS)

worldfix: worldfix.o $(LIBLIZARDS)

worldpop: worldpop.o $(LIBLIZARDS) -lm

orderent: orderent.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

tordbord: tordbord.o $(LIBLIZARDS)

txt2binord: txt2binord.o lzcc.o $(LIBLIZARDS)

terrmast: terrmast.o $(LIBLIZARDS)

bordshuffle: bordshuffle.o $(LIBLIZARDS)

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

worlddump: worlddump.o $(LIBLIZARDS)

$(LIBLIZARDS):
	cd liblizards; make	

clean:
	rm -f *~ *.o *.a *.log $(FILES)
	cd liblizards; make clean
