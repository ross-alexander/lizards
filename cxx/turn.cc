#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

turn_t::turn_t(world_t *w)
{
  world = w;
  order_list = new order_t*[MAX_ORDERS_PER_TURN];
  num_orders = 0;
}

void turn_t::hex_moved_into(hex_t *hex)
{
#ifdef X
  FILE *fptr;
  order_t **order_list = world->order_list;

  char work_string [80];

  int work;

  hex->turns_undisturbed = 0;

  for (work = 0; work < number_orders; work ++)
    if (order_list [work]->x_hex == x && order_list [work]->y_hex == y)
    {
      open_player_mesg (order_list [work]->player, &fptr);

      xy_to_string (x, y, work_string);

      switch (order_list [work]->order_type)
      {
	case BUILD_FERTILE:
	  order_list [work]->order_type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->hex_owner != order_list [work]->player)
	    fprintf (fptr, " - Build Fertile Hex Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     work_string);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to plant Fertile hex here!\n",
		     work_string);

	  break;
	case BUILD_DEN:
	  order_list [work]->order_type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->hex_owner != order_list [work]->player)
	    fprintf (fptr, " - Build Den Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     work_string);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to build Den here!\n",
		     work_string);

	  break;
	case BUILD_BRIDGE:
	  order_list [work]->order_type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->hex_owner != order_list [work]->player)
	    fprintf (fptr, " - Build Bridge Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     work_string);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to build Bridge to the %s!\n",
		     work_string, dir [order_list [work]->moves [0]]);

	  break;
	case DESTROY_FERTILE:
	  order_list [work]->order_type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->hex_owner != order_list [work]->player)
	    fprintf (fptr, " - Destroy Fertile Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     work_string);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to destroy Fertile hex here!\n",
		     work_string);

	  break;
	case DESTROY_BRIDGE:
	  order_list [work]->order_type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->hex_owner != order_list [work]->player)
	    fprintf (fptr, " - Destroy Bridge Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     work_string);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to destroy Bridge to the %s!\n",
		     work_string, dir [order_list [work]->moves [0]]);

	  break;
	case BUILD_RAFT:
	  order_list [work]->order_type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->hex_owner != order_list [work]->player)
	    fprintf (fptr, " - Build Raft Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     work_string);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to build a Raft to the %s!\n",
		     work_string, dir [order_list [work]->moves [0]]);

	  break;
	default:
	  break;
      }

      /* fclose(fptr); */
    }
#endif
}


/* ----------------------------------------------------------------------
--
-- combat
--
-- If flee_flag == 1 defenders can flee
--
---------------------------------------------------------------------- */

