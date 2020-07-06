#define LPAREN 1
#define RPAREN 2
#define COMMA 3
#define IDENT 4
#define NEWLINE 5
#define RUBBISH 6

struct part {
  struct part *next;
  char *text;
  int type;
};
