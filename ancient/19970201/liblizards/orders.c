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

#include "lizhex.h"
#include "lizmesg.h"
#include "lizmisc.h"
#include "lizord.h"

extern char player_code [MAX_PLAYERS + 1][6];
extern char player_name [MAX_PLAYERS + 1][37];
extern char player_clan_name [MAX_PLAYERS + 1][37];
extern char player_home_den [MAX_PLAYERS + 1];
extern char players_in_game;

extern order_t *order_list [MAX_ORDERS_PER_TURN];

extern spy_t *spy_list [MAX_SPYS];
extern int number_spys;

extern char dir [7][3];

extern char lizard_colours [6][12];

extern char liz_cp_desc [MAX_COMBAT_POINTS][10];

extern char liz_hunger_desc [MAX_HUNGER_POINTS][10];

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

void move_order (order_t *order)
{
  FILE *fptr,
       *fptr2;

  char work_string [80],
       temp_string [80];

  int work,
      work2,
      work3,
      work4,
      red,
      green,
      grey,
      black,
      yellow,
      new_red,
      new_green,
      new_grey,
      new_black,
      new_yellow,
      captured_red,
      captured_green,
      captured_grey,
      captured_black,
      captured_yellow,
      capture_level,
      enemy_captured_red,
      enemy_captured_green,
      enemy_captured_grey,
      enemy_captured_black,
      enemy_captured_yellow,
      enemy_capture_level,
      num_captured,
      x,
      y,
      x2,
      y2,
      combat_experience = LZ_FALSE;

  order_t old_order;

  hex_t old_hex,
	new_hex,
	temp_new_hex,
	flee_hex;

  old_order = *order;

  get_hex (order->x_hex, order->y_hex, &old_hex);

  if (order->red_lizards > old_hex.red_lizards)
  {
    order->red_lizards = old_hex.red_lizards;

    if (order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards == 0 &&
	order->order_type == SPLIT)
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      fprintf (fptr, " - Split %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Red Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  }

  if (order->green_lizards > old_hex.green_lizards)
  {
    order->green_lizards = old_hex.green_lizards;

    if (order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards == 0 &&
	order->order_type == SPLIT)
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      fprintf (fptr, " - Split %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Green Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  }

  if (order->grey_lizards > old_hex.grey_lizards)
  {
    order->grey_lizards = old_hex.grey_lizards;

    if (order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards == 0 &&
	order->order_type == SPLIT)
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      fprintf (fptr, " - Split %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Grey Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  }

  if (order->black_lizards > old_hex.black_lizards)
  {
    order->black_lizards = old_hex.black_lizards;

    if (order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards == 0 &&
	order->order_type == SPLIT)
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      fprintf (fptr, " - Split %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Black Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  }

  if (order->yellow_lizards > old_hex.yellow_lizards)
  {
    order->yellow_lizards = old_hex.yellow_lizards;

    if (order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards == 0 &&
	order->order_type == SPLIT)
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      fprintf (fptr, " - Split %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Yellow Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  }

  red = order->red_lizards;
  green = order->green_lizards;
  grey = order->grey_lizards;
  black = order->black_lizards;
  yellow = order->yellow_lizards;

  if (red < 0) red = 0;
  if (green < 0) green = 0;
  if (grey < 0) grey = 0;
  if (black < 0) black = 0;
  if (yellow < 0) yellow = 0;

  if (red + green + grey + black + yellow == 0)
  {
    open_player_mesg (order->player, &fptr);

    xy_to_string (order->x_hex, order->y_hex, work_string);

    if (order->order_type == SPLIT)
      fprintf (fptr, " - Split %s (", work_string);
    else
      if (order->order_type == MOVE)
	fprintf (fptr, " - Move %s (", work_string);
      else
	fprintf (fptr, " - Sail %s (", work_string);

    if (order->moves [1] != 0)
      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
    else
      fprintf (fptr, "%s) ", dir [order->moves [0]]);

    fprintf (fptr, "order ignored. NO Lizards in hex.\n");

    fclose (fptr);

    return;
  }

  if (order->order_type == SPLIT)
  {
    if ((red + green + grey + black + yellow > 1 &&
	 lizards_in_hex (&old_hex) == 0) ||
	(red + green + grey + black + yellow > 0 &&
	 lizards_in_hex (&old_hex) > 0 &&
	 old_hex.hex_owner != order->player))
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      fprintf (fptr, " - Split %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  } else {
    if ((red + green + grey + black + yellow > 0 &&
	 lizards_in_hex (&old_hex) == 0) ||
	(red + green + grey + black + yellow > 0 &&
	 lizards_in_hex (&old_hex) > 0 &&
	 old_hex.hex_owner != order->player))
    {
      open_player_mesg (order->player, &fptr);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      if (order->order_type == MOVE)
	fprintf (fptr, " - Move %s (", work_string);
      else
	fprintf (fptr, " - Sail %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. NO Lizards in hex.\n");

      fclose (fptr);

      return;
    }
  }

  /* Normal Move Order */

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, work_string);

  if (order->order_type == SAIL && old_hex.raft_here == 0)
  {
    fprintf (fptr, " - Sail %s (", work_string);

    if (order->moves [1] != 0)
      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
    else
      fprintf (fptr, "%s) ", dir [order->moves [0]]);

    fprintf (fptr, "order ignored. There is no raft here.\n");

    fclose (fptr);

    return;
  }

  if (old_hex.lizards_immobile > 0)
  {
    if (order->order_type == MOVE)
      fprintf (fptr, " - Move %s (", work_string);
    else
      if (order->order_type == SPLIT)
	fprintf (fptr, " - Split %s (", work_string);
      else
	fprintf (fptr, " - Sail %s (", work_string);

    if (order->moves [1] != 0)
      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
    else
      fprintf (fptr, "%s) ", dir [order->moves [0]]);

    if (old_hex.lizards_immobile > 2)
      fprintf (fptr, "order ignored. Lizards are FROZEN this turn.\n");
    else
      fprintf (fptr, "order ignored. Lizards are MIRED in Swamp this turn.\n");

    fclose (fptr);

    return;
  }

  for (work = 0; work < 2; work ++)
    if (order->moves [work] > 0)
    {
      move_coords (order->moves [work], order->x_hex, order->y_hex,
		   &x, &y);

      xy_to_string (order->x_hex, order->y_hex, work_string);

      if (x == order->x_hex && y == order->y_hex)
      {
	if (order->order_type == SAIL)
	  fprintf (fptr, " - Lizards at %s try to sail off edge of World.\n", work_string,
		   dir [order->moves [work]]);
	else
	  fprintf (fptr, " - Lizards at %s try to move off edge of World.\n", work_string,
		   dir [order->moves [work]]);

	fclose (fptr);

	return;
      }

      if (order->order_type == SAIL)
	fprintf (fptr, " - %s Lizards at %s sail %s (to ",
		 show_lizards (red, green, grey, black, yellow, temp_string),
		 work_string, dir [order->moves [work]]);
      else
	if (order->order_type == SPLIT && work == 0)
	  fprintf (fptr, " - %s Lizards split at %s and move %s (to ",
		   show_lizards (red, green, grey, black, yellow, temp_string),
		   work_string, dir [order->moves [work]]);
	else
	  fprintf (fptr, " - %s Lizards at %s move %s (to ",
		   show_lizards (red, green, grey, black, yellow, temp_string),
		   work_string, dir [order->moves [work]]);

      xy_to_string (x, y, work_string);

      fprintf (fptr, "%s).\n", work_string);

      get_hex (x, y, &new_hex);

      if ((lizards_in_hex (&new_hex) +
	  (red + green + grey + black + yellow)) >
	  MAX_LIZARDS_IN_HEX && new_hex.hex_owner ==
	  order->player)
      {
	fprintf (fptr, " - But %s would contain more that %d Lizards. ",
		 work_string, MAX_LIZARDS_IN_HEX);

	if (order->order_type == SAIL)
	  fprintf (fptr, " Sail order ignored.\n");
	else
	  if (order->order_type == MOVE)
	    fprintf (fptr, " Move order ignored.\n");
	  else
	    fprintf (fptr, " Split order ignored.\n");

	fclose (fptr);

	return;
      }

      if (order->order_type == SAIL && new_hex.bridge_here != 0)
      {
	fprintf (fptr, " - Lizards cannot sail under bridges (they're not high enough).\n");

	fclose (fptr);

	return;
      }

      if (order->order_type == SAIL && new_hex.terrain != WATER &&
	  new_hex.terrain != WHIRLPOOL && new_hex.terrain != CURSED)
      {
	fprintf (fptr, " - CRUNCH! Lizards run the raft aground and it sinks.\n");

	old_hex.raft_here = 0;
      }

      if (new_hex.hex_owner != order->player)
	if (lizards_in_hex (&new_hex) > 0)
	{
	  xy_to_string (x, y, work_string);

	  if (new_hex.terrain == DEN || new_hex.terrain == HOME_DEN)
	  {
	    if (new_hex.hex_owner == 0)
	      fprintf (fptr, " - Lizards attack %d militia Lizards in free Den at %s.\n",
		lizards_in_hex (&new_hex), work_string);
	    else {
	      if (new_hex.terrain == HOME_DEN)
		fprintf (fptr, " - Lizards attack %d Lizards in Home Den of '%s' Clan at %s.\n",
			 lizards_in_hex (&new_hex), player_clan_name [new_hex.hex_owner],
			 work_string);
	      else
		fprintf (fptr, " - Lizards attack %d Lizards in Den of '%s' Clan at %s.\n",
			 lizards_in_hex (&new_hex), player_clan_name [new_hex.hex_owner],
			 work_string);
	    }
	  } else {
	    fprintf (fptr, " - Lizards attack %d '%s' Clan Lizards at %s.\n",
		     lizards_in_hex (&new_hex), player_clan_name [new_hex.hex_owner],
		     work_string);
	  }

	  if (new_hex.hex_owner > 0)
	  {
	    open_player_mesg (new_hex.hex_owner, &fptr2);

	    if (new_hex.terrain == DEN)
	      fprintf (fptr2, "\n - Den at %s attacked by %d '%s' Clan Lizards.\n",
		       work_string, red + green + grey + black + yellow,
		       player_clan_name [order->player]);
	    else
	      if (new_hex.terrain == HOME_DEN)
		fprintf (fptr2, "\n - Home Den at %s attacked by %d '%s' Clan Lizards.\n",
			 work_string, red + green + grey + black + yellow,
			 player_clan_name [order->player]);
	      else
		fprintf (fptr2, "\n - %d Lizards at %s attacked by %d '%s' Clan Lizards.\n",
			 lizards_in_hex (&new_hex), work_string, red + green +
			 grey + black + yellow, player_clan_name [order->player]);

	    if (new_hex.lizards_immobile > 0)
	      if (new_hex.lizards_immobile > 2)
		fprintf (fptr2, " - Lizards at %s are FROZEN and cannot flee from combat.\n",
			 work_string);
	      else
		fprintf (fptr2, " - Lizards at %s are MIRED and cannot flee from combat.\n",
			 work_string);

	    fclose (fptr2);
	  }

	  fclose (fptr);

	  hex_attacked_or_moved_into (x, y, &new_hex);

	  temp_new_hex = new_hex; /* for calculating capture! */

	  put_hex (x, y, &new_hex);
	  put_hex (order->x_hex, order->y_hex, &old_hex);

	  if (new_hex.lizards_immobile > 0)
	    work2 = combat (red, green, grey, black, yellow, &new_red, &new_green,
			    &new_grey, &new_black, &new_yellow, order->x_hex,
			    order->y_hex, x, y, 0);
	  else
	    work2 = combat (red, green, grey, black, yellow, &new_red, &new_green,
			    &new_grey, &new_black, &new_yellow, order->x_hex,
			    order->y_hex, x, y, 1);

	  get_hex (order->x_hex, order->y_hex, &old_hex);
	  get_hex (x, y, &new_hex);

	  /* calculate YOUR captured lizards */

	  capture_level = MIN_CAPTURE_PERCENT +
	    (old_hex.combat_points * CP_CAPTURE_MOD_LEVEL);

	  captured_red = red - new_red;
	  captured_green = green - new_green;
	  captured_grey = grey - new_grey;
	  captured_black = black - new_black;
	  captured_yellow = yellow - new_yellow;

	  /* calculate ENEMY captured lizards */

	  enemy_capture_level = MIN_CAPTURE_PERCENT +
	    (new_hex.combat_points * CP_CAPTURE_MOD_LEVEL);

	  enemy_captured_red = temp_new_hex.red_lizards - new_hex.red_lizards;
	  enemy_captured_green = temp_new_hex.green_lizards - new_hex.green_lizards;
	  enemy_captured_grey = temp_new_hex.grey_lizards - new_hex.grey_lizards;
	  enemy_captured_black = temp_new_hex.black_lizards - new_hex.black_lizards;
	  enemy_captured_yellow = temp_new_hex.yellow_lizards - new_hex.yellow_lizards;

	  /* update the OLD hex */

	  old_hex.red_lizards -= (red - new_red);
	  old_hex.green_lizards -= (green - new_green);
	  old_hex.grey_lizards -= (grey - new_grey);
	  old_hex.black_lizards -= (black - new_black);
	  old_hex.yellow_lizards -= (yellow - new_yellow);

	  red = new_red;
	  green = new_green;
	  grey = new_grey;
	  black = new_black;
	  yellow = new_yellow;

	  open_player_mesg (order->player, &fptr);

	  if (new_hex.hex_owner > 0)
	    open_player_mesg (new_hex.hex_owner, &fptr2);
	  else
	    fptr2 = NULL;

	  switch (work2)
	  {
	    case -1:
	      /* Attackers flee! */

	      xy_to_string (order->x_hex, order->y_hex, work_string);

	      fprintf (fptr, " - Your Lizards flee from combat and remain at %s.\n",
		       work_string);

	      /* ENEMY capture any killed lizards but only if ENEMY are NOT
		 in a Den or Home Den */

	      if (new_hex.terrain != DEN && new_hex.terrain != HOME_DEN)
	      {
		num_captured = ((float) (captured_red + captured_green +
		  captured_grey + captured_black + captured_yellow) / 100.00) *
		  (float) enemy_capture_level;

		capture_lizards (&new_hex, num_captured, &captured_red,
		  &captured_green, &captured_grey, &captured_black,
		  &captured_yellow, &new_red, &new_green, &new_grey, &new_black,
		  &new_yellow);
	      } else {
		num_captured = 0;

		new_red = new_green = new_grey = new_black = new_yellow = 0;
	      }

	      if (fptr2 != NULL)
	      {
		fprintf (fptr2, " - Your Lizards repelled attack. ");

		inc_combat_points (&new_hex);

		fprintf (fptr2, "They are now %s warriors.\n",
			 liz_cp_desc [new_hex.combat_points]);

		if (new_red + new_green + new_grey + new_black + new_yellow > 0)
		  fprintf (fptr2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
		    show_lizards (new_red, new_green, new_grey, new_black,
		      new_yellow, temp_string));

		if (new_hex.terrain == DEN)
		{
		  xy_to_string (x, y, work_string);

		  fprintf (fptr2, " - %s Den at %s is still yours.\n",
			   lizard_colours [new_hex.den_lizard_type],
			   work_string);
		} else
		  if (new_hex.terrain == HOME_DEN)
		  {
		    xy_to_string (x, y, work_string);

		    fprintf (fptr2, " - %s Home Den at %s is still yours.\n",
			     lizard_colours [new_hex.den_lizard_type],
			     work_string);
		  }

		fclose (fptr2);
	      }

	      xy_to_string (x, y, work_string);

	      fclose (fptr);

	      put_hex (x, y, &new_hex);
	      put_hex (order->x_hex, order->y_hex, &old_hex);

	      return;
	    case 1:
	      /* Defenders flee! */

	      xy_to_string (x, y, work_string);

	      work2 = random (6) + 1;

	      work4 = -1;

	      for (work3 = 0; work3 < 6 + 1; work2 ++, work3 ++)
	      {
		if (work2 == 7)
		  work2 = 1;

		move_coords (work2, x, y, &x2, &y2);

		get_hex (x2, y2, &flee_hex);

		if (lizards_in_hex (&flee_hex) == 0 &&
		    flee_hex.terrain != DEN && flee_hex.terrain != HOME_DEN)
		{
		  work4 = 0;

		  break;
		}
	      }

	      if (work4 == -1)
	      {
		if (fptr2 != NULL)
		{
		  fprintf (fptr2, " - Your Lizards cannot flee. They are surrounded by other bands.\n");

		  fclose (fptr2);
		}

		fprintf (fptr, " - Enemy Lizards cannot flee. Your Lizards ATTACK.\n");

		fclose (fptr);

		put_hex (order->x_hex, order->y_hex, &old_hex);
		put_hex (x, y, &new_hex);

		temp_new_hex = new_hex;

		work2 = combat (red, green, grey, black, yellow, &new_red,
				&new_green, &new_grey, &new_black, &new_yellow,
				order->x_hex, order->y_hex, x, y, 0);

		get_hex (order->x_hex, order->y_hex, &old_hex);
		get_hex (x, y, &new_hex);

		/* calculate YOUR captured lizards */

		capture_level = MIN_CAPTURE_PERCENT +
		  (old_hex.combat_points * CP_CAPTURE_MOD_LEVEL);

		captured_red = red - new_red;
		captured_green = green - new_green;
		captured_grey = grey - new_grey;
		captured_black = black - new_black;
		captured_yellow = yellow - new_yellow;

		/* calculate ENEMY captured lizards */

		enemy_capture_level = MIN_CAPTURE_PERCENT +
		  (new_hex.combat_points * CP_CAPTURE_MOD_LEVEL);

		enemy_captured_red = temp_new_hex.red_lizards - new_hex.red_lizards;
		enemy_captured_green = temp_new_hex.green_lizards - new_hex.green_lizards;
		enemy_captured_grey = temp_new_hex.grey_lizards - new_hex.grey_lizards;
		enemy_captured_black = temp_new_hex.black_lizards - new_hex.black_lizards;
		enemy_captured_yellow = temp_new_hex.yellow_lizards - new_hex.yellow_lizards;

		/* update OLD hex */

		old_hex.red_lizards -=
			(red - new_red);
		old_hex.green_lizards -=
			(green - new_green);
		old_hex.grey_lizards -=
			(grey - new_grey);
		old_hex.black_lizards -=
			(black - new_black);
		old_hex.yellow_lizards -=
			(yellow - new_yellow);

		red = new_red;
		green = new_green;
		grey = new_grey;
		black = new_black;
		yellow = new_yellow;

		open_player_mesg (order->player, &fptr);

		if (new_hex.hex_owner > 0)
		  open_player_mesg (new_hex.hex_owner, &fptr2);
		else
		  fptr2 = NULL;

		switch (work2)
		{
		  case -1:
		    /* Attackers flee! */

		    xy_to_string (order->x_hex, order->y_hex, work_string);

		    fprintf (fptr, " - Your Lizards flee from combat and remain at %s.\n",
				 work_string);

		    /* ENEMY capture any killed lizards but only if ENEMY are NOT
		      in a Den or Home Den */

		    if (new_hex.terrain != DEN && new_hex.terrain != HOME_DEN)
		    {
		      num_captured = ((float) (captured_red + captured_green +
			captured_grey + captured_black + captured_yellow) / 100.00) *
			(float) enemy_capture_level;

		      capture_lizards (&new_hex, num_captured, &captured_red,
			&captured_green, &captured_grey, &captured_black,
			&captured_yellow, &new_red, &new_green, &new_grey, &new_black,
			&new_yellow);
		    } else {
		      num_captured = 0;

		      new_red = new_green = new_grey = new_black = new_yellow = 0;
		    }

		    if (fptr2 != NULL)
		    {
		      fprintf (fptr2, " - Your Lizards repelled attack. ");

		      inc_combat_points (&new_hex);

		      fprintf (fptr2, "They are now %s warriors.\n",
			       liz_cp_desc [new_hex.combat_points]);

		      if (new_red + new_green + new_grey + new_black + new_yellow > 0)
			fprintf (fptr2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
			  show_lizards (new_red, new_green, new_grey, new_black,
			  new_yellow, temp_string));

		      fclose (fptr2);
		    }

		    xy_to_string (x, y, work_string);

		    fclose (fptr);

		    put_hex (order->x_hex, order->y_hex, &old_hex);
		    put_hex (x, y, &new_hex);

		    return;
		  case 0:
		    /* Fought to the death */

		    if (lizards_in_hex (&new_hex) == 0 &&
			(red + green + grey + black + yellow) == 0)
		    {
		      xy_to_string (order->x_hex, order->y_hex, work_string);

		      fprintf (fptr," - %s\n", build_mesg (random (4) + 16, work_string, ""));

		      fclose (fptr);

		      if (fptr2 != NULL)
		      {
			xy_to_string (x, y, work_string);

			fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

			fclose (fptr2);
		      }

		      put_hex (order->x_hex, order->y_hex, &old_hex);
		      put_hex (x, y, &new_hex);

		      return;
		    } else
		      if (lizards_in_hex (&new_hex) == 0)
		      {
			/* Capture any killed ENEMY lizards only if NOT IN DEN OR
			   HOME DEN */

			if (new_hex.terrain != DEN && new_hex.terrain != HOME_DEN)
			{
			  num_captured = ((float) (enemy_captured_red +
			    enemy_captured_green + enemy_captured_grey +
			    enemy_captured_black + enemy_captured_yellow) / 100.00) *
			    (float) capture_level;

			  capture_lizards (&old_hex, num_captured, &enemy_captured_red,
			    &enemy_captured_green, &enemy_captured_grey,
			    &enemy_captured_black, &enemy_captured_yellow,
			    &new_red, &new_green, &new_grey, &new_black, &new_yellow);

			  red += new_red;
			  green += new_green;
			  grey += new_grey;
			  black += new_black;
			  yellow += new_yellow;
			} else {
			  num_captured = 0;

			  new_red = new_green = new_grey = new_black = new_yellow = 0;
			}

			fprintf (fptr, " - %s ", build_mesg (random (4) + 20, work_string, ""));

			xy_to_string (x, y, work_string);

			combat_experience = LZ_TRUE;

			if (old_hex.combat_points + 1 > MAX_COMBAT_POINTS)
			  fprintf (fptr, "They are still %s warriors.\n",
			    liz_cp_desc [old_hex.combat_points]);
			else
			  fprintf (fptr, "They are now %s warriors.\n",
			    liz_cp_desc [old_hex.combat_points + 1]);

			if (new_red + new_green + new_grey + new_black + new_yellow > 0)
			  fprintf (fptr, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
			  show_lizards (new_red, new_green, new_grey, new_black,
			  new_yellow, temp_string));

			if (new_hex.terrain == DEN)
			  fprintf (fptr, " - Your Lizards captured %s Den at %s.\n",
				   lizard_colours [new_hex.den_lizard_type],
				   work_string);
			else
			  if (new_hex.terrain == HOME_DEN)
			  {
			    fprintf (fptr, " - Your Lizards captured Home Den at %s.\n",
				       work_string);

			    /* to stop 'time bomb' effect of KILL player order
			       for HOME DENS! */

			    new_hex.turns_undisturbed = 0;

			    player_home_den [old_hex.hex_owner] ++;
			  }

			if (fptr2 != NULL)
			{
			  fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

			  if (new_hex.terrain == DEN || new_hex.terrain == HOME_DEN)
			  {
			    /* so their recruits fail! */

			    if (new_hex.terrain == HOME_DEN)
			      player_home_den [new_hex.hex_owner] --;

			    xy_to_string (x, y, work_string);

			    if (new_hex.terrain == HOME_DEN)
			      fprintf (fptr2, " - Your Home Den at %s is captured by '%s' Clan.\n",
					 work_string, player_clan_name [order->player]);
			    else
			      fprintf (fptr2, " - Your %s Den at %s is captured by '%s' Clan.\n",
					 lizard_colours [new_hex.den_lizard_type],
					 work_string, player_clan_name [order->player]);
			  }

			  fclose (fptr2);
			}
		      } else {
			/* ENEMY capture any killed lizards but only if ENEMY are NOT
			   in a Den or Home Den */

			if (new_hex.terrain != DEN && new_hex.terrain != HOME_DEN)
			{
			  num_captured = ((float) (captured_red + captured_green +
			    captured_grey + captured_black + captured_yellow) / 100.00) *
			    (float) enemy_capture_level;

			  capture_lizards (&new_hex, num_captured, &captured_red,
			    &captured_green, &captured_grey, &captured_black,
			    &captured_yellow, &new_red, &new_green, &new_grey, &new_black,
			    &new_yellow);
			} else {
			  num_captured = 0;

			  new_red = new_green = new_grey = new_black = new_yellow = 0;
			}

			if (fptr2 != NULL)
			{
			  fprintf (fptr2, " - %s ", build_mesg (random (4) + 20, work_string, ""));

			  inc_combat_points (&new_hex);

			  fprintf (fptr, "They are now %s warriors.\n",
				   liz_cp_desc [new_hex.combat_points]);

			  if (new_red + new_green + new_grey + new_black + new_yellow > 0)
			    fprintf (fptr2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
			      show_lizards (new_red, new_green, new_grey, new_black,
			      new_yellow, temp_string));

			  fclose (fptr2);
			}

			xy_to_string (order->x_hex, order->y_hex, work_string);

			fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

			fclose (fptr);

			put_hex (order->x_hex, order->y_hex, &old_hex);
			put_hex (x, y, &new_hex);

			return;
		      }

		    break;
		}
	      } else {
		/* Capture any killed ENEMY lizards */

		num_captured = ((float) (enemy_captured_red +
		  enemy_captured_green + enemy_captured_grey +
		  enemy_captured_black + enemy_captured_yellow) / 100.00) *
		  (float) capture_level;

		capture_lizards (&old_hex, num_captured, &enemy_captured_red,
		  &enemy_captured_green, &enemy_captured_grey,
		  &enemy_captured_black, &enemy_captured_yellow,
		  &new_red, &new_green, &new_grey, &new_black, &new_yellow);

		red += new_red;
		green += new_green;
		grey += new_grey;
		black += new_black;
		yellow += new_yellow;

		if (fptr2 != NULL)
		{
		  fprintf (fptr2, " - Your Lizards at %s flee from combat to",
			   work_string);

		  xy_to_string (x2, y2, work_string);

		  fprintf (fptr2, " %s.\n", work_string);

		  fprintf (fptr2, " - %s Lizards fled.\n",
			       show_lizards (new_hex.red_lizards, new_hex.green_lizards,
			       new_hex.grey_lizards, new_hex.black_lizards,
			       new_hex.yellow_lizards, temp_string));
		}

		put_hex (x, y, &new_hex);

		move_lizards (x, y, x2, y2, new_hex.red_lizards,
				  new_hex.green_lizards, new_hex.grey_lizards,
				  new_hex.black_lizards, new_hex.yellow_lizards);

		get_hex (x, y, &new_hex);
		get_hex (x2, y2, &flee_hex);

		if (flee_hex.terrain == WHIRLPOOL && flee_hex.bridge_here == 0 &&
		  flee_hex.raft_here == 0)
		{
		  fprintf (fptr2, " - Lizards have fled into Whirlpool at %s!\n",
		    work_string);

		  xy_to_string (x2, y2, work_string);

		  work3 = lizards_in_hex (&flee_hex) * WHIRLPOOL_KILL_LEVEL;

		  if (work3 < 1) work3 = 1;

		  fprintf (fptr2, " - %d Lizards are sucked down to their deaths in Whirlpool.\n",
		    work3);

		  kill_lizards_xy (&flee_hex, work3);

		  put_hex (x2, y2, &flee_hex);

		  if (lizards_in_hex (&flee_hex) == 0)
		    fprintf (fptr2, " - NO Lizards resist the Whirlpool.\n");
		  else
		    fprintf (fptr2, " - %s Lizards resist the Whirlpool.\n",
		      show_lizards (flee_hex.red_lizards, flee_hex.green_lizards,
			flee_hex.grey_lizards, flee_hex.black_lizards,
			flee_hex.yellow_lizards, temp_string));
		}

		fprintf (fptr, " - Your Lizards routed enemy. ");

		combat_experience = LZ_TRUE;

		if (old_hex.combat_points + 1 > MAX_COMBAT_POINTS)
		  fprintf (fptr, "They are still %s warriors.\n",
		    liz_cp_desc [old_hex.combat_points]);
		else
		  fprintf (fptr, "They are now %s warriors.\n",
		    liz_cp_desc [old_hex.combat_points + 1]);

		if (new_red + new_green + new_grey + new_black + new_yellow > 0)
		  fprintf (fptr, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
		    show_lizards (new_red, new_green, new_grey, new_black,
		    new_yellow, temp_string));

		xy_to_string (x, y, work_string);

		if (fptr2 != NULL)
		  fclose (fptr2);
	      }

	      break;
	    case 0:
	      /* Fought to the death */

	      if (lizards_in_hex (&new_hex) == 0 &&
		  (red + green + grey + black + yellow) == 0)
	      {
		xy_to_string (order->x_hex, order->y_hex, work_string);

		fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

		fclose (fptr);

		if (fptr2 != NULL)
		{
		  xy_to_string (x, y, work_string);

		  fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

		  fclose (fptr2);
		}

		put_hex (order->x_hex, order->y_hex, &old_hex);
		put_hex (x, y, &new_hex);

		return;
	      } else
		if (lizards_in_hex (&new_hex) == 0)
		{
		  /* Capture any killed ENEMY lizards only if NOT IN DEN OR
		     HOME DEN */

		  if (new_hex.terrain != DEN && new_hex.terrain != HOME_DEN)
		  {
		    num_captured = ((float) (enemy_captured_red +
		      enemy_captured_green + enemy_captured_grey +
		      enemy_captured_black + enemy_captured_yellow) / 100.00) *
		      (float) capture_level;

		    capture_lizards (&old_hex, num_captured, &enemy_captured_red,
		      &enemy_captured_green, &enemy_captured_grey,
		      &enemy_captured_black, &enemy_captured_yellow,
		      &new_red, &new_green, &new_grey, &new_black, &new_yellow);

		    red += new_red;
		    green += new_green;
		    grey += new_grey;
		    black += new_black;
		    yellow += new_yellow;
		  } else {
		    num_captured = 0;

		    new_red = new_green = new_grey = new_black = new_yellow = 0;
		  }

		  fprintf (fptr, " - %s ", build_mesg (random (4) + 20, work_string, ""));

		  combat_experience = LZ_TRUE;

		  if (old_hex.combat_points + 1 > MAX_COMBAT_POINTS)
		    fprintf (fptr, "They are still %s warriors.\n",
		      liz_cp_desc [old_hex.combat_points]);
		  else
		    fprintf (fptr, "They are now %s warriors.\n",
		      liz_cp_desc [old_hex.combat_points + 1]);

		  if (new_red + new_green + new_grey + new_black + new_yellow > 0)
		    fprintf (fptr, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
		      show_lizards (new_red, new_green, new_grey, new_black,
		      new_yellow, temp_string));

		  xy_to_string (x, y, work_string);

		  if (new_hex.terrain == DEN)
		    fprintf (fptr, " - Your Lizards captured %s Den at %s.\n",
			       lizard_colours [new_hex.den_lizard_type],
			       work_string);
		  else
		    if (new_hex.terrain == HOME_DEN)
		    {
		      fprintf (fptr, " - Your Lizards captured %s Home Den at %s.\n",
				 lizard_colours [new_hex.den_lizard_type],
				 work_string);

		      /* to stop 'time bomb' effect of KILL player order
			       for HOME DENS! */

		      new_hex.turns_undisturbed = 0;

		      player_home_den [old_hex.hex_owner] ++;
		    }

		  if (fptr2 != NULL)
		  {
		    fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

		    if (new_hex.terrain == DEN || new_hex.terrain == HOME_DEN)
		    {
		      /* so their recruits fail! */

		      if (new_hex.terrain == HOME_DEN)
			player_home_den [new_hex.hex_owner] --;

		      xy_to_string (x, y, work_string);

		      if (new_hex.terrain == HOME_DEN)
			fprintf (fptr2, " - Your Home Den at %s is captured by '%s' Clan.\n",
			  work_string, player_clan_name [order->player]);
		      else
			fprintf (fptr2, " - Your %s Den at %s is captured by '%s' Clan.\n",
			  lizard_colours [new_hex.den_lizard_type],
			  work_string, player_clan_name [order->player]);
		    }

		    fclose (fptr2);
		  }
		} else {
		  /* ENEMY capture any killed lizards but only if ENEMY are NOT
		   in a Den or Home Den */

		  if (new_hex.terrain != DEN && new_hex.terrain != HOME_DEN)
		  {
		    num_captured = ((float) (captured_red + captured_green +
		      captured_grey + captured_black + captured_yellow) / 100.00) *
		      (float) enemy_capture_level;

		    capture_lizards (&new_hex, num_captured, &captured_red,
		      &captured_green, &captured_grey, &captured_black,
		      &captured_yellow, &new_red, &new_green, &new_grey, &new_black,
		      &new_yellow);
		  } else {
		    num_captured = 0;

		    new_red = new_green = new_grey = new_black = new_yellow = 0;
		  }

		  if (fptr2 != NULL)
		  {
		    fprintf (fptr2, " - %s ", build_mesg (random (4) + 20, work_string, ""));

		    inc_combat_points (&new_hex);

		    fprintf (fptr2, "They are now %s warriors.\n",
			     liz_cp_desc [new_hex.combat_points]);

                    if (new_red + new_green + new_grey + new_black + new_yellow > 0)
		      fprintf (fptr2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n",
			show_lizards (new_red, new_green, new_grey, new_black,
			new_yellow, temp_string));

                    fclose (fptr2);
		  }

		  xy_to_string (order->x_hex, order->y_hex, work_string);

		  fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, work_string, ""));

		  fclose (fptr);

		  put_hex (order->x_hex, order->y_hex, &old_hex);
		  put_hex (x, y, &new_hex);

		  return;
		}

	      break;
	  }
	} else {
	  if (new_hex.hex_owner > 0 && (new_hex.terrain == DEN ||
	      new_hex.terrain == HOME_DEN) && new_hex.hex_owner !=
	      order->player)
	  {
	    xy_to_string (x, y, work_string);

	    if (new_hex.terrain == HOME_DEN)
	    {
	      fprintf (fptr, " - Lizards capture '%s' Clan Home Den at %s without a struggle.\n",
			   player_clan_name [new_hex.hex_owner], work_string);

	      /* to stop 'time bomb' effect of KILL player order
			       for HOME DENS! */

	      new_hex.turns_undisturbed = 0;

	      player_home_den [order->player] ++;
	    } else
	      fprintf (fptr, " - Lizards capture '%s' Clan Den at %s without a struggle.\n",
			   player_clan_name [new_hex.hex_owner], work_string);
	  } else
	    if (new_hex.hex_owner == 0 && (new_hex.terrain == DEN ||
		    new_hex.terrain == HOME_DEN))
	    {
	      xy_to_string (x, y, work_string);

	      if (new_hex.terrain == HOME_DEN)
	      {
		fprintf (fptr, " - Lizards capture free %s Home Den at %s without a struggle.\n",
			     lizard_colours [new_hex.den_lizard_type], work_string);

		/* to stop 'time bomb' effect of KILL player order
			       for HOME DENS! */

		new_hex.turns_undisturbed = 0;

		player_home_den [order->player] ++;
	      } else
		fprintf (fptr, " - Lizards capture free %s Den at %s without a struggle.\n",
			     lizard_colours [new_hex.den_lizard_type], work_string);
	    }
	}

      switch (new_hex.terrain)
      {
	case WATER:
	  if (new_hex.bridge_here == 0 &&
	     (old_hex.raft_here == 0 || (old_hex.raft_here > 0 &&
	      order->order_type != SAIL)) && new_hex.raft_here == 0 &&
	      random (100) < SEA_MONSTER_LEVEL &&
	      terrain_adjacent (x, y, WATER) > 0)
	  {

	    /* Lizards attacked by Sea Monster */

	    xy_to_string (x, y, work_string);

	    fprintf (fptr, " - %s\n", build_mesg (random (4) + 4, work_string, ""));

	    fclose (fptr);

	    work2 = terrain_adjacent (x, y, WATER);

	    work3 = SEA_MONSTER_STRENGTH_LEVEL * work2;

	    put_hex (order->x_hex, order->y_hex, &old_hex);

	    combat_sea_monster (red, green, grey, black, yellow, &new_red,
				    &new_green, &new_grey, &new_black, &new_yellow,
				    order->x_hex, order->y_hex, &work3);

	    get_hex (order->x_hex, order->y_hex, &old_hex);

	    old_hex.red_lizards -=
			(red - new_red);
	    old_hex.green_lizards -=
			(green - new_green);
	    old_hex.grey_lizards -=
			(grey - new_grey);
	    old_hex.black_lizards -=
			(black - new_black);
	    old_hex.yellow_lizards -=
			(yellow - new_yellow);

	    red = new_red;
	    green = new_green;
	    grey = new_grey;
	    black = new_black;
	    yellow = new_yellow;

	    open_player_mesg (order->player, &fptr);

	    if (work3 == 0)
	    {
	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 8, work_string, ""));

	      fprintf (fptr, " - %s Lizards survive at %s.\n",
			 show_lizards (red, green, grey, black, yellow, temp_string),
			 work_string);
	    } else {
	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 12, work_string, ""));

	      fclose (fptr);

	      put_hex (order->x_hex, order->y_hex, &old_hex);

	      return;
	    }
	  }

	  break;
	case WHIRLPOOL:
	  if (new_hex.bridge_here == 0 &&
	     (old_hex.raft_here == 0 || (old_hex.raft_here > 0 &&
	      order->order_type != SAIL)) && new_hex.raft_here == 0)
	  {
	    fprintf (fptr, " - Lizards have moved into Whirlpool at %s.\n",
			 work_string);

	    work3 = (float) (red + green + grey + black + yellow) *
	      WHIRLPOOL_KILL_LEVEL;

	    if (work3 < 1) work3 = 1;

	    fprintf (fptr, " - %d Lizards are sucked down to their deaths in Whirlpool.\n",
			 work3);

	    kill_lizards (red, green, grey, black, yellow, &new_red, &new_green,
			      &new_grey, &new_black, &new_yellow, work3);

	    old_hex.red_lizards -=
			  (red - new_red);
	    old_hex.green_lizards -=
			  (green - new_green);
	    old_hex.grey_lizards -=
			  (grey - new_grey);
	    old_hex.black_lizards -=
			  (black - new_black);
	    old_hex.yellow_lizards -=
			  (yellow - new_yellow);

	    red = new_red;
	    green = new_green;
	    grey = new_grey;
	    black = new_black;
	    yellow = new_yellow;

	    if ((red + green + grey + black + yellow) == 0)
	    {
	      fprintf (fptr, " - NO Lizards resist the Whirlpool.\n");

	      fclose (fptr);

	      put_hex (order->x_hex, order->y_hex, &old_hex);

	      return;
	    } else
	      fprintf (fptr, " - %s Lizards resist the Whirlpool.\n",
			   show_lizards (red, green, grey, black, yellow, temp_string));
	  }

	  break;
	case SWAMP:
	  if (random (100) < SWAMP_MIRE_LEVEL)
	  {
	    /* Lizards mired in Swamp */

	    xy_to_string (x, y, work_string);

	    fprintf (fptr, " - Lizards MIRED in Swamp at %s for next turn.\n",
			 work_string);

	    new_hex.lizards_immobile = 2;

	    if (work == 0)
	      order->moves [1] = 0;
	  }

	  break;
	case CURSED:
	  fprintf (fptr, " - Lizards have moved into CURSED hex at %s.\n",
		       work_string);

	  work3 = (float) (red + green + grey + black + yellow) *
	    CURSED_KILL_LEVEL;

	  if (work3 < 1) work3 = 1;

	  fprintf (fptr, " - %d Lizards choke and collapse, dead.\n",
		       work3);

	  kill_lizards (red, green, grey, black, yellow, &new_red, &new_green,
			    &new_grey, &new_black, &new_yellow, work3);

	  old_hex.red_lizards -=
			  (red - new_red);
	  old_hex.green_lizards -=
			  (green - new_green);
	  old_hex.grey_lizards -=
			  (grey - new_grey);
	  old_hex.black_lizards -=
			  (black - new_black);
	  old_hex.yellow_lizards -=
			  (yellow - new_yellow);

	  red = new_red;
	  green = new_green;
	  grey = new_grey;
	  black = new_black;
	  yellow = new_yellow;

	  if ((red + green + grey + black + yellow) == 0)
	  {
	    fprintf (fptr, " - NO Lizards resist the CURSE.\n");

	    fclose (fptr);

	    put_hex (order->x_hex, order->y_hex, &old_hex);

	    return;
	  } else
	    fprintf (fptr, " - %s Lizards resist the CURSE.\n",
		       show_lizards (red, green, grey, black, yellow, temp_string));

	  break;
	default:
	  break;
      }

      if (order->order_type == SAIL)
      {
	if (new_hex.raft_here > 0)
	  fprintf (fptr, " - CRASH! Your Lizards ram and sink the raft at %s.\n", work_string);

	new_hex.raft_here = old_hex.raft_here;
	old_hex.raft_here = 0;
      }

      xy_to_string (x, y, work_string);

      fclose (fptr);

      hex_attacked_or_moved_into (x, y, &new_hex);

      open_player_mesg (order->player, &fptr);

      put_hex (order->x_hex, order->y_hex, &old_hex);
      put_hex (x, y, &new_hex);

      move_lizards (order->x_hex, order->y_hex, x, y, red, green, grey,
	black, yellow);

      get_hex (x, y, &new_hex);

      /* can increase cp's here without fear of screwing up cp's for
	 merging bands as lizards can only gain cp by moving into hex
	 that USED to contain ENEMY lizards! */

      if (combat_experience)
      {
	combat_experience = LZ_FALSE;

	inc_combat_points (&new_hex);

	put_hex (x, y, &new_hex);
      }

      if (new_hex.terrain == FERTILE)
      {
	xy_to_string (x, y, work_string);

	fprintf (fptr, " - Lizards eat from fertile hex %s.\n", work_string);

	new_hex.turns_hungry = 0;

	put_hex (x, y, &new_hex);
      }

      order->x_hex = x;
      order->y_hex = y;

      get_hex (order->x_hex, order->y_hex, &old_hex);
    }

  fclose (fptr);

  if (old_order.x_hex == order->x_hex && old_order.y_hex == order->y_hex)
  {
    /* order has moved back on self, into starting hex */

    if (old_order.order_type == MOVE || old_order.order_type == SAIL)
    {
      /* do not allow any further moves, sails or splits FROM this hex as
	 all lizards have already moved back into hex */

      for (work = 0; work < number_orders; work ++)
	if (order_list [work] != order &&
	  order_list [work]->x_hex == old_order.x_hex &&
	  order_list [work]->y_hex == old_order.y_hex &&
	  order_list [work]->player == old_order.player &&
	  (order_list [work]->order_type == SPLIT ||
	   order_list [work]->order_type == MOVE ||
	   order_list [work]->order_type == SAIL))
	{
	  open_player_mesg (order_list [work]->player, &fptr);

	  xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
		      work_string);

	  switch (order_list [work]->order_type)
	  {
	    case MOVE:
	      fprintf (fptr, "\n - Move %s (", work_string);
	      break;
	    case SAIL:
	      fprintf (fptr, "\n - Sail %s (", work_string);
	      break;
	    case SPLIT:
	      fprintf (fptr, "\n - Split %s (", work_string);
	      break;
	  }

	  if (order_list [work]->moves [1] != 0)
	    fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]],
	      dir [order_list [work]->moves [1]]);
	  else
	    fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

	  fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");

	  fclose (fptr);

	  order_list [work]->order_type = INACTIVE;
	}
    } else
      if (old_order.order_type == SPLIT)
      {
	/* split moved back on self, do not allow any further splits that
	   would exceed the UNMOVED lizards in hex, and adjust any moves
	   or sails downwards to NOT include the moved lizards */

	get_hex (old_order.x_hex, old_order.y_hex, &old_hex);

	for (work = 0; work < number_orders; work ++)
	  if (order_list [work] != order &&
	    order_list [work]->x_hex == old_order.x_hex &&
	    order_list [work]->y_hex == old_order.y_hex &&
	    order_list [work]->player == old_order.player)
	    if (order_list [work]->order_type == SPLIT)
	    {
	      if (order_list [work]->red_lizards > (old_hex.red_lizards -
		order->red_lizards))
		order_list [work]->red_lizards = (old_hex.red_lizards -
		  order->red_lizards);

	      if (order_list [work]->green_lizards > (old_hex.green_lizards -
		order->green_lizards))
		order_list [work]->green_lizards = (old_hex.green_lizards -
		  order->green_lizards);

	      if (order_list [work]->grey_lizards > (old_hex.grey_lizards -
		order->grey_lizards))
		order_list [work]->grey_lizards = (old_hex.grey_lizards -
		  order->grey_lizards);

	      if (order_list [work]->black_lizards > (old_hex.black_lizards -
		order->black_lizards))
		order_list [work]->black_lizards = (old_hex.black_lizards -
		  order->black_lizards);

	      if (order_list [work]->yellow_lizards > (old_hex.yellow_lizards -
		order->yellow_lizards))
		order_list [work]->yellow_lizards = (old_hex.yellow_lizards -
		  order->yellow_lizards);

	      if (order_list [work]->red_lizards + order_list [work]->green_lizards +
		order_list [work]->grey_lizards + order_list [work]->black_lizards +
		order_list [work]->yellow_lizards == 0)
	      {
		open_player_mesg (order_list [work]->player, &fptr);

		xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
		  work_string);

		fprintf (fptr, "\n - Split %s (", work_string);

		if (order_list [work]->moves [1] != 0)
		  fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]],
		    dir [order_list [work]->moves [1]]);
		else
		  fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

		if (lizards_in_hex (&old_hex) > 0)
		  fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
		else
		  fprintf (fptr, "order ignored. NO Lizards in hex.\n");

		fclose (fptr);

		order_list [work]->order_type = INACTIVE;
	      }
	    } else {
	      if (order_list [work]->order_type == MOVE ||
		order_list [work]->order_type == SAIL)
	      {
		/* adjust moves and sails downwards to NOT include lizards that
		   have already moved by split */

		if (order_list [work]->red_lizards > (old_hex.red_lizards -
		  order->red_lizards))
		  order_list [work]->red_lizards = (old_hex.red_lizards -
		    order->red_lizards);

		if (order_list [work]->green_lizards > (old_hex.green_lizards -
		  order->green_lizards))
		  order_list [work]->green_lizards = (old_hex.green_lizards -
		    order->green_lizards);

		if (order_list [work]->grey_lizards > (old_hex.grey_lizards -
		  order->grey_lizards))
		  order_list [work]->grey_lizards = (old_hex.grey_lizards -
		    order->grey_lizards);

		if (order_list [work]->black_lizards > (old_hex.black_lizards -
		  order->black_lizards))
		  order_list [work]->black_lizards = (old_hex.black_lizards -
		    order->black_lizards);

		if (order_list [work]->yellow_lizards > (old_hex.yellow_lizards -
		  order->yellow_lizards))
		  order_list [work]->yellow_lizards = (old_hex.yellow_lizards -
		    order->yellow_lizards);

		if (order_list [work]->red_lizards + order_list [work]->green_lizards +
		  order_list [work]->grey_lizards + order_list [work]->black_lizards +
		  order_list [work]->yellow_lizards == 0)
		{
		  open_player_mesg (order_list [work]->player, &fptr);

		  xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
		    work_string);

		  if (order_list [work]->order_type == MOVE)
		    fprintf (fptr, "\n - Move %s (", work_string);
		  else
		    fprintf (fptr, "\n - Sail %s (", work_string);

		  if (order_list [work]->moves [1] != 0)
		    fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]],
		      dir [order_list [work]->moves [1]]);
		  else
		    fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

		  if (lizards_in_hex (&old_hex) > 0)
		    fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
		  else
		    fprintf (fptr, "order ignored. NO Lizards in hex.\n");

		  fclose (fptr);

		  order_list [work]->order_type = INACTIVE;
		}
	      }
	    }
      }
  }

  /* adjust any further split from hex to compensate for moved lizards */

  get_hex (old_order.x_hex, old_order.y_hex, &old_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == old_order.x_hex &&
	order_list [work]->y_hex == old_order.y_hex &&
	order_list [work]->player == old_order.player &&
	order_list [work]->order_type == SPLIT)
    {
      if (order_list [work]->red_lizards > old_hex.red_lizards)
	order_list [work]->red_lizards = old_hex.red_lizards;

      if (order_list [work]->green_lizards > old_hex.green_lizards)
	order_list [work]->green_lizards = old_hex.green_lizards;

      if (order_list [work]->grey_lizards > old_hex.grey_lizards)
	order_list [work]->grey_lizards = old_hex.grey_lizards;

      if (order_list [work]->black_lizards > old_hex.black_lizards)
	order_list [work]->black_lizards = old_hex.black_lizards;

      if (order_list [work]->yellow_lizards > old_hex.yellow_lizards)
	order_list [work]->yellow_lizards = old_hex.yellow_lizards;

      if (order_list [work]->red_lizards + order_list [work]->green_lizards +
	  order_list [work]->grey_lizards + order_list [work]->black_lizards +
	  order_list [work]->yellow_lizards == 0)
      {
	open_player_mesg (order_list [work]->player, &fptr);

	xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
		      work_string);

	fprintf (fptr, "\n - Split %s (", work_string);

	if (order_list [work]->moves [1] != 0)
	  fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]],
	    dir [order_list [work]->moves [1]]);
	else
	  fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

	if (lizards_in_hex (&old_hex) > 0)
	  fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
	else
	  fprintf (fptr, "order ignored. NO Lizards in hex.\n");

	fclose (fptr);

	order_list [work]->order_type = INACTIVE;
      }
    }
}

