#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include <glib.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- == operator
--
---------------------------------------------------------------------- */
int order_t::operator==(order_t &o)
{
  return ((player == o.player) && (src->xy == o.src->xy) && (type == o.type));
}


/* ----------------------------------------------------------------------
--
-- movement
--
---------------------------------------------------------------------- */

order_movement_t::order_movement_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t d) : order_t(t, player, src, dst, d)
{
}

int order_movement_t::movement()
{
  player_t *p1 = turn->world->get_player(player);
  player_t *p2 = turn->world->get_player(dst->owner);

  int combat_experience = 0;

  /* --------------------
     The move order is by far the most complex since it involves making sure chain move orders don't happen and resolving
     combat.
     -------------------- */

  /* --------------------
     Check hex is still owned by player who requested the order.
     We know they were since that it checked when the move order was placed on the order queue.
     -------------------- */

  if (src->owner != player)
    {
      p1->mesg(2, " - MOVE order %s %s failed. You no longer owner the hex.", src->title, (char*)dirs);
      return 0;
    }

  /* --------------------
     b0 - the original band in the hex
     b1 - the moving band
     b2 - the defenders band
     b3 - the moved band
     -------------------- */

  band_t *b0 = (band_t*)src->has_feature(BAND);

  if (b0 == NULL)
    {
      p1->mesg(2, " - %s failed. No lizards in the hex.\n", (char*)(*this), (char*)(*src), (char*)dirs);
      return 0;
    }

  /* --------------------
     Take the minimum of the original band and what is currently in the hex
     -------------------- */

  assert(band != NULL);

  band->min(*b0);

  if (band->size() == 0)
    {
      p1->mesg(2, " - %s failed. No lizards available.\n", (char*)(*this), (char*)(*src), (char*)dirs);
      return 0;
    }

  /* --------------------
     Check mired
     -------------------- */

  if (b0->immobile)
    {
      p1->mesg(2, " - MOVE order %s %s failed.  Lizards are %s this turn.\n", (char*)(*src), (char*)dirs,
	       b0->immobile > 2 ? "FROZEN" : "MIRED in Swamp");
      return 0;
    }

  /* --------------------
     Create new moving band and reduce size of original
     -------------------- */

  band_t *b1 = new band_t(*band);

  hex_t *s, *t;
  s = src;
  for (int k = 0; k < 2 && dirs[k].okay() && !b1->immobile; k++)
    {
      dir_t dir = dirs[k];
      t = s->adjacent(dir);

  /* --------------------
     Remove band if all lizards moving
     -------------------- */

      b0 = (band_t*)s->has_feature(BAND);
      *b0 -= *b1;

      if (b0->size() == 0)
	{
	  s->del_feature(b0);
	  assert(s->has_feature(BAND) == NULL);
	}

      p1->mesg(2, " - %s lizards at %s move %s (to %s).\n", b1->describe(), (char*)(*s), (char*)dir, (char*)(*t));

      band_t *b2 = (band_t*)t->has_feature(BAND);

      if ((t->owner == player) && b2 && (b1->size() + b2->size() > MAX_LIZARDS_IN_HEX))
	{
	  p1->mesg(2, " - But %s would contain more than %d lizards.  Move ignored.\n", (char*)t, MAX_LIZARDS_IN_HEX);
	  return 0;
	}

      /* --------------------
	 Check if attacked hex has den
	 -------------------- */
      
      den_t *den = (den_t*)t->has_feature(DEN);

      /* --------------------
	 Attacking enemy hex
	 -------------------- */
      
      if (t->owner != player)
	{
	  if (b2 && (b2->size() > 0))
	    {
	      
	  /* --------------------
	     Combat
	     -------------------- */

	      if (den)
		{
		  if (t->owner == 0)
		    p1->mesg(2, " - Lizards attack %d militia Lizards in free Den at %s.\n", b2->size(), (char*)(*t));
		  else
		    p1->mesg(2, " - Lizards attack %d Lizards in %s Den of '%s' Clan at %s.\n", b2->size(), (char*)(*den), p2->clan, (char*)t);
		}
	      else
		{
		  p1->mesg(2, " - Lizards attack %d '%s' Clan Lizards at %s.\n", b2->size(), p2->clan, (char*)(*t));
		}
	      
	      if (den)
		p2->mesg(2, " - %s Den at %s attacked by %d '%s' Clan Lizards.\n", (char*)den, (char*)(*t), b1->size(), p1->clan);
	      else
		p2->mesg(2, " - %d Lizards at %s attacked by %d '%s' Clan Lizards.\n", b2->size(), (char*)(*t), p1->clan);
	      if (b2->immobile > 0)
		{
		  if (b2->immobile > 2)
		    p2->mesg(2, " - Lizards at %s are FROZEN and cannot flee from combat.\n", (char*)(*t));
		  else
		    p2->mesg(2, " - Lizards at %s are MIRED and cannot flee from combat.\n", (char*)(*t));
		}

	      turn->hex_moved_into(t);
      
	      /* --------------------
		 Run combat
		 -------------------- */
	  
	      band_t *attack = new band_t(*b1);
	      band_t *defend = new band_t(*b2);
	      
	      int result = turn->combat(attack, defend, s, t, b2->immobile ? 0 : 1);
	  
	      hex_t* flee = NULL;
	      int capture_level = MIN_CAPTURE_PERCENT + (b1->combat * CP_CAPTURE_MOD_LEVEL);
	      int enemy_capture_level = MIN_CAPTURE_PERCENT + (b2->combat * CP_CAPTURE_MOD_LEVEL);
	      int num_captured;
	      band_t wounded;
	  
	      /* --------------------
		 If defenders try to flee and cannot then run a second round of combat.
		 Since the defenders cannot flee we cannot get that result again.
		 -------------------- */
	  
	      if (result == COMBAT_DEFEND_FLEE)
		{
		  int dir = misc_t::uniform(DIRECTIONS);
		  
		  int work4 = -1;
		  
		  for (int work3 = 0; work3 < DIRECTIONS; work3 ++)
		    {
		      flee = t->adjacent(dir_t((dir + work3) % DIRECTIONS));
		      if (!flee->has_feature(BAND) && !flee->has_feature(DEN))
			work4 = 0;
		    }
		  if (work4 == -1)
		    {
		      p1->mesg(2, " - Enemy Lizards cannot flee. Your Lizards ATTACK.\n");
		      p2->mesg(2, " - Your Lizards cannot flee. They are surrounded by other bands.\n");
		      result = turn->combat(attack, defend, s, t, 0);
		    }
		  else
		    {
		      /* Calculate wounded defenders */
		      wounded = *b2 - *defend;
		      num_captured = ((double)wounded.size() / 100.00) * (double)capture_level;
		      attack->capture(wounded, num_captured);
		      
		      p2->mesg(2, " - Your Lizards at %s flee from combat to %s.\n", flee->title);
		      p2->mesg(2, " - %s Lizards fled.\n", (char*)defend);
		      
		      /* --------------------
			 Remove old defender band and add to flee hex
			 -------------------- */
		      
		      t->del_feature(b2);
		      flee->add_feature(defend);
		      flee->owner = t->owner;
		      
		      if (flee->has_feature(WHIRLPOOL) && !flee->has_feature(BRIDGE) && !flee->has_feature(RAFT))
			{
			  p2->mesg(2, " - Lizards have fled into Whirlpool at %s!\n", (char*)flee);
			  
			  int work3 = defend->size() * WHIRLPOOL_KILL_LEVEL;
			  if (work3 < 1) work3 = 1;
			  
			  p2->mesg(2, " - %d Lizards are sucked down to their deaths in Whirlpool.\n", work3);
			  defend->kill(work3);
			  
			  if (defend->size() == 0)
			    {
			      p2->mesg(2, " - NO Lizards resist the Whirlpool.\n");
			      flee->del_feature(defend);
			    }
			  else
			    {
			      p2->mesg(2, " - %s Lizards resist the Whirlpool.\n", (char*)defend);
			    }
			}
		      p1->mesg(2, " - Your Lizards routed enemy. ");
		      
		      combat_experience = 1;
		  
		      if (attack->combat + 1 > MAX_COMBAT_POINTS)
			p1->mesg(2, "They are still %s warriors.\n", misc_t::combat_desc(attack->combat));
		      else
			p1->mesg(2, "They are now %s warriors.\n", misc_t::combat_desc(attack->combat + 1));
		      
		      if (num_captured)
			p1->mesg(2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n", (char*)wounded);
		    }
		}
	      switch (result)
		{
		  /* --------------------
		     Defenders flee
		     -------------------- */
	      
		case COMBAT_DEFEND_FLEE:
		  break;
	      
		  /* --------------------
		     Attackers flee
		     -------------------- */
		  
		case COMBAT_ATTACK_FLEE:
		  p1->mesg(2, " - Your Lizards flee from combat and remain at %s.\n", (char*)(*src));
		  
		  /* ENEMY capture any killed lizards but only if ENEMY are NOT in a Den or Home Den */
		  
		  wounded = *b1 - *attack;
		  
		  if (!den)
		    {
		      num_captured = ((double)wounded.size()) / 100.00 * (double)enemy_capture_level;
		      defend->capture(wounded, num_captured);
		    }
		  else
		    {
		      num_captured = 0;
		    }
		  p2->mesg(2, " - Your Lizards repelled attack. ");
		  b2->inc_combat();
		  p2->mesg(2, "They are now %s warriors.\n", misc_t::combat_desc(b2->combat));
		  if (num_captured)
		    p2->mesg(2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n", (char*)wounded);
		  
		  s->add_feature(attack);
		  
		  t->del_feature(b2);
		  t->add_feature(defend);
		  return 1;
		  break;
		  
		  /* --------------------
		     Both sides dead
		     -------------------- */
		  
		case COMBAT_BOTH_DEAD:
		  
		  p1->mesg(2, " - Both sides fight to the death in %s.\n", (char*)(*dst));
		  p2->mesg(2, " - Both sides fight to the death in %s.\n", (char*)(*dst));
		  
		  dst->del_feature(b2);
		  delete b1;
		  delete attack;
		  delete defend;
		  return 0;
		  
		  /* --------------------
		     Attackers all dead
		     -------------------- */
		  
		case COMBAT_ATTACK_DEAD:
		  
		  /* Capture attack dead if not den militia */
		  
		  if (!den)
		    {
		      wounded = *b1 - *attack;
		      num_captured = (double)(wounded.size()) / 100.00 * (double)enemy_capture_level;
		      defend->capture(wounded, num_captured);
		    }
		  else
		    {
		      num_captured = 0;
		    }
		  // fprintf (fptr2, " - %s ", build_mesg (random (4) + 20, work_string, ""));
		  
		  defend->inc_combat();
		  p2->mesg(2, "They are now %s warriors.\n", misc_t::combat_desc(defend->combat));
		  if (num_captured)
		    {
		      p2->mesg(2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n", (char*)wounded);
		    }
		  t->del_feature(b2);
		  t->add_feature(defend);
		  delete attack;
		  return 0;
		  break;
		  
		case COMBAT_DEFEND_DEAD:
		  
		  if (!den)
		    {
		      wounded = *b2 - *defend;
		      num_captured = (double)wounded.size() * capture_level / 100.0;
		      attack->capture(wounded, num_captured);
		    }
		  else
		    {
		      num_captured = 0;
		    }
	      //	  fprintf (fptr, " - %s ", build_mesg (random (4) + 20, work_string, ""));
		  
		  combat_experience = 1;
	      
		  if (attack->combat + 1 > MAX_COMBAT_POINTS)
		    p1->mesg(2, "They are still %s warriors.\n", misc_t::combat_desc(attack->combat));
		  else
		    p1->mesg(2, "They are now %s warriors.\n", misc_t::combat_desc(attack->combat + 1));
		  
		  if (num_captured)
		    p1->mesg(2, " - Your Lizards CAPTURE %s wounded enemy Lizards.\n", (char*)wounded);
		  
		  if (den)
		    {
		      p1->mesg(2, " - Your Lizards captured %s Den at %s.\n", (char*)(*den), (char*)(*dst));
		      den->timebomb = 0;
		      if (den->home)
			{
			  p1->home_dens++;
			  p2->home_dens--;
			}
		      p2->mesg(2, " - Your %s Den at %s is captured by '%s' Clan.\n", (char*)(*den), (char*)(*dst), p1->clan);
		    }
		  delete b1;
		  b1 = attack;
		  break;
		}
	    }
	  else
	    {
	      /* No defenders */
	      if (den)
		{
		  den->timebomb = 0;
		  if (t->owner > 0)
		    {
		      p1->mesg(2, " - Lizards capture '%s' Clan %s Den at %s without a struggle.\n", p2->clan, (char*)(*den), dst->title);
		    }
		  else
		    {
		      p1->mesg(2, " - Lizards capture free %s Den at %s without a struggle.\n", (char*)(*den), dst->title);
		    }
		  if (den->home)
		    p1->home_dens++;
		}
	    }
	}
      
      /* --------------------
	 Lets now actually move the band
	 -------------------- */

      if (t->terrain == WATER)
	{
	  if (!t->has_feature(BRIDGE) && (!s->has_feature(RAFT) && type != SAIL) && (!t->has_feature(RAFT))
	      && misc_t::uniform(100) < SEA_MONSTER_LEVEL && t->adjacent(WATER) > 0)
	    {
		/* Lizards attacked by Sea Monster */
	  //	  fprintf (fptr, " - %s\n", build_mesg (random (4) + 4, work_string, ""));

	      int m = t->terrain_adjacent(WATER);
	      int n = SEA_MONSTER_STRENGTH_LEVEL * m;
	      int r = turn->combat_sea_monster(b1, t, n);

	      if (r == COMBAT_DEFEND_DEAD)
		{
	      //	      fprintf (fptr, " - %s\n", build_mesg (random (4) + 8, work_string, ""));
		  p1->mesg(2, " - %s Lizards survive at %s.\n", (char*)(*b1), (char*)(*t));
		}
	      else
		{
		  p1->mesg(2, " - All your lizards in %s are killed by a sea monster.\n", (char*)(*t));
		  return 0;
		}
	    }
	  if (dst->has_feature(WHIRLPOOL))
	    {
	      if (!t->has_feature(BRIDGE) && (s->has_feature(RAFT) && type != SAIL) && (!t->has_feature(RAFT)))
		{
		  p1->mesg(2, " - Lizards have moved into Whirlpool at %s.\n", (char*)(*t));
		  int m = b1->size() * WHIRLPOOL_KILL_LEVEL;
		  if (m < 1) m = 1;
		  p1->mesg(2, " - %d Lizards are sucked down to their deaths in Whirlpool.\n", m);
		  b1->kill(m);

		  if (b1->size() == 0)
		    {
		      p1->mesg(2, " - NO Lizards resist the Whirlpool.\n");
		      // Need to delete stray bands
		      return 0;
		    }
		}
	      else
		p1->mesg(2, " - %s Lizards resist the Whirlpool.\n", (char*)(*b1));
	    }
	}
      else
	{
	  if (t->has_feature(SWAMP))
	    {
	      if (misc_t::uniform(100) < SWAMP_MIRE_LEVEL)
		{
		  /* Lizards mired in Swamp */
		  p1->mesg(2, " - Lizards MIRED in Swamp at %s for next turn.\n", (char*)(*t));
		  b1->immobile = 2;
		}
	      //		new_hex.lizards_immobile = 2;
	      //		if (work == 0)
	      //		  order->moves [1] = 0;
	    }
	  if (dst->has_feature(CURSED))
	    {
	      p1->mesg(2, " - Lizards have moved into CURSED hex at %s.\n",  (char*)(*t));
	      int m = b1->size() * CURSED_KILL_LEVEL;
	      if (m < 1) m = 1;
	      p1->mesg(2, " - %d Lizards choke and collapse, dead.\n", m);
	      b1->kill(m);
	      
	      if (b1->size() == 0)
		{
		  p1->mesg(2, " - NO Lizards resist the CURSE.\n");
		  // Delete stray bands
		  return 0;
		}
	      else
		{
		  p1->mesg(2, " - %s Lizards resist the CURSE.\n", (char*)(*b1));
		}
	    }
	}
      
  #ifdef Sail
      if (order->order_type == SAIL)
	{
	  if (new_hex.raft_here > 0)
	    fprintf (fptr, " - CRASH! Your Lizards ram and sink the raft at %s.\n", work_string);
	  
	  new_hex.raft_here = old_hex.raft_here;
	  old_hex.raft_here = 0;
	}
#endif
      
      turn->hex_moved_into(t);
      if (b2)
	{
	  *b1 += *b2;
	  t->del_feature(b2);
	}

      t->add_feature(new band_t(*b1));
      t->owner = player;
      
	/* can increase cp's here without fear of screwing up cp's for
	   merging bands as lizards can only gain cp by moving into hex
	   that USED to contain ENEMY lizards! */
  
      if (combat_experience)
	{
	  b1->inc_combat();
	}
  
      if (t->has_feature(FERTILE))
	{
	  p1->mesg(2, " - Lizards eat from fertile hex %s.\n", t->title);
	  b1->hunger = 0;
	}
      s = t;
    }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- movement
--
---------------------------------------------------------------------- */

order_move_t::order_move_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t d) : order_movement_t(t, player, src, dst, d)
{
  type = MOVE;
  phase = MOVEMENT;
};

void order_move_t::show()
{
  turn->world->get_player(player)->mesg(2, " - MOVE from %s (%s) to %s issued.\n", src->title, (char*)dirs, dst->title);
}

int order_move_t::execute()
{
  return movement();
}

char *order_move_t::describe()
{
  if (text) return text;
  text = g_strdup_printf("MOVE order from %s to %s (%s)",
			 (char*)(*src),
			 (char*)(*dst),
			 (char*)dirs);
  return text;
}

/* ----------------------------------------------------------------------
--
-- Split
--
---------------------------------------------------------------------- */
order_split_t::order_split_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dirs_t d) : order_movement_t(t, player, src, dst, d)
{
  type = SPLIT;
  phase = MOVEMENT;
};

void order_split_t::show()
{
  turn->world->get_player(player)->mesg(2, "Split order issued.\n");
}  

int order_split_t::execute()
{
  return movement();
}  

char *order_split_t::describe()
{
  if (text) return text;
  text = g_strdup_printf("SPLIT (%s) order from %s to %s (%s)",
			 (char*)(*band),
			 (char*)(*src),
			 (char*)(*dst),
			 (char*)dirs);
  return text;
}

/* ----------------------------------------------------------------------
--
-- Sneak
--
---------------------------------------------------------------------- */
order_sneak_t::order_sneak_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dirs_t d) : order_movement_t(t, player, src, dst, d)
{
  type = SNEAK;
  phase = MOVEMENT;
};

void order_sneak_t::show()
{
  turn->world->get_player(player)->mesg(2, "Sneak order issued.\n");
}  

int order_sneak_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Sneak order executed.\n");
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Sail
--
---------------------------------------------------------------------- */
order_sail_t::order_sail_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dirs_t d) : order_movement_t(t, player, src, dst, d)
{
  type = SAIL;
  phase = MOVEMENT;
};

