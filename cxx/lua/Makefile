TOP		= ..
CXXFLAGS	= `pkg-config --cflags libxml++-3.0 glib-2.0 lua-5.4` -I$(TOP)
CXX		= g++ -Wall -g -std=c++17
CC		= g++
LDFLAGS		= -L$(TOP) 
LDLIBS		= $(TOP)/base.o $(TOP)/create.o $(TOP)/player.o $(TOP)/debug.o $(TOP)/misc.o $(TOP)/point.o $(TOP)/feature.o $(TOP)/hex.o $(TOP)/map.o $(TOP)/world.o `pkg-config --libs libxml++-3.0 glib-2.0 lua-5.4`

lua	: feature.o player.o map.o hex.o world.o lua.o
