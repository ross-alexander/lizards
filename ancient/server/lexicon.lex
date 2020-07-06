%{
#include "node.h"
#include "parse.h"

extern Node* yylval;
%}

%%
[A-Za-z][A-Za-z0-9]* { yylval = new TextNode(yyleng, yytext); return IDENT; }
.|\n ;
%%


