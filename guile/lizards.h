typedef struct hex {
  int x;
  int y;
  int age;
  int owner;
} Hex;

typedef struct spy {
  int x;
  int y;
  int age;
  int owner;
} Spy;

typedef struct feature {
  int x;
  int y;
  int age;
  int owner;
} Feature;

typedef struct player {
  int id;
  char *clan;
  char *name;
} Player;

typedef struct world
{
  int x;
  int y;
  int turn;
  char *game;
  SCM players;
  SCM hexlist;
  struct hex* map;
} World;

extern long hex_tag;

extern void init_world(void);
extern void init_spy(void);
extern void init_hex(void);
extern void init_player(void);
extern void init_feature(void);
