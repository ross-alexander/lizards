#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizmisc.h"

void Terrain(world_t *world)
{
  int work,
      work2,
      work3,
      work5,
      x,
      y;

  order_t work_order;
  hex_t *hex;
  coord xy, xy2;

  if (world->turn == 0) return;
  printf ("Working terrain...");

  for (y = 0; y < world->x; y++)
    for (x = 0; x < world->y; x++)
      {
	xy.x = x; xy.y = y;
	hex = get_hex (world, xy);
	if (has_feature(hex, VOLCANO))
	  {
	    if (random (100) < VOLCANO_ERUPT_LEVEL)
	      {
		work_order.player = 0;
		work_order.type = ERUPT_VOLCANO;
		work_order.source = xy;
		new_add_order(world, &work_order);
	      }
	    else
	      {
		if (random (100) < VOLCANO_DORMANT_LEVEL &&
		    ((world->flags & LIZ_VOLCANO) == 1))
		  {
		    work_order.player = 0;
		    work_order.type = DORMANT_VOLCANO;
		    work_order.source = xy;
		    new_add_order(world, &work_order);
		  }
	      }
	  }
	if (hex->terrain == PEAK)
	  {
	    if (random (100) < MOUNTAIN_ERUPT_LEVEL &&
		((world->flags & LIZ_PEAK) == 1))
	      {
		work_order.player = 0;
		work_order.type = ERUPT_MOUNTAIN;
		work_order.source = xy;
		new_add_order(world, &work_order);
	      }
	  }
	if (has_feature(hex, WHIRLPOOL))
	  {
	    work_order.player = 0;
	    work_order.type = MOVE_WHIRLPOOL;
	    work_order.source = xy;
	    work5 = terrain_adjacent (world, xy, WATER);
	    if (work5 > 0)
	      {
		do
		  {
		    work3 = random (6) + 1;
		    xy2 = move_coords (world, hex, work3);
		    hex = get_hex (world, xy2);
		  } while ((x == work && y == work2) ||
			   hex->terrain != WATER);
		work_order.dest = xy2;
		new_add_order(world, &work_order);
		/* currently only moves one hex */
	      }
	  }
      }
  printf("\n");
}
