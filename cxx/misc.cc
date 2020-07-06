#include <stdlib.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

static const char *misc_colours[] = { "Red",
			      "Green",
			      "Grey",
			      "Black",
			      "Yellow",
			      "Unsettled" };

static const char* misc_combat_desc[] = { "Average",
				  "Good",
				  "Dangerous",
				  "Ferocious",
				  "Fearsome",
				  "Veteran" };

static const char *misc_hunger_desc[] = { "Sated",
				  "Peckish",
				  "Hungry",
				  "Famished",
				  "Starved",
				  "STARVING" };


int misc_t::uniform(int x)
{
  return random() % x;
}

const char* misc_t::colours(int x)
{
  return misc_colours[x];
}

const char* misc_t::combat_desc(int x)
{
  return misc_combat_desc[x];
}

const char* misc_t::hunger_desc(int x)
{
  return misc_hunger_desc[x];
}

debug_t misc_t::dbg;
