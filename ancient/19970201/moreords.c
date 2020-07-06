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
#include "chants.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"
#include "lizord.h"

extern char data_path [80],
            game_code [5];

extern int world_x,
	   world_y,
	   number_orders,
	   turn_number;

extern char player_code [MAX_PLAYERS + 1][6];
extern char player_name [MAX_PLAYERS + 1][37];
extern char player_clan_name [MAX_PLAYERS + 1][37];
extern char player_home_den [MAX_PLAYERS + 1];
extern char players_in_game;

extern char player_road [MAX_PLAYERS + 1][37];
extern char player_burb [MAX_PLAYERS + 1][37];
extern char player_city [MAX_PLAYERS + 1][37];
extern char player_country [MAX_PLAYERS + 1][37];
extern char player_cred [MAX_PLAYERS + 1][10];
extern char player_output_mode [MAX_PLAYERS + 1];
extern char game_due_date [20];

extern spy_t *spy_list [MAX_SPYS];
extern int number_spys;

extern unsigned int spawn_turn_switch,
		    capture_switch,
		    volcano_dormant_switch,
		    peak_active_switch,
		    world_wrap_switch,
                    victory_points_switch,
                    home_owned_worth,
                    home_victory_points,
                    give_switch;

extern order_t *order_list [MAX_ORDERS_PER_TURN];

extern char dir [7][3];

extern char lizard_colours [6][12];

extern char *message_list [MAX_RANDOM_MESSAGES];

