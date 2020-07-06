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
#include "chants.h"

#include "lizhex.h"
#include "lizmisc.h"


int flying_eye_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char temp_string [80],
       temp_string2 [80];

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  get_hex(world, x, y, &world_hex);

  xy_to_string (x, y, temp_string);

  if (lizards_in_hex (&world_hex) + world_hex.lizard_pop == 0)
  {
    fprintf (fptr, "and they see no Lizards at %s.\n", temp_string);

    /* fclose(fptr); */

    return (0);
  }

  if (x == order->x_hex && y == order->y_hex)
  {
    fprintf (fptr, "and they go cross-eyed but nothing happens!\n");

    /* fclose(fptr); */

    return (0);
  }

  if (world_hex.hex_owner == order->player)
    fprintf (fptr, "they see your Lizards at %s with great clarity.\n",
	     temp_string);
  else
    if (world_hex.hex_owner > 0)
      fprintf (fptr, "they see Lizards from '%s' Clan at %s with great clarity.\n",
	       world->player[world_hex.hex_owner].clan_name, temp_string);
    else
      fprintf (fptr, "they see free Lizards at %s with great clarity.\n",
	       temp_string);

  if (world_hex.lizard_pop > 0)
    if (lizards_in_hex (&world_hex) == 0)
      fprintf (fptr, " - Your Black Lizards can see %d tame Lizards there.\n",
	world_hex.lizard_pop);
    else
      fprintf (fptr, " - Your Black Lizards can see %s (%s & %s) Lizards and %d tame Lizards there.\n",
	       show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	       world_hex.grey_lizards, world_hex.black_lizards, world_hex.yellow_lizards,
	       temp_string2), liz_hunger_desc [world_hex.turns_hungry],
	       liz_cp_desc [world_hex.combat_points], world_hex.lizard_pop);
  else
    fprintf (fptr, " - Your Black Lizards can see %s (%s & %s) Lizards there.\n",
	     show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	     world_hex.grey_lizards, world_hex.black_lizards, world_hex.yellow_lizards,
	     temp_string2), liz_hunger_desc [world_hex.turns_hungry],
	     liz_cp_desc [world_hex.combat_points]);

  /* fclose(fptr); */

  return (1);
}

int search_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char temp_string [80];

  int work,
      work2,
      work3,
      work4,
      x2,
      y2;

  hex_t world_hex;

  work = 0;

  if (order->moves [1] == 0)
    work4 = order->moves [0];
  else
    work4 = order->moves [1];

  for (work2 = 0; work2 < 7; work2 ++)
  {
    move_coords (world,work4, x, y, &x2, &y2);

    get_hex(world, x2, y2, &world_hex);

    for (work3 = 0; work3 < world->num_spies; work3 ++)
      if ((world->spylist[work3]->x_hex == x2) &&
	  (world->spylist[work3]->y_hex == y2) &&
	  (world->spylist[work3]->player > 0))
      {
	xy_to_string (x2, y2, temp_string);

	if (work == 0)
	{
	  open_player_mesg (order->player, &fptr);

	  fprintf (fptr, "their senses extend into the adjacent hexes.\n");

	  work = 1;

	  /* fclose(fptr); */
	}

	if (world->spylist[work3]->player == order->player)
	{
	  open_player_mesg (order->player, &fptr);

	  fprintf (fptr, " - Black Lizards detect your Spy Lizard at %s.\n",
		   temp_string);

	  /* fclose(fptr); */
	}
	else
	  {
	  open_player_mesg (order->player, &fptr);

	  fprintf (fptr, " - Black Lizards destroy Spy Lizard from '%s' Clan at %s!\n",
		   world->player[world->spylist [work3]->player].clan_name, temp_string);

	  /* fclose(fptr); */

	  open_player_mesg (world->spylist[work3]->player, &fptr);

	  fprintf (fptr, "\n - Your Spy Lizard at %s clutches its head and dies!\n",
		   temp_string);

	  /* fclose(fptr); */

	  world->spylist[work3]->player = 0;
	}
      }

    work4 -= 1;

    if (work4 < 0)
      work4 = 6;
  }

  if (work == 0)
  {
    open_player_mesg (order->player, &fptr);

    fprintf (fptr, "but find no enemy Spy Lizards.\n");

    /* fclose(fptr); */

    return (0);
  }

  return (1);
}

