#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "lizord.h"
#include "lizmisc.h"

extern char player_code [MAX_PLAYERS + 1][6],
            game_code [5];

extern order_t *order_list [MAX_ORDERS_PER_TURN];

extern int world_x,
	   world_y,
	   number_orders,
	   turn_number;

extern char dir [7][3];

void hex_attacked_or_moved_into (int x, int y, hex_t *hex)
{
  FILE *fptr;

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

      fclose (fptr);
    }
}

void show_order (order_t *order)
{
  char work_string [255];

  xy_to_string (order->x_hex, order->y_hex, work_string);

  switch (order->order_type)
  {
    case MOVE:
      printf ("Move Order (P%d-%s %s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]], dir [order->moves [1]]);

      break;
    case SPLIT:
      printf ("Split Order (P%d-%s %s %dred %dgrn %dgry %dblk %dyel %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      order->red_lizards, order->green_lizards, order->grey_lizards,
	      order->black_lizards, order->yellow_lizards,
	      dir [order->moves [0]], dir [order->moves [1]]);

      break;
    case BUILD_FERTILE:
      printf ("Build Fertile Order (P%d-%s %s).\n",
	      order->player, player_code [order->player], work_string);

      break;
    case BUILD_DEN:
      printf ("Build Den Order (P%d-%s %s).\n",
	       order->player, player_code [order->player], work_string);

      break;
    case BUILD_BRIDGE:
      printf ("Build Bridge Order (P%d-%s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]]);

      break;
    case DESTROY_FERTILE:
      printf ("Destroy Fertile Order (P%d-%s %s).\n",
	      order->player, player_code [order->player], work_string);

      break;
    case DESTROY_BRIDGE:
      printf ("Destroy Bridge Order (P%d-%s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]]);

      break;
    case RECRUIT:
      printf ("Recruit Order (P%d-%s %s).\n",
	      order->player, player_code [order->player], work_string);

      break;
    case ERUPT_VOLCANO:
      printf ("Erupt Volcano (%s).\n", work_string);

      break;
    case COLLAPSE_BRIDGE:
      printf ("Collapse Bridge (%s).\n", work_string);

      break;
    case MOVE_WHIRLPOOL:
      printf ("Move Whirlpool (%s %s %s).\n", work_string,
	      dir [order->moves [0]], dir [order->moves [1]]);

      break;
    case CHANT:
      printf ("Chant Order (P%d-%s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]]);

      break;
    case LOB:
      printf ("Lob Order (P%d-%s %s at_x%d at_y%d).\n",
	      order->player, player_code [order->player], work_string,
	      order->moves [0], order->moves [1]);

      break;
    case GIVE:
      printf ("Give Order (P%d-%s %s to P%d-%s).\n",
	      order->player, player_code [order->player], work_string,
	      order->moves [0], player_code [order->moves [0]]);

      break;
    case FREE:
      printf ("Free Order (P%d-%s %s).\n",
	      order->player, player_code [order->player], work_string);

      break;
    case BUILD_RAFT:
      printf ("Build Raft Order (P%d-%s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]]);

      break;
    case SAIL:
      printf ("Sail Order (P%d-%s %s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]], dir [order->moves [1]]);

      break;
    case ERUPT_MOUNTAIN:
      printf ("Erupt Mountain (%s).\n", work_string);

      break;
    case DORMANT_VOLCANO:
      printf ("Dormant Volcano (%s).\n", work_string);

      break;
    case KILL_PLAYER:
      printf ("Kill Player Order (P%d).\n",
	      order->player, player_code [order->player]);

      break;
    case SNEAK:
      printf ("Sneak Order (P%d-%s %s %s %s).\n",
	      order->player, player_code [order->player], work_string,
	      dir [order->moves [0]], dir [order->moves [1]]);

      break;
  }
}