void chant_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int number_black_lizards [NUMBER_CHANTS + 1] = { 0, 10, 25, 50, 80, 120, 150, 180, 100, 60, 130 },
      revert_percent [NUMBER_CHANTS + 1] = { 0, 0, 10, 20, 30, 40, 50, 60, 30, 25, 50 },
      success,
      chant,
      work,
      work2,
      x,
      y,
      x2,
      y2;

  hex_t world_hex;

  chant = order->yellow_lizards;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (order->black_lizards == 0 || world_hex.black_lizards == 0 ||
      world_hex.hex_owner != order->player)
  {
    fprintf (fptr, " - Chant Order at %s ignored as hex contain NO Black Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (order->black_lizards > world_hex.black_lizards)
    order->black_lizards = world_hex.black_lizards;

  if (chant == 0)
  {
    fprintf (fptr, " - Chant Order at %s ignored as you're using an unknown chant!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  fprintf (fptr, " - %d Black Lizards at %s begin chanting",
	   order->black_lizards, work_string);

  if (chant == 10)
    fprintf (fptr, "...");
  else
    if (order->moves [1] == 0)
      fprintf (fptr, " %s...", dir [order->moves [0]]);
    else
      fprintf (fptr, " %s %s...", dir [order->moves [0]], dir [order->moves [1]]);

  if (order->black_lizards < number_black_lizards [chant])
  {
    fprintf (fptr, "but their chanting is too weak and pitiful.\n");

    fclose (fptr);

    return;
  }

  fclose (fptr);

  move_coords (order->moves [0], order->x_hex, order->y_hex, &x, &y);
  move_coords (order->moves [1], x, y, &x2, &y2);

  switch (chant)
  {
    case 1: /* Eyes Chant */
      success = flying_eye_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 2: /* Reveal Chant */
      success = search_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 3: /* Shunt Chant */
      success = shunt_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 4: /* Mould Chant */
      success = mould_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 5: /* Coerce Chant */
      success = coerce_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 6: /* Transmute Chant */
      success = transmute_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 7: /* Dark Chant */
      success = dark_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 8: /* Freeze Chant */
      success = freeze_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 9: /* Summon Chant */
      success = summon_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    case 10: /* Gate Chant */
      success = gate_chant (x2, y2, order, number_black_lizards [chant]);

      break;
    default:
      open_player_mesg (order->player, &fptr);

      fprintf (fptr, "but nothing happens!\n");

      fclose (fptr);

      return;
  }

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (order->black_lizards > world_hex.black_lizards)
    order->black_lizards = world_hex.black_lizards;

  xy_to_string (order->x_hex, order->y_hex, work_string);

  open_player_mesg (order->player, &fptr);

  if (success != 0 && order->black_lizards > 0)
    if (world_hex.terrain == TEMPLE)
      if (chant == 7) /* Death Chant cast! */
      {
	fprintf (fptr, " - The Temple at %s is bathed in lightning! Your %d Black Lizards finish chanting.\n",
		 work_string, order->black_lizards);

	work2 = ((float) order->black_lizards / 100.00) * revert_percent [chant];

	if (work2 > 0)
	{
	  fprintf (fptr, " - %d Black Lizards here slowly fade to Red!\n", work2);

	  world_hex.red_lizards += work2;
	  world_hex.black_lizards -= work2;
	}
      } else
	fprintf (fptr, " - The Temple at %s glows! Your %d Black Lizards finish chanting.\n",
		 work_string, order->black_lizards);
    else {
      fprintf (fptr, " - Your %d Black Lizards at %s finish chanting.\n",
	       order->black_lizards, work_string);

      work2 = ((float) order->black_lizards / 100.00) * revert_percent [chant];

      if (work2 > 0)
      {
	fprintf (fptr, " - %d Black Lizards here slowly fade to Red!\n", work2);

	world_hex.red_lizards += work2;
	world_hex.black_lizards -= work2;
      }
    }

  put_hex (order->x_hex, order->y_hex, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	order_list [work]->order_type == CHANT)
    {
      fprintf (fptr, " - Next Chant Order at %s ignored, only one Chant Order allowed per hex, per turn.\n",
	       work_string);

      order_list [work]->order_type = INACTIVE;
    } else
      if (order_list [work] != order &&
	  order_list [work]->x_hex == order->x_hex &&
	  order_list [work]->y_hex == order->y_hex &&
	  order_list [work]->player == order->player &&
	  (order_list [work]->order_type == MOVE ||
	   order_list [work]->order_type == SPLIT ||
	   order_list [work]->order_type == SAIL))
      {
	if (order_list [work]->order_type == MOVE)
	  fprintf (fptr, " - Move %s (", work_string);
	else
	  if (order_list [work]->order_type == SPLIT)
	    fprintf (fptr, " - Split %s (", work_string);
	  else
	    fprintf (fptr, " - Sail %s (", work_string);

	if (order_list [work]->moves [1] != 0)
	  fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]], dir [order_list [work]->moves [1]]);
	else
	  fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

	fprintf (fptr, "order ignored! All Lizards in hex are EXHAUSTED after chanting!\n");

	order_list [work]->order_type = INACTIVE;
      }

  fclose (fptr);
}

