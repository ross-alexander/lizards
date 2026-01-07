#include <stdio.h>
#include <stdlib.h>

#include "lizards.h"
#include "lizgame.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- ShowHexes
--
---------------------------------------------------------------------- */
static void ShowHexes(world_t *world, int player)
{
  int x,y;
  hex_t *hex;
  feature_t *f;
  printf("\'(\n");
  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	char str[5];
	coord xy; xy.x = x; xy.y = y;
	hex = get_hex(world, xy);
	if (hex->owner != player)
	  continue;
	fprintf(stdout,
		"(make-hex %d %d \"%s\" \"%s\" \"%s\" '(",
		hex->xy.x,
		hex->xy.y,
		xy_to_string(hex->xy.x, hex->xy.y, str),
		world->player[hex->owner]->code,
		liz_terrain[hex->terrain]);
	for (f = hex->chain; f; f = f->chain)
	  {
	    switch (f->type)
	      {
	      case DEN:
		fprintf(stdout, "(Feature \"Den\" \"%s\" %d %d)",
			lizard_colours[f->f.den.type],
			f->f.den.pop,
			f->f.den.home ? 1 : 0);
		break;
	      case BAND:
		fprintf(stdout, "(Feature \"Band\" %d %d %d %d %d %s %s)",
			f->f.band.type[RED],
			f->f.band.type[GREEN],
			f->f.band.type[GREY],
			f->f.band.type[BLACK],
			f->f.band.type[YELLOW],
			liz_hunger_desc[f->f.band.hunger],
			liz_cp_desc[f->f.band.combat]);
		break;
	      case TEMPLE:
		fprintf(stdout, "(Feature \"Temple\")");
		break;
	      case RUIN:
		fprintf(stdout, "(Feature \"Ruin\")");
		break;
	      case VOLCANO:
		fprintf(stdout, "(Feature \"Volcano\")");
		break;
	      case CURSED:
		fprintf(stdout, "(Feature \"Cursed\")");
		break;
	      case WHIRLPOOL:
		fprintf(stdout, "(Feature \"Whirlpool\")");
		break;
	      case FERTILE:
		fprintf(stdout, "(Feature \"Fertile\" %d)",
			f->f.fertile.level);
		break;
	      }
	  }
	fprintf(stdout, ")");
	fprintf(stdout, "\'(");
	    /*
	    move_coords(world,NORTH,hex.x,hex.y,&newx, &newy);
	    if ((newx >= 0) && (newx < world->x) &&
		(newy >= 0) && (newy < world->y)) 
	      fprintf(stdout, "(Border \"north\" \"%02d%02d\")", newx, newy);
	    move_coords(world,NORTH_WEST,hex.x,hex.y,&newx, &newy);
	    if ((newx >= 0) && (newx < world->x) &&
		(newy >= 0) && (newy < world->y)) 
	      fprintf(stdout, "(Border \"northwest\" \"%02d%02d\")", newx, newy);
	    move_coords(world,SOUTH_WEST,hex.x,hex.y,&newx, &newy);
	    if ((newx >= 0) && (newx < world->x) &&
		(newy >= 0) && (newy < world->y)) 
	      fprintf(stdout, "(Border \"southwest\" \"%02d%02d\")", newx, newy);
	    move_coords(world,SOUTH,hex.x,hex.y,&newx, &newy);
	    if ((newx >= 0) && (newx < world->x) &&
		(newy >= 0) && (newy < world->y)) 
	      fprintf(stdout, "(Border \"south\" \"%02d%02d\")", newx, newy);
	    move_coords(world,SOUTH_EAST,hex.x,hex.y,&newx, &newy);
	    if ((newx >= 0) && (newx < world->x) &&
		(newy >= 0) && (newy < world->y)) 
	      fprintf(stdout, "(Border \"southeast\" \"%02d%02d\")", newx, newy);
	    move_coords(world,NORTH_EAST,hex.x,hex.y,&newx, &newy);
	    if ((newx >= 0) && (newx < world->x) &&
		(newy >= 0) && (newy < world->y)) 
	      fprintf(stdout, "(Border \"northeast\" \"%02d%02d\")", newx, newy);
	    */
	fprintf(stdout, ")");
	fprintf(stdout, ")\n");
      }
  fprintf(stdout, ")\n");
}

int main(int argc, char *argv[])
{
  world_t *world;
  int i;
  if (argc != 3)
    exit(1);
  world = get_world(LzCheckDir(argv[1], 0), argv[2]);

  printf("(define a (make-world \"%s\" %d %d %d ",
	world->game,
	world->x,
	world->y,
	world->turn);

  printf("\'(");
  for (i = 0; i < world->num_players; i++)
    if (world->player[i])
      printf("(make-player %d %d \"%s\" \"%s\")",
	     i,
	     world->player[i]->ref,
	     world->player[i]->code,
	     world->player[i]->clan);
  fprintf(stdout, ")\n");
  for (i = 0; i < world->num_players; i++)
    ShowHexes(world, i);
  fprintf(stdout, "))\n");
  return 0;
}
