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

typedef struct
{
  int x,
      y,
      spy,
      peak;
} unit_t;

char mock_screen [31][133],
     mock_screen_full;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

int main (int argc, char *argv []);
void show_world (int x, int y, int hex_view, int player, int spy);
void show_hex (int x, int y, int scr_x, int scr_y, int player, int default_all);
void wipe_mock_screen (void);
void hex_fill_mock_screen (int peak);
void str_replace (int x, int y, char *string);

int main (int argc, char *argv [])
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int number_units [MAX_PLAYERS + 1],
      number_peaks,
      work,
      work2,
      work3,
      x_mod,
      x,
      y;

  hex_t world_hex;

  unit_t work_unit;

  unit_t *player_units [MAX_PLAYERS + 1][500];

  randomize ();

  show_title ();

  printf ("LIZARDS! Clan View Reporter. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 4)
  {
    strcpy (data_path, argv [1]);
    turn_number = atoi (argv [2]);
    strncpy (game_code, argv [3], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path turn_number game.\n"
	     "                             e.g. CLANVIEW C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  for (work = 1; work < players_in_game + 1; work ++)
  {
    open_player_mesg (work, &fptr);

    fprintf (fptr, "\n * '%s' Clan World View at end of Turn %d:\n\n",
	     player_clan_name [work], turn_number);

    number_units [work] = 0;

    fclose (fptr);
  }

  printf ("Building lizard views...");

  for (x = 0; x < world_x; x ++)
  {
    for (y = 0; y < world_y; y ++)
    {
      get_hex (x, y, &world_hex);

      if ((world_hex.hex_owner > 0 && lizards_in_hex (&world_hex) > 0) ||
	  (world_hex.hex_owner > 0 && (world_hex.terrain == DEN ||
	   world_hex.terrain == HOME_DEN) && world_hex.lizard_pop > 0))
      {
	work = world_hex.hex_owner;

	work_unit.x = x;
	work_unit.y = y;
	work_unit.spy = 0;

	if (world_hex.terrain == PEAK)
	  work_unit.peak = 1;
	else
	  work_unit.peak = 0;

	if ((player_units [work][number_units [work]] =
	     malloc (sizeof (unit_t))) == NULL)
        {
          printf (" \nFATAL ERROR: Out of memory. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	*player_units [work][number_units [work]] = work_unit;

	number_units [work] ++;
      }
    }
  }

  printf (" \nBuilding spy views...");

  for (work = 0; work < number_spys; work ++)
  {
    if (spy_list [work]->player > 0)
    {
      work2 = spy_list [work]->player;

      work_unit.x = spy_list [work]->x_hex;
      work_unit.y = spy_list [work]->y_hex;
      work_unit.spy = 1;

      get_hex (work_unit.x, work_unit.y, &world_hex);

      if (world_hex.terrain == PEAK)
	work_unit.peak = 1;
      else
	work_unit.peak = 0;

      if ((player_units [work2][number_units [work2]] =
	  malloc (sizeof (unit_t))) == NULL)
      {
        printf (" \nFATAL ERROR: Out of memory. Press SPACE.\n");

        getch ();

	exit (EXIT_FAILURE);
      }

      *player_units [work2][number_units [work2]] = work_unit;

      number_units [work2] ++;
    }
  }

  printf (" \nBuilding player views...");

  for (work = 1; work < players_in_game + 1; work ++)
    for (work2 = 0; work2 < number_units [work]; work2 ++)
      for (work3 = 0; work3 < number_units [work]; work3 ++)
	if (player_units [work][work2]->x == player_units [work][work3]->x &&
	    player_units [work][work2]->y == player_units [work][work3]->y &&
	    work2 != work3)
	{
	  player_units [work][work2]->x = 999;
	  player_units [work][work2]->y = 999;
	}

  wipe_mock_screen ();

  for (work = 1; work < players_in_game + 1; work ++)
  {
    number_peaks = 0;

    open_player_mesg (work, &fptr);

    for (work2 = 0; work2 < number_units [work]; work2 ++)
    {
      if (player_units [work][work2]->peak == 0 &&
	  player_units [work][work2]->x != 999 &&
	  player_units [work][work2]->y != 999)
      {
	xy_to_string (player_units [work][work2]->x,
		      player_units [work][work2]->y,
		      work_string);

	if (mock_screen_full == 0)
	  x_mod = 0;
	else
	  if (mock_screen_full == 1)
	    x_mod = 46;
	  else
	    x_mod = 92;

	if (player_units [work][work2]->spy)
	{
	  strcpy (temp_string, "Spy Lizard at ");
	  strcat (temp_string, work_string);
	  strcat (temp_string, " can see:");

	  str_replace (x_mod, 0, temp_string);
	} else {
	  get_hex (player_units [work][work2]->x, player_units [work][work2]->y,
		   &world_hex);

	  if (world_hex.terrain == DEN)
	  {
	    strcpy (temp_string, "Lizards in Den at ");
	    strcat (temp_string, work_string);
	    strcat (temp_string, " can see:");

	    str_replace (x_mod, 0, temp_string);
	  } else
	    if (world_hex.terrain == HOME_DEN)
	    {
	      strcpy (temp_string, "Lizards in Home Den at ");
	      strcat (temp_string, work_string);
	      strcat (temp_string, " can see:");

	      str_replace (x_mod, 0, temp_string);
	    } else
	      if (world_hex.raft_here > 0)
	      {
		strcpy (temp_string, "Lizards on raft at ");
		strcat (temp_string, work_string);
		strcat (temp_string, " can see:");

		str_replace (x_mod, 0, temp_string);
	      } else {
		strcpy (temp_string, "Lizards at ");
		strcat (temp_string, work_string);
		strcat (temp_string, " can see:");

		str_replace (x_mod, 0, temp_string);
	      }
	}

	show_world (player_units [work][work2]->x,
		    player_units [work][work2]->y, 2, work,
		    player_units [work][work2]->spy);

	if (mock_screen_full == 3)
	{
	  for (work3 = 0; work3 < 22; work3 ++)
	    fprintf (fptr, "%s\n", mock_screen [work3]);

	  fprintf (fptr, "\n");

	  wipe_mock_screen ();
	}

	player_units [work][work2]->x = 999;
	player_units [work][work2]->y = 999;
      } else
	if (player_units [work][work2]->x != 999 &&
	    player_units [work][work2]->y != 999)
	  number_peaks ++;
    }

    if (mock_screen_full > 1 || (number_peaks == 0 && mock_screen_full != 0))
    {
      for (work3 = 0; work3 < 22; work3 ++)
	fprintf (fptr, "%s\n", mock_screen [work3]);

      fprintf (fptr, "\n");

      wipe_mock_screen ();
    }

    for (work2 = 0; work2 < number_units [work]; work2 ++)
    {
      if (player_units [work][work2]->peak == 1 &&
          player_units [work][work2]->x != 999 &&
	  player_units [work][work2]->y != 999)
      {
	xy_to_string (player_units [work][work2]->x,
		      player_units [work][work2]->y,
		      work_string);

	if (mock_screen_full == 0)
	  x_mod = 0;
	else
	  x_mod = 82;

	if (player_units [work][work2]->spy)
	{
	  strcpy (temp_string, "Spy Lizard at ");
	  strcat (temp_string, work_string);
	  strcat (temp_string, " can see:");

	  str_replace (x_mod, 0, temp_string);
	} else {
	  get_hex (player_units [work][work2]->x, player_units [work][work2]->y,
		   &world_hex);

	  if (world_hex.terrain == DEN)
	  {
	    strcpy (temp_string, "Lizards in Den at ");
	    strcat (temp_string, work_string);
	    strcat (temp_string, " can see:");

	    str_replace (x_mod, 0, temp_string);
	  } else
	    if (world_hex.terrain == HOME_DEN)
	    {
	      strcpy (temp_string, "Lizards in Home Den at ");
	      strcat (temp_string, work_string);
	      strcat (temp_string, " can see:");

	      str_replace (x_mod, 0, temp_string);
	    } else
	      if (world_hex.raft_here > 0)
	      {
		strcpy (temp_string, "Lizards on raft at ");
		strcat (temp_string, work_string);
		strcat (temp_string, " can see:");

		str_replace (x_mod, 0, temp_string);
	      } else {
		strcpy (temp_string, "Lizards at ");
		strcat (temp_string, work_string);
		strcat (temp_string, " can see:");

		str_replace (x_mod, 0, temp_string);
	      }

	    str_replace (x_mod, 0, temp_string);
	}

	show_world (player_units [work][work2]->x,
		    player_units [work][work2]->y, 3, work,
		    player_units [work][work2]->spy);

	if (mock_screen_full == 2)
	{
	  for (work3 = 0; work3 < 30; work3 ++)
	    fprintf (fptr, "%s\n", mock_screen [work3]);

	  fprintf (fptr, "\n");

	  wipe_mock_screen ();
	}

	player_units [work][work2]->x = 999;
	player_units [work][work2]->y = 999;
      }
    }

    if (mock_screen_full != 0)
    {
      for (work3 = 0; work3 < 30; work3 ++)
	fprintf (fptr, "%s\n", mock_screen [work3]);

      fprintf (fptr, "\n");

      wipe_mock_screen ();
    }

    fclose (fptr);
  }

  for (work = 1; work < players_in_game + 1; work ++)
    for (work2 = 0; work2 < number_units [work]; work2 ++)
      free (player_units [work][work2]);

  for (work = 0; work < number_spys; work ++)
    free (spy_list [work]);

  printf (" \n\nFinished!\n");

  return (EXIT_SUCCESS);
}

void show_world (int x, int y, int hex_view, int player, int spy)
{
  int x_mod;

  hex_t hex;

  /* in y,x order! */

  char one_hex_odd  [6][2] = { -1,0,-1,-1,-1,1,0,-1,0,1,1,0 },
       one_hex_even [6][2] = { -1,0,0,-1,0,1,1,-1,1,1,1,0 },
       one_hex_disp [6][2] = { 6,13,8,7,8,19,12,7,12,19,14,13 },
       two_hex_odd  [12][2] = { -2,0,-2,-1,-2,1,-1,-2,-1,2,0,-2,0,2,1,-2,
				1,2,1,-1,1,1,2,0 },
       two_hex_even [12][2] = { -2,0,-1,-1,-1,1,-1,-2,-1,2,0,-2,0,2,1,-2,
				1,2,2,-1,2,1,2,0 },
       two_hex_disp [12][2] = { 2,13,4,7,4,19,6,1,6,25,10,1,10,25,14,1,14,25,
				16,7,16,19,18,13 },
       three_hex_odd [18][2] = { -3,0,-3,-1,-3,1,-2,-2,-2,2,-2,-3,-2,3,
				  -1,-3,-1,3,0,-3,0,3,1,-3,1,3,2,-2,2,2,
				  2,-1,2,1,3,0 },
       three_hex_even [18][2] = { -3,0,-2,-1,-2,1,-2,-2,-2,2,-1,-3,-1,3,
				  0,-3,0,3,1,-3,1,3,2,-3,2,3,2,-2,2,2,3,-1,
				  3,1,3,0 },
       one3_hex_disp [6][2] = { 10,19,12,13,12,25,16,13,16,25,18,19 },
       two3_hex_disp [12][2] = { 6,19,8,13,8,25,10,7,10,31,14,7,14,31,18,7,
				 18,31,20,13,20,25,22,19 },
       three_hex_disp [18][2] = { 2,19,4,13,4,25,6,7,6,31,8,1,8,37,12,1,
				  12,37,16,1,16,37,20,1,20,37,22,7,22,31,
				  24,13,24,25,26,19 };

  int work;

  if (hex_view == 3)
    if (mock_screen_full == 0)
      x_mod = 0;
    else
      x_mod = 82;
  else
    if (mock_screen_full == 0)
      x_mod = 0;
    else
      if (mock_screen_full == 1)
	x_mod = 46;
      else
	x_mod = 92;

  if (hex_view == 2)
  {
    hex_fill_mock_screen (0);

    if ((x % 2) == 1)
    {
      get_hex (x, y, &hex);

      if (spy && lizards_in_hex (&hex) == 0)
      {
	str_replace (14 + x_mod, 10, player_code [player]);
	str_replace (13 + x_mod, 11, "SPYliz");
      }

      show_hex (x, y, 13 + x_mod, 10, player, 1);

      for (work = 0; work < 6; work ++)
	show_hex ((x + one_hex_odd [work][1]), (y + one_hex_odd [work][0]),
		  one_hex_disp [work][1] + x_mod, one_hex_disp [work][0],
		  player, 0);

      for (work = 0; work < 12; work ++)
	show_hex ((x + two_hex_odd [work][1]), (y + two_hex_odd [work][0]),
		  two_hex_disp [work][1] + x_mod, two_hex_disp [work][0],
		  player, 0);
    } else {
      get_hex (x, y, &hex);

      if (spy && lizards_in_hex (&hex) == 0)
      {
	str_replace (14 + x_mod, 10, player_code [player]);
	str_replace (13 + x_mod, 11, "SPYliz");
      }

      show_hex (x, y, 13 + x_mod, 10, player, 1);

      for (work = 0; work < 6; work ++)
	show_hex ((x + one_hex_even [work][1]), (y + one_hex_even [work][0]),
		  one_hex_disp [work][1] + x_mod, one_hex_disp [work][0],
		  player, 0);

      for (work = 0; work < 12; work ++)
	show_hex ((x + two_hex_even [work][1]), (y + two_hex_even [work][0]),
		  two_hex_disp [work][1] + x_mod, two_hex_disp [work][0],
		  player, 0);
    }
  } else {
    hex_fill_mock_screen (1);

    if ((x % 2) == 1)
    {
      get_hex (x, y, &hex);

      if (spy && lizards_in_hex (&hex) == 0)
      {
	str_replace (20 + x_mod, 14, player_code [player]);
	str_replace (19 + x_mod, 15, "SPYliz");
      }

      show_hex (x, y, 19 + x_mod, 14, player, 1);

      for (work = 0; work < 6; work ++)
	show_hex ((x + one_hex_odd [work][1]), (y + one_hex_odd [work][0]),
		  one3_hex_disp [work][1] + x_mod, one3_hex_disp [work][0],
		  player, 0);

      for (work = 0; work < 12; work ++)
	show_hex ((x + two_hex_odd [work][1]), (y + two_hex_odd [work][0]),
		  two3_hex_disp [work][1] + x_mod, two3_hex_disp [work][0],
		  player, 0);

      for (work = 0; work < 18; work ++)
	show_hex ((x + three_hex_odd [work][1]), (y + three_hex_odd [work][0]),
		  three_hex_disp [work][1] + x_mod, three_hex_disp [work][0],
		  player, 0);
    } else {
      get_hex (x, y, &hex);

      if (spy && lizards_in_hex (&hex) == 0)
      {
	str_replace (20 + x_mod, 14, player_code [player]);
	str_replace (19 + x_mod, 15, "SPYliz");
      }

      show_hex (x, y, 19 + x_mod, 14, player, 1);

      for (work = 0; work < 6; work ++)
	show_hex ((x + one_hex_even [work][1]), (y + one_hex_even [work][0]),
		  one3_hex_disp [work][1] + x_mod, one3_hex_disp [work][0],
		  player, 0);

      for (work = 0; work < 12; work ++)
	show_hex ((x + two_hex_even [work][1]), (y + two_hex_even [work][0]),
		  two3_hex_disp [work][1] + x_mod, two3_hex_disp [work][0],
		  player, 0);

      for (work = 0; work < 18; work ++)
	show_hex ((x + three_hex_even [work][1]), (y + three_hex_even [work][0]),
		  three_hex_disp [work][1] + x_mod, three_hex_disp [work][0],
		  player, 0);
    }
  }
}

void show_hex (int x, int y, int scr_x, int scr_y, int player, int default_all)
{
  char den_colour [6][4] = { "un-", "red", "grn", "gry", "blk", "yel" },
       work_string [80],
       number [10];

  int work;

  hex_t hex;

  if (world_wrap_switch)
  {
    if (x < 0)
      x += world_x;
    else
      if (x >= world_x)
	x -= world_x;

    if (y < 0)
      y += world_y;
    else
      if (y >= world_y)
	y -= world_y;
  } else {
    if (x < 0 || y < 0 || x >= world_x || y >= world_y)
    {
      return;
    }
  }

  get_hex (x, y, &hex);

  if (hex.hex_owner == player || default_all == 1)
  {
    if (lizards_in_hex (&hex) > 0 || hex.terrain == DEN || hex.terrain == HOME_DEN)
    {
      str_replace (scr_x + 1, scr_y, player_code [hex.hex_owner]);

      work = (hex.red_lizards + hex.green_lizards + hex.grey_lizards +
	      hex.black_lizards + hex.yellow_lizards + hex.lizard_pop);

      itoa (work, work_string, 10);
      strcat (work_string, "liz");

      str_replace (scr_x, scr_y + 1, work_string);
    }
  } else {
    if (hex.terrain != DEN && hex.terrain != RUIN && hex.terrain != SCRUB &&
	hex.terrain != HOME_DEN)
    {
      if (lizards_in_hex (&hex) > 0)
      {
	if (hex.hex_owner > 0)
	  str_replace (scr_x + 1, scr_y, player_code [hex.hex_owner]);

	work = (hex.red_lizards + hex.green_lizards + hex.grey_lizards +
		hex.black_lizards + hex.yellow_lizards + hex.lizard_pop);

	itoa (work, work_string, 10);
	strcat (work_string, "liz");

	str_replace (scr_x, scr_y + 1, work_string);
      }
    } else {
      if (hex.terrain == DEN || hex.terrain == HOME_DEN)
      {
	if (hex.hex_owner > 0)
	  str_replace (scr_x + 1, scr_y, player_code [hex.hex_owner]);
	else
	  str_replace (scr_x + 1, scr_y, "free");
      }
    }
  }

  if (hex.bridge_here == 1 || hex.raft_here > 0)
  {
    if (hex.bridge_here == 1)
      str_replace (scr_x, scr_y + 2, BRIDGE_C);
    else
      str_replace (scr_x, scr_y + 2, RAFT_C);
  } else {
    switch (hex.terrain)
    {
      case PLAIN:
	str_replace (scr_x, scr_y + 2, PLAIN_C);
	break;
      case DEN:
	str_replace (scr_x, scr_y + 2,
		     den_colour [hex.den_lizard_type]);
	str_replace (scr_x + 3, scr_y + 2, DEN_C);
	break;
      case HOME_DEN:
	str_replace (scr_x, scr_y + 2,
		     den_colour [hex.den_lizard_type]);
	str_replace (scr_x + 3, scr_y + 2, HOME_DEN_C);
	break;
      case RUIN:
	str_replace (scr_x, scr_y + 2, RUIN_C);
	break;
      case FERTILE:
	str_replace (scr_x, scr_y + 2, FERTILE_C);
	break;
      case SWAMP:
	str_replace (scr_x, scr_y + 2, SWAMP_C);
	break;
      case PEAK:
	str_replace (scr_x, scr_y + 2, PEAK_C);
	break;
      case VOLCANO:
	str_replace (scr_x, scr_y + 2, VOLCANO_C);
	break;
      case WATER:
	str_replace (scr_x, scr_y + 2, WATER_C);
	break;
      case WHIRLPOOL:
	str_replace (scr_x, scr_y + 2, WHIRLPOOL_C);
	break;
      case SCRUB:
	str_replace (scr_x, scr_y + 2, SCRUB_C);
	break;
      case TEMPLE:
	str_replace (scr_x, scr_y + 2, TEMPLE_C);
	break;
      case CURSED:
	str_replace (scr_x, scr_y + 2, CURSED_C);
    }
  }

  strcpy (work_string, itoa (y + 1, number, 10));
  strcat (work_string, loc_to_string (x, number));

  str_replace (scr_x + 1, scr_y + 3, work_string);
}

void wipe_mock_screen (void)
{
  char work_string [133];

  int work;

  mock_screen_full = 0;

  for (work = 0; work < 132; work ++)
    work_string [work] = ' ';

  work_string [132] = NULL;

  for (work = 0; work < 31; work ++)
    strcpy (mock_screen [work], work_string);
}

void hex_fill_mock_screen (int peak)
{
  int x;

  if (peak)
    if (mock_screen_full == 0)
    {
      x = 0;

      mock_screen_full = 1;
    } else {
      x = 82;

      mock_screen_full = 2;
    }
  else
    if (mock_screen_full == 0)
    {
      x = 0;

      mock_screen_full = 1;
    } else {
      if (mock_screen_full == 1)
      {
	x = 46;

	mock_screen_full = 2;
      } else {
	x = 92;

	mock_screen_full = 3;
      }
    }

  if (peak)
  {
    str_replace (x, 1, "                    ____");
    str_replace (x, 2, "                   /    \\");
    str_replace (x, 3, "              ____/      \\____");
    str_replace (x, 4, "             /    \\      /    \\");
    str_replace (x, 5, "        ____/      \\____/      \\____");
    str_replace (x, 6, "       /    \\      /    \\      /    \\");
    str_replace (x, 7, "  ____/      \\____/      \\____/      \\____");
    str_replace (x, 8, " /    \\      /    \\      /    \\      /    \\");
    str_replace (x, 9, "/      \\____/      \\____/      \\____/      \\");
    str_replace (x, 10, "\\      /    \\      /    \\      /    \\      /");
    str_replace (x, 11, " \\____/      \\____/      \\____/      \\____/");
    str_replace (x, 12, " /    \\      /    \\      /    \\      /    \\");
    str_replace (x, 13, "/      \\____/      \\____/      \\____/      \\");
    str_replace (x, 14, "\\      /    \\      /    \\      /    \\      /");
    str_replace (x, 15, " \\____/      \\____/      \\____/      \\____/");
    str_replace (x, 16, " /    \\      /    \\      /    \\      /    \\");
    str_replace (x, 17, "/      \\____/      \\____/      \\____/      \\");
    str_replace (x, 18, "\\      /    \\      /    \\      /    \\      /");
    str_replace (x, 19, " \\____/      \\____/      \\____/      \\____/");
    str_replace (x, 20, " /    \\      /    \\      /    \\      /    \\");
    str_replace (x, 21, "/      \\____/      \\____/      \\____/      \\");
    str_replace (x, 22, "\\      /    \\      /    \\      /    \\      /");
    str_replace (x, 23, " \\____/      \\____/      \\____/      \\____/");
    str_replace (x, 24, "      \\      /    \\      /    \\      /");
    str_replace (x, 25, "       \\____/      \\____/      \\____/");
    str_replace (x, 26, "            \\      /    \\      /");
    str_replace (x, 27, "             \\____/      \\____/");
    str_replace (x, 28, "                  \\      /");
    str_replace (x, 29, "                   \\____/");
  } else {
    str_replace (x, 1, "              ____");
    str_replace (x, 2, "             /    \\");
    str_replace (x, 3, "        ____/      \\____");
    str_replace (x, 4, "       /    \\      /    \\");
    str_replace (x, 5, "  ____/      \\____/      \\____");
    str_replace (x, 6, " /    \\      /    \\      /    \\");
    str_replace (x, 7, "/      \\____/      \\____/      \\");
    str_replace (x, 8, "\\      /    \\      /    \\      /");
    str_replace (x, 9, " \\____/      \\____/      \\____/");
    str_replace (x, 10, " /    \\      /    \\      /    \\");
    str_replace (x, 11, "/      \\____/      \\____/      \\");
    str_replace (x, 12, "\\      /    \\      /    \\      /");
    str_replace (x, 13, " \\____/      \\____/      \\____/ ");
    str_replace (x, 14, " /    \\      /    \\      /    \\");
    str_replace (x, 15, "/      \\____/      \\____/      \\");
    str_replace (x, 16, "\\      /    \\      /    \\      /");
    str_replace (x, 17, " \\____/      \\____/      \\____/");
    str_replace (x, 18, "      \\      /    \\      /");
    str_replace (x, 19, "       \\____/      \\____/");
    str_replace (x, 20, "            \\      /");
    str_replace (x, 21, "             \\____/");
  }
}

void str_replace (int x, int y, char *string)
{
  int work;

  for (work = 0; work < strlen (string); work ++)
    mock_screen [y][x + work] = string [work];
}
