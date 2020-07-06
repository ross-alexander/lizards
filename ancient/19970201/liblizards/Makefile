DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -ansi -pedantic

DEFINES = -DLZ_DEFAULT_DATA=\"DATA\"
CFLAGS = $(DEBUG) $(DEFINES) -I.. -Icompat
LIBOBJS = filefunc.o lizgame.o lizord.o lizhex.o lizmisc.o lizmesg.o \
strgfunc.o orders.o moreorders.o chants.o blddest.o terrain.o libliz.o
COMPAT = ../compat/compat.o

liblizards.a: $(LIBOBJS) $(COMPAT)
	ar cr liblizards.a $(LIBOBJS) $(COMPAT)
	ranlib liblizards.a
	
clean:
	rm -f *~ *.o *.a
