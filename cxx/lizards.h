#include "enums.h"

#define STL 1

class debug_t {
 public:
  char *logfilename;
  char *bannerstr;
  FILE *stream;
  debug_t();
  ~debug_t();
  void banner(const char *);
  void log(const char *s, ...);
};

class misc_t {
 public:
  static debug_t dbg;
  static int uniform(int);
  static const char *colours(int);
  static const char *combat_desc(int);
  static const char *hunger_desc(int);
};


class base_t {
 public:
  virtual ~base_t();
};

class point_t {
public:
  int x,y;
  point_t();
  point_t(int x_p, int y_p);
  int operator==(const point_t& p);
  int operator!=(const point_t& p);
  point_t& operator+=(const point_t &p);
  point_t operator+(const point_t &p);
};

class adjacent_t {
 public:
  point_t xy;
  class hex_t *hex;
};

class dir_t {
 private:
  int d;
 public:
  dir_t();
  dir_t(int);
  dir_t(const char*);
  operator char*();
  operator int();
  operator Direction();
  dir_t& operator+=(int);
  dir_t& operator-=(int);
  int operator()(char*);
  int okay();
};

class dirs_t {
 private:
  char *text;
  dir_t d[2];
 public:
  ~dirs_t();
  dirs_t();
  dirs_t(dir_t, dir_t);
  dirs_t(dirs_t&);
  operator char*();
  dirs_t& operator=(dir_t&);
  dir_t& operator[](int);
};


/* ----------------------------------------------------------------------
--
-- Feature declations
--
---------------------------------------------------------------------- */

class lizard_t {
 private:
  char *desc;
 public:
  int type;
  int size;
  lizard_t(int, int);
};

class feature_t : public base_t {
 protected:
  char *desc;
  feature_t(int, char*);
 public:
  int type;
  int owner;
  feature_t();
  virtual ~feature_t();
  virtual void debug(FILE *);
  virtual feature_t* clone();
  virtual void save_xml(xmlpp::Element*);
  virtual char *describe();
};

class den_t : public feature_t {
 public:
  int pop;
  int colour;
  int hunger;
  int home;
  int timebomb;
  den_t();
  den_t(den_t&);
  den_t(int);
  den_t *clone();
  char *describe();
  operator char*();
  den_t(xmlpp::Element*);
  void save_xml(xmlpp::Element*);
};

class half_den_t : public feature_t {
 public:
  int undisturbed;
  half_den_t();
  half_den_t(xmlpp::Element*);
};

class band_t : public feature_t {
 public:
  int colour[LIZARD_TYPES];
  int hunger;
  int combat;
  int immobile;
  band_t();
  band_t(band_t&);
  band_t(int[]);
  band_t(xmlpp::Element*);
  band_t* clone();
  int size();
  char *describe();
  void min(band_t&);
  int kill(int);
  int capture(band_t&, int);
  int inc_combat();
  band_t operator-(band_t&);
  band_t& operator+=(band_t&);
  band_t& operator-=(band_t&);
  operator char*();
  void save_xml(xmlpp::Element*);
};

class swamp_t : public feature_t {
 public:
  swamp_t();
  swamp_t(swamp_t&);
  swamp_t(xmlpp::Element*);
  swamp_t *clone();
  void save_xml(xmlpp::Element*);
};

class scrub_t : public feature_t {
 public:
  scrub_t();
  scrub_t(scrub_t&);
  scrub_t(xmlpp::Element*);
  scrub_t* clone();
  void save_xml(xmlpp::Element*);
};

class fertile_t : public feature_t {
 public:
  int level;
  fertile_t();
  fertile_t(fertile_t&);
  fertile_t(xmlpp::Element*);
  fertile_t* clone();
  char *describe();
  void save_xml(xmlpp::Element*);
};

class temple_t : public feature_t {
 public:
  temple_t();
  temple_t(temple_t&);
  temple_t(xmlpp::Element*);
  temple_t* clone();
  void save_xml(xmlpp::Element*);
};

