# Generated automatically from Makefile.in by configure.
DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic -Werror

CFLAGS = $(DEBUG) $(DEFINES) 
LIBOBJS = filefunc.o lizgame.o lizhex.o lizmisc.o strgfunc.o libliz.o
COMPAT = compat.o

liblizards.a: $(LIBOBJS) $(COMPAT)
	ar cr liblizards.a $(LIBOBJS) $(COMPAT)
	ranlib liblizards.a
	
clean:
	rm -f *~ *.o *.a
