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

char data_path [80],
     game_code [5];

int turn_number,
    world_x,
    world_y;

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

spy_t *spy_list [MAX_SPYS];
int number_spys;

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
       filename [255];

  int work,
      work2,
      work3,
      no_change;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! Handmade Map Translator. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__, COPYRIGHT_STRING);

  if (argc == 3)
  {
    strcpy (data_path, argv [1]);
    strncpy (game_code, argv [2], 3);
    game_code [3] = '\0';
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path game.\n"
	     "                             e.g. WORLTRAN C:\\ A\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  sprintf (filename, "%sLHANDMAP.%s", data_path, game_code);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  fgets (work_string, 80, fptr);
  printf ("%s\r", work_string);
  fgets (work_string, 80, fptr);
  printf ("%s\r\n", work_string);

  for (work = 0; work < world_y; work ++)
  {
    fgets (work_string, 80, fptr);

    printf ("%s\r", work_string);

    for (work2 = 0; work2 < world_x; work2 ++)
    {
      no_change = 0;

      get_hex (work2, work, &world_hex);

      switch (work_string [work2])
      {
	case 'L':
	  world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = PLAIN;

	  break;
	case 'D':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = DEN;
	  world_hex.den_lizard_type = 99;

	  break;
	case 'S':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = SWAMP;

	  break;
	case 'F':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = FERTILE;

	  break;
	case 'P':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = PEAK;

	  break;
	case 'R':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = RUIN;

	  break;
	case 'V':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = VOLCANO;

	  break;
	case 'W':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = WHIRLPOOL;

	  break;
	case '.':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = WATER;

	  break;
	case 'C':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = SCRUB;

	  break;
	case 'T':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = TEMPLE;

	  break;
	case 'X':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = CURSED;

	  break;
        case 'B':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = WATER;
          world_hex.bridge_here = TRUE;

	  break;
        case 'A':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = WATER;
          world_hex.raft_here = (RAFT_LIFE - 1) + 5;

	  break;
	case 'H':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = HOME_DEN;
	  world_hex.den_lizard_type = 0;

	  break;
	case '1':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = DEN;
	  world_hex.den_lizard_type = 91;

	  break;
	case '2':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = DEN;
	  world_hex.den_lizard_type = 92;

	  break;
	case '3':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = DEN;
	  world_hex.den_lizard_type = 93;

	  break;
	case '4':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = DEN;
	  world_hex.den_lizard_type = 94;

	  break;
	case '5':
          world_hex.lizard_pop = 0;
	  world_hex.red_lizards = 0;
	  world_hex.green_lizards = 0;
	  world_hex.grey_lizards = 0;
	  world_hex.black_lizards = 0;
	  world_hex.yellow_lizards = 0;

	  world_hex.terrain = DEN;
	  world_hex.den_lizard_type = 95;

	  break;
	case '_':
	  no_change = 1;

	  break;
        default:
          printf ("FATAL ERROR: Unknown symbol. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
      }

      if (!no_change)
	put_hex (work2, work, &world_hex);
    }
  }

  fclose (fptr);

  printf ("Fixing up Dens...");

  for (work = 0; work < world_x; work ++)
  {
    for (work2 = 0; work2 < world_y; work2 ++)
    {
      get_hex (work, work2, &world_hex);

      if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN)
	if (world_hex.den_lizard_type == 99)
	{
	  world_hex.den_lizard_type = random (LIZARD_TYPES) + 1;

	  work3 = terrain_adjacent (work, work2, FERTILE);

	  world_hex.lizard_pop = (work3 * DEN_LIZARD_LEVEL);

	  put_hex (work, work2, &world_hex);
	} else
	  if (world_hex.den_lizard_type > 90)
	  {
	    world_hex.den_lizard_type -= 90;

	    work3 = terrain_adjacent (work, work2, FERTILE);

	    world_hex.lizard_pop = (work3 * DEN_LIZARD_LEVEL);

            work3 = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	    switch (world_hex.den_lizard_type)
	    {
	      case RED_LIZARD:
		world_hex.red_lizards = work3;

		break;
	      case GREEN_LIZARD:
		world_hex.green_lizards = work3;

                break;
	      case GREY_LIZARD:
		world_hex.grey_lizards = work3;

                break;
	      case BLACK_LIZARD:
		world_hex.black_lizards = work3;

                break;
	      case YELLOW_LIZARD:
		world_hex.yellow_lizards = work3;

                break;
	    }

	    put_hex (work, work2, &world_hex);
	  }
    }
  }

  printf (" \nFinished!\n");

  return (EXIT_SUCCESS);
}
