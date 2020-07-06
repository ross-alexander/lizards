#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <getopt.h>
#include <glib.h>
#include <map>
#include <vector>
#include <typeinfo>

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <cairo-svg.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>

#include <libxml++/libxml++.h>


#define TEMPLATE_CODE "TEMPLATE"
#define HOME_HEX_CODE "HOME"

#define TERRAIN_TYPES 16

#define RUIN_CODE	"RUI"
#define PLAIN_CODE	"PLA"
#define SWAMP_CODE	"SWA"
#define SCRUB_CODE	"SCR"
#define FERTILE_CODE	"FER"
#define PEAK_CODE	"PEA"
#define VOLCANO_CODE	"VOL"
#define TEMPLE_CODE	"TEM"
#define CURSED_CODE	"CUR"
#define WHIRLPOOL_CODE	"WHI"
#define WATER_CODE	"WAT"

#define RED_DEN_CODE	"RED"
#define YELLOW_DEN_CODE	"YEL"
#define GREEN_DEN_CODE	"GRN"
#define GREY_DEN_CODE	"GRY"
#define BLACK_DEN_CODE	"BLK"

#define RED_LIZARD_CODE		"RED"
#define YELLOW_LIZARD_CODE	"YEL"
#define GREEN_LIZARD_CODE	"GRN"
#define GREY_LIZARD_CODE	"GRY"
#define BLACK_LIZARD_CODE	"BLK"

#define DEN_LIZARD_LEVEL 50 /* Increase in Max Pop for every adjacent Fertile */
#define DEN_MILITIA_LEVEL 0.30

enum Features {
  FEATURE_BAND,
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

enum Outputs {
  OUTPUT_BLANK,
  OUTPUT_WATER,
  OUTPUT_PLAINS,
  OUTPUT_WHIRLPOOL,
  OUTPUT_BRIDGE,
  OUTPUT_RAFT,
  OUTPUT_PLAIN,
  OUTPUT_FERTILE,
  OUTPUT_SWAMP,
  OUTPUT_SCRUB,
  OUTPUT_PEAK,
  OUTPUT_VOLCANO,
  OUTPUT_BUILDING,
  OUTPUT_HOME_DEN,
  OUTPUT_EMPTY_DEN,
  OUTPUT_RED_DEN,
  OUTPUT_GREEN_DEN,
  OUTPUT_GREY_DEN,
  OUTPUT_BLACK_DEN,
  OUTPUT_YELLOW_DEN,
  OUTPUT_TEMPLE,
  OUTPUT_CURSED,
  OUTPUT_RUIN,
  OUTPUT_BAND,
  OUTPUT_LIZARD,
  OUTPUT_LAST,
};

enum Types {
  RED,
  GREEN,
  GREY,
  BLACK,
  YELLOW,
  UNSETTLED,
};

struct svgmap_t {
  int output;
  const char *svgfile;
  RsvgHandle *handle;
};

struct svgmap_t svgmap[OUTPUT_LAST] = {
  {OUTPUT_BLANK,       	0},
  {OUTPUT_WATER,       	0},
  {OUTPUT_PLAINS,	0},
  {OUTPUT_WHIRLPOOL,   	"svg/whirlpool.svg"},
  {OUTPUT_BRIDGE,      	"svg/bridge.svg"},
  {OUTPUT_RAFT,       	"svg/raft.svg"},
  {OUTPUT_PLAIN,	"svg/plains.svg"},

  {OUTPUT_FERTILE,	"svg/fertile.svg"},
  {OUTPUT_SWAMP,       	"svg/swamp.svg"},
  {OUTPUT_SCRUB,       	"svg/scrub.svg"},
  {OUTPUT_PEAK,		"svg/peak.svg"},
  {OUTPUT_VOLCANO,     	"svg/volcano.svg"},
  {OUTPUT_BUILDING,   	"svg/building.svg"},
  {OUTPUT_HOME_DEN,	"svg/den-home.svg"},
  {OUTPUT_EMPTY_DEN,	"svg/den.svg"},
  {OUTPUT_RED_DEN,	"svg/den-red.svg"},
  {OUTPUT_GREEN_DEN,	"svg/den-green.svg"},
  {OUTPUT_GREY_DEN,	"svg/den-grey.svg"},
  {OUTPUT_BLACK_DEN,	"svg/den-black.svg"},
  {OUTPUT_YELLOW_DEN,	"svg/den-yellow.svg"},

