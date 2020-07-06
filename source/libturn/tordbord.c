#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "strgfunc.h"
#include "lizmisc.h"

#define NUM_POSSIBLE_LINES 22

typedef struct TextOrderRecord {
  char *full;
  char *abb;
  int order;
  int (*func)(world_t *, char *strs[], int player);
} text_t;

static char* order_colours [5] = { "RED", "GRN", "GRY", "BLK", "YEL" };
static char* chant_code [NUMBER_CHANTS] = { "EYES", "REVE", "SHUN", "MOUL",
					    "COER", "TRAN", "DARK", "FREE",
					    "SUMM", "GATE" };

/* ----------------------------------------------------------------------
--
-- add_order
--
---------------------------------------------------------------------- */
static int add_order (world_t *w, order_t *order)
{
  char hex_loc [20];

  int i, split_total;

  hex_t *hex;

  switch (order->type)
    {
    case SPLIT:
      for (i = 0, split_total = 0; i < w->num_orders; i++)
	if (w->order_list[i]->type == SPLIT &&
	    order->player == w->order_list [i]->player &&
	    coord_eq(order->source, w->order_list [i]->source))
	  {
	    split_total += lizards_in_band(&w->order_list[i]->feature);
	  }
      hex = get_hex (w, order->source);
      if (split_total >= lizards_in_hex (hex))
	{
	    xy_to_string (order->source.x, order->source.y, hex_loc);
	    fprintf(stdout, "(order-failed-stage-2 split %d %d \"not enough lizards in hex\")\n",
		    order->source.x, order->source.y);
	    return 0;
	}
      break;
    case MOVE:
      for (i = 0; i < w->num_orders; i++)
	if (w->order_list [i]->type == MOVE &&
	    order->player == w->order_list [i]->player &&
	    coord_eq(order->source, w->order_list [i]->source))
	  {
	    /* Redundant order ignored! */
	    xy_to_string (order->source.x, order->source.y, hex_loc);
	    fprintf(stdout, "(order-failed-stage-2 move %d %d \"not enough lizards in hex\")\n",
		    order->source.x, order->source.y);
	    return 0;
	  }
      break;
#ifdef IgnoreThisStuff

    case SNEAK:
      for (work1 = 0, sneak_total = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == SNEAK &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
	{
	  sneak_total ++;
	}

	get_hex (world, order->x_hex, order->y_hex, &hex);

	if (sneak_total >= spy_lizards_in_hex (world, order->x_hex, order->y_hex, order->player))
	{
	  /* Redundant order ignored! */

	  xy_to_string (order->x_hex, order->y_hex, hex_loc);

	  if (order->moves [1] != 0)
	    player_mesg (world, order->player,
	      " - SNEAK %s %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    player_mesg (world, order->player,
	      " - SNEAK %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]]);

	  return;
	}

      break;
    case CHANT:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == CHANT &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	if (order->moves [1] != 0)
	  player_mesg (order->player,
	    " - CHANT %s %s %s %s ignored, only one chant allowed from hex.\n",
	    hex_loc, dir [order->moves [0]], dir [order->moves [1]],
	    chant_code [order->yellow_lizards]);
	else
	  player_mesg (order->player,
	    " - CHANT %s %s %s ignored, only one chant allowed from hex.\n",
	    hex_loc, dir [order->moves [0]],
	    chant_code [order->yellow_lizards]);

	return;
      }

      break;
    case LOB:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == LOB &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	if (order->moves [1] != 0)
	  player_mesg (order->player,
	    " - LOB %s %s %s ignored, only one lob allowed from hex.\n",
	    hex_loc, dir [order->moves [0]], dir [order->moves [1]]);
	else
	  player_mesg (order->player,
	    " - LOB %s %s ignored, only one lob allowed from hex.\n",
	    hex_loc, dir [order->moves [0]]);

	return;
      }

      break;
    case BUILD_FERTILE:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == BUILD_FERTILE &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	player_mesg (order->player,
	  " - BUILD FERTILE %s ignored, only one fertile build allowed from hex.\n",
	  hex_loc);

	return;
      }

      break;
    case BUILD_DEN:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == BUILD_DEN &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	player_mesg (order->player,
	  " - BUILD DEN %s ignored, only one den build allowed from hex.\n",
	  hex_loc);

	return;
      }

      break;
    case BUILD_BRIDGE:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == BUILD_BRIDGE &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	player_mesg (order->player,
	  " - BUILD BRIDGE %s %s ignored, only one bridge build allowed from hex.\n",
	  hex_loc, order->moves [0]);

	return;
      }

      break;
    case DESTROY_FERTILE:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == DESTROY_FERTILE &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	player_mesg (order->player,
	  " - DESTROY FERTILE %s ignored, only one fertile destroy allowed from hex.\n",
	  hex_loc);

	return;
      }

      break;
    case DESTROY_BRIDGE:
      for (work1 = 0; work1 < world->num_orders; work1 ++)
	if (world->order_list [work1]->order_type == DESTROY_BRIDGE &&
	    order->player == world->order_list [work1]->player &&
	    order->x_hex == world->order_list [work1]->x_hex &&
	    order->y_hex == world->order_list [work1]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	player_mesg (order->player,
	  " - DESTROY BRIDGE %s %s ignored, only one bridge destroy allowed from hex.\n",
	  hex_loc, order->moves [0]);

	return;
      }

      break;
