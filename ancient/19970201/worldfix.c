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

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  int fertile,
      dir,
      loop,
      x,
      y,
      new_x,
      new_y;

  hex_t world_hex,
        adj_hex;

  show_title ();

  printf ("LIZARDS! World Fixup Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 3)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2] + 1));
    strcpy (game_code, argv [2]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "            Command Line Format: EXEFILE data_path game.\n"
	     "                            e.g. WORLFIX C:\\ A\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  printf ("Fixing World...");

  for (y = 0; y < world_y; y ++)
  {
    for (x = 0; x < world_x; x ++)
    {
      get_hex (x, y, &world_hex);

      switch (world_hex.terrain)
      {
	case DEN:
	  fertile = terrain_adjacent (x, y, FERTILE);

	  if (fertile == 0)
	  {
            dir = random (6) + 1;

            for (loop = 0; loop < 6; loop ++, dir ++)
            {
              if (dir > 6) dir = 1;

              move_coords (dir, x, y, &new_x, &new_y);

              if (new_x != x || new_y != y)
              {
                /* get and check hex not part of player's template */

                get_hex (new_x, new_y, &adj_hex);

                if (!adj_hex.template_hex && adj_hex.terrain != DEN)
                {
                  /* convert to fertile and stop */

                  adj_hex.terrain = FERTILE;

                  put_hex (new_x, new_y, &adj_hex);

                  break;
                }
              }
            }

            if (loop != 6) /* broken on new fertile */
            {
              world_hex.lizard_pop = DEN_LIZARD_LEVEL;

	      switch (world_hex.den_lizard_type)
	      {
	        case RED_LIZARD:
	          world_hex.red_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	          break;
	        case GREEN_LIZARD:
	          world_hex.green_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	          break;
	        case GREY_LIZARD:
	          world_hex.grey_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	          break;
	        case BLACK_LIZARD:
	          world_hex.black_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	          break;
	        case YELLOW_LIZARD:
	          world_hex.yellow_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	          break;
	      }

	      put_hex (x, y, &world_hex);
            } else {
              /* convert den to ruin */

	      world_hex.terrain = RUIN;
	      world_hex.lizard_pop = 0;

              switch (world_hex.den_lizard_type)
              {
                case RED_LIZARD:
                  world_hex.red_lizards = 0;

                  break;
                case GREEN_LIZARD:
                  world_hex.green_lizards = 0;

                  break;
                case GREY_LIZARD:
                  world_hex.grey_lizards = 0;

                  break;
                case BLACK_LIZARD:
                  world_hex.black_lizards = 0;

                  break;
                case YELLOW_LIZARD:
                  world_hex.yellow_lizards = 0;

                  break;
              }

	      world_hex.den_lizard_type = 0;

	      put_hex (x, y, &world_hex);
            }
	  } else
	    if (world_hex.lizard_pop > DEN_LIZARD_LEVEL * fertile)
	    {
	      world_hex.lizard_pop = DEN_LIZARD_LEVEL * fertile;

	      switch (world_hex.den_lizard_type)
	      {
		case RED_LIZARD:
		  world_hex.red_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

		  break;
		case GREEN_LIZARD:
		  world_hex.green_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

		  break;
		case GREY_LIZARD:
		  world_hex.grey_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

		  break;
		case BLACK_LIZARD:
		  world_hex.black_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

		  break;
		case YELLOW_LIZARD:
		  world_hex.yellow_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

		  break;
	      }

	      put_hex (x, y, &world_hex);
	    }

	  break;
	case VOLCANO:
	  if (terrain_adjacent (x, y, FERTILE) > 0 ||
	      terrain_adjacent (x, y, DEN) > 0 ||
	      terrain_adjacent (x, y, HOME_DEN) > 0 &&
              !world_hex.template_hex)
	  {
            /* change it into a peak, UNLESS IT IS ON A PLAYER TEMPLATE
               (in which case it will have already been checked for template
               validity, so may effect the world OFF the template. */

	    world_hex.terrain = PEAK;

	    put_hex (x, y, &world_hex);
	  }

	  break;
	default:
	  break;
      }
    }
  }

  /* double check for any dens affected by changes */

  for (y = 0; y < world_y; y ++)
  {
    for (x = 0; x < world_x; x ++)
    {
      get_hex (x, y, &world_hex);

      if (world_hex.terrain == DEN && !world_hex.template_hex)
      {
        fertile = terrain_adjacent (x, y, FERTILE);

        if (world_hex.lizard_pop != DEN_LIZARD_LEVEL * fertile)
	{
	  world_hex.lizard_pop = DEN_LIZARD_LEVEL * fertile;

	  switch (world_hex.den_lizard_type)
	  {
	    case RED_LIZARD:
	      world_hex.red_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	      break;
	    case GREEN_LIZARD:
	      world_hex.green_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	      break;
	    case GREY_LIZARD:
	      world_hex.grey_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	      break;
	    case BLACK_LIZARD:
	      world_hex.black_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	      break;
	    case YELLOW_LIZARD:
	      world_hex.yellow_lizards = (float) world_hex.lizard_pop * (float) DEN_MILITIA_LEVEL;

	      break;
	  }

	  put_hex (x, y, &world_hex);
	}
      }
    }
  }

  printf (" \nFinished!\n");

  return (EXIT_SUCCESS);
}
