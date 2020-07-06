#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizards.h"
#include "lizmisc.h"

void hex_attacked_or_moved_into (world_t *world, hex_t *hex)
{
  FILE *fptr;
  order_t *order,**order_list = world->order_list;
  char source[5], dest[5];
  int work;

  xy_to_string (hex->xy.x, hex->xy.y, source);
  for (work = 0; work < world->num_orders && (order = order_list[work]); work ++)
    if (coord_eq(order->source, hex->xy))
      {
	xy_to_string (order->dest.x, order->dest.y, dest);
	switch (order->type)
	  {
	  case BUILD_FERTILE:
	    order->status = ORDER_IGNORE;
	    if (lizards_in_hex (hex) == 0 || hex->owner != order->player)
	      player_mesg(world, hex->owner,
			       " - Build Fertile Hex Order at %s ignored as there were none of your lizards here at start of the turn!\n",
			       source);
	    else
	      player_mesg(world, hex->owner,
			       " - Lizards at %s disturbed while trying to plant Fertile hex here!\n",
		     source);
	    break;
	case BUILD_DEN:
	  order_list [work]->type = INACTIVE;

	  if (lizards_in_hex (hex) == 0 || hex->owner != order->player)
	    player_mesg(world, hex->owner,
			     " - Build Den Order at %s ignored as there were none of your lizards here at start of the turn!\n",
		     source);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to build Den here!\n",
		     source);
	  break;
	case BUILD_BRIDGE:
	  order->status = ORDER_IGNORE;

	  if (lizards_in_hex (hex) == 0 || hex->owner != order->player)
	    player_mesg(world, hex->owner,
			     " - Build Bridge Order at %s ignored as there were none of your lizards here at start of the turn!\n",
			     source);
	  else
	    fprintf (fptr, " - Lizards at %s disturbed while trying to build Bridge to in %s!\n",
		     source, dest);
	  break;
	  case DESTROY_FERTILE:
	    order->status = ORDER_IGNORE;
	    if (lizards_in_hex (hex) == 0 || hex->owner != order->player)
	      player_mesg(world, hex->owner,
			       " - Destroy Fertile Order at %s ignored as there were none of your lizards here at start of the turn!\n",
			       source);
	    else
	      	    player_mesg(world, hex->owner,
				     " - Lizards at %s disturbed while trying to destroy Fertile hex here!\n",
				     source);
	    break;
	  case DESTROY_BRIDGE:
	    order->status = ORDER_IGNORE;
	    if (lizards_in_hex (hex) == 0 || hex->owner != order->player)
	      	    player_mesg(world, hex->owner,
				     " - Destroy Bridge Order at %s ignored as there were none of your lizards here at start of the turn!\n",
				     source);
	    else
	      	    player_mesg(world, hex->owner,
				     " - Lizards at %s disturbed while trying to destroy Bridge to the %s!\n",
				     source, dest);
	    break;
	  case BUILD_RAFT:
	    order->status = ORDER_IGNORE;
	    if (lizards_in_hex (hex) == 0 || hex->owner != order->player)
	      	    player_mesg(world, hex->owner,
				     " - Build Raft Order at %s ignored as there were none of your lizards here at start of the turn!\n",
				     source);
	    else
	      	    player_mesg(world, hex->owner,
				     " - Lizards at %s disturbed while trying to build a Raft to the %s!\n",
				     source, dest);
	    break;
	  default:
	    break;
	  }
      }
}

void show_order (world_t *w, order_t *order)
{
  char source[5], dest[5];

  xy_to_string (order->source.x, order->source.y, source);
  xy_to_string (order->dest.x, order->dest.y, dest);
  switch (order->type)
    {
	case MOVE:
	  player_mesg(w, order->player, "Move Order (P%d-%s %s %s).\n",
			   order->player,
			   w->player[order->player]->code, source, dest);
	  break;
	case SPLIT:
	  player_mesg(w, order->player, 
			   "Split Order (P%d-%s %s %s %dred %dgrn %dgry %dblk %dyel).\n",
			   order->player, w->player[order->player]->code,
			   source, dest,
			   order->feature.f.band.type[RED],
			   order->feature.f.band.type[GREEN],
			   order->feature.f.band.type[GREY],
			   order->feature.f.band.type[BLACK],
			   order->feature.f.band.type[YELLOW]);
	  break;
    case SNEAK:
      player_mesg(w, order->player, "Sneak Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case RECRUIT:
      player_mesg(w, order->player, "Recruit Order (P%d-%s %s).\n",
		       order->player, w->player[order->player]->code, source);
      break;
    case BUILD_FERTILE:
      player_mesg(w, order->player, "Build Fertile Order (P%d-%s %s).\n",
		       order->player, w->player[order->player]->code, source);
      break;
    case BUILD_DEN:
      player_mesg(w, order->player, "Build Den Order (P%d-%s %s).\n",
		       order->player, w->player[order->player]->code, source);
      break;
    case BUILD_BRIDGE:
      player_mesg(w, order->player,
		       "Build Bridge Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case DESTROY_FERTILE:
      player_mesg(w, order->player,
		       "Destroy Fertile Order (P%d-%s %s).\n",
		       order->player, w->player[order->player]->code, source);
      break;
    case DESTROY_BRIDGE:
      player_mesg(w, order->player,
		       "Destroy Bridge Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case ERUPT_VOLCANO:
      player_mesg(w, order->player, "Erupt Volcano (%s).\n", source);
      break;
    case COLLAPSE_BRIDGE:
      player_mesg(w, order->player, "Collapse Bridge (%s).\n", source);
      break;
    case MOVE_WHIRLPOOL:
      player_mesg(w, order->player, "Move Whirlpool (%s %s).\n",
		       source, dest);
      break;
    case CHANT:
      player_mesg(w, order->player, "Chant Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case LOB:
      player_mesg(w, order->player, "Lob Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case FREE:
      player_mesg(w, order->player, "Free Order (P%d-%s %s).\n",
		       order->player, w->player[order->player]->code, source);
      break;
    case BUILD_RAFT:
      player_mesg(w, order->player, "Build Raft Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case SAIL:
      player_mesg(w, order->player, "Sail Order (P%d-%s %s %s).\n",
		       order->player, w->player[order->player]->code,
		       source, dest);
      break;
    case ERUPT_MOUNTAIN:
      player_mesg(w, order->player, "Erupt Mountain (%s).\n", source);
      break;
    case DORMANT_VOLCANO:
      player_mesg(w, order->player, "Dormant Volcano (%s).\n", source);
      break;
    case KILL_PLAYER:
      player_mesg(w, order->player, "Kill Player Order (P%d-%s).\n",
	      order->player, w->player[order->player]->code);
      break;
#ifdef IgnoreThisStuff
    case GIVE:
      player_mesg(w, order->player, "Give Order (P%d-%s %s to P%d-%s).\n",
	      order->player, w->player[order->player]->code, work_string,
	      order->moves [0], world->player[order->moves [0]]->code);

      break;
#endif
    }
}

void new_show_order(world_t *w, order_t *order)
{
  for (; order; order = order->chain)
    show_order (w, order);
}