  {OUTPUT_TEMPLE,	"svg/temple.svg"},
  {OUTPUT_CURSED,   	"svg/cursed.svg"},
  {OUTPUT_RUIN,		"svg/ruin.svg"},
  {OUTPUT_BAND,       	"svg/band.svg"},
  {OUTPUT_LIZARD,	"svg/lizard.svg"},
};

class misc_t {
 public:
  static void seed(int i) { srand(i); };
  static int uniform(int i) { return rand()%i; };
};

class fbox_t {
public:
  double left, top, right, bottom;
  fbox_t() { left = top = right = bottom = 0.0; };
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

class feature_t {
public:
  virtual ~feature_t() {};
};

class band_t : public feature_t {
public:
  int size[UNSETTLED];
  band_t() { for (int i = 0; i < UNSETTLED; i++) size[i] = 0; };
};

class hex_t {
  feature_t* features[FEATURES];
public:
  point_t xy;
  int owner;
  hex_t() { owner = 0; for (int i = 0; i < FEATURES; i++) features[i] = 0; };
  virtual ~hex_t() {};
  virtual int output() { return OUTPUT_BLANK; }
  int is(const std::type_info &t) { return ((typeid(this) == t) || (typeid(*this) == t)); } 
  feature_t* add_feature(feature_t* f)
  {
    int i = -1;
    feature_t *r = 0;
    if(typeid(f) == typeid(band_t*))
      i = FEATURE_BAND;
    if (i >= 0)
      {
	r = features[i];
	delete r;
	features[i] = f;
      }
    return f;
  }; 
};

class view_t : public hex_t
{
public:
  char *header;
  char *trailer;
  int output;
  view_t(point_t&);
  view_t(class layout_t*, char**);
};

class water_t : public hex_t
{
public:
  water_t() {};
  int output() { return OUTPUT_WATER; };
};

class plains_t : public hex_t
{
public:
  plains_t() {};
  int output() { return OUTPUT_PLAINS; };
};

class den_t : public hex_t
{
public:
  int type;
  int size;
  den_t() {};
  den_t(int c) { type = c; };
  int output() {
    switch(type)
      {
      case RED:
	return OUTPUT_RED_DEN;
	break;
      case GREEN:
	return OUTPUT_GREEN_DEN;
	break;
      case BLACK:
	return OUTPUT_BLACK_DEN;
	break;
      case YELLOW:
	return OUTPUT_YELLOW_DEN;
	break;
      case GREY:
	return OUTPUT_GREY_DEN;
	break;
      default:
	return OUTPUT_EMPTY_DEN;
	break;
      };
  };
};

class fertile_t : public hex_t
{
private:
  int age;
public:
  fertile_t() {};
  int output() { return OUTPUT_FERTILE; };
};

class grid_t {
private:
  hex_t **grid;
public:
  int width, height, size;
  grid_t(int, int);
  hex_t*&operator()(int, int);
  hex_t*&operator()(point_t);
  hex_t*&operator[](int);
  void place(class template_t *);
  fbox_t fbox(double);
  point_t move(point_t p, int);
  void grow();
  void lay_islands();
  int lay_island(point_t p, double);
  int adjacent(point_t p, const std::type_info&);
};

class layout_t {
private:
  std::map<int, point_t*> offsets;
public:
  int size;
  int cost;
  int x_min, y_min, x_max, y_max;
  layout_t(int, int);
  void bounds();
  point_t*& operator[](int);
};

class layouts_t {
private:
  std::map<std::string, layout_t*> layouts;
public:
  layouts_t(const char*);
  layout_t*& operator[](const char*);
}; 

class template_t {
private:
  char** getline(FILE*);
public:
  view_t **view;
  int size; 
  layout_t *layout;
  template_t(layouts_t&, const char *);
};

/* ----------------------------------------------------------------------
--
-- layout
--
---------------------------------------------------------------------- */

layout_t::layout_t(int j, int k)
{
  size = j;
  cost = k;
}

point_t*& layout_t::operator[](int i)
{
  assert((i >= 0) && (i < size));
  return offsets[i];
}

void layout_t::bounds()
{
  assert(size > 0);
  x_min = x_max = offsets[0]->x;
  y_min = y_max = offsets[0]->y;
  for (int i = 1; i < size; i++)
    {
      int x = offsets[i]->x;
      int y = offsets[i]->y;
      if (x < x_min) x_min = x;
      if (x > x_max) x_max = x;

      if (y < y_min) y_min = y;
      if (y > y_max) y_max = y;
    }
}

/* ----------------------------------------------------------------------
 --
 -- layouts
 --
 ---------------------------------------------------------------------- */

layouts_t::layouts_t(const char *f)
{
  FILE *stream = fopen(f, "r");
  assert(stream);
  char line[1024];

  while(fgets(line, 1024, stream))
    {
      int cost;
      int size;
      char id[1024];
      if (sscanf(line, "LAYOUT %s %d %d", (char*)&id, &size, &cost))
	{
	  layout_t* t = new layout_t(size, cost);
	  for (int j = 0; fgets(line, 1024, stream) && j < size; j++)
	    {
	      int lid, x, y;
	      if (sscanf(line, "%d %d %d", &lid, &x, &y))
		{
		  point_t* p = new point_t(x, y);
		  lid--;
		  (*t)[lid] = p;
		  //		  printf("%d - %d %d\n", lid, (*t)[lid]->x, (*t)[lid]->y);
		}
	    }
	  layouts[id] = t;
	  t->bounds();
	}
    }
  fclose(stream);
}

layout_t*& layouts_t::operator[](const char* s)
{
  return layouts[s];
}

/* ----------------------------------------------------------------------
--
-- view
--
---------------------------------------------------------------------- */
view_t::view_t(point_t& pp)
{
  xy = pp;
  output = OUTPUT_BLANK;
  header = trailer = 0;
}

view_t::view_t(layout_t* layout, char **tokens)
{
  /* --------------------
     Get id (number) of template hex
     -------------------- */

  header = trailer = 0;
  int id = strtol(tokens[0], NULL, 10) - 1;
  assert(id >= 0 && id < layout->size);
  point_t* p = (*layout)[id];
  int loop, terrain;
  xy = *p;
  assert(p);

  struct code_map {
    int id;
    const char *code;
  };

  code_map cm[] = {
    { 0, WATER_CODE },
    { 1, PLAIN_CODE },
    { 2, RUIN_CODE },
    { 3, FERTILE_CODE },
    { 4, SWAMP_CODE },
    { 5, PEAK_CODE },
    { 6, VOLCANO_CODE },
    { 7, WHIRLPOOL_CODE },
    { 8, SCRUB_CODE },
    { 9, TEMPLE_CODE },
    { 10, CURSED_CODE },
    { 11, RED_DEN_CODE },
    { 12, YELLOW_DEN_CODE },
    { 13, GREEN_DEN_CODE },
    { 14, GREY_DEN_CODE },
    { 15, BLACK_DEN_CODE },
  };

  for (loop = 0; loop < TERRAIN_TYPES; loop ++)
    if (std::string(tokens[1]) == cm[loop].code)
      {
	terrain = loop;
	break;
      }
  switch(terrain)
    {
    case 0:
      output = OUTPUT_WATER;
      break;
    case 1:
      output = OUTPUT_PLAIN;
      break;
    case 2:
      output = OUTPUT_RUIN;
      break;
    case 3:
      output = OUTPUT_FERTILE;
      break;
    case 4:
      output = OUTPUT_SWAMP;
      break;
    case 5:
      output = OUTPUT_PEAK;
      break;
    case 6:
      output = OUTPUT_VOLCANO;
      break;
    case 7:
      output = OUTPUT_WHIRLPOOL;
      break;
    case 8:
      output = OUTPUT_SCRUB;
      break;
    case 9:
      output = OUTPUT_TEMPLE;
      break;
    case 10:
      output = OUTPUT_CURSED;
      break;
    case 11:
      output = OUTPUT_RED_DEN;
      break;
    case 12:
      output = OUTPUT_YELLOW_DEN;
      break;
    case 13:
      output = OUTPUT_GREEN_DEN;
      break;
    case 14:
      output = OUTPUT_GREY_DEN;
      break;
    case 15:
      output = OUTPUT_BLACK_DEN;
      break;
    default:
      break;
    }
  trailer = new char[5];
  snprintf(trailer, 5, "T%02d", id+1);
}

/* ----------------------------------------------------------------------
--
-- template::getline
--
---------------------------------------------------------------------- */

char** template_t::getline(FILE *f)
{
  char buf[1024];
  char *s;
  unsigned int i, j;

  /* --------------------
     Get line
     -------------------- */

  if ((s = fgets(buf, 1024, f)) == NULL)
    return NULL;

  /* --------------------
     Convert to upper case
     -------------------- */
  
  for (i = 0; i < strlen(s); i++)
    s[i] = islower(s[i]) ? toupper(s[i]) : s[i];

  /* --------------------
     Count the number of tokens that will be generated.  For entries
     like BLK123 we need two tokens, not one.
     -------------------- */
  
  char *ctmp = buf;
  for (i = 0; (j = strcspn(ctmp, " \n")); i++)
    {
      int num = 0;
      int alpha = 0;
      for (unsigned int k = 0; k < j; k++)
	{
	  if (isdigit(ctmp[k]))
	    num = 1;
	  else
	    alpha = 1;
	}
      if (num && alpha)
	i++;
      ctmp += j + strspn(ctmp + j, " \n");
    }

  /* --------------------
     Allocate space for token list
     -------------------- */

  char **tokens = new char*[i+1];
  tokens[i] = 0;

  ctmp = buf;

  for (i = 0; (j = strcspn(ctmp, " \n")); )
    {
      unsigned int k;
      for (k = 0; !isdigit(ctmp[k]) && (k < j); k++)
	;

      /* --------------------
	 k = number of non digit characters at the start of the string
	 -------------------- */

      if (k > 0)
	{
	  tokens[i] = new char[k+1];
	  strncpy(tokens[i], ctmp, k);
	  tokens[i][k] = 0;
	  i++;
	}

      /* --------------------
	 if there are some digit characters at the end of the string
	 -------------------- */

      if (k != j)
	{
	  tokens[i] = new char[j-k+1];
	  strncpy(tokens[i], ctmp+k, j-k);
	  tokens[i][j-k] = 0;
	  i++;
	}
      int l = strspn(ctmp+j, " \n");
      ctmp += j + l;
    }

  return tokens;
}

/* ----------------------------------------------------------------------
--
-- template::template_t
--
---------------------------------------------------------------------- */

template_t::template_t(layouts_t& layouts, const char *path)
{
  char **tokens;
  FILE *fptr = fopen(path, "r");
  assert(fptr != NULL);
  
  if ((tokens = getline(fptr)) == NULL)
    {
      fprintf(stderr, "File %s not a template.\n", path);
      exit(EXIT_FAILURE);
    }
  
  if (strcmp(tokens[0], TEMPLATE_CODE) != 0)
    {
      fprintf(stderr, "File %s not a template.\n", path);
      exit (EXIT_FAILURE);
    }

  if ((layout = layouts[tokens[1]]) == 0)
    {
      fprintf(stderr, "Invalid Layout %s in template\n", tokens[1]);
      exit(EXIT_FAILURE);
    }

  for (int k = 0; tokens[k]; k++)
    free(tokens[k]);
  free(tokens);

  /* cost tmpl list */

  //  int player_points = l->cost;

  /* find home_hex */

  if ((tokens = getline(fptr)) == NULL)
    {
      fprintf(stderr, "Template missing HOME HEX.\n");
      exit (EXIT_FAILURE);
    }

  if (strcmp (tokens[0], HOME_HEX_CODE) != 0)
    {
      fprintf (stderr, "Template missing HOME HEX.\n");
      exit (EXIT_FAILURE);
    }
  int home_hex = strtol(tokens[1], NULL, 10);

  if (home_hex <= 0 || home_hex > layout->size)
    {
      fprintf(stderr, "FATAL ERROR: invalid HOME HEX.\n");
      exit (EXIT_FAILURE);
    }
  home_hex --; /* adjust it down for 0 based hex list */

  for (int k = 0; tokens[k]; k++)
    free(tokens[k]);
  free(tokens);

  /* build hex list, adding up hexes to 1200 points */
  /* read tmpl until eof or first line that begins with NON-DIGIT char */

  view = new view_t*[layout->size];
  size = layout->size;
  for (int k = 0; k < size; k++)
    view[k] = 0;

  while ((tokens = getline(fptr)) != NULL)
    {
      if (!isdigit(tokens[0][0])) /* non-digit char starts line! */
	break;
      int off = strtol(tokens[0], NULL, 10) - 1;
      assert(off >=0 && off < layout->size);
      /* build hex, keeping track of points */
      if ((*layout)[off])
	{
	  view_t *v = new view_t(layout, tokens);
	  assert(view[off] == 0);
	  view[off] = v;
	}
      for (int j = 0; tokens[j]; j++)
	free(tokens[j]);
      free(tokens);
    }
}

/* ----------------------------------------------------------------------
--
-- grid_t
--
---------------------------------------------------------------------- */
grid_t::grid_t(int w, int h)
{
  assert(w > 0);
  assert(h > 0);
  size = w * h;
  grid = new hex_t*[size];
  for (int i = 0; i < size; i++)
    grid[i] = 0;
  width = w;
  height = h;
}

hex_t*&grid_t::operator()(int x, int y)
{
  assert((x >= 0) && (x < width) && (y >= 0) && (y < height));
  return (*this)[y * width + x];
}

hex_t*&grid_t::operator()(point_t p)
{
  return (*this)(p.x, p.y);
}

hex_t*&grid_t::operator[](int i)
{
  assert(i >= 0 && i < width * height);
  hex_t *hex = grid[i];
  if (hex)
    hex->xy = point_t(i%width, i/width);
  return grid[i];
}

void grid_t::place(template_t *tmpl)
{
  point_t c(width / 2, height / 2);

  assert(c.x + tmpl->layout->x_min >= 0);
  assert(c.x - tmpl->layout->x_max < width);

  assert(c.y + tmpl->layout->y_min >= 0);
  assert(c.y - tmpl->layout->y_max < height);

  for (int k = 0; k < tmpl->size; k++)
    {
      view_t *t = tmpl->view[k];
      point_t p = c + t->xy;
      //      printf("%d (%d x %d) (%d x %d)\n", k, t->xy.x, t->xy.y, p.x, p.y);

      view_t *v = dynamic_cast<view_t*>((*this)(p));
      if (v) delete v;
      v = new view_t(*t);
      v->xy = p;
      (*this)(p) = v;
    }
}

fbox_t grid_t::fbox(double r)
{
  fbox_t f;
  int k;
  for (k = 0; (k < size) && ((*this)[k] == 0); k++)
    ;
  if (k == size)
    return f;

  double of = abs(grid[k]->xy.x%2)/2.0;
  f.left = f.right = grid[k]->xy.x;
  f.top = f.bottom = grid[k]->xy.y - of;

  for (; k < size; k++)
    {
      if (grid[k] == 0)
	continue;
      point_t p = (*this)[k]->xy;
      double of = abs(p.x%2)/2.0;
      if (f.left > p.x) f.left = p.x;
      if (f.right < p.x) f.right = p.x;
      if (f.bottom > p.y - of) f.bottom = p.y - of;
      if (f.top < p.y - of) f.top = p.y - of;
    }

  f.left = (1.5*f.left - 1) * r;
  f.bottom = (f.bottom - 0.5) * sqrt(3)*r;
  f.right = (1.5*f.right + 1) * r;
  f.top = (f.top + 0.5) * sqrt(3)*r;
  return f;
}

point_t grid_t::move(point_t p, int dir)
{
  point_t np = p;
  assert(dir >= NORTH && dir <= NORTHWEST);
  switch (dir)
    {
    case NORTH:
      np.y -= 1;
      break;
    case NORTHEAST:
      np.x += 1;
      if ((p.x % 2) == 1)	// x == odd
	np.y -= 1;
      break;
    case SOUTHEAST:
      np.x += 1;
      if ((p.x % 2) == 0)	// x == even
	np.y += 1;
      break;
    case SOUTH:
      np.y += 1;
      break;
    case SOUTHWEST:
      np.x -= 1;
      if ((p.x % 2) == 0)	// x == odd
	np.y += 1;
      break;
    case NORTHWEST:
      np.x -= 1;
      if ((p.x % 2) == 1)	// x == even
	np.y -= 1;
      break;
    case DIRECTIONS:
      break;
    }
  if (np.x < 0)
    np.x += width;
  else if (np.x >= width)
    np.x -= width;
  
  if (np.y < 0)
    np.y += height;
  else if (np.y >= height)
    np.y -= height;
  
  return np;
}

int grid_t::adjacent(point_t p, const std::type_info& t)
{
  int c = 0;
  for (int i = 0; i < DIRECTIONS; i++)
    {
      point_t q = move(p, i);
      if ((*this)(q)->is(t))
	c++;
    }
  return c;
}

void grid_t::grow()
{
  view_t **t = new view_t*[size];
  for (int k = 0; k < size; k++)
    t[k] = dynamic_cast<view_t*>((*this)[k]);

  for (int k = 0; k < size; k++)
    if (t[k])
      {
	for (int i = 0; i < DIRECTIONS; i++)
	  {
	    point_t p = this->move(t[k]->xy, i);
	    if ((*this)(p) == 0)
	      {
		view_t *v = new view_t(p);
		v->output = OUTPUT_WATER;
		(*this)(p) = v;
	      }
	  }
      }
}


int grid_t::lay_island(point_t p, double land_density)
{
  int work4;
  int i, start, dir;
  int number_land_hexes = 0;
  hex_t *a_hex, *c_hex = (*this)(p);

  if (misc_t::uniform(100) < (100 * land_density))
    {
      delete c_hex;
      (*this)(p) = new plains_t;
      number_land_hexes++;
    }

  for (int dir = 0; dir < DIRECTIONS; dir++)
    {
      point_t z = this->move(p, dir);
      a_hex = (*this)(z);
      number_land_hexes++;
      delete a_hex;
      (*this)(z) = new plains_t;
    }
  if ((p.x <= (width / 5) || p.x >= (width - (width / 5))) &&
      (p.y <= (height / 5) || p.y >= (height - (height / 5))))
    work4 = (int)(140 * land_density);
  else
    work4 = (int)(60 * land_density);

  if (misc_t::uniform(100) < work4)
    {
      start = misc_t::uniform(6);
      if (misc_t::uniform(100) < (100 * land_density))
	dir = 1;
      else
	dir = -1;
      for (i = 0; i < DIRECTIONS && misc_t::uniform(100) < 35; i++)
	{
	  start = (start + dir + DIRECTIONS) % DIRECTIONS;
	  point_t z = move(p, start);
	  a_hex = (*this)(z);
	  if (dynamic_cast<plains_t*>(a_hex))
	    {
	      number_land_hexes--;
	      delete a_hex;
	      (*this)(z) = new water_t();
	    }
	}
    }
  return number_land_hexes;
}

void grid_t::lay_islands()
{
  double den_p = 0.18,
    red_den_p = 0.20,
    green_den_p = 0.20,
    grey_den_p = 0.20,
    black_den_p = 0.20,
    yellow_den_p = 0.20,
    ruin_p = 0.02,
    swamp_p = 0.18,
    fertile_p = 0.04,
    peak_p = 0.015,
    volcano_p = 0.015,
    whirlpool_p = 0.01,
    scrub_p = 0.15,
    temple_p = 0.02,
    cursed_p = 0.0,
    plain_p = 0.55,
    land_density = 0.50;

  fprintf(stderr, "Layout down water...\n");
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      (*this)(x,y) = new water_t();

  fprintf(stderr, "Raising Islands I...\n");
  for (int y = 1; y < height; y += 4)
    for (int x = 1; x < width; x += 4)
      lay_island (point_t(x, y), land_density);

  int land = 0;
  for (int y = 0; y < height; y ++)
    for (int x = 0; x < width; x ++)
      if (dynamic_cast<plains_t*>((*this)(x,y)))
	  land++;

  int work = (int)(width * height * land_density - land);
  int work2 = (int)((double)(work) / (width * height - land) * 100);

  fprintf(stderr, "Raising Islands II...\n");
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      {
	hex_t *hex = (*this)(x, y);
	if (work > 0)
	  {
	    if (dynamic_cast<water_t*>(hex) && (misc_t::uniform(100) < work2))
	      {
		delete hex;
		(*this)(x, y) = new plains_t();
	      }
	  }
	else
	  {
	    if (dynamic_cast<plains_t*>(hex) && (misc_t::uniform(100) < -work2))
	      {
		delete hex;
		(*this)(x, y) = new water_t();
	      }
	  }
      }

  land = 0;
  for (int y = 0; y < height; y ++)
    for (int x = 0; x < width; x ++)
      if (dynamic_cast<plains_t*>((*this)(x,y)))
	land++;

  printf("Land = %d\n", land);

  int den_n = (int)((double) land * den_p);
  int red_den_n = (int)((double) den_n * red_den_p);
  int green_den_n = (int)((double) den_n * green_den_p);
  int grey_den_n = (int)((double) den_n * grey_den_p);
  int black_den_n = (int)((double) den_n * black_den_p);
  int yellow_den_n = (int)((double) den_n * yellow_den_p);
  int ruin_n = (int)((double) land * ruin_p);
  int swamp_n = (int)((double) land * swamp_p);
  int fertile_n = (int)((double) land * fertile_p);
  int peak_n = (int)((double) land * peak_p);
  int volcano_n = (int)((double) land * volcano_p);
  int whirlpool_n = (int)((double) land * whirlpool_p);
  int scrub_n = (int)((double) land * scrub_p);
  int temple_n = (int)((double) land * temple_p);
  int plain_n = (int)((double) land * plain_p);
  int cursed_n = (int)((double) land * cursed_p);

  while (red_den_n + green_den_n + grey_den_n + black_den_n + yellow_den_n < den_n)
    switch (misc_t::uniform (UNSETTLED))
      {
      case RED:
	if (red_den_p != 0.0)
	  red_den_n ++;
	break;
      case GREEN:
	if (green_den_p != 0.0)
	  green_den_n ++;
	break;
      case GREY:
	if (grey_den_p != 0.0)
	  grey_den_n ++;
	break;
      case BLACK:
	if (black_den_p != 0.0)
	  black_den_n ++;
	break;
      case YELLOW:
	if (yellow_den_p != 0.0)
	  yellow_den_n ++;
	break;
      }

  printf ("World has %d land hexes and %d water hexes.\n", land,
	  (width * height) - land);
  printf ("World needs %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_n, red_den_n,
	  green_den_n, grey_den_n, black_den_n, yellow_den_n, ruin_n, swamp_n,
	  fertile_n, peak_n, volcano_n, whirlpool_n, scrub_n, temple_n,
	  plain_n, cursed_n);

  int den_a = 0;
  int red_den_a = 0;
  int green_den_a = 0;
  int grey_den_a = 0;
  int black_den_a = 0;
  int yellow_den_a = 0;
  int ruin_a = 0;
  int swamp_a = 0;
  int fertile_a = 0;
  int peak_a = 0;
  int volcano_a = 0;
  int whirlpool_a = 0;
  int scrub_a = 0;
  int temple_a = 0;
  int plain_a = 0;
  int cursed_a = 0;

  printf ("Placing Dens...");
  work = 0;
  while (work < den_n)
    {
      hex_t *hex;
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height)); while (dynamic_cast<water_t*>(hex));
      point_t q = hex->xy;
      if (!(hex->is(typeid(plains_t)) &&
	    !hex->is(typeid(den_t)) &&
	    adjacent(q, typeid(plains_t)) > 0 &&
	    adjacent(q, typeid(fertile_t)) == 0 &&
	    adjacent(q, typeid(den_t)) == 0))
	  continue;
      int work3 = 1;
      int nfertile;
      den_t* den;
      do
	{
	  work2 = misc_t::uniform(UNSETTLED);
	  switch (work2)
	    {
	    case RED:
	      if (red_den_a < red_den_n)
		{
		  work3 = 0;
		  red_den_a ++;
		}
	      break;
	    case GREEN:
	      if (green_den_a < green_den_n)
		{
		  work3 = 0;
		  green_den_a ++;
		}
	      break;
	    case GREY:
	      if (grey_den_a < grey_den_n)
		{
		  grey_den_a ++;
		  work3 = 0;
		}
	      break;
	    case BLACK:
	      if (black_den_a < black_den_n)
		{
		  black_den_a ++;
		  work3 = 0;
		}
	      break;
	    case YELLOW:
	      if (yellow_den_a < yellow_den_n)
		{
		  yellow_den_a ++;
		  work3 = 0;
		}
	      break;
	    }
	  if (work3 == 0)
	    {
	      den = new den_t(work2);
	      den_a++;
	      delete hex;
	      (*this)(q) = den;

	      printf("Place new den (%d of %d) at %d,%d type %d\n", work, den_n, q.x, q.y, work2);
	    }
	}
      while (work3 != 0);