int turn_t::combat (band_t *attack, band_t *defend, hex_t *src, hex_t *dst, int defend_flee)
{
  double attack_percent,
    defend_percent;
  
  int attacking_deaths,
    defending_deaths,
    attack_hit,
    defend_hit,
    round,
    work,
    lizards;
  
  player_t *p1 = world->get_player(src->owner);
  player_t *p2 = world->get_player(dst->owner);
  
  round = 0;
  
  if (dst->has_feature(DEN))
    attack_hit = DEN_ATTACK;
  else
    attack_hit = BASE_ATTACK;
  defend_hit = BASE_ATTACK;
  
  attack_hit += (attack->combat * CP_MOD_LEVEL) - (defend->combat * CP_MOD_LEVEL);

  do
    {
      defending_deaths = 0;
      attacking_deaths = 0;
      
      /* --------------------
	 Do attack killing defenders (only half attack)
	 -------------------- */
      
      lizards = attack->size()/2;
      
      if (lizards == 0)
	lizards = 1;
      
      for (work = 0; work < lizards; work ++)
	if ((misc_t::uniform(100) <= attack_hit) && (defending_deaths < defend->size()))
	  defending_deaths ++;
      
    /* --------------------
       Do defenders killing attackers
       -------------------- */
      
      lizards = defend->size()/2;
      
      if (lizards == 0)
	lizards = 1;
      
      for (work = 0; work < lizards; work ++)
	if ((misc_t::uniform(100) <= defend_hit) && (attacking_deaths < attack->size()))
	  attacking_deaths ++;
      round ++;
      
      /* --------------------
	 Kill lizards in the bands
       -------------------- */
      
      attack->kill(attacking_deaths);
      defend->kill(defending_deaths);
      
      /* --------------------
	 Check if attackers flee
	 -------------------- */
      
      if ((attack->size() > 0) && (defend->size() > 0))
	{
	  attack_percent = 100 * ((double)defend->size() / (double)attack->size());
	  defend_percent = 100 * ((double)attack->size() / (double)defend->size());
	  
	  if (attack_percent > 150)
	    {
	      attack_percent -= 125;
	      
	      if (misc_t::uniform(100) <= attack_percent)
		{
		  p1->mesg(3, " - Combat round %d! ", round);
		  p1->mesg(3, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
			   attack->size(), defend->size());
		  
		  p2->mesg(3, " - Combat round %d! ", round);
		  p2->mesg(3, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
			   defend->size(), attack->size());
		  p2->mesg(3, " - Attacking Lizards flee!\n");
		  return (COMBAT_ATTACK_FLEE);
		}
	    }
	}
      
      if (!dst->has_feature(DEN) && (defend_percent > 150) && (defend_flee == 1))
	{
	  defend_percent -= 125;
	  if (misc_t::uniform(100) <= defend_percent)
	    {
	      p1->mesg(3, " - Combat round %d! ", round);
	      p1->mesg(3, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
		       attack->size(), defend->size());
	      p1->mesg(3, " - Enemy Lizards attempt to flee!\n");
	      
	      p2->mesg(3, " - Combat round %d! ", round);
	      p2->mesg(3, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
		       defend->size(), attack->size());
	      return (COMBAT_DEFEND_FLEE);
	    }
	}
    } while ((attack->size() > 0) && (defend->size() > 0));

  p1->mesg(3, " - Combat round %d! ", round);
  p1->mesg(3, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	   attack->size(), defend->size());

  p2->mesg(3, " - Combat round %d! ", round);
  p2->mesg(3, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	   defend->size(), attack->size());

  if (attack->size() > 0)
    return COMBAT_DEFEND_DEAD;
  else if (defend->size() > 0)
    return COMBAT_ATTACK_DEAD;
  else
    return COMBAT_BOTH_DEAD;
}

/* ----------------------------------------------------------------------
--
-- combat_sea_monster
--
---------------------------------------------------------------------- */

int turn_t::combat_sea_monster (band_t *attack, hex_t *src, int sea_monster)
{
  int attacking_deaths,
      defending_deaths,
      attack_hit,
      defend_hit,
      round,
      work,
      lizards;

  round = 0;

  attack_hit = BASE_ATTACK + (attack->combat * CP_MOD_LEVEL);
  defend_hit = MONSTER_DEFEND;

  do
    {
      defending_deaths = 0;
      attacking_deaths = 0;
      
      lizards = attack->size() / 2;
      
      if (lizards == 0)
	lizards = 1;
      
      for (work = 0; work < lizards; work++)
	if ((misc_t::uniform(100) <= attack_hit) && (defending_deaths < sea_monster))
	  defending_deaths++;
      
      for (work = 0; work < (sea_monster / 2); work ++)
	if ((misc_t::uniform(100) >= defend_hit) && (attacking_deaths < attack->size()))
	  attacking_deaths ++;

      round ++;
      attack->kill(attacking_deaths);
      sea_monster -= defending_deaths;
    }
  while ((attack->size()) > 0 && (sea_monster > 0));

  player_t *p = world->get_player(src->owner);
  p->mesg(3, " - Combat round %d! ", round);
  p->mesg(3, "Surviving Lizards: %d.\n", attack->size());
  if (attack->size() > 0)
    return COMBAT_DEFEND_DEAD;
  else if (sea_monster > 0)
    return COMBAT_ATTACK_DEAD;
  else
    return COMBAT_BOTH_DEAD;
}

