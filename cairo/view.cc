#include <map>
#include <vector>
#include <string>

#include <cairomm/context.h>
#include <librsvg/rsvg.h>

#include "lizards.h"

view_t::view_t(point_t xy, int s, std::string _id, std::string _owner)
{
  loc = xy; symbol = s, id = _id; owner = _owner;
  //  printf("view_t((%d, %d), %d, %s, %s)\n", xy.x, xy.y, s, _id.c_str(), _owner.c_str());
  band = 0;
}
