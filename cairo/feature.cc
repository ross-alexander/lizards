#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <assert.h>
#include <string.h>
#include <sys/syslog.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- feature_t
--
---------------------------------------------------------------------- */

feature_t::~feature_t()
{};

int feature_t::isa(const char* t)
{
  return 0;
} 

serial_t feature_t::serialize()
{
  return serial_t();
}


const char* feature_t::isa(void)
{
  return typeid(*this).name();
}


feature_t* feature_t::factory(serial_t fs)
{
  std::map<std::string, int> fm = {{"BAND", 1}};

  const char *type;
  assert(fs.get("type", type));
  assert(fm.count(type) > 0);
  switch(fm[type])
    {
    case 1:
      band_t *band = new band_t();
      std::map<std::string, int> bm = {
	{RED_LIZARD_CODE, RED},
	{GREEN_LIZARD_CODE, GREEN},
	{GREY_LIZARD_CODE, GREY},
	{BLACK_LIZARD_CODE, BLACK},
	{YELLOW_LIZARD_CODE, YELLOW}};
      std::map<std::string, int>::iterator k = bm.begin();
      for (; k != bm.end(); k++)
	{
	  int lizards;
	  if (fs.get(k->first.c_str(), lizards))
	    {
	      misc_t::log(LOG_DEBUG, "Band %s -> %d", k->first.c_str(), lizards);
	      band->set(k->second, lizards);
	    }
	}
      return band;
      break;
    }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- band_t
--
---------------------------------------------------------------------- */

band_t::band_t() { for (int i = 0; i < UNSETTLED; i++) type[i] = 0; };
int band_t::size()
{
  int size = 0; for (int i = 0; i < UNSETTLED; i++) size += type[i];
  return size;
}

int band_t::set(int t, int s)
{
  if (t < 0 || t >= UNSETTLED)
    return 0;
  type[t] = s;
  return s;
}

int band_t::get(int t)
{
  return (t < 0 || t >= UNSETTLED) ? 0 : type[t];
}


serial_t band_t::serialize()
{
  serial_map_t s;
  s.add("type", "BAND");
  for (int i = 0; i < UNSETTLED; i++)
    if (type[i])
      s.add(misc_t::band_colours[i].c_str(), type[i]);
  return s;
}

int band_t::isa(const char *t)
{
  return BAND == t;
}