int shunt_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char work_string [255],
       work_string2 [255],
       temp_string [255];

  int players [MAX_PLAYERS + 1], already_seen [MAX_PLAYERS + 1];

  int direction,
      hexes,
      work,
      work2,
      work3,
      dest_x,
      dest_y,
      x2,
      y2,
      x3,
      y3,
      world_edge_f = LZ_FALSE,
      blocked_hex_f = LZ_FALSE;

  hex_t hex,
	new_hex;

  open_player_mesg (order->player, &fptr);

  get_hex(world, x, y, &hex);

  if (hex.terrain == DEN || hex.terrain == HOME_DEN)
  {
    xy_to_string (x, y, work_string);
    fprintf (fptr, "and the Den at %s glows, but nothing happens.\n",
	     work_string);
    /* fclose(fptr); */
    return (0);
  }

  if (lizards_in_hex (&hex) == 0)
  {
    fprintf (fptr, "but there are no Lizards there.\n");

    /* fclose(fptr); */

    return (0);
  }

  if (order->moves [1] == 0)
    direction = order->moves [0];
  else
    direction = order->moves [1];

  hexes = (order->black_lizards / base_liz) + 1;

  x2 = x;
  y2 = y;

  dest_x = -1;
  dest_y = -1;

  for (work = 0; work < hexes; work ++)
  {
    move_coords (world,direction, x2, y2, &x3, &y3);

    if (x2 == x3 && y2 == y3) /* check off the world edge */
    {
      world_edge_f = LZ_TRUE;

      break;
    }

    get_hex(world, x3, y3, &hex);

    if (lizards_in_hex (&hex) == 0 && hex.terrain != DEN && hex.terrain != HOME_DEN)
    {
      dest_x = x3;
      dest_y = y3;
    } else {
      blocked_hex_f = LZ_TRUE;

      break;
    }

    x2 = x3;
    y2 = y3;
  }

  if (world_edge_f)
  {
    fprintf (fptr, "but that's the edge of the world.\n");

    /* fclose(fptr); */

    return (0);
  }

  if (blocked_hex_f && dest_x == -1 && dest_y == -1)
  {
    fprintf (fptr, "but a strange force prevents them from completing.\n");

    /* fclose(fptr); */

    return (0);
  }

  get_hex(world, x, y, &hex);

  get_hex(world, dest_x, dest_y, &new_hex);

  xy_to_string (x, y, work_string);
  xy_to_string (dest_x, dest_y, work_string2);

  if (hex.hex_owner == order->player)
  {
    fprintf (fptr, "your Lizards at %s are seized by a powerful force and hurled %s!\n",
      work_string, dir [direction]);

    fprintf (fptr, " - Only to be dumped at %s.\n", work_string2);

    /* fclose(fptr); */

    if (x == order->x_hex && y == order->y_hex)
    {
      order->x_hex = dest_x;
      order->y_hex = dest_y;
    }
  } else {
    fprintf (fptr, "'%s' Clan Lizards at %s are seized by a powerful force and hurled %s!\n",
      world->player[hex.hex_owner].clan_name, work_string, dir [direction]);

    fprintf (fptr, " - Only to be dumped at %s.\n", work_string2);

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    fprintf (fptr, "\n - Your Lizards at %s are seized by a powerful force and hurled %s!\n",
      work_string, dir [direction]);

    fprintf (fptr, " - Only to be dumped at %s.\n", work_string2);

    /* fclose(fptr); */
  }

  new_hex.hex_owner = hex.hex_owner;
  new_hex.lizards_immobile = 0;
  new_hex.turns_hungry = hex.turns_hungry;
  new_hex.combat_points = hex.combat_points;
  new_hex.red_lizards = hex.red_lizards;
  new_hex.green_lizards = hex.green_lizards;
  new_hex.grey_lizards = hex.grey_lizards;
  new_hex.black_lizards = hex.black_lizards;
  new_hex.yellow_lizards = hex.yellow_lizards;

  hex.turns_hungry = 0;
  hex.combat_points = 0;
  hex.lizards_immobile = 0;
  hex.red_lizards = 0;
  hex.green_lizards = 0;
  hex.grey_lizards = 0;
  hex.black_lizards = 0;
  hex.yellow_lizards = 0;

  put_hex (world, x, y, &hex);
  put_hex (world, dest_x, dest_y, &new_hex);

  if (new_hex.terrain == WHIRLPOOL)
  {
    if (new_hex.bridge_here == 0 && new_hex.raft_here == 0)
    {
      open_player_mesg (new_hex.hex_owner, &fptr);

      fprintf (fptr, " - Lizards have been shunted into Whirlpool at %s.\n",
	       work_string2);

      work2 = (float) lizards_in_hex (&new_hex) * WHIRLPOOL_KILL_LEVEL;

      if (work2 > 1)
	work3 = random (work2) + 1;
      else
	work3 = 1;

      fprintf (fptr, " - %d Lizards are sucked down to their deaths in Whirlpool.\n",
			 work3);

      kill_lizards_xy (&new_hex, work3);

      put_hex (world, dest_x, dest_y, &new_hex);

      if (lizards_in_hex (&new_hex) == 0)
      {
	fprintf (fptr, " - NO Lizards resist the Whirlpool.\n");

	/* fclose(fptr); */
      } else {
	fprintf (fptr, " - %s Lizards resist the Whirlpool.\n",
	  show_lizards (new_hex.red_lizards, new_hex.green_lizards,
	    new_hex.grey_lizards, new_hex.black_lizards,
	    new_hex.yellow_lizards, temp_string));

	/* fclose(fptr); */
      }
    }
  } else {
    if (new_hex.terrain == CURSED)
    {
      open_player_mesg (new_hex.hex_owner, &fptr);

      fprintf (fptr, " - Lizards have been shunted into CURSED hex at %s.\n",
		       work_string2);

      work2 = (float) lizards_in_hex (&new_hex) * CURSED_KILL_LEVEL;

      if (work2 > 1)
	work3 = random (work2) + 1;
      else
	work3 = 1;

      fprintf (fptr, " - %d Lizards choke and collapse, dead.\n",
	       work3);

      kill_lizards_xy (&new_hex, work3);

      put_hex (world, dest_x, dest_y, &new_hex);

      if (lizards_in_hex (&new_hex) == 0)
      {
	fprintf (fptr, " - NO Lizards resist the CURSE.\n");

	/* fclose(fptr); */
      } else {
	fprintf (fptr, " - %s Lizards resist the CURSE.\n",
	  show_lizards (new_hex.red_lizards, new_hex.green_lizards,
	    new_hex.grey_lizards, new_hex.black_lizards,
	    new_hex.yellow_lizards, temp_string));

	/* fclose(fptr); */
      }
    }
  }

  if (hex.terrain != SCRUB)
  {
    in_sight (world, x, y, players);

    players [order->player] = 0;

    if (hex.hex_owner != order->player)
      players [hex.hex_owner] = 0;

    for (work = 1; work < MAX_PLAYERS + 1; work ++)
      already_seen [work] = 0;

    for (work = 1; work < MAX_PLAYERS + 1; work ++)
      if (players [work] > 0)
      {
	open_player_mesg (work, &fptr);

	fprintf (fptr, "\n - '%s' Clan Lizards at %s are seized by a powerful force and hurled %s!\n",
	world->player[hex.hex_owner].clan_name, work_string, dir [direction]);

	already_seen [work] = 1;

	/* fclose(fptr); */
      }
  }

  if (new_hex.terrain != SCRUB)
  {
    in_sight (world, dest_x, dest_y, players);

    players [order->player] = 0;

    if (hex.hex_owner != order->player)
      players [hex.hex_owner] = 0;

    for (work = 1; work < MAX_PLAYERS + 1; work ++)
      if (players [work] > 0)
      {
	open_player_mesg (work, &fptr);

	if (already_seen [work] == 1)
	  fprintf (fptr, " - Only to be dumped at %s.\n", work_string2);
	else
	  fprintf (fptr, "\n - '%s' Clan Lizards are dumped at %s by a powerful force!\n",
	    world->player[hex.hex_owner].clan_name, work_string2);

	/* fclose(fptr); */
      }
  }

  return (1);
}

