#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizgame.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- Pass1
--
---------------------------------------------------------------------- */
static void ReportPass1(world_t *world, coord xy)
{
  hex_t *hex;
  char hexstring[5];
  int work2;
  feature_t *den;

  hex = get_hex(world, xy);

  if ((den = has_feature(hex, DEN)) && hex->owner > 0)
    {
      xy_to_string (xy.x, xy.y, hexstring);
      player_mesg(world, hex->owner,
	"(%s \"%s\" %d \"%s\")\n",
	den->f.den.home ? "home-den" : "den",
	hexstring,
	den->f.den.pop,
	liz_colours[den->f.den.type]);
    }
  work2 = terrain_adjacent (world, xy, FERTILE);

#ifdef X      
  if (work2 == 0)
    if (world_hex.den_lizard_type == 0)
      fprintf (world->player[world_hex.hex_owner]->fptr,
	       "\n - Unsettled Den has NO Fertile hexes adjacent.");
    else
      if (world_hex.turns_den_hungry > 0)
	if (world_hex.terrain == HOME_DEN)
	  fprintf (world->player[world_hex.hex_owner]->fptr,
		   " Home Den has STARVED for %d turns!\n",
		   world_hex.turns_den_hungry);
	else
	  fprintf (world->player[world_hex.hex_owner]->fptr,
		   " Den has STARVED for %d turns!\n",
		   world_hex.turns_den_hungry);
      else
	if (world_hex.terrain == HOME_DEN)
	  fprintf (world->player[world_hex.hex_owner]->fptr,
		   " Home Den is STARVING!\n");
	else
	  fprintf (world->player[world_hex.hex_owner]->fptr,
		   " Den is STARVING!\n");
#endif
}
/* ----------------------------------------------------------------------
--
-- Pass2
--
---------------------------------------------------------------------- */
static void ReportPass2(world_t *world, coord xy)
{
  int i;
  hex_t *hex;
  feature_t *band;
  char source[5];

  hex = get_hex(world, xy);
  band = has_feature(hex, BAND);
  if (band && hex->owner > 0)
    {
      xy_to_string (xy.x, xy.y, source);
      player_mesg(world, hex->owner,
		       "(band \"%s\"", source);
      for (i = 0; i < LIZARD_TYPES; i++)
	player_mesg(world, hex->owner, " %d",	       
			 band->f.band.type[i]);
      player_mesg(world, hex->owner, " \"%s\" \"%s\")\n",
		       liz_hunger_desc[band->f.band.hunger],
		       liz_cp_desc[band->f.band.combat]);
#ifdef X
      if (world_hex.raft_here > 0)
	{
	  fprintf (world->player[world_hex.hex_owner]->fptr,
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
	      fprintf (world->player[world_hex.hex_owner]->fptr,
		       "(sinks NEXT turn)");
	    else
	      fprintf (world->player[world_hex.hex_owner]->fptr,
		       "(sinks in %d turns)",
		       world_hex.raft_here);
	}
#endif
    }
}

/* ----------------------------------------------------------------------
--
-- ClanReport
--
---------------------------------------------------------------------- */
void ClanReport(world_t *world)
{
  char work_string [80];
  int i, x, y;

  printf ("Messaging Players...\n");
  printf ("Reporting Clans and Dens...\n");

  for (i = 1; i < world->num_players; i++)
    player_mesg(world, i,
		"(clan-report \"%s\" '(\n",
		world->player[i]->clan);

  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      ReportPass1(world, coord_cons(x,y));
 
  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      ReportPass2(world, coord_cons(x,y));
  
  printf ("Reporting Spys...\n");

  for (i = 0; i < world->num_spies; i++)
    {
      if (world->spylist[i]->player > 0)
	{
	  xy_to_string (world->spylist[i]->x_hex,
			world->spylist[i]->y_hex,
			work_string);
	  
	  player_mesg(world, world->spylist[i]->player,
			   "(spy %s %d)\n",
			   work_string,
			   world->spylist[i]->turns_alone);
	}
    }
  for (i = 1; i < world->num_players; i++)
    player_mesg(world, i, "))\n");
}
