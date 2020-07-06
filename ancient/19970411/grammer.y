%token INTEGER IDENT LPAREN RPAREN COMMA WORLD PLAYER HEX UNIT PIPE
UNSEATTLED RED GREEN GREY BLACK YELLOW
PLAIN DEN RUIN FERTILE SWAMP PEAK VOLCANO
WATER WHIRLPOOL SCRUB TEMPLE CURSED HOMEDEN
SATED PECKISH HUNGRY FAMISHED STARVED STARVING
AVERAGE GOOD DANGEROUS FEROCIOUS FEARSOME VERERAN
SPAWN CAPTURE VOLCANO PEAK WRAP VICTORY HOME HOMEVICTORY GIVE
%{
#include "parse.h"
extern int yylex();
int yyerror(char *);
%}

%%
parse	: world player_list
	;

world	: WORLD LPAREN x COMMA y COMMA switchlist RPAREN
	;
x	: INTEGER ;
y	: INTEGER ;

switchlist : switch
	| switchlist PIPE switch
	;
switch	: INTEGER | SPAWN | CAPTURE | VOLCANO | PEAK | WRAP
	| VICTORY | HOME | HOMEVICTORY | GIVE;

player_list: player unitlist hexlist
	| player_list player unitlist hexlist
	;

player: PLAYER LPAREN code COMMA units COMMA spies COMMA visable RPAREN
{ printf("Player %s\n", $3); }
	;

code	: IDENT ;
units	: INTEGER ;
spies	: INTEGER ;
visable	: INTEGER ;

unitlist : unit
	| unitlist unit
	;

unit	: UNIT LPAREN x COMMA y RPAREN
{ fprintf(stdout, "Unit %s %s\n", $3, $5); }
	;

hexlist : hex
	| hexlist hex
	;

hex	: HEX LPAREN x COMMA y COMMA flags COMMA terrain COMMA
	owner COMMA dentype COMMA hungry COMMA combat COMMA
	red COMMA green COMMA grey COMMA black COMMA yellow COMMA
	denpop COMMA integer_list RPAREN
	;
flags	: INTEGER ;
terrain	: INTEGER | PLAIN | DEN | RUIN | FERTILE | SWAMP | PEAK | VOLCANO
	| WATER | WHIRLPOOL | SCRUB | TEMPLE | CURSED | HOMEDEN
	;
owner	: INTEGER
	| IDENT;
dentype	: INTEGER | UNSEATTLED | RED | GREEN | GREY | BLACK | YELLOW ;
hungry	: INTEGER
	| SATED | PECKISH | HUNGRY | FAMISHED | STARVED | STARVING;
combat	: INTEGER
	| AVERAGE | GOOD | DANGEROUS | FEROCIOUS | FEARSOME | VERERAN ;
red	: INTEGER ;
green	: INTEGER ;
grey	: INTEGER ;
black	: INTEGER ;
yellow	: INTEGER ;
denpop	: INTEGER ;

integer_list : INTEGER
	| integer_list COMMA INTEGER
	;

%%
#include <stdlib.h>
#include <stdio.h>

extern Lexicon lexicon;

int yyerror(char *string)
{
  fprintf(stderr, "Error in line %d : %s\n", lexicon.lineNo, string);
  exit(1);
}