int mould_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char temp_string [255];
  int players [MAX_PLAYERS + 1];

  int work,
      work2,
      work3,
      work4,
      x2,
      y2;

  hex_t world_hex,
	hex;

  order_t work_order;

  xy_to_string (x, y, temp_string);

  get_hex(world, x, y, &world_hex);

  work2 = world_hex.terrain;

  work4 = 0;

  do
  {
    switch (work2)
    {
      case SCRUB:
      case PLAIN:
      case RUIN:
      case FERTILE:
      case SWAMP:
      case PEAK:
      case VOLCANO:
	work4 = 1;

	work3 = random (5);

	switch (work3)
	{
	  case 0:
	  case 1:
	  case 2:
	  case 3:
	    world_hex.terrain = WATER;

	    break;
	  case 4:
	    world_hex.terrain = WHIRLPOOL;

	    break;
	}

	break;
      case WATER:
      case WHIRLPOOL:
	work4 = 0;

	work3 = random (14);

	switch (work3)
	{
	  case 0:
	  case 1:
	  case 2:
	    world_hex.terrain = SCRUB;

	    break;
	  case 3:
	  case 4:
	  case 5:
	    world_hex.terrain = PLAIN;

	    break;
	  case 6:
	  case 7:
	    world_hex.terrain = FERTILE;

	    break;
	  case 8:
	  case 9:
	    world_hex.terrain = SWAMP;

	    break;
	  case 10:
	    world_hex.terrain = PEAK;

	    break;
	  case 11:
	    world_hex.terrain = VOLCANO;

	    break;
	  case 12:
	    world_hex.terrain = RUIN;

	    break;
	  case 13:
	    world_hex.terrain = TEMPLE;

	    break;
	}

	break;
      case DEN:
      case HOME_DEN:
      case TEMPLE:
      case CURSED:
	open_player_mesg (order->player, &fptr);

	fprintf (fptr, "but the world resists...\n");

	/* fclose(fptr); */

	return (0);
    }
  } while (work2 == world_hex.terrain);

  in_sight (world, x, y, players);

  players [order->player] = 0;

  open_player_mesg (order->player, &fptr);

  if (work4 == 1)
  {
    fprintf (fptr, "\n - Dust engulfs %s as the hex sinks into the sea!\n - Your black lizards have created a ",
	     temp_string);

    if (world_hex.terrain == WHIRLPOOL)
      fprintf (fptr, "Whirlpool there!\n");
    else
      fprintf (fptr, "Water hex there.\n");

    /* fclose(fptr); */

    put_hex (world, x, y, &world_hex);

    for (work = 1; work < 7; work ++)
    {
      move_coords (world,work, x, y, &x2, &y2);

      if (x2 != x || y2 != y)
      {
	get_hex(world, x2, y2, &hex);

	if (hex.bridge_here == 1)
	{
	  /* set up a collapse order to kill any lizards in hex */

	  work_order.player = hex.hex_owner;
	  work_order.order_type = COLLAPSE_BRIDGE;
	  work_order.x_hex = x2;
	  work_order.y_hex = y2;

	  collapse_bridge_order (&work_order, order->player, 0);
	}
      }
    }

    get_hex(world, x, y, &world_hex);

    open_player_mesg (order->player, &fptr);
  } else {
    fprintf (fptr, "\n - Mist engulfs %s as the hex rises out of the sea!\n",
	     temp_string);

    switch (world_hex.terrain)
    {
      case SCRUB:
	fprintf (fptr, " - Your black lizards have created a Scrub hex there. ");

	break;
      case PLAIN:
	fprintf (fptr, " - Your black lizards have created a Plain hex there. ");

	break;
      case FERTILE:
	fprintf (fptr, " - Your black lizards have created a Fertile hex there. ");

	break;
      case SWAMP:
	fprintf (fptr, " - Your black lizards have created a Swamp hex there. ");

	break;
      case PEAK:
	fprintf (fptr, " - Your black lizards have raised a Peak from the depths! ");

	break;
      case VOLCANO:
	fprintf (fptr, " - Your black lizards have raised a Volcano from the depths! ");

	break;
      case RUIN:
	fprintf (fptr, " - Your black lizards have raised an ancient and crusted Ruin there! ");

	break;
      case TEMPLE:
	fprintf (fptr, " - Your black lizards have raised a barnacle festooned Temple there! ");

	break;
    }

    if (world_hex.bridge_here > 0)
    {
      fprintf (fptr, "Bridge here is destroyed.\n");

      world_hex.bridge_here = 0;
    } else
      fprintf (fptr, "\n");
  }

  /* fclose(fptr); */

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      if (work4 == 1)
      {
	fprintf (fptr, "\n - Dust engulfs %s as the hex sinks into the sea! There is now a ",
		 temp_string);

	if (world_hex.terrain == WHIRLPOOL)
	  fprintf (fptr, "Whirlpool there!\n");
	else
	  fprintf (fptr, "Water hex there.\n");
      } else {
	fprintf (fptr, "\n - Mist engulfs %s as the hex rises out of the sea! There is now a ",
		 temp_string);

	switch (world_hex.terrain)
	{
	  case SCRUB:
	    fprintf (fptr, "Scrub hex there.\n");

	    break;
	  case PLAIN:
	    fprintf (fptr, "Plain hex there.\n");

	    break;
	  case FERTILE:
	    fprintf (fptr, "Fertile hex there.\n");

	    break;
	  case SWAMP:
	    fprintf (fptr, "Swamp hex there.\n");

	    break;
	  case PEAK:
	    fprintf (fptr, "Peak hex there!\n");

	    break;
	  case VOLCANO:
	    fprintf (fptr, "Volcano hex there!\n");

	    break;
	  case RUIN:
	    fprintf (fptr, "Ruin there!\n");

	    break;
	  case TEMPLE:
	    fprintf (fptr, "Temple there!\n");

	    break;
	}
      }

      /* fclose(fptr); */
    }

  put_hex (world, x, y, &world_hex);

  return (1);
}

int coerce_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char work_string [80];

  hex_t hex;

  get_hex(world, x, y, &hex);

  open_player_mesg (order->player, &fptr);

  if (lizards_in_hex (&hex) == 0)
  {
    fprintf (fptr,  "but there are no Lizards there.\n");

    /* fclose(fptr); */

    return (0);
  }

  if (lizards_in_hex (&hex) > 0 && hex.hex_owner == order->player)
  {
    fprintf (fptr, "but there are no enemy Lizards there.\n");

    /* fclose(fptr); */

    return (0);
  }

  if (hex.terrain == DEN || hex.terrain == HOME_DEN || hex.terrain == TEMPLE)
  {
    xy_to_string (x, y, work_string);

    fprintf (fptr, "but their chant is absorbed by the ");

    switch (hex.terrain)
    {
      case DEN:
	fprintf (fptr, "Den at %s.\n", work_string);

	break;
      case HOME_DEN:
	fprintf (fptr, "Home Den at %s.\n", work_string);

	break;

      case TEMPLE:
	fprintf (fptr, "Temple at %s.\n", work_string);

	break;
    }

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    switch (hex.terrain)
    {
      case DEN:
	fprintf (fptr, "\n - Walls of %s Den at %s begin to glow! Your Lizards here feel strange.\n",
		 lizard_colours [hex.den_lizard_type], work_string);
	break;
      case HOME_DEN:
	fprintf (fptr, "\n - Walls of %s Home Den at %s begin to glow! Your Lizards here feel strange.\n",
		 lizard_colours [hex.den_lizard_type], work_string);
	break;
      case TEMPLE:
	fprintf (fptr, "\n - Temple at %s begins to crackle and hiss! Your Lizards here feel strange.\n",
		 work_string);
	break;
    }

    /* fclose(fptr); */

    return (0);
  }

  if (order->black_lizards >= lizards_in_hex (&hex))
  {
    fprintf (fptr, "and the hex is filled with power!\n");

    xy_to_string (x, y, work_string);

    fprintf (fptr, " - %d '%s' Clan Lizards at %s are compelled to join your Clan!\n",
	     lizards_in_hex (&hex), world->player[hex.hex_owner].clan_name, work_string);

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    fprintf (fptr, "\n - A strange energy surrounds your Lizards at %s.\n", work_string);
    fprintf (fptr, " - Their minds are clouded and they are no longer yours to control!\n");

    /* fclose(fptr); */

    hex.hex_owner = order->player;

    put_hex (world, x, y, &hex);

    return (1);
  } else {
    fprintf (fptr, "but fail to coerce the enemy Lizards there.\n");

    xy_to_string (x, y, work_string);

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    fprintf (fptr, "\n - A strange energy surrounds your Lizards at %s. Their minds are clouded but they resist!\n",
	     work_string);

    /* fclose(fptr); */

    return (0);
  }
}

