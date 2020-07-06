#include <stdio.h>
#include <stdlib.h>

#include "decode.h"

extern int yyleng;
extern char *yytext;
extern int yylex(void);

struct TurnInfo {
  int turn;
  int player;
  int num_players;
  char *game;
  char **clan_code;
  char **clan_name;
};

char *liz_colour[] = {"Unseattled", "Red", "Green", "Grey", "Black", "Yellow"};

char* liz_cp_desc [] = { "Average",
			   "Good",
			   "Dangerous",
			   "Ferocious",
			   "Fearsome",
			   "Veteran" };

char *liz_hunger_desc [] = { "Sated",
			       "Peckish",
			       "Hungry",
			       "Famished",
			       "Starved",
			       "STARVING" };


char** Process(struct part *ptr);
char** Process(struct part *ptr)
{
  struct part *tmp, *old;
  char **array;
  int needcomma, cnt, total;
  tmp = ptr;
  if (!tmp) return NULL;
  cnt = 0;
  while (tmp)
    {
      if (tmp->type == IDENT)
	cnt++;
      tmp = tmp->next;
    }
  total = cnt;
  array = (char**)malloc(sizeof(char*) * (cnt + 1));
  array[cnt] = NULL;
  tmp = ptr;
  if (tmp->type != RPAREN)
    return NULL;
  needcomma = 0;
  tmp = tmp->next;
  while(tmp && (tmp->type != LPAREN))
    {
      old = tmp;
      if (needcomma && (tmp->type != COMMA))
	return NULL;
      else if (!needcomma && (tmp->type != IDENT))
	return NULL;
      else if (!needcomma)
	{
	  array[--cnt] = tmp->text;
	  needcomma = 1;
	}
      else
	needcomma = 0;
      tmp = tmp->next;
      free(old);
    }
  if (tmp && tmp->type != LPAREN)
    return NULL;
  tmp = tmp->next;
  if (tmp && tmp->type != IDENT)
    return NULL;
  array[--cnt] = tmp->text;
  if (cnt)
    return NULL;
  return array;
}

void Phase(struct TurnInfo *turn, char **array);
void Phase(struct TurnInfo *turn, char **array)
{
  if (!strcmp("world", array[1]))
    printf("<sect>World Phase (All Den Growth and Seattling)\n<p>\n");
  else if (!strcmp("artillary", array[1]))
    printf("<sect>Artillary Phase (all Chant, Lob and Free orders)\n<p>\n");
  else if (!strcmp("movement", array[1]))
    printf("<sect>Movement Phase (all Move, Split, Sail and Sneak orders)\n<p>\n");
  else if (!strcmp("development", array[1]))
    printf("<sect>Development Phase (all Build, Destroy, Recruit and Give orders)\n<p>\n");
}

int PrintLizards(int min, int max, char **array);
int PrintLizards(int min, int max, char **array)
{
  int i;
  int cur = 0;
  int total = 0;
  int count = 0;
  for (i = min; i <= max; i++)
    if(atoi(array[i - min]))
      count++;
  for (i = min; i <= max; i++)
    {
      if (atoi(array[i - min]))
	{
	  if ((cur > 0) && (count > 1) && (cur < (count - 1)))
	    printf(", ");
	  else if ((cur > 0) && (count > 1))
	    printf(" and ");
	  printf("%s %s", array[i - min], liz_colour[i]);
	  cur++;
	  total += atoi(array[i - min]);
	}
    }
  if (!total)
    printf(" no");
  return total;
}

