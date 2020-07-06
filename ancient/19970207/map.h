#ifndef __LZ_MAP_H
#define __LZ_MAP_H

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

#define PLAIN_C   " ____ "
#define DEN_C     "DEN"               /* Special case, colour suffix added! */
#define HOME_DEN_C "HOM"
#define RUIN_C    " Ruin "
#define FERTILE_C "| | | "
#define SWAMP_C   "_\\|/_ "
#define PEAK_C    "^/\\^  "
#define VOLCANO_C "/|\\^  "
#define WATER_C   "~ ~ ~ "
#define WHIRLPOOL_C "Whirl!"
#define BRIDGE_C  "Bridge"
#define SCRUB_C "*_*_*_"
#define TEMPLE_C "Temple"
#define CURSED_C "Cursed"
#define RAFT_C   "=Raft="

#define PLAIN_GC ".lb pla.blk\n.ke f10\n.bf mono\n.kr OFF"
#define UNS_DEN_GC ".lb uden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define RED_DEN_GC ".lb rden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define GRN_DEN_GC ".lb gden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define GRY_DEN_GC ".lb grden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define BLK_DEN_GC ".lb bden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define YEL_DEN_GC ".lb yden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define HOME_DEN_GC ".lb hden.blk\n.ke f10\n.bf mono\n.kr OFF"
#define RUIN_GC ".lb rui.blk\n.ke f10\n.bf mono\n.kr OFF"
#define FERTILE_GC ".lb fer.blk\n.ke f10\n.bf mono\n.kr OFF"
#define SWAMP_GC ".lb swp.blk\n.ke f10\n.bf mono\n.kr OFF"
#define PEAK_GC ".lb mtn.blk\n.ke f10\n.bf mono\n.kr OFF"
#define VOLCANO_GC ".lb vol.blk\n.ke f10\n.bf mono\n.kr OFF"
#define WATER_GC ".lb wat.blk\n.ke f10\n.bf mono\n.kr OFF"
#define WHIRLPOOL_GC ".lb whi.blk\n.ke f10\n.bf mono\n.kr OFF"
#define BRIDGE_GC ".lb brg.blk\n.ke f10\n.bf mono\n.kr OFF"
#define SCRUB_GC ".lb bru.blk\n.ke f10\n.bf mono\n.kr OFF"
#define TEMPLE_GC ".lb tem.blk\n.ke f10\n.bf mono\n.kr OFF"
#define CURSED_GC ".lb cur.blk\n.ke f10\n.bf mono\n.kr OFF"
#define RAFT_GC ".lb raf.blk\n.ke f10\n.bf mono\n.kr OFF"

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

#define UNUSED_VAR 6

typedef struct
{
  int world_x, world_y;
  int terrain,
    hex_owner;                     /* Which player last captured hex */
  char  bridge_here,
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
  
  int red_lizards,
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


extern spy_t *spy_list [MAX_SPYS];
extern int number_spys;

#endif