static int lizards_in_band(feature_t *f)
{
  if (f == NULL)
    return 0;
  if (f->type == BAND)
    {
      band_t *b = (band_t*)f;
      return b->size();
    }
  return 0;
}

static int lizards_in_hex(hex_t *h)
{
  return(lizards_in_band(h->has_feature(BAND)));
}


/* ----------------------------------------------------------------------
--
-- add_order
--
---------------------------------------------------------------------- */
int turn_t::add_order(order_t *order)
{
  int i, split_total, sneak_total;
  hex_t *hex;

  player_t *p = world->get_player(order->player);

  switch (order->type)
    {
    case MOVE:
      if (order->player != order->src->owner)
	{
	  p->mesg(2, " - MOVE %s %s order failed, you do not own this hex.\n", order->src->title, (char*)(order->dirs));
	  return 0;
	}
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    /* Redundant order ignored! */
	    p->mesg(2, " - MOVE %s %s order fail, not enough lizards in hex.\n", order->src->title, (char*)order->dirs);
	    return 0;
	  }
      break;
    case SPLIT:
      for (i = 0, split_total = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    split_total += lizards_in_band(order_list[i]->band);
	  }
      hex = order->src;
      if (split_total >= lizards_in_hex(hex))
	{
	    /* Redundant order ignored! */
	  p->mesg(2, " - SPLIT %s %s %s order failed, not enough lizards in hex.\n",
		  (char*)(order->src), (char*)(order->dirs), (char*)(order->band));
	  return 0;
	}
      break;

    case SNEAK:
      for (i = 0, sneak_total = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    sneak_total++;
	  }
      if (sneak_total >= order->src->count_feature(SPY, order->player))
	{
	  /* Redundant order ignored! */
	  p->mesg(2, " - SNEAK %s %s ignored, not enough lizards starting in hex.\n", (char*)order->src, (char*)(order->dirs));
	  return 0;
	}
      break;

    case RECRUIT:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    /* Redundant order ignored! */
	    p->mesg(2, " - RECRUIT %s ignored, only one recruit allowed from hex.\n",
		    order->src->title);
	    return 0;
	  }
      break;

    case CHANT:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    order_chant_t *c = (order_chant_t*)order;
	    p->mesg(2, " - CHANT %s %s %s %s ignored, only one chant allowed from hex.\n", (char*)order->src, c->abbrev, (char*)(order->dirs));
	    return 0;
	  }
      break;

    case LOB:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    p->mesg(2, " - LOB %s %s ignored, only one lob allowed from hex.\n", (char*)order->src, (char*)(order->dirs));
	    return 0;
	  }
      break;

    case BUILD_FERTILE:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    /* Redundant order ignored! */
	    
	    p->mesg(2, " - BUILD FERTILE %s ignored, only one fertile build allowed from hex.\n", (char*)order->src);
	    return 0;
	  }
      break;

    case BUILD_DEN:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    /* Redundant order ignored! */
	    p->mesg(2, " - BUILD DEN %s ignored, only one den build allowed from hex.\n", (char*)order->src);
	    return 0;
	  }
      break;

    case BUILD_BRIDGE:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    /* Redundant order ignored! */
	    p->mesg(2, " - BUILD BRIDGE %s %s ignored, only one bridge build allowed from hex.\n", (char*)order->src, (char*)order->dirs);
	    return 0;
	  }
      break;

    case DESTROY_FERTILE:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    /* Redundant order ignored! */
	    p->mesg(2, " - DESTROY FERTILE %s ignored, only one fertile destroy allowed from hex.\n", (char*)order->src);
	    return 0;
	  }
      break;

    case DESTROY_BRIDGE:
      for (i = 0; i < num_orders; i++)
	if (*order == *order_list[i])
	  {
	    p->mesg(2, " - DESTROY BRIDGE %s %s ignored, only one bridge destroy allowed from hex.\n", (char*)order->src, (char*)order->dirs);
	    return 0;
	  }
      break;
    default:
      break;
  }
  order_list[num_orders] = order;

  printf("%02d : %s added to order list.\n", num_orders, (char*)(*order));

  world->get_player(order->player)->num_orders++;
  num_orders ++;
  if (num_orders >= MAX_ORDERS_PER_TURN)
    {
      printf ("FATAL ERROR: Exceeded Maximum of %d orders per turn.\n", MAX_ORDERS_PER_TURN);
      exit (EXIT_FAILURE);
    }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- Do terrain
