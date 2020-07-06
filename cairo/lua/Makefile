TOP		= ..
include		$(TOP)/Makefile.rules

ARCH = $(TOP)/liblizards.a

LUA_CFLAGS = -I$(TOP)/lua-5.4.0/include
LUA_LIBS = -L$(TOP)/lua-5.4.0/lib

CXXFLAGS = -g -Wall -fPIC -I$(TOP) -Iinclude $(CAIRO_CFLAGS) $(RSVG_CFLAGS) $(GTK_CFLAGS) $(GLIB_CFLAGS) $(JSON_CFLAGS) $(CAIROMM_CFLAGS) $(RSVG_CFLAGS) $(LUA_CFLAGS)
LDLIBS	=  $(LUA_LIBS) -llua $(RSVG_LIBS) $(GTK_LIBS) $(GLIB_LIBS) $(XML_LIBS) $(CAIROMM_LIBS) $(CAIRO_LIBS) $(JSON_LIBS) -lfmt

all	: lizards.so

lua_lizards_wrap.o : lua_lizards_wrap.cxx
	$(CXX) $(CXXFLAGS) -c $^ -o $@

lua_lizards_wrap.cxx : $(TOP)/lizards.swg $(TOP)/lizards.h
	swig -c++ -lua -o $@ $<

lizards.so : lua_lizards_wrap.o $(ARCH)
	$(CXX) -shared $^ $(LDLIBS) -o $@
