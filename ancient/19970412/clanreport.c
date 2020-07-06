#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

int main (int argc, char *argv [])
{
  FILE *fptr;

  char lizard_colour [6][12] = { "Unsettled", "Red", "Green", "Grey", "Black", "Yellow" };

  char work_string [80],
       temp_string [80];

  int work,
      work2,
      pass,
      x,
      y;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! Clan Status Reporter. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 3)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2]) + 1);
    strcpy (game_code, argv[2]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path world.turn game.\n"
	     "                             e.g. CLANREPO C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  world = (World*)malloc(sizeof(World));
  get_world(world);

  if (world->num_players == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  printf ("Messaging Players...");

  for (work = 1; work <= world->num_players; work ++)
  {
    open_player_mesg (work, &fptr);

    fprintf (fptr, "\n * Clan Listing for '%s' at end of Turn %d:\n\n",
	     world->player[work].clan_name, world->turn);

    fclose (fptr);
  }

  printf (" \nReporting Clans and Dens...");

  for (pass = 0; pass < 2; pass ++)
  {
    for (x = 0; x < world->x; x ++)
    {
      for (y = 0; y < world->y; y ++)
      {
	get_hex (world, x, y, &world_hex);

	if (pass == 0)
	{
	  if ((world_hex.terrain == DEN || world_hex.terrain == HOME_DEN) &&
	      world_hex.hex_owner > 0)
	  {
	    xy_to_string (x, y, work_string);

	    open_player_mesg (world_hex.hex_owner, &fptr);

	    if (lizards_in_hex (&world_hex) > 0)
	    {
	      if (world_hex.den_lizard_type == 0)
		fprintf (fptr, " - %s Unsettled Den contains %s (%s & %s) Lizards",
			 work_string, show_lizards (world_hex.red_lizards,
			 world_hex.green_lizards, world_hex.grey_lizards,
			 world_hex.black_lizards, world_hex.yellow_lizards,
			 temp_string), liz_hunger_desc [world_hex.turns_hungry],
			 liz_cp_desc [world_hex.combat_points]);
	      else
		if (world_hex.terrain == HOME_DEN)
		  {
		    char *tmp1, *tmp2, *tmp3, *tmp4, *tmp5;
		    tmp1 = work_string;
		    tmp2 = lizard_colour [world_hex.den_lizard_type];
		    tmp3 = show_lizards (world_hex.red_lizards,
					 world_hex.green_lizards,
					 world_hex.grey_lizards,
					 world_hex.black_lizards,
					 world_hex.yellow_lizards,
					 temp_string); 
		    tmp4 = liz_hunger_desc [world_hex.turns_hungry];
		    tmp5 = liz_cp_desc [world_hex.combat_points];
		    fprintf (fptr, " - %s %s Home Den contains %s (%s & %s) Lizards",
			     tmp1, tmp2, tmp3, tmp4, tmp5);
		  }
		else
		  fprintf (fptr, " - %s %s Den contains %s (%s & %s) Lizards",
			   work_string, lizard_colour [world_hex.den_lizard_type],
			   show_lizards (world_hex.red_lizards,
			   world_hex.green_lizards, world_hex.grey_lizards,
			   world_hex.black_lizards, world_hex.yellow_lizards,
			   temp_string), liz_hunger_desc [world_hex.turns_hungry],
			   liz_cp_desc [world_hex.combat_points]);

	      fprintf (fptr, " and");
	    } else
	      if (world_hex.den_lizard_type == 0)
		fprintf (fptr, " - %s Unsettled Den contains NO Fighting Lizards! and",
			 work_string);
	      else
		if (world_hex.terrain == HOME_DEN)
		  fprintf (fptr, " - %s %s Home Den contains NO Fighting Lizards! and",
			   work_string, lizard_colour [world_hex.den_lizard_type]);
		else
		  fprintf (fptr, " - %s %s Den contains NO Fighting Lizards! and",
			   work_string, lizard_colour [world_hex.den_lizard_type]);

	    if (world_hex.den_lizard_type == 0)
	      fprintf (fptr, " %d nomad Lizards.",
		       world_hex.lizard_pop);
	    else
	      fprintf (fptr, " %d Den Lizards.",
		       world_hex.lizard_pop);

	    work2 = terrain_adjacent (world, x, y, FERTILE);

	    if (work2 == 0)
	      if (world_hex.den_lizard_type == 0)
		fprintf (fptr, "\n - Unsettled Den has NO Fertile hexes adjacent.");
	      else
		if (world_hex.turns_den_hungry > 0)
		  if (world_hex.terrain == HOME_DEN)
		    fprintf (fptr, " Home Den has STARVED for %d turns!",
			     world_hex.turns_den_hungry);
		  else
		    fprintf (fptr, " Den has STARVED for %d turns!",
			     world_hex.turns_den_hungry);
		else
		  if (world_hex.terrain == HOME_DEN)
		    fprintf (fptr, " Home Den is STARVING!");
		  else
		    fprintf (fptr, " Den is STARVING!");

	    fprintf (fptr, "\n");

	    fclose (fptr);
	  }
	} else {
	  if (world_hex.terrain != DEN && world_hex.terrain != HOME_DEN &&
	      lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner > 0)
	  {
	    xy_to_string (x, y, work_string);

	    open_player_mesg (world_hex.hex_owner, &fptr);

	    if (world_hex.raft_here > 0)
	    {
	      fprintf (fptr, " - %s Band of %s (%s & %s) Lizards on raft ",
		       work_string, show_lizards (world_hex.red_lizards,
		       world_hex.green_lizards, world_hex.grey_lizards,
		       world_hex.black_lizards, world_hex.yellow_lizards,
		       temp_string), liz_hunger_desc [world_hex.turns_hungry],
		       liz_cp_desc [world_hex.combat_points]);

	      if (world_hex.green_lizards > 0)
		if (world_hex.raft_here == 1)
		  fprintf (fptr, "(sinks NEXT turn)");
		else
		  fprintf (fptr, "(sinks in %d turns)",
		    world_hex.raft_here);
	    } else
	      fprintf (fptr, " - %s Band of %s (%s & %s) Lizards",
		       work_string, show_lizards (world_hex.red_lizards,
		       world_hex.green_lizards, world_hex.grey_lizards,
		       world_hex.black_lizards, world_hex.yellow_lizards,
		       temp_string), liz_hunger_desc [world_hex.turns_hungry],
		       liz_cp_desc [world_hex.combat_points]);

	    fprintf (fptr, ".\n");

	    fclose (fptr);
	  }
	}
      }
    }
  }

  printf (" \nReporting Spys...");

  for (work = 0; work < world->num_spies; work ++)
  {
    if (world->spylist[work]->player > 0)
    {
      xy_to_string (world->spylist [work]->x_hex,
		    world->spylist [work]->y_hex,
		    work_string);

      open_player_mesg (world->spylist[work]->player, &fptr);

      fprintf (fptr, " - %s Spy Lizard. Spy for %d turns.\n",
	       work_string,
	       world->spylist [work]->turns_alone);

      fclose (fptr);
    }
  }

  for (work = 0; work < world->num_spies; work ++)
    free (world->spylist [work]);

  printf (" \n\nFinished!\n");

  return (EXIT_SUCCESS);
}
