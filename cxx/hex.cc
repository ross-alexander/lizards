#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

hex_t::hex_t(void)
{
  owner = 0;
  terrain = WATER;
  //  adjlist = new adjacent_t[DIRECTIONS];
#ifndef STL
  flistlen = 0;
  flist = 0;
#endif
  title = 0;
};

hex_t::~hex_t(void)
{
  //  debug.log("~hex_t(%p %s)", this, title);
  clear_features();
}

void hex_t::set(int x_p, int y_p)
{
  xy.x = x_p;
  xy.y = y_p;
};

hex_t* hex_t::adjacent(const char *dir)
{
  int d = (int)dir_t(dir);
  
  if (d < 0)
    return NULL;
  else
    return adjacent(d);
}  

hex_t& hex_t::operator=(hex_t& h)
{
  owner = h.owner;
  terrain = h.terrain;
  //  undisturbed = h.undisturbed;
  xy = h.xy;
  title = strdup(h.title);
  //  for (int i = 0; i < DIRECTIONS; i++)
  //    {
  //      adjlist[i].xy = h.adjlist[i].xy;
  //      adjlist[i].hex = NULL;
  //    }
#ifdef STL
  for (unsigned i = 0; i < h.fv.size(); i++)
    fv.push_back(h.fv[i]->clone());
#else
  flistlen = h.flistlen;
  flist = new feature_t*[flistlen];
  for (int i = 0; i < flistlen; i++)
    if (h.flist[i])
      flist[i] = h.flist[i]->clone();
    else
      flist[i] = 0;
#endif
  return *this;
}

hex_t* hex_t::adjacent(dir_t d)
{
  if (!d.okay())
    return NULL;
  assert(map);
  return (*map)(map->move(xy, d));
  //  assert(adjlist[(int)d].hex != NULL);

  //  return adjlist[(int)d].hex;
}

int hex_t::terrain_adjacent(Terrain t)
{
  int j = 0;
  for (int i = 0; i < DIRECTIONS; i++)
    j += adjacent(i)->terrain == t ? 1 : 0;
  return j;
}

#ifdef KeepAdjacent
void hex_t::set_adjacent(dir_t dir, hex_t *hex)
{
  adjlist[dir].hex = hex;
  adjlist[dir].xy = hex->xy;
}
#endif

int hex_t::num_features(void)
{
#ifdef STL
  return fv.size();
#else
  int i = 0;
  for (int j = 0; j < flistlen; j++)
    i += flist[j] ? 1 : 0;
  return i;
#endif
}

feature_t* hex_t::feature(int i)
{
  if (i < 0 || i >= num_features())
    return 0;
  return fv[i];
}

feature_t* hex_t::has_feature(feature_t* f)
{
#ifdef STL
  for (unsigned int i = 0; i < fv.size(); i++)
    if (fv[i] == f)
      return fv[i];
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    if (flist[i] == f)
      return flist[i];
#endif
  return 0;
}

feature_t* hex_t::has_feature(Feature f)
{
#ifdef STL
  for (unsigned int i = 0; i < fv.size(); i++)
    if (fv[i]->type == f)
      {
	misc_t::dbg.log("Hex(%s) found %s", title, fv[i]->describe());
	return fv[i];
      }	
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    if (flist[i]->type == f)
      return flist[i];
#endif
  return 0;
}

int hex_t::count_feature(Feature f)
{
  int count = 0;
#ifdef STL
  for (unsigned int i = 0; i < fv.size(); i++)
    if (fv[i]->type == f)
      count++;
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    if (flist[i]->type == f)
      count++;
#endif
  return count;
}

int hex_t::count_feature(Feature f, int owner)
{
  int count = 0;
#ifdef STL
  for (unsigned int i = 0; i < fv.size(); i++)
    if (fv[i]->type == f && fv[i]->owner == owner)
      count++;
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    if (flist[i]->type == f && flist[i]->owner == owner)
      count++;
#endif
  return count;
}

int hex_t::feature_adjacent(Feature f)
{
  int j = 0;
  for (int i = NORTH; i <= NORTHWEST; i++)
    if (adjacent(i)->has_feature(f))
      j++;
  return j;
}

feature_t* hex_t::add_feature(feature_t *f)
{
#ifdef STL
  if (has_feature(f))
    return f;
  fv.push_back(f);
  misc_t::dbg.log("Hex(%s) added %s", title, f->describe());
#else
  int i;
  for (i = 0; i < flistlen && flist[i]; i++)
    {
      if (flist[i] == f)
	return f;
    }

  misc_t::dbg.log("add_feature(%d %s) i = %d, flistlen = %d", f->type, title, i, flistlen);

  if (i == flistlen)
    {
      feature_t **tmp = new feature_t*[flistlen + 10];
      for (int j = 0; j < flistlen; j++)
	tmp[j] = flist[j];
      for (int j = flistlen; j < flistlen + 10; j++)
	tmp[j] = 0;
      delete flist;
      flist = tmp;
      flistlen += 10;
    }
  flist[i] = f;
#endif
  return f;
}

