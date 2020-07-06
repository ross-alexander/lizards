#include <vector>
#include <map>
#include <string>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <string.h>
#include <assert.h>
#include <cairomm/context.h>
#include <librsvg/rsvg.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- point
--
---------------------------------------------------------------------- */

point_t::point_t() { x = y = 0; };

point_t::point_t(int x_p, int y_p) { x = x_p; y = y_p; };
int point_t::operator==(const point_t& p)
{
  return (x == p.x && y == p.y);
};
int point_t::operator!=(const point_t& p)
{
  return (x != p.x || y != p.y);
};


point_t& point_t::operator+=(const point_t& p)
{
  x += p.x;
  y += p.y;
  return *this;
}

point_t& point_t::operator-=(const point_t& p)
{
  x -= p.x;
  y -= p.y;
  return *this;
}

point_t point_t::operator+(const point_t &p)
{
  return point_t(x + p.x, y + p.y);
}

point_t point_t::operator-(const point_t &p)
{
  return point_t(x - p.x, y - p.y);
}

point_t& point_t::clip(int w, int h)
{
  x = (x%w + w)%w;
  y = (y%h + h)%h;
  return *this;
}

std::string point_t::tostring()
{
  return fmt::sprintf("point(%02d, %02d)", x, y);
}

/* ----------------------------------------------------------------------
--
-- fpoint
--
---------------------------------------------------------------------- */


fpoint_t::fpoint_t()
{
  x = y = 0.0;
}

fpoint_t::fpoint_t(double px, double py)
{
  x = px;
  y = py;
}

fpoint_t fpoint_t::operator+(const fpoint_t &p)
{
  return fpoint_t(x + p.x, y + p.y);
}

/* ----------------------------------------------------------------------
--
-- fbox
--
---------------------------------------------------------------------- */

fbox_t::fbox_t() { left = top = right = bottom = 0.0; empty = 1;}

fbox_t::fbox_t(double l, double t, double r, double b)
{
  left = l;
  top = t;
  right = r;
  bottom = b;
  empty = 0;
}

fbox_t& fbox_t::setunion(fbox_t b)
{
  fbox_t& a = (*this);
  if (empty)
    {
      left = b.left;
      top = b.top;
      right = b.right;
      bottom = b.bottom;
      empty = 0;
    }
  else
    {
      if (b.left < a.left) a.left = b.left;
      if (b.right > a.right) a.right = b.right;
      if (b.top < a.top) a.top = b.top;
      if (b.bottom > a.bottom) a.bottom = b.bottom;
    }
  return a;
}

std::string fbox_t::tostring()
{
  return empty ? fmt::sprintf("fbox_t({}}") : fmt::sprintf("fbox_t(%5.2f %5.2f %5.2f %5.2f)", left, top, right, bottom);
}

/* ----------------------------------------------------------------------
--
-- fplace
--
---------------------------------------------------------------------- */

fplace_t::fplace_t()
{
}


fplace_t::fplace_t(point_t p, double radius, double border)
{
  double yshift = (double)(abs(p.x) % 2) / 2.0;
  double cx = 1.50 * p.x * radius;
  double cy = sqrt(3) * ((double)p.y - yshift) * radius;
  
  center = fpoint_t(cx, cy);
  bounds.left = cx - radius - border;
  bounds.right = cx + radius + border;
  bounds.top = cy - sqrt(3)/2 * radius - border;
  bounds.bottom = cy + sqrt(3)/2 * radius + border;
  
  for (int j = 0; j < 6; j++)
    {
      double x = cos(j * 60 * M_PI / 180);
      double y = sin(j * 60 * M_PI / 180);
      hull.push_back(fpoint_t(cx + x * radius, cy + y * radius));
    }
}