class ruin_t : public feature_t {
 public:
  ruin_t();
  ruin_t(ruin_t&);
  ruin_t(xmlpp::Element*);
  ruin_t* clone();
  void save_xml(xmlpp::Element*);
};

class cursed_t : public feature_t {
 public:
  cursed_t();
  cursed_t(cursed_t&);
  cursed_t(xmlpp::Element*);
  cursed_t* clone();
  void save_xml(xmlpp::Element*);
};

class peak_t : public feature_t {
 public:
  peak_t();
  peak_t(peak_t&);
  peak_t(xmlpp::Element*);
  peak_t *clone();
  void save_xml(xmlpp::Element*);
};

class volcano_t : public feature_t {
 public:
  volcano_t();
  volcano_t(volcano_t&);
  volcano_t(xmlpp::Element*);
  volcano_t *clone();
  char *describe();
  void save_xml(xmlpp::Element*);
};

class whirlpool_t : public feature_t {
 public:
  whirlpool_t();
  whirlpool_t(whirlpool_t&);
  whirlpool_t* clone();
  whirlpool_t(xmlpp::Element*);
  void save_xml(xmlpp::Element*);
};

class bridge_t : public feature_t {
 public:
  bridge_t();
  bridge_t(xmlpp::Element*);
  bridge_t* clone();
  void save_xml(xmlpp::Element*);
};

class raft_t : public feature_t {
 public:
  int life;
  raft_t();
  raft_t(raft_t&);
  raft_t* clone();
  raft_t(xmlpp::Element*);
  void save_xml(xmlpp::Element*);
};

class spy_t : public feature_t {
 public:
  int away;
  spy_t();
  spy_t(xmlpp::Element*);
  void save_xml(xmlpp::Element*);
};

class hex_t : public base_t {
 private:
  // adjacent_t *adjlist;
#ifdef STL
  std::vector<feature_t*> fv;
#else
  int flistlen;
  feature_t **flist;
#endif
public:
  class map_t *map;
  point_t xy;
  int tmpl;
  int seen;
  int owner;
  char *title;
  Terrain terrain;

  hex_t(void);
  ~hex_t(void);
  hex_t& operator=(hex_t&);
  void save(FILE *);
  void save_features(FILE *);
  void set(int, int);
  void setmap(class map_t*);
  void save_xml(xmlpp::Element*);
  void remap(class map_t*);
  int size();
  // void set_adjacent(dir_t, hex_t*);
  hex_t* adjacent(dir_t);
  hex_t* adjacent(const char*);
  int num_features(void);
  feature_t* feature(int);
  feature_t* has_feature(Feature);
  feature_t* has_feature(feature_t*);
  int terrain_adjacent(Terrain);
  int feature_adjacent(Feature);
  feature_t* add_feature(feature_t*);
  feature_t* del_feature(feature_t*);
  int count_feature(Feature);
  int count_feature(Feature, int);
  feature_t* pop_feature();
  void clear_features();
  void debug(FILE *);
  void reload(class xmlpp::Element*, class map_t *);
  void disturbed();
  operator char*();
};

class player_t : public base_t {
 public:
  int num;
  int status;
  char *ref;
  char *code;
  char *clan;
  char *startup;
  FILE *fptr;
  int num_orders;
  int home_dens;
  void debug(FILE *);
  void mesg(int, const char*, ...);
  player_t();
};

