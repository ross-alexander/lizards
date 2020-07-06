#include <String.h>
extern "C" {
#include "lizard.h"
#include "map.h"
}

class LzPaths {
 public:
  String gameCode;
  String dataPath;

  LzPaths(int, char **);
  LzPaths(char *, char*);
};

// ----------------------------------------------------------------------

class Hex {
 public:
  char terrain,
       hex_owner,                     /* Which player last captured hex */
       bridge_here,
       den_lizard_type,
       turns_hungry,                  /* Hunger factor for Lizard bands */
       turns_den_hungry,              /* For escalating starvation! */
       combat_points,                 /* For Lizard bands combat experience */
       turns_undisturbed,             /* For building / destroying purposes */
       bridge_dir,
       raft_here,
       lizards_immobile,
       template_hex,                  /* for world adjustment post template */
       unused [UNUSED_VAR];

  unsigned int red_lizards,
               green_lizards,
               grey_lizards,
               black_lizards,
               yellow_lizards,
               lizard_pop;            /* Passive Lizard pop for Den hexes */
};

// ----------------------------------------------------------------------

class Player {
 public:
  String playerCode;
  String playerName;
  String playerClanName;
  String playerHomeDen;
  String playerRoad;
};

class Game {
 public:
  int number_players;
  Player *players;
  Game(LzPaths&);
  String game_due_date;
  spy_t *spy_list [MAX_SPYS];
  int number_spys;
  int turn_number;
};

class World {
 public:
  int x, y;
  unsigned int spawn_turn_switch,
             capture_switch,
	     volcano_dormant_switch,
	     peak_active_switch,
	     world_wrap_switch,
             victory_points_switch,
             home_owned_worth,
	     home_victory_points,
	     give_switch;
  Hex *hexes;
  World(void);
};