void split_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work;

  order_t old_order;

  hex_t world_hex;

  old_order = *order;

  xy_to_string (order->x_hex, order->y_hex, work_string);

  open_player_mesg (order->player, &fptr);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if ((lizards_in_hex (&world_hex) > 0 &&
      world_hex.hex_owner != order->player) ||
      lizards_in_hex (&world_hex) == 0)
  {
    fprintf (fptr, " - Split %s (", work_string);

    if (order->moves [1] != 0)
      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
    else
      fprintf (fptr, "%s) ", dir [order->moves [0]]);

    fprintf (fptr, "order ignored. NO Lizards in hex.\n");

    fclose (fptr);

    return;
  } else
    if (world_hex.hex_owner == order->player &&
	lizards_in_hex (&world_hex) < (order->red_lizards + order->green_lizards +
	order->grey_lizards + order->black_lizards + order->yellow_lizards) &&
	(order->red_lizards + order->green_lizards + order->grey_lizards +
	order->black_lizards + order->yellow_lizards) != 1)
    {
      if (order->red_lizards > world_hex.red_lizards)
	if (world_hex.red_lizards == 0)
	{
	  fprintf (fptr, " - Split %s (", work_string);

	  if (order->moves [1] != 0)
	    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    fprintf (fptr, "%s) ", dir [order->moves [0]]);

	  fprintf (fptr, "order ignored. NO Red Lizards in hex.\n");

	  fclose (fptr);

	  return;
	} else
	  order->red_lizards = world_hex.red_lizards;

      if (order->green_lizards > world_hex.green_lizards)
	if (world_hex.green_lizards == 0)
	{
	  fprintf (fptr, " - Split %s (", work_string);

	  if (order->moves [1] != 0)
	    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    fprintf (fptr, "%s) ", dir [order->moves [0]]);

	  fprintf (fptr, "order ignored. NO Green Lizards in hex.\n");

	  fclose (fptr);

	  return;
	} else
	  order->green_lizards = world_hex.green_lizards;

      if (order->grey_lizards > world_hex.grey_lizards)
	if (world_hex.grey_lizards == 0)
	{
	  fprintf (fptr, " - Split %s (", work_string);

	  if (order->moves [1] != 0)
	    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    fprintf (fptr, "%s) ", dir [order->moves [0]]);

	  fprintf (fptr, "order ignored. NO Grey Lizards in hex.\n");

	  fclose (fptr);

	  return;
	} else
	  order->grey_lizards = world_hex.grey_lizards;

      if (order->black_lizards > world_hex.black_lizards)
	if (world_hex.black_lizards == 0)
	{
	  fprintf (fptr, " - Split %s (", work_string);

	  if (order->moves [1] != 0)
	    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    fprintf (fptr, "%s) ", dir [order->moves [0]]);

	  fprintf (fptr, "order ignored. NO Black Lizards in hex.\n");

	  fclose (fptr);

	  return;
	} else
	  order->black_lizards = world_hex.black_lizards;

      if (order->yellow_lizards > world_hex.yellow_lizards)
	if (world_hex.yellow_lizards == 0)
	{
	  fprintf (fptr, " - Split %s (", work_string);

	  if (order->moves [1] != 0)
	    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    fprintf (fptr, "%s) ", dir [order->moves [0]]);

	  fprintf (fptr, "order ignored. NO Yellow Lizards in hex.\n");

	  fclose (fptr);

	  return;
	} else
	  order->yellow_lizards = world_hex.yellow_lizards;
    }

  if (world_hex.lizards_immobile > 0)
  {
    fprintf (fptr, " - Split %s (", work_string);

    if (order->moves [1] != 0)
      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
    else
      fprintf (fptr, "%s) ", dir [order->moves [0]]);

    if (world_hex.lizards_immobile > 2)
      fprintf (fptr, "order ignored. Lizards are FROZEN this turn.\n");
    else
      fprintf (fptr, "order ignored. Lizards are MIRED in Swamp this turn.\n");

    fclose (fptr);

    return;
  }

  if (order->player > 0)
    if ((order->red_lizards + order->green_lizards + order->grey_lizards +
	 order->black_lizards + order->yellow_lizards) == 1)
    {
      if (lizards_in_hex (&world_hex) == 0 &&
	  player_spy_here (order->x_hex, order->y_hex, order->player) > -1)
      {

	/* don't do anything as spy lizard already here */

      } else {
	if ((spy_list [number_spys] = malloc (sizeof (spy_t))) == NULL)
	{
          fprintf (stderr, "FATAL ERROR: Out of memory.\n");
	  exit (EXIT_FAILURE);
	}

	spy_list [number_spys]->player = order->player;
	spy_list [number_spys]->x_hex = order->x_hex;
	spy_list [number_spys]->y_hex = order->y_hex;
	spy_list [number_spys]->turns_alone = 0;
	spy_list [number_spys]->mired = 0;
        spy_list [number_spys]->moved_this_turn = 0;

	if (order->red_lizards == 1)
	  if (world_hex.red_lizards == 0)
	  {
            fprintf (fptr, " - Split %s (", work_string);

            if (order->moves [1] != 0)
	      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	    else
	      fprintf (fptr, "%s) ", dir [order->moves [0]]);

	    fprintf (fptr, " ignored. NO Red Lizards in hex.\n");

	    free (spy_list [number_spys]);

	    fclose (fptr);

	    return;
	  } else {
	    world_hex.red_lizards -= 1;

	    spy_list [number_spys]->lizard_type = RED_LIZARD;
	  }
	else
	  if (order->green_lizards == 1)
	    if (world_hex.green_lizards == 0)
	  {
	    fprintf (fptr, " - Split %s (", work_string);

	    if (order->moves [1] != 0)
	      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
	    else
	      fprintf (fptr, "%s) ", dir [order->moves [0]]);

	    fprintf (fptr, " ignored. NO Green Lizards in hex.\n");

	    free (spy_list [number_spys]);

	    fclose (fptr);

	    return;
	  } else {
	    world_hex.green_lizards -= 1;

	    spy_list [number_spys]->lizard_type = GREEN_LIZARD;
	  }
	  else
	    if (order->grey_lizards == 1)
	      if (world_hex.grey_lizards == 0)
	      {
                fprintf (fptr, " - Split %s (", work_string);

		if (order->moves [1] != 0)
		  fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
		else
		  fprintf (fptr, "%s) ", dir [order->moves [0]]);

		fprintf (fptr, " ignored. NO Grey Lizards in hex.\n");

		free (spy_list [number_spys]);

		fclose (fptr);

		return;
	      } else {
		world_hex.grey_lizards -= 1;

		spy_list [number_spys]->lizard_type = GREY_LIZARD;
	      }
	    else
	      if (order->black_lizards == 1)
		if (world_hex.black_lizards == 0)
		{
                  fprintf (fptr, " - Split %s (", work_string);

		  if (order->moves [1] != 0)
		    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
		  else
		    fprintf (fptr, "%s) ", dir [order->moves [0]]);

		  fprintf (fptr, " ignored. NO Black Lizards in hex.\n");

		  free (spy_list [number_spys]);

		  fclose (fptr);

		  return;
		} else {
		  world_hex.black_lizards -= 1;

		  spy_list [number_spys]->lizard_type = BLACK_LIZARD;
		}
	      else
		if (world_hex.yellow_lizards == 0)
		{
                  fprintf (fptr, " - Split %s (", work_string);

		  if (order->moves [1] != 0)
		    fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
		  else
		    fprintf (fptr, "%s) ", dir [order->moves [0]]);

		  fprintf (fptr, " ignored. NO Yellow Lizards in hex.\n");

		  free (spy_list [number_spys]);

		  fclose (fptr);

		  return;
		} else {
		  world_hex.yellow_lizards -= 1;

		  spy_list [number_spys]->lizard_type = YELLOW_LIZARD;
		}

	number_spys += 1;
      }
    }

  fclose (fptr);

  put_hex (order->x_hex, order->y_hex, &world_hex);

  if ((order->red_lizards + order->green_lizards + order->grey_lizards +
       order->black_lizards + order->yellow_lizards) == 1)
    sneak_order (order);
  else
    move_order (order);

  /* adjust any further moves from hex to compensate for split lizards */

  get_hex (old_order.x_hex, old_order.y_hex, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == old_order.x_hex &&
	order_list [work]->y_hex == old_order.y_hex &&
	order_list [work]->player == old_order.player &&
	(order_list [work]->order_type == MOVE ||
	 order_list [work]->order_type == SAIL))
    {
      if (order_list [work]->red_lizards > world_hex.red_lizards)
	order_list [work]->red_lizards = world_hex.red_lizards;

      if (order_list [work]->green_lizards > world_hex.green_lizards)
	order_list [work]->green_lizards = world_hex.green_lizards;

      if (order_list [work]->grey_lizards > world_hex.grey_lizards)
	order_list [work]->grey_lizards = world_hex.grey_lizards;

      if (order_list [work]->black_lizards > world_hex.black_lizards)
	order_list [work]->black_lizards = world_hex.black_lizards;

      if (order_list [work]->yellow_lizards > world_hex.yellow_lizards)
	order_list [work]->yellow_lizards = world_hex.yellow_lizards;

      if (order_list [work]->red_lizards + order_list [work]->green_lizards +
	  order_list [work]->grey_lizards + order_list [work]->black_lizards +
	  order_list [work]->yellow_lizards == 0)
      {
	open_player_mesg (order_list [work]->player, &fptr);

	xy_to_string (order_list [work]->x_hex, order_list [work]->y_hex,
		      work_string);

	if (order_list [work]->order_type == MOVE)
	  fprintf (fptr, "\n - Split %s (", work_string);
	else
	  fprintf (fptr, "\n - Sail %s (", work_string);

	if (order_list [work]->moves [1] != 0)
	  fprintf (fptr, "%s %s) ", dir [order_list [work]->moves [0]],
	    dir [order_list [work]->moves [1]]);
	else
	  fprintf (fptr, "%s) ", dir [order_list [work]->moves [0]]);

	if (lizards_in_hex (&world_hex) > 0)
	  fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
	else
	  fprintf (fptr, "order ignored. NO Lizards in hex.\n");

	fclose (fptr);

	order_list [work]->order_type = INACTIVE;
      }
    }
}

