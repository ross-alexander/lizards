#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "orders.h"
#include "moreords.h"
#include "terrain.h"
#include "blddest.h"

#include "lizhex.h"
#include "lizmesg.h"
#include "lizmisc.h"
#include "lizord.h"

extern order_t *order_list [MAX_ORDERS_PER_TURN];

extern char dir [7][3];

extern char *message_list [MAX_RANDOM_MESSAGES];

void erupt_volcano_order (order_t *order)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int players [MAX_PLAYERS + 2],
    new_line [MAX_PLAYERS + 2];

  int work,
      work2,
      x,
      y;

  hex_t	hex;

  order_t work_order;

  get_hex (world, order->x_hex, order->y_hex, &hex);

  if (hex.terrain != VOLCANO)
    return;

  for (work = 0; work < MAX_PLAYERS + 2; work ++)
    new_line [work] = 0;

  xy_to_string (order->x_hex, order->y_hex, work_string);

  in_sight (world, order->x_hex, order->y_hex, players);

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - Volcano at %s ERUPTS!\n", work_string);

      new_line [work] = 1;

      /* fclose(fptr); */
    }

  /* Yes! It's supposed to be 0 to 6 so as to check the actual
     hex erupting for lizards as well! */

  for (work = 0; work < 7; work ++)
  {
    move_coords (world, work, order->x_hex, order->y_hex, &x, &y);

    get_hex (world, x, y, &hex);

    xy_to_string (x, y, work_string);

    if (hex.bridge_here == 1)
    {
      work_order.player = hex.hex_owner;
      work_order.x_hex = x;
      work_order.y_hex = y;

      collapse_bridge_order (&work_order, work_order.player, 1);

      get_hex (world, x, y, &hex);
    }

    if ((work2 = lizards_in_hex (&hex)) > 0 && hex.hex_owner > 0)
    {
      open_player_mesg (hex.hex_owner, &fptr);

      if (work == 0)
      {
	fprintf (fptr, " - Lizards in hex %s are destroyed by eruption!\n",
		 work_string);

	hex.red_lizards = 0;
	hex.green_lizards = 0;
	hex.grey_lizards = 0;
	hex.black_lizards = 0;
	hex.yellow_lizards = 0;
      } else {
	work2 = (float) lizards_in_hex (&hex) * LAVA_KILL_LEVEL;

	if (work2 > 1)
	  work2 = random (work2) + 1;
	else
	  work2 = 1;

	if (new_line [hex.hex_owner] == 0)
	{
	  fprintf (fptr, "\n - %d Lizards in hex %s are killed by flying lava!\n",
		   work2, work_string);

	  new_line [hex.hex_owner] = 1;
	} else
	  fprintf (fptr, " - %d Lizards in hex %s are killed by flying lava!\n",
		   work2, work_string);

	kill_lizards_xy (&hex, work2);

	fprintf (fptr, " - %s Lizards in hex %s survive eruption.\n",
		 show_lizards (hex.red_lizards,
		 hex.green_lizards, hex.grey_lizards,
		 hex.black_lizards, hex.yellow_lizards, temp_string),
		 work_string);

	/* fclose(fptr); */

	hex_attacked_or_moved_into (world, x, y, &hex);

	open_player_mesg (hex.hex_owner, &fptr);

	if (hex.terrain == DEN || hex.terrain == HOME_DEN)
	{
	  if (hex.terrain == HOME_DEN)
	    if (new_line [hex.hex_owner] == 0)
	    {
	       fprintf (fptr, "\n - Home Den at %s destroyed by lava flow!\n",
		       work_string);

	       new_line [hex.hex_owner] = 1;
	    } else
	      fprintf (fptr, " - Home Den at %s destroyed by lava flow!\n",
		       work_string);
	  else
	    if (new_line [hex.hex_owner] == 0)
	    {
	      fprintf (fptr, "\n - Den at %s destroyed by lava flow!\n",
		       work_string);

	      new_line [hex.hex_owner] = 1;
	    } else
	      fprintf (fptr, " - Den at %s destroyed by lava flow!\n",
		       work_string);

	  hex.terrain = RUIN;
	  hex.den_lizard_type = 0;
	  hex.turns_den_hungry = 0;
	  hex.lizard_pop = 0;
	}
      }

      /* fclose(fptr); */
    }

    for (work2 = 0; work2 < world->num_spies; work2 ++)
      if (world->spylist [work2]->x_hex == x &&
	  world->spylist [work2]->y_hex == y &&
	  world->spylist [work2]->player > 0)
      {
	open_player_mesg (world->spylist [work2]->player, &fptr);

	if (work == 0 || random (100) < SPY_LAVA_KILL_LEVEL)
	{
	  if (new_line [world->spylist [work2]->player] == 0)
	  {
	    fprintf (fptr, "\n - Spy Lizard at %s killed by flying lava!\n",
		     work_string);

	    new_line [world->spylist [work2]->player] = 1;
	  } else
	    fprintf (fptr, " - Spy Lizard at %s killed by flying lava!\n",
		     work_string);

	  world->spylist [work2]->player = 0;
	}

	/* fclose(fptr); */
      }

    if (work > 0 && random (100) < VOLCANO_SINK_LEVEL)
    {
      switch (hex.terrain)
      {
	case WATER:
	  if (random (100) < 50)
	    hex.terrain = PLAIN;
	  else
	    hex.terrain = SWAMP;

	  break;
	case PLAIN:
	case FERTILE:
	case SWAMP:
	  hex.terrain = WATER;

	  break;
	default: /* Ruins, Peaks, Volcanos, Whirlpools unaffected! */
	  break;
      }
    }

    put_hex (world, x, y, &hex);
  }
}

