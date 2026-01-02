#define BLANK typeid(hex_t).name()
#define WATER typeid(water_t).name()
#define PLAINS typeid(plains_t).name()
#define RUIN typeid(ruin_t).name()
#define SWAMP typeid(swamp_t).name()
#define VOLCANO typeid(volcano_t).name()
#define WHIRLPOOL typeid(whirlpool_t).name()
#define FERTILE typeid(fertile_t).name()
#define TEMPLE typeid(temple_t).name()
#define PEAK typeid(peak_t).name()
#define SCRUB typeid(scrub_t).name()
#define CURSED typeid(cursed_t).name()

#define DEN typeid(den_t).name()
#define EMPTY_DEN typeid(den_t).name()
#define RED_DEN typeid(red_den_t).name()
#define GREEN_DEN typeid(green_den_t).name()
#define YELLOW_DEN typeid(yellow_den_t).name()
#define GREY_DEN typeid(grey_den_t).name()
#define BLACK_DEN typeid(black_den_t).name()
#define HOME_DEN typeid(home_den_t).name()

#define BAND typeid(band_t).name()
#define RAFT typeid(raft_t).name()
#define BRIDGE typeid(bridge_t).name()
#define HALF_DEN typeid(half_den_t).name()

#define TEMPLATE_CODE "TEMPLATE"
#define HOME_HEX_CODE "HOME"

#define TERRAIN_TYPES 16

#define WATER_CODE	"WAT"
#define PLAINS_CODE	"PLA"
#define SCRUB_CODE	"SCR"
#define FERTILE_CODE	"FER"
#define SWAMP_CODE	"SWA"
#define RUIN_CODE	"RUI"
#define TEMPLE_CODE	"TEM"
#define PEAK_CODE	"PEA"
#define VOLCANO_CODE	"VOL"
#define CURSED_CODE	"CUR"
#define WHIRLPOOL_CODE	"WHI"

#define RED_DEN_CODE	"RED"
#define YELLOW_DEN_CODE	"YEL"
#define GREEN_DEN_CODE	"GRN"
#define GREY_DEN_CODE	"GRY"
#define BLACK_DEN_CODE	"BLK"
#define EMPTY_DEN_CODE  "DEN"

#define RED_LIZARD_CODE		"RED"
#define YELLOW_LIZARD_CODE	"YEL"
#define GREEN_LIZARD_CODE	"GRN"
#define GREY_LIZARD_CODE	"GRY"
#define BLACK_LIZARD_CODE	"BLK"

#define DEN_LIZARD_LEVEL 50 /* Increase in Max Pop for every adjacent Fertile */
#define DEN_MILITIA_LEVEL 0.30

enum Format {
  FORMAT_FILE_TXT,
  FORMAT_FILE_XML,
  FORMAT_FILE_JS,
  FORMAT_STRING_TXT,
  FORMAT_STRING_XML,
  FORMAT_STRING_JS
};

enum Overlay {
  OVERLAY_CHECK,
  OVERLAY_MARK,
  OVERLAY_CLEAR
};

enum Features {
  FEATURE_BAND,
  FEATURE_RAFT,
  FEATURES,
};

enum Direction {
  NORTH,
  NORTHEAST,
  SOUTHEAST,
  SOUTH,
  SOUTHWEST,
  NORTHWEST,
  DIRECTIONS,
};

enum Types {
  RED,
  GREEN,
  GREY,
  BLACK,
  YELLOW,
  UNSETTLED,
};

enum Owned {
  OWNED_ALL,
  OWNED_BAND_ONLY,
  OWNED_SPY_ONLY,
  OWNED_BAND_SPY
};


/* ----------------------------------------------------------------------
--
-- class definitions
--
---------------------------------------------------------------------- */

class serial_t {
 public:
  struct json_object *json;
  serial_t(struct json_object*);
  serial_t();
  serial_t(const char*);
  serial_t(std::string&);
  serial_t(int);
  serial_t(double);
  static serial_t from_file(const char*);
  static serial_t from_string(const char*);
  int to_file(const char*);
  const char *to_string(unsigned int i = 0);
  int get(const char*, int&);
  int get(const char*, const char*&);
  int get(const char*, std::string&);
  int get(const char*, class serial_map_t&);
  int get(const char*, class serial_array_t&);
};

class serial_array_t : public serial_t {
 public:
  serial_array_t();
  serial_array_t(serial_t);
  serial_t& add(serial_t);
  unsigned int size();
  serial_t operator[](int);
};

class serial_map_t : public serial_t {
 public:
  serial_map_t();
  serial_map_t(serial_t);
  serial_t& add(const char*, serial_t);
  serial_t& add(std::string, serial_t);
};

class misc_t {
 public:
  static void rseed(int i);
  static int uniform(int i);
  static void log(int, const char *s, ...);
  static std::string read_file(const char *s);
  static std::map<int, std::string> band_colours;
  static std::map<std::string, int> formats;
};

