#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizhex.h"
#include "lizmesg.h"
#include "lizmisc.h"

static int move_combat (world_t *w, order_t *order, feature_t *attack)
{
  hex_t *old_hex, *new_hex, *flee_hex;
  char source[5], dest[5], flee[5], temp_string[80];
  feature_t *defend, *den;
  FILE *fptr, *fptr2;
  int work2, work3, work4, combat_experience;
  coord xy;

  debug(w, "move_combat entered");

  old_hex = get_hex(w, order->source);
  new_hex = get_hex(w, order->dest);
  combat_experience = LIZ_FALSE;
  open_player_mesg (w, order->player, &fptr);
  xy_to_string (old_hex->xy.x, old_hex->xy.y, source);
  xy_to_string (new_hex->xy.x, new_hex->xy.y, dest);

  if (lizards_in_hex (new_hex) > 0)
    {
      defend = has_feature(new_hex, BAND);
      if ((den = has_feature(new_hex, DEN)))
	{
	  if (new_hex->owner == 0)
	    fprintf (fptr, " - Lizards attack %d militia Lizards in free Den at %s.\n",
		     lizards_in_hex (new_hex), dest);
	  else if (den->f.den.home == 1)
	    fprintf (fptr, " - Lizards attack %d Lizards in Home Den of '%s' Clan at %s.\n",
		     lizards_in_hex (new_hex),
		     w->player[new_hex->owner]->clan,
		     dest);
	  else
	    fprintf (fptr, " - Lizards attack %d Lizards in Den of '%s' Clan at %s.\n",
		     lizards_in_hex (new_hex),
		     w->player[new_hex->owner]->clan,
		     dest);
	}
      else
	fprintf (fptr, " - Lizards attack %d '%s' Clan Lizards at %s.\n",
		 lizards_in_hex (new_hex),
		 w->player[new_hex->owner]->clan,
		 dest);
      if (new_hex->owner > 0)
	{
	  open_player_mesg (w, new_hex->owner, &fptr2);
	  if ((den = has_feature(new_hex, DEN)))
	    if (den->f.den.home)
	      fprintf (fptr2, "\n - Den at %s attacked by %d '%s' Clan Lizards.\n",
		       dest, lizards_in_band(&order->feature),
		       w->player[order->player]->clan);
	    else
	      fprintf (fptr2, "\n - Home Den at %s attacked by %d '%s' Clan Lizards.\n",
		       dest, lizards_in_band(&order->feature),
		       w->player[order->player]->clan);
	  else
	    fprintf (fptr2, "\n - %d Lizards at %s attacked by %d '%s' Clan Lizards.\n",
		     lizards_in_hex (new_hex), dest,
		     lizards_in_band (&order->feature),
		     w->player[order->player]->clan);

	  if (defend->f.band.immobile > 0)
	    if (defend->f.band.immobile > 2)
	      fprintf (fptr2, " - Lizards at %s are FROZEN and cannot flee from combat.\n",
		       dest);
	    else
	      fprintf (fptr2, " - Lizards at %s are MIRED and cannot flee from combat.\n",
		       dest);
	}
      hex_attacked_or_moved_into (w, new_hex);

      work2 = new_combat (w, attack, defend, old_hex, new_hex,
			  defend->f.band.immobile > 0 ? 1 : 0);

      if (new_hex->owner > 0)
	open_player_mesg (w, new_hex->owner, &fptr2);
      else
	fptr2 = NULL;
      switch (work2)
	{
	case -1:
	  /* Attackers flee! */
	  fprintf (fptr, " - Your Lizards flee from combat and remain at %s.\n",
		   source);
	  if (fptr2 != NULL)
	    {
	      fprintf (fptr2, " - Your Lizards repelled attack. ");
	      inc_combat_points (defend);
	      fprintf (fptr2, "They are now %s warriors.\n",
		       liz_cp_desc[defend->f.band.combat]);
	      if (has_feature(new_hex, DEN))
		{
		  fprintf (fptr2, " - %s %s at %s is still yours.\n",
			   den->f.den.home ? "Home Den" : "Den",
			   liz_colours[den->f.den.type],
			   dest);
		}
	    }
	  return combat_experience;
	case 1:
	  /* Defenders flee! */
	  work2 = random (6) + 1;
	  work4 = -1;
	  for (work3 = 0; work3 < 6 + 1; work2++, work3++)
	    {
	      if (work2 == 7)
		work2 = 1;
	      xy = move_coords (w, new_hex, work2);
	      flee_hex = get_hex (w, xy);
	      xy_to_string(xy.x, xy.y, flee);
	      if (lizards_in_hex(flee_hex) == 0 && !has_feature(flee_hex, DEN))
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
		}
	      fprintf (fptr, " - Enemy Lizards cannot flee. Your Lizards ATTACK.\n");
	      work2 = new_combat (w, attack, defend, old_hex, new_hex, 0);

	      /* calculate YOUR captured lizards */
	      open_player_mesg (w, order->player, &fptr);
	      if (new_hex->owner > 0)
		open_player_mesg (w, new_hex->owner, &fptr2);
	      else
		fptr2 = NULL;
	      switch (work2)
		{
		case -1:
		  /* Attackers flee! */
		  fprintf (fptr, " - Your Lizards flee from combat and remain at %s.\n",
			   source);
		  if (fptr2 != NULL)
		    {
		      fprintf (fptr2, " - Your Lizards repelled attack. ");
		      inc_combat_points (defend);
		      fprintf (fptr2, "They are now %s warriors.\n",
			       liz_cp_desc[defend->f.band.combat]);
		    }
		  return combat_experience;
		case 0:
		  /* Fought to the death */
		  if (lizards_in_band (attack) == 0 &&
		      lizards_in_band (defend) == 0)
		    {
		      fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, source, ""));
		      if (fptr2 != NULL)
			{
			  fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, dest, ""));
			}
		      return combat_experience;
		    }
		  else if (lizards_in_band (defend) == 0)
		    {
		      /* Capture any killed ENEMY lizards only if NOT IN DEN OR
		         HOME DEN */
		      if (den != NULL)
			{
			  fprintf (fptr, " - %s ", build_mesg (random (4) + 20, source, ""));
			  combat_experience = LIZ_TRUE;
			  if (attack->f.band.combat + 1 > MAX_COMBAT_POINTS)
			    fprintf (fptr, "They are still %s warriors.\n",
				     liz_cp_desc[attack->f.band.combat]);
			  else
			    fprintf (fptr, "They are now %s warriors.\n",
				     liz_cp_desc[attack->f.band.combat + 1]);
			  if (den != NULL)
			    {
			      fprintf (fptr, " - Your Lizards captured %s %s at %s.\n",
				       den->f.den.home ? "Home Den" : "Den",
				       liz_colours[den->f.den.type],
				       dest);
			      if (den->f.den.home)
				w->player[old_hex->owner]->home_den++;
			    }
			}
		      if (fptr2 != NULL)
			{
			  fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, dest, ""));
			  if (den != NULL)
			    {
			      /* so their recruits fail! */
			      if (den->f.den.home)
				w->player[new_hex->owner]->home_den--;
			      if (den->f.den.home)
				fprintf (fptr2, " - Your Home Den at %s is captured by '%s' Clan.\n",
					 dest,
					 w->player[order->player]->clan);
			      else
				fprintf (fptr2, " - Your %s Den at %s is captured by '%s' Clan.\n",
					 liz_colours[den->f.den.type],
					 dest,
					 w->player[order->player]->clan);
			    }
			}
		    }
		  else
		    {
		      /* ENEMY capture any killed lizards but only if ENEMY are NOT
		         in a Den or Home Den */
		      if (fptr2 != NULL)
			{
			  fprintf (fptr2, " - %s ", build_mesg (random (4) + 20, dest, ""));
			  inc_combat_points (defend);
			  fprintf (fptr, "They are now %s warriors.\n",
				   liz_cp_desc[defend->f.band.combat]);
			}
		      fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, source, ""));
		      return combat_experience;
		    }
		  break;
		}
	    }
	  else
	    {
	      if (fptr2 != NULL)
		{
		  fprintf (fptr2, " - Your Lizards at %s flee from combat to",
			   dest);
		  fprintf (fptr2, " %s.\n", flee);
		  fprintf (fptr2, " - %s Lizards fled.\n",
			   new_show_lizards (attack, temp_string));
		}
	      move_band (w, flee_hex, attack);
	      if (has_feature(flee_hex, WHIRLPOOL) &&
		  !has_feature(flee_hex, BRIDGE) &&
		  !has_feature(flee_hex, RAFT))
		{
		  fprintf (fptr2, " - Lizards have fled into Whirlpool at %s!\n",
			   flee);
		  work3 = (int)(lizards_in_band (defend) * WHIRLPOOL_KILL_LEVEL);
		  if (work3 < 1)
		    work3 = 1;
		  fprintf (fptr2, " - %d Lizards are sucked down to their deaths in Whirlpool.\n",
			   work3);
		  kill_lizards (defend, work3);
		  if (lizards_in_band (defend) == 0)
		    fprintf (fptr2, " - NO Lizards resist the Whirlpool.\n");
		  else
		    fprintf (fptr2, " - %s Lizards resist the Whirlpool.\n",
			     new_show_lizards (defend, temp_string));
		}
	      fprintf (fptr, " - Your Lizards routed enemy. ");
	      combat_experience = LIZ_TRUE;
	      if (defend->f.band.combat + 1 > MAX_COMBAT_POINTS)
		fprintf (fptr, "They are still %s warriors.\n",
			 liz_cp_desc[defend->f.band.combat]);
	      else
		fprintf (fptr, "They are now %s warriors.\n",
			 liz_cp_desc[defend->f.band.combat + 1]);
	    }
	  break;
	  /* ---------- Fought to the death ---------- */
	case 0:
	  if (lizards_in_band (attack) == 0 &&
	      lizards_in_band (defend) == 0)
	    {
	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, source, ""));
	      if (fptr2 != NULL)
		{
		  fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, dest, ""));
		}
	      return combat_experience;
	    }
	  else if (lizards_in_band (defend) == 0)
	    {
	      /* Capture any killed ENEMY lizards only if NOT IN DEN OR
	         HOME DEN */
	      fprintf (fptr, " - %s ", build_mesg (random (4) + 20, source, ""));
	      combat_experience = LIZ_TRUE;
	      if (attack->f.band.combat + 1 > MAX_COMBAT_POINTS)
		fprintf (fptr, "They are still %s warriors.\n",
			 liz_cp_desc[attack->f.band.combat]);
	      else
		fprintf (fptr, "They are now %s warriors.\n",
			 liz_cp_desc[attack->f.band.combat]);
	      if (den != NULL)
		if (den->f.den.home == 0)
		  fprintf (fptr, " - Your Lizards captured %s Den at %s.\n",
			   liz_colours[den->f.den.type],
			   dest);
		else
		  {
		    fprintf (fptr, " - Your Lizards captured %s Home Den at %s.\n",
			     liz_colours[den->f.den.type],
			     dest);
		  w->player[old_hex->owner]->home_den++;
		  }
	      if (fptr2 != NULL)
		{
		  fprintf (fptr2, " - %s\n", build_mesg (random (4) + 16, dest, ""));
		  if (den != NULL)
		    {
		      /* so their recruits fail! */
		      if (den->f.den.home)
			w->player[new_hex->owner]->home_den--;
		      if (den->f.den.home)
			fprintf (fptr2, " - Your Home Den at %s is captured by '%s' Clan.\n",
			   dest, w->player[order->player]->clan);
		      else
			fprintf (fptr2, " - Your %s Den at %s is captured by '%s' Clan.\n",
				 liz_colours[den->f.den.type],
				 dest, w->player[order->player]->clan);
		    }
		}
	    }
	  else
	    {
	      /* ENEMY capture any killed lizards but only if ENEMY are NOT
	         in a Den or Home Den */
	      if (fptr2 != NULL)
		{
		  fprintf (fptr2, " - %s ", build_mesg (random (4) + 20, dest, ""));
		  inc_combat_points (defend);
		  fprintf (fptr2, "They are now %s warriors.\n",
			   liz_cp_desc[defend->f.band.combat]);
		}
	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 16, source, ""));
	      return combat_experience;
	    }
	  break;
	}
    }
  else
    {
      if (new_hex->owner > 0 && den && new_hex->owner != order->player)
	{
	  if (den->f.den.home)
	    {
	      fprintf (fptr, " - Lizards capture '%s' Clan Home Den at %s without a struggle.\n",
		       w->player[new_hex->owner]->clan, dest);
	      w->player[order->player]->home_den++;
	    }
	  else
	    fprintf (fptr, " - Lizards capture '%s' Clan Den at %s without a struggle.\n",
		     w->player[new_hex->owner]->clan, dest);
	}
      else if (new_hex->owner == 0 && den)
	{
	  if (den->f.den.home)
	    {
	      fprintf (fptr, " - Lizards capture free %s Home Den at %s without a struggle.\n",
		       liz_colours[den->f.den.type], dest);
	      w->player[order->player]->home_den++;
	    }
	  else
	    fprintf (fptr, " - Lizards capture free %s Den at %s without a struggle.\n",
		     liz_colours[den->f.den.type], dest);
	}
    }
  return combat_experience;
}
/* ----------------------------------------------------------------------
   --
   -- Move Order
   --
   ---------------------------------------------------------------------- */