/* special case since bridges collapse all over the place, the messaging
   required differs from time to time */

void collapse_bridge_order (order_t *order, unsigned int active_player,
			    unsigned int no_spacing)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int players [MAX_PLAYERS + 2];
#ifdef FULL
  int new_line [MAX_PLAYERS + 2];
#endif

  int work;

  hex_t world_hex;

#ifdef FULL
  for (work = 0; work < MAX_PLAYERS + 2; work ++)
    new_line [work] = 0;
#endif

  get_hex (world, order->x_hex, order->y_hex, &world_hex);

  in_sight (world, order->x_hex, order->y_hex, players);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  if (world_hex.bridge_here == 1)
  {
    world_hex.bridge_here = 0;

    put_hex (world, order->x_hex, order->y_hex, &world_hex);

    for (work = 1; work < MAX_PLAYERS + 1; work ++)
      if (players [work] > 0)
      {
	open_player_mesg (work, &fptr);

	if (work == active_player || no_spacing == 1)
	  fprintf (fptr, " - Bridge at %s collapses!\n", work_string);
	else
	  fprintf (fptr, "\n - Bridge at %s collapses!\n", work_string);
#ifdef FULL
	new_line [work] = 1;
#endif
	/* fclose(fptr); */
      }

      if (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner > 0)
      {
	open_player_mesg (world_hex.hex_owner, &fptr);

	if (world_hex.terrain == WHIRLPOOL)
	{
	  fprintf (fptr, " - %s Lizards at %s fall into Whirlpool below!\n",
		   show_lizards (world_hex.red_lizards, world_hex.green_lizards,
		   world_hex.grey_lizards, world_hex.black_lizards,
		   world_hex.yellow_lizards, temp_string), work_string);

	  /* fclose(fptr); */

	  whirlpool_kill (world, order->x_hex, order->y_hex);
	} else {
	  fprintf (fptr, " - %s Lizards at %s fall into Water!\n",
		   show_lizards (world_hex.red_lizards, world_hex.green_lizards,
		   world_hex.grey_lizards, world_hex.black_lizards,
		   world_hex.yellow_lizards, temp_string), work_string);

	  /* fclose(fptr); */

	  bridge_collapse_kill (world, order->x_hex, order->y_hex);
	}
      }
  } else
    return;
}

void move_whirlpool_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int players [MAX_PLAYERS + 2],
    new_line [MAX_PLAYERS + 2];

  int work,
      work2,
      x,
      y;

  hex_t old_hex,
	new_hex;

  get_hex (world, order->x_hex, order->y_hex, &old_hex);

  if (old_hex.terrain != WHIRLPOOL)
    return;

  for (work = 0; work < MAX_PLAYERS + 2; work ++)
    new_line [work] = 0;

  for (work = 0; work < 2; work ++)
  {
    get_hex (world, order->x_hex, order->y_hex, &old_hex);

    in_sight (world, order->x_hex, order->y_hex, players);

    move_coords (world, order->moves [work], order->x_hex, order->y_hex, &x, &y);

    xy_to_string (order->x_hex, order->y_hex, work_string);

    get_hex (world, x, y, &new_hex);

    if (new_hex.terrain != WATER || (x == order->x_hex &&
	y == order->y_hex))
      return;
    else {
      for (work2 = 1; work2 <= world->num_players; work2 ++)
	if (players [work2] > 0)
	{
	  open_player_mesg (work2, &fptr);

	  xy_to_string (order->x_hex, order->y_hex, work_string);

	  if (new_line [work2] == 0)
	  {
	    fprintf (fptr, "\n - Whirlpool at %s moves %s (to ",
		     work_string, dir [order->moves [work]]);

	    new_line [work2] = 1;
	  } else
	    fprintf (fptr, " - Whirlpool at %s moves %s (to ",
		     work_string, dir [order->moves [work]]);

	  xy_to_string (x, y, work_string);

	  fprintf (fptr, "%s)!\n", work_string);

	  /* fclose(fptr); */
	}

      old_hex.terrain = WATER;
      new_hex.terrain = WHIRLPOOL;

      put_hex (world, x, y, &new_hex);
      put_hex (world, order->x_hex, order->y_hex, &old_hex);

      if (new_hex.bridge_here == 0 && new_hex.raft_here == 0)
	whirlpool_kill (world, x, y);

      order->x_hex = x;
      order->y_hex = y;
    }
  }
}

void erupt_mountain_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int players [MAX_PLAYERS + 2];

  int work;

  hex_t world_hex;

  if ((world->flags & LZ_PEAK) == 0)
    return;

  get_hex (world, order->x_hex, order->y_hex, &world_hex);

  if (world_hex.terrain != PEAK)
    return;

  world_hex.terrain = VOLCANO;

  put_hex (world, order->x_hex, order->y_hex, &world_hex);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  in_sight (world, order->x_hex, order->y_hex, players);

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - Rumbling fills %s as the Peak here becomes an active Volcano.\n", work_string);

      /* fclose(fptr); */
    }
}

void dormant_volcano_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int players [MAX_PLAYERS + 2];

  int work;

  hex_t world_hex;

  if ((world->flags & LZ_VOLCANO) == 0)
    return;

  get_hex (world, order->x_hex, order->y_hex, &world_hex);

  if (world_hex.terrain != VOLCANO)
    return;

  xy_to_string (order->x_hex, order->y_hex, work_string);

  in_sight (world, order->x_hex, order->y_hex, players);

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - The rumbling at %s subsides as the Volcano here becomes dormant.\n", work_string);

      /* fclose(fptr); */
    }

  world_hex.terrain = PEAK;

  put_hex (world, order->x_hex, order->y_hex, &world_hex);
}