void order_sail_t::show()
{
  turn->world->get_player(player)->mesg(2, "Sail order issued.\n");
}  

int order_sail_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Sail order executed.\n");
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Recruit
--
---------------------------------------------------------------------- */
order_recruit_t::order_recruit_t(class turn_t *t, int player, hex_t *src) : order_t(t, player, src)
{
  type = RECRUIT;
  phase = DEVELOPMENT;
};

void order_recruit_t::show()
{
  turn->world->get_player(player)->mesg(2, " - RECRUIT %s order issued.\n", (char*)(*src));
}  

char* order_recruit_t::describe()
{
  if (text) return text;
  text = g_strdup_printf("RECRUIT order %s", (char*)(*src));
  return text;
}  


int order_recruit_t::execute()
{
  player_t *p1 = turn->world->get_player(player);

  den_t *den = (den_t*)src->has_feature(DEN);
  
  if (den == NULL)
    {
      if (src->has_feature(RUIN))
	p1->mesg(2, " - Attempted to Recruit from Den at %s. Alas, it is now a Ruin.\n", (char*)(*src));
      else
	p1->mesg(2, " - Attempted to Recruit from Den at %s that isn't yours or doesn't exist.\n", (char*)(*src));
      return 0;
    }
  if (src->owner != player)
    {
      p1->mesg(2, " - Attempted to Recruit from Den at %s that isn't yours or doesn't exist.\n", (char*)(*src));
      return 0;
    }

  if (den->colour == UNSETTLED)
    {
      p1->mesg(2, " - Attempted to recruit from an un-settled Den at %s.\n", (char*)(*src));
      return 0;
    }

  if (p1->home_dens == 0)
    {
      p1->mesg(2, " - Attempted to recruit from %s Den at %s. Alas, your Clan no longer control a Home Den.\n",
	       (char*)(*den), (char*)(*src));
      p1->mesg(2, " - None of the Den Lizards feel compelled to join your Clan.\n");
      return 0;
    }

  band_t* band = (band_t*)src->has_feature(BAND);

  if (band == NULL)
    band = new band_t();

  int sz = band->size();
  if (sz >= MAX_LIZARDS_IN_HEX)
    {
      p1->mesg(2, " - Attempted to recruit from %s Den at %s but hex already contains 300 of your Lizards.\n",
	       (char*)(*den), (char*)(*src));
      return 0;
    }

  p1->mesg(2, " - Recruiting from %s Den at %s.\n", (char*)(*den), (char*)(*src));

  int recsz = (double)den->pop * DEN_RECRUIT_LEVEL;

  if (sz + recsz > MAX_LIZARDS_IN_HEX)
    recsz = MAX_LIZARDS_IN_HEX - sz;

  /*
  world_hex.combat_points =
    average_combat_points (lizards_in_hex (&world_hex), world_hex.combat_points,
			   work, 0);

  world_hex.turns_hungry =
    average_hunger_points (lizards_in_hex (&world_hex), world_hex.turns_hungry,
			   work, 0);
  */

  band->colour[den->colour] += recsz;
  den->pop -= recsz;
  band_t tmp;
  tmp.colour[den->colour] = recsz;
  src->add_feature(band);

  p1->mesg(2, " - %s lizards recruited from Den", (char*)tmp);
  p1->mesg(2, " leaving %d Den Lizards at %s.\n", den->pop, (char*)(*src));

  for (int i = 0; i < turn->num_orders; i++)
    if ((*turn->order_list[i]) == *this)
      {
	p1->mesg(2, " - Next Recruit Order at %s ignored.\n", (char*)(*src));
	turn->order_list[i]->type = INACTIVE;
      }
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Build_Fertile
--
---------------------------------------------------------------------- */
order_build_fertile_t::order_build_fertile_t(class turn_t *t, int player, hex_t *src) : order_t(t, player, src)
{
  type = BUILD_FERTILE;
  phase = DEVELOPMENT;
};

void order_build_fertile_t::show()
{
  turn->world->get_player(player)->mesg(2, "Build Fertile order issued.\n");
}  

int order_build_fertile_t::execute()
{
  band_t *band = (band_t*)src->has_feature(BAND);
  player_t *p = turn->world->get_player(player);
  if (!band || (src->owner != player))
  {
    p->mesg(2, " - Plant Fertile Hex Order at %s ignored as hex contains none of your Lizards!\n", (char*)(*src));
    return 0;
  }

  if (band->colour[GREEN] < BUILD_FERTILE_LEVEL)
  {
    if (band->colour[GREEN] == 0)
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as hex contains no Green Lizards!\n", (char*)(*src));
    else
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as hex contains only %d Green Lizards!\n", (char*)(*src), band->colour[GREEN]);
    return 0;
  }

  if (src->has_feature(DEN))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as hex occupied by Den!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(RUIN))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as hex occupied by Ruin\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(PEAK))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as Green Lizards cannot plant on Peak!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(VOLCANO))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as Green Lizards cannot plant on Volcano!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(TEMPLE))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as Green Lizards will not desecrate Temple here!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(FERTILE))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as this hex is ALREADY Fertile!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(CURSED))
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as hex is CURSED and nothing will grow there.\n", (char*)(*src));
      return 1;
    }

  if (src->terrain == WATER)
    {
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as Green Lizards cannot plant in Water!\n", (char*)(*src));
      return 0;
    }

  p->mesg(2, " - Fertile hex planted at %s by %d Green Lizards.\n", (char*)(*src), band->colour[GREEN]);

  feature_t *f;
  if ((f = src->has_feature(SCRUB)))
    {
      src->del_feature(f);
    }
  if ((f = src->has_feature(SWAMP)))
    {
      src->del_feature(f);
    }

  src->add_feature(new fertile_t());

  /*
  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	(world->order_list [work]->order_type == BUILD_FERTILE ||
	 world->order_list [work]->order_type == DESTROY_FERTILE ||
	 world->order_list [work]->order_type == BUILD_RAFT ))
    {
      if (world->order_list [work]->order_type == BUILD_FERTILE)
        p->mesg(2, " - NEXT Build Fertile Hex Order at %s ignored, only one Build Fertile Order allowed per hex, per turn.\n",
		 (char*)(*src));
      else
        if (world->order_list [work]->order_type == DESTROY_FERTILE)
          p->mesg(2, " - Destroy Fertile Hex Order at %s ignored, Green lizards exhausted from planting.\n",
		 (char*)(*src));
	else
          p->mesg(2, " - Build Raft Order at %s ignored, Green lizards exhausted from planting.\n",
		 (char*)(*src));

      world->order_list [work]->order_type = INACTIVE;
    }
  */
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Build Bridge
--
---------------------------------------------------------------------- */
order_build_bridge_t::order_build_bridge_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dir_t d) : order_t(t, player, src, dst, d)
{
  type = BUILD_BRIDGE;
  phase = DEVELOPMENT;
};

