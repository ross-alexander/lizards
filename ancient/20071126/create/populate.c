#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizmisc.h"
#include "lizhex.h"
#include "create.h"

typedef coord den_xy_t;

void WorldPopulate(world_t *world, int home_den_colour, int start_lizard_colour)
{
  char work_string [80];
  int *grid_status = (int*)malloc(sizeof(int) * world->x * world->y);
  int num_dens,
    work,
    work2,
    work3,
    work4,
    x,
    y,
    breaker,
    breaker2,
    breaker3,
    div_x,
    div_y,
    grid_x,
    grid_y,
    div_size;
  coord xy;
  hex_t *world_hex, *hex;
  feature_t *den, *band;
  den_xy_t **den_xy_list = (den_xy_t**)alloca(sizeof(den_xy_t*) * world->x * world->y);
  for (work = 0; work < world->x; work ++)
    for (work2 = 0; work2 <world->y; work2 ++)
      grid_status [work2 * world->x + work] = 0;

  div_size = (int)(sqrt (world->num_players));
  div_x = div_size;
  div_y = div_size;

  while (div_x * div_y < world->num_players)
    if (random (2) == 0)
      div_x ++;
    else
      div_y ++;

  grid_x = (int)(ceil ((double) world->x / div_x));
  grid_y = (int)(ceil ((double) world->y / div_y));

  for (work = 1; work < world->num_players; work ++)
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
	      breaker2++;
	      if (breaker2 == 100)
		{
		  printf ("FATAL ERROR: No free locations for new player.\n");
		  exit (EXIT_FAILURE);
		}
	    } while (grid_status [world->y * x + y] == 1);
	  grid_status [world->y * x + y] = 1;
	  num_dens = 0;
	  for (work2 = grid_x * x; work2 < grid_x * (x + 1); work2 ++)
	    for (work3 = grid_y * y; work3 < grid_y * (y + 1); work3 ++)
	      {
		world_hex = get_hex (world, coord_cons(work2, work3));
		if (has_feature(world_hex, DEN))
		  {
		    if ((den_xy_list [num_dens] = (den_xy_t*)malloc (sizeof (den_xy_t))) == NULL)
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
	    } while ((den_xy_list [work2]->x == 0 ||
		      den_xy_list [work2]->x == world->x - 1 ||
		      den_xy_list [work2]->y == 0 ||
		      den_xy_list [work2]->y == world->y) &&
		     breaker3 != 100);
	  if (breaker3 == 100)
	    {
	      printf ("FATAL ERROR: No free dens in location for new player.\n");
	      exit (EXIT_FAILURE);
	    }
	  world_hex = get_hex (world, *den_xy_list [work2]);
	  den = has_feature(world_hex, DEN);
	  band = has_feature(world_hex, BAND);
	  world_hex->owner = work;
	  den->f.den.home = 1;
	  world_hex->tmpl = 1;
	  band->f.band.type[den->f.den.type - 1] = 0;
	  den->f.den.type = home_den_colour;
	  band->f.band.type[den->f.den.type - 1] = HOME_SIZE;

	  if ((work3 = feature_adjacent (world, *den_xy_list [work2],
					 FERTILE)) > 1)
	    {
	      breaker = 0;
	      for (work4 = 1; work4 < 7 && work3 > 1; work4 ++)
		{
		  xy = move_coords (world, world_hex, work4);
		  if (!xy.invalid)
		    {
		      hex = get_hex (world, xy);
		      if (has_feature(hex, FERTILE))
			{
			  free(release_feature(world, has_feature(hex, FERTILE)));
			  work3 --;
			}
		    }
		}
	      den->f.den.pop = 50;
	    }
	  breaker2 ++;
	  if (breaker2 == 100)
	    {
	      printf ("FATAL ERROR: Home den adjustment failed.\n");
	      exit (EXIT_FAILURE);
	    }
	} while (breaker == 100);
      
      printf ("Home Den placed at %s.\n", xy_to_string (den_xy_list [work2]->x,
							den_xy_list [work2]->y,
							work_string));
      
      x = den_xy_list [work2]->x;
      y = den_xy_list [work2]->y;
      for (work2 = 0; work2 < num_dens; work2 ++)
	{
	  if (den_xy_list [work2] != NULL)
	    free (den_xy_list [work2]);
	  den_xy_list [work2] = NULL;
	}
    }
  free(grid_status);
}
