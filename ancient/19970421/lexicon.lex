%{
#include "parse.h"
#include "grammer.h"

extern StringRing *ring;

%}

%%
World { return WORLD; }
Player { return PLAYER; }
Hex { return HEX; }
Unit { return UNIT; }
"(" { return LPAREN; }
")" { return RPAREN; }
"," { return COMMA; }
[0-9]+ { yylval = RingInsert(ring, yyleng, yytext); return INTEGER; }
[A-Za-z][A-Za-z0-9-]+ { yylval = RingInsert(ring, yyleng, yytext); return IDENT; }
.	;
\n	lexicon.lineNo++;
