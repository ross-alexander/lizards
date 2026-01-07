#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

/*
       +------+      +------+
       | 1,0  |      | 3,0  |
       |      |      |      |
+------+  1B  +------+  1D  +
| 0,0  |      | 2,0  |      |
|      |      |      |      |
+  1A  +------+  1C  +------+
|      | 1,1  |      | 3,1  |
|      |      |      |      |
+------+  2B  +------+  2D  +
| 0,1  |      | 2,1  |      |
|      |      |      |      |
+  2B  +------+  2C  +------+
|      |      |      |
|      |      |      |
+------+      +------+
*/

point_t map_t::move(point_t &p, Direction dir)
{
  point_t np = p;
  switch (dir)
    {
    case NORTH:
      np.y -= 1;
      break;
    case NORTHEAST:
      np.x += 1;
      if ((p.x % 2) == 1)	// x == odd
	np.y -= 1;
      break;
    case SOUTHEAST:
      np.x += 1;
      if ((p.x % 2) == 0)	// x == even
	np.y += 1;
      break;
    case SOUTH:
      np.y += 1;
      break;
    case SOUTHWEST:
      np.x -= 1;
      if ((p.x % 2) == 0)	// x == odd
	np.y += 1;
      break;
    case NORTHWEST:
      np.x -= 1;
      if ((p.x % 2) == 1)	// x == even
	np.y -= 1;
      break;
    case DIRECTIONS:
      assert(dir >= NORTH && dir <= NORTHWEST);
      break;
    }
  if (np.x < 0)
    np.x += width;
  else if (np.x >= width)
    np.x -= width;
  
  if (np.y < 0)
    np.y += height;
  else if (np.y >= height)
    np.y -= height;
  
  return np;
}

hex_t *map_t::hex(const int s)
{
  if (s > size)
    {
      fprintf(stderr, "Illegal access of hex %d.\n", s);
      exit(1);
    }
  return &hexes[s];
}

hex_t *map_t::xy(const int x, const int y)
{
  assert(x >=0 && x < width);
  assert(y >= 0 && y < height);
  hex_t *h = &hexes[y * width + x];
  return h;
};

hex_t *map_t::hex(const int x, const int y)
{
  if ((x < 0 || x >= width) || (y < 0 || y >= height))
    {
      fprintf(stderr, "Illegal hex access (%d, %d).\n", x, y);
      exit(1);
    }
  hex_t *h = &hexes[y * width + x];
  assert(h->xy.x == x && h->xy.y == y);
  return h;
};

hex_t *map_t::hex(const point_t p)
{
  return hex(p.x, p.y);
}

hex_t *map_t::operator()(const int x, const int y)
{
  return hex(x, y);
};

hex_t *map_t::operator()(const int s)
{
  return hex(s);
}

hex_t *map_t::operator()(const point_t p)
{
  return hex(p.x, p.y);
}

hex_t *map_t::operator()(const char *t)
{
  for (int i = 0; i < size; i++)
    {
      if (strcmp(hexes[i].title, t) == 0)
	return hexes + i;
    }
  return NULL;
}


hex_t *map_t::operator[](const int s)
{
  return hex(s);
}

hex_t *map_t::operator[](const point_t p)
{
  return hex(p.x, p.y);
}

map_t::map_t()
{
  width = height = size = 0;
  hexes = 0;
  printf("map_t() called\n");

  misc_t::dbg.log("map_t::map_t() exited.");
}

map_t::map_t(const int x, const int y)
{
  width = x;
  height = y;
  size = x * y;
  hexes = new hex_t[size];
  misc_t::dbg.log("map_t::map_t(%d, %d) exited.", x, y);
}

