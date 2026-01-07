#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizhex.h"
#include "lizmisc.h"
#include "create.h"

void WorldFix(world_t *world)
{
  int fertile,
      dir,
      loop,
      x,
      y;
  coord xy, xy2;
  hex_t *hex,
        *adj_hex;
  feature_t *den, *volcano, *band, *f;

  printf ("Fixing World...\n");
  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	xy = coord_cons(x, y);
	hex = get_hex (world, xy);

	if ((den = has_feature(hex, DEN)) && !hex->tmpl)
	  {
	    band = has_feature(hex, BAND);
	    if ((fertile = feature_adjacent (world, xy, FERTILE)) == 0)
	      {
		dir = random (6);
		for (loop = 0; loop < 6; loop++)
		  {
		    xy2 = move_coords (world, hex, dir+1);
		    if (!xy2.invalid)
		      {
			/* get and check hex not part of player's template */
			adj_hex = get_hex (world, xy2);
			if (!adj_hex->tmpl && !has_feature(adj_hex, DEN))
			  {
			    /* convert to fertile and stop */
			    adj_hex->terrain = PLAIN;
			    clear_features(world, adj_hex);
			    add_feature(world,
					create_feature(world, FERTILE, adj_hex->xy),
					adj_hex);
			    break;
			  }
		      }
		    dir = (dir + 1) % 6;
		  }
		if (loop != 6) /* broken on new fertile */
		  {
		    den->f.den.pop = DEN_LIZARD_LEVEL;
		    band->f.band.type[den->f.den.type - 1] = (int)((double)den->f.den.pop * DEN_MILITIA_LEVEL);
		  }
		else
		  {
		    /* convert den to ruin */
		    free(release_feature(world, band));
		    free(release_feature(world, den));
		    add_feature(world, create_feature(world, RUIN, xy), hex);
		  }
	      }
	    else if (den->f.den.pop > DEN_LIZARD_LEVEL * fertile)
	      {
		den->f.den.pop = DEN_LIZARD_LEVEL * fertile;
		band->f.band.type[den->f.den.type - 1] = (int)((double)den->f.den.pop *  DEN_MILITIA_LEVEL);
	      }
	  }
	if ((volcano = has_feature(hex, VOLCANO)))
	  {
	    if (((feature_adjacent (world, xy, FERTILE) > 0) ||
		 (feature_adjacent (world, xy, DEN) > 0) || !hex->tmpl))
	      {
		/* change it into a peak, UNLESS IT IS ON A PLAYER TEMPLATE
		   (in which case it will have already been checked for
		   template validity, so may effect the world OFF the
		   template. */
		free(release_feature(world, volcano));
	      }
	  }
      }
  
  /* double check for any dens affected by changes */

  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      {
	xy = coord_cons(x, y);
	hex = get_hex (world, xy);
	if ((den = has_feature(hex, DEN)) && !hex->tmpl)
	  {
	    band = has_feature(hex, BAND);
	    fertile = feature_adjacent (world, xy, FERTILE);
	    if (den->f.den.pop != DEN_LIZARD_LEVEL * fertile)
	      {
		den->f.den.pop = DEN_LIZARD_LEVEL * fertile;
		band->f.band.type[den->f.den.type - 1] = (int)((double)(den->f.den.pop) *  DEN_MILITIA_LEVEL);
	      }
	  }
      }
  for (y = 0; y < world->y; y ++)
    for (x = 0; x < world->x; x ++)
      {
	xy = coord_cons(x, y);
	hex = get_hex (world, xy);
	for (f = hex->chain; f != NULL; f = f->chain)
	  f->owner = hex->owner;
      }
}
