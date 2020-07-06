/* ----------------------------------------------------------------------
--
-- Declare variable in library rather than in each program
--
---------------------------------------------------------------------- */
#include <stdio.h>
#include <time.h>

#include "lizards.h"

char* liz_dir [7] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char* liz_colours[6] = { "Unsettled",
			 "Red",
			 "Green",
			 "Grey",
			 "Black",
			 "Yellow" };

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

char *liz_terrain[] = {	"ERROR",
			"Plain",
			"Swamp",
			"Scrub",
			"Peak",
			"Water" };
			
char *order_types[] = {"inactive",
		       "move",
		       "split",
		       "build-fertile",
		       "build-den",
		       "build-bridge",
		       "destroy-fertile",
		       "destroy-bridge",
		       "recruit",
		       "erupt-volcano",
		       "collapse-bridge",
		       "move-whirlpool",
		       "chant",
		       "lob",
		       "give",
		       "free",
		       "build-raft",
		       "sail",
		       "erupt-mountain",
		      "dormant-volcano",
		       "kill-player",
		       "sneak"
};