map_t::map_t(xmlpp::Element* e)
{
  assert(e->get_name() == "map");
  assert(e->get_attribute("width"));
  assert(e->get_attribute("height"));

  width = strtol(e->get_attribute("width")->get_value().data(), NULL, 10);
  height = strtol(e->get_attribute("height")->get_value().data(), NULL, 10);

  size = width * height;
  hexes = new hex_t[size];

  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      xy(i, j)->set(i, j);

  printf("Loading map (%02d x %02d)\n", width, height);

  xmlpp::Node::NodeSet ns = e->find("hex");
  assert(ns.size() == (unsigned int)size);

  for (unsigned int i = 0; i < ns.size(); i++)
    {
      xmlpp::Element *t = dynamic_cast<xmlpp::Element*>(ns[i]);
      assert(t != NULL);

      assert(t->get_attribute("x"));
      assert(t->get_attribute("y"));

      int x = strtol(t->get_attribute("x")->get_value().data(), NULL, 10);
      int y = strtol(t->get_attribute("y")->get_value().data(), NULL, 10);
      
      hex_t *hex = (*this)(x, y);
      hex->reload(t, this);
      assert(hex->map);
    }
}

void map_t::create_nowrap()
{
  misc_t::dbg.log("map_tt::create_nowrap() entered.");

  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      {
	xy(i,j)->setmap(this);
	xy(i,j)->set(i,j);
      }

#ifdef KeepAdjacent
  for (int k = 0; k < height; k++)
    for (int j = 0; j < width; j++)
      for (int i = 0; i < DIRECTIONS; i++)
	{
	  hex_t *ahex = hex(j, k);
	  point_t p = old_move_coords((Direction)i, width, height, ahex->xy);
	  ahex->set_adjacent((Direction)i, hex(p));
	}
#endif

  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      {
	hex_t *ahex = hex(i,j);
	char string[12]; // Maximum 26^3 x and 999 y
	sprintf(string, "%d", ahex->xy.y + 1);
	int k = strlen(string);
	if (ahex->xy.x < 26)
	  {
	    string [k+0] = ('A' + ahex->xy.x);
	    string [k+1] = '\0';
	  }
	else
	  {
	    string [k+0] = ('A' + (ahex->xy.x / 26) - 1);
	    string [k+1] = ('A' + (ahex->xy.x % 26));
	    string [k+2] = '\0';
	  }
	ahex->title = strdup(string);
      }
}

void map_t::resize(const int x, const int y)
{
  misc_t::dbg.log("map_t::resize(%d, %d) entered.", x, y);
  delete [] hexes;
  width = x;
  height = y;
  size = x * y;
  hexes = new hex_t[size];
  for (int j = 0; j < y; j++)
    for (int i = 0; i < x; i++)
      hex(i, j)->set(i, j);
  misc_t::dbg.log("map_t::resize(%d, %d) exited.", x, y);
}

map_t::~map_t()
{
  misc_t::dbg.log("map_t::~map_t() entered.");
  if (hexes)
    delete [] hexes;
  misc_t::dbg.log("map_t::~map_t() exited.");
}

void map_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element* xn_map = p->add_child_element("map");
  xn_map->set_attribute("width", Glib::ustring::compose("%1", width));
  xn_map->set_attribute("height", Glib::ustring::compose("%1", height));

  for (int i = 0; i < size; i++)
    hexes[i].save_xml(xn_map);
}

void map_t::debug(FILE* stream)
{
  fprintf(stream, "Map(height=%d width=%d)\n", height, width);
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      hex(x,y)->debug(stream);
}

/* ----------------------------------------------------------------------
--
-- assignment operator
--
---------------------------------------------------------------------- */
map_t& map_t::operator=(map_t& m)
{
  if (hexes)
    {
      for (int i = 0; i < size; i++)
	delete hexes[i];
      delete [] hexes;
    }
  width = m.width;
  height = m.height;
  size = width * height;

  hexes = new hex_t[size];
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
      xy(x, y)->set(x, y);

  for (int i = 0; i < size; i++)
    {
      hexes[i] = m.hexes[i];
      hexes[i].remap(this);
    }
  return (*this);
}

/* ----------------------------------------------------------------------
--
-- copy constructor
--
---------------------------------------------------------------------- */
map_t::map_t(map_t& m)
{
  width = m.width;
  height = m.height;
  size = width * height;
  
  hexes = new hex_t[size];
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
      xy(x, y)->set(x, y);

  for (int i = 0; i < size; i++)
    {
      hexes[i] = m.hexes[i];
      hexes[i].remap(this);
    }
}
