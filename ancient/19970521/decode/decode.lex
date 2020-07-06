%{
#include "decode.h"

extern int dcleng;
extern char *dctext;
%}
%%
"("[ ]* { dcleng = yyleng; dctext = yytext; return LPAREN; }
")"[ ]* { dcleng = yyleng; dctext = yytext; return RPAREN; }
","[ ]* { dcleng = yyleng; dctext = yytext; return COMMA; }
[A-Za-z0-9_ ]+ { dcleng = yyleng; dctext = yytext; return IDENT; }
\n { dcleng = yyleng; dctext = yytext; return NEWLINE; }
. { dcleng = yyleng; dctext = yytext; return RUBBISH; }
%%