feature_t* hex_t::del_feature(feature_t *f)
{
  if (f == 0) return f;
#ifdef STL
  std::vector<feature_t*>::iterator i;
  for (i = fv.begin(); i != fv.end(); ++i)
    {
      if (*i == f)
	{
	  fv.erase(i);
	  break;
	}
    }

#else
  int i, j;
  for (i = 0; i < flistlen && flist[i]; i++)
    if (flist[i] == f)
      break;
  if (flist[i] != f)
    return 0;

  /* Shuffle all other features down one */

  for (j = i; j < flistlen && flist[j]; j++)
    flist[j] = flist[j+1];
  flist[j+1] = NULL;
#endif
  return f;
}

feature_t* hex_t::pop_feature()
{
#ifdef STL
  feature_t *f = 0;
  if (fv.size())
    {
      f = fv.back();
      fv.pop_back();
    }
  return f;
#else
  feature_t *f;
  f = (flistlen && flist[0]) ? del_feature(flist[0]) : NULL;
  return f;
#endif
}

void hex_t::clear_features()
{
#ifdef STL
  misc_t::dbg.log("%s cleared", title);
  for (unsigned int i = 0; i < fv.size(); i++)
    delete fv[i];
  fv.clear();
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    delete flist[i];
  delete [] flist;
  flist = 0;
  flistlen = 0;
#endif
}

int hex_t::size()
{
  int size;
  band_t *band;
  den_t *den;
  size = ((den = (den_t*)has_feature(DEN))) ? den->pop : 0;
  return size + ((band = (band_t*)has_feature(BAND)) ? band->size() : 0);
}

void hex_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element *xn_hex = p->add_child_element("hex");
  xn_hex->set_attribute("x", Glib::ustring::compose("%1", xy.x));
  xn_hex->set_attribute("y", Glib::ustring::compose("%1", xy.y));
  xn_hex->set_attribute("title", Glib::ustring::compose("%1", title));
  xn_hex->set_attribute("terrain", Glib::ustring::compose("%1", terrain));
  xn_hex->set_attribute("owner", Glib::ustring::compose("%1", owner));

#ifdef KeepAdjacent
  for (int i = 0; i < DIRECTIONS; i++)
    {
      if (adjacent(i))
	{
	  xmlpp::Element *xn_adj = xn_hex->add_child_element("adj");
	  xn_adj->set_attribute("d", Glib::ustring::compose("%1", i));
	  xn_adj->set_attribute("x", Glib::ustring::compose("%1", adjlist[i].xy.x));
	  xn_adj->set_attribute("y", Glib::ustring::compose("%1", adjlist[i].xy.y));
	}
    }
#endif

  for (unsigned int i = 0; i < fv.size(); i++)
#ifdef STL
    fv[i]->save_xml(xn_hex);
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    flist[i]->save_xml(xn_hex);
#endif
}

void hex_t::debug(FILE *stream)
{
  fprintf(stream, "Hex(x=%d y=%d title=%s terrain=%d owner=%d)\n",
	  xy.x, xy.y, title, terrain, owner);
#ifdef STL
  for (unsigned int i = 0; i < fv.size(); i++)
    fv[i]->debug(stream);
#else
  for (int i = 0; i < flistlen && flist[i]; i++)
    flist[i]->debug(stream);
#endif
}

void hex_t::reload(xmlpp::Element *e, map_t *_map)
{
  assert(e->get_attribute("title"));
  assert(e->get_attribute("owner"));
  assert(e->get_attribute("terrain"));

  owner = strtol(e->get_attribute("owner")->get_value().data(), NULL, 10);
  terrain = (Terrain)strtol(e->get_attribute("terrain")->get_value().data(), NULL, 10);
  title = strdup(e->get_attribute("title")->get_value().data());
  map = _map;
  xmlpp::Node::NodeSet ns = e->find("./*");

  for (unsigned int j = 0; j < ns.size(); j++)
    {
      xmlpp::Element *t = dynamic_cast<xmlpp::Element*>(ns[j]);
#ifdef KeepAdjacent
      if (t->get_name() == "adj")
	{
	  assert(t->get_attribute("d") != NULL);
	  assert(t->get_attribute("x") != NULL);
	  assert(t->get_attribute("y") != NULL);

	  int dir = strtol(t->get_attribute("d")->get_value().data(), NULL, 10);
	  int x = strtol(t->get_attribute("x")->get_value().data(), NULL, 10);
	  int y = strtol(t->get_attribute("y")->get_value().data(), NULL, 10);
	  adjlist[dir].hex = (*map)(x, y);
	  adjlist[dir].xy = point_t(x, y);
	}
#endif
      Glib::ustring name = t->get_name();
      if (name == "den") add_feature(new den_t(t));
      if (name == "band") add_feature(new band_t(t));
      if (name == "fertile") add_feature(new fertile_t(t));
      if (name == "ruin") add_feature(new ruin_t(t));
      if (name == "temple") add_feature(new temple_t(t));
      if (name == "cursed") add_feature(new cursed_t(t));
      if (name == "whirlpool") add_feature(new whirlpool_t(t));
      if (name == "volcano") add_feature(new volcano_t(t));
      if (name == "peak") add_feature(new peak_t(t));
      if (name == "swamp") add_feature(new swamp_t(t));
      if (name == "scrub") add_feature(new scrub_t(t));
    }
}

hex_t::operator char*()
{
  return title;
}

void hex_t::remap(map_t* _map)
{
  //  for (int i = 0; i < DIRECTIONS; i++)
  //    adjlist[i].hex = map(adjlist[i].xy);
  map = _map;
}

void hex_t::setmap(map_t *m)
{
  map = m;
}
