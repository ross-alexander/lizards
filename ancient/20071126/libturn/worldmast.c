#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizmesg.h"
#include "lizmisc.h"

#include "orders.h"
#include "moreords.h"
#include "terrain.h"
#include "blddest.h"


char *message_list [MAX_RANDOM_MESSAGES];

/* ----------------------------------------------------------------------
--
-- DoPass
--
---------------------------------------------------------------------- */
static void DoPass(world_t *w, int pass)
{
  int player, order_num, result;
  order_t *order;

  for (player = 1; player < w->num_players; player++)
    {
      switch (pass)
	{
	case 1:
	  player_mesg (w, player, "(artillery-phase)\n");
	  break;
	case 2:
	  player_mesg (w, player, "(movement-phase)\n");
	  break;
	case 3:
	  player_mesg (w, player, "(development-phase)\n");
	  break;
	}
    }

  if (pass == 0)
    printf ("LIZARDS! SYSTEM PASS:\n");
  else
    printf ("PASS %d:\n", pass);

  for (order_num = 0; order_num < w->num_orders; order_num ++)
    {
      result = 1;
      order = w->order_list[order_num];
      while (order)
	{
	  if (order->status == ORDER_IGNORE)
	    continue;
	  switch (order->type)
	    {
	    case MOVE:
	      if (pass == 2)
		{
		  show_order (w, order);
		  order->status = ORDER_CURRENT;
		  result = move_order (w, order);
		  order->status = ORDER_COMPLETE;
		}
	      break;
	    case SPLIT:
	      if (pass == 2)
		{
		  show_order (w, order);
		  order->status = ORDER_CURRENT;
		  result = split_order (w, order);
		  order->status = ORDER_COMPLETE;
		}
	      break;
	    case SNEAK:
	      if (pass == 2)
		{
		  show_order (w, order);
		  order->status = ORDER_CURRENT;
		  result = sneak_order (w, order);
		  order->status = ORDER_COMPLETE;
		}
	      break;
	    case RECRUIT:
	      if (pass == 3)
		{
		  show_order (w, order);
		  order->status = ORDER_CURRENT;
		  result = recruit_order (w, order);
		  order->status = ORDER_COMPLETE;
		}
	      break;
	    case BUILD_FERTILE:
	      if (pass == 3)
		{
		  show_order (w, order);
		  order->status = ORDER_CURRENT;
		  build_fertile_order (w, order);
		  order->status = ORDER_COMPLETE;
		}
	      break;
#ifdef X  	      
	    case BUILD_DEN:
	      if (pass == 3)
		{
		  show_order (order_list [order_num]);
		  build_den_order (order_list [order_num]);
		}
	      break;
	      
	    case BUILD_BRIDGE:
	      if (pass == 3)
		{
		  show_order (order_list [order_num]);
		  build_bridge_order (order_list [order_num]);
		}
	      break;
	      
	    case DESTROY_FERTILE:
	      if (pass == 3)
		{
		  show_order (order_list [order_num]);
		  destroy_fertile_order (order_list [order_num]);
		}
	      break;
	      
	    case DESTROY_BRIDGE:
	      if (pass == 3)
		{
		  show_order (order_list [order_num]);
		  destroy_bridge_order (order_list [order_num]);
		}
	      break;
	    case ERUPT_VOLCANO:
	      if (pass == 2)
		{
		  show_order (order_list [order_num]);
		  erupt_volcano_order (order_list [order_num]);
		}
	      break;
	      
	    case COLLAPSE_BRIDGE:
	      if (pass == 2)
		{
		  printf ("Collapse Bridge Order.\n");
		  collapse_bridge_order (order_list [order_num], 0, 0);
		}
	      
	      break;
	    case MOVE_WHIRLPOOL:
	      if (pass == 2)
		{
		  show_order (order_list [order_num]);
		  move_whirlpool_order (order_list [order_num]);
		}
	      break;
	      
	    case CHANT:
	      if (pass == 1)
		{
		  show_order (order_list [order_num]);
		  chant_order (order_list [order_num]);
		}
	      break;
	      
	    case GIVE:
	      if (pass == 3)
		{
		  show_order (order_list [order_num]);
		  give_order (order_list [order_num]);
		}
	      break;
	      
	    case FREE:
	      if (pass == 1)
		{
		  show_order (order_list [order_num]);
		  free_order (order_list [order_num]);
		}
	      break;
	      
	    case LOB:
	      if (pass == 1)
		{
		  show_order (order_list [order_num]);
		  lob_order (order_list [order_num]);
		}
	      break;
	      
	    case BUILD_RAFT:
	      if (pass == 3)
		{
		  show_order (order_list [order_num]);
		  build_raft_order (order_list [order_num]);
		}
	      break;
	      
	    case SAIL:
	      if (pass == 2)
		{
		  show_order (order_list [order_num]);
		  move_order (order_list [order_num]);
		}
	      
	      break;
	    case ERUPT_MOUNTAIN:
	      if (pass == 2)
		{
		  show_order (order_list [order_num]);
		  erupt_mountain_order (order_list [order_num]);
		}
	      break;
	      
	    case DORMANT_VOLCANO:
	      if (pass == 2)
		{
		  show_order (order_list [order_num]);	      
		  dormant_volcano_order (order_list [order_num]);
		}
	      break;
	      
	    case KILL_PLAYER:
	      if (pass == 0)
		{
		  show_order (order_list [order_num]);
		  kill_player_order (order_list [order_num]);
		}
	      break;
#endif	      
	    default:
	      printf ("Inactive Order.\n");
	      break;
	    }
	  if (result)
	    order = order->chain;
	  else
	    {
	      fprintf(stdout, "Order failed.  Subsequent orders in chain ignored.\n");
	      order = NULL;
	    }
	}
    }
}

void WorldMast(world_t *world)
{
  int pass;
  get_message_file();

  for (pass = 0; pass < 4; pass ++)
    {
      DoPass(world, pass);
      printf ("PASS %d END.\n", pass);
    }
}