      int work4 = 0;
      do
	{
	  nfertile = 0;
	  for (int d = 0; d < DIRECTIONS; d++)
	    {
	      point_t r = move(q, d);
	      hex_t *t = (*this)(r);
	      if (misc_t::uniform(100) < 50 && work2 < 4 && t->is(typeid(plains_t)))
		{
		  delete t;
		  (*this)(r) = new fertile_t();
		  work2++;
		}
	    }
	  work4++;
	}
      while (nfertile == 0 && work4 < 20);
      if (work4 != 20)
	{
	  den->size = DEN_LIZARD_LEVEL * nfertile;
	  work3 = (int)((double)(den->size) * DEN_MILITIA_LEVEL);
	  band_t *band = new band_t();
	  band->size[den->type] = work3;
	  hex->add_feature(band);
	}
      work++;
    }

  for (int y = 0; y < height; y ++)
    for (int x = 0; x < width; x ++)
      assert((*this)(x, y));

  printf ("\n");

}

/* ----------------------------------------------------------------------
--
-- Paint
--
---------------------------------------------------------------------- */
void Extents_Path(Cairo::RefPtr<Cairo::Context> cr, grid_t *grid, double r)
{
  cr->begin_new_path();
  for (int j = 0; j < grid->size; j++)
    {
      hex_t *hex = (*grid)[j];
      if (hex == 0)
	continue;
      point_t p = hex->xy;
      double yshift = abs(p.x)%2 / 2.0;
      double cx = 1.50 * r * p.x;
      double cy = sqrt(3) * r * (p.y - yshift);

      for (int i = 0; i < 6; i++)
	{
	  double x = cos(i * 60 * M_PI / 180);
	  double y = sin(i * 60 * M_PI / 180);
	  if (i)
	    cr->line_to(cx + x * r, cy + y * r);
	  else
	    cr->move_to(cx + x * r, cy + y * r);
	}
      cr->close_path();
      cr->move_to(cx, cy);
    }
}

