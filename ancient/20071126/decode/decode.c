#include "decode.h"
#include "decode.tab.h"

void decode_display(Node *n);

void list_display(List *l)
{
  if (l->head) decode_display(l->head);
  if (l->tail) printf(" ");
  if (l->tail) list_display(l->tail);
}

void decode_display(Node *n)
{
  switch(n->type)
    {
    case IDENT:
    case INTEGER:
      printf("%s", n->value);
      break;
    case STRING:
      printf("\"%s\"", n->value);
      break;
    case LIST:
      printf("'(");
      list_display(n->value);
      printf(")");
      break;
    case FUNC:
      printf("(");
      list_display(n->value);
      printf(")");
      break;
    }
}

extern int decodedebug;

Node *parse;

int main()
{
  decodedebug = 0;
  yyinit();
  decodeparse();
  decode_display(parse);
  printf("\n");
  exit(0);
}
