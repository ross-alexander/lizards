#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include "lizards.h"

// ----------------------------------------------------------------------
//
// feature_t functions
//
// ----------------------------------------------------------------------

feature_t::feature_t()
{
  owner = 0;
  desc = 0;
}

feature_t::feature_t(int t, char *d)
{
  owner = 0;
  type = t;
  desc = d;
}

feature_t::~feature_t()
{
  misc_t::dbg.log("Feature(%s) destructed", describe());
}

feature_t* feature_t::clone()
{
  return new feature_t(*this);
}

void feature_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element* xn_f = p->add_child_element("feature");
  xn_f->set_attribute("type", Glib::ustring::compose("%1", type));
}			       

void feature_t::debug(FILE *stream)
{
  fprintf(stream, "Feature(type=%d owner=%d)\n", type, owner);
}

char *feature_t::describe()
{
  if (desc == 0)
    desc = g_strdup_printf("feature %d", type);
  return (char*)desc;
}

// ----------------------------------------------------------------------
//
// band_t functions
//
// ----------------------------------------------------------------------

band_t::band_t()
{
  desc = NULL;
  type = BAND;
  for (int i = 0; i < LIZARD_TYPES; i++)
    colour[i] = 0;
  hunger = combat = immobile = 0;
}

band_t::band_t(int b[])
{
  desc = NULL;
  type = BAND;
  for (int i = 0; i < LIZARD_TYPES; i++)
    colour[i] = b[i];
  hunger = combat = immobile = 0;
}

band_t::band_t(band_t&b)
{
  type = BAND;
  desc = NULL;
  for (int i = 0; i < LIZARD_TYPES; i++)
    colour[i] = b.colour[i];
  hunger = b.hunger;
  combat = b.combat;
  immobile = b.immobile;
}

band_t* band_t::clone()
{
  return new band_t(*this);
}

band_t::band_t(xmlpp::Element *e)
{
  type = BAND;
  desc = NULL;

  xmlpp::Attribute* a_hunger = e->get_attribute("hunger");
  xmlpp::Attribute* a_combat = e->get_attribute("combat");
  xmlpp::Attribute* a_immobile = e->get_attribute("immobile");

  assert(a_hunger != NULL);
  assert(a_combat != NULL);
  assert(a_immobile != NULL);

  hunger = strtol(a_hunger->get_value().data(), NULL, 10);
  combat = strtol(a_combat->get_value().data(), NULL, 10);
  immobile = strtol(a_immobile->get_value().data(), NULL, 10);
  
  xmlpp::Attribute* a;

  colour[RED] = (a = e->get_attribute("red")) ? strtol(a->get_value().data(), NULL, 10) : 0;
  colour[GREEN] = (a = e->get_attribute("green")) ? strtol(a->get_value().data(), NULL, 10) : 0;
  colour[GREY] = (a = e->get_attribute("grey")) ? strtol(a->get_value().data(), NULL, 10) : 0;
  colour[BLACK] = (a = e->get_attribute("black")) ? strtol(a->get_value().data(), NULL, 10) : 0;
  colour[YELLOW] = (a = e->get_attribute("yellow")) ? strtol(a->get_value().data(), NULL, 10) : 0;
}

int band_t::size()
{
  int j = 0;
  for (int i = 0; i < LIZARD_TYPES; i++)
    j += colour[i];
  return j;
}

void band_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element *xn_band = p->add_child_element("band");
  static const char *xmlcolours[] = {"red", "green", "grey", "black", "yellow" };
  for (int i = 0; i < LIZARD_TYPES; i++)
    if (colour[i])
      {
	xn_band->set_attribute(xmlcolours[i], Glib::ustring::compose("%1", colour[i]));
      }

  xn_band->set_attribute("hunger", Glib::ustring::compose("%1", hunger));
  xn_band->set_attribute("combat", Glib::ustring::compose("%1", combat));
  xn_band->set_attribute("immobile", Glib::ustring::compose("%1", immobile));
}			       