void Decode(struct TurnInfo *turn, char **array);
void Decode(struct TurnInfo *turn, char **array)
{
  int i;
  if (!strcmp("Phase", array[0]))
    Phase(turn, array);
  else if (!strcmp("Header", array[0]))
    {
      turn->game = array[1];
      turn->turn = atoi(array[2]);
      turn->player = atoi(array[3]);
      turn->num_players = atoi(array[4]);
      turn->clan_code = (char**)malloc(sizeof(char*) * (turn->num_players+1));
      turn->clan_name = (char**)malloc(sizeof(char*) * (turn->num_players+1));
      for (i = 1; i <= turn->num_players; i++)
	{
	  turn->clan_code[i] = array[3 + i * 3];
	  turn->clan_name[i] = array[4 + i * 3];
	}
      printf("<!doctype lizdoc system>\n");
      printf("<lizdoc game=\"%s\" turn=\"%d\" ",
	     turn->game, turn->turn);
      printf("clanname=\"%s\" clancode=\"%s\">\n",
	     turn->clan_name[turn->player], turn->clan_code[turn->player]);
    }
  else if (!strcmp("Trailer", array[0]))
    {
      printf("</lizdoc>\n");
    }
  else if (!strcmp("Spawning", array[0]))
    {
    }
  else if (!strcmp("ClanReport", array[0]))
    {
      printf("<sect>Clan Report for Clan %s at and end of turn %d\n<p>\n",
	     turn->clan_name[turn->player],
	     turn->turn);
    }
  else if (!strcmp("ClanReport", array[0]))
    {
      printf("<sect>Clans Ranking at the end of turn %d\n<p>\n",
	     turn->turn);
    }
  else if (!strcmp("Rank", array[0]))
    {
      if (atoi(array[2]))
	printf("<item>%02d - %5d Player %s, \'%s\' (%s) Clan.\n",
	       atoi(array[1]),
	       atoi(array[2]),
	       array[3],
	       array[4],
	       array[5]);
      else
	printf("<item>%02d - EXTINCT Player %s, \'%s\' (%s) Clan.\n",
	       atoi(array[1]),
	       array[3],
	       array[4],
	       array[5]);
    }
  else if (!strcmp("HomeDen", array[0]))
    {
      int total;
      printf("<item>%s - Home Den with %s %s den lizards and ",
	     array[1],
	     array[2],
	     liz_colour[atoi(array[3])]);
      total = PrintLizards(1, 5, &array[4]);
      if (total)
	printf(" (total %d lizards, %s & %s) warrior lizards.",
	       total,
	       liz_hunger_desc[atoi(array[9])],
	       liz_cp_desc[atoi(array[10])]);
      printf("\n");
    }
  else if (!strcmp("Den", array[0]))
    {
      int total;
      if (atoi(array[2]))
	printf("<item>%s - Den with %s %s den lizards and ",
	       array[1],
	       array[2],
	       liz_colour[atoi(array[3])]);
      else
	printf("<item>%s - Den with no den lizards and ",
	       array[1]);
      total = PrintLizards(1, 5, &array[4]);
      if (total)
	printf(" (total %d lizards, %s & %s) warrior lizards.",
	       total,
	       liz_hunger_desc[atoi(array[9])],
	       liz_cp_desc[atoi(array[10])]);
      printf("\n");
    }
  else if (!strcmp("Band", array[0]))
    {
      int total;
      printf("<item>%s - Band of ",
	     array[1]);
      total = PrintLizards(1, 5, &array[2]);
      if (total)
	printf(" (total %d lizards, %s & %s) warrior lizards.",
	       total,
	       liz_hunger_desc[atoi(array[7])],
	       liz_cp_desc[atoi(array[8])]);
      printf("\n");
    }
  else
    {
      printf("<item> ");
      for (i = 0; array[i]; i++)
	printf("%s ", array[i]);
      printf("\n");
    }
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */
int dcleng;
char *dctext;

int main(int argc, char *argv[])
{
  struct TurnInfo turn;
  struct part *ptr, *tmp;
  char **array, *buf;
  int i, size, cnt;
  size = 0; cnt = 0;
  ptr = NULL;
  buf = NULL;
  while ((i = yylex()))
    {
      if ((cnt + dcleng) > size)
	{
	  size += 4096;
	  buf = realloc(buf, size);
	}
      strcpy(buf + cnt, dctext);
      cnt += dcleng;
      switch(i)
	{
	case LPAREN:
	case RPAREN:
	case COMMA:
	  tmp = (struct part*)malloc(sizeof(struct part));
	  tmp->type = i;
	  tmp->next = ptr;
	  ptr = tmp;
	  break;
	case IDENT:
	  tmp = (struct part*)malloc(sizeof(struct part));
	  tmp->type = i;
	  tmp->text = (char*)malloc(dcleng + 1);
	  strcpy(tmp->text, dctext);
	  tmp->next = ptr;
	  ptr = tmp;
	  break;
	case NEWLINE:
	  if ((array = Process(ptr)))
	    Decode(&turn, array);
	  else
	    printf("<item> %s", buf);
	  free(buf);
	  size = cnt = 0;
	  buf = NULL;
	  ptr = NULL;
	  break;
	}
    }
  exit(EXIT_SUCCESS);
}