/* ----------------------------------------------------------------------
--
-- point_t
--
---------------------------------------------------------------------- */

class point_t {
public:
  int x, y;
  point_t();
  point_t(int, int);
  int operator==(const point_t& p);
  int operator!=(const point_t& p);
  point_t& operator+=(const point_t &p);
  point_t& operator-=(const point_t &p);
  point_t operator+(const point_t &p);
  point_t operator-(const point_t &p);
  point_t& clip(int, int);
  std::string tostring();
};

class fpoint_t {
 public:
  double x, y;
  fpoint_t();
  fpoint_t(double, double);
  fpoint_t operator+(const fpoint_t&);
};

class fbox_t {
 private:
  int empty;
 public:
  double left, top, right, bottom;
  fbox_t();
  fbox_t(double, double, double, double);
  fbox_t& setunion(fbox_t);
  std::string tostring();
};

class fplace_t {
 public:
  fpoint_t center;
  fbox_t bounds;
  std::vector <fpoint_t>hull;
  fplace_t();
  fplace_t(point_t, double, double);
};

class single_t {
 public:
  char *id;
  char *file;
  fpoint_t center;
  fbox_t bounds;
  std::vector<fpoint_t> path;
};

class feature_t {
 public:
  virtual ~feature_t();
  virtual int isa(const char*);
  virtual const char* isa(void);
  virtual serial_t serialize();
  static feature_t* factory(serial_t);
};

class band_t : public feature_t {
 private:
  int type[UNSETTLED];
 public:
  band_t();
  int get(int);
  int set(int, int);
  int size();
  serial_t serialize();
  int isa(const char*);
};

class raft_t : public feature_t {
 public:
  int age;
};

class bridge_t : public feature_t {
 public:
  int direction;
};

class half_den_t : public feature_t {
 public:
  int undisturbed;
};

class hex_t {
 private:
  std::map<std::string, feature_t*> features;
 public:
  static hex_t* factory(const char*);
  static hex_t* factory(serial_t);
  point_t xy;
  std::string id;
  int owner;
  int active;
  hex_t();
  hex_t(serial_t);
  virtual ~hex_t();
  std::string getid();
  void setid(std::string);
  virtual int isa(const char*);
  virtual const char* isa(void);
  feature_t* get(const char*);
  feature_t* get(feature_t*);
  feature_t* set(feature_t* f);
  char *output_band_size(int player);
  virtual serial_t serialize();
  static std::map<std::string,std::string> type_code;
  static std::map<std::string,int> code_ref;
  virtual std::string tostring();
  void debug();
};

class water_t : public hex_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class fertile_t : public hex_t
{
private:
public:
  int age;
  fertile_t();
  int isa(const char*);
  std::string tostring();
};

class plains_t : public hex_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class temple_t : public hex_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class ruin_t : public hex_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class cursed_t : public hex_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class swamp_t : public hex_t
{
public:
  int isa(const char*);
  virtual std::string tostring();
};

class peak_t : public hex_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class volcano_t : public peak_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class whirlpool_t : public water_t
{
 public:
  int isa(const char*);
  virtual std::string tostring();
};

class scrub_t : public hex_t
{
public:
  int isa(const char*);
  virtual std::string tostring();
};

class den_t : public hex_t
{
public:
  static den_t* factory(int);
  int type;
  int pop;
  int home;
  den_t();
  den_t(int c);
  serial_t serialize();
  int isa(const char*);
  std::string tostring();
};

class red_den_t : public den_t {
 public:
  red_den_t();
  int isa(const char*);
  virtual std::string tostring();
};

class green_den_t : public den_t {
 public:
  green_den_t();
  int isa(const char*);
  virtual std::string tostring();
};

class yellow_den_t : public den_t {
 public:
  yellow_den_t();
  int isa(const char*);
  virtual std::string tostring();
};

class grey_den_t : public den_t {
 public:
  grey_den_t();
  int isa(const char*);
  virtual std::string tostring();
};

class black_den_t : public den_t {
 public:
  black_den_t();
  int isa(const char*);
  virtual std::string tostring();
};


class home_den_t : public den_t {
 public:
  home_den_t();
  int isa(const char*);
  virtual std::string tostring();
};

/* ----------------------------------------------------------------------
--
-- grid_t
--
----------------------------------------------------------------------*/


class grid_t {
private:
  hex_t **grid;
  hex_t* random();
public:
  int width, height, size;
  grid_t(int, int);
  grid_t(serial_t);
  hex_t* operator()(int, int);
  hex_t* operator()(point_t);
  hex_t* operator[](int);
  hex_t* get(point_t);
  hex_t* set(point_t, hex_t*, int);
  int adjacent(point_t p, const char*);
  void replace(point_t p, hex_t*);
  point_t move(point_t p, int);

