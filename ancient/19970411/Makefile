# Generated automatically from Makefile.in by configure.
MV = mv
YACC = bison -y

DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic

DEFINES = -DLZ_DEFAULT_DATA=\"DATA\"  -DSTDC_HEADERS=1 -DHAVE_ERRNO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC_H=1 -DHAVE_STRING_H=1 
CFLAGS = $(DEBUG) $(DEFINES) -Iliblizards -Icompat

TEMPLATE = template.o

FILES = worldinit worldffro worldfill # worldpop worldfix \
#tordbord terrmast bordshuffle worldopen worldmast worldshut \
#clanreport clanrank messapnd clanview \
#psmapper worlddump lizmenu customstart \
#parse

LIBLIZARDS = liblizards/liblizards.a

CURSES = compat/curses.o
CURSESLIB = -lncurses

all: $(LIBLIZARDS) $(CURSES) $(FILES)

$(LIBLIZARDS):
	make -C liblizards

$(CURSES):
	make -C compat

lizmenu: lizmenu.o $(LIBLIZARDS) $(TEMPLATE) $(CURSES)
	$(CC) $^ -o $@ $(CURSESLIB)

lizmenu.o: lizmenu.c compat/cio.h

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

customstart: customstart.o $(LIBLIZARDS) -lm

grammer.c grammer.h: grammer.y parse.h
	$(YACC) -d grammer.y
	$(MV) y.tab.c grammer.c
	$(MV) y.tab.h grammer.h

lexicon.c: grammer.h lexicon.lex parse.h
	$(LEX) lexicon.lex
	$(MV) lex.yy.c lexicon.c

parse.o: parse.c parse.h

parse: lexicon.o grammer.o parse.o
	$(CC) $^ -lfl -o $@
clean:
	rm -f *~ *.o *.a *.log $(FILES)
	cd liblizards; make clean