--
---------------------------------------------------------------------- */

int turn_t::terrain()
{
  printf ("Working terrain...\n");

  for (int x = 0; x < world->map->width; x++)
    for (int y = 0; y < world->map->height; y++)
      {
	hex_t *hex = (*world->map)(x, y);

	if (hex->has_feature(VOLCANO))
	  {
	    if (misc_t::uniform(100) < VOLCANO_ERUPT_LEVEL && world->turn > 0)
	      {
		order_t* o = new order_erupt_volcano_t(this, 0, hex);
		add_order(o);
		}
	      else
		{
		  if (misc_t::uniform(100) < VOLCANO_DORMANT_LEVEL && ((world->flags & LIZ_VOLCANO) == 1) && (world->turn > 0))
		    {
		      order_t *o = new order_dormant_volcano_t(this, 0, hex);
		      add_order(o);
		    }
		}
	  }

	if (hex->has_feature(PEAK))
	  {
	    if (misc_t::uniform(100) < MOUNTAIN_ERUPT_LEVEL && ((world->flags & LIZ_PEAK) == 1) && world->turn > 0)
	      {
		order_t *o = new order_erupt_mountain_t(this, 0, hex);
		add_order(o);
	      }
	  }
	if (hex->has_feature(WHIRLPOOL) && (world->turn > 0))
	  {
	    dirs_t d;
	    hex_t *d1, *d2, *dst;
	    int k;
	    if (hex->terrain_adjacent(WATER) > 0)
	      {
		do
		  {
		    d1 = hex->adjacent(dir_t(k = misc_t::uniform(DIRECTIONS)));
		  } while (d1 == NULL || d1->terrain != WATER);

		dst = d1;
		d[0] = k;

		if (d1->terrain_adjacent(WATER) > 0)
		  {
		    do
		      {
			d2 = d1->adjacent(k = misc_t::uniform(DIRECTIONS));
		      } while (d2 == NULL || d2->terrain != WATER);

		    dst = d2;
		    d[1] = k;
		  }
		order_t *o = new order_move_whirlpool_t(this, 0, hex, dst, d);
		add_order(o);
	      }
	  }
      }
  return 1;
}



/* ----------------------------------------------------------------------
--
-- lizard_pop_growth
--
---------------------------------------------------------------------- */

int turn_t::pop_growth (int present_pop, int max_pop)
{
  if (present_pop > max_pop)
    {
      return (max_pop - present_pop) / 2;
    }
  else
    {
      double z = 0.6 - (((double)present_pop / (double)max_pop) * 0.5);

      if (world->flags & LIZ_SPAWN)
	z *= 10.0;
      
      return (double)present_pop * z;
    }
}

