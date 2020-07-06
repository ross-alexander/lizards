#include <conio.h>
#include <math.h>
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

#define HOME_SIZE 400

char data_path [80],
     game_code [5];

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

int world_x,
    world_y,
    number_orders,
    turn_number;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][7] = { "", "Red", "Green", "Grey", "Black", "Yellow" };

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  typedef struct
  {
    int x,
	y;
  } den_xy_t;

  char work_string [80];

  int num_home_dens,
      home_den_colour = RED_LIZARD,
      start_lizard_colour = RED_LIZARD,
      work,
      work2,
      x,
      y;

  hex_t world_hex,
	hex;

  den_xy_t *home_den_xy_list [MAX_PLAYERS + 1];

  randomize ();

  show_title ();

  printf ("LIZARDS! Handmade Populate Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 5)
  {
    strcpy (data_path, argv [1]);
    strncpy (game_code, argv [2], 3);
    game_code [3] = NULL;
    strupr (game_code);
    home_den_colour = atoi (argv [3]);
    start_lizard_colour = atoi (argv [4]);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n\n"
	    "   Command Line Format: EXEFILE data_path game home_colour band_color.\n"
	    "     e.g. WORLHPOP C:\\ A 1 1\n\n"
	    "   Lizard colour codes are 1=Red, 2=Green, 3=Grey, 4=Black, 5=Yellow.\n\n");

    exit (EXIT_FAILURE);
  }

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  /* build home den list to see wether homes should be placed randomly
     or simply assigned randomly from existing empty home dens
     (presumably from a handmade map) */

  num_home_dens = 0;

  for (y = 0; y < world_y; y ++)
    for (x = 0; x < world_x; x ++)
    {
      get_hex (x, y, &hex);

      if (hex.terrain == HOME_DEN)
      {
	if ((home_den_xy_list [num_home_dens] = malloc (sizeof (den_xy_t))) == NULL)
        {
          printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	home_den_xy_list [num_home_dens]->x = x;
	home_den_xy_list [num_home_dens]->y = y;

	num_home_dens ++;
      }
    }

  if (num_home_dens != players_in_game)
  {
    printf ("FATAL ERROR: Pre-generated home dens were found, but the number\n"
	    "             of players does NOT match the number of empty\n"
            "             home dens on map. Check your handmade map! Press SPACE.\n\n");

    getch ();

    exit (EXIT_FAILURE);
  }

  for (work = 1; work < players_in_game + 1; work ++)
    player_home_den [work] = 1;

  for (work = 1; work < players_in_game + 1; work ++)
  {
    printf ("Player %d: ", work);

    do {
      work2 = random (num_home_dens);
    } while (home_den_xy_list [work2]->x == -1);

    get_hex (home_den_xy_list [work2]->x, home_den_xy_list [work2]->y,
	     &world_hex);

    world_hex.hex_owner = work;

    world_hex.green_lizards = 0;
    world_hex.grey_lizards = 0;
    world_hex.black_lizards = 0;
    world_hex.yellow_lizards = 0;

    if (home_den_colour > LIZARD_TYPES)
    {
      printf ("FATAL ERROR: Unknown colour code on command line. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    world_hex.den_lizard_type = home_den_colour;

    world_hex.lizard_pop = (terrain_adjacent (home_den_xy_list [work2]->x,
      home_den_xy_list [work2]->y, FERTILE) * DEN_LIZARD_LEVEL);

    switch (start_lizard_colour)
    {
      case RED_LIZARD:
	world_hex.red_lizards = HOME_SIZE;

	break;
      case GREEN_LIZARD:
	world_hex.green_lizards = HOME_SIZE;

	break;
      case GREY_LIZARD:
	world_hex.grey_lizards = HOME_SIZE;

	break;
      case BLACK_LIZARD:
	world_hex.black_lizards = HOME_SIZE;

	break;
      case YELLOW_LIZARD:
	world_hex.yellow_lizards = HOME_SIZE;

	break;
      default:
        printf ("FATAL ERROR: Unknown colour code on command line. Press SPACE.\n");

        getch ();

	exit (EXIT_FAILURE);
    }

    printf ("Home Den filled at %s.\n", xy_to_string (home_den_xy_list [work2]->x,
	    home_den_xy_list [work2]->y, work_string));

    put_hex (home_den_xy_list [work2]->x, home_den_xy_list [work2]->y, &world_hex);

    x = home_den_xy_list [work2]->x;
    y = home_den_xy_list [work2]->y;

    home_den_xy_list [work2]->x = -1;
    home_den_xy_list [work2]->y = -1;
  }

  put_home_den_status ();

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}
