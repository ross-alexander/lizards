#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizards.h"
#include "lizgame.h"
#include "lizmisc.h"

static void printhex(world_t *w, hex_t *hex, int left, int top, int right, int bottom, int indent, int nl, int line)
{
  int i, j;
  char str[5], *t, band[25];
  feature_t *f;
  switch(line)
    {
    case 0:
      for (i = 0; i < indent * 16; i++) printf(" ");
      printf("%s%s%s",
	     left ? "*" : "-",
	     top ? "--------------" : "          ",
	     right ? "*" : "-");
      if (nl) printf("\n");
      break;
    case 1:
      for (i = 0; i < indent * 16; i++) printf(" ");
      if (left) printf("|"); else printf(" ");
      xy_to_string(hex->xy.x, hex->xy.y, str);
      printf("%-7s", w->player[hex->owner]->code);
      printf("%7s", str);
      if (right) printf("|"); else printf(" ");
      if (nl) printf("\n");
      break;
    case 2:
      t = liz_terrain[hex->terrain];
      for (i = 0; i < indent * 16; i++) printf(" ");
      if (left) printf("|"); else printf(" ");
      for (j = 0; j < 7 - strlen(t) / 2; j++) printf(" ");
      printf("%s", t);
      for (j = 0; j < 7 - (strlen(t) + 1) / 2; j++) printf(" ");
      if (right) printf("|"); else printf(" ");
      if (nl) printf("\n");
      break;
    case 3:
      for (i = 0; i < indent * 16; i++) printf(" ");
      if (left) printf("|"); else printf(" ");
      if ((f = has_feature(hex, BAND)))
	{
	  sprintf(band, "%d.%d.%d.%d.%d", f->f.band.type[RED],
		 f->f.band.type[GREEN], f->f.band.type[GREY],
		 f->f.band.type[BLACK], f->f.band.type[YELLOW]);
	  printf("%14s", band);
	}
      else
	printf("%-14s", "");
      if (right) printf("|"); else printf(" ");
      if (nl) printf("\n");
      break;
    case 4:
      for (i = 0; i < indent * 16; i++) printf(" ");
      if (left) printf("|"); else printf(" ");
      strcpy(band, " ");
      if ((f = has_feature(hex, DEN))) strcat(band, f->f.den.home ? "H":"D");
      if (has_feature(hex, FERTILE)) strcat(band, "F");
      if (has_feature(hex, WHIRLPOOL)) strcat(band, "W");
      if (has_feature(hex, TEMPLE)) strcat(band, "T");
      if (has_feature(hex, RUIN)) strcat(band, "R");
      printf("%-14s", band);
      if (right) printf("|"); else printf(" ");
      if (nl) printf("\n");
      break;
    case 5:
      for (i = 0; i < indent * 16; i++) printf(" ");
      printf("%s%s%s",
	     bottom ? (left ? "*" : "-") : (left ? "|" : " "),
	     bottom ? "--------------" : "              ",
	     bottom ? (right ? "*" : "-") : (right ? "|" : " "));
      if (nl) printf("\n");
      break;
    }
}      
      
int main(int argc, char *argv[])
{
  world_t *world;
  int i, ch;
  coord xy;
  hex_t *c, *nw, *ne, *n, *se, *sw, *s;
  hex_t *nene, *nese, *sese;
  hex_t *nwnw, *nwsw, *swsw;
  hex_t *swsws, *sws, *ss, *ses, *seses;
  xy.x = 1; xy.y = 1;

  while ((ch = getopt(argc, argv, "h:x:y:")) != EOF)
    switch (ch)
      {
      case 'x':
	xy.x = atoi(optarg);
	break;
      case 'y':
	xy.y = atoi(optarg);
	break;
      case 'h':
	xy = new_string_to_loc(optarg);
	break;
      }
  if ((argc - optind)  != 2)
    exit(1);
  world = get_world(LzCheckDir(argv[optind], 0), argv[optind+1]);
  printf("(showhex %d %d)\n", xy.x, xy.y);
  c = get_hex(world, xy);

  nw = get_hex(world, move_coords(world, c, NORTH_WEST));
  sw = get_hex(world, move_coords(world, c, SOUTH_WEST));
  ne = get_hex(world, move_coords(world, c, NORTH_EAST));
  se = get_hex(world, move_coords(world, c, SOUTH_EAST));
  n = get_hex(world, move_coords(world, c, NORTH));
  s = get_hex(world, move_coords(world, c, SOUTH));
  nene = get_hex(world, move_coords(world, ne, NORTH_EAST));
  nese = get_hex(world, move_coords(world, ne, SOUTH_EAST));
  sese = get_hex(world, move_coords(world, se, SOUTH_EAST));
  nwnw = get_hex(world, move_coords(world, nw, NORTH_WEST));
  nwsw = get_hex(world, move_coords(world, nw, SOUTH_WEST));
  swsw = get_hex(world, move_coords(world, sw, SOUTH_WEST));
  swsws = get_hex(world, move_coords(world, swsw, SOUTH));
  sws   = get_hex(world, move_coords(world, sw,   SOUTH));
  ss    = get_hex(world, move_coords(world, s,    SOUTH));
  ses   = get_hex(world, move_coords(world, se,   SOUTH));
  seses = get_hex(world, move_coords(world, sese, SOUTH));
  for (i = 0; i < 3; i++)
    {
      printhex(world, nwnw, 1, 1, 1, 0, 0, 0, i);
      printhex(world,    n, 1, 1, 1, 0, 1, 0, i);
      printhex(world, nene, 1, 1, 1, 0, 1, 1, i);
    }
  for (i = 3; i < 21; i++)
    {
      printhex(world,  nwnw, 1, 1, 1, 0, 0, 0, i -  0);
      printhex(world,  nwsw, 1, 1, 1, 0, 0, 0, i -  6);
      printhex(world,  swsw, 1, 1, 1, 0, 0, 0, i - 12);
      printhex(world, swsws, 1, 1, 1, 1, 0, 0, i - 18);
      printhex(world,    nw, 0, 1, 0, 0, 0, 0, i -  3);
      printhex(world,    sw, 0, 1, 0, 0, 0, 0, i -  9);
      printhex(world,   sws, 0, 1, 0, 1, 0, 0, i - 15);
      printhex(world,     n, 1, 1, 1, 0, 0, 0, i -  0);
      printhex(world,     c, 1, 1, 1, 0, 0, 0, i -  6);
      printhex(world,     s, 1, 1, 1, 0, 0, 0, i - 12);
      printhex(world,    ss, 1, 1, 1, 0, 0, 0, i - 18);
      printhex(world,    ne, 0, 1, 0, 0, 0, 0, i -  3);
      printhex(world,    se, 0, 1, 0, 0, 0, 0, i -  9);
      printhex(world,   ses, 0, 1, 0, 1, 0, 0, i - 15);
      printhex(world,  nene, 1, 1, 1, 0, 0, 1, i -  0);
      printhex(world,  nese, 1, 1, 1, 0, 0, 1, i -  6);
      printhex(world,  sese, 1, 1, 1, 0, 0, 1, i - 12);
      printhex(world, seses, 1, 1, 1, 0, 0, 1, i - 18);
    }
  for (i = 21; i < 24; i++)
    {
      printhex(world, swsws, 1, 1, 1, 1, 0, 0, i - 18);
      printhex(world,    ss, 1, 1, 1, 1, 1, 0, i - 18);
      printhex(world, seses, 1, 1, 1, 1, 1, 1, i - 18);
    }
  return 0;
}
