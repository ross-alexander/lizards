# Generated automatically from Makefile.in by configure.
MV = mv
YACC = bison -y
LIBLEX = -lfl

FILES = decode

DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic

all: $(FILES)

decode: lex.o decode.o
	$(CC) $^ $(LIBLEX) -o $@

lex.c: decode.lex
	$(LEX) decode.lex
	mv lex.yy.c lex.c

clean:
	rm -f *~ *.o *.a *.log $(FILES)