/* ----------------------------------------------------------------------
--
-- ProcessDen
--
---------------------------------------------------------------------- */
void turn_t::open_den(hex_t *hex, den_t *den)
{
  player_t*p = world->get_player(hex->owner);
  
  int adj = hex->feature_adjacent(FERTILE);
  int ruined = 0;

  if (adj == 0)
    {
      if (world->turn > 0)
	{
	  if (den->colour == UNSETTLED)
	    {
	      if (hex->owner > 0)
		p->mesg(2, " - Unsettled den at %s collapses into ruins.\n", (char*)(*hex));
	      ruined = 1;
	    }
	  else
	    {
	      int starve = ((double) den->pop * (DEN_STARVE_LEVEL + (den->hunger * DEN_INCREASE_STARVE_LEVEL)));

	      if ((den->pop - starve) < MINIMUM_POP_LEVEL)
		{
		  ruined = 1;
		  p->mesg(2, " - %s at %s starves to death and collapses.\n", (char*)(*den), (char*)(*hex));
		}
	      else
		{
		  den->pop -= starve;
		  den->hunger++;
		  assert(den->pop >= MINIMUM_POP_LEVEL);
		  p->mesg(2, " - %d lizards in %s at %s starved to death, leaving %d left.\n", starve, (char*)(*den), (char*)(*hex), den->pop);
		}
	    }
	}
    }
  else
    {
      if (den->colour == UNSETTLED)
	{
	  int growth = POP_ADDITION;
	  den->pop += growth;
	  p->mesg(2, " - %s Nomadic lizards join %s Den at %s.\n", growth, (char*)den, (char*)(*hex));
	  if (den->pop > MINIMUM_POP_LEVEL)
	    {
	      den->colour = misc_t::uniform(LIZARD_TYPES);
	      p->mesg(2, " - Den at %s has been settled and is now a %s Den.\n", (char*)(*hex), (char*)(*den));
	    }
	}
      else 
	{
	  int max_pop = (adj * DEN_LIZARD_LEVEL);
	  if (den->pop < max_pop)
	    {
	      int growth = pop_growth (den->pop, max_pop);
	      den->pop += growth;
	      p->mesg(2, " - %s Den at %s grows by %d to %d lizards.\n", (char*)(*den), (char*)(*hex), growth, den->pop);
	    }
	  else
	    {
	      if (den->pop > max_pop && (world->turn > 0))
		{
		  int growth = pop_growth (den->pop, max_pop);
		  
		  if (growth == 0)
		    growth = -1;
		  
		  den->pop += growth;
		  p->mesg(2, " - %s Den at %s becomes over populated and looses %d lizards, leaving %d.\n",
			  (char*)(*den), (char*)(*hex), -growth, den->pop);
		}
	    }
	}
    }
  if (ruined)
    {
      hex->del_feature(den);
      hex->add_feature(new ruin_t());
    }
}

/* ---------------------------------------------------------------------- */

void turn_t::open_hex(hex_t *hex)
{
  if (half_den_t *den = dynamic_cast<half_den_t*>(hex->has_feature(HALF_DEN)))
    den->undisturbed--;

  band_t *b = (band_t*)hex->has_feature(BAND);

  if (b && b->immobile > 0)
    {
      if (b->immobile == 3)
	b->immobile = 0;
      else
	b->immobile -= 1;
    }
	  
  if ((hex->has_feature(CURSED) && (world->turn > 0)))
    {
      if (b && b->size() > 0)
	{
	  int i = b->size() * CURSED_KILL_LEVEL;
	  if (i < 1) i = 1;
	  
	  player_t *p = world->get_player(hex->owner);
	  p->mesg(2, "Cursed(%s, %d, %d, %d, %d, %d, %d)\n", (char*)hex, b->size());
	  b->kill(i);
	}
      /*
      for (work3 = 0; work3 < world->num_spies; work3 ++)
	if (world->spylist [work3]->x_hex == world_hex.x &&
	    world->spylist [work3]->y_hex == world_hex.y)
	  {
	    fprintf (world->player[world_hex.hex_owner].fptr,
		     "SpyCursed(%s)\n",
		     work_string);
	    world->spylist [work3]->player = 0;
	  }
      */
    }
    
  den_t *d = (den_t*)hex->has_feature(DEN);
  if (d != NULL) open_den(hex, d);
  
  if (b)
    {
      if (hex->has_feature(FERTILE))
	b->hunger = 0;
      else
	{
	  if (b->hunger < MAX_HUNGER_POINTS && (world->turn > 0))
	    b->hunger++;
	}
    }
}