  void place_template(class template_t *, int, point_t, int);

  fbox_t fbox(double);
  void grow();
  int lay_islands(double);
  int lay_island(point_t p, double);
  void generate_typical();
  int check();
  class output_t *output(int);
  void save(const char*);
  serial_t serialize();
  std::string tostring();
  std::vector<hex_t*>owned(int, int);
  std::vector<hex_t*>all();
};

// --------------------
// layout_t
// --------------------

class layout_hex_t {
 public:
  point_t xy;
  std::string id;
};

class layout_t {
 public:
  int size;
  int cost;
  int x_min, y_min, x_max, y_max;
  std::map<std::string, layout_hex_t*> offsets;
  layout_t();
  layout_t(int, int);
  void bounds();
  layout_hex_t*& operator[](const char*);
  grid_t* realize();
};

// --------------------
//   layouts_t
//   --------------------

class layouts_t {
 private:
  void load_file_txt(const char*);
  void load_file_js(const char*);
  void read_string_js(std::string);
 public:
  std::map<std::string, layout_t*> layouts;
  layouts_t(int, const char*);
  std::vector<std::string> keys();
  layout_t*& operator[](std::string);
  layout_t* layout(std::string);
}; 

struct rgb_t {
  double red;
  double green;
  double blue;
};

// --------------------
//   template_t
//   --------------------

class template_t {
 private:
  char** getline(FILE*);
  hex_t* hex_from_type(int);
  hex_t* hexify(layout_t*, char**);
  void build_from_layout(layout_t*);
  void load_file_txt(layouts_t*, const char*);
  void load_file_js(layouts_t*, const char*);
  void read_js(layouts_t*, std::string);
  int cost_priv;
  std::map<std::string, struct code_map*>  hex_codes;
  std::map<std::string, struct code_map*>  warrior_codes;
  std::map<std::string, struct code_map*>  pop_codes;

  std::map<int, struct code_map*>  hex_ids;
  std::map<int, struct code_map*>  warrior_ids;
  std::map<int, struct code_map*>  pop_ids;
 public:
  std::string layout_id;
  std::map<std::string, hex_t*> map;
  int size; 
  int cost();
  hex_t* active;
  hex_t* home;
  layout_t *layout;
  template_t();
  template_t(layout_t*);
  template_t(layouts_t*, int, const char*);
  int valid();
  grid_t* realize();
  void save(const char*);
  hex_t *getactive();
  hex_t *setactive(const char*);
};


/* --------------------
-- view
-------------------- */

class view_hex_t {
 public:
  point_t xy;
  fplace_t place;
  hex_t *hex;
  int marking;
  int symbol;
  int background;
  int band;
  std::string header_text, footer_text, center_text, owner;
  view_hex_t();
  view_hex_t(point_t, hex_t*);
  int mark(int, int);
};

class view_t {
 private:
  std::map<std::string, view_hex_t*> overlay;
  friend class output_t;
  friend class output_private_t;
 public:
  grid_t *grid;
  int width, height;
  view_hex_t* get(point_t);
  view_hex_t* set(point_t, view_hex_t*);
  view_t(grid_t*, int);
  void mark_hex(hex_t*, int);
  int mark(int);
  std::vector<view_hex_t*> extract(int);
};

/* --------------------
   singles_t
-------------------- */

class singles_t {
 public:
  fbox_t bounds;
  std::vector<single_t> singles;
  unsigned int size();
};

/* --------------------
   output_t
-------------------- */

class output_t {
  friend class output_private_t;
private:
  grid_t *grid;
  class game_t *game;
  class output_private_t *priv;
  int player;
  fbox_t edge;
  view_t *view;
  void place();
  void defaults();
  double width;
  double height;
public:
  std::map<std::string, rgb_t> colours;
  std::map<std::string, double> dimensions;
  std::map<std::string, std::string> params;
  output_t(grid_t*, int);
  output_t(game_t*, int);
  void set_colour(std::string, double, double, double);
  int json(const char *file);
  int svg(const char *file);
  hex_t* hit(double, double, double, double);
};

/* --------------------
-- player_t
-------------------- */

class player_t {
 public:
  player_t();
  player_t(serial_t);
  std::string tostring();
  int format;
  int index;
  std::string startup;
  std::string clan;
  std::string code;
  std::vector<std::string> messages;
  serial_t serialize();
};

/* --------------------
-- game_t
-------------------- */

typedef std::vector<player_t*> player_v;

class game_t {
 private:
  std::string id;
 public:
  std::vector<player_t*>players;
  grid_t *grid;
  unsigned int nplayers;
  game_t(int, const char*);
  game_t(player_t **);
  player_t* player(unsigned int);
  grid_t *bootstrap();
  void report();
  std::string tostring();
  serial_t serialize();
  int set_loglevel(int);
};

extern int log_level;