int transmute_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char work_string [80],
       work_string2 [255];

  int work;

  hex_t hex;

  get_hex(world, x, y, &hex);

  open_player_mesg (order->player, &fptr);

  if (lizards_in_hex (&hex) == 0)
  {
    fprintf (fptr, "but there are no Lizards there.\n");

    /* fclose(fptr); */

    return (0);
  }

  if (hex.terrain == DEN || hex.terrain == HOME_DEN || hex.terrain == TEMPLE)
  {
    xy_to_string (x, y, work_string);

    fprintf (fptr, "but their chant is absorbed by the ");

    switch (hex.terrain)
    {
      case DEN:
	fprintf (fptr, "Den at %s.\n", work_string);

	break;
      case HOME_DEN:
	fprintf (fptr, "Home Den at %s.\n", work_string);

	break;

      case TEMPLE:
	fprintf (fptr, "Temple at %s.\n", work_string);

	break;
    }

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    if (hex.hex_owner != order->player)
      fprintf (fptr, "\n");

    switch (hex.terrain)
    {
      case DEN:
	fprintf (fptr, " - Walls of %s Den at %s begin to glow! Your Lizards here feel very strange.\n",
		 lizard_colours [hex.den_lizard_type], work_string);
	break;
      case HOME_DEN:
	fprintf (fptr, " - Walls of %s Home Den at %s begin to glow! Your Lizards here feel very strange.\n",
		 lizard_colours [hex.den_lizard_type], work_string);
	break;
      case TEMPLE:
	fprintf (fptr, " - Temple at %s begins to crackle and hiss! Your Lizards here feel very strange.\n",
		 work_string);
	break;
    }

    /* fclose(fptr); */

    return (0);
  }

  fprintf (fptr, "and the hex is filled with power!\n");

  xy_to_string (x, y, work_string);

  if (order->player != hex.hex_owner)
  {
    fprintf (fptr, " - %d '%s' Clan Lizards at %s are sickened as their skins melt and flow!\n",
             lizards_in_hex (&hex), world->player[hex.hex_owner].clan_name, work_string);

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    fprintf (fptr, "\n - A strange malaise grips your Lizards at %s. Their skins flow, changing colour...\n", work_string);

    /* fclose(fptr); */

    transmute_hex (&hex);

    put_hex (world, x, y, &hex);
  } else {
    fprintf (fptr, " - A strange malaise grips your Lizards at %s. Their skins flow, changing colour...\n", work_string);

    transmute_hex (&hex);

    put_hex (world, x, y, &hex);

    fprintf (fptr, " - %s now contains %s Lizards.\n", work_string,
      show_lizards (hex.red_lizards, hex.green_lizards, hex.grey_lizards,
      hex.black_lizards, hex.yellow_lizards, work_string2));

    /* fclose(fptr); */
  }

  /* go through orders killing any splits for hex and changing any
    moves for hex */

  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
      world->order_list [work]->x_hex == x &&
      world->order_list [work]->y_hex == y &&
      world->order_list [work]->player == hex.hex_owner &&
      world->order_list [work]->order_type == SPLIT)
    {
      open_player_mesg (hex.hex_owner, &fptr);

      fprintf (fptr, " - Split Order at %s ignored as the lizards have been TRANSMUTED.\n",
        work_string);

      world->order_list [work]->order_type = INACTIVE;

      /* fclose(fptr); */
    } else
      if (world->order_list [work] != order &&
        world->order_list [work]->x_hex == x &&
	world->order_list [work]->y_hex == y &&
	world->order_list [work]->player == hex.hex_owner &&
	world->order_list [work]->order_type == MOVE)
      {
        /* adjust move to new lizard colours in hex */

        world->order_list [work]->red_lizards = hex.red_lizards;
        world->order_list [work]->green_lizards = hex.green_lizards;
        world->order_list [work]->grey_lizards = hex.grey_lizards;
        world->order_list [work]->black_lizards = hex.black_lizards;
        world->order_list [work]->yellow_lizards = hex.yellow_lizards;
      }

  return (1);
}