/* ----------------------------------------------------------------------
--
-- WorldOpen(World *world)
--
-- For each hex
--   Kill cursed
--   Handle band hunger levels
--   Expand dens
-- For each spy
--   Check for going native
--
---------------------------------------------------------------------- */
int turn_t::worldopen()
{
  int loop, i;
  player_t *p;

  printf ("Messaging players...");

  for (loop = 1; loop < world->num_players; loop ++)
    {
      p = world->get_player(loop);
      p->mesg(2, "Header(%d, %d, %d", world->turn, loop, world->num_players);
      for (i = 1; i < world->num_players; i++)
	{
	  player_t *p2 = world->get_player(i);
	  p->mesg(2, ", %d, %s, %s", i, p2->code, p2->clan);
	}
      p->mesg(2, ")\n");

      if (world->flags & LIZ_SPAWN)
	p->mesg(2, "Spawning(on)\n");
      else
	p->mesg(2, "Spawning(off)\n");
    }

  for (loop = 1; loop < world->num_players; loop++)
    {
      p = world->get_player(loop);
      p->mesg(2, "Phase(world)\n");
    }
  printf (" \nOpening world...\n");
  
  /* ---------
     --
     -- process each work hex in turn
     --
     --------- */

  for (int y = 0; y < world->map->height; y++)
    for (int x = 0; x < world->map->width; x++)
      open_hex((*world->map)(x, y));

  /* Update spy turns alone */

#ifdef HaveSpies

  printf (" \nUpdating spys...\n");
  for (loop = 0; loop < world->num_spies; loop++)
    {
      world->spylist [loop]->turns_alone ++;
      get_hex (world,
	       world->spylist [loop]->x_hex,
	       world->spylist [loop]->y_hex, &world_hex);
      
      /* Free immobile Spys */
      
      if (world->spylist [loop]->mired > 0)
	world->spylist [loop]->mired -= 1;
       
      /* Clear Spy moved flags */
      
      world->spylist[loop]->moved_this_turn = 0;
    }
#endif

  printf ("Counting HOME DENS...\n");

  for (loop = 0; loop < world->num_players; loop ++)
    world->get_player(loop)->home_dens = 0;

  for (int y = 0; y < world->map->height; y++)
    for (int x = 0; x < world->map->width; x++)
      {
	hex_t *hex = (*world->map)(x, y);
	den_t *den = (den_t*)hex->has_feature(DEN);
	if (den && den->home)
	  world->get_player(hex->owner)->home_dens++;
      }

  return 1;
}

/* ----------------------------------------------------------------------
--
-- DoPass
--
---------------------------------------------------------------------- */
void turn_t::do_pass(int pass)
{
  int player;

  for (player = 1; player < world->num_players; player++)
    {
      switch (pass)
	{
	case ARTILLARY:
	  world->get_player(player)->mesg(1, "Phase(artillery)\n");
	  break;
	case MOVEMENT:
	  world->get_player(player)->mesg(1, "Phase(movement)\n");
	  break;
	case DEVELOPMENT:
	  world->get_player(player)->mesg(1, "Phase(development)\n");
	  break;
	}
    }

  if (pass == SYSTEM)
    printf ("LIZARDS! SYSTEM PASS:\n");
  else
    printf ("PASS %d:\n", pass);

  int total = num_orders;

  /* --------------------
     The orders are processed in reverse order so that completed orders
     can be "removed" from any additional post order checking.
     This should be done with member functions in the turn_t class (get order, etc)
     but that can come later.
     -------------------- */

  for (int i = total - 1; i >= 0; i--)
    {
      order_t *o = order_list[i];
      int result;
      num_orders--;
      if (o->phase == pass && o->type != INACTIVE)
	{
	  o->show();
	  result = o->execute();
	  o->type = INACTIVE;
	}
    }
   num_orders = total;
}

