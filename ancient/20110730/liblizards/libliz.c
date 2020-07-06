/* ----------------------------------------------------------------------
--
-- Declare variable in library rather than in each program
--
---------------------------------------------------------------------- */
#include <stdio.h>

#include "lizard.h"

char *game_code, *data_path, *disk_data_path, *email_data_path;

int number_orders, turn_number;

char *strWorldInit = "worldinit";
char *strWorldFfro = "worldffro";
char *strWorldFill = "worldfill";
char *strWorldPop = "worldpop";
char *strWorldFix = "worldfix";
char *strOrderEnt = "orderent";

World *world;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][12] = { "Unsettled", "Red", "Green", "Grey", "Black", "Yellow" };

char* liz_cp_desc [MAX_COMBAT_POINTS + 1] = { "Average",
					      "Good",
					      "Dangerous",
					      "Ferocious",
					      "Fearsome",
					      "Veteran" };

char *liz_hunger_desc [MAX_HUNGER_POINTS + 1] = { "Sated",
						  "Peckish",
						  "Hungry",
						  "Famished",
						  "Starved",
						  "STARVING" };

char *liz_terrain[] = {	"Plain",
			"Plain",
			"Plain",
			"Plain",
			"Swamp",
			"Peak",
			"Peak",
			"Water",
			"Water",
			"Scrub",
			"Plain",
			"Plain",
			"Plain" };
			
