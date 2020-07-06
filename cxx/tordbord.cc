#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

#define NUM_POSSIBLE_LINES 22

struct text_t {
  const char *full;
  const char *abb;
  int order;
  int (*func)(turn_t *, char *strs[], int player);
};

struct chant_text_t {
  const char *text;
  int order_num;
};


static const char* order_colours [5] = { "RED", "GRN", "GRY", "BLK", "YEL" };
static chant_text_t chant_code [NUMBER_CHANTS] = {
  { "EYES", CHANT_EYES },
  { "REVE", CHANT_REVE },
  { "SUMM", CHANT_SUMM },
  { "MOUL", CHANT_MOUL },
  { "FREE", CHANT_FREE },
  { "COER", CHANT_COER },
  { "TRAN", CHANT_TRAN },
  { "DARK", CHANT_DARK },
  { "SHUN", CHANT_SHUN },
  { "GATE", CHANT_GATE },
};

/* ----------------------------------------------------------------------
--
-- order_move
--
---------------------------------------------------------------------- */
static int order_move(turn_t *t, char *strs[], int player)
{
  order_t *order;
  hex_t *hex, *src, *dst;
  band_t *b;
  point_t xy;
  int i;

  if ((hex = src = (*t->world->map)(strs[1])) == NULL)
    {
      printf("Unknown hex %s.\n", strs[1]);
      return 0;
    }

  dirs_t dirs;

  for (i = 0; i < 2; i++)
    if (strs[i+2] && (dst = hex->adjacent(strs[i+2])))
      {
	dirs[i](strs[i+2]);
	hex = dst;
      }
  
  if (src == hex)
    return 0;

  order = new order_move_t(t, player, src, dst, dirs);

  printf("Create %s\n", (char*)(*order));

  assert(order != NULL);

  /* --------------------
     Add band to order even if empty and let the move resolver deal with it
     -------------------- */
  
  if ((b = (band_t*)src->has_feature(BAND)))
    order->band = new band_t(*b);
  else
    order->band = new band_t();

  if ((t->add_order(order) == 0))
    delete order;
  return 1;
}


