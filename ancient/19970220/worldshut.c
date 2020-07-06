#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

int  main (int argc, char *argv []);
void DoHex(int, int);
void DoSpies(void);

unsigned int spawn_turn_switch = 0,
  capture_switch = 0,
  volcano_dormant_switch = 0,
  peak_active_switch = 0,
  world_wrap_switch = 0,
  victory_points_switch = 0,
  home_owned_worth = 0,
  home_victory_points = 0,
  give_switch = 0;


/* ----------------------------------------------------------------------
--
-- DoHex
--
---------------------------------------------------------------------- */

void DoHex(int x, int y)
{
  char hex_string[80];

  hex_t world_hex;
  int hex_change, work3, work4;
  FILE *fptr;

  get_hex (x, y, &world_hex);
  xy_to_string(x, y, hex_string);

  hex_change = 0;

  if (world_hex.raft_here > 0)
    {
      world_hex.raft_here--;
      hex_change = 1;
      if (world_hex.raft_here == 0)
	{
	  open_player_mesg (world_hex.hex_owner, &fptr);

	  if (lizards_in_hex (&world_hex) > 0 && world_hex.terrain == WHIRLPOOL)
	    {
	      work3 = (double) lizards_in_hex (&world_hex) * WHIRLPOOL_KILL_LEVEL;

	      if (work3 > 1)
		work3 = random (work3) + 1;
	      else
		work3 = 1;

	      kill_lizards_xy (&world_hex, work3);

	      fprintf(fptr, "RaftWhirlpool: %s %d %d %d %d %d %d\n",
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
	      fprintf (fptr, "RaftSinks: %s\n", hex_string);
	    }
	  fclose (fptr);
	}
    }

  /* Check 'turns_undisturbed' variable for Den building! */

  if (world_hex.turns_undisturbed > 0)
    {
      if (world_hex.turns_undisturbed == 1)
	if (world_hex.terrain != HOME_DEN)
	  {
	    open_player_mesg (world_hex.hex_owner, &fptr);
	    fprintf(fptr, "HomeDenCollapses: %s\n", hex_string);
	    fclose (fptr);

	    world_hex.turns_undisturbed = 0;
	    hex_change = 1;
	  }
	else
	  if (world_hex.terrain == HOME_DEN)
	    {
	      int players [MAX_PLAYERS + 1];

	      in_sight (x, y, players);

	      for (work3 = 1; work3 < MAX_PLAYERS + 1; work3 ++)
		if (players [work3] > 0)
		  {
		    open_player_mesg (work3, &fptr);
		    fprintf (fptr, "SeeHomeDenCollapse: %s\n", hex_string);
		    fclose (fptr);
		  }

	      world_hex.terrain = RUIN;
	      world_hex.lizard_pop = 0;
	      world_hex.turns_undisturbed = 0;
	      hex_change = 1;
	    }
    }

  /* Check Lizard band hunger points! */

  if (world_hex.turns_hungry >= MAX_HUNGER_POINTS &&
      lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner > 0 &&
      world_hex.terrain != FERTILE && world.turn > 0)
    {
      for (work3 = 0, work4 = 0; work3 < lizards_in_hex (&world_hex); work3 ++)
	if (random (100) > STARVE_PER_TURN)
	  work4 ++;

      if (work4 == 0) work4 = 1;


      kill_lizards_xy (&world_hex, work4);

      open_player_mesg (world_hex.hex_owner, &fptr);
      fprintf (fptr, "Starve: %s %d %d %d %d %d %d",
	       hex_string,
	       work4,
	       world_hex.red_lizards,
	       world_hex.green_lizards,
	       world_hex.grey_lizards,
	       world_hex.black_lizards,
	       world_hex.yellow_lizards);
      fclose (fptr);
      hex_change = 1;
    }
  
  if (hex_change)
    put_hex (x, y, &world_hex);
}


/* ----------------------------------------------------------------------
--
-- DoSpies
--
---------------------------------------------------------------------- */

void DoSpies(void)
{
  int work;
  hex_t world_hex;
  FILE *fptr;
  char hex_string[80];

  for (work = 0; work < number_spys; work ++)
    {
      get_hex (spy_list [work]->x_hex, spy_list [work]->y_hex, &world_hex);
      xy_to_string (spy_list [work]->x_hex, spy_list [work]->y_hex, hex_string);

      if (lizards_in_hex (&world_hex) > 0 && spy_list [work]->player > 0 &&
	  spy_list [work]->player == world_hex.hex_owner)
	{
	  open_player_mesg (spy_list [work]->player, &fptr);
	  
	  xy_to_string (spy_list [work]->x_hex, spy_list [work]->y_hex,
			hex_string);

	  if (lizards_in_hex (&world_hex) >= MAX_LIZARDS_IN_HEX)
	    {
	      fprintf (fptr, "SpyFailRejoin: %s\n", hex_string);
	      fclose (fptr);
	    }
	  else
	    {
	      fprintf (fptr, "SpyRejoin: %s\n", hex_string);
	      switch (spy_list [work]->lizard_type)
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

	      fclose (fptr);
	      put_hex (spy_list [work]->x_hex, spy_list [work]->y_hex, &world_hex);
	      spy_list [work]->player = 0;
	    }
	}

      if (spy_list [work]->turns_alone > SPY_BUSH_TURNS && spy_list [work]->player > 0)
	{
	  open_player_mesg (spy_list [work]->player, &fptr);
	  fprintf (fptr, "SpyNative: %s\n", hex_string);
	  fclose (fptr);
	  spy_list [work]->player = 0;
	}
    }
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

int main (int argc, char *argv [])
{
  int work,
    work2,
    total_home_dens;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Shut. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 4)
    {
      data_path = LzCheckDir(argv[1], 0);
      world.turn = atoi (argv[2]);
      game_code = (char*)malloc(strlen(argv[3] + 1));
      strcpy (game_code, argv[3]);
      strupr (game_code);
    } else {
      printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	      "             Command Line Format: EXEFILE data_path world.turn game.\n"
	      "                             e.g. WORLDSHUT C:\\ 1 A\n");

      exit (EXIT_FAILURE);
    }

  get_player_list ();

  if (players_in_game == 0)
    {
      printf ("FATAL ERROR: Game %s has no players.\n", game_code);
      exit (EXIT_FAILURE);
    }

  get_spy_list ();

  get_world(&world);

  get_switch_list ();

  printf ("\nShutting world...\n");

  for (work = 0; work < world.x; work ++)
    for (work2 = 0; work2 < world.y; work2 ++)
      DoHex(work, work2);
  
  printf ("Updating spys...\n");

  DoSpies();

  printf ("Counting HOME DENS...\n");

  total_home_dens = 0;

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    player[work].home_den = 0;

  for (work = 0; work < world.x; work ++)
    {
      for (work2 = 0; work2 < world.y; work2 ++)
	{
	  get_hex (work, work2, &world_hex);

	  if (world_hex.terrain == HOME_DEN)
	    total_home_dens ++;

	  if (world_hex.terrain == HOME_DEN && world_hex.hex_owner > 0)
	    player[world_hex.hex_owner].home_den ++;
	}
    }

  put_spy_list ();

  put_home_den_status ();

  printf (" \n\nFinished!\n");

  return (EXIT_SUCCESS);
}
