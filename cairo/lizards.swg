%module lizards

%include "std_map.i"
%include "std_vector.i"
%include "std_string.i"
%include "typemaps.i"

%{
#include <string>
#include <map>
#include <vector>
#include <typeinfo>
#include <cairomm/context.h>
#include <librsvg/rsvg.h>
#include <fmt/format.h>
#include <fmt/printf.h>

#include "lizards.h"
%}

%constant char* BAND;
%constant char* PEAK;
%constant char* VOLCANO;
%constant char* WATER;
%constant char* SCRUB;
%constant char* PLAINS;
%constant char* FERTILE;
%constant char* TEMPLE;
%constant char* RUIN;
%constant char* SWAMP;
%constant char* DEN;
%constant char* RED_DEN;
%constant char* BLACK_DEN;
%constant char* GREEN_DEN;
%constant char* YELLOW_DEN;
%constant char* GREY_DEN;
%constant char* HOME_DEN;

enum {
  RED,
  GREEN,
  GREY,
  BLACK,
  YELLOW,
  UNSETTLED,
};

enum {
  FORMAT_FILE_TXT = 0,
  FORMAT_FILE_XML,
  FORMAT_FILE_JS,
  FORMAT_STRING_TXT,
  FORMAT_STRING_XML,
  FORMAT_STRING_JS,
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

enum Owned {
  OWNED_ALL,
  OWNED_BAND_ONLY,
  OWNED_SPY_ONLY,
  OWNED_BAND_SPY
};

%extend point_t {
  std::string __str__()
    {
      return fmt::sprintf("point_t(%02d, %02d)", self->x, self->y);
    }
};

#ifdef SWIGPYTHON
%typemap(in) const std::type_info&
{
  void *v;

  /*
  printf("++ %s\n", PyString_AsString(PyObject_Unicode(PyObject_Repr($input))));

  PyObject *o;
  if (PyObject_HasAttrString($input, "__class__"))
    {
      o = PyObject_GetAttrString($input, "__class__");
    }
  else
    o = PyObject_Str($input);
  printf("++ %s\n", PyString_AsString(PyObject_Unicode(o)));
  */

  if ((SWIG_NewPointerObj($input, (void **) &v, $descriptor(band_t*), SWIG_POINTER_EXCEPTION)) == 0)
    $1 = (std::type_info*)&typeid(band_t);
  else
    $1 = (std::type_info*)&typeid($input);
}
%typemap(out) feature_t*
{
  if ($1)
    {
      if (dynamic_cast<band_t*>($1))
	$result = SWIG_NewPointerObj(SWIG_as_voidptr($1), SWIG_TypeQuery("band_t*"), 0 |  0 );
      else
	$result = SWIG_NewPointerObj(SWIG_as_voidptr($1), SWIG_TypeQuery("feature_t*"), 0 |  0 );
    }
  else
    $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), SWIG_TypeQuery("feature_t*"), 0 |  0 );
}

%typemap(out) hex_t*
{
   if ($1)
    {
      if (dynamic_cast<fertile_t*>($1)) $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), SWIG_TypeQuery("fertile_t*"), 0 |  0 );
      else if (dynamic_cast<den_t*>($1)) $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), SWIG_TypeQuery("den_t*"), 0 |  0 );
      else $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), SWIG_TypeQuery("hex_t*"), 0 |  0 );
    }
}
#endif

#ifdef SWIGLUA

%typemap(out) feature_t*
{
  if ($1)
    {
      swig_type_info *info;
      if (dynamic_cast<band_t*>($1)) info = SWIG_TypeQuery("band_t*");
      else info = SWIG_TypeQuery("feature_t*");
      assert(info);
      SWIG_NewPointerObj(L, $1, info, 0|0);
      SWIG_arg++;
    }
}

%typemap(out) hex_t*
{
  if ($1)
    {
      swig_type_info *info;
      if (dynamic_cast<fertile_t*>($1)) info = SWIG_TypeQuery("fertile_t*");
      else if (dynamic_cast<plains_t*>($1)) info = SWIG_TypeQuery("plains_t*");
      else if (dynamic_cast<temple_t*>($1)) info = SWIG_TypeQuery("temple_t*");
      else if (dynamic_cast<ruin_t*>($1)) info = SWIG_TypeQuery("ruin_t*");
      else if (dynamic_cast<swamp_t*>($1)) info = SWIG_TypeQuery("swamp_t*");
      else if (dynamic_cast<scrub_t*>($1)) info = SWIG_TypeQuery("scrub_t*");
      else if (dynamic_cast<water_t*>($1)) info = SWIG_TypeQuery("water_t*");
      else if (dynamic_cast<peak_t*>($1)) info = SWIG_TypeQuery("peak_t*");
      else if (dynamic_cast<volcano_t*>($1)) info = SWIG_TypeQuery("volcano_t*");
      else if (dynamic_cast<whirlpool_t*>($1)) info = SWIG_TypeQuery("whirlpool_t*");
      else if (dynamic_cast<cursed_t*>($1)) info = SWIG_TypeQuery("cursed_t*");
      else if (dynamic_cast<red_den_t*>($1)) info = SWIG_TypeQuery("red_den_t*");
      else if (dynamic_cast<black_den_t*>($1)) info = SWIG_TypeQuery("black_den_t*");
      else if (dynamic_cast<yellow_den_t*>($1)) info = SWIG_TypeQuery("yellow_den_t*");
      else if (dynamic_cast<green_den_t*>($1)) info = SWIG_TypeQuery("green_den_t*");
      else if (dynamic_cast<grey_den_t*>($1)) info = SWIG_TypeQuery("grey_den_t*");
      else if (dynamic_cast<den_t*>($1)) info = SWIG_TypeQuery("den_t*");
      else info = SWIG_TypeQuery("hex_t*");
      assert(info);
      SWIG_NewPointerObj(L, $1, info, 0|0);
      SWIG_arg++;
    }
}
#endif


