#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizards.h"

#include "lizhex.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- in_sight
--
---------------------------------------------------------------------- */

void in_sight (world_t *world_t, int x, int y, int *players)
{
  /* In y,x order! */

  int one_hex_odd  [6][2] = {{-1,0},{-1,-1},{-1,1},{0,-1},{0,1},{1,0}};
  int one_hex_even [6][2] = {{-1,0},{0,-1},{0,1},{1,-1},{1,1},{1,0}};
  int two_hex_odd  [12][2] = {{-2,0},{-2,-1},{-2,1},{-1,-2},{-1,2},{0,-2},
			      {0,2},{1,-2},{1,2},{1,-1},{1,1},{2,0}};
  int two_hex_even [12][2] = {{-2,0},{-1,-1},{-1,1},{-1,-2},{-1,2},{0,-2},
			      {0,2},{1,-2},{1,2},{2,-1},{2,1},{2,0}};

  int three_hex_odd [18][2] = {{-3,0},{-3,-1},{-3,1},{-2,-2},{-2,2},{-2,-3},
			       {-2,3},{-1,-3},{-1,3},{0,-3},{0,3},{1,-3},
			       {1,3},{2,-2},{2,2},{2,-1},{2,1},{3,0}};

  int three_hex_even [18][2] = {{-3,0},{-2,-1},{-2,1},{-2,-2},{-2,2},{-1,-3},
				{-1,3},{0,-3},{0,3},{1,-3},{1,3},{2,-3},
				{2,3},{2,-2},{2,2},{3,-1},{3,1},{3,0}};

  int work;
  hex_t hex;
  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    players [work] = 0;

  if ((x % 2) == 1)
  {
    players_here (world_t, x, y, players);

    for (work = 0; work < 6; work ++)
      players_here (world_t, (x + one_hex_odd [work][1]),
		    (y + one_hex_odd [work][0]),
		    players);

    for (work = 0; work < 12; work ++)
      players_here (world_t,
		    (x + two_hex_odd [work][1]),
		    (y + two_hex_odd [work][0]),
		    players);

    for (work = 0; work < 18; work ++)
    {
      get_hex(world_t, (x + three_hex_odd [work][1]), (y + three_hex_odd [work][0]), &hex);

      if (hex.terrain == PEAK)
	players_here (world_t,
		      (x + three_hex_odd [work][1]),
		      (y + three_hex_odd [work][0]),
		      players);
    }
  } else {
    players_here (world_t, x, y, players);

    for (work = 0; work < 6; work ++)
      players_here (world_t,
		    (x + one_hex_even [work][1]),
		    (y + one_hex_even [work][0]),
		    players);

    for (work = 0; work < 12; work ++)
      players_here (world_t,
		    (x + two_hex_even [work][1]),
		    (y + two_hex_even [work][0]),
		    players);

    for (work = 0; work < 18; work ++)
    {
      get_hex(world_t, (x + three_hex_even [work][1]), (y + three_hex_even [work][0]), &hex);

      if (hex.terrain == PEAK)
	players_here (world_t,
		      (x + three_hex_even [work][1]),
		      (y + three_hex_even [work][0]),
		      players);
    }
  }
}
