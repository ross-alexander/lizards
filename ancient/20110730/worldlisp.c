#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "lizgame.h"
#include "lizmisc.h"
#include "lizhex.h"

void ShowHexes(World *);

/* ----------------------------------------------------------------------
--
-- ShowHexes
--
---------------------------------------------------------------------- */
void ShowHexes(World *world)
{
  int x,y;
  hex_t hex;
  printf("\'(\n");
  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	int flags = 0x0000;
	get_hex(world, x, y, &hex);
	    if (hex.bridge_here) 
	      flags |= LZ_BRIDGE_NS;
	    if (hex.raft_here)
	      flags |= LZ_RAFT_HERE;
	    
	    fprintf(stdout,
		    "(make-hex %d %d \"%02d%02d\" \"%s\" \"%s\" %d '(",
		    hex.x,
		    hex.y,
		    hex.x,
		    hex.y,
		    world->player[hex.hex_owner].code,
		    liz_terrain[hex.terrain],
		    hex.terrain == FERTILE ? 10 : 0
		    );
	    /*
	    if (hex.terrain == HOME_DEN) {
	      fprintf(stdout, "(Feature \"HomeDen\" \"%s\" %d)",
		      lizard_colours[hex.den_lizard_type],
		      hex.lizard_pop); }
	    if (hex.terrain == DEN) {
	      fprintf(stdout, "(Feature \"Den\" \"%s\" %d)",
		      lizard_colours[hex.den_lizard_type],
		      hex.lizard_pop); }
	    if (hex.terrain == TEMPLE) { fprintf(stdout, "(Feature \"Temple\")"); }
	    if (hex.terrain == RUIN) { fprintf(stdout, "(Feature \"Ruin\")"); }
	    if (hex.terrain == VOLCANO) { fprintf(stdout, "(Feature \"Volcano\")"); }
	    if (hex.terrain == CURSED) { fprintf(stdout, "(Feature \"Cursed\")"); }
	    if (hex.terrain == WHIRLPOOL) { fprintf(stdout, "(Feature \"Whirlpool\")"); }
	    if ((hex.red_lizards + hex.green_lizards + hex.grey_lizards +
		 hex.black_lizards + hex.yellow_lizards) > 0)
	      {
		fprintf(stdout, "(Feature \"Band\" %s %s (",
			liz_hunger_desc[hex.turns_hungry],
			liz_cp_desc[hex.combat_points]);
		if (hex.red_lizards) fprintf(stdout,"(\"Red\" %d)", hex.red_lizards);
		if (hex.green_lizards) fprintf(stdout,"(\"Green\" %d)", hex.green_lizards);
		if (hex.grey_lizards) fprintf(stdout,"(\"Grey\" %d)", hex.grey_lizards);
		if (hex.black_lizards) fprintf(stdout,"(\"Black\" %d)", hex.black_lizards);
		if (hex.yellow_lizards) fprintf(stdout,"(\"Yellow\" %d)", hex.yellow_lizards);
		fprintf(stdout, "))");
	      }
	    */
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
  int i;

  if (argc == 3)
    {
      data_path = LzCheckDir(argv[1], 0);
      game_code = (char*)malloc(strlen(argv[2] + 1));
      strcpy (game_code, argv [2]);
      strupr (game_code);
    }
  else
    exit(1);
  world = get_world();

  printf("(define a (make-world \"%s\" %d %d %d ",
	game_code,
	world->x,
	world->y,
	world->turn);

  printf("\'(");
  for (i = 0; i <= world->num_players; i++)
    {
      printf("(make-player %d \"%s\" \"%s\")",
	     i,
	     world->player[i].code,
	     world->player[i].clan_name);
    }
  fprintf(stdout, ")\n");
  ShowHexes(world);
  fprintf(stdout, "))\n");
  return 0;
}
