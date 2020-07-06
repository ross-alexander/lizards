#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizgame.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- CloseHex
--
---------------------------------------------------------------------- */

static void CloseHex(world_t *w, int x, int y)
{
  char str[5];

  hex_t *hex;
  int work3, work4;
  feature_t *raft, *band;
  coord xy;
  xy.x = x; xy.y = y;
  
  hex = get_hex (w, xy);
  xy_to_string(x, y, str);

  if ((raft = has_feature(hex, RAFT)))
    {
      raft->f.raft.age--;
      if (raft->f.raft.age == 0)
	{
	  if ((band = has_feature(hex, BAND)) && has_feature(hex, WHIRLPOOL))
	    {
	      work3 = (int)((double) lizards_in_band (band) * WHIRLPOOL_KILL_LEVEL);
	      if (work3 > 1)
		work3 = random (work3) + 1;
	      else
		work3 = 1;
	      kill_lizards (band, work3);
	      player_mesg(w, hex->owner, "(raft-whirlpool %s %d)\n",
		      str,
		      work3);
	    }
	  else
	    {
	      player_mesg(w, hex->owner, "(raft-sinks %s)\n", str);
	    }
	  free(release_feature(w, raft));
	}
    }
  
  /* Check 'turns_undisturbed' variable for Den building! */
#ifdef IgnoreThisStuff
  if ((build = has_feature(world, BUILDING)))
    {
      if (build->f.build.undisturbed == 2)
	if ( != HOME_DEN)
	  {
	    open_player_mesg (world, world_hex.hex_owner, &fptr);
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
		    open_player_mesg (world, work3, &fptr);
		    fprintf (fptr, "SeeHomeDenCollapse(%s)\n", hex_string);
		  }

	      world_hex.terrain = RUIN;
	      world_hex.lizard_pop = 0;
	      world_hex.turns_undisturbed = 0;
	      hex_change = 1;
	    }
    }
#endif
  /* Check Lizard band hunger points! */

  if ((band = has_feature(hex, BAND)) &&
      (band->f.band.hunger >= MAX_HUNGER_POINTS) &&
      (hex->owner) > 0 &&
      (!has_feature(hex, FERTILE)) &&
      (w->turn > 0))
    {
      for (work3 = 0, work4 = 0; work3 < lizards_in_band (band); work3 ++)
	if (random (100) > STARVE_PER_TURN)
	  work4 ++;
      if (work4 == 0) work4 = 1;
      kill_lizards (band, work4);

      player_mesg(w, hex->owner,
	       "(starve %s %d)\n",
		       str,
		       work4);
    }
}


/* ----------------------------------------------------------------------
--
-- CloseSpies
--
---------------------------------------------------------------------- */

static void CloseSpies(world_t *w)
{
  int work;
  hex_t *hex;
  char str[5];
  coord xy;
  feature_t *band;

  for (work = 0; work < w->num_spies; work ++)
    {
      xy.x = w->spylist[work]->x_hex;
      xy.y = w->spylist[work]->y_hex;
      xy_to_string (xy.x, xy.y, str);
      if ((band = has_feature(hex, BAND)) &&
	  w->spylist[work]->player > 0 &&
	  w->spylist[work]->player == hex->owner)
	{
	  if (lizards_in_band (band) >= MAX_LIZARDS_IN_HEX)
	    {
	      player_mesg(w, w->spylist[work]->player,
			       "(spy-fail-rejoin %s)\n", str);
	    }
	  else
	    {
	      player_mesg(w, w->spylist[work]->player,
			       "(spy-rejoin %s)\n", str);
	      band->f.band.type[w->spylist[work]->lizard_type]++;
	      w->spylist[work]->player = 0;
	    }
	}
      if (w->spylist[work]->turns_alone > SPY_BUSH_TURNS &&
	  w->spylist[work]->player > 0)
	{
	  player_mesg(w, w->spylist[work]->player,
			   "(spy-native %s)\n", str);
	  w->spylist[work]->player = 0;
	}
    }
}

void WorldClose(world_t *world)
{
  int x, y, loop;
  hex_t* hex;
  coord xy;
  feature_t *f;

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      CloseHex(world, x, y);

  printf ("\nUpdating spys...\n");

  CloseSpies(world);

  printf ("Counting HOME DENS...\n");

  for (loop = 0; loop < world->num_players; loop ++)
    if (world->player[loop])
      world->player[loop]->home_den = 0;

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	xy.x = x; xy.y = y;
	hex = get_hex (world, xy);
	if ((f = has_feature(hex, DEN)) && f->f.den.home)
	  world->player[hex->owner]->home_den++;
      }
}
