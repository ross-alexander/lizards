%{
#include <string.h>
#include <libxml/tree.h>

#include "common.h"
#include "parse.tab.h"
%}

%start id dt
%%
<id>HDR	{ BEGIN dt; return tHDR; }
<id>FLG	{ BEGIN dt; return tFLG; }
<id>PHR	{ BEGIN dt; return tPHR; } 
<id>PLR	{ BEGIN dt; return tPLR; }
<id>HEX { BEGIN dt; return tHEX; }
<id>FEA { BEGIN dt; return tFEA; }

<dt>[0-9]+ { yylval.text = strdup(yytext); return tINT; }
<dt>[A-Za-z0-9][A-Za-z0-9 ]* { yylval.text = strdup(yytext); return tSTRING; }
<dt>","	return tCOMMA;
<dt>" "	;
<dt>\n  { BEGIN id; }
%%

void tokeninit()
{
  BEGIN id;
}

int yywrap()
{
  return 1;
}