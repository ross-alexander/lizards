#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizmisc.h"
#include "lizards.h"
#include "create.h"

static void old_move_coords (world_t *world_t,
		  int direction, int x, int y, int *new_x, int *new_y)
{
  *new_x = x;
  *new_y = y;

  switch (direction)
  {
    case NORTH:
      *new_y -= 1;
      break;
    case NORTH_EAST:
      *new_x += 1;
      if ((x % 2) == 1)
	*new_y -= 1;
      break;
    case SOUTH_EAST:
      *new_x += 1;
      if ((x % 2) != 1)
	*new_y += 1;
      break;
    case SOUTH:
      *new_y += 1;
      break;
    case SOUTH_WEST:
      *new_x -= 1;
      if ((x % 2) != 1)
	*new_y += 1;
      break;
    case NORTH_WEST:
      *new_x -= 1;
      if ((x % 2) == 1)
	*new_y -= 1;
      break;
  }

  if (*new_x < 0)
    *new_x += world_t->x;
  else
    if (*new_x >= world_t->x)
      *new_x -= world_t->x;

  if (*new_y < 0)
    *new_y += world_t->y;
  else
    if (*new_y >= world_t->y)
      *new_y -= world_t->y;
}

void WorldInit(world_t *w)
{
  int x, y, i;
  hex_t *hex;
  coord xy;
  char title[5];

  w->hexes = (hex_t*)malloc(sizeof(hex_t) * w->x * w->y);
  for (y = 0; y < w->y; y++)
    for (x = 0; x < w->x; x++)
      {
	xy.x = x; xy.y = y;
	hex = get_hex(w, xy);
	hex->xy = xy;
	hex->terrain = WATER;
	hex->owner = 0;
	xy_to_string(x, y, title);
	hex->title = strdup(title);
	for (i = NORTH; i <= NORTH_WEST; i++)
	  {
	    old_move_coords(w, i, x, y,
			&hex->edge[i-1].x,
			&hex->edge[i-1].y);
	    hex->edge[i-1].invalid = 0;
	  }
      }
}
