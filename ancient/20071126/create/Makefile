# Generated automatically from Makefile.in by configure.
DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic

DEFINES = -I../common
CFLAGS = $(DEBUG) $(DEFINES) 
LIBOBJS = init.o fill.o populate.o customstart.o fix.o

libcreate.a: $(LIBOBJS)
	ar cr $@ $(LIBOBJS)
	ranlib $@

clean:
	rm -f *~ *.o *.a
