#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alloca.h>

#include <libxml/tree.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- Pass1
--
---------------------------------------------------------------------- */
void ReportPass1(world_t *world, int x, int y)
{
  hex_t *chex;
  den_t *den;
  chex = (*world->map)(x, y);
  if (((den = (den_t*)(chex->has_feature(DEN))) != NULL) && (chex->owner > 0))
    {
      fprintf(world->player[chex->owner]->fptr,
	      "* %s at %s with %d %s lizards.\n",
	      den->home ? "Home Den" : "Den",
	      chex->title,
	      den->pop,
	      misc_t::colours(den->colour));
      /*
      xy_to_string (x, y, hexstring);
      show_lizards (world_hex.red_lizards,
		    world_hex.green_lizards,
		    world_hex.grey_lizards,
		    world_hex.black_lizards,
		    world_hex.yellow_lizards,
		    bandstring);
     
      if (world_hex.terrain == HOME_DEN)
	fprintf (world->player[world_hex.hex_owner].fptr,
		 "HomeDen(%s,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",
		 hexstring,
		 world_hex.lizard_pop,
		 world_hex.den_lizard_type,
		 world_hex.red_lizards,
		 world_hex.green_lizards,
		 world_hex.grey_lizards,
		 world_hex.black_lizards,
		 world_hex.yellow_lizards,
		 world_hex.turns_hungry,
		 world_hex.combat_points);
      else
	fprintf (world->player[world_hex.hex_owner].fptr,
		 "Den(%s,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",
		 hexstring,
		 world_hex.lizard_pop,
		 world_hex.den_lizard_type,
		 world_hex.red_lizards,
		 world_hex.green_lizards,
		 world_hex.grey_lizards,
		 world_hex.black_lizards,
		 world_hex.yellow_lizards,
		 world_hex.turns_hungry,
		 world_hex.combat_points);

      work2 = terrain_adjacent (world, x, y, FERTILE);
      
      if (work2 == 0)
	if (world_hex.den_lizard_type == 0)
	  fprintf (world->player[world_hex.hex_owner].fptr,
		   "\n - Unsettled Den has NO Fertile hexes adjacent.");
	else
	  if (world_hex.turns_den_hungry > 0)
	    if (world_hex.terrain == HOME_DEN)
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Home Den has STARVED for %d turns!\n",
		       world_hex.turns_den_hungry);
	    else
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Den has STARVED for %d turns!\n",
		       world_hex.turns_den_hungry);
	  else
	    if (world_hex.terrain == HOME_DEN)
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Home Den is STARVING!\n");
	    else
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Den is STARVING!\n");
      */
    }
}

/* ----------------------------------------------------------------------
--
-- Pass2
--
---------------------------------------------------------------------- */
void ReportPass2(world_t *world, int x, int y)
{
  hex_t *hex;
  hex = (*world->map)(x, y);
  band_t *band = (band_t*)hex->has_feature(BAND);
  raft_t *raft = (raft_t*)hex->has_feature(RAFT);
  if (band != NULL && hex->owner > 0)
    {
      char *desc = band->describe();
      if (raft != NULL)
	{
	  /*
	  fprintf (world->player[world_hex.hex_owner].fptr,
		   " - %s Band of %s (%s & %s) Lizards on raft ",
		   work_string, show_lizards (world_hex.red_lizards,
					      world_hex.green_lizards,
					      world_hex.grey_lizards,
					      world_hex.black_lizards,
					      world_hex.yellow_lizards,
					      temp_string),
		   liz_hunger_desc [world_hex.turns_hungry],
		   liz_cp_desc [world_hex.combat_points]);
	  
	  if (world_hex.green_lizards > 0)
	    if (world_hex.raft_here == 1)
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       "(sinks NEXT turn)");
	    else
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       "(sinks in %d turns)",
		       world_hex.raft_here);
	  */
	}
      else
	{
	  fprintf (world->player[hex->owner]->fptr,
		   "%s - Band of %s (%s & %s) warrior lizards.\n",
		   hex->title,
		   desc,
		   misc_t::hunger_desc(band->hunger),
		   misc_t::combat_desc(band->combat));
	}
    }
}

/* ----------------------------------------------------------------------
--
-- ClanReport
--
---------------------------------------------------------------------- */
int turn_t::clanreport()
{
  int work, x, y;

  printf ("Messaging Players...");

  for (work = 1; work < world->num_players; work ++)
  {
    fprintf (world->player[work]->fptr,
	     "Clan Report for clan %s (%s)\n",
	     world->player[work]->clan,
	     world->player[work]->code);
  }

  printf (" \nReporting Clans and Dens...");

  for (y = 0; y < world->map->height; y ++)
    for (x = 0; x < world->map->width; x ++)
      ReportPass1(world, x, y);
  for (y = 0; y < world->map->height; y ++)
    for (x = 0; x < world->map->width; x ++)
      ReportPass2(world, x, y);
  /*

  printf (" \nReporting Spys...");

  for (work = 0; work < world->num_spies; work ++)
  {
    if (world->spylist[work]->player > 0)
    {
      xy_to_string (world->spylist [work]->x_hex,
		    world->spylist [work]->y_hex,
		    work_string);

      fprintf (world->player[world->spylist[work]->player].fptr,
	       "Spy(%s, %d)\n",
	       work_string,
	       world->spylist[work]->turns_alone);
    }
  }
  */
  printf("\n");
  return 1;
}