char* band_t::describe()
{
  char buf[128];
  int ptr = 0;
  int count = 0;
  for (int i = 0; i < LIZARD_TYPES; i++)
    if (colour[i] > 0) count++;
  if (count == 0)
    sprintf(buf, "No");
  for (int i = 0; i < LIZARD_TYPES; i++)
    if (colour[i] > 0)
      {
	if (count > 2)
	  sprintf(buf + ptr, "%d %s, ", colour[i], misc_t::colours(i));
	else if (count == 2)
	  sprintf(buf + ptr, "%d %s and ", colour[i], misc_t::colours(i));
	else
	  sprintf(buf + ptr, "%d %s", colour[i], misc_t::colours(i));
	ptr = strlen(buf);
	count--;
      }
  if (desc)
    delete desc;
  desc = strdup(buf);
  return desc;
}

band_t::operator char*()
{
  return describe();
}

int band_t::inc_combat()
{
  if (combat + 1 > MAX_COMBAT_POINTS)
    return 0;
  combat++;
  return 1;
}

void band_t::min(band_t& b)
{
  for (int i = 0; i < LIZARD_TYPES; i++)
    if (b.colour[i] < colour[i])
      colour[i] = b.colour[i];
}

band_t band_t::operator-(band_t&b)
{
  band_t result;
  for (int i = 0; i < LIZARD_TYPES; i++)
    {
      result.colour[i] = colour[i] - b.colour[i];
      assert(result.colour[i] >= 0);
    }
  result.hunger = (hunger + b.hunger)/2;
  result.combat = (combat + b.combat)/2;
  return result;
}

band_t& band_t::operator+=(band_t&b)
{
  for (int i = 0; i < LIZARD_TYPES; i++)
    {
      colour[i] = colour[i] + b.colour[i];
      assert(colour[i] >= 0);
    }
  assert(size() <= MAX_LIZARDS_IN_HEX);
  hunger = (hunger + b.hunger)/2;
  combat = (combat + b.combat)/2;
  return *this;
}

band_t& band_t::operator-=(band_t&b)
{
  for (int i = 0; i < LIZARD_TYPES; i++)
    {
      colour[i] = colour[i] - b.colour[i];
      assert(colour[i] >= 0);
    }
  hunger = (hunger + b.hunger)/2;
  combat = (combat + b.combat)/2;
  return *this;
}

int band_t::kill(int deaths)
{
  int frags = 0;
  while ((deaths > 0) && size())
    {
      int x = misc_t::uniform(LIZARD_TYPES);
      if (colour[x] > 0)
	{
	  colour[x]--;
	  deaths--;
	  frags++;
	}
    }
  return frags;
}


int band_t::capture(band_t& b, int num_captured)
{
  int work, num, col, work4, count;
  int cap[LIZARD_TYPES];

  for (int i = 0; i < LIZARD_TYPES; i++)
    cap[i] = 0;

  count = 0;
  work = num_captured < b.size() ? num_captured : b.size();

  while ((work > 0) && (size() < MAX_LIZARDS_IN_HEX))
  {
    if (work > 10)
      num = misc_t::uniform(work / 10) + 1;
    else
      num = 1;
    
    do
    {
      col = misc_t::uniform(LIZARD_TYPES);
      work4 = colour[col];
    } while (work4 == 0);

    if (num > work4)
      num = work4;

    if ((size() + num) > MAX_LIZARDS_IN_HEX)
      num = MAX_LIZARDS_IN_HEX - size();

    b.colour[col] -= num;
    colour[col] += num;
    cap[col] += num;
    work -= num;
    count += num;
  }
  for (int i = 0; i < LIZARD_TYPES; i++)
    colour[i] = cap[i];
  return count;
}
  
/* ----------------------------------------------------------------------
--
-- half_den_t functions
--
---------------------------------------------------------------------- */
half_den_t::half_den_t()
{
  undisturbed = 1;
};

/* ----------------------------------------------------------------------
--
-- den_t functions
--
---------------------------------------------------------------------- */

