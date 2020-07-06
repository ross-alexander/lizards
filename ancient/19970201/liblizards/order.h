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
  char player,
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
