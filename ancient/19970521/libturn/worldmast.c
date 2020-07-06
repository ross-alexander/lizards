#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "orders.h"
#include "moreords.h"
#include "terrain.h"
#include "blddest.h"

#include "filefunc.h"

#include "lizgame.h"
#include "lizmesg.h"
#include "lizmisc.h"
#include "lizord.h"

int main (int argc, char *argv []);
void DoPass(World *, int);
void WorldMast(World *);

char *message_list [MAX_RANDOM_MESSAGES];

/* ----------------------------------------------------------------------
--
-- DoPass
--
---------------------------------------------------------------------- */
void DoPass(World *world, int pass)
{
  int player, order_num;
  order_t **order_list = world->order_list;

  for (player = 1; player <= world->num_players; player++)
    {
      switch (pass)
	{
	case 0:
	  fprintf (world->player[player].fptr, "Phase(artillery)\n");
	  break;
	case 1:
	  fprintf (world->player[player].fptr, "Phase(movement)\n");
	  break;
	case 2:
	  fprintf (world->player[player].fptr, "Phase(development)\n");
	  break;
	}
    }

  if (pass == -1)
    printf ("LIZARDS! SYSTEM PASS:\n");
  else
    printf ("PASS %d:\n", pass);

  for (order_num = 0; order_num < world->num_orders; order_num ++)
    {
      switch (order_list [order_num]->order_type)
	{
	case MOVE:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      move_order (order_list [order_num]);
	    }
	  break;

	case SPLIT:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      split_order (order_list [order_num]);
	    }
	  break;
	  
	case BUILD_FERTILE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      build_fertile_order (order_list [order_num]);
	    }
	  break;
	  
	case BUILD_DEN:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      build_den_order (order_list [order_num]);
	    }
	  break;

	case BUILD_BRIDGE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      build_bridge_order (order_list [order_num]);
	    }
	  break;
	
	case DESTROY_FERTILE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      destroy_fertile_order (order_list [order_num]);
	    }
	  break;
	  
	case DESTROY_BRIDGE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      destroy_bridge_order (order_list [order_num]);
	    }
	  break;
	  
	case RECRUIT:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      fprintf (world->player[player].fptr, "\n");
	      recruit_order (order_list [order_num]);
	    }
	  break;
	  
	case ERUPT_VOLCANO:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      erupt_volcano_order (order_list [order_num]);
	    }
	  break;
	  
	case COLLAPSE_BRIDGE:
	  if (pass == 1)
	    {
	      printf ("Collapse Bridge Order.\n");
	      collapse_bridge_order (order_list [order_num], 0, 0);
	    }

	  break;
	case MOVE_WHIRLPOOL:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      move_whirlpool_order (order_list [order_num]);
	    }
	  break;

	case CHANT:
	  if (pass == 0)
	    {
	      show_order (order_list [order_num]);
	      chant_order (order_list [order_num]);
	    }
	  break;

	case GIVE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      give_order (order_list [order_num]);
	    }
	  break;
	  
	case FREE:
	  if (pass == 0)
	    {
	      show_order (order_list [order_num]);
	      free_order (order_list [order_num]);
	    }
	  break;
	
	case LOB:
	  if (pass == 0)
	    {
	      show_order (order_list [order_num]);
	      lob_order (order_list [order_num]);
	    }
	  break;

	case BUILD_RAFT:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      build_raft_order (order_list [order_num]);
	    }
	  break;
	  
	case SAIL:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      move_order (order_list [order_num]);
	    }
	  
	  break;
	case ERUPT_MOUNTAIN:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      erupt_mountain_order (order_list [order_num]);
	    }
	  break;

	case DORMANT_VOLCANO:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);	      
	      dormant_volcano_order (order_list [order_num]);
	    }
	  break;

	case KILL_PLAYER:
	  if (pass == -1)
	    {
	      show_order (order_list [order_num]);
       	      kill_player_order (order_list [order_num]);
	    }
	  break;

	case SNEAK:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      sneak_order (order_list [order_num]);
	    }
	  break;

	default:
	  printf ("Inactive Order.\n");
	  break;
	}
    }
}

void WorldMast(World *world)
{
  int pass;
  get_message_file();

  for (pass = -1; pass < 3; pass ++)
    {
      DoPass(world, pass);
      printf ("PASS %d END.\n", pass);
    }
}
