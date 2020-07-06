#include <string>
#include <map>
#include <vector>
#include <typeinfo>
#include <cairomm/context.h>
#include <librsvg/rsvg.h>

#include <assert.h>

#include "lizards.h"

#define min(a, b) (a < b ? a : b)

view_hex_t::view_hex_t()
{
  hex = 0;
  band = 0;
}

view_hex_t::view_hex_t(point_t _p, hex_t *_hex)
{
  xy = _p;
  hex = _hex;
  band = 0;
}

int view_hex_t::mark(int p, int m)
{
  unsigned int mask = 1 << p;
  int old = marking & mask;
  switch(m)
    {
    case OVERLAY_CHECK:
      break;
    case OVERLAY_CLEAR:
      marking = marking & ~mask;
      break;
    case OVERLAY_MARK:
      marking = marking | mask;
      break;
    }
  return old;
}

/* ----------------------------------------------------------------------
--
-- mark_hex
--
---------------------------------------------------------------------- */

void view_t::mark_hex(hex_t* hex, int d)
{
  if (!overlay.count(hex->getid()))
    overlay[hex->getid()] = new view_hex_t(hex->xy, hex);
  if (d > 0)
    for (int i = 0; i < 6; i++)
      {
	point_t npt = grid->move(hex->xy, i);
	mark_hex((*grid)(npt), d-1);
      }
}

/* ----------------------------------------------------------------------
--
-- view_t
--
---------------------------------------------------------------------- */

view_t::view_t(grid_t* g, int p)
{
  grid = g;
  width = g->width;
  height = g->height;

  std::vector<hex_t*> hexes = p == 0 ? g->all() : g->owned(p, OWNED_BAND_ONLY);

  for (auto hex : hexes)
    {
      if (p == 0 && hex)
	{
	  overlay[hex->getid()] = new view_hex_t(hex->xy, hex);
	}
      else if (hex->owner == p && hex->get(BAND))
	{
	  mark_hex(hex, hex->isa(PEAK) ? 3 : 2);
	}
    }
}
