CFLAGS=-g -DHAVE_CONFIG_H

SHARED=string.so list.so province.so feature.so world.so player.so border.so

all: lizards $(SHARED)

lizards: hash.o y.tab.o lex.yy.o main.o
	$(CC) -rdynamic hash.o y.tab.o lex.yy.o main.o -ldl -o lizards

lex.yy.c: tokenize.l
	$(LEX) tokenize.l

y.tab.c y.tab.h: parse.y
	$(YACC) -d parse.y

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c -o hash.o

province.so: province.o
	$(CC) -shared province.o -o province.so

province.o: shared.h province.c
	$(CC) $(CFLAGS) -fpic -c province.c -o province.o

feature.so: feature.o
	$(CC) -shared feature.o -o feature.so

feature.o: shared.h feature.c
	$(CC) $(CFLAGS) -fpic -c feature.c -o feature.o

world.so: world.o
	$(CC) -shared world.o -o world.so

world.o: shared.h world.c
	$(CC) $(CFLAGS) -fpic -c world.c -o world.o

player.so: player.o
	$(CC) -shared player.o -o player.so

player.o: shared.h player.c
	$(CC) $(CFLAGS) -fpic -c player.c -o player.o

border.so: border.o
	$(CC) -shared border.o -o border.so

border.o: shared.h border.c
	$(CC) $(CFLAGS) -fpic -c border.c -o border.o

list.so: list.o
	$(CC) -shared list.o -o list.so

list.o: shared.h list.c
	$(CC) $(CFLAGS) -fpic -c list.c -o list.o

string.so: string.o
	$(CC) -shared string.o -o string.so

string.o: shared.h string.c
	$(CC) $(CFLAGS) -fpic -c string.c -o string.o

.phony: clean

clean:
	rm -f *.so *.o
