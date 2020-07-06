/* ----------------------------------------------------------------------
--
-- Declare variable in library rather than in each program
--
---------------------------------------------------------------------- */
#include "lizard.h"
#include "map.h"

char *game_code, *data_path, *disk_data_path, *email_data_path;

int world_x, world_y, number_orders, turn_number, number_spys;

char *strWorldInit = "worldinit";
char *strWorldFfro = "worldffro";
char *strWorldFill = "worldfill";
char *strWorldPop = "worldpop";
char *strWorldFix = "worldfix";
char *strOrderEnt = "orderent";

char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char player_clan_name [MAX_PLAYERS + 1][37];

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_cred [MAX_PLAYERS + 1][10];
char player_output_mode [MAX_PLAYERS + 1];

char game_due_date [20];

int players_in_game;

spy_t *spy_list [MAX_SPYS];
int number_spys;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][12] = { "Unsettled", "Red", "Green", "Grey", "Black", "Yellow" };
