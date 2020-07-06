#define tCOMMA 257
#define tINT 258
#define tSTRING 259
#define tHDR 260
#define tFLG 261
#define tPHR 262
#define tPLR 263
#define tHEX 264
#define tFEA 265
typedef union {
  char *text;
  xmlNodePtr node;
} YYSTYPE;
extern YYSTYPE yylval;