int turn_t::worldmaster()
{
  int total = num_orders;

  /* --------------------
     Create copy of the list
     -------------------- */

  order_t** tmplist = new order_t*[total];
  for (int i = 0; i < total; i++)
    tmplist[i] = order_list[i];

  /* --------------------
     Reverse the list
     -------------------- */

  for (int i = 0; i < total; i++)
    order_list[total - i - 1] = tmplist[i];
  
  //  get_message_file();
  
  for (int pass = SYSTEM; pass <= DEVELOPMENT; pass++)
    {
      do_pass(pass);
      printf ("PASS %d END.\n", pass);
    }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- close_hex
--
---------------------------------------------------------------------- */

void turn_t::close_hex(hex_t *hex)
{
  player_t *p = world->get_player(hex->owner);
  raft_t *raft = dynamic_cast<raft_t*>(hex->has_feature(RAFT));
  if (raft)
    {
      raft->life--;
      if (raft->life == 0)
	{
	  band_t *band = dynamic_cast<band_t*>(hex->has_feature(BAND));
	  if (band && hex->has_feature(WHIRLPOOL))
	    {
	      int work3 = (double) band->size() * WHIRLPOOL_KILL_LEVEL;
	      if (work3 > 1)
		work3 = misc_t::uniform(work3) + 1;
	      else
		work3 = 1;
	      band->kill(work3);
	      p->mesg(2, "RaftWhirlpool(%s, %d, %s)\n",
		      (char*)hex,
		      work3,
		      (char*)band);
	    }
	  else
	    {
	      p->mesg(2, "RaftSinks(%s)\n", (char*)hex);
	    }
	}
    }
  
  /* Check 'turns_undisturbed' variable for Den building! */

  if (half_den_t *t = dynamic_cast<half_den_t*>(hex->has_feature(HALF_DEN)))
    {
      if (t->undisturbed == 0)
	{
	  p->mesg(2, " - Half completed den at %s collapses into ruins.\n", (char*)hex);
	  hex->del_feature(t);
	  hex->add_feature(new ruin_t());
	}
    }

  den_t *den = dynamic_cast<den_t*>(hex->has_feature(DEN));
  if (den && den->timebomb)
    {
      assert(den->home);
      if (den->timebomb-- == 0)
	{
	  /*
	  int players[MAX_PLAYERS + 1];

	      in_sight (world, x, y, players);

	      for (work3 = 1; work3 < MAX_PLAYERS + 1; work3 ++)
		if (players [work3] > 0)
		  {
		    open_player_mesg (work3, &fptr);
		    fprintf (fptr, "SeeHomeDenCollapse(%s)\n", hex_string);
		  }

	      world_hex.terrain = RUIN;
	      world_hex.lizard_pop = 0;
	      world_hex.turns_undisturbed = 0;
	      hex_change = 1;
	  */
	  }
      }
  
  /* Check Lizard band hunger points! */

  band_t *band = dynamic_cast<band_t*>(hex->has_feature(BAND));
  if (band && band->hunger >= MAX_HUNGER_POINTS && hex->owner > 0 && world->turn > 0 && !(hex->has_feature(FERTILE)))
    {
      int work3, work4;
      for (work3 = 0, work4 = 0; work3 < band->size(); work3 ++)
	if (misc_t::uniform(100) > STARVE_PER_TURN)
	  work4 ++;
      if (work4 == 0) work4 = 1;

      band->kill(work4);
      p->mesg(2, "Starve(%s, %d, %s)\n", (char*)hex, work4, (char*)band);
    }
}


/* ----------------------------------------------------------------------
--
-- CloseSpies
--
---------------------------------------------------------------------- */

void turn_t::close_spies()
{
#ifdef Ignore
  int work;
  hex_t world_hex;
  FILE *fptr;
  char hex_string[80];

  for (work = 0; work < world->num_spies; work ++)
    {
      get_hex (world,
	       world->spylist[work]->x_hex,
	       world->spylist[work]->y_hex, &world_hex);
      xy_to_string (world->spylist[work]->x_hex, world->spylist[work]->y_hex, hex_string);

      if (lizards_in_hex (&world_hex) > 0 && world->spylist[work]->player > 0 &&
	  world->spylist[work]->player == world_hex.hex_owner)
	{
	  open_player_mesg (world->spylist[work]->player, &fptr);
	  
	  xy_to_string (world->spylist[work]->x_hex, world->spylist[work]->y_hex,
			hex_string);

	  if (lizards_in_hex (&world_hex) >= MAX_LIZARDS_IN_HEX)
	    {
	      fprintf (fptr, "SpyFailRejoin(%s)\n", hex_string);
	    }
	  else
	    {
	      fprintf (fptr, "SpyRejoin(%s)\n", hex_string);
	      switch (world->spylist[work]->lizard_type)
		{
		case RED_LIZARD:
		  world_hex.red_lizards += 1;

		  break;
		case GREEN_LIZARD:
		  world_hex.green_lizards += 1;

		  break;
		case GREY_LIZARD:
		  world_hex.grey_lizards += 1;

		  break;
		case BLACK_LIZARD:
		  world_hex.black_lizards += 1;

		  break;
		case YELLOW_LIZARD:
		  world_hex.yellow_lizards += 1;

		  break;
		}
	      put_hex (world,
		       world->spylist[work]->x_hex,
		       world->spylist[work]->y_hex,
		       &world_hex);
	      world->spylist[work]->player = 0;
	    }
	}

      if (world->spylist[work]->turns_alone > SPY_BUSH_TURNS && world->spylist[work]->player > 0)
	{
	  open_player_mesg (world->spylist[work]->player, &fptr);
	  fprintf (fptr, "SpyNative(%s)\n", hex_string);
	  world->spylist[work]->player = 0;
	}
    }
#endif
}

/* ----------------------------------------------------------------------

For each hex
   Band hunger
   Raft life
   Den building
For each spy
   Reunite with band
 For each player
   Count home dens

---------------------------------------------------------------------- */

int turn_t::worldclose()
{
  int x, y, loop;
  hex_t world_hex;

  for (y = 0; y < world->map->height; y++)
    for (x = 0; x < world->map->width; x++)
      close_hex((*world->map)(x, y));
  
  printf ("\nUpdating spys...\n");

  //  CloseSpies(world);

  printf ("Counting home dens...\n");

  for (loop = 0; loop < world->num_players; loop ++)
    world->get_player(loop)->home_dens = 0;

  for (y = 0; y < world->map->height; y++)
    for (x = 0; x < world->map->width; x++)
      {
	hex_t *hex = (*world->map)(x, y);
	den_t *den = (den_t*)hex->has_feature(DEN);
	if (den && den->home)
	  world->get_player(hex->owner)->home_dens++;
      }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- ordershuffle
--
---------------------------------------------------------------------- */

int turn_t::ordershuffle()
{
  int work;
  for (work = 0; work < num_orders; work++)
    {
      printf("%02d - %s\n", work, (char*)(*order_list[work]));
      order_list[work]->show();
    }

#ifdef Ignore
  if (world->num_orders > 1)
    for (work = 0; work < world->num_orders; work ++)
    {
      work2 = random (world->num_orders);
      do
	{
	  work3 = random (world->num_orders);
	}
      while (work3 == work2);
      
      order_ptr = world->order_list [work2];
      world->order_list [work2] = world->order_list [work3];
      world->order_list [work3] = order_ptr;
    }
}
#endif

  return 1;
}