class map_t : public base_t {
 private:
  hex_t* hexes;
  hex_t* xy(int x, int y);
 public:
  int width, height, size;
  map_t();
  map_t(const int, const int);
  map_t(xmlpp::Element*);
  map_t(map_t&);
  ~map_t();
  map_t& operator=(map_t&);
  hex_t* hex(const int, const int);
  hex_t* hex(const int);
  hex_t* hex(const point_t);
  hex_t* operator[](const int);
  hex_t* operator[](const point_t);
  hex_t* operator()(const int, const int);
  hex_t* operator()(const int);
  hex_t* operator()(const point_t);
  hex_t* operator()(const char*);
  void resize(const int, const int);
  void desc_short(FILE*);
  void desc_long(FILE*);
  point_t move(point_t&, Direction);
  void create_nowrap();
  void save_xml(xmlpp::Element*);
  void debug(FILE*);
};

class order_t {
 protected:
  char *text;
 public:
  class turn_t *turn;
  Order type;
  Phase phase;
  int player;
  hex_t* src, *dst;
  dirs_t dirs;
  band_t *band;
  order_t(class turn_t*, int);
  order_t(class turn_t*, int, hex_t*);
  order_t(class turn_t*, int, hex_t*, hex_t*);
  order_t(class turn_t*, int, hex_t*, hex_t*, dir_t);
  order_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  virtual char* describe();
  virtual void show();
  virtual int execute();
  operator char*();
  int operator==(order_t&);
  int operator<=(order_t&);
};

/* --------------------
   Movement orders
   -------------------- */

class order_movement_t : public order_t {
 public:
  order_movement_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t);
  int movement();
};

class order_split_t : public order_movement_t {
 public:
  order_split_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t);
  char* describe();
  void show();
  int execute();
};

class order_move_t : public order_movement_t {
 public:
  order_move_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t);
  char* describe();
  void show();
  int execute();
};

class order_sail_t : public order_movement_t {
 public:
  order_sail_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t);
  void show();
  int execute();
};

class order_sneak_t : public order_movement_t {
 public:
  order_sneak_t(class turn_t *t, int player, hex_t *src, hex_t* dst, dirs_t);
  void show();
  int execute();
};

/* --------------------
   Build orders
   -------------------- */

class order_build_fertile_t : public order_t {
 public:
  order_build_fertile_t(class turn_t*, int, hex_t *);
  void show();
  int execute();
};

class order_destroy_fertile_t : public order_t {
 public:
  order_destroy_fertile_t(class turn_t*, int, hex_t *);
  void show();
  int execute();
};

class order_build_raft_t : public order_t {
 public:
  order_build_raft_t(class turn_t*, int, hex_t*, hex_t*, dir_t);
  void show();
  int execute();
};

class order_build_bridge_t : public order_t {
 public:
  order_build_bridge_t(class turn_t*, int, hex_t*, hex_t*, dir_t);
  void show();
  int execute();
};

class order_destroy_bridge_t : public order_t {
 public:
  order_destroy_bridge_t(class turn_t*, int, hex_t*, hex_t*, dir_t);
  void show();
  int execute();
};

class order_build_den_t : public order_t {
 public:
  order_build_den_t(class turn_t*, int, hex_t *);
  void show();
  int execute();
};

/* --------------------
   Artillary orders
   -------------------- */

class order_chant_t : public order_t {
 public:
  int chant;
  const char *abbrev, *name;

  order_chant_t(class turn_t*, int, hex_t*, hex_t*);
  order_chant_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};

