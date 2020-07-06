#ifndef __LIZARD_H
#define __LIZARD_H

#include "compat.h"

#define LIZ_TRUE 1
#define LIZ_FALSE 0

#define VER_STRING "Ver 1.1b"

#define COPYRIGHT_STRING "1991-97"

#define MAX_PLAYERS 16
#define MAX_CLAN_CODE 3
#define MAX_CLAN_NAME 35

#define MAX_RANDOM_MESSAGES 100

#define MAX_SPYS 1000

#define MAX_LIZARDS_IN_HEX 300

#define PERCENT_HOME_DENS_FOR_VICTORY 0.60

/* various text and binary orders files used by Lizards! */

#define TEXT_ORDS_F                   "LORDS"   /* text order file */
#define BBS_TEXT_ORDS_F               "LEXOR"   /* extra text order file */
#define TEXT_START_ORDS_F             "LSTAR"   /* startup text order file */
#define ORDS_PROC_F                   "LORDB"  /* binary orders */
#define ORDS_SHUF_F                   "LORDR"  /* shuffled binary orders */
#define POSTSCRIPT_F                  "LPOST"  /* postscript turn file */

/* various text output files for players. */

#define PLAYER_MESG_F                 "LP"   /* general player text */
#define PLAYER_LIST_F                 "LL"   /* modem user view list */
#define PLYMAP_MESG_F                 "LM"   /* text maps */
#define PLYPLY_MESG_F                 "LC"   /* text messages */
#define PLYERR_MESG_F		      "LE"   /* error messages */

/* Lizards! binary and text data files. */

#define WORLD_DATA_F                  "LWLDATA"   /* binary hex file */
#define WORLD_STAT_F                  "LWLSTAT"   /* text world statistics */
#define WORLD_TURN_F                  "LWLTURN"   /* lizards turn file */
#define WORLD_SWITCH_F                "LWLSWIT"   /* text game switches */
#define REAL_WORLD_SWITCH_F           "LWLSWITC"
#define HOME_STAT_F                   "LPLYHOM"  /* list of home dens */
#define PLAYER_F                      "LPLAYER"  /* text player list */
#define SPY_F                         "LSPYS"    /* binary spy file */
#define LIZ_MESG_F                    "LMESGS.TXT" /* text random mesg file */

#define LZ_GAMECODELEN 8
#define LZ_MAXPATH 256

#define LZ_FALSE 0
#define LZ_TRUE 1

#define PLAIN 0
#define DEN 1
#define RUIN 2
#define FERTILE 3
#define SWAMP 4
#define PEAK 5
#define VOLCANO 6
#define WATER 7
#define WHIRLPOOL 8
#define SCRUB 9
#define TEMPLE 10
#define CURSED 11
#define HOME_DEN 12

#define RED_LIZARD 1
#define GREEN_LIZARD 2
#define GREY_LIZARD 3
#define BLACK_LIZARD 4
#define YELLOW_LIZARD 5

#define LIZARD_TYPES 5

#define MAX_COMBAT_POINTS 5
#define CP_MOD_LEVEL 3 /* %attack increase for every CP */

#define MIN_CAPTURE_PERCENT 30
#define CP_CAPTURE_MOD_LEVEL 3 /* %capture increase for every CP */

#define MAX_HUNGER_POINTS 5 /* before starving */
#define STARVE_PER_TURN 50 /* % chance of starving to death */

#define BASE_ATTACK 50
#define DEN_ATTACK 25
#define BASE_DEFEND 50
#define LOB_ATTACK 70
#define MONSTER_DEFEND 50

#define DEN_LIZARD_LEVEL 50 /* Increase in Max Pop for every adjacent Fertile */
#define DEN_RECOVER_LEVEL 0.25
#define DEN_STARVE_LEVEL 0.20
#define DEN_INCREASE_STARVE_LEVEL 0.15
#define DEN_MILITIA_LEVEL 0.30
#define MINIMUM_POP_LEVEL 10
#define POP_ADDITION 4

#define DEN_RECRUIT_LEVEL 0.35

#define SPY_CAPTURE_LEVEL 200 /* Lizards in hex that give automatic capture! */

#define SPY_BUSH_TURNS 3

#define SWAMP_BRIDGE_EAT_LEVEL 10 /* Chance of Swamp eating Bridges */

#define SEA_MONSTER_LEVEL 10
#define SWAMP_MIRE_LEVEL 20
#define VOLCANO_ERUPT_LEVEL 8
#define VOLCANO_SINK_LEVEL 30
#define VOLCANO_DORMANT_LEVEL 8
#define MOUNTAIN_ERUPT_LEVEL 8
#define WHIRLPOOL_KILL_LEVEL 0.80
#define COLLAPSE_KILL_LEVEL 0.40
#define CURSED_KILL_LEVEL 0.80

#define SPY_LAVA_KILL_LEVEL 50
#define LAVA_KILL_LEVEL 0.50

#define SEA_MONSTER_STRENGTH_LEVEL 15