void Paint_Path(Cairo::RefPtr<Cairo::Context> cr, grid_t *grid, double r)
{
  for (int j = 0; j < grid->size; j++)
    {
      hex_t *hex = (*grid)[j];
      if (hex == 0)
	continue;
      view_t *v = dynamic_cast<view_t*>((*grid)[j]);
      point_t p = hex->xy;

      int output;
      char *trailer = 0;
      if (v != 0)
	{
	  output = v->output;
	  trailer = v->trailer;
	}
      else
	{
	  output = hex->output();
	}

      // printf("%d (%d x %d)\n", j, p.x, p.y);

      double yshift = abs(p.x)%2 / 2.0;
      double cx = 1.50 * r * p.x;
      double cy = sqrt(3) * r * (p.y - yshift);

      //      printf("%d : %d %d\n", j, tt[j].x, tt[j].y);

      cr->begin_new_path();
      for (int i = 0; i < 6; i++)
	{
	  double x = cos(i * 60 * M_PI / 180);
	  double y = sin(i * 60 * M_PI / 180);
	  if (i)
	    cr->line_to(cx + x * r, cy + y * r);
	  else
	    cr->move_to(cx + x * r, cy + y * r);
	}
      cr->close_path();
      switch(output)
	{
	case OUTPUT_WATER:
	case OUTPUT_WHIRLPOOL:
	case OUTPUT_BRIDGE:
	  cr->set_source_rgb(0.80, 0.94, 0.99);
	  break;
	default:
	  cr->set_source_rgb(0.45, 0.97, 0.56);
	  break;
	}
      if (output)
	cr->fill_preserve();
      cr->set_source_rgb(0.0, 0.0, 0.0);
      cr->set_line_width(0.4);
      cr->stroke();

      if (svgmap[output].handle)
	{
	  cr->save();
	  cairo_t *c = (cairo_t*)(cr->cobj());
	  cr->translate(cx, cy + 0.6*r);
	  cr->scale(r / 150, r/150);
	  cr->translate(-100, -200);
	  rsvg_handle_render_cairo(svgmap[output].handle, c);
	  cr->restore();
	}

      cr->move_to(cx, cy + sqrt(3)/2*r - 2);
      Cairo::TextExtents e;
      if (trailer)
	{
	  cr->get_text_extents(v->trailer, e);
	  cr->rel_move_to(-e.width/2, 0);
	  cr->show_text(v->trailer);
	}
    }
}