void order_build_bridge_t::show()
{
  turn->world->get_player(player)->mesg(2, "Build bridge order issued.\n");
}  

int order_build_bridge_t::execute()
{
  player_t *p = turn->world->get_player(player);
  band_t *b = (band_t*)src->has_feature(BAND);

  if (!band || src->owner != player)
    {
      p->mesg(2, " - Build Bridge Order at %s ignored as hex contains none of your Lizards!\n", (char*)(*src));
      return 0;
    }

  int grey = b->colour[GREY];

  if (grey < BUILD_BRIDGE_LEVEL)
    {
      if (grey == 0)
	p->mesg(2, " - Build Bridge Order at %s ignored as hex contains no Grey Lizards!\n", (char*)(*src));
      else
	p->mesg(2, " - Build Bridge Order at %s ignored as hex contains only %d Grey Lizards!\n",(char*)(*src), grey);
      return 0;
    }
  
  if (src->terrain == WATER)
  {
    p->mesg(2, " - Build Bridge Order at %s ignored as Grey Lizards are not on solid ground!\n", (char*)(*src));
    return 0;
  }

  p->mesg(2, " - Surveying hex %s of %s for Bridge.", (char*)(dirs[0]), (char*)(*src));
  if (dst->has_feature(BRIDGE))
    {
      p->mesg(2, " Build Bridge Order ignored as there is already a Bridge there!\n");
      return 0;
    }
  if (dst->has_feature(RAFT))
    {
      p->mesg(2, " Build Bridge Order ignored as there is a Raft there!\n");
      return 0;
    }
  if (dst->has_feature(BAND))
    {
      p->mesg(2, " Build Bridge Order ignored as there are Lizards in hex!\n");
      return 0;
    }
  if (dst->has_feature(DEN))
    {
      p->mesg(2, " Build Bridge Order ignored as hex occupied by Den!\n");
      return 0;
    }
  if (dst->has_feature(RUIN))
    {
      p->mesg(2, " Build Bridge Order ignored as hex occupied by Ruin!\n");
      return 0;
    }
  if (dst->has_feature(SWAMP))
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Swamp!\n");
      return 0;
    }
  if (dst->has_feature(PEAK))
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Peak!\n");
      return 0;
    }
  if (dst->has_feature(VOLCANO))
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Volcano!\n");
      return 0;
    }
  if (dst->has_feature(TEMPLE))
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Volcano!\n");
      return 0;
    }
  if (dst->has_feature(FERTILE))
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Fertile!\n");
      return 0;
    }
  if (dst->has_feature(SCRUB))
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Scrub!\n");
      return 0;
    }
  if (dst->has_feature(CURSED))
    {
      p->mesg(2, " - Build Bridge Order at %s ignored as hex is CURSED.\n", (char*)(*src));
      return 0;
    }
  if (dst->terrain == PLAIN)
    {
      p->mesg(2, " Build Bridge Order ignored as hex is Plain!\n");
      return 0;
    }

  assert(dst->terrain == WATER);

  dir_t ld = dirs[0];
  ld -= 1;
  int no_land = 0;

  for (int i = 0; i < 3; i++)
  {
    hex_t *hex = dst->adjacent(ld);
    if (hex->terrain == WATER)
      no_land++;
    ld += 1;
  }
  if (no_land == 3)
  {
    p->mesg(2, " Build Bridge Order ignored as bridge MUST lead to another LAND hex!\n");
    return 0;
  }
  p->mesg(2, " Grey Lizards build Bridge at %s.\n", (char*)(*src));

  dst->add_feature(new bridge_t());

  /*
  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	(world->order_list [work]->order_type == BUILD_BRIDGE ||
	 world->order_list [work]->order_type == DESTROY_BRIDGE ||
	 world->order_list [work]->order_type == BUILD_DEN))
    {
      xy_to_string (world->order_list [work]->x_hex, world->order_list [work]->y_hex,
	(char*)(*src));

      if (world->order_list [work]->order_type == BUILD_BRIDGE)
        p->mesg(2, " - Next Build Bridge Order at %s ignored, only on Build Bridge Order allowed per hex, per turn.\n",
	       (char*)(*src));
      else
	if (world->order_list [work]->order_type == DESTROY_BRIDGE)
          p->mesg(2, " - Destroy Bridge Order at %s ignored, Grey lizards exhausted from building bridge.\n",
	       (char*)(*src));
	else
          p->mesg(2, " - Build Den Order at %s ignored, Grey lizards exhausted from building bridge.\n",
	       (char*)(*src));

      world->order_list [work]->order_type = INACTIVE;
    }
  */
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Build Raft
--
---------------------------------------------------------------------- */
order_build_raft_t::order_build_raft_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dir_t d) : order_t(t, player, src, dst, d)
{
  type = BUILD_RAFT;
  phase = DEVELOPMENT;
};

