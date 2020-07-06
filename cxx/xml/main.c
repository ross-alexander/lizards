extern int yydebug;
extern void tokeninit();

int main(int argc, char *argv)
{
  if (argc > 1)
    yydebug=1;
  tokeninit();
  yyparse();
}