void Paint(Cairo::RefPtr<Cairo::Context> cr, grid_t *grid, double r)
{
  double x1, x2, y1, y2;

  cr->select_font_face("Bitstream Vera Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
  cr->set_font_size(12);

  Cairo::FontOptions font_options;

  font_options.set_hint_style(Cairo::HINT_STYLE_NONE);
  font_options.set_hint_metrics(Cairo::HINT_METRICS_OFF);
  font_options.set_antialias(Cairo::ANTIALIAS_GRAY);
  cr->set_font_options(font_options);

  cr->save();
  Extents_Path(cr, grid, r);
  cr->get_stroke_extents(x1, y1, x2, y2);
  cr->restore();
  cr->save();
  cr->translate(-x1 + 10, -y1 + 10);
  Paint_Path(cr, grid, r);
  cr->restore();
}

#ifdef PNG
void PNG(template_t *t, double r)
{
  fbox_t f = t->fbox(r);

      
  Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, (int)(f.right - f.left + 20.0), (f.top - f.bottom + 20.0));
  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
      
  cr->save(); // save the state of the context
  cr->set_source_rgb(0.86, 0.85, 0.47);
  cr->paint();    // fill image with the color
  cr->restore();  // color is back to black now
  Paint(cr, t, r);
  std::string filename = g_strdup_printf("image.png");
  surface->write_to_png(filename);
}
#endif