void order_build_raft_t::show()
{
  turn->world->get_player(player)->mesg(2, "Build Raft order issued.\n");
}  

int order_build_raft_t::execute()
{
  player_t *p = turn->world->get_player(player);
  band_t *band = (band_t*)src->has_feature(BAND);

  if (!band || src->owner != player)
    {
      p->mesg(2, " - Build Raft Order at %s ignored as hex contains none of your Lizards!\n", (char*)(*src));
      return 0;
    }

  int green = band->colour[GREEN];

  if (green < BUILD_RAFT_LEVEL)
  {
    if (green == 0)
      p->mesg(2, " - Build Raft Order at %s ignored as hex contains no Green Lizards!\n", (char*)(*src));
    else
      p->mesg(2, " - Build Raft Order at %s ignored as hex contains only %d Green Lizards!\n", (char*)(*src), green);
    return 0;
  }
  
  if (!src->has_feature(SCRUB))
    {
      p->mesg(2, " - Build Raft Order at %s ignored as Green Lizards are not in Scrub hex!\n", (char*)(*src));
      return 0;
    }
  
  if (dst->terrain != WATER)
    {
      p->mesg(2, " - Build Raft Order ignored as Green Lizards can only build rafts in WATER!\n");
      return 0;
    }

  if (dst->has_feature(BRIDGE))
  {
    p->mesg(2, " - Build Raft Order ignored as there is a Bridge at %s.\n", (char*)(*src));
    return 0;
  }
  
  if (dst->has_feature(RAFT))
    {
    p->mesg(2, " - Build Raft Order ignored as there is already a Raft at %s.\n", (char*)(*src));
    return 0;
    }
  
  p->mesg(2, " - Green Lizards build Raft at %s from Scrub hex.\n", (char*)(*src));

  raft_t *raft = new raft_t;
  raft->life = misc_t::uniform(RAFT_LIFE) + 5;
  dst->add_feature(raft);

  /*
  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	(world->order_list [work]->order_type == BUILD_RAFT ||
	 world->order_list [work]->order_type == BUILD_FERTILE ||
	 world->order_list [work]->order_type == DESTROY_FERTILE))
    {
      xy_to_string (world->order_list [work]->x_hex, world->order_list [work]->y_hex,
	(char*)(*src));

      if (world->order_list [work]->order_type == BUILD_RAFT)
	p->mesg(2, " - Next Build Raft Order at %s ignored, only allowed one Build Raft Order per hex, per turn.\n",
	       (char*)(*src));
      else
	if (world->order_list [work]->order_type == BUILD_FERTILE)
	  p->mesg(2, " - Build Fertile Hex Order at %s ignored, Green lizards exhausted from building raft.\n",
	    (char*)(*src));
	else
          p->mesg(2, " - Destroy Fertile Hex Order at %s ignored, Green lizards exhausted from building raft.\n",
	    (char*)(*src));

      world->order_list [work]->order_type = INACTIVE;
    }
  */
  turn->world->get_player(player)->mesg(2, "Build Raft order executed.\n");
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Build Den
--
---------------------------------------------------------------------- */
order_build_den_t::order_build_den_t(class turn_t *t, int player, hex_t *src) : order_t(t, player, src)
{
  type = BUILD_DEN;
  phase = DEVELOPMENT;
};

void order_build_den_t::show()
{
  turn->world->get_player(player)->mesg(2, "Build Den order issued.\n");
}  

int order_build_den_t::execute()
{
  den_t *den = NULL;
  feature_t* f;
  band_t* band = (band_t*)src->has_feature(BAND);
  player_t *p = turn->world->get_player(player);

  if (!band || (player != src->owner))
    {
      p->mesg(2, " - Build Den Order at %s ignored as hex contains none of your Lizards!\n", (char*)(*src));
      return 0;
    }
  
  if (band->colour[GREY] < BUILD_DEN_LEVEL)
  {
    if (band->colour[GREY] == 0)
      p->mesg(2, " - Build Den Order at %s ignored as hex contains no Grey Lizards!\n", (char*)(*src));
    else
      p->mesg(2, " - Build Den Order at %s ignored as hex contains only %d Grey Lizards!\n", (char*)(*src), band->colour[GREY]);
    return 0;
  }

  if (src->has_feature(DEN))
    {
      p->mesg(2, " - Build Den Order at %s ignored as hex ALREADY occupied by Den!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(PEAK))
    {
      p->mesg(2, " - Build Den Order at %s ignored as Grey Lizards cannot build on Peak!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(VOLCANO))
    {
      p->mesg(2, " - Build Den Order at %s ignored as Grey Lizards cannot build on Volcano!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(TEMPLE))
    {
      p->mesg(2, " - Build Den Order at %s ignored as Grey Lizards will not desecrate Temple here!\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(CURSED))
    {
      p->mesg(2, " - Build Den Order at %s ignored as hex is CURSED and nothing will live there.\n", (char*)(*src));
      return 0;
    }
  if (src->has_feature(SWAMP))
    {
      p->mesg(2, " - Build Den Order at %s ignored as Grey Lizards cannot build in Swamp!\n", (char*)(*src));
      return 0;
    }
  if (src->terrain == WATER)
    {
      p->mesg(2, " - Build Den Order at %s ignored as Grey Lizards cannot build in Water!\n", (char*)(*src));
      return 0;
    }
  if ((f = src->has_feature(RUIN)))
    {
      src->del_feature(f);
      p->mesg(2, " - The %d Grey Lizards rebuild the Ruin at %s into a Den. Den is presently un-populated.\n", band->colour[GREY], (char*)(*src));
      den = new den_t();
      src->add_feature(den);
    }
  else if ((f = src->has_feature(HALF_DEN)))
    {
      half_den_t *t = dynamic_cast<half_den_t*>(f);
      assert(t->undisturbed == 0);
      src->del_feature(t);
      p->mesg(2, " - Den completed at %s by %d Grey Lizards. Den is presently un-populated.\n", (char*)(*src), band->colour[GREY]);
      den = new den_t();
      src->add_feature(den);
    }
  else
    {
      p->mesg(2, " - Den at %s has been half built. Den MUST be completed NEXT TURN with another Build order!\n", (char*)(*src));
      src->add_feature(new half_den_t());
      if ((f = src->has_feature(FERTILE)))
	src->del_feature(f);
      if ((f = src->has_feature(SCRUB)))
	src->del_feature(f);
    }

  /*
  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	(world->order_list [work]->order_type == BUILD_DEN ||
	 world->order_list [work]->order_type == BUILD_BRIDGE ||
	 world->order_list [work]->order_type == DESTROY_BRIDGE))
    {
      if (world->order_list [work]->order_type == BUILD_DEN)
        p->mesg(2, " - NEXT Build Den Order at %s ignored, only one Build Den Order allowed per hex, per turn.\n",
	       (char*)(*src));
      else
	if (world->order_list [work]->order_type == BUILD_BRIDGE)
          p->mesg(2, " - Build Bridge Order at %s ignored, Grey lizards exhausted from building den.\n",
	       (char*)(*src));
	else
          p->mesg(2, " - Destroy Bridge Order at %s ignored, Grey lizards exhausted from building den.\n",
	       (char*)(*src));

      world->order_list [work]->order_type = INACTIVE;
    }
  */
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Destroy Fertile
--
---------------------------------------------------------------------- */
order_destroy_fertile_t::order_destroy_fertile_t(class turn_t *t, int player, hex_t *src) : order_t(t, player, src)
{
  type = DESTROY_FERTILE;
  phase = DEVELOPMENT;
}

void order_destroy_fertile_t::show()
{
  turn->world->get_player(player)->mesg(2, "Destroy Fertile order issued.\n");
}  

int order_destroy_fertile_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Destroy Fertile order executed.\n");
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Destroy Bridge
--
---------------------------------------------------------------------- */
order_destroy_bridge_t::order_destroy_bridge_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dir_t d) : order_t(t, player, src, dst, d)
{
  type = DESTROY_BRIDGE;
  phase = DEVELOPMENT;
}

void order_destroy_bridge_t::show()
{
  turn->world->get_player(player)->mesg(2, "Destroy Bridge order issued.\n");
}  

int order_destroy_bridge_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Destroy Bridge executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Lob
--
---------------------------------------------------------------------- */
order_lob_t::order_lob_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dirs_t d) : order_t(t, player, src, dst, d)
{
  type = LOB;
  phase = ARTILLARY;
}

void order_lob_t::show()
{
  turn->world->get_player(player)->mesg(2, "Lob order issued.\n");
}  

int order_lob_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Lob order executed.\n");
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Chant
--
---------------------------------------------------------------------- */
order_chant_t::order_chant_t(class turn_t *t, int player, hex_t *src, hex_t *dst) : order_t(t, player, src, dst)
{
  type = CHANT;
  phase = ARTILLARY;
  chant = NUMBER_CHANTS;
}

order_chant_t::order_chant_t(class turn_t *t, int player, hex_t *src, hex_t *dst, dirs_t d) : order_t(t, player, src, dst, d)
{
  type = CHANT;
  phase = ARTILLARY;
  chant = NUMBER_CHANTS;
}

void order_chant_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant order issued.\n");
}

int order_chant_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Message
--
---------------------------------------------------------------------- */
order_message_t::order_message_t(class turn_t *t, int p1, int p2, char *text) : order_t(t, p1)
{
  turn = t;
  player = p1;
  type = MESSAGE;
  phase = SYSTEM;
}

void order_message_t::show()
{
  turn->world->get_player(player)->mesg(2, "Message order issued.\n");
}

int order_message_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Message order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Eyes
--
---------------------------------------------------------------------- */
order_chant_eyes_t::order_chant_eyes_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_EYES;
}

void order_chant_eyes_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Eyes order issued.\n");
}

int order_chant_eyes_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Eyes order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Summon
--
---------------------------------------------------------------------- */
order_chant_summ_t::order_chant_summ_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_SUMM;
}

void order_chant_summ_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Summon order issued.\n");
}

int order_chant_summ_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Summon order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Reveil
--
---------------------------------------------------------------------- */
order_chant_reve_t::order_chant_reve_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_REVE;
}

void order_chant_reve_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Reveil order issued.\n");
}

int order_chant_reve_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Revil order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Mould
--
---------------------------------------------------------------------- */
order_chant_moul_t::order_chant_moul_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_MOUL;
};