den_t:: den_t(xmlpp::Element *e)
{
  desc = NULL;
  type = DEN;
  hunger = strtol(e->get_attribute("hunger")->get_value().data(), NULL, 10);
  pop = strtol(e->get_attribute("population")->get_value().data(), NULL, 10);
  colour = strtol(e->get_attribute("colour")->get_value().data(), NULL, 10);
  if (e->get_attribute("timebomb"))
    timebomb = strtol(e->get_attribute("timebomb")->get_value().data(), NULL, 10);
  else
    timebomb = 0;
  home = e->get_attribute("home") ? 1 : 0;
}

den_t::den_t()
{
  type = DEN;
  pop = 0;
  colour = 0;
  home = 0;
  desc = NULL;
  timebomb = 0;
}

den_t::den_t(den_t& d)
{
  type = DEN;
  pop = d.pop;
  colour = d.colour;
  home = d.home;
  timebomb = d.timebomb;
  desc = NULL;
}

den_t::den_t(int c)
{
  type = DEN;
  pop = 0;
  home = 0;
  timebomb = 0;
  desc = NULL;
  colour = c;
}

den_t* den_t::clone()
{
  return new den_t(*this);
}

void den_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element* xn_f = p->add_child_element("den");
  xn_f->set_attribute("hunger", Glib::ustring::compose("%1", hunger));
  xn_f->set_attribute("population", Glib::ustring::compose("%1", pop));
  xn_f->set_attribute("colour", Glib::ustring::compose("%1", colour));
  if (home)
    xn_f->set_attribute("home", Glib::ustring::compose("%1", home));
  if (timebomb)
    xn_f->set_attribute("timebomb", Glib::ustring::compose("%1", timebomb));
}			       

char *den_t::describe()
{
  if (desc)
    delete desc;
  desc = g_strdup_printf("%s %s", misc_t::colours(colour), home ? "Home Den" : "Den");
  return desc;
}

den_t::operator char*()
{
  return describe();
}

// ----------------------------------------------------------------------
//
// fertile_t functions
//
// ----------------------------------------------------------------------

fertile_t::fertile_t() : feature_t(FERTILE, 0)
{
  level = 0;
}

fertile_t::fertile_t(xmlpp::Element* e) : feature_t(FERTILE, 0)
{
  level = strtol(e->get_attribute("level")->get_value().data(), NULL, 10);
}

fertile_t::fertile_t(fertile_t& f) : feature_t(FERTILE, 0)
{
  level = f.level;
}

void fertile_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element *xn_f = p->add_child_element("fertile");
  xn_f->set_attribute("level", Glib::ustring::compose("%1", level));
}

fertile_t* fertile_t::clone()
{
  return new fertile_t(*this);
}
char *fertile_t::describe()
{
  if (desc) delete desc;
  desc = g_strdup_printf("Fertile %d", level);
  return desc;
}

// ----------------------------------------------------------------------
//
// swamp_t functions
//
// ----------------------------------------------------------------------

swamp_t::swamp_t() : feature_t(SWAMP, (char*)"Swamp")
{
}

swamp_t::swamp_t(swamp_t& f) : feature_t(SWAMP, (char*)"Swamp")
{
}

swamp_t::swamp_t(xmlpp::Element* e) : feature_t(SWAMP, (char*)"Swamp")
{
}

swamp_t* swamp_t::clone()
{
  return new swamp_t(*this);
}

void swamp_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("swamp");
}


// ----------------------------------------------------------------------
//
// peak_t functions
//
// ----------------------------------------------------------------------

peak_t::peak_t() : feature_t(PEAK, (char*)"Peak")
{
}

peak_t::peak_t(peak_t& f) : feature_t(PEAK, (char*)"Peak")
{
}

peak_t::peak_t(xmlpp::Element* e) : feature_t(PEAK, (char*)"Peak")
{
}

peak_t* peak_t::clone()
{
  return new peak_t(*this);
}

void peak_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("peak");
}			       

// ----------------------------------------------------------------------
//
// scrub_t functions
//
// ----------------------------------------------------------------------

scrub_t::scrub_t()
{
  type = SCRUB;
}

scrub_t::scrub_t(scrub_t& f)
{
  type = SCRUB;
}

scrub_t::scrub_t(xmlpp::Element* e)
{
  type = SCRUB;
}

scrub_t* scrub_t::clone()
{
  return new scrub_t(*this);
}

