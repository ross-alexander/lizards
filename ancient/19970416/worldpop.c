#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "lizgame.h"
#include "lizmisc.h"
#include "lizhex.h"

#define HOME_SIZE 400

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  typedef struct
  {
    int x,
	y;
  } den_xy_t;

  char work_string [80],
       grid_status [50][50];

  int home_den_colour = RED_LIZARD,
      start_lizard_colour = RED_LIZARD,
      num_dens,
      work,
      work2,
      work3,
      work4,
      x,
      y,
      x2,
      y2,
      breaker,
      breaker2,
      breaker3;

  unsigned int div_x,
	       div_y,
	       grid_x,
	       grid_y,
	       div_size;

  hex_t world_hex,
	hex;

  den_xy_t *den_xy_list [100];

  randomize ();

  show_title ();

  printf ("LIZARDS! World Populate Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 5)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2] + 1));
    strcpy (game_code, argv [2]);
    strupr (game_code);
    home_den_colour = atoi (argv [3]);
    start_lizard_colour = atoi (argv [4]);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n\n"
	    "   Command Line Format: EXEFILE data_path game home_colour band_color.\n"
	    "     e.g. WORLPOP C:\\ A 1 1\n\n"
	    "   Lizard colour codes are 1=Red, 2=Green, 3=Grey, 4=Black, 5=Yellow.\n\n");

    exit (EXIT_FAILURE);
  }

  world = (World*)malloc(sizeof(World));
  get_world (world);

  if (world->num_players == 0)
  {
    printf ("FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  for (work = 0; work < 50; work ++)
    for (work2 = 0; work2 < 50; work2 ++)
      grid_status [work][work2] = 0;

  for (work = 1; work < world->num_players + 1; work ++)
    world->player[work].home_den = 1;

  div_size = sqrt (world->num_players);

  div_x = div_size;

  div_y = div_size;

  while (div_x * div_y < world->num_players)
    if (random (2) == 0)
      div_x ++;
    else
      div_y ++;

  grid_x = ceil ((float) world->x / div_x);
  grid_y = ceil ((float) world->y / div_y);

  for (work = 1; work < world->num_players + 1; work ++)
  {
    printf ("Player %d: ", work);

    breaker = 0;

    do
    {
      breaker2 = 0;

      do
      {
	x = random (div_x);
	y = random (div_y);

	breaker2 ++;

	if (breaker2 == 100)
        {
          printf ("FATAL ERROR: No free locations for new player.\n");
	  exit (EXIT_FAILURE);
	}
      } while (grid_status [x][y] == 1);

      grid_status [x][y] = 1;

      num_dens = 0;

      for (work2 = grid_x * x; work2 < grid_x * (x + 1); work2 ++)
	for (work3 = grid_y * y; work3 < grid_y * (y + 1); work3 ++)
	{
	  get_hex (world, work2, work3, &world_hex);

	  if (world_hex.terrain == DEN)
	  {
	    if ((den_xy_list [num_dens] = malloc (sizeof (den_xy_t))) == NULL)
	    {
              printf ("FATAL ERROR: Out of memory.\n");
	      exit (EXIT_FAILURE);
	    }

	    den_xy_list [num_dens]->x = work2;
	    den_xy_list [num_dens]->y = work3;

	    num_dens ++;
	  }
	}

      breaker ++;

      if (breaker == 100)
      {
        printf ("FATAL ERROR: No free dens in location for new player.\n");
	exit (EXIT_FAILURE);
      }
    } while (num_dens == 0);

    printf ("%d Dens in grid. ", num_dens);

    breaker2 = 0;

    do
    {
      breaker3 = 0;

      do
      {
	 work2 = random (num_dens);

	 breaker3 ++;
      } while ((den_xy_list [work2]->x == 0 || den_xy_list [work2]->x == world->x - 1 ||
	       den_xy_list [work2]->y == 0 || den_xy_list [work2]->y == world->y) &&
	       breaker3 != 100);

      if (breaker3 == 100)
      {
        printf ("FATAL ERROR: No free dens in location for new player.\n");
	exit (EXIT_FAILURE);
      }

      get_hex (world, den_xy_list [work2]->x, den_xy_list [work2]->y, &world_hex);

      world_hex.hex_owner = work;
      world_hex.terrain = HOME_DEN;

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

      world_hex.den_lizard_type = home_den_colour;

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
          printf ("FATAL ERROR: Unknown colour code on command line.\n");
	  exit (EXIT_FAILURE);
      }

      if ((work3 = terrain_adjacent (world, den_xy_list [work2]->x, den_xy_list [work2]->y,
	  FERTILE)) > 1)
      {
	breaker = 0;

	for (work4 = 1; work4 < 7 && work3 > 1; work4 ++)
	{
	  move_coords (world, work4,
		       den_xy_list [work2]->x, den_xy_list [work2]->y,
		       &x2, &y2);

	  if (den_xy_list [work2]->x != x2 || den_xy_list [work2]->y != y2)
	  {
	    get_hex (world, x2, y2, &hex);

	    if (hex.terrain == FERTILE)
	    {
	      hex.terrain = PLAIN;

	      work3 --;

	      put_hex (world, x2, y2, &hex);
	    }
	  }
	}

	world_hex.lizard_pop = 50;
      }

      breaker2 ++;

      if (breaker2 == 100)
      {
        printf ("FATAL ERROR: Home den adjustment failed.\n");

        

	exit (EXIT_FAILURE);
      }
    } while (breaker == 100);

    printf ("Home Den placed at %s.\n", xy_to_string (den_xy_list [work2]->x,
	    den_xy_list [work2]->y, work_string));

    put_hex (world, den_xy_list [work2]->x, den_xy_list [work2]->y, &world_hex);

    x = den_xy_list [work2]->x;
    y = den_xy_list [work2]->y;

    for (work2 = 0; work2 < num_dens; work2 ++)
    {
      if (den_xy_list [work2] != NULL)
        free (den_xy_list [work2]);

      den_xy_list [work2] = NULL;
    }
  }

  put_world(world);
  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}
