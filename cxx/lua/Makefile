TOP		= ..
CXXFLAGS	= `pkg-config --cflags libxml++-2.6 glib-2.0 lua` -I$(TOP)
CXX		= g++ -Wall -g
CC		= g++
LDFLAGS		= -L$(TOP) `pkg-config --libs libxml++-2.6 glib-2.0 lua`
LDLIBS		= $(TOP)/base.o $(TOP)/create.o $(TOP)/player.o $(TOP)/debug.o $(TOP)/misc.o $(TOP)/point.o $(TOP)/feature.o $(TOP)/hex.o $(TOP)/map.o $(TOP)/world.o `pkg-config --libs libxml++-2.6 glib-2.0 lua`

lua	: feature.o player.o map.o hex.o world.o lua.o