void recruit_order (order_t *order)
{
  FILE *fptr;

  char work_string [80];

  int work,
      work2;

  hex_t world_hex;

  xy_to_string (order->x_hex, order->y_hex, work_string);

  open_player_mesg (order->player, &fptr);

  get_hex (order->x_hex, order->y_hex, &world_hex);

  if (world_hex.terrain != DEN && world_hex.terrain != HOME_DEN)
  {
    if (world_hex.terrain == RUIN)
      fprintf (fptr, " - Attempted to Recruit from Den at %s. Alas, it is now a Ruin.\n",
	      work_string);
    else
      fprintf (fptr, " - Attempted to Recruit from Den at %s that isn't yours or doesn't exist.\n",
	work_string);


    fclose (fptr);

    return;
  } else
    if (world_hex.hex_owner != order->player)
    {
      fprintf (fptr, " - Attempted to Recruit from Den at %s that isn't yours or doesn't exist.\n",
	       work_string);

      fclose (fptr);

      return;
    }

  if (world_hex.den_lizard_type == 0)
  {
    fprintf (fptr, " - Attempted to recruit from an un-settled Den at %s.\n",
	     work_string);

    fclose (fptr);

    return;
  }

  if (player_home_den [world_hex.hex_owner] == 0)
  {
    if (world_hex.terrain == DEN)
      fprintf (fptr, " - Attempted to recruit from %s Den at %s. Alas, your Clan no longer control a Home Den.\n",
		 lizard_colours [world_hex.den_lizard_type], work_string);
    else
      if (world_hex.terrain == HOME_DEN)
	fprintf (fptr, " - Attempted to recruit from Home Den at %s. Alas, your Clan no longer control a Home Den.\n",
		 work_string);

    fprintf (fptr, " - None of the Den Lizards feel compelled to join your Clan.\n");

    fclose (fptr);

    return;
  }

  work2 = world_hex.red_lizards +
	  world_hex.green_lizards +
	  world_hex.grey_lizards +
	  world_hex.black_lizards +
	  world_hex.yellow_lizards;

  if (work2 >= MAX_LIZARDS_IN_HEX)
  {
    if (world_hex.terrain == DEN)
       fprintf (fptr, " - Attempted to recruit from %s Den at %s but hex already contains 300 of your Lizards.\n",
		 lizard_colours [world_hex.den_lizard_type], work_string);
    else
      if (world_hex.terrain == HOME_DEN)
	fprintf (fptr, " - Attempted to recruit from Home Den at %s but hex already contains 300 of your Lizards.\n",
		 work_string);

    fclose (fptr);

    return;
  }

  if (world_hex.terrain == DEN)
    fprintf (fptr, " - Recruiting from %s Den at %s. ",
	       lizard_colours [world_hex.den_lizard_type],
	       work_string);
  else
    if (world_hex.terrain == HOME_DEN)
      fprintf (fptr, " - Recruiting from Home Den at %s. ", work_string);

  work = ((float) world_hex.lizard_pop) *
	  DEN_RECRUIT_LEVEL;

  if (work + work2 > MAX_LIZARDS_IN_HEX)
    work = MAX_LIZARDS_IN_HEX - work2;

  world_hex.combat_points =
    average_combat_points (lizards_in_hex (&world_hex), world_hex.combat_points,
			   work, 0);

  world_hex.turns_hungry =
    average_hunger_points (lizards_in_hex (&world_hex), world_hex.turns_hungry,
			   work, 0);

  switch (world_hex.den_lizard_type)
  {
    case RED_LIZARD:
      world_hex.lizard_pop -= work;
      world_hex.red_lizards += work;

      fprintf (fptr, "%d Red lizards recruited from Den", work);

      break;
    case GREEN_LIZARD:
      world_hex.lizard_pop -= work;
      world_hex.green_lizards += work;

      fprintf (fptr, "%d Green lizards recruited from Den", work);
      break;
    case GREY_LIZARD:
      world_hex.lizard_pop -= work;
      world_hex.grey_lizards += work;

      fprintf (fptr, "%d Grey lizards recruited from Den", work);

      break;
    case BLACK_LIZARD:
      world_hex.lizard_pop -= work;
      world_hex.black_lizards += work;

      fprintf (fptr, "%d Black lizards recruited from Den", work);

      break;
    case YELLOW_LIZARD:
      world_hex.lizard_pop -= work;
      world_hex.yellow_lizards += work;

      fprintf (fptr, "%d Yellow lizards recruited from Den", work);

      break;
  }

  fprintf (fptr, " leaving %d Den Lizards at %s.\n",
	   world_hex.lizard_pop,
	   work_string);

  put_hex (order->x_hex, order->y_hex, &world_hex);

  for (work = 0; work < number_orders; work ++)
    if (order_list [work] != order &&
	order_list [work]->x_hex == order->x_hex &&
	order_list [work]->y_hex == order->y_hex &&
	order_list [work]->player == order->player &&
	order_list [work]->order_type == RECRUIT)
    {
      fprintf (fptr, " - Next Recruit Order at %s ignored.\n",
	       work_string);

      order_list [work]->order_type = INACTIVE;
    }

  fclose (fptr);
}

