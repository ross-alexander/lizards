#define HOME_DEN_SCORE 50
#define DEN_SCORE 25
#define BRIDGE_SCORE 10
#define TEMPLE_SCORE 30

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  FILE *fptr;

  int scores [MAX_PLAYERS + 1],
      lizard_heads [MAX_PLAYERS + 1],
      scorers [MAX_PLAYERS + 1],
      work,
      work2,
      work3,
      work4,
      work5,
      x,
      y,
      total_home_dens = 0,
      home_dens_for_victory = 0,
      victory_points_for_victory = 0,
      winner_score = 0,
      winner_player = 0,
      tie_flag = LZ_FALSE;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! Clan Rank Program. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 3)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2]) + 1);
    strcpy (game_code, argv[2]);
    strupr (game_code);
  } else {
    fprintf (stderr, "FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path world.turn game.\n"
	     "                             e.g. CLANRANK C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  world = (World*)malloc(sizeof(World));
  get_world(world);

  if (world->num_players == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    scorers [work] = 0;

  printf ("Messaging Players...");

  open_player_mesg (0, &fptr);

  fprintf (fptr, "\n");

  for (work2 = 1; work2 <= world->num_players; work2 ++)
	fprintf (fptr, " %d - Player %02d, '%s' (%3s) Clan.\n",
		 world->player[work2].home_den, work2, world->player[work2].clan_name,
                 world->player[work2].code);

  fprintf (fptr, "\n");

  fclose (fptr);

  for (work = 0; work <= world->num_players; work ++)
  {
    open_player_mesg (work, &fptr);

    fprintf (fptr, "\n * Clans Ranking at end of Turn %d:\n\n", world->turn);

    fclose (fptr);

    scores [work] = 0;
    lizard_heads [work] = 0;
    scorers [work] = work;
  }

  printf (" \nCounting Heads...");

  for (x = 0; x < world->x; x ++)
  {
    for (y = 0; y < world->y; y ++)
    {
      get_hex (world, x, y, &world_hex);

      if (world_hex.terrain == HOME_DEN)
	total_home_dens ++;

      if ((world_hex.terrain == DEN || world_hex.terrain == HOME_DEN) &&
	  world_hex.hex_owner > 0)
      {
	scores [world_hex.hex_owner] += world_hex.lizard_pop;
	lizard_heads [world_hex.hex_owner] += world_hex.lizard_pop;

	if (world_hex.terrain == HOME_DEN)
	{
	  if (!(world->flags & LZ_VICTORY))
	    scores [world_hex.hex_owner] += HOME_DEN_SCORE;
	} else
	  scores [world_hex.hex_owner] += DEN_SCORE;

	scores [world_hex.hex_owner] += lizards_in_hex (&world_hex);
	lizard_heads [world_hex.hex_owner] += lizards_in_hex (&world_hex);
      } else
	if (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner > 0)
	{
	  scores [world_hex.hex_owner] += lizards_in_hex (&world_hex);
	  lizard_heads [world_hex.hex_owner] += lizards_in_hex (&world_hex);
	}

      if (world_hex.bridge_here > 0 && world_hex.hex_owner > 0)
	scores [world_hex.hex_owner] += BRIDGE_SCORE;

      if (world_hex.terrain == TEMPLE && world_hex.hex_owner > 0)
	scores [world_hex.hex_owner] += TEMPLE_SCORE;
    }
  }

  printf (" \nCounting Spys...");

  for (work = 0; work < world->num_spies; work ++)
  {
    if (world->spylist [work]->player > 0)
    {
      scores [world->spylist [work]->player] ++;
      lizard_heads [world->spylist [work]->player] ++;
    }
  }

  printf (" \nSorting Clans...");

  /* if victory points switch, add the points they get for owning
     each home den */

  if (world->flags & LZ_VICTORY)
    for (work = 1; work <= world->num_players; work ++)
      if (world->player[work].home_den > 0)
	scores [work] += ((int) world->player[work].home_den *
	  world->home_owned_worth);

  do
  {
    work2 = 0;

    for (work = 1; work <= world->num_players; work ++)
    {
      if (scores [work] < scores [work + 1])
      {
	work2 = 1;

	work3 = scores [work];
	work4 = scorers [work];
	work5 = lizard_heads [work];

	scores [work] = scores [work + 1];
	scorers [work] = scorers [work + 1];
	lizard_heads [work] = lizard_heads [work + 1];
	scores [work + 1] = work3;
	scorers [work + 1] = work4;
	lizard_heads [work + 1] = work5;
      }
    }
  } while (work2 != 0);

  for (work = 1; work <= world->num_players; work ++)
    if (lizard_heads [work] == 0)
      scores [work] = 0;

  printf (" \nMessaging Players...");

  for (work = 0; work <= world->num_players; work ++)
  {
    open_player_mesg (work, &fptr);

    for (work2 = 1; work2 <= world->num_players; work2 ++)
      if (scores [work2] == 0)
	fprintf (fptr, " %02d - Player %02d - EXTINCT - '%s' (%3s) Clan.\n",
		 work2, scorers [work2], world->player[scorers [work2]].clan_name,
		 world->player[scorers [work2]].code);
      else
	fprintf (fptr, " %02d - %6d Player %02d, '%s' (%3s) Clan.\n",
		 work2, scores [work2], scorers [work2],
		 world->player[scorers [work2]].clan_name, world->player[scorers [work2]].code);

    fclose (fptr);
  }

  /* check victory conditions if required! */

  printf (" \nChecking VICTORY CONDITIONS...");

  if (world->flags & LZ_VICTORY)
  {
    /* Check Victory POINTS Conditions! */

    victory_points_for_victory =
      total_home_dens * world->home_victory_points;

    for (work = 0; work <= world->num_players; work ++)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - THERE ARE %d HOME DENS LEFT IN THIS GAME. YOU MUST GAIN %d VICTORY POINTS TO WIN.\n",
	       total_home_dens, victory_points_for_victory);

      if (work > 0)
        if (world->player[work].home_den == 0)
	  fprintf (fptr, " - YOU DO NOT CONTROL ANY HOME DENS! YOU CANNOT RECRUIT THIS TURN!\n");
        else
	  if (world->player[work].home_den == 1)
	    fprintf (fptr, " - YOU PRESENTLY CONTROL %d HOME DEN.\n", world->player[work].home_den);
	  else
	    fprintf (fptr, " - YOU PRESENTLY CONTROL %d HOME DENS.\n", world->player[work].home_den);

      fclose (fptr);
    }

    for (work = 1; work <= world->num_players; work ++)
    {
      for (work2 = 0; work2 <= world->num_players; work2 ++)
	if (scorers [work2] == work)
	  break;

      if (world->player[work].home_den >= 1 &&
	  victory_points_for_victory > 0 &&
	  scores [work2] >= victory_points_for_victory)
      {
        if (winner_player > 0)
        {
          if (scores [work2] > winner_score)
          {
            winner_player = work;
            winner_score = scores [work2];
          } else
            if (scores [work2] == winner_score)
            {
              tie_flag = LZ_TRUE;
            }
        } else {
          winner_player = work;
          winner_score = scores [work2];
        }
      }
    }

    if (winner_player > 0 && !tie_flag)
    {
      for (work3 = 0; work3 <= world->num_players; work3 ++)
      {
        open_player_mesg (work3, &fptr);

        if (work3 == winner_player)
	{
	  fprintf (fptr, "\n * '%s' Clan YOU HAVE WON THIS GAME OF LIZARDS!\n",
		     world->player[winner_player].clan_name);
	  fprintf (fptr, " * CONGRATULATIONS '%s' Clan!\n\n",
		     world->player[winner_player].clan_name);
	  fprintf (fptr, " - Game %s has reached victory conditions and has ended.\n",
		     game_code);
	} else {
	  fprintf (fptr, "\n * '%s' Clan HAS WON THIS GAME OF LIZARDS!\n",
		    world->player[winner_player].clan_name);
	  fprintf (fptr, " * CONGRATULATIONS '%s' Clan!\n\n",
		     world->player[winner_player].clan_name);
	  fprintf (fptr, " - Game %s has reached victory conditions and has ended.\n",
		     game_code);
	}

	fclose (fptr);
      }

      printf (" \n\n'%s' Clan\nHAS WON THIS GAME OF LIZARDS!\n",
		world->player[winner_player].clan_name);
      printf ("Game %s has reached victory conditions and has ended.\n",
	      game_code);
    }
  } else {
    /* old, 'half the home dens' victory condition */

    /* Check Victory Conditions! */

    if (total_home_dens > 0)
      home_dens_for_victory = total_home_dens / 2;
    else
      home_dens_for_victory = 0;

    if (home_dens_for_victory * 2 <= total_home_dens)
      home_dens_for_victory ++;

    for (work = 0; work <= world->num_players; work ++)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - THERE ARE %d HOME DENS LEFT IN THIS GAME. YOU MUST CONTROL %d HOME DENS TO WIN.\n",
	       total_home_dens, home_dens_for_victory);

      if (work > 0)
        if (world->player[work].home_den == 0)
	  fprintf (fptr, " - YOU DO NOT CONTROL ANY HOME DENS! YOU CANNOT RECRUIT THIS TURN!\n");
        else
	  if (world->player[work].home_den == 1)
	    fprintf (fptr, " - YOU PRESENTLY CONTROL %d HOME DEN.\n", world->player[work].home_den);
	  else
	    fprintf (fptr, " - YOU PRESENTLY CONTROL %d HOME DENS.\n", world->player[work].home_den);

      fclose (fptr);
    }

    for (work = 1; work <= world->num_players; work ++)
      if (world->player[work].home_den >= home_dens_for_victory &&
	  home_dens_for_victory > 0)
      {
	for (work3 = 0; work3 <= world->num_players; work3 ++)
	{
	  open_player_mesg (work3, &fptr);

	  if (work3 == work)
	  {
	    fprintf (fptr, "\n * '%s' Clan YOU HAVE WON THIS GAME OF LIZARDS!\n",
		     world->player[work].clan_name);
	    fprintf (fptr, " * CONGRATULATIONS '%s' Clan!\n\n",
		     world->player[work].clan_name);
	    fprintf (fptr, " - Game %s has reached victory conditions and has ended.\n",
		     game_code);
	  } else {
	    fprintf (fptr, "\n * '%s' Clan HAS WON THIS GAME OF LIZARDS!\n",
		     world->player[work].clan_name);
	    fprintf (fptr, " * CONGRATULATIONS '%s' Clan!\n\n",
		     world->player[work].clan_name);
	    fprintf (fptr, " - Game %s has reached victory conditions and has ended.\n",
		     game_code);
	  }

	  fclose (fptr);
	}

	printf (" \n\n'%s' Clan\nHAS WON THIS GAME OF LIZARDS!\n",
		world->player[work].clan_name);
	printf ("Game %s has reached victory conditions and has ended.\n",
		game_code);
      }
  }

  for (work = 0; work < world->num_spies; work ++)
    free (world->spylist [work]);

  printf (" \n\nFinished!\n");

  return (EXIT_SUCCESS);
}
