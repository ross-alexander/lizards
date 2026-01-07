# Generated automatically from Makefile.in by configure.
YACC = bison -y
DEBUG = -g -Wall -Wstrict-prototypes -Wmissing-prototypes -pedantic
CFLAGS = -g
FILES = decode

all: $(FILES)

decode: lex.decode.o decode.tab.o decode.o
	$(CC) $^ -o $@

decode.o: decode.c decode.h
	$(CC) $(CFLAGS) -c $< -o $@

lex.decode.c: lex.decode.l decode.tab.h
	$(LEX) -Pdecode $<

decode.tab.c decode.tab.h: decode.tab.y decode.h
	$(YACC) -l -d -t -v -b decode -p decode $<

clean:
	rm -f *~ *.o *.a *.log $(FILES)
