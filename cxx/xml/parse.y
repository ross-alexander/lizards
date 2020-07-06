%{
#include <stdio.h>
#include <libxml/tree.h>
%}

%token tCOMMA tINT tSTRING tHDR tFLG tPHR tPLR tHEX tFEA

%union {
  char *text;
  xmlNodePtr node;
}

%type <node> hex hexes features plr players
%type <text> tSTRING tINT

%%
parse: hdr flg phr players hexes
{
  xmlDocPtr doc = xmlNewDoc("1.0");
  xmlNodePtr node = xmlNewNode(NULL, "swamp");
  xmlDocSetRootElement(doc, node);
  xmlAddChild(node, $4);
  xmlAddChildList(node, $5);
  xmlSaveFile("out.xml", doc);
}
	;

hdr: tHDR tSTRING tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT
	{ printf("hdr\n"); }
	;


flg	: tFLG
	| tFLG tSTRING
	{ printf("flg\n"); }
	;

phr: tPHR tINT
	{ printf("phr\n"); }
	;


players	: plr
	{ $$ = xmlNewNode(NULL, "players");
	  xmlAddChild($$, $1); }
	| players plr
	{
	  xmlAddChild($1, $2);
	  $$ = $1;
	}
	;

plr	: tPLR tINT tCOMMA tINT tCOMMA tSTRING tCOMMA tSTRING tCOMMA tINT
{
  $$ = xmlNewNode(NULL, "player");
  xmlNewProp($$, "id", $2);
  xmlNewProp($$, "indx", $4);
  xmlNewProp($$, "clan", $6);
  xmlNewProp($$, "name", $8);
}
	;


hex	: tHEX
	  tINT tCOMMA tINT tCOMMA tSTRING tCOMMA tINT  tCOMMA tINT
	  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT
	  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT
	  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT
	  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT  tCOMMA tINT
	{
	  $$ = xmlNewNode(NULL, "hex");
	  xmlNewProp($$, "x", $2);
	  xmlNewProp($$, "y", $4);
	  xmlNewProp($$, "id", $6);
	  xmlNewProp($$, "type", $8);
	  xmlNewProp($$, "owner", $10);
	}

hexes	: hex
	{
	  $$ = xmlNewNode(NULL, "hexes");
	  xmlAddChild($$, $1);
	}
	| hex features
	{
	  $$ = xmlNewNode(NULL, "hexes");
	  xmlAddChild($$, $1);
	  xmlAddChild($1, $2);
	}
	| hexes hex
	{
	  xmlAddChild($1, $2);
	  $$ = $1;
	}
	| hexes hex features
	{
	  xmlAddChild($1, $2);
	  xmlAddChild($2, $3);
	  $$ = $1;
	}
	;

features : tFEA tINT tCOMMA tSTRING nums
	   { 
	     $$ = xmlNewNode(NULL, "feature");
	   }
	 | features
	   tFEA tINT tCOMMA tSTRING nums
	   { $$ = $1; }
	 ;

nums	: tCOMMA tINT
	| nums tCOMMA tINT
	;
%%
void yyerror(const char *error)
{
  fprintf(stderr, "parse error: %s\n", error);
  exit(1);
}