/* ----------------------------------------------------------------------
--
-- order_split
--
---------------------------------------------------------------------- */
static int order_split(turn_t *t, char *strs[], int player)
{
  hex_t *hex, *src, *dst;
  int i, j, num;
  order_t *order;

  if ((hex = src = (*t->world->map)(strs[1])) == NULL)
    {
      printf("Unknown hex %s\n", strs[1]);
      return 0;
    }

  int count[LIZARD_TYPES] = {0,0,0,0,0};
  int stop = 0;
  for (i = 2; !stop; i++)
    {
      if ((strs[i] == NULL) || (num = atoi (strs[i])) == 0)
	{
	  stop = 1;
	  break;
	}
      char *t = strs[i] + strlen(strs[i]) - 3;
      for (j = 0; j < LIZARD_TYPES; j++)
	if (strcmp(t, order_colours[j]) == 0)
	  {
	    count[j] = num;
	    break;
	  }
      if (j == LIZARD_TYPES) stop = 1;
    }

  band_t *band = new band_t(count);

  dirs_t dirs;

  for (j = 0; j < 2; j++)
    if (strs[i+j] && (dst = hex->adjacent(strs[i+j])))
      {
	dirs[j](strs[i+j]);
	hex = dst;
      }

  if (src == hex)
    return 0;

  order = new order_split_t(t, player, src, dst, dirs);

  printf("Create %s\n", (char*)order);

  /* --------------------
     Add band to order even if empty and let the move resolver deal with it
     -------------------- */
  
  order->band = new band_t(*band);

  if (t->add_order(order) == 0)
    delete order;
  delete band;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_sail
--
---------------------------------------------------------------------- */
static int order_sail(turn_t *t, char *strs[], int player)
{
  order_t *order;
  hex_t *hex, *src, *dst;
  band_t *b;
  point_t xy;
  int i;

  if ((hex = src = (*t->world->map)(strs[1])) == NULL)
    {
      printf("Unknown hex %s.\n", strs[1]);
      return 0;
    }

  dir_t dirs[2];

  for (i = 0; i < 2; i++)
    if (strs[i+2] && (dst = hex->adjacent(dirs[i])))
      {
	dirs[i](strs[i+2]);
	hex = dst;
      }
  
  if (src == hex)
    return 0;

  if (dirs[1] < 0)
    printf("Create order SAIL from %s to %s (%s)\n",
	   (char*)(*order->src),
	   (char*)(*order->dst),
	   (char*)(dirs[0]));
  else
    printf("Create order SAIL from %s to %s (%s %s)\n",
	   (char*)(*order->src),
	   (char*)(*order->dst),
	   (char*)(dirs[0]), (char*)(dirs[1]));
  
  assert(order != NULL);

  /* --------------------
     Add band to order even if empty and let the move resolver deal with it
     -------------------- */
  
  if ((b = (band_t*)src->has_feature(BAND)))
      order->band = new band_t(*b);
  else
    order->band = new band_t();

  if (t->add_order(order) == 0)
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_sneak
--
---------------------------------------------------------------------- */
static int order_sneak(turn_t *t, char *strs[], int player)
{
  order_t *order;
  hex_t *hex, *src, *dst;
  point_t xy;
  int i;

  if ((hex = src = (*t->world->map)(strs[1])) == NULL)
    {
      printf("Unknown hex %s.\n", strs[1]);
      return 0;
    }

  dirs_t dirs;

  for (i = 0; i < 2; i++)
    if (strs[i+2] && (dst = hex->adjacent(dirs[i])))
      {
	dirs[i](strs[i+2]);
	hex = dst;
      }
  
  if (src == hex)
    return 0;

  order = new order_sneak_t(t, player, src, dst, dirs);

  assert(order != NULL);

  printf("Create order SNEAK from %s to %s (%s)\n",
	 (char*)(*order->src),
	 (char*)(*order->dst),
	 (char*)(dirs));
  
  if (t->add_order(order) == 0)
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_recruit
--
---------------------------------------------------------------------- */
static int order_recruit(turn_t *t, char *strs[], int player)
{
  hex_t *hex = (*t->world->map)(strs[1]);
  order_t *order;

  if (hex == NULL)
    {
      fprintf(stdout,
	      "(order-failed-stage-1 recruit %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }
  order = new order_recruit_t(t, player, hex);
  if (!t->add_order(order))
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_build_fertile
--
---------------------------------------------------------------------- */
static int order_build_fertile(turn_t *t, char *strs[], int player)
{
  hex_t *hex;
  order_t *order;

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      fprintf(stdout, "(order-failed-stage-1 build_fertile %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }

  order = new order_build_fertile_t(t, player, hex);
  if (!t->add_order(order))
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_destroy_fertile
--
---------------------------------------------------------------------- */
static int order_destroy_fertile(turn_t *t, char *strs[], int player)
{
  hex_t *hex;
  order_t *order;

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      fprintf(stdout, "(order-failed-stage-1 destroy_fertile %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }

  order = new order_destroy_fertile_t(t, player, hex);
  if (!t->add_order(order))
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_build_raft
--
---------------------------------------------------------------------- */
static int order_build_raft(turn_t *t, char *strs[], int player)
{
  hex_t *hex;
  order_t *order;

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      fprintf(stdout, "(order-failed-stage-1 destroy_fertile %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }

  dir_t dir(strs[2]);
  if (hex_t *dst = hex->adjacent(dir))
    {
      order = new order_build_raft_t(t, player, hex, dst, dir);
    }
  if (!t->add_order(order))
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_build_bridge
--
---------------------------------------------------------------------- */
static int order_build_bridge(turn_t *t, char *strs[], int player)
{
  hex_t *hex;
  order_t *order;

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      fprintf(stdout, "(order-failed-stage-1 build_bridge %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }

  dir_t dir(strs[2]);
  if (hex_t *dst = hex->adjacent(dir))
    {
      order = new order_build_bridge_t(t, player, hex, dst, dir);
    }
  if (!t->add_order(order))
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_destroy_bridge
--
---------------------------------------------------------------------- */
static int order_destroy_bridge(turn_t *t, char *strs[], int player)
{
  hex_t *hex;
  order_t *order;

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      fprintf(stdout, "(order-failed-stage-1 destroy_bridge %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }

  dir_t dir(strs[2]);
  if (hex_t *dst = hex->adjacent(dir))
    {
      order = new order_destroy_bridge_t(t, player, hex, dst, dir);
    }
  if (!t->add_order(order))
    delete order;
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_build_den
--
---------------------------------------------------------------------- */
static int order_build_den(turn_t *t, char *strs[], int player)
{
  hex_t *hex;
  order_t *order;

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      fprintf(stdout, "(order-failed-stage-1 build_den %s \"illegal hex\")\n",
	      hex->title);
      return 0;
    }

  order = new order_build_den_t(t, player, hex);
  if (!t->add_order(order))
    delete order;
  return 1;
}



/* ----------------------------------------------------------------------
--
-- order_message
--
---------------------------------------------------------------------- */
static int order_message(turn_t *t, char *strs[], int player)
{
#ifdef Ignore
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
#endif
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_chant
--
---------------------------------------------------------------------- */
static int order_chant(turn_t *t, char *strs[], int player)
{
  if (player == 0)
    {
      printf ("FATAL ERROR: Order issued when player number unknown.\n");
      exit (EXIT_FAILURE);
    }

  hex_t *src, *dst;
  player_t *p = t->world->get_player(player);

  if ((src = (*t->world->map)(strs[1])) == NULL)
    {
      p->mesg(2, " - Chant %s failed, illegal hex.\n", strs[1]);
      return 0;
    }

  int chant = NUMBER_CHANTS;

  for (int i = 0; i < NUMBER_CHANTS; i++)
    if (strcmp (strs[2], chant_code[i].text) == 0)
      {
	chant = i;
	break;
      }

  if (chant == NUMBER_CHANTS)
    {
      p->mesg(2, "Chant %s %s failed, unknown chant.\n", strs[1], strs[2]);
    }

  int count[LIZARD_TYPES] = {0,0,0,0,0};
  int i, j, num;
  int stop = 0;
  for (i = 2; !stop; i++)
    {
      if ((strs[i] == NULL) || (num = atoi (strs[i])) == 0)
	{
	  stop = 1;
	  break;
	}
      char *t = strs[i] + strlen(strs[i]) - 3;
      for (j = 0; j < LIZARD_TYPES; j++)
	if (strcmp(t, order_colours[j]) == 0)
	  {
	    count[j] = num;
	    break;
	  }
      if (j == LIZARD_TYPES) stop = 1;
    }

  order_t *order = NULL;
  dst = src;
  dirs_t dirs;

  if (chant == CHANT_GATE)
    {
      if (strs[i] || ((dst = (*t->world->map)(strs[i])) != NULL))
	order = new order_chant_gate_t(t, player, src, dst);
    }
  else
    {
      for (j = 0; j < 2; j++)
	if (strs[i+j] && (dirs[j](strs[i+j]) >= 0))
	  dst = dst->adjacent(dirs[j]);
      if (src != dst)
	switch(chant)
	  {
	  case CHANT_EYES:
	    order = new order_chant_eyes_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_REVE:
	    order = new order_chant_reve_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_SUMM:
	    order = new order_chant_summ_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_SHUN:
	    order = new order_chant_shun_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_TRAN:
	    order = new order_chant_tran_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_MOUL:
	    order = new order_chant_moul_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_DARK:
	    order = new order_chant_dark_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_COER:
	    order = new order_chant_coer_t(t, player, src, dst, dirs);
	    break;
	  case CHANT_FREE:
	    order = new order_chant_free_t(t, player, src, dst, dirs);
	    break;
	  }
    }
  if (order)
    {
      printf("Create order CHANT %s %s (%s)\n", strs[2], src->title, dst->title);
      if (!t->add_order(order))
	delete order;
    }
  else
    {
      t->world->get_player(player)->mesg(2, " - 'CH %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n", src->title);
    }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- give_message
--
---------------------------------------------------------------------- */
static int order_give(turn_t *t, char *strs[], int player)
{
#ifdef Ignore
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
#endif
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_lob
--
---------------------------------------------------------------------- */
static int order_lob(turn_t *t, char *strs[], int player)
{
#ifdef Ignore

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
#endif
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_free
--
---------------------------------------------------------------------- */
static int order_free(turn_t *t, char *strs[], int player)
{
  hex_t *hex;

  if (player == 0)
    {
      printf ("FATAL ERROR: Order issued when player number unknown.\n");
      exit (EXIT_FAILURE);
    }

  if ((hex = (*t->world->map)(strs[1])) == NULL)
    {
      printf("Unknown hex %s\n", strs[1]);
      return 0;
    }

  int count[LIZARD_TYPES] = {0,0,0,0,0};
  int num;

  for (int i = 2; i < 10; i++)
    {
      if ((num = atoi (strs[i])) == 0)
	break;
      for (int j = 0; j < LIZARD_TYPES; j++)
	if (strcmp (strs[i] + strlen(strs[i]) - 3, order_colours [j]) == 0)
	  break;
    }

  band_t *band = new band_t(count);

  order_t *order = new order_free_t(t, player, hex);
  order->band = band;

  if (!t->add_order(order))
    delete order;
  return 1;
  

  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_kill
--
---------------------------------------------------------------------- */
static int order_kill(turn_t *t, char *strs[], int player)
{
#ifdef Ignore

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
#endif
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_erupt
--
---------------------------------------------------------------------- */
static int order_erupt(turn_t *t, char *strs[], int player)
{
#ifdef Ignore
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
#endif
  return 1;
}


/* ----------------------------------------------------------------------
--
-- order_spawn
--
---------------------------------------------------------------------- */
static int order_spawn(turn_t *t, char *strs[], int player)
{
#ifdef Ignore
  sprintf (filename, "%s%s%s", world->path, WORLD_SWITCH_F, world->game);

  if ((fptr = fopen (filename, "wt")) == '\0')
    {
      printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
      exit (EXIT_FAILURE);
    }
  fprintf (fptr, "SPAWN\n");
  fclose (fptr);
#endif
  return 1;
}

/* ----------------------------------------------------------------------
--
-- order_mod_mesg
--
---------------------------------------------------------------------- */
static int order_mod_mesg(turn_t *t, char *strs[], int player)
{
  #ifdef Ignore
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
#endif
  return 1;
}

/* ----------------------------------------------------------------------
--
-- ProcessFile
--
---------------------------------------------------------------------- */
static text_t possible_lines [NUM_POSSIBLE_LINES] = {
  {"PLAYER", "PL", PLAYER, NULL},
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
  {"FREE", "FR", FREE, order_free},
  {"CHANT", "CH", CHANT, order_chant},
  {"GIVE", "GI", GIVE, order_give},
  {"LOB", "LO", LOB, order_lob},
  {"MESSAGE", "MS", MESSAGE, order_message},
  {"MOD_MESG", "MMS", MOD_MESG, order_mod_mesg},
  {"KILL", "KI", KILL_PLAYER, order_kill},
  {"ERUPT", "ER", ERUPT_VOLCANO, order_erupt},
  {"SPAWN_TURN", "ST", SPAWN_TURN, order_spawn},
  { NULL, NULL, NULL, NULL },
};

static int ProcessFile(FILE *in_fptr, turn_t *t)
{
  char str[256], *strs[15];
  int i, present_player;
  text_t *line_type;
  world_t *world = t->world;

  while (fgets (str, 256, in_fptr) != NULL)
    {
      for (i = 0; i < (int)(strlen(str)); i++)
	if (islower(str[i]))
	  str[i] = toupper(str[i]);

      /* --------------------
	 Break string into tokens
	 -------------------- */

      for (i = 0, strs[i] = strtok(str, " \n");
	   (i < 12) && strs[i] && strlen(strs[i]);
	   strs[i] = strtok(NULL, " \n"))
	i++;

      for (i = 0; strs[i]; i++)
	printf("%s ", strs[i]);

      printf("\n");

      /* --------------------
	 Match token to full or abbviated instruction
	 -------------------- */

      for (i = 0; possible_lines[i].full; i++)
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
	(*line_type->func)(t, strs, present_player);
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
	  default:
	    fprintf(stdout, "Unknown order found.\n");
	    break;
	  }
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */
int turn_t::textorderconvert()
{
  char filename [255];
  int i;
  FILE *in_fptr;

  sprintf (filename, "%s/%s.%s%03d", world->dir, world->game, TEXT_ORDS_F,
	   world->turn);
  num_orders = 0;
  for (i = 0; i < world->num_players; i++)
    if (world->player[i]) world->player[i]->num_orders = 0;
  if ((in_fptr = fopen (filename, "r")) != NULL)
    {
      ProcessFile(in_fptr, this);
      fclose (in_fptr);
    }
  else
    {
      fprintf(stderr, "Cannot open file %s", filename);
      return 0;
    }
  for (i = 1; i < world->num_players; i++)
    if (world->player[i] && (world->player[i]->num_orders == 0 && world->turn > 0))
      {
	player_t *p = world->get_player(i);
	p->mesg(2, " - SORRY, BUT THE MODERATOR DID NOT RECEIVE ORDERS FROM YOU FOR THIS TURN REPORT.\n"
		" - PLEASE POST YOUR ORDERS FOR THIS TURN EARLY, TO ENSURE THIS DOESN'T HAPPEN AGAIN.\n");
      }
  /*  remove (filename); */ /* remove order file now we've finished with it */
  return 1;
}