#endif
  }
  w->order_list [w->num_orders] = order;
  w->num_orders ++;
  if (w->num_orders >= MAX_ORDERS_PER_TURN)
    {
      printf ("FATAL ERROR: Exceeded Maximum of %d orders per turn.\n",
	      MAX_ORDERS_PER_TURN);
      exit (EXIT_FAILURE);
    }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- free_order
--
---------------------------------------------------------------------- */
static void free_order(order_t *order)
{
  order_t *next;
  while (order)
    {
      next = order->chain;
      free(order);
      order = next;
    }
}

/* ----------------------------------------------------------------------
--
-- order_move
--
---------------------------------------------------------------------- */
static int order_move(world_t *w, char *strs[], int player)
{
  order_t *order, *orders = NULL;
  hex_t *hex;
  feature_t *f;
  coord xy;
  int i, dir;

  xy = new_string_to_loc(strs[1]);
  for (i = 2; i < 4; i++)
    if (strs[i] && (dir = string_to_direction (strs[i])) &&
	(hex = get_hex(w, xy)) &&
	(hex->owner == player)  &&
	get_hex(w, xy = move_coords(w, hex, dir)))
      {
	order = new_order(player, MOVE, new_string_to_loc (strs[1]));
	order->dest = xy;
	if ((f = has_feature(hex, BAND)))
	  {
	    memcpy(&order->feature, f, sizeof(feature_t));
	  }
	else
	  {
	    memset(&order->feature, 0, sizeof(feature_t));
	    order->feature.type = BAND;
	  }
	orders = new_order_append (orders, order);      
	printf("Create order MOVE from (%d,%d) to (%d,%d)\n",
	       order->source.x,order->source.y,
	       order->dest.x,order->dest.y);
       
      }
  if (orders && (add_order (w, orders) == 0))
    free_order(orders);
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_split
--
---------------------------------------------------------------------- */
static int order_split(world_t *w, char *strs[], int player)
{
  coord xy;
  hex_t *hex;
  feature_t f;
  int i, j, num;
  order_t *order, *orders = NULL;
  xy = new_string_to_loc(strs[1]);
  if ((hex = get_hex(w, xy)) == NULL)
    return 0;
  if (hex->owner != player)
    {
      fprintf(stdout,
	      "(order-failed-stage-1 split %s \"does not own hex\")\n",
	      w->player[player]->code);
      return 0;
    }

  for (i = 2; i < 10; i++)
    {
      if ((num = atoi (strs[i])) == 0)
	break;
      for (j = 0; j < LIZARD_TYPES; j++)
	if (strcmp (strs[i] + strlen(strs[i]) - 3, order_colours [j]) == 0)
	  break;
      f.f.band.type[j] = num;
      f.type = BAND;
    }
  for (j = i; j < i + 2; j++)
    {
      coord source, dest;
      int dir;
      dir = string_to_direction (strs[j]);
      source = hex->xy;
      dest = move_coords(w, hex, dir);
      if (dir && get_hex(w, dest) && lizards_in_band(&f))
	{
	  printf("Create order SPLIT (%d,%d,%d,%d,%d) from (%d,%d) to (%d,%d)\n",
		 f.f.band.type[RED], f.f.band.type[GREEN],f.f.band.type[GREY],
		 f.f.band.type[BLACK], f.f.band.type[YELLOW],
		 source.x,source.y,dest.x,dest.y);
 	  order = new_order(player, SPLIT, source);
	  order->dest = dest;
	  memcpy(&order->feature, &f, sizeof(feature_t));
	  orders = new_order_append(orders, order);
	}
      hex = get_hex(w, dest);
    }
  if (orders && (add_order (w, orders) == 0))
    free_order(orders);
  return 1;
}
/* ----------------------------------------------------------------------
--
-- order_sail
--
---------------------------------------------------------------------- */
static int order_sail(world_t *w, char *strs[], int player)
{
  coord xy;
  hex_t *hex;
  feature_t *f;
  int i, dir;
  order_t *order, *orders = NULL;
  xy = new_string_to_loc(strs[1]);

  if ((hex = get_hex(w, xy)) == NULL)
    return 0;
  if (hex->owner != player)
    {
      fprintf(stdout,
	      "(order-failed-stage-1 split %s \"does not own hex\")\n",
	      w->player[player]->code);
      return 0;
    }

  xy = new_string_to_loc(strs[1]);
  for (i = 2; i < 4; i++)
    if (strs[i] && (dir = string_to_direction (strs[i])) &&
	(hex = get_hex(w, xy)) &&
	(hex->owner == player)  &&
	get_hex(w, xy = move_coords(w, hex, dir)))
    {
      order = new_order(player, SAIL, new_string_to_loc (strs[1]));
      order->dest = xy;
      if ((f = has_feature(hex, BAND)))
	{
	  memcpy(&order->feature, f, sizeof(feature_t));
	}
      else
	{
	  memset(&order->feature, 0, sizeof(feature_t));
	  order->feature.type = BAND;
	}
      orders = new_order_append (orders, order);      
      printf("Create order SAIL from (%d,%d) to (%d,%d)\n",
	     order->source.x,order->source.y,
	     order->dest.x,order->dest.y);
       
    }
  if (orders && (add_order (w, orders) == 0))
    free_order(orders);
  return 1;
}
/* ----------------------------------------------------------------------
--
-- order_sneak
--
---------------------------------------------------------------------- */
static int order_sneak(world_t *w, char *strs[], int player)
{
  coord xy;
  hex_t *hex;
  int i, dir;
  order_t *order, *orders;
  xy = new_string_to_loc(strs[1]);

  if ((hex = get_hex(w, xy)) == NULL)
    return 0;
  if (hex->owner != player)
    {
      fprintf(stdout,
	      "(order-failed-stage-1 split %s \"does not own hex\")\n",
	      w->player[player]->code);
      return 0;
    }

  xy = new_string_to_loc(strs[1]);
  for (i = 2; i < 4; i++)
    if (strs[i] && (dir = string_to_direction (strs[i])) &&
	(hex = get_hex(w, xy)) &&
	(hex->owner == player)  &&
	get_hex(w, xy = move_coords(w, hex, dir)))
      {
	order = new_order(player, SNEAK, new_string_to_loc (strs[1]));
	order->dest = xy;
	orders = new_order_append (orders, order);      
	printf("Create order SNEAK from (%d,%d) to (%d,%d)\n",
	       order->source.x,order->source.y,
	       order->dest.x,order->dest.y);
      }
  if (orders && (add_order (w, orders) == 0))
    free_order(orders);
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_recruit
--
---------------------------------------------------------------------- */
static int order_recruit(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy;
  hex_t *hex;
  order_t *order;
  xy = new_string_to_loc(strs[1]);
  if ((hex = get_hex(w, xy)) == NULL)
    {
      fprintf(stdout,
	      "(order-failed-stage-1 recruit %s \"illegal hex\")\n",
	      xy_to_string(xy.x, xy.y, str));
      return 0;
    }
  order = new_order(player, RECRUIT, xy);
  if (!add_order (w, order))
    free(order);
  else
    printf("Create order RECRUIT from (%d,%d)\n",
	   order->source.x,order->source.y);
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_build_fertile
--
---------------------------------------------------------------------- */
static int order_build_fertile(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy;
  hex_t *hex;
  order_t *order;
  feature_t *f;

  xy = new_string_to_loc(strs[1]);
  xy_to_string(xy.x, xy.y, str);

  if ((hex = get_hex(w, xy)) == NULL)
    {
      player_mesg (w, player, " - 'BF %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    str);
      return 0;
    }
  order = new_order(player, BUILD_FERTILE, xy);
  if ((f = has_feature(hex, BAND)))
    {
      memcpy(&order->feature, f, sizeof(feature_t));
    }
  else
    {
      memset(&order->feature, 0, sizeof(feature_t));
      order->feature.type = BAND;
    }
  if (!add_order(w, order))
    free(order);
  printf("Create order BUILD_FERTILE at %s\n", str);
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_destroy_fertile
--
---------------------------------------------------------------------- */
static int order_destroy_fertile(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy;
  hex_t *hex;
  order_t *order;
  feature_t *f;

  xy = new_string_to_loc(strs[1]);
  xy_to_string(xy.x, xy.y, str);

  if ((hex = get_hex(w, xy)) == NULL)
    {
      player_mesg (w, player, " - 'DF %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    str);
      return 0;
    }
  order = new_order(player, DESTROY_FERTILE, xy);
  if ((f = has_feature(hex, BAND)))
    {
      memcpy(&order->feature, f, sizeof(feature_t));
    }
  else
    {
      memset(&order->feature, 0, sizeof(feature_t));
      order->feature.type = BAND;
    }
  if (!add_order(w, order))
    free(order);
  printf("Create order BUILD_FERTILE at %s\n", str);
  return 1;
}
/* ----------------------------------------------------------------------
--
-- order_build_raft
--
---------------------------------------------------------------------- */
static int order_build_raft(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy, xy2;
  hex_t *hex;
  order_t *order;
  feature_t *f;
  int dir;

  xy = new_string_to_loc(strs[1]);
  xy_to_string(xy.x, xy.y, str);

  if ((hex = get_hex(w, xy)) == NULL)
    {
      player_mesg (w, player, " - 'BR %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    str);
      return 0;
    }
  if ((dir = string_to_direction (strs[2])) &&
      get_hex(w, xy2 = move_coords(w, hex, dir)))
    {
      order = new_order(player, DESTROY_FERTILE, xy);
      order->dest = xy2;
      if ((f = has_feature(hex, BAND)))
	{
      memcpy(&order->feature, f, sizeof(feature_t));
	}
      else
	{
	  memset(&order->feature, 0, sizeof(feature_t));
	  order->feature.type = BAND;
	}
      if (!add_order(w, order))
	free(order);
      printf("Create order BUILD_RAFT at %s\n", str);
      return 1;
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- order_build_bridge
--
---------------------------------------------------------------------- */
static int order_build_bridge(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy, xy2;
  hex_t *hex;
  order_t *order;
  feature_t *f;
  int dir;

  xy = new_string_to_loc(strs[1]);
  xy_to_string(xy.x, xy.y, str);

  if ((hex = get_hex(w, xy)) == NULL)
    {
      player_mesg (w, player, " - 'BB %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.",
	    str);
      return 0;
    }

  if ((dir = string_to_direction (strs[2])) &&
      get_hex(w, xy2 = move_coords(w, hex, dir)))
    {
      order = new_order(player, DESTROY_FERTILE, xy);
      order->dest = xy2;
      if ((f = has_feature(hex, BAND)))
	{
	  memcpy(&order->feature, f, sizeof(feature_t));
	}
      else
	{
	  memset(&order->feature, 0, sizeof(feature_t));
	  order->feature.type = BAND;
	}
      if (!add_order(w, order))
	free(order);
      printf("Create order BUILD_FERTILE at %s\n", str);
      return 1;
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- order_destroy_bridge
--
---------------------------------------------------------------------- */
static int order_destroy_bridge(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy, xy2;
  hex_t *hex;
  order_t *order;
  feature_t *f;
  int dir;

  xy = new_string_to_loc(strs[1]);
  xy_to_string(xy.x, xy.y, str);

  if ((hex = get_hex(w, xy)) == NULL)
    {
      player_mesg (w, player, " - 'DB %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    str);
      return 0;
    }
  if ((dir = string_to_direction (strs[2])) &&
      get_hex(w, xy2 = move_coords(w, hex, dir)))
    {
      order = new_order(player, DESTROY_BRIDGE, xy);
      order->dest = xy2;
      if ((f = has_feature(hex, BAND)))
	{
	  memcpy(&order->feature, f, sizeof(feature_t));
	}
      else
	{
	  memset(&order->feature, 0, sizeof(feature_t));
	  order->feature.type = BAND;
	}
      if (!add_order(w, order))
	free(order);
      printf("Create order BUILD_FERTILE at %s\n", str);
      return 1;
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- order_build_den
--
---------------------------------------------------------------------- */
static int order_build_den(world_t *w, char *strs[], int player)
{
  char str[5];
  coord xy;
  hex_t *hex;
  order_t *order;
  feature_t *f;

  xy = new_string_to_loc(strs[1]);
  xy_to_string(xy.x, xy.y, str);

  if ((hex = get_hex(w, xy)) == NULL)
    {
      player_mesg (w, player, " - 'BB %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    str);
      return 0;
    }
  order = new_order(player, BUILD_DEN, xy);
  if ((f = has_feature(hex, BAND)))
    {
      memcpy(&order->feature, f, sizeof(feature_t));
    }
  else
    {
      memset(&order->feature, 0, sizeof(feature_t));
      order->feature.type = BAND;
    }
  if (!add_order(w, order))
    free(order);
  printf("Create order BUILD_DEN at %s\n", str);
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_message
--
---------------------------------------------------------------------- */
static int order_message(world_t *w, char *strs[], int player)
{
	  /*
	    if (present_player == 0)
	    {
	    printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");
	    exit (EXIT_FAILURE);
	    }
	    strcpy (tmp_string, work_string);
	    work1 = 0;
	    while (fgets (tmp_string2, 140, in_fptr) != '\0')
	    {
	    strcpy (mesg_string [work1], tmp_string2);
	    work1 ++;
	    if (strcmp (strupr (tmp_string2), "ENDMESG\n") == 0)
	    {
	    work1 --;
	    break;
	    }
	    if (work1 == 40)
	    break;
	    }
	    sprintf (filename, "%s%s%02d_%03d.%s", world->path,
	    PLYPLY_MESG_F, present_player, world->turn, world->game);
	    if ((fptr = fopen (filename, "at")) == '\0')
	    {
	    printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
	    filename);
	    exit (EXIT_FAILURE);
	    }
	    if (tmp_string [0] == '\0')
	    fprintf (fptr, " - Message SENT by you to ALL Players\n");
	    else
	    fprintf (fptr, " - Message SENT by you to Player(s): %s\n", tmp_string);
	    for (work2 = 0; work2 < work1; work2 ++)
	    fprintf (fptr, "   %s", mesg_string [work2]);
	    fprintf (fptr, "\n");
	    if (tmp_string [0] == '\0')
	    {
	    for (work3 = 1; work3 < world->num_players + 1; work3 ++)
	    {
	    if (work3 != present_player)
	    {
	    sprintf (filename, "%s%s%02d_%03d.%s", world->path,
	    PLYPLY_MESG_F, work3, world->turn, world->game);
	    if ((fptr = fopen (filename, "at")) == '\0')
	    {
	    printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
	    filename);
	    exit (EXIT_FAILURE);
	    }
	    fprintf (fptr, " - Message from Player %d, '%s' Clan:\n",
	    present_player,
	    world->player[present_player]->clan);
	    for (work2 = 0; work2 < work1; work2 ++)
	    fprintf (fptr, "   %s", mesg_string [work2]);
	    fprintf (fptr, "\n");
	    }
	    }
	    }
	    else
	    {
	    while (1)
	    {
	    strtoken (tmp_string2, tmp_string, " ");
	    if (tmp_string2 [0] == '\0')
	    break;
	    if (atoi (tmp_string2) != present_player &&
	    atoi (tmp_string2) != 0)
	    {
	    sprintf (filename, "%s%s%02d_%03d.%s", world->path,
	    PLYPLY_MESG_F, atoi (tmp_string2), world->turn, world->game);
	    if ((fptr = fopen (filename, "at")) == '\0')
	    {
	    printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
	    filename);
	    exit (EXIT_FAILURE);
	    }
	    fprintf (fptr, " - Message from Player %d, '%s' Clan:\n",
	    present_player,
	    world->player[present_player]->clan);
	    for (work2 = 0; work2 < work1; work2 ++)
	    fprintf (fptr, "   %s", mesg_string [work2]);
	    fprintf (fptr, "\n");
	    }
	    }
	    }
	  */
  return 1;
}

/* ----------------------------------------------------------------------
--
-- ProcessFile
--
---------------------------------------------------------------------- */
static text_t possible_lines [NUM_POSSIBLE_LINES] = {
  {"", "/*", INACTIVE, NULL},
  {"PLAYER", "PL", PLAYER, NULL},
  {"MESSAGE", "MS", MESSAGE, order_message},
  {"MOVE", "MO", MOVE, order_move},
  {"SPLIT", "SP",SPLIT, order_split},
  {"SAIL", "SA", SAIL, order_sail},
  {"SNEAK", "SN", SNEAK, order_sneak},
  {"BUILD_RAFT", "BR", BUILD_RAFT, order_build_raft},
  {"BUILD_FERTILE", "BF", BUILD_FERTILE, order_build_fertile},
  {"BUILD_DEN", "BD", BUILD_DEN, order_build_den},
  {"BUILD_BRIDGE", "BB", BUILD_BRIDGE, order_build_bridge},
  {"DESTROY_FERTILE", "DF", DESTROY_FERTILE, order_destroy_fertile},
  {"DESTROY_BRIDGE", "DB", DESTROY_BRIDGE, order_destroy_bridge},
  {"RECRUIT", "RE", RECRUIT, order_recruit},
  {"CHANT", "CH", CHANT, NULL},
  {"MOD_MESG", "MMS", MOD_MESG, NULL},
  {"GIVE", "GI", GIVE, NULL},
  {"LOB", "LO", LOB, NULL},
  {"FREE", "FR", FREE, NULL},
  {"KILL", "KI", KILL_PLAYER, NULL},
  {"ERUPT", "ER", ERUPT_VOLCANO, NULL},
  {"SPAWN_TURN", "ST", SPAWN_TURN, NULL}
};

static int ProcessFile(FILE *in_fptr, world_t *world)
{
  char str[256], *strs[15];
  int i, present_player;
  text_t *line_type;

  while (fgets (str, 256, in_fptr) != NULL)
    {
      for (i = 0; i < (int)(strlen(str)); i++)
	if (islower(str[i]))
	  str[i] = toupper(str[i]);
      
      for (i = 0, strs[i] = strtok(str, " \n");
	   (i < 12) && strs[i] && strlen(strs[i]);
	   strs[i] = strtok(NULL, " \n"))
	i++;
      for (i = 0; strs[i]; i++)
	printf("%s ", strs[i]);

      for (i = 0; i < NUM_POSSIBLE_LINES; i++)
	if (strcmp (strs[0], possible_lines[i].full) == 0 ||
	    strcmp (strs[0], possible_lines[i].abb) == 0)
	  break;
      line_type = &possible_lines[i];
      if (present_player == 0 && (line_type->order != PLAYER))
	{
	  fprintf (stdout, "(order-fail-stage-1 \"Order issued when player number unknown\")\n");
	  continue;
	}
      if (line_type->func)
	(*line_type->func)(world, strs, present_player);
      else
	switch (line_type->order)
	  {
	  case PLAYER:
	    i = atoi (strs[1]);
	    if ((i >= 1) && (i < world->num_players)  && world->player[i])
	      {
		present_player = i;
		printf ("Present Player: %d - %s.\n\n", i,
			world->player[i]->code);
	      }
	    else
	      {
		fprintf(stdout, "(order-failed-stage-1 player %d \"player does not exist\")\n", i);
		present_player = 0;
	      }
	    break;
#ifdef IgnoreThisStuff
      case 22:          /* Chant Order */
      case 23:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = CHANT;

	strtoken (tmp_string2, tmp_string, " ");

	string_to_loc (tmp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (world, work_x, work_y, &world_hex);

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	strtoken (tmp_string2, tmp_string, " ");

	if ((work_order.black_lizards = atoi (tmp_string2)) == 0)
	  work_order.black_lizards = world_hex.black_lizards;
	else
	  strtoken (tmp_string2, tmp_string, " ");

	if (work_order.black_lizards > world_hex.black_lizards)
	  work_order.black_lizards = world_hex.black_lizards;

	for (work2 = 0; work2 < NUMBER_CHANTS; work2 ++)
	  if (strcmp (tmp_string2, chant_code [work2]) == 0)
	  {
	    work2 ++;
	    break;
	  }

	if (work2 == (NUMBER_CHANTS + 1))
	  work_order.yellow_lizards = 0;
	else
	  work_order.yellow_lizards = work2;

	strtoken (tmp_string2, tmp_string, " ");

	if (work2 == 10)
	  if (tmp_string [0] == '\0')
	    string_to_loc (tmp_string2, &work_order.red_lizards,
			   &work_order.green_lizards);
	  else {
	    strcat (tmp_string2, tmp_string);

	    string_to_loc (tmp_string2, &work_order.red_lizards,
			   &work_order.green_lizards);
	  }
	else {
	  work_order.moves [0] = string_to_direction (tmp_string2);

	  work_order.moves [1] = string_to_direction (tmp_string);
	}

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world->x &&
	    work_order.y_hex < world->y && (work_order.moves [0] != 0 ||
	    work_order.yellow_lizards == 10) && work_order.yellow_lizards > 0)
	{
	  add_order (&work_order);
	} else {
	  player_mesg (present_player, " - 'CH %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 24:		/* System Message */
      case 25:
	strcpy (tmp_string, work_string);
	work1 = 0;
	while (fgets (tmp_string2, 140, in_fptr) != '\0')
	{
	  strcpy (mesg_string [work1], tmp_string2);
	  work1 ++;
	  if (strcmp (strupr (tmp_string2), "ENDMESG\n") == 0)
	  {
	    work1 --;
	    break;
	  }
	  if (work1 == 40)
	    break;
	}

	/* send 'the other bit' to PLAYER 0 ONLY - given to the rest of the
	   players by MESSAPND.EXE later! */

	sprintf (filename, "%s%s00_%03d.%s", world->path, PLYPLY_MESG_F,
	  world->turn, world->game);

	if ((fptr = fopen (filename, "at")) == '\0')
	{
          printf ("FATAL ERROR: Unable to append to %s file.\n",
		   filename);
	  exit (EXIT_FAILURE);
	}

	fprintf (fptr, " * The Other Bit:\n\n");

	for (work2 = 0; work2 < work1; work2 ++)
	  fprintf (fptr, "   %s", mesg_string [work2]);

	/* fclose (fptr); */

	break;
      case 26:		/* Give Order */
      case 27:
	if (present_player == 0)
	{
          printf ("FATAL ERROR: Order issued when player number unknown.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = GIVE;

        strtoken (tmp_string2, tmp_string, " ");

	string_to_loc (tmp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	work_order.moves [0] = atoi (tmp_string);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world->x &&
	    work_order.y_hex < world->y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  player_mesg (present_player, " - 'GI %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 28:		/* Lob Order */
      case 29:
	if (present_player == 0)
	{
	  printf ("FATAL ERROR: Order issued when player number unknown.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = LOB;

        strtoken (tmp_string2, tmp_string, " ");

	string_to_loc (tmp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (world, work_x, work_y, &world_hex);

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = world_hex.yellow_lizards;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

        strtoken (tmp_string2, tmp_string, " ");

	work_order.moves [0] = string_to_direction (tmp_string2);

	work_order.moves [1] = string_to_direction (tmp_string);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world->x &&
	    work_order.y_hex < world->y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  player_mesg (present_player, " - 'LO %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 30:		/* Free Order */
      case 31:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = FREE;

	strtoken (tmp_string2, tmp_string, " ");

	string_to_loc (tmp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

	while (1)
	{
          strtoken (tmp_string2, tmp_string, " ");

	  work1 = atoi (tmp_string2);

	  if (work1 == 0)
	    break;

	  strsplt (tmp_string3, tmp_string2, strlen (tmp_string2) - 3);

	  for (work2 = 0; work2 < LIZARD_TYPES; work2 ++)
	    if (strcmp (tmp_string3, lizard_colours [work2]) == 0)
	      break;

	  switch (work2)
	  {
	    case 0:
	      work_order.red_lizards = work1;

	      break;
	    case 1:
	      work_order.green_lizards = work1;

	      break;
	    case 2:
	      work_order.grey_lizards = work1;

	      break;
	    case 3:
	      work_order.black_lizards = work1;

	      break;
	    case 4:
	      work_order.yellow_lizards = work1;

	      break;
	    default:
	      break;
	  }
	}

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world->x &&
	    work_order.y_hex < world->y)
	{
	  add_order (&work_order);
	} else {
	  player_mesg (present_player, " - 'FR %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 36:		/* Kill Player Order */
      case 37:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = KILL_PLAYER;

	work_order.x_hex = 0;
	work_order.y_hex = 0;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	add_order (&work_order);

	break;
      case 40:		/* Erupt Volcano Order */
      case 41:
	work_order.player = 0;
	work_order.order_type = ERUPT_VOLCANO;

	strtoken (tmp_string2, tmp_string, " ");

	string_to_loc (tmp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (world, work_x, work_y, &world_hex);

	if (work_order.x_hex >= 0 && work_order.y_hex >= 0 &&
	    work_order.x_hex < world->x && work_order.y_hex < world->y &&
	    world_hex.terrain == VOLCANO)
	{
	  add_order (&work_order);
	}

	break;
      case 42:
      case 43:  	/* Spawn Turn Order */
	sprintf (filename, "%s%s%s", world->path, WORLD_SWITCH_F, world->game);

	if ((fptr = fopen (filename, "wt")) == '\0')
	{
	  printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
	  exit (EXIT_FAILURE);
	}
	fprintf (fptr, "SPAWN\n");
	fclose (fptr);
	break;

      default:
	/* screwed order parse! */

	token_table = '\0';
	screwed [0] = '\0';

	token_table = make_token_table (token_table, tmp_string, " ");

	if (strcmp (token_table [0], "?") != 0) /* order */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [1], "?") != 0) /* hex */
	{
	  strcat (screwed, token_table [1]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [4], "?") != 0) /* red liz */
	{
	  strcat (screwed, token_table [4]);
	  strcat (screwed, "RED ");
	}

	if (strcmp (token_table [5], "?") != 0) /* grn liz */
	{
	  strcat (screwed, token_table [5]);
	  strcat (screwed, "GRN ");
	}

	if (strcmp (token_table [6], "?") != 0) /* gry liz */
	{
	  strcat (screwed, token_table [6]);
	  strcat (screwed, "GRY ");
	}

	if (strcmp (token_table [7], "?") != 0) /* yel liz */
	{
	  strcat (screwed, token_table [7]);
	  strcat (screwed, "YEL ");
	}

	if (strcmp (token_table [8], "?") != 0) /* blk liz */
	{
	  strcat (screwed, token_table [8]);
	  strcat (screwed, "BLK ");
	}

	if (strcmp (token_table [9], "?") != 0) /* extra */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [2], "?") != 0) /* m1 */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [3], "?") != 0) /* m2 */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	player_mesg (present_player, " - '%s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	  screwed);

	free_malloced_table (token_table);

	break;
#endif
	}
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */
void TextOrderConvert(world_t *world)
{
  char filename [255];
  int i;
  FILE *in_fptr;

  sprintf (filename, "%s%s.%s%03d", world->path, world->game, TEXT_ORDS_F,
	   world->turn);
  world->num_orders = 0;
  for (i = 0; i < world->num_players; i++)
    if (world->player[i]) world->player[i]->num_orders = 0;
  if ((in_fptr = fopen (filename, "r")) != NULL)
    {
      ProcessFile(in_fptr, world);
      fclose (in_fptr);
    }
  else
    {
      debug(world, "Cannot open file %s", filename);
      return;
    }
  for (i = 1; i < world->num_players; i++)
    if (world->player[i] && !world->player[i]->num_orders && world->turn > 0)
      player_mesg (world, i, " - SORRY, BUT THE MODERATOR DID NOT RECEIVE ORDERS FROM YOU FOR THIS TURN REPORT.\n"
			" - PLEASE POST YOUR ORDERS FOR THIS TURN EARLY, TO ENSURE THIS DOESN'T HAPPEN AGAIN.\n");
  
  /*  remove (filename); */ /* remove order file now we've finished with it */
}