void transmute_hex (hex_t *hex)
{
  int work,
      work2,
      work3,
      work4,
      lizards,
      to_red = LZ_FALSE,
      to_green = LZ_FALSE,
      to_grey = LZ_FALSE,
      to_black = LZ_FALSE,
      to_yellow = LZ_FALSE,
      colours;

  colours = 0;

  if (hex->red_lizards > 0) colours ++;
  if (hex->green_lizards > 0) colours ++;
  if (hex->grey_lizards > 0) colours ++;
  if (hex->black_lizards > 0) colours ++;
  if (hex->yellow_lizards > 0) colours ++;

  if (colours > 3) colours = 3;

  for (work = 0; work < colours; work ++)
  {
    if (to_red && to_green && to_grey && to_black && to_yellow)
      break;

    do {
      work2 = random (LIZARD_TYPES) + 1;

      switch (work2)
      {
	case RED_LIZARD:
	  if (!to_red)
	    lizards = hex->red_lizards;
	  else
	    lizards = 0;

	  break;
	case GREEN_LIZARD:
	  if (!to_green)
	    lizards = hex->green_lizards;
	  else
	    lizards = 0;

	  break;
	case GREY_LIZARD:
	  if (!to_grey)
	    lizards = hex->grey_lizards;
	  else
	    lizards = 0;

	  break;
	case BLACK_LIZARD:
	  if (!to_black)
	    lizards = hex->black_lizards;
	  else
	    lizards = 0;

	  break;
	case YELLOW_LIZARD:
	  if (!to_yellow)
	    lizards = hex->yellow_lizards;
	  else
	    lizards = 0;

	  break;
      }
    } while (lizards == 0);

    do
      work3 = random (LIZARD_TYPES) + 1;
    while (work3 == work2);

    switch (work2)
    {
      case RED_LIZARD:
	switch (work3)
	{
	  case GREEN_LIZARD:
	    work4 = hex->green_lizards;
	    hex->green_lizards = hex->red_lizards;
	    hex->red_lizards = work4;
	    to_green = LZ_TRUE;
	    break;
	  case GREY_LIZARD:
	    work4 = hex->grey_lizards;
	    hex->grey_lizards = hex->red_lizards;
	    hex->red_lizards = work4;
	    to_grey = LZ_TRUE;
	    break;
	  case BLACK_LIZARD:
	    work4 = hex->black_lizards;
	    hex->black_lizards = hex->red_lizards;
	    hex->red_lizards = work4;
	    to_black = LZ_TRUE;
	    break;
	  case YELLOW_LIZARD:
	    work4 = hex->yellow_lizards;
	    hex->yellow_lizards = hex->red_lizards;
	    hex->red_lizards = work4;
	    to_yellow = LZ_TRUE;
	    break;
	}

	break;
      case GREEN_LIZARD:
	switch (work3)
	{
	  case RED_LIZARD:
	    work4 = hex->red_lizards;
	    hex->red_lizards = hex->green_lizards;
	    hex->green_lizards = work4;
	    to_red = LZ_TRUE;
	    break;
	  case GREY_LIZARD:
	    work4 = hex->grey_lizards;
	    hex->grey_lizards = hex->green_lizards;
	    hex->green_lizards = work4;
	    to_grey = LZ_TRUE;
	    break;
	  case BLACK_LIZARD:
	    work4 = hex->black_lizards;
	    hex->black_lizards = hex->green_lizards;
	    hex->green_lizards = work4;
	    to_black = LZ_TRUE;
	    break;
	  case YELLOW_LIZARD:
	    work4 = hex->yellow_lizards;
	    hex->yellow_lizards = hex->green_lizards;
	    hex->green_lizards = work4;
	    to_yellow = LZ_TRUE;
	    break;
	}

	break;
      case GREY_LIZARD:
	switch (work3)
	{
	  case RED_LIZARD:
	    work4 = hex->red_lizards;
	    hex->red_lizards = hex->grey_lizards;
	    hex->grey_lizards = work4;
	    to_red = LZ_TRUE;
	    break;
	  case GREEN_LIZARD:
	    work4 = hex->green_lizards;
	    hex->green_lizards = hex->grey_lizards;
	    hex->grey_lizards = work4;
	    to_green = LZ_TRUE;
	    break;
	  case BLACK_LIZARD:
	    work4 = hex->black_lizards;
	    hex->black_lizards = hex->grey_lizards;
	    hex->grey_lizards = work4;
	    to_black = LZ_TRUE;
	    break;
	  case YELLOW_LIZARD:
	    work4 = hex->yellow_lizards;
	    hex->yellow_lizards = hex->grey_lizards;
	    hex->grey_lizards = work4;
	    to_yellow = LZ_TRUE;
	    break;
	}

	break;
      case BLACK_LIZARD:
	switch (work3)
	{
	  case RED_LIZARD:
	    work4 = hex->red_lizards;
	    hex->red_lizards = hex->black_lizards;
	    hex->black_lizards = work4;
	    to_red = LZ_TRUE;
	    break;
	  case GREEN_LIZARD:
	    work4 = hex->green_lizards;
	    hex->green_lizards = hex->black_lizards;
	    hex->black_lizards = work4;
	    to_green = LZ_TRUE;
	    break;
	  case GREY_LIZARD:
	    work4 = hex->grey_lizards;
	    hex->grey_lizards = hex->black_lizards;
	    hex->black_lizards = work4;
	    to_grey = LZ_TRUE;
	    break;
	  case YELLOW_LIZARD:
	    work4 = hex->yellow_lizards;
	    hex->yellow_lizards = hex->black_lizards;
	    hex->black_lizards = work4;
	    to_yellow = LZ_TRUE;
	    break;
	}

	break;
      case YELLOW_LIZARD:
	switch (work3)
	{
	  case RED_LIZARD:
	    work4 = hex->red_lizards;
	    hex->red_lizards = hex->yellow_lizards;
	    hex->yellow_lizards = work4;
	    to_red = LZ_TRUE;
	    break;
	  case GREEN_LIZARD:
	    work4 = hex->green_lizards;
	    hex->green_lizards = hex->yellow_lizards;
	    hex->yellow_lizards = work4;
	    to_green = LZ_TRUE;
	    break;
	  case GREY_LIZARD:
	    work4 = hex->grey_lizards;
	    hex->grey_lizards = hex->yellow_lizards;
	    hex->yellow_lizards = work4;
	    to_grey = LZ_TRUE;
	    break;
	  case BLACK_LIZARD:
	    work4 = hex->black_lizards;
	    hex->black_lizards = hex->yellow_lizards;
	    hex->yellow_lizards = work4;
	    to_black = LZ_TRUE;
	    break;
	}

	break;
    }
  }
}

