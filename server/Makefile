# Generated automatically from Makefile.in by configure.
CFLAGS=-g -O2  -DSTDC_HEADERS=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DHAVE_UNISTD_H=1 -DHAVE_STDIO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_NETDB_H=1 -DHAVE_SIGNAL_H=1 -DHAVE_SYS_TIME_H=1  -Wall
LIBS=
CC=gcc
LEX=flex++

all: server connect parse

server: server.o

connect: connect.o

parse: node.o lexicon.o main.o
	$(CXX) node.o lexicon.o main.o -lfl -o parse

lexicon.cc: lexicon.lex
	$(LEX) lexicon.lex
	mv lex.yy.cc lexicon.cc
