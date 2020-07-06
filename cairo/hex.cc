#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <assert.h>
#include <string.h>
#include <sys/syslog.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- hex_t
--
---------------------------------------------------------------------- */

hex_t::hex_t()
{
  owner = 0;
  active = 0;
  //  ref = 0;
}

hex_t::~hex_t() {};

void hex_t::setid(std::string _id)
{
  id = _id;
}

std::string hex_t::getid()
{
  return(id);
}

int hex_t::isa(const char* t)
{
  return(!strcmp(BLANK, t));
}

const char* hex_t::isa(void)
{
  return typeid(*this).name();
}

feature_t* hex_t::get(const char *t)
{
  return features.count(t) ? features[t] : 0;
}

feature_t *hex_t::get(feature_t* f)
{
  return get(typeid(f).name());
}

feature_t* hex_t::set(feature_t* f)
{
  const char *tid = typeid(*f).name();
  delete features[tid];
  features[tid] = f;
  return f;
};

char *hex_t::output_band_size(int player)
{
  if (band_t *band = dynamic_cast<band_t*>(get(BAND)))
    {
      char buf[6];
      snprintf(buf, 6, "%d", band->size());
      return strdup(buf);
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- water_t
--
---------------------------------------------------------------------- */

int water_t::isa(const char *t)
{
  return (!strcmp(WATER, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- fertile_t
--
---------------------------------------------------------------------- */

fertile_t::fertile_t() { age = 0; };

int fertile_t::isa(const char *t)
{
  return (FERTILE == t) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- plains_t
--
---------------------------------------------------------------------- */

int plains_t::isa(const char *t)
{
  return (!strcmp(PLAINS, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- temple_t
--
---------------------------------------------------------------------- */

int temple_t::isa(const char *t)
{
  return (TEMPLE == t) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- ruin_t
--
---------------------------------------------------------------------- */

int ruin_t::isa(const char *t)
{
  return (!strcmp(RUIN, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- cursed_t
--
---------------------------------------------------------------------- */

int cursed_t::isa(const char *t)
{
  return (!strcmp(CURSED, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- swamp_t
--
---------------------------------------------------------------------- */

int swamp_t::isa(const char *t)
{
  return (!strcmp(SWAMP, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- peak_t
--
---------------------------------------------------------------------- */

int peak_t::isa(const char *t)
{
  return (!strcmp(PEAK, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- volcano_t
--
---------------------------------------------------------------------- */

int volcano_t::isa(const char *t)
{
  return (!strcmp(VOLCANO, t)) || peak_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- whirlpool_t
--
---------------------------------------------------------------------- */

int whirlpool_t::isa(const char *t)
{
  return (!strcmp(WHIRLPOOL, t)) || water_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- scrub_t
--
---------------------------------------------------------------------- */

int scrub_t::isa(const char *t)
{
  return (!strcmp(SCRUB, t)) || hex_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- den_t
--
---------------------------------------------------------------------- */

den_t::den_t() { hex_t(); type = UNSETTLED; pop = 0; home = 0;};
den_t::den_t(int c) { hex_t(); type = c; pop = 0; home = 0;};

red_den_t::red_den_t() : den_t(RED) {};
green_den_t::green_den_t() : den_t(GREEN) {};
grey_den_t::grey_den_t() : den_t(GREY) {};
yellow_den_t::yellow_den_t() : den_t(YELLOW) {};
black_den_t::black_den_t() : den_t(BLACK) {};
home_den_t::home_den_t() : den_t(UNSETTLED) { home = 1; };

den_t* den_t::factory(int t)
{
  switch(t)
    {
    case RED:
      return new red_den_t();
      break;
    case GREEN:
      return new green_den_t();
      break;
    case GREY:
      return new grey_den_t();
      break;
    case YELLOW:
      return new yellow_den_t();
      break;
    case BLACK:
      return new black_den_t();
      break;
    default:
      return new den_t();
      break;
    }
}

int den_t::isa(const char *t)
{
  return (!strcmp(DEN, t)) || hex_t::isa(t);
}

int black_den_t::isa(const char *t)
{
  return (!strcmp(BLACK_DEN, t)) || den_t::isa(t);
}

int red_den_t::isa(const char *t)
{
  return (!strcmp(RED_DEN, t)) || den_t::isa(t);
}

int green_den_t::isa(const char *t)
{
  return (!strcmp(GREEN_DEN, t)) || den_t::isa(t);
}

int grey_den_t::isa(const char *t)
{
  return (!strcmp(GREY_DEN, t)) || den_t::isa(t);
}

int yellow_den_t::isa(const char *t)
{
  return (!strcmp(YELLOW_DEN, t)) || den_t::isa(t);
}

int home_den_t::isa(const char *t)
{
  return (!strcmp(HOME_DEN, t)) || den_t::isa(t);
}

/* ----------------------------------------------------------------------
--
-- serialize
--
---------------------------------------------------------------------- */

std::map<std::string,std::string> hex_t::type_code = {
  {WATER, WATER_CODE},
  {FERTILE, FERTILE_CODE},
  {PLAINS, PLAINS_CODE},
  {SCRUB, SCRUB_CODE},
  {WATER, WATER_CODE},
  {WHIRLPOOL, WHIRLPOOL_CODE},
  {RED_DEN, RED_DEN_CODE},
  {GREY_DEN, GREY_DEN_CODE},
  {GREEN_DEN, GREEN_DEN_CODE},
  {YELLOW_DEN, YELLOW_DEN_CODE},
  {BLACK_DEN, BLACK_DEN_CODE},
  {SWAMP, SWAMP_CODE},
  {VOLCANO, VOLCANO_CODE},
  {TEMPLE, TEMPLE_CODE},
  {RUIN, RUIN_CODE},
  {PEAK, PEAK_CODE},
  {EMPTY_DEN, EMPTY_DEN_CODE},
};

std::map<std::string, int> hex_t::code_ref = {
  {WATER_CODE, 1}, 
  {PLAINS_CODE, 2},
  {FERTILE_CODE, 3},
  {SWAMP_CODE, 4},
  {SCRUB_CODE, 5},
  {CURSED_CODE, 6},
  {PEAK_CODE, 7},
  {VOLCANO_CODE, 8},
  {WHIRLPOOL_CODE, 9},
  {RUIN_CODE, 10},
  {TEMPLE_CODE, 11},
  {RED_DEN_CODE, 12},
  {GREEN_DEN_CODE, 13},
  {GREY_DEN_CODE, 14},
  {YELLOW_DEN_CODE, 15},
  {BLACK_DEN_CODE, 16},
  {EMPTY_DEN_CODE, 17},
};

serial_t hex_t::serialize(void)
{
  serial_map_t map;
  assert(getid().length() != 0);
  map.add("id", getid().c_str());
  map.add("x", xy.x);
  map.add("y", xy.y);
  map.add("owner", owner);
  const char *tid = typeid(*this).name();

  assert(type_code.count(tid) > 0);
  
  map.add("type", type_code[tid].data());

  if (!features.empty())
    {
      serial_array_t array;
      map.add("features", array);
      std::map<std::string, feature_t*>::iterator i;
      for (i = features.begin(); i != features.end(); i++)
	array.add((*i).second->serialize());
    }

  return map;
}

serial_t den_t::serialize(void)
{
  serial_map_t s(hex_t::serialize());
  s.add("pop", pop);
  return s;
}


hex_t* hex_t::factory(serial_t s)
{
  serial_map_t m(s);
  const char *t;
  assert(m.get("type", t));
  hex_t *hex = factory(t);
  assert(hex);
  m.get("x", hex->xy.x);
  m.get("y", hex->xy.y);
  m.get("owner", hex->owner);
  //  m.get("ref", hex->ref);
  const char *id;
  m.get("id", id);
  hex->setid(id);
  misc_t::log(LOG_DEBUG, "hex %s %s %2dx%2d", t, id, hex->xy.x, hex->xy.y);

  serial_array_t fa;
  if (m.get("features", fa))
    {
      for (unsigned int j = 0; j < fa.size(); j++)
	{
	  serial_t fs = fa[j];
	  feature_t *f = feature_t::factory(fs);
	  if (f) hex->set(f);
	}
    }
  return hex;
}

hex_t* hex_t::factory(const char* t)
{
  int id = 0;
  hex_t *hex = 0;
  if (code_ref.count(t) > 0)
    id = code_ref[t];
  assert(code_ref.count(t) > 0);

  switch(id)
    {
    case 0:
      hex = new hex_t();
      break;
    case 1:
      hex = new water_t();
      break;
    case 2:
      hex = new plains_t();
      break;
    case 3:
      hex = new fertile_t();
      break;
    case 4:
      hex = new swamp_t();
      break;
    case 5:
      hex = new scrub_t();
      break;
    case 6:
      hex = new cursed_t();
      break;
    case 7:
      hex = new peak_t();
      break;
    case 8:
      hex = new volcano_t();
      break;
    case 9:
      hex = new whirlpool_t();
      break;
    case 10:
      hex = new ruin_t();
      break;
    case 11:
      hex = new temple_t();
      break;
    case 12:
      hex = new red_den_t();
      break;
    case 13:
      hex = new green_den_t();
      break;
    case 14:
      hex = new grey_den_t();
      break;
    case 15:
      hex = new yellow_den_t();
      break;
    case 16:
      hex = new black_den_t();
      break;
    case 17:
      hex = new den_t();
      break;
    }
  return hex;
}

std::string hex_t::tostring()
{
  return fmt::sprintf("hex_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string scrub_t::tostring()
{
  return fmt::sprintf("scrub_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string swamp_t::tostring()
{
  return fmt::sprintf("swamp_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string cursed_t::tostring()
{
  return fmt::sprintf("cursed_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string ruin_t::tostring()
{
  return fmt::sprintf("ruin_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string whirlpool_t::tostring()
{
  return fmt::sprintf("whirlpool_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string temple_t::tostring()
{
  return fmt::sprintf("temple_t(%s %dx%d)", id, xy.x, xy.y);
}
std::string water_t::tostring()
{
  return fmt::sprintf("water_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string plains_t::tostring()
{
  return fmt::sprintf("plains_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string peak_t::tostring()
{
  return fmt::sprintf("peak_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string volcano_t::tostring()
{
  return fmt::sprintf("volcano_t(%s %dx%d)", id, xy.x, xy.y);
}

std::string fertile_t::tostring()
{
  return fmt::sprintf("fertile_t(%s %dx%d age=%d)", id, xy.x, xy.y, age);
}

std::string den_t::tostring()
{
  return fmt::sprintf("den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}

std::string red_den_t::tostring()
{
  return fmt::sprintf("red_den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}
std::string black_den_t::tostring()
{
  return fmt::sprintf("black_den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}
std::string yellow_den_t::tostring()
{
  return fmt::sprintf("yellow_den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}
std::string green_den_t::tostring()
{
  return fmt::sprintf("green_den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}
std::string grey_den_t::tostring()
{
  return fmt::sprintf("grey_den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}
std::string home_den_t::tostring()
{
  return fmt::sprintf("home_den_t(%s %dx%d type=%d pop=%d home=%d)", id, xy.x, xy.y, type, pop, home);
}