int dark_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char temp_string [80],
       temp_string2 [80];

  int players [MAX_PLAYERS + 1];

  int work,
      work2;

  hex_t world_hex;

  xy_to_string (x, y, temp_string);

  get_hex(world, x, y, &world_hex);

  in_sight (world, x, y, players);

  players [order->player] = 0;

  open_player_mesg (order->player, &fptr);

  fprintf (fptr, "a dark, black cloud settles over %s!\n", temp_string);

  /* fclose(fptr); */

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - A large, black cloud can be seen settling over %s!\n",
	       temp_string);

      /* fclose(fptr); */
    }

  if ((world_hex.terrain == DEN || world_hex.terrain == HOME_DEN) &&
      world_hex.hex_owner > 0)
  {
    if (order->player != world_hex.hex_owner)
    {
      open_player_mesg (order->player, &fptr);

      if (world_hex.terrain == HOME_DEN)
	fprintf (fptr, " - The chant destroys %s Home Den at %s!\n",
		 lizard_colours [world_hex.den_lizard_type], temp_string);
      else
	fprintf (fptr, " - The chant destroys %s Den at %s!\n",
		 lizard_colours [world_hex.den_lizard_type], temp_string);

      /* fclose(fptr); */
    }

    open_player_mesg (world_hex.hex_owner, &fptr);

    if (world_hex.terrain == HOME_DEN)
      fprintf (fptr, " - The black cloud destroys %s Home Den at %s! ",
	       lizard_colours [world_hex.den_lizard_type], temp_string);
    else
      fprintf (fptr, " - The black cloud destroys %s Den at %s! ",
	       lizard_colours [world_hex.den_lizard_type], temp_string);

    if (lizards_in_hex (&world_hex) > 0)
      fprintf (fptr, "%s Lizards and %d Den Lizards die!\n",
	       show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	       world_hex.grey_lizards, world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string2), world_hex.lizard_pop);
    else
      fprintf (fptr, "%d Den Lizards die!\n", world_hex.lizard_pop);

    world_hex.red_lizards = 0;
    world_hex.green_lizards = 0;
    world_hex.grey_lizards = 0;
    world_hex.black_lizards = 0;
    world_hex.yellow_lizards = 0;
    world_hex.lizard_pop = 0;

    /* fclose(fptr); */
  } else
    if (lizards_in_hex (&world_hex) > 0)
    {
      if (order->player != world_hex.hex_owner)
      {
	open_player_mesg (order->player, &fptr);

	fprintf (fptr, " - The black cloud destroys Lizards at %s!\n", temp_string);

	/* fclose(fptr); */
      }

      open_player_mesg (world_hex.hex_owner, &fptr);

      fprintf (fptr, " - The black cloud destroys Lizards at %s! ", temp_string);

      fprintf (fptr, "%s Lizards die!\n",
	       show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	       world_hex.grey_lizards, world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string2));

      world_hex.red_lizards = 0;
      world_hex.green_lizards = 0;
      world_hex.grey_lizards = 0;
      world_hex.black_lizards = 0;
      world_hex.yellow_lizards = 0;
      world_hex.lizard_pop = 0;

      /* fclose(fptr); */
    }

  for (work2 = 0; work2 < world->num_spies; work2 ++)
    if ((world->spylist[work2]->x_hex == x) &&
	(world->spylist[work2]->y_hex == y) &&
	(world->spylist[work2]->player > 0))
    {
      open_player_mesg (world_hex.hex_owner, &fptr);

      fprintf (fptr, " - The black cloud destroys Spy Lizard at %s! ", temp_string);

      world->spylist[work2]->player = 0;
      /* fclose(fptr); */
    }

  world_hex.terrain = CURSED;

  world_hex.bridge_here = 0;

  world_hex.hex_owner = 0;

  put_hex (world, x, y, &world_hex);

  return (1);
}

int freeze_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char work_string [80],
       work_string2 [80];

  hex_t hex;

  open_player_mesg (order->player, &fptr);

  get_hex(world, x, y, &hex);

  if (lizards_in_hex (&hex) == 0)
  {
    fprintf (fptr, "but there's no Lizards there!\n");

    /* fclose(fptr); */

    return (0);
  }

  xy_to_string (x, y, work_string);
  xy_to_string (order->x_hex, order->y_hex, work_string2);

  if (hex.hex_owner != order->player)
  {
    fprintf (fptr, "'%s' Clan Lizards at %s are FROZEN for this turn!\n",
	     world->player[hex.hex_owner].clan_name, work_string);

    /* fclose(fptr); */

    open_player_mesg (hex.hex_owner, &fptr);

    fprintf (fptr, "\n - Your Lizards at %s have been FROZEN by '%s' Clan Lizards at %s.\n",
	     work_string, world->player[order->player].clan_name, work_string2);

    /* fclose(fptr); */
  } else
    fprintf (fptr, "your Lizards at %s are FROZEN for this turn!\n",
	     work_string);

  hex.lizards_immobile = 3;

  put_hex (world, x, y, &hex);

  return (1);
}

int summon_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr;

  char work_string [80],
       work_string2 [80];

  int players [MAX_PLAYERS + 1];

  int work,
      work2,
      work3,
      red,
      green,
      grey,
      black,
      yellow,
      new_red,
      new_green,
      new_grey,
      new_black,
      new_yellow;

  hex_t hex;

  open_player_mesg (order->player, &fptr);

  get_hex(world, x, y, &hex);

  if (hex.terrain != WATER && hex.terrain != WHIRLPOOL)
  {
    fprintf (fptr, "but no Sea Monster appears.\n");

    /* fclose(fptr); */

    return (0);
  }

  /* fclose(fptr); */

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    players [work] = 0;

  in_sight (world, x, y, players);

  xy_to_string (x, y, work_string);

  for (work = 0; work < world->num_spies; work ++)
    if (world->spylist [work]->x_hex == x && world->spylist [work]->y_hex == y &&
	world->spylist [work]->player > 0)
    {
      open_player_mesg (world->spylist [work]->player, &fptr);

      fprintf (fptr, "\n - Spy Lizard CONSUMED by a HUGE, angry Sea Monster at %s.\n",
	       work_string);

      players [world->spylist [work]->player] = 0;

      world->spylist[work]->player = 0;

      /* fclose(fptr); */
    }

  if (lizards_in_hex (&hex) > 0)
  {
    open_player_mesg (hex.hex_owner, &fptr);

    players [hex.hex_owner] = 0;

    fprintf (fptr, "\n - Lizards at %s ATTACKED by a HUGE, angry Sea Monster.\n",
		      work_string);

    /* fclose(fptr); */

    work2 = terrain_adjacent (world, x, y, WATER);

    work3 = SEA_MONSTER_STRENGTH_LEVEL * work2 * 2;

    put_hex (world, x, y, &hex);

    red = hex.red_lizards;
    green = hex.green_lizards;
    grey = hex.grey_lizards;
    black = hex.black_lizards;
    yellow = hex.yellow_lizards;

    combat_sea_monster (world,
			red, green, grey, black, yellow, &new_red,
			&new_green, &new_grey, &new_black, &new_yellow,
			x, y, &work3);

    get_hex(world, x, y, &hex);

    hex.red_lizards = new_red;
    hex.green_lizards = new_green;
    hex.grey_lizards = new_grey;
    hex.black_lizards = new_black;
    hex.yellow_lizards = new_yellow;

    red = new_red;
    green = new_green;
    grey = new_grey;
    black = new_black;
    yellow = new_yellow;

    open_player_mesg (hex.hex_owner, &fptr);

    if (work3 == 0)
    {
      fprintf (fptr, " - Sea Monster slain.\n");

      fprintf (fptr, " - %s Lizards survive at %s.\n",
			 show_lizards (red, green, grey, black, yellow, work_string2),
			 work_string);

      /* fclose(fptr); */
    } else {
      fprintf (fptr, " - ALL Lizards eaten by Sea Monster...\n");

      /* fclose(fptr); */
    }
  }

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - A HUGE, angry Sea Monster surfaces at %s!\n",
	       work_string);

      /* fclose(fptr); */
    }

  put_hex (world, x, y, &hex);

  return (1);
}

