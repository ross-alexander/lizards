#include <conio.h>
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

char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_clan_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char players_in_game;

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_cred [MAX_PLAYERS + 1][10];
char player_output_mode [MAX_PLAYERS + 1];
char game_due_date [20];

int world_x,
    world_y,
    number_orders,
    turn_number;

spy_t *spy_list [MAX_SPYS];
int number_spys;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][7] = { "", "Red", "Green", "Grey", "Black", "Yellow" };

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
	     victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

int main (int argc, char *argv [])
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work,
      work2,
      work3,
      work4,
      hex_change,
      total_home_dens;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Shut. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 4)
  {
    data_path = LzCheckDir(argv[1], 0);
    turn_number = atoi (argv[2]);
    game_code = (char*)malloc(strlen(argv[3] + 1));
    strcpy (game_code, argv[3]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path turn_number game.\n"
	     "                             e.g. WORLDSHUT C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  get_world ();

  get_switch_list ();

  printf ("Shutting world...");

  for (work = 0; work < world_x; work ++)
  {
    for (work2 = 0; work2 < world_y; work2 ++)
    {
      get_hex (work, work2, &world_hex);

      hex_change = 0;

      if (world_hex.raft_here > 0)
      {
	world_hex.raft_here --;

	hex_change = 1;

	if (world_hex.raft_here == 0)
	{
	  open_player_mesg (world_hex.hex_owner, &fptr);

	  xy_to_string (work, work2, work_string);

	  fprintf (fptr, "\n - Raft at %s is waterlogged and sinks", work_string);

	  if (lizards_in_hex (&world_hex) > 0 && world_hex.terrain == WHIRLPOOL)
	  {
	    fprintf (fptr, " into the whirlpool here!\n");

	    work3 = (float) lizards_in_hex (&world_hex) * WHIRLPOOL_KILL_LEVEL;

	    if (work3 > 1)
	      work3 = random (work3) + 1;
	    else
	      work3 = 1;

	    fprintf (fptr, " - %d Lizards at %s are sucked down to their deaths in Whirlpool!\n",
		   work3, work_string);

	    kill_lizards_xy (&world_hex, work3);

	    if (lizards_in_hex (&world_hex) == 0)
	      fprintf (fptr, " - NO Lizards resist the Whirlpool!\n");
	    else
	      fprintf (fptr, " - %s Lizards at %s resist the Whirlpool!\n",
		       show_lizards (world_hex.red_lizards,
		       world_hex.green_lizards, world_hex.grey_lizards,
		       world_hex.black_lizards, world_hex.yellow_lizards,
		       temp_string), work_string);
	  } else
	    fprintf (fptr, "!\n");

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

	    xy_to_string (work, work2, work_string);

	    fprintf (fptr, "\n - Half built Den at %s collapses due to lack of attention.\n",
		     work_string);

	    fclose (fptr);

	    world_hex.turns_undisturbed = 0;

	    hex_change = 1;
	  } else
	    if (world_hex.terrain == HOME_DEN)
	    {
	      char players [MAX_PLAYERS + 1];

	      in_sight (work, work2, players);

	      xy_to_string (work, work2, work_string);

	      for (work3 = 1; work3 < MAX_PLAYERS + 1; work3 ++)
		if (players [work3] > 0)
		{
		  open_player_mesg (work3, &fptr);

		  fprintf (fptr, "\n - Rumbling fills %s as the Home Den there collapses in on itself.\n",
			   work_string);

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
	  world_hex.terrain != FERTILE && turn_number > 0)
      {
	for (work3 = 0, work4 = 0; work3 < lizards_in_hex (&world_hex); work3 ++)
	  if (random (100) > STARVE_PER_TURN)
	    work4 ++;

	if (work4 == 0) work4 = 1;

	open_player_mesg (world_hex.hex_owner, &fptr);

	xy_to_string (work, work2, work_string);

	fprintf (fptr, "\n - Lizards at %s starve!", work_string);

	fprintf (fptr, " %d Lizards die.", work4);

	kill_lizards_xy (&world_hex, work4);

	if (lizards_in_hex (&world_hex) == 0)
	{
	  fprintf (fptr, " ALL the Lizards in that Band have STARVED TO DEATH!\n");
	} else {
	  fprintf (fptr, " %s Lizards are still alive at %s.\n",
		   show_lizards (world_hex.red_lizards,
		   world_hex.green_lizards,
		   world_hex.grey_lizards,
		   world_hex.black_lizards,
		   world_hex.yellow_lizards,
		   temp_string), work_string);
	}

	fclose (fptr);

	hex_change = 1;
      }

      if (hex_change)
	put_hex (work, work2, &world_hex);
    }
  }

  printf (" \nUpdating spys...");

  for (work = 0; work < number_spys; work ++)
  {
    get_hex (spy_list [work]->x_hex, spy_list [work]->y_hex, &world_hex);

    if (lizards_in_hex (&world_hex) > 0 && spy_list [work]->player > 0 &&
	spy_list [work]->player == world_hex.hex_owner)
    {
      open_player_mesg (spy_list [work]->player, &fptr);

      xy_to_string (spy_list [work]->x_hex, spy_list [work]->y_hex,
		      work_string);

      if (lizards_in_hex (&world_hex) >= MAX_LIZARDS_IN_HEX)
      {
	fprintf (fptr, "\n - Spy Lizard cannot re-join Lizards at %s as hex is too crowded.\n",
		 work_string);

	fclose (fptr);
      } else {
	fprintf (fptr, "\n - Spy Lizard re-joins Lizards at %s.\n",
		 work_string);

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

    if (spy_list [work]->turns_alone > SPY_BUSH_TURNS &&
	spy_list [work]->player > 0)
    {
      open_player_mesg (spy_list [work]->player, &fptr);

      xy_to_string (spy_list [work]->x_hex, spy_list [work]->y_hex,
		    work_string);

      fprintf (fptr, "\n - Spy Lizard at %s returns to the wild!\n", work_string);

      fclose (fptr);

      spy_list [work]->player = 0;
    }
  }

  printf (" \nCounting HOME DENS...");

  total_home_dens = 0;

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    player_home_den [work] = 0;

  for (work = 0; work < world_x; work ++)
  {
    for (work2 = 0; work2 < world_y; work2 ++)
    {
      get_hex (work, work2, &world_hex);

      if (world_hex.terrain == HOME_DEN)
	total_home_dens ++;

      if (world_hex.terrain == HOME_DEN && world_hex.hex_owner > 0)
	player_home_den [world_hex.hex_owner] ++;
    }
  }

  put_spy_list ();

  put_home_den_status ();

  printf (" \n\nFinished!\n");

  return (EXIT_SUCCESS);
}