int move_order (world_t *w, order_t *order)
{
  FILE *fptr;
  char source[5], dest[5], temp_string[80];
  int i, sum, work2, work3;
  int combat_experience = LIZ_FALSE;
  hex_t *old_hex, *new_hex;
  feature_t *raft,  *staying, *moving, *band;

  open_player_mesg (w, order->player, &fptr);
  old_hex = get_hex (w, order->source);
  xy_to_string (order->source.x, order->source.y, source);
  xy_to_string (order->dest.x, order->dest.y, dest);

  if (old_hex->owner != order->player)
    {
      fprintf (fptr, "(move-order-failed %s %s \"Do not own hex\")\n",
	       source, dest);
      return 0;
    }
  if ((staying = has_feature (old_hex, BAND)) == NULL)
    {
      fprintf (fptr, "(%s-order-failed %s %s \"No lizards in hex\")\n",
	       order_types[order->type], source, dest);
      return 0;
    }
  order->feature.owner = order->player;
  moving = clone_feature(&order->feature);
  for (i = 0; moving && i < LIZARD_TYPES; i++)
    if (moving->f.band.type[i] > staying->f.band.type[i])
      {
	moving->f.band.type[i] = staying->f.band.type[i];
	if (lizards_in_band (staying) == 0 && order->type == SPLIT)
	  {
	    fprintf (fptr, " - Split %s %s ", source, dest);
	    fprintf (fptr, "order ignored. No %s Lizards in hex.\n",
		     liz_colours[i]);
	    return 0;
	  }
      }

  for (i = 0; moving && i < LIZARD_TYPES; i++)
    if (moving->f.band.type[i] < 0)
      moving->f.band.type[i] = 0;
  
  /* Normal Move Order */

  if (order->type == SAIL && !has_feature(old_hex, RAFT))
    {
      fprintf (fptr, " - Sail from %s to %s ", source, dest);
      fprintf (fptr, "order ignored. There is no raft here.\n");
      return 0;
    }
  if (staying->f.band.immobile > 0)
    {
      if (order->type == MOVE)
	fprintf (fptr, " - Move ");
      else if (order->type == SPLIT)
	fprintf (fptr, " - Split ");
      else
	fprintf (fptr, " - Sail ");
      fprintf (fptr, "from %s to %s ", source, dest);
      if (staying->f.band.immobile > 2)
	fprintf (fptr, "order ignored. Lizards are FROZEN this turn.\n");
      else
	fprintf (fptr, "order ignored. Lizards are MIRED in Swamp this turn.\n");
      return 0;
    }
    fprintf (fptr, " - %s Lizards at %s ",
	     new_show_lizards (moving, temp_string),
	     source);
    if (order->type == SAIL)
      fprintf (fptr, "sail ");
    else if (order->type == SPLIT)
      fprintf (fptr, "split and move to ");
    else
      fprintf (fptr, "move to ");
    fprintf (fptr, "%s.\n", dest);

  new_hex = get_hex (w, order->dest);

  if ((lizards_in_hex (new_hex) + lizards_in_band(moving)) >
      MAX_LIZARDS_IN_HEX && (new_hex->owner == order->player))
    {
      fprintf (fptr, " - But %s would contain more that %d Lizards. ",
	       dest, MAX_LIZARDS_IN_HEX);
      if (order->type == SAIL)
	fprintf (fptr, " Sail order ignored.\n");
      else if (order->type == MOVE)
	fprintf (fptr, " Move order ignored.\n");
      else
	fprintf (fptr, " Split order ignored.\n");
      return 0;
    }
  if (order->type == SAIL && has_feature(new_hex, BRIDGE))
    {
      fprintf (fptr, " - Lizards cannot sail under bridges (they're not high enough).\n");
      return 0;
    }
  if (order->type == SAIL && new_hex->terrain != WATER)
    {
      fprintf (fptr, " - CRUNCH! Lizards run the raft aground and it sinks.\n");
      free(release_feature(w, has_feature(old_hex, RAFT)));
    }

/* We split off the moving/attacking lizards before we do anything so
   as they are killed we don't have to adjust original band */

  if (split_band(staying, moving) == 0)
    free(release_feature(w, staying));

  if (new_hex->owner != order->player)
    move_combat (w, order, moving);

  switch (new_hex->terrain)
    {
    case WATER:
      if (!has_feature(new_hex, BRIDGE) &&
	  (!has_feature(new_hex, RAFT) || 
	   (has_feature(new_hex, RAFT) && order->type != SAIL)) &&
	  random (100) < SEA_MONSTER_LEVEL &&
	  (terrain_adjacent (w, order->dest, WATER) > 0))
	{
	  /* Lizards attacked by Sea Monster */
	  fprintf (fptr, " - %s\n", build_mesg (random (4) + 4, dest, ""));
	  work2 = terrain_adjacent (w, order->dest, WATER);
	  work3 = SEA_MONSTER_STRENGTH_LEVEL * work2;
	  if (combat_sea_monster (w, moving, new_hex->xy, work3))
	    {
	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 12, dest, ""));
	      free(moving);
	      return 0;
	    }
	  else
	    {
	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 8, dest, ""));
	      fprintf (fptr, " - %s Lizards survive at %s.\n",
		       new_show_lizards (moving, temp_string),
		       dest);
	    }
	}
      if (has_feature(new_hex, WHIRLPOOL) &&
	  !has_feature(new_hex, BRIDGE) &&
	  (!has_feature(old_hex, RAFT) ||
	   (has_feature(old_hex, RAFT) && order->type != SAIL)) &&
	  !has_feature(new_hex, RAFT))
	{
	  fprintf (fptr, " - Lizards have moved into Whirlpool at %s.\n",
		   dest);
	  work3 = (int)((double) sum * WHIRLPOOL_KILL_LEVEL);
	  if (work3 < 1) work3 = 1;
	  fprintf (fptr, " - %d Lizards are sucked down to their deaths in Whirlpool.\n",
		   work3);
	  sum = kill_lizards (moving, work3);
	  
	  if (sum == 0)
	    {
	      fprintf (fptr, " - NO Lizards resist the Whirlpool.\n");
	      return 0;
	    }
	  else
	    fprintf (fptr, " - %s Lizards resist the Whirlpool.\n",
	       new_show_lizards (moving, temp_string));
	}
      break;
    case SWAMP:
      if (random (100) < SWAMP_MIRE_LEVEL)
	{
	  /* Lizards mired in Swamp */
	  fprintf (fptr, " - Lizards MIRED in Swamp at %s for next turn.\n",
		   dest);
	  moving->f.band.immobile = 2;
	}
      break;
