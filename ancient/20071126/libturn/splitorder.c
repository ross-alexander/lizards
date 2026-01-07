#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "orders.h"
#include "lizhex.h"
#include "lizmesg.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- Split Order
--
---------------------------------------------------------------------- */
int split_order (world_t *w, order_t *order)
{
  FILE *fptr;
  char source[5], dest[5];
  feature_t *f;
  int i, result;
  order_t *otmp;

  hex_t *hex;

  xy_to_string (order->source.x, order->source.y, source);
  xy_to_string (order->dest.x, order->dest.y, dest);

  open_player_mesg (w, order->player, &fptr);
  hex = get_hex(w, order->source);

  if ((lizards_in_hex (hex) > 0 && hex->owner != order->player) ||
      lizards_in_hex (hex) == 0)
    {
      fprintf (fptr, " - Split %s to %s ", source, dest);
      fprintf (fptr, "order ignored. NO Lizards in hex.\n");
      return 0;
    }
  f = has_feature(hex, BAND);
  for (i = 0; i < LIZARD_TYPES; i++)
    if (order->feature.f.band.type[i] > f->f.band.type[i])
      if (f->f.band.type[i] == 0)
	{
	  fprintf (fptr, " - Split %s %s ", source, dest);
	  fprintf (fptr, "order ignored. No %s Lizards in hex.\n",
		   liz_colours[i + 1]);
	  return 0;
	}
      else
	order->feature.f.band.type[i] = f->f.band.type[i];
  if (f->f.band.immobile > 0)
    {
      fprintf (fptr, " - Split %s %s ", source, dest);
      if (f->f.band.immobile > 2)
	fprintf (fptr, "order ignored. Lizards are FROZEN this turn.\n");
      else
	fprintf (fptr, "order ignored. Lizards are MIRED in Swamp this turn.\n");
      return 0;
    }
  
  if (order->player > 0)
    if (lizards_in_band(&order->feature) == 1)
      {
	if (player_spy_here (w, order->source.x,order->source.y, order->player))
	  {	    
	    /* don't do anything as spy lizard already here */ 
	  }
	else
	  {
	    if ((w->spylist[w->num_spies] = calloc (1,sizeof(spy_t))) == NULL)
	      {
		fprintf (stderr, "FATAL ERROR: Out of memory.\n");
		exit (EXIT_FAILURE);
	      }
	    w->spylist [w->num_spies]->player = order->player;
	    w->spylist [w->num_spies]->x_hex = order->source.x;
	    w->spylist [w->num_spies]->y_hex = order->source.y;
	    w->spylist [w->num_spies]->turns_alone = 0;
	    w->spylist [w->num_spies]->mired = 0;
	    w->spylist [w->num_spies]->moved_this_turn = 0;
	    for (i = 0; i < LIZARD_TYPES; i++)
	      if (order->feature.f.band.type[i])
		{
		  f->f.band.type[i] -= 1;
		  w->spylist [w->num_spies]->lizard_type = i;
		}
	  }
	w->num_spies++;
	result = sneak_order (w, order);
	for (otmp = order; otmp; otmp = otmp->chain)
	  if (otmp->type == SPLIT)
	    otmp->type = SNEAK;
      }
    else
      result = move_order (w, order);

#ifdef X
  /* adjust any further moves from hex to compensate for split lizards */

  get_hex(w, old_order.x_hex, old_order.y_hex, &world_hex);

  for (work = 0; work < w->num_orders; work ++)
    if (w->order_list [work] != order &&
	w->order_list [work]->x_hex == old_order.x_hex &&
	w->order_list [work]->y_hex == old_order.y_hex &&
	w->order_list [work]->player == old_order.player &&
	(w->order_list [work]->order_type == MOVE ||
	 w->order_list [work]->order_type == SAIL))
      {
	if (w->order_list [work]->red_lizards > world_hex.red_lizards)
	  w->order_list [work]->red_lizards = world_hex.red_lizards;

	if (w->order_list [work]->green_lizards > world_hex.green_lizards)
	  w->order_list [work]->green_lizards = world_hex.green_lizards;

	if (w->order_list [work]->grey_lizards > world_hex.grey_lizards)
	  w->order_list [work]->grey_lizards = world_hex.grey_lizards;

	if (w->order_list [work]->black_lizards > world_hex.black_lizards)
	  w->order_list [work]->black_lizards = world_hex.black_lizards;

	if (w->order_list [work]->yellow_lizards > world_hex.yellow_lizards)
	  w->order_list [work]->yellow_lizards = world_hex.yellow_lizards;

	if (w->order_list [work]->red_lizards + w->order_list [work]->green_lizards +
	    w->order_list [work]->grey_lizards + w->order_list [work]->black_lizards +
	    w->order_list [work]->yellow_lizards == 0)
	  {
	    open_player_mesg (w->order_list [work]->player, &fptr);

	    xy_to_string (w->order_list [work]->x_hex, w->order_list [work]->y_hex,
			  work_string);

	    if (w->order_list [work]->order_type == MOVE)
	      fprintf (fptr, "\n - Split %s (", work_string);
	    else
	      fprintf (fptr, "\n - Sail %s (", work_string);

	    if (w->order_list [work]->moves [1] != 0)
	      fprintf (fptr, "%s %s) ", dir [w->order_list [work]->moves [0]],
		       dir [w->order_list [work]->moves [1]]);
	    else
	      fprintf (fptr, "%s) ", dir [w->order_list [work]->moves [0]]);

	    if (lizards_in_hex (&world_hex) > 0)
	      fprintf (fptr, "order ignored. ALL Lizards in hex have already moved.\n");
	    else
	      fprintf (fptr, "order ignored. NO Lizards in hex.\n");
	    w->order_list [work]->status = IGNORE;
	  }
      }
#endif
  return 1;
}
