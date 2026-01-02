TOP		= ..
include		$(TOP)/Makefile.rules

ARCH = $(TOP)/liblizards.a

LUA_CFLAGS = -I$(TOP)/lua-dist/include
LUA_LIBS = -L$(TOP)/lua-dist/lib

CXXFLAGS = -g -Wall -fPIC -I$(TOP) -Iinclude $(CAIRO_CFLAGS) $(RSVG_CFLAGS) $(GLIB_CFLAGS) $(JSON_CFLAGS) $(CAIROMM_CFLAGS) $(RSVG_CFLAGS) $(LUA_CFLAGS)
LDLIBS	=  $(LUA_LIBS) -llua $(RSVG_LIBS) $(GLIB_LIBS) $(XML_LIBS) $(CAIROMM_LIBS) $(CAIRO_LIBS) $(JSON_LIBS) -lfmt

all	: lizards.so lizards-lua

lizards-lua: lizards-lua.o lua_lizards_wrap.o $(ARCH)
	$(CXX) $^ $(LDLIBS) -o $@

lizards-lua.o: lizards-lua.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

lua_lizards_wrap.o : lua_lizards_wrap.cxx
	$(CXX) $(CXXFLAGS) -c $^ -o $@

lua_lizards_wrap.cxx : $(TOP)/lizards.swg $(TOP)/lizards.h
	swig -c++ -lua -o $@ $<

lizards.so : lua_lizards_wrap.o $(ARCH)
	$(CXX) -shared $^ $(LDLIBS) -o $@