void order_chant_moul_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Mould order issued.\n");
}  

int order_chant_moul_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Mould order executed.\n");
  return 1;
}  


/* ----------------------------------------------------------------------
--
-- Chant Shunt
--
---------------------------------------------------------------------- */
order_chant_shun_t::order_chant_shun_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_SHUN;
};

void order_chant_shun_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Shunt order issued.\n");
}  

int order_chant_shun_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Shunt order executed.\n");
  return 1;
}  

/* ----------------------------------------------------------------------
--
-- Chant Freeze
--
---------------------------------------------------------------------- */
order_chant_free_t::order_chant_free_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_FREE;
}

void order_chant_free_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Freeze order issued.\n");
}

int order_chant_free_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Freeze order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Dark
--
---------------------------------------------------------------------- */
order_chant_dark_t::order_chant_dark_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_DARK;
}

void order_chant_dark_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Dark order issued.\n");
}

int order_chant_dark_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Dark order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Transmute
--
---------------------------------------------------------------------- */
order_chant_tran_t::order_chant_tran_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_TRAN;
}

void order_chant_tran_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Transmute order issued.\n");
}

int order_chant_tran_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Tranmute order executed.\n");
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Chant Coerce
--
---------------------------------------------------------------------- */
order_chant_coer_t::order_chant_coer_t(class turn_t *t, int p, hex_t*src, hex_t*dst, dirs_t d) : order_chant_t(t, p, src, dst, d)
{
  chant = CHANT_COER;
}

