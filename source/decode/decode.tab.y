%token LEFT RIGHT STRING INTEGER QUOTE IDENT LIST FUNC
%{
#include <stdlib.h>

#include "decode.h"

  Node* BuildNode(int t, void *v)
    {
      Node *tmp = (Node*)malloc(sizeof(Node));
      tmp->type = t;
      tmp->value = v;
      return tmp;
    }

  List* BuildList(Node *s, List *l)
    {
      List *tmp = (List*)malloc(sizeof(List));
      tmp->head = s;
      tmp->tail = l;
      return tmp;
    }

  List* AppendList(List *l, Node *n)
    {
      if (l->tail)
	AppendList(l->tail, n);
      else
	l->tail = BuildList(n, NULL);
      return l;
    }

  extern void decode_display(List *t);

%}

%union {
  char *token;
  Node *node;
};

%type <token> IDENT INTEGER STRING
%type <node> parse plist decode
%%
decode	: parse { parse = $1; }
	;

parse	: LEFT plist RIGHT { $$ = BuildNode(FUNC, $2); }
	| QUOTE LEFT plist RIGHT { $$ = BuildNode(LIST, $3); }
	| IDENT { $$ = BuildNode(IDENT, $1); }
	| INTEGER {  $$ = BuildNode(INTEGER, $1); }
	| STRING {  $$ = BuildNode(STRING, $1); }
	;

plist	: parse { $$ = BuildList($1, NULL); }
	| plist parse { $$ = AppendList($1, $2);}
	;
%%
decodeerror(const char *str)
{
  fprintf(stderr, "syntax error: %s\n", str);
  exit(EXIT_FAILURE);
}
