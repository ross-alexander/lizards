#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define YYBYACC 1
#line 2 "parse.y"
#include <stdio.h>
#include <libxml/tree.h>
#line 8 "parse.y"
typedef union {
  char *text;
  xmlNodePtr node;
} YYSTYPE;
#line 14 "parse.tab.c"
#define tCOMMA 257
#define tINT 258
#define tSTRING 259
#define tHDR 260
#define tFLG 261
#define tPHR 262
#define tPLR 263
#define tHEX 264
#define tFEA 265
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    6,    7,    7,    8,    5,    5,    4,    1,    2,
    2,    2,    2,    3,    3,    9,    9,
};
short yylen[] = {                                         2,
    5,   10,    1,    2,    2,    1,    2,   10,   48,    1,
    2,    2,    3,    5,    6,    2,    3,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    4,    0,    0,
    0,    5,    0,    6,    0,    0,    0,    0,    0,    0,
    7,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    2,    0,    0,    0,    0,    0,    0,
    0,   16,    0,    0,    0,    0,   17,    8,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    9,
};
short yydgoto[] = {                                       2,
   19,   20,   26,   14,   15,    3,    6,   10,   48,
};
short yysindex[] = {                                   -260,
 -257,    0, -250, -253, -247, -249, -244,    0, -243, -246,
 -241,    0, -240,    0, -254, -239, -237, -236, -242, -238,
    0, -233, -231, -232, -230, -235, -242, -229, -226, -225,
 -223, -222, -235, -220, -227, -219, -224, -218, -217, -215,
 -216, -213, -214,    0, -212, -211, -210, -208, -213, -207,
 -206,    0, -205, -208, -204, -202,    0,    0, -201, -199,
 -198, -196, -195, -193, -192, -190, -189, -187, -186, -184,
 -183, -181, -180, -178, -177, -175, -174, -172, -171, -169,
 -168, -166, -165, -163, -162, -160, -159, -157, -156, -154,
 -153, -151, -150, -148, -147, -145, -144,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0, -203,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    5,   21,
    0,    0,    0,    0,    0,    6,    7,    0,    0,    0,
    0,    0,    8,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    1,    0,    0,
    0,    0,    0,    3,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
   20,    0,   24,   41,    0,    0,    0,    0,   13,
};
#define YYTABLESIZE 272
short yytable[] = {                                       1,
   14,    4,   15,    7,   10,   11,   12,   13,   13,   18,
    5,    8,    9,   11,   12,   16,   13,   17,   22,   23,
    1,   24,   25,   28,   30,   18,   29,   31,   34,   32,
   35,   40,   36,   37,   42,   38,   39,   41,   43,   27,
   44,   45,   46,   47,   49,   51,   50,   52,   53,   55,
   33,   56,   57,   58,   59,   21,   60,   61,    3,   62,
   63,   54,   64,   65,    0,   66,   67,    0,   68,   69,
    0,   70,   71,    0,   72,   73,    0,   74,   75,    0,
   76,   77,    0,   78,   79,    0,   80,   81,    0,   82,
   83,    0,   84,   85,    0,   86,   87,    0,   88,   89,
    0,   90,   91,    0,   92,   93,    0,   94,   95,    0,
   96,   97,    0,   98,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   14,   14,   15,   15,   10,   11,
   12,   13,
};
short yycheck[] = {                                     260,
    0,  259,    0,  257,    0,    0,    0,    0,  263,  264,
  261,  259,  262,  258,  258,  257,  263,  258,  258,  257,
    0,  258,  265,  257,  257,  264,  258,  258,  258,  265,
  257,  259,  258,  257,  259,  258,  257,  257,  257,   20,
  258,  257,  259,  257,  259,  257,  259,  258,  257,  257,
   27,  258,  258,  258,  257,   15,  258,  257,  262,  258,
  257,   49,  258,  257,   -1,  258,  257,   -1,  258,  257,
   -1,  258,  257,   -1,  258,  257,   -1,  258,  257,   -1,
  258,  257,   -1,  258,  257,   -1,  258,  257,   -1,  258,
  257,   -1,  258,  257,   -1,  258,  257,   -1,  258,  257,
   -1,  258,  257,   -1,  258,  257,   -1,  258,  257,   -1,
  258,  257,   -1,  258,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  264,  265,  264,  265,  264,  264,
  264,  264,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 265
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"tCOMMA","tINT","tSTRING","tHDR",
"tFLG","tPHR","tPLR","tHEX","tFEA",
};
char *yyrule[] = {
"$accept : parse",
"parse : hdr flg phr players hexes",
"hdr : tHDR tSTRING tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT",
"flg : tFLG",
"flg : tFLG tSTRING",
"phr : tPHR tINT",
"players : plr",
"players : players plr",
"plr : tPLR tINT tCOMMA tINT tCOMMA tSTRING tCOMMA tSTRING tCOMMA tINT",
"hex : tHEX tINT tCOMMA tINT tCOMMA tSTRING tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT tCOMMA tINT",
"hexes : hex",
"hexes : hex features",
"hexes : hexes hex",
"hexes : hexes hex features",
"features : tFEA tINT tCOMMA tSTRING nums",
"features : features tFEA tINT tCOMMA tSTRING nums",
"nums : tCOMMA tINT",
"nums : nums tCOMMA tINT",
};
#endif
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 116 "parse.y"
void yyerror(const char *error)
{
  fprintf(stderr, "parse error: %s\n", error);
  exit(1);
}
#line 204 "parse.tab.c"
#define YYABORT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, reading %d (%s)\n", yystate,
                    yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: state %d, shifting to state %d\n",
                    yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: state %d, error recovery shifting\
 to state %d\n", *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: error recovery discarding state %d\n",
                            *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, error recovery discards token %d (%s)\n",
                    yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("yydebug: state %d, reducing by rule %d (%s)\n",
                yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 18 "parse.y"
{
  xmlDocPtr doc = xmlNewDoc("1.0");
  xmlNodePtr node = xmlNewNode(NULL, "swamp");
  xmlDocSetRootElement(doc, node);
  xmlAddChild(node, yyvsp[-1].node);
  xmlAddChildList(node, yyvsp[0].node);
  xmlSaveFile("out.xml", doc);
}
break;
case 2:
#line 29 "parse.y"
{ printf("hdr\n"); }
break;
case 4:
#line 35 "parse.y"
{ printf("flg\n"); }
break;
case 5:
#line 39 "parse.y"
{ printf("phr\n"); }
break;
case 6:
#line 44 "parse.y"
{ yyval.node = xmlNewNode(NULL, "players");
	  xmlAddChild(yyval.node, yyvsp[0].node); }
break;
case 7:
#line 47 "parse.y"
{
	  xmlAddChild(yyvsp[-1].node, yyvsp[0].node);
	  yyval.node = yyvsp[-1].node;
	}
break;
case 8:
#line 54 "parse.y"
{
  yyval.node = xmlNewNode(NULL, "player");
  xmlNewProp(yyval.node, "id", yyvsp[-8].text);
  xmlNewProp(yyval.node, "indx", yyvsp[-6].text);
  xmlNewProp(yyval.node, "clan", yyvsp[-4].text);
  xmlNewProp(yyval.node, "name", yyvsp[-2].text);
}
break;
case 9:
#line 70 "parse.y"
{
	  yyval.node = xmlNewNode(NULL, "hex");
	  xmlNewProp(yyval.node, "x", yyvsp[-46].text);
	  xmlNewProp(yyval.node, "y", yyvsp[-44].text);
	  xmlNewProp(yyval.node, "id", yyvsp[-42].text);
	  xmlNewProp(yyval.node, "type", yyvsp[-40].text);
	  xmlNewProp(yyval.node, "owner", yyvsp[-38].text);
	}
break;
case 10:
#line 80 "parse.y"
{
	  yyval.node = xmlNewNode(NULL, "hexes");
	  xmlAddChild(yyval.node, yyvsp[0].node);
	}
break;
case 11:
#line 85 "parse.y"
{
	  yyval.node = xmlNewNode(NULL, "hexes");
	  xmlAddChild(yyval.node, yyvsp[-1].node);
	  xmlAddChild(yyvsp[-1].node, yyvsp[0].node);
	}
break;
case 12:
#line 91 "parse.y"
{
	  xmlAddChild(yyvsp[-1].node, yyvsp[0].node);
	  yyval.node = yyvsp[-1].node;
	}
break;
case 13:
#line 96 "parse.y"
{
	  xmlAddChild(yyvsp[-2].node, yyvsp[-1].node);
	  xmlAddChild(yyvsp[-1].node, yyvsp[0].node);
	  yyval.node = yyvsp[-2].node;
	}
break;
case 14:
#line 104 "parse.y"
{ 
	     yyval.node = xmlNewNode(NULL, "feature");
	   }
break;
case 15:
#line 109 "parse.y"
{ yyval.node = yyvsp[-5].node; }
break;
#line 440 "parse.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("yydebug: state %d, reading %d (%s)\n",
                        YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("yydebug: after reduction, shifting from state %d \
to state %d\n", *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
