#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"
#include "orders.h"

#include "moreords.h"
#include "terrain.h"
#include "blddest.h"

#include "lizhex.h"
#include "lizmisc.h"

extern char player_code [MAX_PLAYERS + 1][6];
extern char player_name [MAX_PLAYERS + 1][37];
extern char player_clan_name [MAX_PLAYERS + 1][37];
extern char player_home_den [MAX_PLAYERS + 1];
extern char players_in_game;

extern order_t *order_list [MAX_ORDERS_PER_TURN];

extern char game_code [5];

extern int world_x,
	   world_y,
	   number_orders,
	   turn_number;

extern spy_t *spy_list [MAX_SPYS];
extern int number_spys;

extern char dir [7][3];

extern char lizard_colours [6][12];

extern char *message_list [MAX_RANDOM_MESSAGES];

extern unsigned int spawn_turn_switch,
		    capture_switch,
                    volcano_dormant_switch,
		    peak_active_switch,
		    world_wrap_switch,
                    victory_points_switch,
                    home_owned_worth,
                    home_victory_points,
                    give_switch;

void build_fertile_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex contains none of your Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.green_lizards < BUILD_FERTILE_LEVEL)
  {
    if (world_hex.green_lizards == 0)
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex contains no Green Lizards!\n",
	       work_string);
    else
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex contains only %d Green Lizards!\n",
	       work_string, world_hex.green_lizards);

    fclose (fptr);

    return;
  }

  switch (world_hex.terrain)
  {
    case DEN:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex occupied by Den!\n",
	       work_string);

      fclose (fptr);

      return;
    case HOME_DEN:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex occupied by Home Den!\n",
	       work_string);

      fclose (fptr);

      return;
    case RUIN:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex occupied by Ruin!\n",
	       work_string);

      fclose (fptr);

      return;
    case PEAK:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as Green Lizards cannot plant on Peak!\n",
	       work_string);

      fclose (fptr);

      return;
    case VOLCANO:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as Green Lizards cannot plant on Volcano!\n",
	       work_string);

      fclose (fptr);

      return;
    case WATER:
    case WHIRLPOOL:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as Green Lizards cannot plant in Water!\n",
	       work_string);

      fclose (fptr);

      return;
    case TEMPLE:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as Green Lizards will not desecrate Temple here!\n",
	       work_string);

      fclose (fptr);

      return;
    case FERTILE:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as this hex is ALREADY Fertile!\n",
	       work_string);

      fclose (fptr);

      return;
    case CURSED:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as hex is CURSED and nothing will grow there.\n", work_string);

      fclose (fptr);

      return;
    case SWAMP:
    case PLAIN:
    case SCRUB:
      fprintf (fptr, " - Fertile hex planted at %s by %d Green Lizards.\n",
	       work_string, world_hex.green_lizards);
  }

  world_hex.terrain = FERTILE;

  put_hex (order->x_hex, order->y_hex, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	(order_list [work]->order_type == BUILD_FERTILE ||
	 order_list [work]->order_type == DESTROY_FERTILE ||
	 order_list [work]->order_type == BUILD_RAFT ))
    {
      if (order_list [work]->order_type == BUILD_FERTILE)
        fprintf (fptr, " - NEXT Build Fertile Hex Order at %s ignored, only one Build Fertile Order allowed per hex, per turn.\n",
		 work_string);
      else
        if (order_list [work]->order_type == DESTROY_FERTILE)
          fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored, Green lizards exhausted from planting.\n",
		 work_string);
	else
          fprintf (fptr, " - Build Raft Order at %s ignored, Green lizards exhausted from planting.\n",
		 work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void build_den_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Build Den Order at %s ignored as hex contains none of your Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.grey_lizards < BUILD_DEN_LEVEL)
  {
    if (world_hex.grey_lizards == 0)
      fprintf (fptr, " - Build Den Order at %s ignored as hex contains no Grey Lizards!\n",
	       work_string);
    else
      fprintf (fptr, " - Build Den Order at %s ignored as hex contains only %d Grey Lizards!\n",
	       work_string, world_hex.grey_lizards);

    fclose (fptr);

    return;
  }

  switch (world_hex.terrain)
  {
    case DEN:
      fprintf (fptr, " - Build Den Order at %s ignored as hex ALREADY occupied by Den!\n",
	       work_string);

      fclose (fptr);

      return;
    case HOME_DEN:
      fprintf (fptr, " - Build Den Order at %s ignored as hex ALREADY occupied by Home Den!\n",
	       work_string);

      fclose (fptr);

      return;
    case PEAK:
      fprintf (fptr, " - Build Den Order at %s ignored as Grey Lizards cannot build on Peak!\n",
	       work_string);

      fclose (fptr);

      return;
    case VOLCANO:
      fprintf (fptr, " - Build Den Order at %s ignored as Grey Lizards cannot build on Volcano!\n",
	       work_string);

      fclose (fptr);

      return;
    case WATER:
    case WHIRLPOOL:
      fprintf (fptr, " - Build Den Order at %s ignored as Grey Lizards cannot build in Water!\n",
	       work_string);

      fclose (fptr);

      return;
    case TEMPLE:
      fprintf (fptr, " - Build Den Order at %s ignored as Grey Lizards will not desecrate Temple here!\n",
	       work_string);

      fclose (fptr);

      return;
    case CURSED:
      fprintf (fptr, " - Build Den Order at %s ignored as hex is CURSED and nothing will live there.\n", work_string);

      fclose (fptr);

      return;
    case SWAMP:
      fprintf (fptr, " - Build Den Order at %s ignored as Grey Lizards cannot build in Swamp!\n",
	       work_string);

      fclose (fptr);

      return;
    case FERTILE:
    case PLAIN:
    case SCRUB:
      if (world_hex.turns_undisturbed == 1)
      {
	fprintf (fptr, " - Den completed at %s by %d Grey Lizards. Den is presently un-populated.\n",
		 work_string, world_hex.grey_lizards);

	world_hex.terrain = DEN;
	world_hex.den_lizard_type = 0;
	world_hex.hex_owner = order->player;
	world_hex.turns_undisturbed = 0;

	put_hex (order->x_hex, order->y_hex, &world_hex);
      } else {
	fprintf (fptr, " - Den at %s has been half built. Den MUST be completed NEXT TURN with another Build order!\n",
		 work_string);

	world_hex.turns_undisturbed = 2;

	put_hex (order->x_hex, order->y_hex, &world_hex);
      }

      break;
    case RUIN:
      fprintf (fptr, " - The %d Grey Lizards rebuild the Ruin at %s into a Den. Den is presently un-populated.\n",
	       world_hex.grey_lizards, work_string);

      world_hex.terrain = DEN;
      world_hex.den_lizard_type = 0;
      world_hex.hex_owner = order->player;
      world_hex.turns_undisturbed = 0;

      put_hex (order->x_hex, order->y_hex, &world_hex);

      break;
  }

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	(order_list [work]->order_type == BUILD_DEN ||
	 order_list [work]->order_type == BUILD_BRIDGE ||
	 order_list [work]->order_type == DESTROY_BRIDGE))
    {
      if (order_list [work]->order_type == BUILD_DEN)
        fprintf (fptr, " - NEXT Build Den Order at %s ignored, only one Build Den Order allowed per hex, per turn.\n",
	       work_string);
      else
	if (order_list [work]->order_type == BUILD_BRIDGE)
          fprintf (fptr, " - Build Bridge Order at %s ignored, Grey lizards exhausted from building den.\n",
	       work_string);
	else
          fprintf (fptr, " - Destroy Bridge Order at %s ignored, Grey lizards exhausted from building den.\n",
	       work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void build_bridge_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work,
      work2,
      x,
      y,
      x2,
      y2,
      off_world,
      no_land;

  hex_t world_hex,
	work_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Build Bridge Order at %s ignored as hex contains none of your Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.grey_lizards < BUILD_BRIDGE_LEVEL)
  {
    if (world_hex.grey_lizards == 0)
      fprintf (fptr, " - Build Bridge Order at %s ignored as hex contains no Grey Lizards!\n",
	       work_string);
    else
      fprintf (fptr, " - Build Bridge Order at %s ignored as hex contains only %d Grey Lizards!\n",
	       work_string, world_hex.grey_lizards);

    fclose (fptr);

    return;
  }

  if (world_hex.terrain == WATER || world_hex.terrain == WHIRLPOOL)
  {
    fprintf (fptr, " - Build Bridge Order at %s ignored as Grey Lizards are not on solid ground!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  move_coords (order->moves [0], order->x_hex, order->y_hex,
		     &x, &y);

  if (order->x_hex == x && order->y_hex == y)
  {
    fprintf (fptr, " - Build Bridge Order at %s ignored as attempted to build OFF EDGE OF WORLD!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  get_hex (x, y, &world_hex);

  fprintf (fptr, " - Surveying hex %s of %s for Bridge.",
	   dir [order->moves [0]], work_string);

  if (world_hex.bridge_here == 1)
  {
    fprintf (fptr, " Build Bridge Order ignored as there is already a Bridge there!\n");

    fclose (fptr);

    return;
  }

  if (world_hex.raft_here > 0)
  {
    fprintf (fptr, " Build Bridge Order ignored as there is a Raft there!\n");

    fclose (fptr);

    return;
  }

  if (lizards_in_hex (&world_hex) > 0)
  {
    fprintf (fptr, " Build Bridge Order ignored as there are Lizards in hex!\n");

    fclose (fptr);

    return;
  }

  switch (world_hex.terrain)
  {
    case DEN:
      fprintf (fptr, " Build Bridge Order ignored as hex occupied by Den!\n");

      fclose (fptr);

      return;
    case HOME_DEN:
      fprintf (fptr, " Build Bridge Order ignored as hex occupied by Home Den!\n");

      fclose (fptr);

      return;
    case RUIN:
      fprintf (fptr, " Build Bridge Order ignored as hex occupied by Ruin!\n");

      fclose (fptr);

      return;
    case SWAMP:
      fprintf (fptr, " Build Bridge Order ignored as hex is Swamp!\n");

      fclose (fptr);

      return;
    case PEAK:
      fprintf (fptr, " Build Bridge Order ignored as hex is Peak!\n");

      fclose (fptr);

      return;
    case VOLCANO:
      fprintf (fptr, " Build Bridge Order ignored as hex is Volcano!\n");

      fclose (fptr);

      return;
    case TEMPLE:
      fprintf (fptr, " Build Bridge Order ignored as hex is Volcano!\n");

      fclose (fptr);

      return;
    case FERTILE:
      fprintf (fptr, " Build Bridge Order ignored as hex is Fertile!\n");

      fclose (fptr);

      return;
    case PLAIN:
      fprintf (fptr, " Build Bridge Order ignored as hex is Plain!\n");

      fclose (fptr);

      return;
    case SCRUB:
      fprintf (fptr, " Build Bridge Order ignored as hex is Scrub!\n");

      fclose (fptr);

      return;
    case CURSED:
      fprintf (fptr, " - Build Bridge Order at %s ignored as hex is CURSED.\n", work_string);

      fclose (fptr);

      return;
    case WATER:
    case WHIRLPOOL:
      break;
  }

  off_world = 0;

  no_land = 0;

  work2 = order->moves [0] - 1;

  for (work = 0; work < 3; work ++, work2 ++)
  {
    if (work2 == 0)
      work2 = 6;
    else
      if (work2 == 7)
	work2 = 1;

    move_coords (work2, x, y, &x2, &y2);

    if (x == x2 && y == y2)
      off_world ++;

    get_hex (x2, y2, &work_hex);

    if (work_hex.terrain == WATER || work_hex.terrain == WHIRLPOOL)
      no_land ++;

    xy_to_string (x, y, work_string);
  }

  if (off_world == 3)
  {
    fprintf (fptr, " Build Bridge Order ignored as bridge would lead OFF EDGE OF WORLD!\n");

    fclose (fptr);

    return;
  }

  if (no_land == 3)
  {
    fprintf (fptr, " Build Bridge Order ignored as bridge MUST lead to another LAND hex!\n",
	       work_string);

    fclose (fptr);

    return;
  }

  fprintf (fptr, " Grey Lizards build Bridge at %s.\n", work_string);

  world_hex.hex_owner = order->player;
  world_hex.bridge_here = 1;

  put_hex (x, y, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	(order_list [work]->order_type == BUILD_BRIDGE ||
	 order_list [work]->order_type == DESTROY_BRIDGE ||
	 order_list [work]->order_type == BUILD_DEN))
    {
      xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
	work_string);

      if (order_list [work]->order_type == BUILD_BRIDGE)
        fprintf (fptr, " - Next Build Bridge Order at %s ignored, only on Build Bridge Order allowed per hex, per turn.\n",
	       work_string);
      else
	if (order_list [work]->order_type == DESTROY_BRIDGE)
          fprintf (fptr, " - Destroy Bridge Order at %s ignored, Grey lizards exhausted from building bridge.\n",
	       work_string);
	else
          fprintf (fptr, " - Build Den Order at %s ignored, Grey lizards exhausted from building bridge.\n",
	       work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void destroy_fertile_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex contains none of your Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.green_lizards < BUILD_FERTILE_LEVEL)
  {
    if (world_hex.green_lizards == 0)
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex contains no Green Lizards!\n",
	       work_string);
    else
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex contains only %d Green Lizards!\n",
	       work_string, world_hex.green_lizards);

    fclose (fptr);

    return;
  }

  switch (world_hex.terrain)
  {
    case DEN:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex occupied by Den!\n",
	       work_string);

      fclose (fptr);

      return;
    case HOME_DEN:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex occupied by Home Den!\n",
	       work_string);

      fclose (fptr);

      return;
    case RUIN:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex occupied by Ruin!\n",
	       work_string);

      fclose (fptr);

      return;
    case SWAMP:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex is Swamp!\n",
	       work_string);

      fclose (fptr);

      return;
    case PEAK:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex is Peak!\n",
	       work_string);

      fclose (fptr);

      return;
    case VOLCANO:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex is Volcano!\n",
	       work_string);

      fclose (fptr);

      return;
    case WATER:
    case WHIRLPOOL:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex is Water!\n",
	       work_string);

      fclose (fptr);

      return;
    case TEMPLE:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as hex occupied by a Temple!\n",
	       work_string);

      fclose (fptr);

      return;
    case SCRUB:
      fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored as this hex is Scrub!\n",
	       work_string);

      fclose (fptr);

      return;
    case PLAIN:
      fprintf (fptr, " - Plant Fertile Hex Order at %s ignored as this hex is Plain!\n",
	       work_string);

      fclose (fptr);

      return;
    case CURSED:
      fprintf (fptr, " - Destroy Fertile Hex at %s ignored as hex is CURSED.\n", work_string);

      fclose (fptr);

      return;
    case FERTILE:
      fprintf (fptr, " - Fertile hex at %s poisoned by %d Green Lizards.\n",
	       work_string, world_hex.green_lizards);
  }

  world_hex.terrain = PLAIN;

  put_hex (order->x_hex, order->y_hex, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	(order_list [work]->order_type == DESTROY_FERTILE ||
	 order_list [work]->order_type == BUILD_FERTILE ||
	 order_list [work]->order_type == BUILD_RAFT))
    {
      xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
	work_string);

      if (order_list [work]->order_type == DESTROY_FERTILE)
        fprintf (fptr, " - Next Destroy Fertile Hex Order at %s ignored, only one Destroy Fertile Order allowed per hex, per turn.\n",
	       work_string);
      else
	if (order_list [work]->order_type == BUILD_FERTILE)
          fprintf (fptr, " - Build Fertile Order at %s ignored, Green lizards exhausted from digging up hex.\n",
	    work_string);
	else
          fprintf (fptr, " - Build Raft Order at %s ignored, Green lizards exhausted from digging up hex.\n",
	    work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void destroy_bridge_order (order_t *order)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80],
       players [MAX_PLAYERS + 1];

  int work,
      x,
      y;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Destroy Bridge Order at %s ignored as hex contains none of your Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.grey_lizards < BUILD_BRIDGE_LEVEL)
  {
    if (world_hex.grey_lizards == 0)
      fprintf (fptr, " - Destroy Bridge Order at %s ignored as hex contains no Grey Lizards!\n",
	       work_string);
    else
      fprintf (fptr, " - Destroy Bridge Order at %s ignored as hex contains only %d Grey Lizards!\n",
	       work_string, world_hex.grey_lizards);

    fclose (fptr);

    return;
  }

  move_coords (order->moves [0], order->x_hex, order->y_hex,
		     &x, &y);

  if (order->x_hex == x && order->y_hex == y)
  {
    fprintf (fptr, " - Destroy Bridge Order at %s ignored as attempted to destroy Bridge OFF EDGE OF WORLD!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  get_hex (x, y, &world_hex);

  fprintf (fptr, " - Examining hex %s of %s for Bridge.",
	   dir [order->moves [0]], work_string);

  if (world_hex.bridge_here == 0)
  {
    fprintf (fptr, " Destroy Bridge Order ignored as there is NO Bridge there!\n");

    fclose (fptr);

    return;
  }

  xy_to_string (x, y, work_string);

  fprintf (fptr, " Grey Lizards destroy Bridge at %s.\n", work_string);

  world_hex.bridge_here = 0;

  in_sight (x, y, players);

  fclose (fptr);

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      if (work == order->player)
	fprintf (fptr, " - Bridge at %s collapses!\n", work_string);
      else
	fprintf (fptr, "\n - Bridge at %s collapses!\n", work_string);

      fclose (fptr);
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

      fclose (fptr);

      whirlpool_kill (x, y);
    } else {
      fprintf (fptr, " - %s Lizards at %s fall into Water!\n",
	       show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	       world_hex.grey_lizards, world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string), work_string);

      fclose (fptr);

      bridge_collapse_kill (x, y);
    }
  }

  put_hex (x, y, &world_hex);

  open_player_mesg (order->player, &fptr);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	(order_list [work]->order_type == DESTROY_BRIDGE ||
	 order_list [work]->order_type == BUILD_BRIDGE ||
	 order_list [work]->order_type == BUILD_DEN))
    {
      xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
	work_string);

      if (order_list [work]->order_type == DESTROY_BRIDGE)
        fprintf (fptr, " - Next Destroy Bridge Order at %s ignored, only one Destroy Bridge Order allowed per hex, per turn.\n",
		 work_string);
      else
	if (order_list [work]->order_type == BUILD_BRIDGE)
          fprintf (fptr, " - Build Bridge Order at %s ignored, Grey lizards exhausted from destroying bridge.\n",
	    work_string);
	else
          fprintf (fptr, " - Build Den Order at %s ignored, Grey lizards exhausted from destroying bridge.\n",
	    work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void build_raft_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work,
      x,
      y;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Build Raft Order at %s ignored as hex contains none of your Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.green_lizards < BUILD_RAFT_LEVEL)
  {
    if (world_hex.green_lizards == 0)
      fprintf (fptr, " - Build Raft Order at %s ignored as hex contains no Green Lizards!\n",
	       work_string);
    else
      fprintf (fptr, " - Build Raft Order at %s ignored as hex contains only %d Green Lizards!\n",
	       work_string, world_hex.green_lizards);

    fclose (fptr);

    return;
  }

  if (world_hex.terrain != SCRUB)
  {
    fprintf (fptr, " - Build Raft Order at %s ignored as Green Lizards are not in Scrub hex!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  move_coords (order->moves [0], order->x_hex, order->y_hex,
		     &x, &y);

  if (order->x_hex == x && order->y_hex == y)
  {
    fprintf (fptr, " - Build Raft Order at %s ignored as attempted to build OFF EDGE OF WORLD!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  get_hex (x, y, &world_hex);

  if (world_hex.terrain != WATER && world_hex.terrain != WHIRLPOOL)
  {
    fprintf (fptr, " - Build Raft Order ignored as Green Lizards can only build rafts in WATER!\n");

    fclose (fptr);

    return;
  }

  xy_to_string (x, y, work_string);

  if (world_hex.bridge_here == 1)
  {
    fprintf (fptr, " - Build Raft Order ignored as there is a Bridge at %s.\n",
      work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.raft_here > 0)
  {
    fprintf (fptr, " - Build Raft Order ignored as there is already a Raft at %s.\n",
	     work_string);

    fclose (fptr);

    return;
  }

  fprintf (fptr, " - Green Lizards build Raft at %s from Scrub hex.\n", work_string);

  world_hex.raft_here = random (RAFT_LIFE) + 5;

  put_hex (x, y, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	(order_list [work]->order_type == BUILD_RAFT ||
	 order_list [work]->order_type == BUILD_FERTILE ||
	 order_list [work]->order_type == DESTROY_FERTILE))
    {
      xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
	work_string);

      if (order_list [work]->order_type == BUILD_RAFT)
	fprintf (fptr, " - Next Build Raft Order at %s ignored, only allowed one Build Raft Order per hex, per turn.\n",
	       work_string);
      else
	if (order_list [work]->order_type == BUILD_FERTILE)
	  fprintf (fptr, " - Build Fertile Hex Order at %s ignored, Green lizards exhausted from building raft.\n",
	    work_string);
	else
          fprintf (fptr, " - Destroy Fertile Hex Order at %s ignored, Green lizards exhausted from building raft.\n",
	    work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}
