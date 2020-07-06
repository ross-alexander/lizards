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
void Pass1(World *, int, int);
void Pass1(World *world, int x, int y)
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
		 "HomeDen(%s, %d, %d, %s, %s, %s)\n",
		 hexstring,
		 world_hex.lizard_pop,
		 world_hex.den_lizard_type,
		 bandstring,
		 liz_hunger_desc [world_hex.turns_hungry],
		 liz_cp_desc [world_hex.combat_points]);
      else
	fprintf (world->player[world_hex.hex_owner].fptr,
		 "Den(%s, %d, %s, %s, %s, %s)\n",
		 hexstring,
		 world_hex.lizard_pop,
		 lizard_colour[world_hex.den_lizard_type],
		 bandstring,
		 liz_hunger_desc [world_hex.turns_hungry],
		 liz_cp_desc [world_hex.combat_points]);
    
      work2 = terrain_adjacent (world, x, y, FERTILE);
      
      if (work2 == 0)
	if (world_hex.den_lizard_type == 0)
	  fprintf (world->player[world_hex.hex_owner].fptr,
		   "\n - Unsettled Den has NO Fertile hexes adjacent.");
	else
	  if (world_hex.turns_den_hungry > 0)
	    if (world_hex.terrain == HOME_DEN)
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Home Den has STARVED for %d turns!",
		       world_hex.turns_den_hungry);
	    else
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Den has STARVED for %d turns!",
		       world_hex.turns_den_hungry);
	  else
	    if (world_hex.terrain == HOME_DEN)
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Home Den is STARVING!");
	    else
	      fprintf (world->player[world_hex.hex_owner].fptr,
		       " Den is STARVING!");
            fprintf (world->player[world_hex.hex_owner].fptr, "\n");
    }
}
/* ----------------------------------------------------------------------
--
-- Pass2
--
---------------------------------------------------------------------- */
void Pass2(World*, int, int);
void Pass2(World *world, int x, int y)
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
		   " - %s Band of %s (%s & %s) Lizards",
		   work_string, show_lizards (world_hex.red_lizards,
					      world_hex.green_lizards,
					      world_hex.grey_lizards,
					      world_hex.black_lizards,
					      world_hex.yellow_lizards,
					      temp_string),
		   liz_hunger_desc [world_hex.turns_hungry],
		   liz_cp_desc [world_hex.combat_points]);
	}
      fprintf (world->player[world_hex.hex_owner].fptr, ".\n");      
    }
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

int main (int argc, char *argv [])
{
  char work_string [80];

  int work,
      x,
      y;

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
  }
  else
    {
      printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	      "             Command Line Format: EXEFILE data_path world.turn game.\n"
	      "                             e.g. CLANREPO C:\\ 1 A\n");
      
      exit (EXIT_FAILURE);
    }

  world = (World*)malloc(sizeof(World));
  get_world(world);
  OpenPlayerFiles(world);

  if (world->num_players == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

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
      Pass1(world, x, y);

  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      Pass2(world, x, y);

  printf (" \nReporting Spys...");

  for (work = 0; work < world->num_spies; work ++)
  {
    if (world->spylist[work]->player > 0)
    {
      xy_to_string (world->spylist [work]->x_hex,
		    world->spylist [work]->y_hex,
		    work_string);

      fprintf (world->player[world->spylist[work]->player].fptr,
	       " - %s Spy Lizard. Spy for %d turns.\n",
	       work_string,
	       world->spylist [work]->turns_alone);
    }
  }

  for (work = 0; work < world->num_spies; work ++)
    free (world->spylist [work]);
  
  printf (" \n\nFinished!\n");
  ClosePlayerFiles(world);

  return (EXIT_SUCCESS);
}
