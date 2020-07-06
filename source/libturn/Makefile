# Generated automatically from Makefile.in by configure.
DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic

DEFINES = -I. -I../common

CFLAGS = $(DEBUG) $(DEFINES) 
LIBOBJS = lizord.o lizmesg.o moreorders.o chants.o blddest.o \
terrain.o tordbord.o terrmast.o bordshuffle.o worldopen.o worldmast.o \
worldshut.o clanreport.o clanrank.o messapnd.o \
combat.o \
moveorder.o sneakorder.o splitorder.o recruitorder.o

libturn.a: $(LIBOBJS)
	ar cr libturn.a $(LIBOBJS)
	ranlib libturn.a

clean:
	rm -f *~ *.o *.a