void order_chant_coer_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Coerce order issued.\n");
}

int order_chant_coer_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Coerce order executed.\n");
  return 1;
}


/* ----------------------------------------------------------------------
--
-- Chant Gate
--
---------------------------------------------------------------------- */
order_chant_gate_t::order_chant_gate_t(class turn_t *t, int p, hex_t*src, hex_t*dst) : order_chant_t(t, p, src, dst)
{
  chant = CHANT_GATE;
};

void order_chant_gate_t::show()
{
  turn->world->get_player(player)->mesg(2, "Chant Gate order issued.\n");
}  

int order_chant_gate_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Chant Gate order executed.\n");
  return 1;
}  


/* ----------------------------------------------------------------------
--
-- Free
--
---------------------------------------------------------------------- */
order_free_t::order_free_t(class turn_t *t, int p, hex_t *src) : order_t(t, p, src)
{
  type = FREE;
  phase = ARTILLARY;
};

void order_free_t::show()
{
  turn->world->get_player(player)->mesg(2, "Free order issued.\n");
}  

int order_free_t::execute()
{
  player_t *p = turn->world->get_player(player);
  band_t *b = (band_t*)src->has_feature(BAND);

  if (!band || src->owner != player)
  {
    p->mesg(2, " - Free Order at %s ignored as there are none of your lizards here!\n", (char*)(*src));
    return 0;
  }

  int count = 0;
  int free_type = UNSETTLED;

  for (int i = 0; i < LIZARD_TYPES; i++)
    if (band[i] > 0)
      {
	count++;
	free_type = i;
      }

  if (count != 1)
    {
      p->mesg(2, " - Free Order at %s ignored.  Only one type of lizards can be freed.\n", (char*)(*src));
      return 0;
    }

  den_t *den = (den_t*)src->has_feature(DEN);

  if (den && den->colour != UNSETTLED)
    {
      if (free_type != den->colour)
	{
	  p->mesg(2, " - Free Order at %s ignored as %s Den here will only accept %s lizards!\n", (char*)(*src),
		  misc_t::colours(den->colour), misc_t::colours(den->colour));
	  return 0;
	}
    }

  band->min(*b);
  int freed = b->size();

  if (freed == 0)
    {
      p->mesg(2, " - Free Order at %s ignored as you have no lizards here.\n", (char*)(*src));
      return 0;
    }

  p->mesg(2, " - You free %s Lizards at %s. ", (char*)(*band), (char*)(*src));
  if (den)
    {
      if (den->colour == UNSETTLED)
	p->mesg(2, " Lizards enter the un-settled Den here.\n");
      else
	    p->mesg(2, " Lizards join the Den population here.\n");
      
      den->pop = freed;

      *b -= *band;
      
      if (freed >= MINIMUM_POP_LEVEL && den->colour == UNSETTLED)
	{
	  den->colour = free_type;
	  p->mesg(2, " - Den is settled by freed Lizards and becomes a %s Den!\n", misc_t::colours(den->colour));
	}
    }
  else 
    {
      p->mesg(2, "Lizards return to the wild.\n");
      *b -= *band;
    }
  
  /* whip through orders looking for any moves from hex and adjusting the
    new numbers accordingly (to compensate for the freed lizards) */

    /*
  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	world->order_list [work]->order_type == MOVE)
    {
      world->order_list [work]->red_lizards = world_hex.red_lizards;
      world->order_list [work]->green_lizards = world_hex.green_lizards;
      world->order_list [work]->grey_lizards = world_hex.grey_lizards;
      world->order_list [work]->black_lizards = world_hex.black_lizards;
      world->order_list [work]->yellow_lizards = world_hex.yellow_lizards;
    }
    */
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Erupt Volcano
--
---------------------------------------------------------------------- */
order_erupt_volcano_t::order_erupt_volcano_t(class turn_t *t, int p, hex_t *src) : order_t(t, p, src)
{
  type = ERUPT_VOLCANO;
  phase = MOVEMENT;
};

char *order_erupt_volcano_t::describe()
{
  if (text) delete text;
  text = g_strdup_printf("Erupt Volcano in hex %s\n", (char*)(*src));
  return text;
}

/* ----------------------------------------------------------------------
--
-- Erupt Mountain
--
---------------------------------------------------------------------- */
order_erupt_mountain_t::order_erupt_mountain_t(class turn_t *t, int p, hex_t *src) : order_t(t, p, src)
{
  type = ERUPT_MOUNTAIN;
  phase = MOVEMENT;
};

char *order_erupt_mountain_t::describe()
{
  if (text) delete text;
  text = g_strdup_printf("Erupt Mountain in hex %s\n", (char*)(*src));
  return text;
}

/* ----------------------------------------------------------------------
--
-- Dormant Volcano
--
---------------------------------------------------------------------- */
order_dormant_volcano_t::order_dormant_volcano_t(class turn_t *t, int p, hex_t *src) : order_t(t, p, src)
{
  type = DORMANT_VOLCANO;
  phase = MOVEMENT;
}

char *order_dormant_volcano_t::describe()
{
  if (text) delete text;
  text = g_strdup_printf("Dormant Volcano in hex %s\n", (char*)(*src));
  return text;
}

/* ----------------------------------------------------------------------
--
-- Move Whirlpool
--
---------------------------------------------------------------------- */
order_move_whirlpool_t::order_move_whirlpool_t(class turn_t *t, int p, hex_t *src, hex_t *dst, dirs_t d) : order_t(t, p, src, dst, d)
{
  type = MOVE_WHIRLPOOL;
  phase = MOVEMENT;
}

char *order_move_whirlpool_t::describe()
{
  if (text) delete text;
  text = g_strdup_printf("Move Whirlpool %s from %s to %s\n", (char*)dirs, (char*)(*src), (char*)(*dst));
  return text;
}


/* ----------------------------------------------------------------------
--
-- Inactive order
--
---------------------------------------------------------------------- */

order_t::order_t(turn_t *t, int p)
{
  turn = t;
  player = p;
  src = NULL;
  dst = NULL;
  band = NULL;
  text = NULL;
  type = INACTIVE;
}

order_t::order_t(turn_t *t, int p, hex_t *s)
{
  turn = t;
  player = p;
  src = s;
  dst = NULL;
  band = NULL;
  text = NULL;
  type = INACTIVE;
}

order_t::order_t(turn_t *t, int p, hex_t *s, hex_t *d)
{
  turn = t;
  player = p;
  src = s;
  dst = d;
  band = NULL;
  text = NULL;
  type = INACTIVE;
}

order_t::order_t(turn_t *t, int p, hex_t *s, hex_t *d, dir_t d0)
{
  turn = t;
  player = p;
  src = s;
  dst = d;
  dirs[0] = d0;
  text = NULL;
  band = NULL;
  type = INACTIVE;
}

order_t::order_t(turn_t *t, int p, hex_t *s, hex_t *d, dirs_t dd)
{
  turn = t;
  player = p;
  src = s;
  dst = d;
  dirs = dd;
  band = NULL;
  text = NULL;
  type = INACTIVE;
}

void order_t::show()
{
  turn->world->get_player(player)->mesg(2, "Inactive order issued.\n");
}

int order_t::execute()
{
  turn->world->get_player(player)->mesg(2, "Inactive order executed.\n");
  return 1;
}

char* order_t::describe()
{
  if (text)
    return text;
  text = g_strdup_printf("INACTIVE order");
  return text;
}

order_t::operator char*()
{
  return describe();
}

/* ----------------------------------------------------------------------
--
-- Old code
--
---------------------------------------------------------------------- */

#ifdef OldCode
}



