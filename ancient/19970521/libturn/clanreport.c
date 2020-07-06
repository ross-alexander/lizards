#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

char lizard_colour [6][12] = { "Unsettled", "Red", "Green", "Grey", "Black", "Yellow" };

/* ----------------------------------------------------------------------
--
-- Pass1
--
---------------------------------------------------------------------- */
void ReportPass1(World *, int, int);
void ReportPass1(World *world, int x, int y)
{
  hex_t world_hex;
  char *hexstring = (char*)alloca(256);
  char *bandstring = (char*)alloca(256);
  int work2;
  get_hex(world, x, y, &world_hex);

  if ((world_hex.terrain == DEN || world_hex.terrain == HOME_DEN) &&
      world_hex.hex_owner > 0)
    {
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
    }
}
/* ----------------------------------------------------------------------
--
-- Pass2
--
---------------------------------------------------------------------- */
void ReportPass2(World*, int, int);
void ReportPass2(World *world, int x, int y)
{
  hex_t world_hex;
  char *work_string = (char*)alloca(256);
  char *temp_string = (char*)alloca(256);
  get_hex(world, x, y, &world_hex);
  if (world_hex.terrain != DEN && world_hex.terrain != HOME_DEN &&
      lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner > 0)
    {
      xy_to_string (x, y, work_string);
      
      if (world_hex.raft_here > 0)
	{
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
	}
      else
	{
	  fprintf (world->player[world_hex.hex_owner].fptr,
		   "Band(%s, %d, %d, %d, %d, %d, %d, %d)\n",
		   work_string,
		   world_hex.red_lizards,
		   world_hex.green_lizards,
		   world_hex.grey_lizards,
		   world_hex.black_lizards,
		   world_hex.yellow_lizards,
		   world_hex.turns_hungry,
		   world_hex.combat_points);
	}
    }
}

/* ----------------------------------------------------------------------
--
-- ClanReport
--
---------------------------------------------------------------------- */
void ClanReport(World*);
void ClanReport(World *world)
{
  char work_string [80];

  int work,
      x,
      y;

  printf ("Messaging Players...");

  for (work = 1; work <= world->num_players; work ++)
  {
    fprintf (world->player[work].fptr,
	     "ClanReport(%s)\n",
	     world->player[work].clan_name);
  }

  printf (" \nReporting Clans and Dens...");

  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      ReportPass1(world, x, y);

  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      ReportPass2(world, x, y);

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
}