class order_chant_eyes_t : public order_chant_t {
 public:
  order_chant_eyes_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_reve_t : public order_chant_t {
 public:
  order_chant_reve_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_summ_t : public order_chant_t {
 public:
  order_chant_summ_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_gate_t : public order_chant_t {
 public:
  order_chant_gate_t(class turn_t*, int, hex_t*, hex_t*);
  void show();
  int execute();
};
class order_chant_shun_t : public order_chant_t {
 public:
  order_chant_shun_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_tran_t : public order_chant_t {
 public:
  order_chant_tran_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_moul_t : public order_chant_t {
 public:
  order_chant_moul_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_coer_t : public order_chant_t {
 public:
  order_chant_coer_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_dark_t : public order_chant_t {
 public:
  order_chant_dark_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};
class order_chant_free_t : public order_chant_t {
 public:
  order_chant_free_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};

class order_lob_t : public order_t {
 public:
  order_lob_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  void show();
  int execute();
};

/* --------------------
   Development / Den orders
   -------------------- */

class order_recruit_t : public order_t {
 public:
  order_recruit_t(class turn_t*, int, hex_t*);
  char* describe();
  void show();
  int execute();
};

class order_free_t : public order_t {
 public:
  order_free_t(class turn_t*, int, hex_t*);
  void show();
  int execute();
};

/* --------------------
   System orders
   -------------------- */

class order_message_t : public order_t {
 public:
  order_message_t(class turn_t*, int, int, char*);
  void show();
  int execute();
};

class order_erupt_mountain_t : public order_t {
 public:
  order_erupt_mountain_t(class turn_t*, int, hex_t*);
  char* describe();
  //  void show();
  //  int execute();
};

class order_erupt_volcano_t : public order_t {
 public:
  order_erupt_volcano_t(class turn_t*, int, hex_t*);
  char* describe();
  //  void show();
  //  int execute();
};

class order_dormant_volcano_t : public order_t {
 public:
  order_dormant_volcano_t(class turn_t*, int, hex_t*);
  char* describe();
  //  void show();
  //  int execute();
};

class order_move_whirlpool_t : public order_t {
 public:
  order_move_whirlpool_t(class turn_t*, int, hex_t*, hex_t*, dirs_t);
  char* describe();
  //  void show();
  //  int execute();
};


class world_t : public base_t {
public:
  time_t due;
  int turn;
  int flags;
  char *game;
  char *path;
  char *dir;
  int num_players;
  player_t **player;
  map_t *map;
  map_t **oldmaps;

  world_t(int, int, const char *, const char *);
  world_t(int x, int y, const char *playerfile, int argc, char *argv[]);
  world_t(const char *);
  ~world_t();

  void load();
  void save();

  int open_player_files(void);
  int close_player_files(void);
  player_t* get_player(int);
  player_t* clear_player(int);

  void desc_short(FILE*);
  void desc_long(FILE*);
  void debug(FILE*);
 private:
  char** getline(FILE*);
  int lay_island(int x, int y, double land_density);
  void lay_template(int, int, hex_t*, FILE*, class cgrid_t&);
  void build_hex (int *points, hex_t *hex, char **tokens);
  void build_startup(int p, FILE *template_fptr, FILE *log_fptr, class cgrid_t&);
  void fill(char *filename, int optind, int argc, char *argv[]);
  void custom_startup();
  void populate(int, int);
  void fix();
};

class turn_t {
 public:
  world_t *world;
  order_t **order_list;
  int num_orders;

  turn_t(world_t*);
  int textorderconvert();
  int add_order(order_t*);
  int terrain();
  int ordershuffle();
  int worldopen();
  int worldmaster();
  int worldclose();
  int clanreport();
  int clanrank();
  int mesgappend();

  int combat(band_t*, band_t*, hex_t*, hex_t*, int);
  int combat_sea_monster(band_t*, hex_t*, int);
  void hex_moved_into(hex_t*);
  int pop_growth(int, int);
  void open_hex(hex_t*);
  void open_den(hex_t*, den_t*);
  void close_hex(hex_t*);
  void do_pass(int);
  void close_spies();
};

extern char *colours[];

extern void WorldFill(world_t *, char *, int, int, char **);
extern void WorldPopulate(world_t *, int, int);
extern void WorldFix(world_t *);
extern void CustomStartup(world_t *);
extern void ClanReport(world_t*);

// ----------------------------------------------------------------------
//
// Global variables and misc functions
//
// ----------------------------------------------------------------------
extern const char* liz_colours[];
extern const char* liz_hunger_desc[];
extern const char* liz_combat_desc[];

extern debug_t dbg;
extern char *progname;
extern int uniform(int);