int gate_chant (int x, int y, order_t *order, int base_liz)
{
  FILE *fptr,
       *fptr2;

  char work_string [255],
       work_string2 [255],
       temp_string [255];

    int players [MAX_PLAYERS + 1],
       already_seen [MAX_PLAYERS + 1];

  int work,
      dest_x,
      dest_y,
      result,
      new_red,
      new_green,
      new_grey,
      new_black,
      new_yellow,
      capture_level,
      enemy_capture_level,
      captured_red,
      captured_green,
      captured_grey,
      captured_black,
      captured_yellow,
      enemy_captured_red,
      enemy_captured_green,
      enemy_captured_grey,
      enemy_captured_black,
      enemy_captured_yellow,
      num_captured,
      lizards_arrived_f = LZ_FALSE;

  hex_t hex,
	new_hex,
	temp_hex;

  open_player_mesg (order->player, &fptr);

  get_hex(world, x, y, &hex);

  if (lizards_in_hex (&hex) == 0)
  {
    fprintf (fptr, "but there are no Lizards there.\n");

    /* fclose(fptr); */

    return (0);
  }

  if (hex.terrain != TEMPLE)
  {
    fprintf (fptr, "but there's no Temple there.\n");

    /* fclose(fptr); */

    return (0);
  }

  dest_x = order->red_lizards;
  dest_y = order->green_lizards;

  get_hex(world, x, y, &hex);
  get_hex(world, dest_x, dest_y, &new_hex);

  xy_to_string (x, y, work_string);
  xy_to_string (dest_x, dest_y, work_string2);

  if (new_hex.terrain != TEMPLE || (x == dest_x && y == dest_y))
  {
    if (x != dest_x || y != dest_y)
      fprintf (fptr, "but they fail to connect with %s!\n", work_string2);
    else
      fprintf (fptr, "but all that happens is the Temple at %s glows redly.\n",
	work_string);

    /* fclose(fptr); */

    return (0);
  }

  if (new_hex.hex_owner != order->player)
  {
    if (lizards_in_hex (&new_hex) > 0)
    {
      fprintf (fptr, "your Lizards at %s disappear!\n", work_string);
      fprintf (fptr, " - Only to re-appear in Temple at %s.\n", work_string2);

      open_player_mesg (new_hex.hex_owner, &fptr2);

      fprintf (fptr2, "\n - Temple at %s is suddenly occupied by %d Lizards from Clan '%s'.\n",
	work_string2, lizards_in_hex (&hex), world->player[order->player].clan_name);

      if (lizards_in_hex (&new_hex) + lizards_in_hex (&hex) > MAX_LIZARDS_IN_HEX)
      {
	fprintf (fptr, " - The Temple at %s showers sparks and smoke!\n", work_string2);
	fprintf (fptr2, " - The Temple at %s showers sparks and smoke!\n", work_string2);

	work = lizards_in_hex (&new_hex) + lizards_in_hex (&hex) - MAX_LIZARDS_IN_HEX;

	kill_lizards_xy (&hex, work);

	fprintf (fptr, " - %d Lizards here suddenly explode in sodden gouts, showering the temple.\n",
		 work);
	fprintf (fptr2, " - %d enemy Lizards here suddenly explode in sodden gouts, showering the temple.\n",
		 work);
      }

      if (lizards_in_hex (&hex) > 0)
	fprintf (fptr, " - %d Clan '%s' Lizards already occupy the Temple! Combat ensues...\n",
		 lizards_in_hex (&new_hex), world->player[new_hex.hex_owner].clan_name);

      /* fclose(fptr); */
      /* fclose (fptr2); */

      open_player_mesg (new_hex.hex_owner, &fptr);

      if (lizards_in_hex (&hex) > 0)
      {
	fprintf (fptr, " - Your Lizards at %s attack the enemy Lizards!\n", work_string2);

	/* fclose(fptr); */

	put_hex (world, x, y, &hex);
	put_hex (world, dest_x, dest_y, &new_hex);

	temp_hex = hex; /* for calculating capture! */

	result = combat (world,
			 new_hex.red_lizards, new_hex.green_lizards,
			 new_hex.grey_lizards, new_hex.black_lizards,
			 new_hex.yellow_lizards, &new_red, &new_green,
			 &new_grey, &new_black, &new_yellow, dest_x, dest_y,
			 x, y, -1);

	get_hex(world, x, y, &hex);
	get_hex(world, dest_x, dest_y, &new_hex);

	/* calculate YOUR captured lizards */

	capture_level = MIN_CAPTURE_PERCENT +
	  (hex.combat_points * CP_CAPTURE_MOD_LEVEL);

	captured_red = new_hex.red_lizards - new_red;
	captured_green = new_hex.green_lizards - new_green;
	captured_grey = new_hex.grey_lizards - new_grey;
	captured_black = new_hex.black_lizards - new_black;
	captured_yellow = new_hex.yellow_lizards - new_yellow;

	/* calculate ENEMY captured lizards */

	enemy_capture_level = MIN_CAPTURE_PERCENT +
	  (new_hex.combat_points * CP_CAPTURE_MOD_LEVEL);

	enemy_captured_red = temp_hex.red_lizards - hex.red_lizards;
	enemy_captured_green = temp_hex.green_lizards - hex.green_lizards;
	enemy_captured_grey = temp_hex.grey_lizards - hex.grey_lizards;
	enemy_captured_black = temp_hex.black_lizards - hex.black_lizards;
	enemy_captured_yellow = temp_hex.yellow_lizards - hex.yellow_lizards;

	/* update new hex */

	new_hex.red_lizards = new_red;
	new_hex.green_lizards = new_green;
	new_hex.grey_lizards = new_grey;
	new_hex.black_lizards = new_black;
	new_hex.yellow_lizards = new_yellow;

	if (lizards_in_hex (&hex) == 0)
	{
	  open_player_mesg (order->player, &fptr);

	  fprintf (fptr, " - All of your Lizards in Temple at %s were killed.\n",
		 work_string2);

	  /* fclose(fptr); */

	  open_player_mesg (new_hex.hex_owner, &fptr);

	  fprintf (fptr, " - Your Lizards in Temple at %s killed all of the Clan '%s' Lizards here.\n",
		 work_string2, world->player[order->player].clan_name);

	  inc_combat_points (&new_hex);

	  fprintf (fptr, " - They are now %s warriors.\n",
		 liz_cp_desc [new_hex.combat_points]);

	  /* find captures for surviving ORIGINAL lizards in temple */

	  num_captured = ((float) (enemy_captured_red + enemy_captured_green +
		  enemy_captured_grey + enemy_captured_black +
		  enemy_captured_yellow) / 100.00) *
		  (float) enemy_capture_level;

	  capture_lizards (world,
			   &new_hex, num_captured, &enemy_captured_red,
	    &enemy_captured_green, &enemy_captured_grey, &enemy_captured_black,
	    &enemy_captured_yellow, &new_red, &new_green, &new_grey, &new_black,
	    &new_yellow);

	  if (new_red + new_green + new_grey + new_black + new_yellow > 0)
	  {
	    fprintf (fptr, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
	      show_lizards (new_red, new_green, new_grey, new_black, new_yellow,
		temp_string));
	  }

	  /* fclose(fptr); */

	  put_hex (world, dest_x, dest_y, &new_hex);

	  return (0);
	} else {
	  open_player_mesg (new_hex.hex_owner, &fptr);

	  fprintf (fptr, " - All of your Lizards in Temple at %s were killed.\n",
		   work_string2);

	  /* fclose(fptr); */

	  open_player_mesg (order->player, &fptr);

	  fprintf (fptr, " - Your Lizards in Temple at %s killed all of the Clan '%s' Lizards here.\n",
		 work_string2, world->player[new_hex.hex_owner].clan_name);

	  inc_combat_points (&hex);

	  fprintf (fptr, " - They are now %s warriors.\n",
		 liz_cp_desc [hex.combat_points]);

	  /* find captures for surviving lizards gateing into temple */

	  num_captured = ((float) (captured_red + captured_green +
		  captured_grey + captured_black + captured_yellow) / 100.00) *
		  (float) capture_level;

	  capture_lizards (world,
			   &hex, num_captured, &captured_red, &captured_green,
	    &captured_grey, &captured_black, &captured_yellow, &new_red,
	    &new_green, &new_grey, &new_black, &new_yellow);

	  if (new_red + new_green + new_grey + new_black + new_yellow > 0)
	  {
	    fprintf (fptr, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
	      show_lizards (new_red, new_green, new_grey, new_black, new_yellow,
		temp_string));
	  }

	  put_hex (world, x, y, &hex);

	  /* fclose(fptr); */
	}
      } else {
	/* all of the gating lizards must have been killed by explosion. */

	fprintf (fptr, " - None of the enemy Lizards at %s survive!\n", work_string2);

	/* fclose(fptr); */
      }
    } else {
      if (lizards_in_hex (&new_hex) + lizards_in_hex (&hex) > MAX_LIZARDS_IN_HEX)
      {
	fprintf (fptr, "your Lizards at %s disappear!\n", work_string);
	fprintf (fptr, " - Only to re-appear in Temple at %s.\n", work_string2);

	fprintf (fptr, " - The Temple at %s showers sparks and smoke!\n", work_string2);

	work = lizards_in_hex (&new_hex) + lizards_in_hex (&hex) - MAX_LIZARDS_IN_HEX;

	kill_lizards_xy (&hex, work);

	fprintf (fptr, " - %d Lizards here suddenly explode in sodden gouts, showering the temple.\n",
		 work);
      } else {
	fprintf (fptr, "your Lizards at %s disappear!\n", work_string);
	fprintf (fptr, " - Only to re-appear in Temple at %s.\n", work_string2);
      }
    }
  } else {
    if (lizards_in_hex (&new_hex) + lizards_in_hex (&hex) > MAX_LIZARDS_IN_HEX)
    {
      fprintf (fptr, "your Lizards at %s disappear!\n", work_string);
      fprintf (fptr, " - Only to re-appear in Temple at %s.\n", work_string2);

      fprintf (fptr, " - The Temple at %s showers sparks and smoke!\n", work_string2);

      work = lizards_in_hex (&new_hex) + lizards_in_hex (&hex) - MAX_LIZARDS_IN_HEX;

      kill_lizards_xy (&hex, work);

      fprintf (fptr, " - %d Lizards here suddenly explode in sodden gouts, showering the temple.\n",
	       work);
    } else {
      fprintf (fptr, "your Lizards at %s disappear!\n", work_string);
      fprintf (fptr, " - Only to re-appear in Temple at %s.\n", work_string2);
    }
  }

  /* fclose(fptr); */

  if (lizards_in_hex (&hex) > 0)
  {
    lizards_arrived_f = LZ_TRUE;

    order->x_hex = dest_x;
    order->y_hex = dest_y;

    new_hex.hex_owner = hex.hex_owner;
    new_hex.lizards_immobile = 0;

    if (lizards_in_hex (&new_hex) > 0)
    {
      new_hex.combat_points = average_combat_points (
	lizards_in_hex (&new_hex), new_hex.combat_points,
	lizards_in_hex (&hex), hex.combat_points);

      new_hex.turns_hungry = average_hunger_points (
	lizards_in_hex (&new_hex), new_hex.turns_hungry,
	lizards_in_hex (&hex), hex.turns_hungry);
    } else {
      new_hex.combat_points = hex.combat_points;
      new_hex.turns_hungry = hex.turns_hungry;
    }

    new_hex.red_lizards += hex.red_lizards;
    new_hex.green_lizards += hex.green_lizards;
    new_hex.grey_lizards += hex.grey_lizards;
    new_hex.black_lizards += hex.black_lizards;
    new_hex.yellow_lizards += hex.yellow_lizards;

    hex.lizards_immobile = 0;
    hex.turns_hungry = 0;
    hex.combat_points = 0;
    hex.red_lizards = 0;
    hex.green_lizards = 0;
    hex.grey_lizards = 0;
    hex.black_lizards = 0;
    hex.yellow_lizards = 0;
  }

  put_hex (world, x, y, &hex);
  put_hex (world, dest_x, dest_y, &new_hex);

  in_sight (world, x, y, players);

  players [order->player] = 0;

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    already_seen [work] = 0;

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      fprintf (fptr, "\n - '%s' Clan Lizards in Temple at %s suddenly disappear!\n",
	       world->player[hex.hex_owner].clan_name, work_string);

      already_seen [work] = 1;

      /* fclose(fptr); */
    }

  if (lizards_arrived_f)
  {
    in_sight (world, dest_x, dest_y, players);

    players [order->player] = 0;

    if (hex.hex_owner != order->player)
      players [hex.hex_owner] = 0;

    for (work = 1; work < MAX_PLAYERS + 1; work ++)
      if (players [work] > 0)
      {
	open_player_mesg (work, &fptr);

	if (already_seen [work] == 1)
	  fprintf (fptr, " - Only to re-appear in Temple at %s.\n", work_string2);
	else
	  fprintf (fptr, "\n - '%s' Clan Lizards suddenly appear in Temple at %s!\n",
	         world->player[hex.hex_owner].clan_name, work_string2);

	/* fclose(fptr); */
      }
  }

  return (1);
}
