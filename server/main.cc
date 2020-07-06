#include "node.h"
#include "FlexLexer.h"

Node *yylval;

int main(int argc, char *argv[])
{
  yyFlexLexer *lexer = new yyFlexLexer;
  while (lexer->yylex())
    yylval->Dump();
  return 0;
}