#ifdef X
    case CURSED:
      fprintf (fptr, " - Lizards have moved into CURSED hex at %s.\n",
	       dest);
      work3 = (float) (red + green + grey + black + yellow) *
	CURSED_KILL_LEVEL;
      if (work3 < 1)
	work3 = 1;
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
	  put_hex (world, order->x_hex, order->y_hex, &old_hex);
	  return;
	}
      else
	fprintf (fptr, " - %s Lizards resist the CURSE.\n",
	       show_lizards (red, green, grey, black, yellow, temp_string));
      break;
#endif
    default:
      break;
    }
  if (order->type == SAIL)
    {
      if ((raft = has_feature(new_hex, RAFT)))
	{
	  fprintf (fptr, " - CRASH! Your Lizards ram and sink the raft at %s.\n", dest);
	  destroy_feature(w, raft);
	  raft = has_feature(old_hex, RAFT);
	  move_feature(w, raft, new_hex);
	}
    }
  hex_attacked_or_moved_into (w, new_hex);
  move_band (w, new_hex, moving);

  {
    feature_t *f;
    if ((f = has_feature(get_hex(w, order->source), BAND)))
      debug(w, "Source hex %s %s", source, new_show_lizards (f, temp_string));
    if ((f = has_feature(get_hex(w, order->dest), BAND)))
      debug(w, "Dest hex %s %s", dest, new_show_lizards (f, temp_string));
  }
  /* can increase cp's here without fear of screwing up cp's for
     merging bands as lizards can only gain cp by moving into hex
     that USED to contain ENEMY lizards! */