void destroy_fertile_order (order_t *order)
{
  FILE *fptr;

  char (char*)(*src) [80];

  int work;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, (char*)(*src));

  get_hex (world, order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex contains none of your Lizards!\n",
	     (char*)(*src));

    /* fclose(fptr); */

    return;
  }

  if (world_hex.green_lizards < BUILD_FERTILE_LEVEL)
  {
    if (world_hex.green_lizards == 0)
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex contains no Green Lizards!\n",
	       (char*)(*src));
    else
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex contains only %d Green Lizards!\n",
	       (char*)(*src), world_hex.green_lizards);

    /* fclose(fptr); */

    return;
  }

  switch (world_hex.terrain)
  {
    case DEN:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex occupied by Den!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case HOME_DEN:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex occupied by Home Den!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case RUIN:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex occupied by Ruin!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case SWAMP:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex is Swamp!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case PEAK:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex is Peak!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case VOLCANO:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex is Volcano!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case WATER:
    case WHIRLPOOL:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex is Water!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case TEMPLE:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as hex occupied by a Temple!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case SCRUB:
      p->mesg(2, " - Destroy Fertile Hex Order at %s ignored as this hex is Scrub!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case PLAIN:
      p->mesg(2, " - Plant Fertile Hex Order at %s ignored as this hex is Plain!\n",
	       (char*)(*src));

      /* fclose(fptr); */

      return;
    case CURSED:
      p->mesg(2, " - Destroy Fertile Hex at %s ignored as hex is CURSED.\n", (char*)(*src));

      /* fclose(fptr); */

      return;
    case FERTILE:
      p->mesg(2, " - Fertile hex at %s poisoned by %d Green Lizards.\n",
	       (char*)(*src), world_hex.green_lizards);
  }

  world_hex.terrain = PLAIN;

  put_hex (world, order->x_hex, order->y_hex, &world_hex);

  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	(world->order_list [work]->order_type == DESTROY_FERTILE ||
	 world->order_list [work]->order_type == BUILD_FERTILE ||
	 world->order_list [work]->order_type == BUILD_RAFT))
    {
      xy_to_string (world->order_list [work]->x_hex, world->order_list [work]->y_hex,
	(char*)(*src));

      if (world->order_list [work]->order_type == DESTROY_FERTILE)
        p->mesg(2, " - Next Destroy Fertile Hex Order at %s ignored, only one Destroy Fertile Order allowed per hex, per turn.\n",
	       (char*)(*src));
      else
	if (world->order_list [work]->order_type == BUILD_FERTILE)
          p->mesg(2, " - Build Fertile Order at %s ignored, Green lizards exhausted from digging up hex.\n",
	    (char*)(*src));
	else
          p->mesg(2, " - Build Raft Order at %s ignored, Green lizards exhausted from digging up hex.\n",
	    (char*)(*src));

      world->order_list [work]->order_type = INACTIVE;
    }

  /* fclose(fptr); */
}

