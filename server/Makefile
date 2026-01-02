CFLAGS=-g -O2 -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DHAVE_STDIO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_STRINGS_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DHAVE_UNISTD_H=1 -DHAVE_STDIO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_NETDB_H=1 -DHAVE_SIGNAL_H=1 -DHAVE_SYS_TIME_H=1 -Wall
LIBS=
CC=gcc
CXX=g++ -std=c++17
LEX=flex++
ALL=server connect parse

all: $(ALL)

server: server.o

connect: connect.o

parse: node.o lexicon.o main.o
	$(CXX) node.o lexicon.o main.o -o parse

lexicon.cc: lexicon.lex
	$(LEX) lexicon.lex
	mv lex.yy.cc lexicon.cc

clean:
	rm $(ALL) *.o