#ifdef X
  if (combat_experience)
    {
      combat_experience = LIZ_FALSE;
      inc_combat_points (band);
    }
#endif
  if (new_hex->terrain == FERTILE)
    {
      fprintf (fptr, " - Lizards eat from fertile hex %s.\n", dest);
      band->f.band.hunger = 0;
   }

#ifdef X
  order->x_hex = x;
  order->y_hex = y;
  get_hex (world, order->x_hex, order->y_hex, &old_hex);
  if (old_order.x_hex == order->x_hex && old_order.y_hex == order->y_hex)
    {
      /* order has moved back on self, into starting hex */
      if (old_order.order_type == MOVE || old_order.order_type == SAIL)
	{
	  /* do not allow any further moves, sails or splits FROM this hex as
	     all lizards have already moved back into hex */
	  for (work = 0; work < world->num_orders; work++)
	    if (world->order_list[work] != order &&
		world->order_list[work]->x_hex == old_order.x_hex &&
		world->order_list[work]->y_hex == old_order.y_hex &&
		world->order_list[work]->player == old_order.player &&
		(world->order_list[work]->order_type == SPLIT ||
		 world->order_list[work]->order_type == MOVE ||
		 world->order_list[work]->order_type == SAIL))
	      {
		open_player_mesg (world->order_list[work]->player, &fptr);
		xy_to_string (world->order_list[work]->x_hex, world->order_list[work]->y_hex,
			      work_string);
		switch (world->order_list[work]->order_type)
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
		if (world->order_list[work]->moves[1] != 0)
		  fprintf (fptr, "%s %s) ", dir[world->order_list[work]->moves[0]],
			   dir[world->order_list[work]->moves[1]]);
		else
		  fprintf (fptr, "%s) ", dir[world->order_list[work]->moves[0]]);
		fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
		/* fclose(fptr); */
		world->order_list[work]->order_type = INACTIVE;
	      }
	}
      else if (old_order.order_type == SPLIT)
	{
	  /* split moved back on self, do not allow any further splits that
	     would exceed the UNMOVED lizards in hex, and adjust any moves
	     or sails downwards to NOT include the moved lizards */
	  get_hex (world, old_order.x_hex, old_order.y_hex, &old_hex);
	  for (work = 0; work < world->num_orders; work++)
	    if (world->order_list[work] != order &&
		world->order_list[work]->x_hex == old_order.x_hex &&
		world->order_list[work]->y_hex == old_order.y_hex &&
		world->order_list[work]->player == old_order.player)
	      if (world->order_list[work]->order_type == SPLIT)
		{
		  if (world->order_list[work]->red_lizards > (old_hex.red_lizards -
							order->red_lizards))
		    world->order_list[work]->red_lizards = (old_hex.red_lizards -
							order->red_lizards);
		  if (world->order_list[work]->green_lizards > (old_hex.green_lizards -
						      order->green_lizards))
		    world->order_list[work]->green_lizards = (old_hex.green_lizards -
						      order->green_lizards);
		  if (world->order_list[work]->grey_lizards > (old_hex.grey_lizards -
						       order->grey_lizards))
		    world->order_list[work]->grey_lizards = (old_hex.grey_lizards -
						       order->grey_lizards);
		  if (world->order_list[work]->black_lizards > (old_hex.black_lizards -
						      order->black_lizards))
		    world->order_list[work]->black_lizards = (old_hex.black_lizards -
						      order->black_lizards);
		  if (world->order_list[work]->yellow_lizards > (old_hex.yellow_lizards -
						     order->yellow_lizards))
		    world->order_list[work]->yellow_lizards = (old_hex.yellow_lizards -
						     order->yellow_lizards);
		  if (world->order_list[work]->red_lizards + world->order_list[work]->green_lizards +
		      world->order_list[work]->grey_lizards + world->order_list[work]->black_lizards +
		      world->order_list[work]->yellow_lizards == 0)
		    {
		      open_player_mesg (world->order_list[work]->player, &fptr);
		      xy_to_string (world->order_list[work]->x_hex, world->order_list[work]->y_hex,
				    work_string);
		      fprintf (fptr, "\n - Split %s (", work_string);
		      if (world->order_list[work]->moves[1] != 0)
			fprintf (fptr, "%s %s) ", dir[world->order_list[work]->moves[0]],
				 dir[world->order_list[work]->moves[1]]);
		      else
			fprintf (fptr, "%s) ", dir[world->order_list[work]->moves[0]]);
		      if (lizards_in_hex (&old_hex) > 0)
			fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
		      else
			fprintf (fptr, "order ignored. NO Lizards in hex.\n");
		      /* fclose(fptr); */
		      world->order_list[work]->order_type = INACTIVE;
		    }
		}
	      else
		{
		  if (world->order_list[work]->order_type == MOVE ||
		      world->order_list[work]->order_type == SAIL)
		    {
		      /* adjust moves and sails downwards to NOT include lizards that
		         have already moved by split */
		      if (world->order_list[work]->red_lizards > (old_hex.red_lizards -
							order->red_lizards))
			world->order_list[work]->red_lizards = (old_hex.red_lizards -
							order->red_lizards);
		      if (world->order_list[work]->green_lizards > (old_hex.green_lizards -
						      order->green_lizards))
			world->order_list[work]->green_lizards = (old_hex.green_lizards -
						      order->green_lizards);
		      if (world->order_list[work]->grey_lizards > (old_hex.grey_lizards -
						       order->grey_lizards))
			world->order_list[work]->grey_lizards = (old_hex.grey_lizards -
						       order->grey_lizards);
		      if (world->order_list[work]->black_lizards > (old_hex.black_lizards -
						      order->black_lizards))
			world->order_list[work]->black_lizards = (old_hex.black_lizards -
						      order->black_lizards);
		      if (world->order_list[work]->yellow_lizards > (old_hex.yellow_lizards -
						     order->yellow_lizards))
			world->order_list[work]->yellow_lizards = (old_hex.yellow_lizards -
						     order->yellow_lizards);
		      if (world->order_list[work]->red_lizards + world->order_list[work]->green_lizards +
			  world->order_list[work]->grey_lizards + world->order_list[work]->black_lizards +
			  world->order_list[work]->yellow_lizards == 0)
			{
			  open_player_mesg (world->order_list[work]->player, &fptr);
			  xy_to_string (world->order_list[work]->x_hex, world->order_list[work]->y_hex,
					work_string);
			  if (world->order_list[work]->order_type == MOVE)
			    fprintf (fptr, "\n - Move %s (", work_string);
			  else
			    fprintf (fptr, "\n - Sail %s (", work_string);
			  if (world->order_list[work]->moves[1] != 0)
			    fprintf (fptr, "%s %s) ", dir[world->order_list[work]->moves[0]],
				     dir[world->order_list[work]->moves[1]]);
			  else
			    fprintf (fptr, "%s) ", dir[world->order_list[work]->moves[0]]);
			  if (lizards_in_hex (&old_hex) > 0)
			    fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
			  else
			    fprintf (fptr, "order ignored. NO Lizards in hex.\n");
			  /* fclose(fptr); */
			  world->order_list[work]->order_type = INACTIVE;
			}
		    }
		}
	  /* adjust any further split from hex to compensate for moved lizards */
	  get_hex (world, old_order.x_hex, old_order.y_hex, &old_hex);
	  for (work = 0; work < world->num_orders; work++)
	    if (world->order_list[work] != order &&
		world->order_list[work]->x_hex == old_order.x_hex &&
		world->order_list[work]->y_hex == old_order.y_hex &&
		world->order_list[work]->player == old_order.player &&
		world->order_list[work]->order_type == SPLIT)
	      {
		if (world->order_list[work]->red_lizards > old_hex.red_lizards)
		  world->order_list[work]->red_lizards = old_hex.red_lizards;
		if (world->order_list[work]->green_lizards > old_hex.green_lizards)
		  world->order_list[work]->green_lizards = old_hex.green_lizards;
		if (world->order_list[work]->grey_lizards > old_hex.grey_lizards)
		  world->order_list[work]->grey_lizards = old_hex.grey_lizards;
		if (world->order_list[work]->black_lizards > old_hex.black_lizards)
		  world->order_list[work]->black_lizards = old_hex.black_lizards;
		if (world->order_list[work]->yellow_lizards > old_hex.yellow_lizards)
		  world->order_list[work]->yellow_lizards = old_hex.yellow_lizards;
		if (world->order_list[work]->red_lizards + world->order_list[work]->green_lizards +
		    world->order_list[work]->grey_lizards + world->order_list[work]->black_lizards +
		    world->order_list[work]->yellow_lizards == 0)
		  {
		    open_player_mesg (world->order_list[work]->player, &fptr);
		    xy_to_string (world->order_list[work]->x_hex, world->order_list[work]->y_hex,
				  work_string);
		    fprintf (fptr, "\n - Split %s (", work_string);
		    if (world->order_list[work]->moves[1] != 0)
		      fprintf (fptr, "%s %s) ", dir[world->order_list[work]->moves[0]],
			       dir[world->order_list[work]->moves[1]]);
		    else
		      fprintf (fptr, "%s) ", dir[world->order_list[work]->moves[0]]);
		    if (lizards_in_hex (&old_hex) > 0)
		      fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
		    else
		      fprintf (fptr, "order ignored. NO Lizards in hex.\n");
		    /* fclose(fptr); */
		    world->order_list[work]->order_type = INACTIVE;
		  }
	      }
	}
    }
#endif
  return 1;
}
