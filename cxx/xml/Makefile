CC	= gcc -I/usr/local/include
LDLIBES	= -lxml2

std2xml: token.o parse.tab.o main.o
	$(CC) $(LDFLAGS) $^ $(LDLIBES) -o $@

parse.tab.c parse.tab.h: parse.y
	$(YACC) -t -v -d -b parse parse.y

token.c: token.l parse.tab.h

