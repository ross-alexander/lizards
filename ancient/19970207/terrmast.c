#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "filefunc.h"
#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

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
  FILE *fptr;

  char work_string [80],
       filename [255];

  int work,
      work2,
      work3,
      work4,
      work5,
      x,
      y,
      new_x,
      new_y,
      number_orders;

  order_t work_order;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! Terrain Master. %s Last compiled %s.\n\n",
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
	     "             Command Line Format: EXEFILE path_name turn_number game.\n"
	     "                             e.g. TERRMAST C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_PROC_F, turn_number, game_code);

  if ((fptr = fopen (filename, "r+b")) == NULL)
  {
    fprintf (stderr, "FATAL ERROR: Unable to read/write to %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  number_orders = num_records (fptr, sizeof (order_t));

  printf ("Working terrain...");

  for (work = 0; work < world_x; work ++)
  {
    for (work2 = 0; work2 < world_y; work2 ++)
    {
      get_hex (work, work2, &world_hex);

      switch (world_hex.terrain)
      {
	case VOLCANO:
	  if (random (100) < VOLCANO_ERUPT_LEVEL && turn_number > 0)
	  {
	    work_order.player = 0;
	    work_order.order_type = ERUPT_VOLCANO;
	    work_order.moves [0] = 0;
	    work_order.moves [1] = 0;
	    work_order.red_lizards = 0;
	    work_order.green_lizards = 0;
	    work_order.grey_lizards = 0;
	    work_order.black_lizards = 0;
	    work_order.yellow_lizards = 0;
	    work_order.x_hex = work;
	    work_order.y_hex = work2;

	    put_rec (fptr, sizeof (order_t), number_orders, &work_order);

	    number_orders ++;
	  } else {
	    if (random (100) < VOLCANO_DORMANT_LEVEL &&
		volcano_dormant_switch == 1 && turn_number > 0)
	    {
	      work_order.player = 0;
	      work_order.order_type = DORMANT_VOLCANO;
	      work_order.moves [0] = 0;
	      work_order.moves [1] = 0;
	      work_order.red_lizards = 0;
	      work_order.green_lizards = 0;
	      work_order.grey_lizards = 0;
	      work_order.black_lizards = 0;
	      work_order.yellow_lizards = 0;
	      work_order.x_hex = work;
	      work_order.y_hex = work2;

	      put_rec (fptr, sizeof (order_t), number_orders, &work_order);

	      number_orders ++;
	    }
	  }

	  break;
	case PEAK:
	  if (random (100) < MOUNTAIN_ERUPT_LEVEL &&
	      peak_active_switch == 1 && turn_number > 0)
	  {
	     work_order.player = 0;
	     work_order.order_type = ERUPT_MOUNTAIN;
	     work_order.moves [0] = 0;
	     work_order.moves [1] = 0;
	     work_order.red_lizards = 0;
	     work_order.green_lizards = 0;
	     work_order.grey_lizards = 0;
	     work_order.black_lizards = 0;
	     work_order.yellow_lizards = 0;
	     work_order.x_hex = work;
	     work_order.y_hex = work2;

	     put_rec (fptr, sizeof (order_t), number_orders, &work_order);

	     number_orders ++;
	  }

	  break;
	case WHIRLPOOL:
	  if (turn_number > 0)
	  {
	    work_order.player = 0;
	    work_order.order_type = MOVE_WHIRLPOOL;
	    work_order.red_lizards = 0;
	    work_order.green_lizards = 0;
	    work_order.grey_lizards = 0;
	    work_order.black_lizards = 0;
	    work_order.yellow_lizards = 0;
	    work_order.x_hex = work;
	    work_order.y_hex = work2;
	    work_order.moves [0] = 0;
	    work_order.moves [1] = 0;

	    work5 = terrain_adjacent (work, work2, WATER);

	    if (work5 > 0)
	    {
	      do
	      {
		work3 = random (6) + 1;

		move_coords (work3, work, work2, &x, &y);

		get_hex (x, y, &world_hex);
	      } while ((x == work && y == work2) ||
		world_hex.terrain != WATER);

	      work_order.moves [0] = work3;

	      work5 = terrain_adjacent (x, y, WATER);

	      if (work5 > 0)
	      {
		do
		{
		  work4 = random (6) + 1;

		  move_coords (work4, x, y, &new_x, &new_y);

		  get_hex (new_x, new_y, &world_hex);
		} while ((new_x == x && new_y == y) ||
			(world_hex.terrain != WATER));

		work_order.moves [1] = work4;
	      } else
		work_order.moves [1] = 0;
	    }

	    put_rec (fptr, sizeof (order_t), number_orders, &work_order);

	    number_orders ++;
	  }

	  break;
      }
    }
  }

  fclose (fptr);

  printf (" \n\nFinished!\n");

  return (0);
}
