#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "orders.h"
#include "moreords.h"
#include "terrain.h"
#include "blddest.h"
#include "chants.h"

#include "lizmesg.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- Sneak Order
--
---------------------------------------------------------------------- */
int sneak_order (world_t *w, order_t *order)
{
  FILE *fptr;
  char source[5], dest[5];

  spy_t *spy;
  hex_t *old_hex, *new_hex;

  xy_to_string (order->source.x, order->source.y, source);
  xy_to_string (order->dest.x, order->dest.y, dest);

  spy = player_spy_here (w, order->source.x, order->source.y, order->player);
  old_hex = get_hex(w, order->source);
  if (spy)
    {
      /* Spy Move Order */
      open_player_mesg (w, order->player, &fptr);

      if (spy->moved_this_turn > 1)
	{
	  fprintf (fptr, " - Sneak %s %s ", source, dest);
	  fprintf (fptr, "order ignored. Spy Lizard has already moved this turn.\n");
	  return 0;
	}
      if (spy->mired > 0)
	{
	  fprintf (fptr, " - Sneak %s %s ", source, dest);
	  fprintf (fptr, "order ignored. Spy Lizard is MIRED in Swamp this turn.\n");
	  return 0;
	}
      spy->x_hex = order->dest.x;
      spy->y_hex = order->dest.y;
      spy->moved_this_turn += 1;
    
      if (order->type == SPLIT)
	fprintf (fptr, " - Spy Lizard splits at %s and moves to %s\n",
		 source, dest);
      else
	fprintf (fptr, " - Spy Lizard sneaks from %s and moves to %s\n",
		 source, dest);

      new_hex = get_hex(w, order->dest);
#ifdef X
  
      if ((new_hex.terrain == DEN || new_hex.terrain == HOME_DEN) &&
	  new_hex.hex_owner != order->player)
	{
      	  /* Spy captured by Enemy Den! */
	  xy_to_string (x, y, work_string);
	  if (new_hex.hex_owner > 0)
	    fprintf (fptr," - %s\n",
		     build_mesg (random (4) + 28, work_string,
				 w->player[new_hex.hex_owner]->clan));
	  else
	    fprintf (fptr, " - Spy Lizard captured by Den at %s.\n",
		     work_string);
      	  if (new_hex.hex_owner > 0)
	    {
	      open_player_mesg (new_hex.hex_owner, &fptr2);
	      fprintf (fptr2,
		       "\n - %s\n",
		       build_mesg (random (4) + 24, work_string,
				   w->player[order->player]->clan));
	    }
      	  w->spylist [spy]->player = 0;
	  return 0;
	}
      else
	{
	  switch (new_hex.terrain)
	    {
	    case WATER:
	      if (new_hex.bridge_here == 0 && old_hex.raft_here == 0 &&
		  new_hex.raft_here == 0 && random (100) < SEA_MONSTER_LEVEL
		  && terrain_adjacent (world, x, y, WATER) > 0)
		{
		  /* Spy consumed by Sea Monster */
		  xy_to_string (x, y, work_string);
		  fprintf (fptr, " - %s\n", build_mesg (random (4), work_string, ""));
		  w->spylist [spy]->player = 0;
		  return 0;
		}
	      break;
	    case WHIRLPOOL:
	      if (new_hex.bridge_here == 0 &&  old_hex.raft_here == 0 &&
		  new_hex.raft_here == 0)
		{
		  xy_to_string (x, y, work_string);
		  fprintf (fptr, " - Spy Lizard moved into Whirlpool at %s.\n",
			   work_string);
		  fprintf (fptr, " - %s\n",
			   build_mesg (random (4) + 40, work_string, ""));
		  w->spylist [spy]->player = 0;
		  return 0;
		}	  
	      break;
	    case SWAMP:
	      if (random (100) < SWAMP_MIRE_LEVEL)
		{
		  /* Spy mired in Swamp */
		  xy_to_string (x, y, work_string);
		  fprintf (fptr,
			   " - Spy Lizard MIRED in Swamp at %s for next turn.\n",
			   work_string);
		  if (work == 0)
		    order->moves [1] = 0;
		  w->spylist [spy]->mired = 2;
		}
	      break;
	    case CURSED:
	      fprintf (fptr, " - Spy Lizard moved into CURSED hex at %s. ",
		       work_string);
	      fprintf (fptr, " Spy Lizard is poisoned and collapses, dead.\n");
	      w->spylist [spy]->player = 0;
	      return;
	    default:
	      break;
	    }
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
							     w->player[new_hex.hex_owner]->clan));
		      else
			fprintf (fptr, " - Spy Lizard captured by Lizards at %s.\n",
				 work_string);
		  
		      if (new_hex.hex_owner > 0)
			{
			  open_player_mesg (new_hex.hex_owner, &fptr2);
		      
			  fprintf (fptr2,"\n - %s\n", build_mesg (random (4) + 32, work_string,
								  w->player[order->player]->clan));
		      
			  /* fclose (fptr2); */
			}
		  
		      w->spylist[spy]->player = 0;
		  
		      /* fclose(fptr); */
		  
		      put_hex(world, x, y, &new_hex);
		  
		      return;
		    }
		}
	    }
	}
#endif
      return 1;
    }
  else
    {
      fprintf(fptr, "(sneak-order-failed %s %s \"No spy in hex\")\n",
	      source, dest);
      return 0;
    }
}