#define BUILD_FERTILE_LEVEL 10
#define BUILD_RAFT_LEVEL 20
#define BUILD_BRIDGE_LEVEL 20
#define BUILD_DEN_LEVEL 50

#define RAFT_LIFE 3

#define NUMBER_CHANTS 10

#define PLAIN_PC "pla "
#define EMPTY_DEN_PC "eden "
#define RED_DEN_PC "rden "
#define GRN_DEN_PC "gden "
#define GRY_DEN_PC "grden "
#define BLK_DEN_PC "bden "
#define YEL_DEN_PC "yden "
#define HOME_DEN_PC "hden "
#define RUIN_PC "rui "
#define FERTILE_PC "fer "
#define SWAMP_PC "swp "
#define PEAK_PC "mou "
#define VOLCANO_PC "vol "
#define WATER_PC "wat "
#define WHIRLPOOL_PC "whi "
#define BRIDGE_PC "bri "
#define SCRUB_PC "bru "
#define TEMPLE_PC "tem "
#define CURSED_PC "cur "
#define RAFT_PC "raf "
#define HALF_DEN_PC "uden "

#define LZ_BRIDGE_NONE 0x0001
#define LZ_BRIDGE_NS 0x0002
#define LZ_BRIDGE_NE 0x0004
#define LZ_BRIDGE_SE 0x0008
#define LZ_RAFT_HERE 0x0010

#define LZ_SPAWN 0x0001
#define LZ_CAPTURE 0x0002
#define LZ_VOLCANO 0x0004
#define LZ_PEAK 0x0008
#define LZ_WRAP 0x0010
#define LZ_VICTORY 0x0020
#define LZ_GIVE 0x0040

/* ----------------------------------------------------------------------
--
-- Lots of global variables
--
---------------------------------------------------------------------- */
#define NUM_TEXT_ORDERS 9

#define MAX_ORDERS_PER_TURN 600

#define INACTIVE 0
#define MOVE 1
#define SPLIT 2
#define BUILD_FERTILE 3
#define BUILD_DEN 4
#define BUILD_BRIDGE 5
#define DESTROY_FERTILE 6
#define DESTROY_BRIDGE 7
#define RECRUIT 8
#define ERUPT_VOLCANO 9
#define COLLAPSE_BRIDGE 10
#define MOVE_WHIRLPOOL 11
#define CHANT 12
#define LOB 13
#define GIVE 14
#define FREE 15
#define BUILD_RAFT 16
#define SAIL 17
#define ERUPT_MOUNTAIN 18
#define DORMANT_VOLCANO 19
#define KILL_PLAYER 20
#define SNEAK 21

#define NORTH 1
#define NORTH_EAST 2
#define SOUTH_EAST 3
#define SOUTH 4
#define SOUTH_WEST 5
#define NORTH_WEST 6

typedef struct
{
  int player,
       order_type,
       moves [2];

  int red_lizards,
      green_lizards,
      grey_lizards,
      black_lizards,
      yellow_lizards,
      x_hex,
      y_hex;
} order_t;

typedef struct
{
  int x, y,
    terrain,
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
    tmpl_hex,                  /* for world adjustment post template */
    red_lizards,
    green_lizards,
    grey_lizards,
    black_lizards,
    yellow_lizards,
    lizard_pop;            /* Passive Lizard pop for Den hexes */
} hex_t;

typedef struct
{
  int player,
      x_hex,
      y_hex,
      lizard_type,
      turns_alone,
      moved_this_turn,		      /* Check against multiple spy moves */
      mired,
      unused [4];
} spy_t;

typedef struct
{
  int x, y, spy, peak;
} unit_t;

typedef struct {
  int cnt;
  unit_t *list;
} BandList;

typedef struct
{
  int x, y, centred_x, centred_y, wrapped;
} see_t;

typedef struct {
  int cnt;
  see_t *list;
} HexList;

typedef struct {
  char code[MAX_CLAN_CODE + 1];
  char clan_name[MAX_CLAN_NAME + 1];
  int home_den;
  FILE *fptr;
} Player;

typedef struct {
  int x;
  int y;
  int turn;
  char *due_date;
  int flags;
  int home_owned_worth;
  int home_victory_points;
  int num_spies;
  spy_t **spylist;
  int num_players;
  Player *player;
  hex_t *hexes;
  int num_orders;
  order_t *order_list [MAX_ORDERS_PER_TURN];
} World;

extern char *ShowLizards(char *, hex_t*);

extern char dir[7][3];
extern char lizard_colours[6][12];
extern char* liz_hunger_desc[MAX_HUNGER_POINTS + 1];
extern char* liz_cp_desc[MAX_COMBAT_POINTS + 1];
extern char* liz_terrain[14];

extern char *data_path;
extern char *game_code;
extern char *message_list [MAX_RANDOM_MESSAGES];

extern World *world;
extern char *strWorldInit, *strWorldFfro, *strWorldFill, *strWorldPop;
extern char *strWorldFix, *strOrderEnt;

#endif
