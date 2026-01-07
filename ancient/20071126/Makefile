# Generated automatically from Makefile.in by configure.
MV = mv

DEBUG = -g -Wall -Wmissing-prototypes

DEFINES = -DLIZ_DEFAULT_DATA=\"data\"  -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DHAVE_GETOPT_H=1 -DHAVE_ERRNO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC_H=1 -DHAVE_STRING_H=1 
CFLAGS = $(DEBUG) $(DEFINES) -Icommon -Icreate

FILES = worldinit worldturn showhex pscenprn

LIBLIZARDS = common/liblizards.a
LIBTURN = libturn/libturn.a
CREATE = create/libcreate.a

all:
	make -C common
	make -C create
	make -C libturn
	make files

files: $(FILES)

worldinit: worldinit.o $(CREATE)  $(LIBLIZARDS) -lm

worldturn: worldturn.o $(LIBTURN) $(LIBLIZARDS)

runturn: runturn.o $(LIBLIZARDS)

pscenprn: pscenprn.o $(LIBLIZARDS)

worldlisp: worldlisp.o $(LIBLIZARDS)

showhex: showhex.o $(LIBLIZARDS)

clean:
	rm -f *~ *.o *.a *.log $(FILES)
	cd common; make clean
	cd libturn; make clean
	cd create; make clean
	cd decode; make clean
