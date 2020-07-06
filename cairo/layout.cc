#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <assert.h>

#include <cairomm/context.h>
#include <librsvg/rsvg.h>

#include <json-c/json.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- layout
--
---------------------------------------------------------------------- */

layout_t::layout_t()
{
  size = cost = 0;
}

layout_t::layout_t(int j, int k)
{
  size = j;
  cost = k;
}

layout_hex_t*& layout_t::operator[](const char* id)
{
  return offsets[id];
}

void layout_t::bounds()
{
  assert(size > 0);
  x_min = y_min = INT_MAX;
  x_max = y_max = INT_MIN;

  for(auto const &i : offsets)
    {
      int x = i.second->xy.x;
      int y = i.second->xy.y;

      if (x < x_min) x_min = x;
      if (x > x_max) x_max = x;

      if (y < y_min) y_min = y;
      if (y > y_max) y_max = y;
    }
}

grid_t* layout_t::realize()
{
  template_t *tmpl = new template_t(this);
  return tmpl->realize();
}
