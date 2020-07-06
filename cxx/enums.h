#define LZ_VERSION "0.0.1"

#define LZ_TRUE 1
#define LZ_FALSE 0

/* various text and binary orders files used by Lizards! */

#define TEXT_ORDS_F                   "LORDS"   /* text order file */
#define POSTSCRIPT_F                  ".ps"  /* postscript turn file */
#define PSHEADER_F	       	"lzheader.ps"

/* various text output files for players. */

#define PLAYER_MESG_F                 "LP"   /* general player text */
#define PLAYER_LIST_F                 "LL"   /* modem user view list */
#define PLYMAP_MESG_F                 "LM"   /* text maps */
#define PLYPLY_MESG_F                 "LC"   /* text messages */
#define PLYERR_MESG_F		      "LE"   /* error messages */
/* Lizards! binary and text data files. */

#define DEN_LIZARD_LEVEL 50
#define DEN_RECOVER_LEVEL 0.25
#define DEN_STARVE_LEVEL 0.20
#define DEN_INCREASE_STARVE_LEVEL 0.15
#define DEN_MILITIA_LEVEL 0.30
#define MINIMUM_POP_LEVEL 10
#define POP_ADDITION 4
#define DEN_RECRUIT_LEVEL 0.35

#define HOME_SIZE 400

#define MAX_LIZARDS_IN_HEX 300
#define MAX_CLAN_CODE 3
#define MAX_CLAN_NAME 20
#define MAX_ORDERS_PER_TURN 18
#define MAX_RANDOM_MESSAGES 20

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
#define DEN_RECRUIT_LEVEL 0.35
#define DEN_RECOVER_LEVEL 0.25
#define DEN_STARVE_LEVEL 0.20
#define DEN_INCREASE_STARVE_LEVEL 0.15
#define DEN_MILITIA_LEVEL 0.30
#define MINIMUM_POP_LEVEL 10
#define POP_ADDITION 4

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

#define LIZ_SPAWN 0x0001
#define LIZ_CAPTURE 0x0002
#define LIZ_VOLCANO 0x0004
#define LIZ_PEAK 0x0008
#define LIZ_WRAP 0x0010
#define LIZ_VICTORY 0x0020
#define LIZ_GIVE 0x0040

#define LIZARD_TYPES 5

enum Terrain {
  WATER,
  PLAIN,
};

enum Direction {
  NORTH,
  NORTHEAST,
  SOUTHEAST,
  SOUTH,
  SOUTHWEST,
  NORTHWEST,
  DIRECTIONS
};

enum Types {
  RED,
  GREEN,
  GREY,
  BLACK,
  YELLOW,
  UNSETTLED,
};

enum Feature {
  GENERIC,
  FERTILE,
  SWAMP,
  SCRUB,
  PEAK,
  VOLCANO,
  WHIRLPOOL,
  CURSED,
  RUIN,
  TEMPLE,
  DEN,
  HALF_DEN,
  BAND,
  SPY,
  BRIDGE,
  RAFT,
  BUILDING
};

enum Order {
  INACTIVE,
  PLAYER,
  MOVE,
  SPLIT,
  SAIL,
  SNEAK,
  RECRUIT,
  BUILD_RAFT,
  BUILD_FERTILE,
  BUILD_DEN,
  BUILD_BRIDGE,
  DESTROY_FERTILE,
  DESTROY_BRIDGE,
  FREE,
  LOB,
  GIVE,
  CHANT,
  MESSAGE,
  ERUPT_MOUNTAIN,
  ERUPT_VOLCANO,
  DORMANT_VOLCANO,
  MOVE_WHIRLPOOL,
  KILL_PLAYER,
  MOD_MESG,
  SPAWN_TURN,
};

enum Chants {
  CHANT_EYES,
  CHANT_REVE,
  CHANT_SUMM,
  CHANT_MOUL,
  CHANT_FREE,
  CHANT_COER,
  CHANT_TRAN,
  CHANT_DARK,
  CHANT_SHUN,
  CHANT_GATE,
  NUMBER_CHANTS,
};

enum Phase {
  SYSTEM,
  ARTILLARY,
  MOVEMENT,
  DEVELOPMENT,
};

enum Status {
  PLAYER_INIT,
  PLAYER_DEAD,
  PLAYER_RETIRED,
  PLAYER_ACTIVE
};

enum Combat {
  COMBAT_ATTACK_FLEE,
  COMBAT_DEFEND_FLEE,
  COMBAT_ATTACK_DEAD,
  COMBAT_DEFEND_DEAD,
  COMBAT_BOTH_DEAD,
};