void scrub_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("swamp");
}			       


// ----------------------------------------------------------------------
//
// temple_t functions
//
// ----------------------------------------------------------------------

temple_t::temple_t() : feature_t(TEMPLE, (char*)"Temple")
{
}

temple_t::temple_t(temple_t&) : feature_t(TEMPLE, (char*)"Temple")
{
}

temple_t::temple_t(xmlpp::Element* e) : feature_t(TEMPLE, (char*)"Temple")
{
}

temple_t* temple_t::clone()
{
  return new temple_t(*this);
}

void temple_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("temple");
}			       

// ----------------------------------------------------------------------
//
// ruin_t functions
//
// ----------------------------------------------------------------------
ruin_t::ruin_t() : feature_t(RUIN, (char*)"Ruin")
{
}

ruin_t::ruin_t(ruin_t&) : feature_t(RUIN, (char*)"Ruin")
{
}

ruin_t::ruin_t(xmlpp::Element* e) : feature_t(RUIN, (char*)"Ruin")
{
}

ruin_t* ruin_t::clone()
{
  return new ruin_t(*this);
}

void ruin_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("ruin");
}			       

// ----------------------------------------------------------------------
//
// cursed_t functions
//
// ----------------------------------------------------------------------
cursed_t::cursed_t()
{
  type = CURSED;
}

cursed_t::cursed_t(cursed_t& f)
{
  type = CURSED;
}

cursed_t* cursed_t::clone()
{
  return new cursed_t(*this);
}

cursed_t::cursed_t(xmlpp::Element* e)
{
  type = CURSED;
}

void cursed_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("cursed");
}			       

// ----------------------------------------------------------------------
//
// volcano_t functions
//
// ----------------------------------------------------------------------
volcano_t::volcano_t()
{
  type = VOLCANO;
}

volcano_t::volcano_t(volcano_t& f)
{
  type = VOLCANO;
}

volcano_t::volcano_t(xmlpp::Element* e)
{
  type = VOLCANO;
  desc = (char*)"Volcano";
}

volcano_t* volcano_t::clone()
{
  return new volcano_t(*this);
}

void volcano_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("volcano");
}

char *volcano_t::describe()
{
  return desc;
}

// ----------------------------------------------------------------------
//
// whirlpool_t functions
//
// ----------------------------------------------------------------------
whirlpool_t::whirlpool_t() : feature_t(WHIRLPOOL, (char*)"Whirlpool")
{
}

whirlpool_t::whirlpool_t(whirlpool_t& f) : feature_t(WHIRLPOOL, (char*)"Whirlpool")
{
}

whirlpool_t::whirlpool_t(xmlpp::Element* e) : feature_t(WHIRLPOOL, (char*)"Whirlpool")
{
}

whirlpool_t* whirlpool_t::clone()
{
  return new whirlpool_t(*this);
}

void whirlpool_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("whirlpool");
}			       

/* ----------------------------------------------------------------------
--
-- raft_t functions
--
---------------------------------------------------------------------- */
raft_t::raft_t() : feature_t()
{
  type = RAFT;
  life = 0;
}

raft_t::raft_t(raft_t& f) : feature_t(f)
{
  type = RAFT;
  life = 0;
}

raft_t::raft_t(xmlpp::Element* e)
{
  type = RAFT;
  life = strtol(e->get_attribute("life")->get_value().data(), NULL, 10);
}

raft_t* raft_t::clone()
{
  return new raft_t(*this);
}

void raft_t::save_xml(xmlpp::Element *p)
{
  xmlpp::Element *xn_f = p->add_child_element("fertile");
  xn_f->set_attribute("life", Glib::ustring::compose("%1", life));
}

/* ----------------------------------------------------------------------
--
-- bridge_t functions
--
---------------------------------------------------------------------- */
bridge_t::bridge_t() : feature_t()
{
  type = BRIDGE;
}

bridge_t::bridge_t(xmlpp::Element* e)
{
  type = BRIDGE;
}

void bridge_t::save_xml(xmlpp::Element *p)
{
  p->add_child_element("bridge");
}

bridge_t* bridge_t::clone()
{
  return new bridge_t(*this);
}

