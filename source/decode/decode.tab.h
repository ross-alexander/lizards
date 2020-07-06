typedef union {
  char *token;
  Node *node;
} YYSTYPE;
#define	LEFT	258
#define	RIGHT	259
#define	STRING	260
#define	INTEGER	261
#define	QUOTE	262
#define	IDENT	263
#define	LIST	264
#define	FUNC	265


extern YYSTYPE decodelval;