void destroy_bridge_order (order_t *order)
{
  FILE *fptr;

  char (char*)(*src) [80],
       temp_string [80];

  int players [MAX_PLAYERS + 1];

  int work,
      x,
      y;

  hex_t world_hex;

  open_player_mesg (order->player, &fptr);

  xy_to_string (order->x_hex, order->y_hex, (char*)(*src));

  get_hex (world, order->x_hex, order->y_hex, &world_hex);

  if (lizards_in_hex (&world_hex) == 0 ||
      (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner != order->player))
  {
    p->mesg(2, " - Destroy Bridge Order at %s ignored as hex contains none of your Lizards!\n",
	     (char*)(*src));

    /* fclose(fptr); */

    return;
  }

  if (world_hex.grey_lizards < BUILD_BRIDGE_LEVEL)
  {
    if (world_hex.grey_lizards == 0)
      p->mesg(2, " - Destroy Bridge Order at %s ignored as hex contains no Grey Lizards!\n",
	       (char*)(*src));
    else
      p->mesg(2, " - Destroy Bridge Order at %s ignored as hex contains only %d Grey Lizards!\n",
	       (char*)(*src), world_hex.grey_lizards);

    /* fclose(fptr); */

    return;
  }

  move_coords (world,
	       order->moves [0], order->x_hex, order->y_hex,
	       &x, &y);

  if (order->x_hex == x && order->y_hex == y)
  {
    p->mesg(2, " - Destroy Bridge Order at %s ignored as attempted to destroy Bridge OFF EDGE OF WORLD!\n",
	     (char*)(*src));

    /* fclose(fptr); */

    return;
  }

  get_hex (world, x, y, &world_hex);

  p->mesg(2, " - Examining hex %s of %s for Bridge.",
	   dir [order->moves [0]], (char*)(*src));

  if (world_hex.bridge_here == 0)
  {
    p->mesg(2, " Destroy Bridge Order ignored as there is NO Bridge there!\n");

    /* fclose(fptr); */

    return;
  }

  xy_to_string (x, y, (char*)(*src));

  p->mesg(2, " Grey Lizards destroy Bridge at %s.\n", (char*)(*src));

  world_hex.bridge_here = 0;

  in_sight (world, x, y, players);

  /* fclose(fptr); */

  for (work = 1; work < MAX_PLAYERS + 1; work ++)
    if (players [work] > 0)
    {
      open_player_mesg (work, &fptr);

      if (work == order->player)
	p->mesg(2, " - Bridge at %s collapses!\n", (char*)(*src));
      else
	p->mesg(2, "\n - Bridge at %s collapses!\n", (char*)(*src));

      /* fclose(fptr); */
    }

  if (lizards_in_hex (&world_hex) > 0 && world_hex.hex_owner > 0)
  {
    open_player_mesg (world_hex.hex_owner, &fptr);

    if (world_hex.terrain == WHIRLPOOL)
    {
      p->mesg(2, " - %s Lizards at %s fall into Whirlpool below!\n",
	       show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	       world_hex.grey_lizards, world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string), (char*)(*src));

      /* fclose(fptr); */

      whirlpool_kill (world, x, y);
    } else {
      p->mesg(2, " - %s Lizards at %s fall into Water!\n",
	       show_lizards (world_hex.red_lizards, world_hex.green_lizards,
	       world_hex.grey_lizards, world_hex.black_lizards,
	       world_hex.yellow_lizards, temp_string), (char*)(*src));

      /* fclose(fptr); */

      bridge_collapse_kill (world, x, y);
    }
  }

  put_hex (world, x, y, &world_hex);

  open_player_mesg (order->player, &fptr);

  for (work = 0; work < world->num_orders; work ++)
    if (world->order_list [work] != order &&
	world->order_list [work]->x_hex == order->x_hex &&
	world->order_list [work]->y_hex == order->y_hex &&
	world->order_list [work]->player == order->player &&
	(world->order_list [work]->order_type == DESTROY_BRIDGE ||
	 world->order_list [work]->order_type == BUILD_BRIDGE ||
	 world->order_list [work]->order_type == BUILD_DEN))
    {
      xy_to_string (world->order_list [work]->x_hex, world->order_list [work]->y_hex,
	(char*)(*src));

      if (world->order_list [work]->order_type == DESTROY_BRIDGE)
        p->mesg(2, " - Next Destroy Bridge Order at %s ignored, only one Destroy Bridge Order allowed per hex, per turn.\n",
		 (char*)(*src));
      else
	if (world->order_list [work]->order_type == BUILD_BRIDGE)
          p->mesg(2, " - Build Bridge Order at %s ignored, Grey lizards exhausted from destroying bridge.\n",
	    (char*)(*src));
	else
          p->mesg(2, " - Build Den Order at %s ignored, Grey lizards exhausted from destroying bridge.\n",
	    (char*)(*src));

      world->order_list [work]->order_type = INACTIVE;
    }

  /* fclose(fptr); */
}

#endif
