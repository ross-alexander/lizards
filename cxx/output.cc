#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include <cairo/cairo.h>
#include <librsvg/rsvg.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

/*
              +-----+
             /       \
            /         \
           /           \
    +-----+   - 2r -    +
   /       \           /
  /    |    \         /
 /     |     \       /
+   sqrt(3)   +-----+
 \     |     /
  \    |    /
   \       /
    +-----+
       |<- 1.5r->|

*/

Outputs output_t::order(hex_t *hex)
{
  feature_t *f;
  if (hex->has_feature(BRIDGE))
    return OUTPUT_BRIDGE;
  else if (hex->has_feature(RAFT))
    return OUTPUT_RAFT;
  else if (hex->has_feature(BUILDING))
    return OUTPUT_BUILDING;
  else if ((f = hex->has_feature(DEN)))
    {
      den_t *den = (den_t*)f;
      if (den->home)
	return OUTPUT_HOME_DEN;
      else
	{
	  switch (den->colour)
	    {
	    case UNSETTLED:
	      return OUTPUT_EMPTY_DEN;
	      break;
	    case RED:
	      return OUTPUT_RED_DEN;
	      break;
	    case GREEN:
	      return OUTPUT_GREEN_DEN;
	      break;
	    case GREY:
	      return OUTPUT_GREY_DEN;
	      break;
	    case BLACK:
	      return OUTPUT_BLACK_DEN;
	      break;
	    case YELLOW:
	      return OUTPUT_YELLOW_DEN;
	      break;
	    }
	}
    }
  else if (hex->has_feature(RUIN))
    return OUTPUT_RUIN;
  else if (hex->has_feature(FERTILE))
    return OUTPUT_FERTILE;
  else if (hex->has_feature(VOLCANO))
    return OUTPUT_VOLCANO;
  else if (hex->has_feature(WHIRLPOOL))
    return OUTPUT_WHIRLPOOL;
  else if (hex->has_feature(CURSED))
    return OUTPUT_CURSED;
  else if (hex->has_feature(TEMPLE))
    return OUTPUT_TEMPLE;
  else if (hex->has_feature(PEAK))
    return OUTPUT_PEAK;
  else if (hex->has_feature(SWAMP))
    return OUTPUT_SWAMP;
  else if (hex->has_feature(SCRUB))
    return OUTPUT_SCRUB;
  else
    {
      switch (hex->terrain)
	{
	case PLAIN:
	  return OUTPUT_PLAIN;
	  break;
	case WATER:
	  return OUTPUT_WATER;
	  break;
	}
    }
  return OUTPUT_BLANK;
}


output_t::output_t(world_t *w)
{
  world = w;
}

void output_t::display(void)
{
  printf("Lizards default output.\n");
}