void sneak_order (order_t *order)
{
  FILE *fptr,
       *fptr2;

  char work_string [80];

  int work,
      work2,
      x,
      y,
      spy;

  hex_t old_hex,
	new_hex;

  spy = moveable_player_spy_here (order->x_hex, order->y_hex, order->player);

  get_hex (order->x_hex, order->y_hex, &old_hex);

  if (spy > -1)
  {

    /* Spy Move Order */

    open_player_mesg (order->player, &fptr);

    xy_to_string (order->x_hex, order->y_hex, work_string);

    if (spy_list [spy]->moved_this_turn)
    {
      fprintf (fptr, " - Sneak %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. Spy Lizard has already moved this turn.\n");

      fclose (fptr);

      return;
    }

    if (spy_list [spy]->mired > 0)
    {
      fprintf (fptr, " - Sneak %s (", work_string);

      if (order->moves [1] != 0)
	fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
      else
	fprintf (fptr, "%s) ", dir [order->moves [0]]);

      fprintf (fptr, "order ignored. Spy Lizard is MIRED in Swamp this turn.\n");

      fclose (fptr);

      return;
    }

    spy_list [spy]->moved_this_turn = 1;

    for (work = 0; work < 2; work ++)
      if (order->moves [work] > 0)
      {
	xy_to_string (order->x_hex, order->y_hex, work_string);

        move_coords (order->moves [work], order->x_hex, order->y_hex,
		     &x, &y);

        if (x == order->x_hex && y == order->y_hex)
	{
	  fprintf (fptr, " - Spy Lizard at %s tries to move %s off edge of World.\n",
		   work_string, dir [order->moves [work]]);

          fclose (fptr);

          return;
	}

	if (order->order_type == SPLIT && work == 0)
	  fprintf (fptr, " - Spy Lizard splits at %s and moves %s (to ", work_string,
		   dir [order->moves [work]]);
	else
	  fprintf (fptr, " - Spy Lizard at %s moves %s (to ", work_string,
		   dir [order->moves [work]]);

	xy_to_string (x, y, work_string);

	fprintf (fptr, "%s).\n", work_string);

	get_hex (x, y, &new_hex);

	if ((new_hex.terrain == DEN || new_hex.terrain == HOME_DEN) &&
	    new_hex.hex_owner != order->player)
	{

	  /* Spy captured by Enemy Den! */

	  xy_to_string (x, y, work_string);

	  if (new_hex.hex_owner > 0)
	    fprintf (fptr," - %s\n", build_mesg (random (4) + 28, work_string,
	      player_clan_name [new_hex.hex_owner]));
	  else
	    fprintf (fptr, " - Spy Lizard captured by Den at %s.\n",
		     work_string);

	  if (new_hex.hex_owner > 0)
	  {
	    open_player_mesg (new_hex.hex_owner, &fptr2);

	    fprintf (fptr2,"\n - %s\n", build_mesg (random (4) + 24, work_string,
	      player_clan_name [order->player]));

	    fclose (fptr2);
	  }

	  spy_list [spy]->player = 0;

	  fclose (fptr);

	  return;
	} else {
	  switch (new_hex.terrain)
	  {
	    case WATER:
	      if (new_hex.bridge_here == 0 && old_hex.raft_here == 0 &&
		  new_hex.raft_here == 0 && random (100) < SEA_MONSTER_LEVEL
		  && terrain_adjacent (x, y, WATER) > 0)
	      {

		/* Spy consumed by Sea Monster */

		xy_to_string (x, y, work_string);

		fprintf (fptr, " - %s\n", build_mesg (random (4), work_string, ""));

		spy_list [spy]->player = 0;

		fclose (fptr);

		return;
	      }

	      break;
	    case WHIRLPOOL:
	      if (new_hex.bridge_here == 0 &&  old_hex.raft_here == 0 &&
		  new_hex.raft_here == 0)
	      {
		xy_to_string (x, y, work_string);

		fprintf (fptr, " - Spy Lizard moved into Whirlpool at %s.\n",
			   work_string);

		fprintf (fptr, " - %s\n", build_mesg (random (4) + 40, work_string, ""));

		spy_list [spy]->player = 0;

		fclose (fptr);

		return;
	      }

	      break;
	    case SWAMP:
	      if (random (100) < SWAMP_MIRE_LEVEL)
	      {

		/* Spy mired in Swamp */

		xy_to_string (x, y, work_string);

		fprintf (fptr, " - Spy Lizard MIRED in Swamp at %s for next turn.\n",
			   work_string);

		if (work == 0)
		  order->moves [1] = 0;

		spy_list [spy]->mired = 2;
	      }

	      break;
	    case CURSED:
	      fprintf (fptr, " - Spy Lizard moved into CURSED hex at %s. ",
		       work_string);

	      fprintf (fptr, " Spy Lizard is poisoned and collapses, dead.\n");

	      spy_list [spy]->player = 0;

	      fclose (fptr);

	      return;
	    default:
	      break;
	  }

	  spy_list [spy]->x_hex = x;
	  spy_list [spy]->y_hex = y;

	  order->x_hex = x;
	  order->y_hex = y;

	  if ((work2 = lizards_in_hex (&new_hex)) > 0)
	  {
	    if (new_hex.hex_owner != order->player)
	    {

	      /* Spy captured by enemy band? */

	      if (random (SPY_CAPTURE_LEVEL) < work2)
	      {
		xy_to_string (x, y, work_string);

		if (new_hex.hex_owner > 0)
		  fprintf (fptr," - %s\n", build_mesg (random (4) + 36, work_string,
		    player_clan_name [new_hex.hex_owner]));
		else
		  fprintf (fptr, " - Spy Lizard captured by Lizards at %s.\n",
			     work_string);

		if (new_hex.hex_owner > 0)
		{
		  open_player_mesg (new_hex.hex_owner, &fptr2);

		  fprintf (fptr2,"\n - %s\n", build_mesg (random (4) + 32, work_string,
		    player_clan_name [order->player]));

		  fclose (fptr2);
		}

		spy_list [spy]->player = 0;

		fclose (fptr);

		put_hex (x, y, &new_hex);

		return;
	      }
	    }
	  }
	}
      }

    fclose (fptr);

    return;
  } else {
    open_player_mesg (order->player, &fptr);

    xy_to_string (order->x_hex, order->y_hex, work_string);

    fprintf (fptr, " - Sneak %s (", work_string);

    if (order->moves [1] != 0)
      fprintf (fptr, "%s %s) ", dir [order->moves [0]], dir [order->moves [1]]);
    else
      fprintf (fptr, "%s) ", dir [order->moves [0]]);

    if (spy == -2)
    {
      fprintf (fptr, "order ignored. You have Spys in hex, but they are mired or have already moved.\n");

      fclose (fptr);
    } else {
      fprintf (fptr, "order ignored. No Spy Lizard in hex.\n");

      fclose (fptr);
    }
  }
}
