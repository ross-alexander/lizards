#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizhex.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- lizard_pop_growth
--
---------------------------------------------------------------------- */

static int lizard_pop_growth (world_t *world, int present_pop, int max_pop)
{
  double fwork,
	fwork2;

  int work;

  if (present_pop > max_pop)
    {
      if (present_pop - max_pop < 1)
	return (1);
      else
	return (-((present_pop - max_pop) / 2));
  }
  else
    {
      fwork = ((double) present_pop / (double) max_pop);
      fwork2 = 0.6 - ((fwork / 0.10) * 0.05);
      work = (int)((double) present_pop * fwork2);
    }
  return (work);
}
/* ----------------------------------------------------------------------
--
-- ProcessDen
--
---------------------------------------------------------------------- */
static void ProcessDen(world_t *world, hex_t *hex, feature_t *den)
{
  int work3, work4;
  char work_string[80];
  int owner = hex->owner;
  feature_t *band = has_feature(hex, BAND);

  work3 = feature_adjacent (world, hex->xy, FERTILE);
  xy_to_string(hex->xy.x, hex->xy.y, work_string);
  if (work3 == 0)
    {
      if (world->turn < 1)
	return;
      if (den->f.den.type == 0)
	{
	  if (hex->owner > 0)
	    player_mesg(world, hex->owner,
		     "(nomad-starve %s %d)\n",
		     work_string,
		     den->f.den.pop);
	  free(release_feature(world, den));
	  add_feature(world, create_feature(world, RUIN, hex->xy), hex);
	}
      else
	{
	  work4 = (int)((double) den->f.den.pop *
		   (DEN_STARVE_LEVEL + (den->f.den.hunger *
					DEN_INCREASE_STARVE_LEVEL)));
	  if ((den->f.den.pop - work4) < MINIMUM_POP_LEVEL)
	    {
	      free(release_feature(world, den));
	      add_feature(world, create_feature(world, RUIN, hex->xy), hex);
	      if (hex->owner == 0)
		{
		  if (band)
		    free(release_feature(world, band));
		}
	      else
		{
		  player_mesg (world, hex->owner,
			   "(%s-starve-run %s %d)\n", 
			   den->f.den.home ? "homeden" : "den",
			   work_string,
			   den->f.den.type);
		}
	    }
	  else 
	    {
	      if (hex->owner > 0)
		player_mesg (world, owner,
			 "(%s-starve %s %d %d %d)\n",
			 den->f.den.home ? "homeden" : "den",
			 work_string,
			 den->f.den.type,
			 work4,
			 den->f.den.pop);
	      den->f.den.hunger++;
	      den->f.den.pop -= work4;
	    }
	}
    }
  else
    {
      if (den->f.den.type == 0)
	{
	  work4 = POP_ADDITION;
	  den->f.den.pop += work4;
	  
	  player_mesg (world, owner,
		   "(nomads-join-den %s %d)\n",
		   work_string,
		   work4);
	  if (den->f.den.pop > MINIMUM_POP_LEVEL)
	    {
	      den->f.den.type = random (5) + 1;
	      player_mesg (world, owner,
		       "(den-settled %s  %d)\n",
		       work_string,
		       den->f.den.type);
	    }
	}
      else 
	{
	  if (den->f.den.pop < (work3 * DEN_LIZARD_LEVEL))
	    {
	      work4 = lizard_pop_growth (world, den->f.den.pop,
					 work3 * DEN_LIZARD_LEVEL);
	      den->f.den.pop += work4;
	    }
	  else
	    {
	      if (den->f.den.pop > (work3 * DEN_LIZARD_LEVEL) &&
		  world->turn > 0)
		{
		  work4 = lizard_pop_growth (world, den->f.den.pop,
					     work3 * DEN_LIZARD_LEVEL);
		  
		  if (work4 == 0)
		    work4 = -1;
		  den->f.den.pop += work4;
		  if (owner > 0)
		    player_mesg(world, owner,
			     "(den-overpopulation %s %d)\n",
			     work_string,
			     -work4);
		}
	    }
	}
    }
}

