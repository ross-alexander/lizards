/* ----------------------------------------------------------------------
--
-- Declare variable in library rather than in each program
--
---------------------------------------------------------------------- */
#include <stdio.h>

#include "lizard.h"
#include "map.h"

char *game_code, *data_path, *disk_data_path, *email_data_path;

int number_orders, turn_number, number_spys;

char *strWorldInit = "worldinit";
char *strWorldFfro = "worldffro";
char *strWorldFill = "worldfill";
char *strWorldPop = "worldpop";
char *strWorldFix = "worldfix";
char *strOrderEnt = "orderent";

Player player[MAX_PLAYERS + 1];

/*
char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char player_clan_name [MAX_PLAYERS + 1][37];
*/

int players_in_game;
World world;

spy_t *spy_list [MAX_SPYS];
int number_spys;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][12] = { "Unsettled", "Red", "Green", "Grey", "Black", "Yellow" };

char liz_cp_desc [MAX_COMBAT_POINTS + 1][10] = { "Average",
						 "Good",
						 "Dangerous",
						 "Ferocious",
						 "Fearsome",
						 "Veteran" };

char liz_hunger_desc [MAX_HUNGER_POINTS + 1][10] = { "Sated",
						     "Peckish",
						     "Hungry",
						     "Famished",
						     "Starved",
						     "STARVING" };

unsigned int spawn_turn_switch,
  capture_switch,
  volcano_dormant_switch,
  peak_active_switch,
  world_wrap_switch,
  victory_points_switch,
  home_owned_worth,
  home_victory_points,
  give_switch;

