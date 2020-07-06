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
-- Recruit Order
--
---------------------------------------------------------------------- */
int recruit_order (world_t *w, order_t *order)
{
  FILE *fptr;
  char source[5];
  int work,
  work2;
  hex_t *hex;
  feature_t *f, *band;

  xy_to_string (order->source.x, order->source.y, source);
  open_player_mesg (w, order->player, &fptr);

  hex = get_hex(w, order->source);

  if (!(f = has_feature(hex, DEN)) ||
      (hex->owner != order->player))
    {
      fprintf (fptr,
	       "(recruit-failed %s \"Den isn't yours or doesn't do exist\")\n",
	       source);
      return 0;
    }
  if (f->f.den.type == 0)
    {
      fprintf (fptr, "(recruit-failed %s \"Attempted to recruit from an un-settled Den\")\n", source);
      return 0;
    }

  if (w->player[order->player]->home_den == 0)
    {
      if (f->type == DEN)
	fprintf (fptr, " - Attempted to recruit from %s Den at %s. Alas, your Clan no longer control a Home Den.\n",
		 liz_colours [f->f.den.type], source);
      else
	  fprintf (fptr, " - Attempted to recruit from Home Den at %s. Alas, your Clan no longer control a Home Den.\n",
		   source);
      fprintf (fptr, " - None of the Den Lizards feel compelled to join your Clan.\n");
      return 0;
    }
  work2 = lizards_in_hex(hex);
  if (work2 >= MAX_LIZARDS_IN_HEX)
    {
      if (f->type == DEN)
	fprintf (fptr, " - Attempted to recruit from %s Den at %s but hex already contains %d of your Lizards.\n",
		 liz_colours [f->f.den.type], source, MAX_LIZARDS_IN_HEX);
      else
	fprintf (fptr, " - Attempted to recruit from Home Den at %s but hex already contains %d of your Lizards.\n",
		 source, work2);
      return 0;
    }

  if (f->type == DEN)
    fprintf (fptr, " - Recruiting from %s Den at %s. ",
	     liz_colours [f->f.den.type],
	     source);
  else
    fprintf (fptr, " - Recruiting from Home Den at %s. ", source);

  work = (int)(((double) f->f.den.pop) * DEN_RECRUIT_LEVEL);

  if (work + work2 > MAX_LIZARDS_IN_HEX)
    work = MAX_LIZARDS_IN_HEX - work2;

  if ((band = has_feature(hex, BAND)) == NULL)
    {
      band = create_feature(w, BAND, order->source);
      add_feature(w, band, hex);
    }
#ifdef X
  world_hex.combat_points =
    average_combat_points (lizards_in_hex (&world_hex),
			   world_hex.combat_points,
			   work, 0);

  world_hex.turns_hungry =
    average_hunger_points (lizards_in_hex (&world_hex), world_hex.turns_hungry,
			   work, 0);
#endif
  f->f.den.pop -= work;
  band->f.band.type[f->f.den.type - 1] += work;
  fprintf (fptr, "%d %s lizards recruited from Den", work,
	   liz_colours[f->f.den.type]);
  fprintf (fptr, " leaving %d Den Lizards at %s.\n",
	   f->f.den.pop,
	   source);

  for (work = 0; work < w->num_orders; work ++)
    if (w->order_list [work] != order &&
	coord_eq(w->order_list [work]->source, order->source) &&
	w->order_list [work]->player == order->player &&
	w->order_list [work]->type == RECRUIT)
      {
	fprintf (fptr, " - Next Recruit Order at %s ignored.\n",
		 source);
	w->order_list [work]->status = ORDER_IGNORE;
      }
  return 1;
}
