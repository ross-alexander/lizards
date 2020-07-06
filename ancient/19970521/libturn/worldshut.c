#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

void CloseHex(World *, int, int);
void CloseSpies(World *);
void WorldClose(World *);

/* ----------------------------------------------------------------------
--
-- CloseHex
--
---------------------------------------------------------------------- */

void CloseHex(World *world, int x, int y)
{
  char hex_string[80];

  hex_t world_hex;
  int hex_change, work3, work4;
  FILE *fptr;

  get_hex (world, x, y, &world_hex);
  xy_to_string(x, y, hex_string);

  hex_change = 0;

  if (world_hex.raft_here > 0)
    {
      world_hex.raft_here--;
      hex_change = 1;
      if (world_hex.raft_here == 0)
	{
	  open_player_mesg (world_hex.hex_owner, &fptr);

	  if ((lizards_in_hex (&world_hex) > 0) &&
	      (world_hex.terrain == WHIRLPOOL))
	    {
	      work3 = (double) lizards_in_hex (&world_hex) * WHIRLPOOL_KILL_LEVEL;

	      if (work3 > 1)
		work3 = random (work3) + 1;
	      else
		work3 = 1;

	      kill_lizards_xy (&world_hex, work3);

	      fprintf(fptr, "RaftWhirlpool(%s, %d, %d, %d, %d, %d, %d)\n",
		      hex_string,
		      work3,
		      world_hex.red_lizards,
		      world_hex.green_lizards,
		      world_hex.grey_lizards,
		      world_hex.black_lizards,
		      world_hex.yellow_lizards
		      );
	    }
	  else
	    {
	      fprintf (fptr, "RaftSinks(%s)\n", hex_string);
	    }
	}
    }

  /* Check 'turns_undisturbed' variable for Den building! */

  if (world_hex.turns_undisturbed > 0)
    {
      if (world_hex.turns_undisturbed == 1)
	if (world_hex.terrain != HOME_DEN)
	  {
	    open_player_mesg (world_hex.hex_owner, &fptr);
	    fprintf(fptr, "HomeDenCollapses(%s)\n", hex_string);
	    world_hex.turns_undisturbed = 0;
	    hex_change = 1;
	  }
	else
	  if (world_hex.terrain == HOME_DEN)
	    {
	      int players [MAX_PLAYERS + 1];

	      in_sight (world, x, y, players);

	      for (work3 = 1; work3 < MAX_PLAYERS + 1; work3 ++)
		if (players [work3] > 0)
		  {
		    open_player_mesg (work3, &fptr);
		    fprintf (fptr, "SeeHomeDenCollapse(%s)\n", hex_string);
		  }

	      world_hex.terrain = RUIN;
	      world_hex.lizard_pop = 0;
	      world_hex.turns_undisturbed = 0;
	      hex_change = 1;
	    }
    }

  /* Check Lizard band hunger points! */

  if ((world_hex.turns_hungry >= MAX_HUNGER_POINTS) &&
      (lizards_in_hex (&world_hex) > 0) &&
      (world_hex.hex_owner) > 0 &&
      (world_hex.terrain != FERTILE) &&
      (world->turn > 0))
    {
      for (work3 = 0, work4 = 0; work3 < lizards_in_hex (&world_hex); work3 ++)
	if (random (100) > STARVE_PER_TURN)
	  work4 ++;

      if (work4 == 0) work4 = 1;

      kill_lizards_xy (&world_hex, work4);

      fprintf (world->player[world_hex.hex_owner].fptr,
	       "Starve(%s, %d, %d, %d, %d, %d, %d)",
	       hex_string,
	       work4,
	       world_hex.red_lizards,
	       world_hex.green_lizards,
	       world_hex.grey_lizards,
	       world_hex.black_lizards,
	       world_hex.yellow_lizards);
      hex_change = 1;
    }
  
  if (hex_change)
    put_hex (world, x, y, &world_hex);
}


/* ----------------------------------------------------------------------
--
-- CloseSpies
--
---------------------------------------------------------------------- */

void CloseSpies(World *world)
{
  int work;
  hex_t world_hex;
  FILE *fptr;
  char hex_string[80];

  for (work = 0; work < world->num_spies; work ++)
    {
      get_hex (world,
	       world->spylist[work]->x_hex,
	       world->spylist[work]->y_hex, &world_hex);
      xy_to_string (world->spylist[work]->x_hex, world->spylist[work]->y_hex, hex_string);

      if (lizards_in_hex (&world_hex) > 0 && world->spylist[work]->player > 0 &&
	  world->spylist[work]->player == world_hex.hex_owner)
	{
	  open_player_mesg (world->spylist[work]->player, &fptr);
	  
	  xy_to_string (world->spylist[work]->x_hex, world->spylist[work]->y_hex,
			hex_string);

	  if (lizards_in_hex (&world_hex) >= MAX_LIZARDS_IN_HEX)
	    {
	      fprintf (fptr, "SpyFailRejoin(%s)\n", hex_string);
	    }
	  else
	    {
	      fprintf (fptr, "SpyRejoin(%s)\n", hex_string);
	      switch (world->spylist[work]->lizard_type)
		{
		case RED_LIZARD:
		  world_hex.red_lizards += 1;

		  break;
		case GREEN_LIZARD:
		  world_hex.green_lizards += 1;

		  break;
		case GREY_LIZARD:
		  world_hex.grey_lizards += 1;

		  break;
		case BLACK_LIZARD:
		  world_hex.black_lizards += 1;

		  break;
		case YELLOW_LIZARD:
		  world_hex.yellow_lizards += 1;

		  break;
		}
	      put_hex (world,
		       world->spylist[work]->x_hex,
		       world->spylist[work]->y_hex,
		       &world_hex);
	      world->spylist[work]->player = 0;
	    }
	}

      if (world->spylist[work]->turns_alone > SPY_BUSH_TURNS && world->spylist[work]->player > 0)
	{
	  open_player_mesg (world->spylist[work]->player, &fptr);
	  fprintf (fptr, "SpyNative(%s)\n", hex_string);
	  world->spylist[work]->player = 0;
	}
    }
}

void WorldClose(World *world)
{
  int x, y, loop;
  hex_t world_hex;

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      CloseHex(world, x, y);
  
  printf ("\nUpdating spys...\n");

  CloseSpies(world);

  printf ("Counting HOME DENS...\n");

  for (loop = 0; loop <= world->num_players; loop ++)
    world->player[loop].home_den = 0;

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	  get_hex (world, x, y, &world_hex);
	  if (world_hex.terrain == HOME_DEN && (world_hex.hex_owner > 0))
	    world->player[world_hex.hex_owner].home_den ++;
	}
}