extern "C" {
  cairo_status_t stdout_write(void *closure, const unsigned char *data, unsigned int length)
  {
    fwrite(data, sizeof(char), length, stdout);
    return CAIRO_STATUS_SUCCESS;
  }
}

void SVG(grid_t *grid, double r, char *file)
{
  fbox_t f = grid->fbox(r);

  Cairo::RefPtr<Cairo::SvgSurface> surface = Cairo::SvgSurface::create(file,  (int)(f.right - f.left + 20.0), (f.top - f.bottom + 20.0));
  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
      
  cr->save(); // save the state of the context
  cr->set_source_rgb(0.86, 0.85, 0.47);
  cr->restore();  // color is back to black now
  Paint(cr, grid, r);
}

int main(int argc, char *argv[])
{
  double radius = 40;
  const char *custom = "CUST1.MAP";
  rsvg_init();

  for (int i = 0; i < OUTPUT_LAST; i++)
    {
      if (svgmap[i].svgfile)
	svgmap[i].handle = rsvg_handle_new_from_file(svgmap[i].svgfile, 0);
      else
	svgmap[i].handle = 0;
    }

  misc_t::seed(time(0));
  int ch;
  char *file = (char*)"cairo.svg";
  while ((ch = getopt(argc, argv, "d:c:f:")) > 0)
    switch (ch)
      {
      case 'c':
	custom = optarg;
	break;
      case 'd':
	chdir(optarg);
	break;
      case 'f':
	file = optarg;
	break;
      }
  layouts_t layouts("LAYOUT.TXT");
  template_t* t = new template_t(layouts, custom);
  grid_t* g = new grid_t(16, 16);
  g->lay_islands();

  //  g->place(t);
  //  g->grow();


  // PNG(&t, radius);
  SVG(g, radius, file);
  exit(0);
}
