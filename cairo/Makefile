include Makefile.rules

CXXFLAGS = -g -Wall -std=c++17 -fPIC $(CAIROMM_CFLAGS) $(RSVG_CFLAGS) $(GTK_CFLAGS) $(GLIB_CFLAGS) $(JSON_CFLAGS)
LDLIBS	= $(RSVG_LIBS) $(GTK_LIBS) $(GLIB_LIBS) $(CAIROMM_LIBS) $(JSON_LIBS) -lfmt

OBJECTS = misc.o point.o feature.o hex.o output.o serial.o overlay.o game.o grid.o layout.o layouts.o template.o

all: liblizards.a lizards lua/lizards.so

lua/lizards.so : liblizards.a
	make -C lua

liblizards.a: $(OBJECTS)
	$(AR) r $@ $^


Test0001.svg: lizards defs.svg
	./lizards -osvg

lizards: $(OBJECTS) main.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

lizards.o: lizards.cc lizards.h

feature.o: feature.cc lizards.h

hex.o: hex.cc lizards.h

point.o: point.cc lizards.h

map.o: map.cc lizards.h

overlay.o: overlay.cc lizards.h

create.o: create.cc lizards.h

game.o: game.cc lizards.h

grid.o: grid.cc lizards.h

output.o: output.cc lizards.h

layout.o: layout.cc lizards.h

layouts.o: layouts.cc lizards.h

clean:
	rm -f lizards $(OBJECTS) *~