void give_order (order_t *order)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work,
      spy;

  hex_t world_hex;

  if (!give_switch)
  {
    open_player_mesg (order->player, &fptr);

    xy_to_string (order->x_hex, order->y_hex, work_string);

    fprintf (fptr, " - Give Order at %s ignored as Give is not allowed in this game.\n",
	     work_string);

    fclose (fptr);

    return;
  }

  get_hex (order->x_hex, order->y_hex, &world_hex);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  spy = player_spy_here (order->x_hex, order->y_hex, order->player);

  if (world_hex.hex_owner != order->player && spy == -1)
  {
    open_player_mesg (order->player, &fptr);

    fprintf (fptr, " - Give Order at %s ignored as hex doesn't belong to you!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (lizards_in_hex (&world_hex) == 0 && world_hex.terrain != DEN &&
      world_hex.terrain != HOME_DEN && spy == -1)
  {
    open_player_mesg (order->player, &fptr);

    fprintf (fptr, " - Give Order at %s ignored as there's nothing there to give!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (strcmp (player_name [order->moves [0]], "****") == 0)
  {
    open_player_mesg (order->player, &fptr);

    fprintf (fptr, " - Give Order at %s ignored as you cannot give lizards to EXTINCT Clans!\n",
      work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.terrain == HOME_DEN && world_hex.hex_owner == order->player)
  {
    open_player_mesg (order->player, &fptr);

    fprintf (fptr, " - Give Order at %s ignored as you cannot gift Home Dens!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  xy_to_string (order->x_hex, order->y_hex, work_string);

  open_player_mesg (order->moves [0], &fptr);

  fprintf (fptr, "\n");

  fclose (fptr);

  if (world_hex.hex_owner != order->player && spy > -1)
  {
    for (work = 0; work < number_spys; work ++)
      if (spy_list [work]->x_hex == order->x_hex && spy_list [work]->y_hex ==
	  order->y_hex && spy_list [work]->player == order->player)
      {
	open_player_mesg (order->player, &fptr);

	fprintf (fptr, " - You give Spy Lizard at %s to Clan '%s'.\n",
		 work_string, player_clan_name [order->moves [0]]);

	fclose (fptr);

	open_player_mesg (order->moves [0], &fptr);

	fprintf (fptr, " - Clan '%s' gives you Spy Lizard at %s.\n",
	       player_clan_name [order->player], work_string);

	fclose (fptr);

	spy_list [work]->player = order->moves [0];
      }
  } else {
    open_player_mesg (order->player, &fptr);

    if (world_hex.terrain == DEN)
      fprintf (fptr, " - You give %s Den with %d Den Lizards at %s to Clan '%s'.\n",
	       lizard_colours [world_hex.den_lizard_type], world_hex.lizard_pop,
	       work_string, player_clan_name [order->moves [0]]);

    if (lizards_in_hex (&world_hex) > 0)
      fprintf (fptr, " - You give %s Lizards at %s to Clan '%s'.\n",
	       show_lizards (world_hex.red_lizards,
	       world_hex.green_lizards,
	       world_hex.grey_lizards,
	       world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string),
	       work_string, player_clan_name [order->moves [0]]);

    fclose (fptr);

    open_player_mesg (order->moves [0], &fptr);

    if (world_hex.terrain == DEN)
      fprintf (fptr, " - Clan '%s' gives you %s Den with %d Den Lizards at %s.\n",
	       player_clan_name [order->player],
	       lizard_colours [world_hex.den_lizard_type],
	       world_hex.lizard_pop, work_string);

    if (lizards_in_hex (&world_hex) > 0)
      fprintf (fptr, " - Clan '%s' gives you %s Lizards at %s.\n",
	       player_clan_name [order->player],
	       show_lizards (world_hex.red_lizards,
	       world_hex.green_lizards,
	       world_hex.grey_lizards,
	       world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string),
	       work_string);

    fclose (fptr);

    if (spy > -1)
      for (work = 0; work < number_spys; work ++)
	if (spy_list [work]->x_hex == order->x_hex && spy_list [work]->y_hex ==
	    order->y_hex && spy_list [work]->player == order->player)
	{
	  open_player_mesg (order->player, &fptr);

	  fprintf (fptr, " - You give Spy Lizard at %s to Clan '%s'.\n",
		   work_string, player_clan_name [order->moves [0]]);

	  fclose (fptr);

	  open_player_mesg (order->moves [0], &fptr);

	  fprintf (fptr, " - Clan '%s' gives you Spy Lizard at %s.\n",
		   player_clan_name [order->player], work_string);

	  fclose (fptr);

	  spy_list [work]->player = order->moves [0];
	}

    world_hex.hex_owner = order->moves [0];

    put_hex (order->x_hex, order->y_hex, &world_hex);
  }

  open_player_mesg (order->player, &fptr);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->player == order->player &&
	order_list [work]->order_type == GIVE)
    {
      fprintf (fptr, " - Next Give Order is ignored, only one Give Order allowed per hex, per turn.\n");

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void free_order (order_t *order)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work,
      freed,
      free_colour,
      red,
      green,
      grey,
      black,
      yellow;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if ((lizards_in_hex (&world_hex) == 0) ||
      (lizards_in_hex (&world_hex) > 0 &&
       world_hex.hex_owner != order->player))
  {
    fprintf (fptr, " - Free Order at %s ignored as there are none of your lizards here!\n", work_string);

    fclose (fptr);

    return;
  }

  if (order->red_lizards > world_hex.red_lizards)
  {
    order->red_lizards = world_hex.red_lizards;

    if ((order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards) == 0)
    {
      fprintf (fptr, " - Free Order at %s ignored. No Red Lizards in hex!\n",
	work_string);

      fclose (fptr);

      return;
    }
  }

  if (order->green_lizards > world_hex.green_lizards)
  {
    order->green_lizards = world_hex.green_lizards;

    if ((order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards) == 0)
    {
      fprintf (fptr, " - Free Order at %s ignored. No Green Lizards in hex!\n",
	work_string);

      fclose (fptr);

      return;
    }
  }

  if (order->grey_lizards > world_hex.grey_lizards)
  {
    order->grey_lizards = world_hex.grey_lizards;

    if ((order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards) == 0)
    {
      fprintf (fptr, " - Free Order at %s ignored. No Grey Lizards in hex!\n",
	work_string);

      fclose (fptr);

      return;
    }
  }

  if (order->black_lizards > world_hex.black_lizards)
  {
    order->black_lizards = world_hex.black_lizards;

    if ((order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards) == 0)
    {
      fprintf (fptr, " - Free Order at %s ignored. No Black Lizards in hex!\n",
	work_string);

      fclose (fptr);

      return;
    }
  }

  if (order->yellow_lizards > world_hex.yellow_lizards)
  {
    order->yellow_lizards = world_hex.yellow_lizards;

    if ((order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards) == 0)
    {
      fprintf (fptr, " - Free Order at %s ignored. No Yellow Lizards in hex!\n",
	work_string);

      fclose (fptr);

      return;
    }
  }

  if ((order->red_lizards + order->green_lizards + order->grey_lizards +
       order->black_lizards + order->yellow_lizards) == 0)
  {
    red = world_hex.red_lizards;
    green = world_hex.green_lizards;
    grey = world_hex.grey_lizards;
    black = world_hex.black_lizards;
    yellow = world_hex.yellow_lizards;
  } else {
    red = order->red_lizards;
    green = order->green_lizards;
    grey = order->grey_lizards;
    black = order->black_lizards;
    yellow = order->yellow_lizards;
  }

  freed = red + green + grey + black + yellow;

  if (world_hex.terrain == DEN && world_hex.den_lizard_type > 0)
  {
    switch (world_hex.den_lizard_type)
    {
      case RED_LIZARD:
	if (red != freed)
	{
	  fprintf (fptr, " - Free Order at %s ignored as %s Den here will only accept %s lizards!\n", work_string,
			 lizard_colours [world_hex.den_lizard_type], lizard_colours [world_hex.den_lizard_type]);

	  fclose (fptr);

	  return;
	}

	break;
      case GREEN_LIZARD:
	if (green != freed)
	{
	  fprintf (fptr, " - Free Order at %s ignored as %s Den here will only accept %s lizards!\n", work_string,
			 lizard_colours [world_hex.den_lizard_type], lizard_colours [world_hex.den_lizard_type]);

	  fclose (fptr);

	  return;
	}

      	break;
      case GREY_LIZARD:
	if (grey != freed)
	{
	  fprintf (fptr, " - Free Order at %s ignored as %s Den here will only accept %s lizards!\n", work_string,
			 lizard_colours [world_hex.den_lizard_type], lizard_colours [world_hex.den_lizard_type]);

	  fclose (fptr);

	  return;
	}

      	break;
      case BLACK_LIZARD:
	if (black != freed)
	{
	  fprintf (fptr, " - Free Order at %s ignored as %s Den here will only accept %s lizards!\n", work_string,
			 lizard_colours [world_hex.den_lizard_type], lizard_colours [world_hex.den_lizard_type]);

	  fclose (fptr);

	  return;
	}

      	break;
      case YELLOW_LIZARD:
	if (yellow != freed)
	{
	  fprintf (fptr, " - Free Order at %s ignored as %s Den here will only accept %s lizards!\n", work_string,
			 lizard_colours [world_hex.den_lizard_type], lizard_colours [world_hex.den_lizard_type]);

	  fclose (fptr);

	  return;
	}

      	break;
    }
  }

  if (freed > 0)
  {
    fprintf (fptr, " - You free %s Lizards at %s. ",
	     show_lizards (red, green, grey, black, yellow,
	     temp_string), work_string);

    if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN)
    {
      free_colour = 0;

      if (red == freed)
	free_colour = RED_LIZARD;

      if (green == freed)
	free_colour = GREEN_LIZARD;

      if (grey == freed)
	free_colour = GREY_LIZARD;

      if (black == freed)
	free_colour = BLACK_LIZARD;

      if (yellow == freed)
	free_colour = YELLOW_LIZARD;

      if (world_hex.den_lizard_type == 0)
	fprintf (fptr, " Lizards enter the un-settled Den here.\n");
      else
	fprintf (fptr, " Lizards join the Den population here.\n");

      world_hex.lizard_pop += red;
      world_hex.lizard_pop += green;
      world_hex.lizard_pop += grey;
      world_hex.lizard_pop += black;
      world_hex.lizard_pop += yellow;

      world_hex.red_lizards -= red;
      world_hex.green_lizards -= green;
      world_hex.grey_lizards -= grey;
      world_hex.black_lizards -= black;
      world_hex.yellow_lizards -= yellow;

      if (world_hex.lizard_pop >= MINIMUM_POP_LEVEL &&
	  world_hex.den_lizard_type == 0)
	if (free_colour != 0)
	{
	  world_hex.den_lizard_type = free_colour;

	  fprintf (fptr, " - Den is settled by freed Lizards and becomes a %s Den!\n",
		   lizard_colours [world_hex.den_lizard_type]);
	} else {
	  world_hex.den_lizard_type = random (5) + 1;

	  fprintf (fptr, " - Den is settled by freed Lizards and becomes a %s Den!\n",
		   lizard_colours [world_hex.den_lizard_type]);
	}
    } else {
      fprintf (fptr, "Lizards return to the wild.\n");

      world_hex.red_lizards -= red;
      world_hex.green_lizards -= green;
      world_hex.grey_lizards -= grey;
      world_hex.black_lizards -= black;
      world_hex.yellow_lizards -= yellow;
    }
  }

  put_hex (order->x_hex, order->y_hex, &world_hex);

  /* whip through orders looking for any moves from hex and adjusting the
    new numbers accordingly (to compensate for the freed lizards) */

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	order_list [work]->order_type == MOVE)
    {
      order_list [work]->red_lizards = world_hex.red_lizards;
      order_list [work]->green_lizards = world_hex.green_lizards;
      order_list [work]->grey_lizards = world_hex.grey_lizards;
      order_list [work]->black_lizards = world_hex.black_lizards;
      order_list [work]->yellow_lizards = world_hex.yellow_lizards;
    }

  fclose (fptr);
}

void lob_order (order_t *order)
{
  FILE *fptr;

  char work_string [80],
       work_string2 [80];

  int work,
      work2,
      x,
      y,
      x2,
      y2;

  hex_t world_hex,
	new_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (order->yellow_lizards == 0 || world_hex.yellow_lizards == 0 ||
      world_hex.hex_owner != order->player)
  {
    fprintf (fptr, " - Lob Order at %s ignored as hex contain NO Yellow Lizards!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  move_coords (order->moves [0], order->x_hex, order->y_hex, &x, &y);
  move_coords (order->moves [1], x, y, &x2, &y2);

  get_hex (x2, y2, &new_hex);

  if (new_hex.hex_owner == order->player &&
      lizards_in_hex (&new_hex) > 0)
  {
    fprintf (fptr, " - Lob Order at %s ignored as Yellow Lizards will not attack Lizards on their side!\n",
	     work_string);

    fclose (fptr);

    return;
  }

  xy_to_string (x2, y2, work_string2);

  fprintf (fptr, " - %d Yellow Lizards at %s hurl stones and sticks ",
	   order->yellow_lizards, work_string);

  if (order->moves [1] == 0)
    fprintf (fptr, "%s ", dir [order->moves [0]]);
  else
    fprintf (fptr, "%s %s ", dir [order->moves [0]], dir [order->moves [1]]);

  fprintf (fptr, "at %s.\n", work_string2);

  switch (new_hex.terrain)
  {
    case DEN:
      fprintf (fptr, " - They clatter off the Den there.\n");

      fclose (fptr);

      if (lizards_in_hex (&new_hex) > 0 && new_hex.hex_owner > 0)
      {
	open_player_mesg (new_hex.hex_owner, &fptr);

	fprintf (fptr, "\n - Lizards at %s showered with sticks and stones from '%s' Clan Lizards at %s.\n"
		 " - Den here protects your Lizards from the attack.\n",
		 work_string2, player_clan_name [order->player], work_string);

	fclose (fptr);
      }

      return;
    case HOME_DEN:
      fprintf (fptr, " They clatter off the Home Den there.\n");

      fclose (fptr);

      if (lizards_in_hex (&new_hex) > 0 && new_hex.hex_owner > 0)
      {
	open_player_mesg (new_hex.hex_owner, &fptr);

	fprintf (fptr, "\n - Lizards at %s showered with sticks and stones from '%s' Clan Lizards at %s.\n"
		 " - Home Den here protects your Lizards from the attack.\n",
		 work_string2, player_clan_name [order->player], work_string);

	fclose (fptr);
      }

      return;
    case TEMPLE:
      fprintf (fptr, "  - They clatter off the Temple there.\n");

      fclose (fptr);

      if (lizards_in_hex (&new_hex) > 0 && new_hex.hex_owner > 0)
      {
	open_player_mesg (new_hex.hex_owner, &fptr);

	fprintf (fptr, "\n - Lizards at %s showered with sticks and stones from '%s' Clan Lizards at %s.\n"
		 " - Temple here protects your Lizards from the attack.\n",
		 work_string2, player_clan_name [order->player], work_string);

	fclose (fptr);
      }

      return;
    case RUIN:
      fprintf (fptr, " - They clatter off the Ruin here.\n");

      fclose (fptr);

      if (lizards_in_hex (&new_hex) > 0 && new_hex.hex_owner > 0)
      {
	open_player_mesg (new_hex.hex_owner, &fptr);

	fprintf (fptr, "\n - Lizards at %s showered with sticks and stones from '%s' Clan Lizards at %s.\n"
		 " - Ruin here protects your Lizards from the attack.\n",
		 work_string2, player_clan_name [order->player], work_string);

	fclose (fptr);
      }

      return;
    default:
      break;
  }

  for (work = 0, work2 = 0; work < world_hex.yellow_lizards &&
       work2 < lizards_in_hex (&new_hex); work ++)
    if (random (100) > LOB_ATTACK)
      work2 ++;

  if (work2 == 0)
  {
    fprintf (fptr, " - But they don't hit any Lizards.\n");

    fclose (fptr);

    if (lizards_in_hex (&new_hex) > 0)
    {
      open_player_mesg (new_hex.hex_owner, &fptr);

      fprintf (fptr, "\n - Lizards at %s showered with sticks and stones from '%s' Clan Lizards at %s. Nobody is hit.\n",
     	     work_string2, player_clan_name [order->player], work_string);

      fclose (fptr);
    }
  } else {
    fprintf (fptr, " - They kill %d Lizards there!\n", work2);

    fclose (fptr);

    kill_lizards_xy (&new_hex, work2);

    put_hex (x2, y2, &new_hex);

    /* adjust any orders for this hex, downward to compensate for the loss
       of lizards */

    for (work = 0; work < number_orders; work ++)
      if (order_list [work]->x_hex == x2 &&
          order_list [work]->y_hex == y2 &&
	  order_list [work]->player == new_hex.hex_owner)
    {
      if (order_list [work]->order_type == CHANT)
      {
	if (order_list [work]->black_lizards > new_hex.black_lizards)
	  order_list [work]->black_lizards = new_hex.black_lizards;
      } else {
	/* adjust order lizards downward to compensate for loss in hex */

	if (order_list [work]->red_lizards > new_hex.red_lizards)
	  order_list [work]->red_lizards = new_hex.red_lizards;

	if (order_list [work]->green_lizards > new_hex.green_lizards)
	  order_list [work]->green_lizards = new_hex.green_lizards;

	if (order_list [work]->grey_lizards > new_hex.grey_lizards)
	  order_list [work]->grey_lizards = new_hex.grey_lizards;

	if (order_list [work]->black_lizards > new_hex.black_lizards)
	  order_list [work]->black_lizards = new_hex.black_lizards;

	if (order_list [work]->yellow_lizards > new_hex.yellow_lizards)
	  order_list [work]->yellow_lizards = new_hex.yellow_lizards;
      }
    }

    open_player_mesg (new_hex.hex_owner, &fptr);

    fprintf (fptr, "\n - Lizards at %s showered with sticks and stones from '%s' Clan Lizards at %s. %d Lizards are killed!\n",
    	     work_string2, player_clan_name [order->player], work_string, work2);

    if (lizards_in_hex (&new_hex) == 0)
    {
      fprintf (fptr, " - NO Lizards survive attack at %s!\n", work_string2);

      fclose (fptr);
    } else {
      fprintf (fptr, " - %s Lizards survive attack at %s.\n",
	       show_lizards (new_hex.red_lizards, new_hex.green_lizards,
	       new_hex.grey_lizards, new_hex.black_lizards,
	       new_hex.yellow_lizards, work_string), work_string2);

      fclose (fptr);

      hex_attacked_or_moved_into (x2, y2, &new_hex);
    }
  }

  open_player_mesg (order->player, &fptr);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	order_list [work]->order_type == LOB)
    {
      xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
	work_string);

      fprintf (fptr, " - Next Lob Order at %s ignored, only allowed one Lob Order per hex, per turn.\n",
	       work_string);

      order_list [work]->order_type = INACTIVE;
    } else
      if (order_list [work] != order &&
	  order_list [work]->x_hex == order->x_hex &&
	  order_list [work]->y_hex == order->y_hex &&
	  order_list [work]->player == order->player &&
	  (order_list [work]->order_type == MOVE ||
	   order_list [work]->order_type == SPLIT ||
	   order_list [work]->order_type == SAIL))
      {
	xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
	  work_string);

	if (order_list [work]->order_type == MOVE)
	  fprintf (fptr, " - Move %s (", work_string);
	else
	  if (order_list [work]->order_type == SPLIT)
	    fprintf (fptr, " - Split %s (", work_string);
	  else
	    fprintf (fptr, " - Sail %s (", work_string);

	if (order_list [work]->moves [1] != 0)
	  fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]], dir [order_list [work]->moves [1]]);
	else
	  fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

	fprintf (fptr, "order ignored! Lizards recovering from lob attack.\n");

	order_list [work]->order_type = INACTIVE;
      }

  fclose (fptr);
}

void kill_player_order (order_t *order)
{
  int x,
      y;

  hex_t world_hex;

  strcpy (player_name [order->player], "****");

  save_player_list (game_due_date);

  for (x = 0; x < world_x; x ++)
    for (y = 0; y < world_y; y ++)
    {
      get_hex (x, y, &world_hex);

      if (world_hex.hex_owner == order->player)
      {
	if (world_hex.terrain == DEN)
	{
	  world_hex.hex_owner = 0;
	  world_hex.turns_hungry = 0;
	  world_hex.combat_points = 0;

	  put_hex (x, y, &world_hex);
	} else {
	  if (world_hex.terrain == HOME_DEN)
	  {
            world_hex.hex_owner = 0;
	    world_hex.turns_undisturbed = 4;

	    put_hex (x, y, &world_hex);
	  }
	}
      }
    }
}