/* ---------------------------------------------------------------------- */

static void OpenHex(world_t *w, int x, int y)
{
  hex_t *hex;
  coord xy;
  int owner, work3;
  char str[5];
  feature_t *f, *band;
  xy.x = x; xy.y = y;
  hex = get_hex(w, xy);
  owner = hex->owner;
  xy_to_string (hex->xy.x, hex->xy.y, str);

  for (f = hex->chain; f; f = f->chain)
    switch(f->type)
      {
      case DEN:
	ProcessDen(w, hex, f);
	break;
      case BUILDING:
	if (f->f.build.undisturbed)
	  f->f.build.undisturbed--;
	break;
      case BAND:
	if (f->f.band.immobile > 0)
	  if (f->f.band.immobile == 3)
	    f->f.band.immobile = 0;
	  else
	    f->f.band.immobile -= 1;
	if (has_feature(hex, FERTILE))
	  f->f.band.hunger = 0;
	else
	  if (f->f.band.hunger < MAX_HUNGER_POINTS && (w->turn > 0))
	    f->f.band.hunger += 1;
	break;
      case CURSED:
	if (w->turn > 0)
	  if ((band = has_feature(hex, BAND)))
	    {
	      work3 = (int)((double) lizards_in_band (band) * CURSED_KILL_LEVEL);
	      if (work3 < 1) work3 = 1;

	      player_mesg(w, hex->owner,
		       "(band-cursed %s %d)\n",
		       str,
		       lizards_in_band(band));
	      kill_lizards (band, work3);
	    }
	for (work3 = 0; work3 < w->num_spies; work3 ++)
	  if (w->spylist [work3]->x_hex == hex->xy.x &&
	      w->spylist [work3]->y_hex == hex->xy.y)
	    {
	      player_mesg (w, w->spylist[work3]->player,
		       "(spy-cursed %s)\n",
		       str);
	      w->spylist [work3]->player = 0;
	    }
      }
}
/* ----------------------------------------------------------------------
--
-- WorldOpen(World *world)
--
---------------------------------------------------------------------- */
void WorldOpen(world_t *world)
{
  int loop, i, x, y;
  hex_t *hex;

  printf ("Messaging players...\n");

  for (loop = 0; loop < world->num_players; loop ++)
    {
      player_mesg (world, loop,
			"(header \"%s\" %d %d %d '(",
	       world->game, world->turn,
	       loop, world->num_players);
      for (i = 1; i < world->num_players; i++)
	player_mesg(world, loop,
		    "'(%d \"%s\" \"%s\" \"%s\")",
		    i, world->player[i]->ref,
		    world->player[i]->code,
		    world->player[i]->clan);
      player_mesg(world, loop, ")\n");
    }

  for (loop = 0; loop < world->num_players; loop ++)
    player_mesg (world, loop, "'(");

  for (loop = 1; loop < world->num_players; loop++)
    player_mesg(world, loop, "(world-phase)\n");
  printf ("Opening world...\n");
  
  /* ---------
     --
     -- process each work hex in turn
     --
     --------- */

  for (i = 0; i < world->num_players; i++)
    if (world->player[i]) world->player[i]->home_den = 0;

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	feature_t *f;
	coord xy; xy.x = x; xy.y = y;
	if ((f = has_feature((hex = get_hex(world, xy)), DEN)) &&
	    f->f.den.home)
	  world->player[hex->owner]->home_den++;
	OpenHex(world, x, y);
      }
  /* Update spy turns alone */

  printf ("Updating spys...\n");
  for (loop = 0; loop < world->num_spies; loop++)
    {
      world->spylist [loop]->turns_alone ++;
      hex = get_hex (world, coord_cons(world->spylist [loop]->x_hex,
	       world->spylist [loop]->y_hex));
      
      /* Free immobile Spys */
      
      if (world->spylist [loop]->mired > 0)
	world->spylist [loop]->mired -= 1;
       
      /* Clear Spy moved flags */
      
      world->spylist[loop]->moved_this_turn = 0;
    }
}
