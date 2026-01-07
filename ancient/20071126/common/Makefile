# Generated automatically from Makefile.in by configure.
DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic

CFLAGS = $(DEBUG) $(DEFINES) 
LIBOBJS = filefunc.o lizgame.o lizmisc.o strgfunc.o lizglobs.o newfuncs.o
COMPAT = compat.o

liblizards.a: $(LIBOBJS) $(COMPAT)
	ar cr liblizards.a $(LIBOBJS) $(COMPAT)
	ranlib liblizards.a

clean:
	rm -f *~ *.o *.a
