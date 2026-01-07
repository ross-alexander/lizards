%{
#include <string.h>

#include "decode.h"
#include "decode.tab.h"
%}
%start N S
%%
<N>"(" { return LEFT; }
<N>")" { return RIGHT; }
<N>"'" { return QUOTE; }
<N>[0-9]+ { decodelval.token = strdup(yytext); return INTEGER; }
<N>[A-Za-z][A-Za-z0-9_-]+ { decodelval.token = strdup(yytext); return IDENT; }
<N>\" { BEGIN S; }
<S>\" { yytext[yyleng - 1] = '\0'; decodelval.token = strdup(yytext); BEGIN N; return STRING; }
<S>. { yymore(); }
<N>. ;
<N>\n ;
%%
int yywrap()
{
  return 1;
}
void yyinit()
{
  BEGIN N;
}