class point_t {
public:
  int x,y;
  point_t();
  point_t(int x_p, int y_p);
  int operator==(const point_t& p);
  int operator!=(const point_t& p);
  point_t& operator+=(const point_t &p);
  point_t operator+(const point_t &p);
  point_t operator-(const point_t &p);
  point_t& clip(int, int);
  std::string tostring();
};


class fpoint_t {
 public:
  double x,y;
  fpoint_t();
  fpoint_t(double x_p, double y_p);
};

class fbox_t {
 public:
  double left, top, right, bottom;
  std::string tostring();
  fbox_t& setunion(fbox_t&);
};

class fplace_t {
 public:
  fpoint_t center;
  fbox_t bounds;
  std::vector<fpoint_t> hull;
  fplace_t(point_t, double, double);
};

class feature_t {
public:
  int isa(const char*);
  const char* isa(void);
};

class band_t : public feature_t {
  int type[UNSETTLED];
public:
  int set(int, int);
  int get(int);
  int size();
};

class raft_t : public feature_t {
public:
  int age;
};

class hex_t {
public:
  int owner;
//  int active;
  std::string id;
  std::string getid();
  void setid(std::string);
  point_t xy;
  bool isa(const char*);
  const char *isa(void);
  feature_t* get(const char*);
  feature_t* set(feature_t*);
  std::string tostring();
};

class water_t : public hex_t {};
class whirlpool_t : public water_t {};
class plains_t : public hex_t {};
class fertile_t : public hex_t {};
class ruin_t : public hex_t {};
class swamp_t : public hex_t {};
class temple_t : public hex_t {};
class scrub_t : public hex_t {};
class peak_t : public hex_t {};
class volcano_t : public peak_t {};
class cursed_t : public hex_t {};
class den_t : public hex_t {
public:
  int pop;
  int home;
};
class red_den_t : public den_t {};
class black_den_t : public den_t {};
class yellow_den_t : public den_t {};
class green_den_t : public den_t {};
class grey_den_t : public den_t {};

class layout_t {
public:
  grid_t *realize();
};

%template(vectors) std::vector<std::string>;

class layouts_t {
public:
  std::map<std::string, layout_t*> layouts;
  layouts_t(int, char*);
  layout_t* layout(std::string);
  std::vector<std::string> keys();
};

class template_t {
public:
  template_t(layouts_t*, int, char*);
  template_t(layout_t*);
  int cost();
  grid_t *realize();
  void save(const char*);
  hex_t *getactive();
  hex_t *setactive(const char*);
};

class grid_t {
public:
  grid_t(int, int);
  int lay_islands(double);
  void generate_typical();
  hex_t *operator()(point_t);
  int width;
  int height;
  hex_t* set(point_t, hex_t*, int);
  point_t move(point_t, int);
  void replace(point_t, hex_t*);
  void place_template(template_t*, int, point_t, int);
  output_t* output(int);
  std::string tostring();
  std::vector<hex_t*> owned(int, int);
  serial_t serialize();
};

%template(layout_m) std::map<std::string, layout_t*>;
%template(hex_pv) std::vector<hex_t*>;
%template(fpoint_v) std::vector<fpoint_t>;
%template(point_v) std::vector<point_t>;
%template(single_v) std::vector<single_t>;
%template(player_v) std::vector<player_t*>;
%template(double_m) std::map<std::string, double>;
%template(rgb_m) std::map<std::string, rgb_t>;


%extend std::map<std::string, double> {
  double __setitem(string s, double d) { (*self)[s] = d; return d; }
  double __getitem(string s) { return (*self)[s]; }
}

class single_t {
 public:
  char *id;
  char *file;
  fpoint_t center;
  fbox_t bounds;
  std::vector<fpoint_t> path;
};

class singles_t {
 public:
  fbox_t bounds;
  std::vector<single_t> singles;
  unsigned int size();
};

struct rgb_t {
  double red;
  double green;
  double blue;
};

class output_t {
public:
  std::map<std::string, rgb_t> colours;
  std::map<std::string, double> dimensions;
  output_t(game_t*, int);
  output_t(grid_t*, int);
  void set_colour(std::string, double, double, double);
  int json(const char*);
  int svg(const char*);
};

class player_t {
public:
  std::string startup, clan, code;
};


class game_t {
public:
  grid_t *grid;
  int nplayers;
  std::vector<player_t*> players;
  void report();
  std::string tostring();
  game_t(int, const char*);
  player_t* player(int i);
  grid_t *bootstrap();
  int set_loglevel(int);
};

class view_t {
public:
  view_t(grid_t*, int);
};

class serial_t {
 private:
  int state;
  char *path;
 public:
  serial_t();
  int to_file(const char*);
  static serial_t from_file(const char*);
};
