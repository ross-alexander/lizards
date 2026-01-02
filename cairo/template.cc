#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <assert.h>
#include <syslog.h>
#include <string.h>

#include <json-c/json.h>

#include "lizards.h"

#define GROUP_SIZE 10		/* lizards are dealt with in 10 groups */

#define WATER_COST 0
#define PLAIN_COST 15
#define RUIN_COST 40
#define FERTILE_COST 25
#define SWAMP_COST 10
#define PEAK_COST 10
#define VOLCANO_COST 10
#define WHIRLPOOL_COST 5
#define SCRUB_COST 10
#define TEMPLE_COST 40
#define CURSED_COST 15

#define RED_WARRIOR_COST 10	/* all per 10 pop lizards */
#define YELLOW_WARRIOR_COST 15
#define GREEN_WARRIOR_COST 20
#define GREY_WARRIOR_COST 20
#define BLACK_WARRIOR_COST 30

#define RED_POP_COST 15 	/* all per 10 pop lizards */
#define YELLOW_POP_COST 25
#define GREEN_POP_COST 25
#define GREY_POP_COST 30
#define BLACK_POP_COST 50

#define MINIMUM_POP_PER_DEN 3	/* 3 groups of 10 pop lizards! */

#define RED_DEN_COST 65 + (MINIMUM_POP_PER_DEN * RED_POP_COST)
#define YELLOW_DEN_COST 100 + (MINIMUM_POP_PER_DEN * YELLOW_POP_COST)
#define GREEN_DEN_COST 110 + (MINIMUM_POP_PER_DEN * GREEN_POP_COST)
#define GREY_DEN_COST 95 + (MINIMUM_POP_PER_DEN * GREY_POP_COST)
#define BLACK_DEN_COST 150 + (MINIMUM_POP_PER_DEN * BLACK_POP_COST)

#define PLAYER_SETUP_POINTS 1200

struct code_map {
  int id;
  const char *code;
  const char *type_id;
  int cost;
};

static code_map hex_code_map[] = {
  { 1, WATER_CODE, WATER, WATER_COST },
  { 2, PLAINS_CODE, PLAINS, PLAIN_COST },
  { 3, FERTILE_CODE, FERTILE, FERTILE_COST },
  { 4, SWAMP_CODE, SWAMP, SWAMP_COST },
  { 5, SCRUB_CODE, SCRUB, SCRUB_COST },
  { 6, CURSED_CODE, CURSED, CURSED_COST },
  { 7, PEAK_CODE, PEAK, PEAK_COST },
  { 8, VOLCANO_CODE, VOLCANO, VOLCANO_COST },
  { 9, WHIRLPOOL_CODE, WHIRLPOOL, WHIRLPOOL_COST },
  { 10, RUIN_CODE, RUIN, RUIN_COST },
  { 11, TEMPLE_CODE, TEMPLE, TEMPLE_COST },
  { 12, RED_DEN_CODE, RED_DEN, RED_DEN_COST },
  { 13, GREEN_DEN_CODE, GREEN_DEN, GREEN_DEN_COST },
  { 14, GREY_DEN_CODE, GREY_DEN, GREY_DEN_COST },
  { 15, YELLOW_DEN_CODE, YELLOW_DEN, YELLOW_DEN_COST },
  { 16, BLACK_DEN_CODE, BLACK_DEN, BLACK_DEN_COST },
  { 17, EMPTY_DEN_CODE, EMPTY_DEN, 0 },
};

static code_map warrior_code_map[] = {
  { GREEN, GREEN_LIZARD_CODE, BAND, GREEN_WARRIOR_COST },
  { GREY, GREY_LIZARD_CODE, BAND, GREY_WARRIOR_COST },
  { BLACK, BLACK_LIZARD_CODE, BAND, BLACK_WARRIOR_COST },
  { YELLOW, YELLOW_LIZARD_CODE, BAND, YELLOW_WARRIOR_COST },
  { RED, RED_LIZARD_CODE, BAND, RED_WARRIOR_COST},
};

static code_map pop_code_map[] = {
  { GREEN, GREEN_LIZARD_CODE, GREEN_DEN, GREEN_POP_COST },
  { GREY, GREY_LIZARD_CODE, GREY_DEN, GREY_POP_COST },
  { BLACK, BLACK_LIZARD_CODE, BLACK_DEN, BLACK_POP_COST },
  { YELLOW, YELLOW_LIZARD_CODE,YELLOW_DEN, YELLOW_POP_COST },
  { RED, RED_LIZARD_CODE, RED_DEN, RED_POP_COST},
};

hex_t* template_t::hex_from_type(int terrain)
{
  hex_t *hex = 0;
  switch(terrain)
    {
    case 0:
      hex = new hex_t();
      break;
    case 1:
      hex = new water_t();
      break;
    case 2:
      hex = new plains_t();
      break;
    case 3:
      hex = new fertile_t();
      break;
    case 4:
      hex = new swamp_t();
      break;
    case 5:
      hex = new scrub_t();
      break;
    case 6:
      hex = new cursed_t();
      break;
    case 7:
      hex = new peak_t();
      break;
    case 8:
      hex = new volcano_t();
      break;
    case 9:
      hex = new whirlpool_t();
      break;
    case 10:
      hex = new ruin_t();
      break;
    case 11:
      hex = new temple_t();
      break;
    case 12:
      hex = new red_den_t();
      break;
    case 13:
      hex = new green_den_t();
      break;
    case 14:
      hex = new grey_den_t();
      break;
    case 15:
      hex = new yellow_den_t();
      break;
    case 16:
      hex = new black_den_t();
      break;
    case 17:
      hex = new den_t();
      break;
    }
  return hex;
}

/* ----------------------------------------------------------------------
--
-- hexify
--
---------------------------------------------------------------------- */

hex_t *template_t::hexify(layout_t* layout, char **tokens)
{
  /* --------------------
     Get id (string) of template hex
     -------------------- */

  int terrain;
  layout_hex_t* p = (*layout)[tokens[0]];
  assert(p);
  hex_t *hex = 0;

  if (hex_codes.count(tokens[1]))
    {
      code_map *map = hex_codes[tokens[1]];
      terrain = map->id;
      cost_priv += map->cost;
      hex = hex_from_type(terrain);
      if (hex == 0)
	misc_t::log(LOG_DEBUG, "Unknown terrain %s at %s", tokens[1], tokens[0]);
    }

  band_t *band = new band_t;

  for (int i = 2; tokens[i]; i++)
    {
      if(warrior_codes.count(tokens[i]) && tokens[i+1])
	{
	  code_map *map = warrior_codes[tokens[i]];
	  int c = strtol(tokens[i+1], 0, 10);
	  c -= c%GROUP_SIZE;
	  band->set(map->id, c);
	  cost_priv += c/GROUP_SIZE * map->cost;
	  i++;
	}
    }
  if (band->size())
    hex->set(band);
  else
    delete band;
  assert(hex);
  hex->xy = p->xy;
  hex->setid(p->id);
  return hex;
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
  const char *sepstr = " \r\n";

  /* --------------------
     Get line
     -------------------- */

  if ((s = fgets(buf, 1024, f)) == 0)
    return 0;

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
  for (i = 0; (j = strcspn(ctmp, sepstr)); i++)
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
      ctmp += j + strspn(ctmp + j, sepstr);
    }

  /* --------------------
     Allocate space for token list
     -------------------- */

  char **tokens = new char*[i+1];
  tokens[i] = 0;

  ctmp = buf;

  for (i = 0; (j = strcspn(ctmp, sepstr)); )
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
      int l = strspn(ctmp+j, sepstr);
      ctmp += j + l;
    }
  return tokens;
}

/* ----------------------------------------------------------------------
--
-- template::template_t
--
---------------------------------------------------------------------- */

/* --------------------
   All constructors call init to do the duplicate initialization.
   -------------------- */

void template_t::build_from_layout(layout_t* ll)
{
  layout = ll;
  cost_priv = layout->cost;
  active = 0;
  home = 0;
  size = layout->size;
}

/* -------------------- 
   Can create blank hex maps from layouts.  Saves duplicate code with
   the layout since the map creation and placement needs to be done
   regardless.
   -------------------- */

template_t::template_t(layout_t* layout)
{
  build_from_layout(layout);
}

/* ----------------------------------------------------------------------
--
-- load_file_txt
--
   Load file from text using the original lizards format.  This is a
   surprisingly compact format but does require fair bit of logic to
   parse.
--
---------------------------------------------------------------------- */

void template_t::load_file_txt(layouts_t* layouts, const char *path)
{
  char **tokens;
  FILE *fptr = fopen(path, "r");
  if (fptr == 0)
    misc_t::log(LOG_ERR, "Could not open %s", path);
  assert(fptr != 0);

  if ((tokens = getline(fptr)) == 0)
    {
      misc_t::log(LOG_DEBUG, "File %s is not a text template.", path);
      exit(EXIT_FAILURE);
    }
  
  if (strcmp(tokens[0], TEMPLATE_CODE) != 0)
    {
      misc_t::log(LOG_DEBUG, "File %s is not a text template.", path);
      exit (EXIT_FAILURE);
    }

  layout_id = strtol(tokens[1], 0, 10);
  if ((layout = (*layouts)[layout_id]) == 0)
    {
      misc_t::log(LOG_DEBUG, "Invalid Layout %s in template", tokens[1]);
      exit(EXIT_FAILURE);
    }

  for (int k = 0; tokens[k]; k++)
    free(tokens[k]);
  free(tokens);

  /* cost tmpl list */

  build_from_layout(layout);

  char *home_id;
  
  while ((tokens = getline(fptr)) != 0)
    {
      if (strcmp (tokens[0], HOME_HEX_CODE) == 0)
	{
	  home_id = tokens[1];
	}
      else if (isdigit(tokens[0][0])) /* non-digit char starts line! */
	{
	  /* build hex, keeping track of points */
	  char *off = tokens[0];
	  if ((*layout)[off])
	    {
	      hex_t *hex = hexify(layout, tokens);
	      assert(map.count(off) == 0);
	      map[off] = hex;
	    }
	}
      for (int j = 0; tokens[j]; j++)
	free(tokens[j]);
      free(tokens);
    }

  /* --------------------
     Set HOME DEN
     -------------------- */

  if (home_id == 0 || map.count(home_id) == 0)
    {
      misc_t::log(LOG_DEBUG, "FATAL ERROR: invalid HOME HEX.");
      exit (EXIT_FAILURE);
    }
  home = map[home_id];
}

/* ----------------------------------------------------------------------
--
-- load_file_js
--
---------------------------------------------------------------------- */

void template_t::read_js(layouts_t *layouts, std::string s)
{
  json_object *js = json_tokener_parse(s.c_str());
  assert(js != 0);
  assert(json_object_is_type(js, json_type_object));

  json_object *t;
  assert(json_object_object_get_ex(js, "layout", &t));

  layout_id = json_object_get_string(t);
  layout_t *layout = (*layouts)[layout_id];
  assert(layout);

  /* --------------------
     Run init now to create the right size arrays etc
     -------------------- */

  build_from_layout(layout);
  if (json_object_object_get_ex(js, "hexes", &t))
    {
      assert(json_object_is_type(t, json_type_array));
      int len = json_object_array_length(t);
      for (int i = 0; i < len; i++)
	{
	  json_object *h = json_object_array_get_idx(t, i);
	  json_object *tt;
	  assert(json_object_is_type(h, json_type_object));
	  json_object_object_get_ex(h, "id", &tt);
	  const char *id = json_object_get_string(tt);

	  assert(layout->offsets.count(id) > 0);
	  
	  int hex_type = 0;
	  if (json_object_object_get_ex(h, "type", &tt))
	    {
	      const char *type = json_object_get_string(tt);
	      if (hex_codes.count(type))
		{
		  code_map *c = hex_codes[type];
		  hex_type = c->id;
		  cost_priv += c->cost;
		  misc_t::log(LOG_DEBUG, "Hex %s %s", id, type);
		}
	    }
	  hex_t* hex = hex_from_type(hex_type);
	  assert(hex);
	  layout_hex_t* p = (*layout)[id];
	  assert(p);
	  hex->xy = p->xy;
	  hex->setid(p->id);
	  map[hex->getid()] = hex;
	  if (json_object_object_get_ex(h, "title", &tt))
	    {
	      const char *title = json_object_get_string(tt);
	      hex->setid(title);
	    }
	  if (json_object_object_get_ex(h, "band", &tt))
	    {
	      band_t *band = new band_t();
	      json_object_object_foreach(tt, key, val)
		{
		  if (warrior_codes.count(key))
		    {
		      int c = json_object_get_int(val);
		      c -= c%GROUP_SIZE;
		      if (c > 0)
			{
			  band->set(warrior_codes[key]->id, c);
			  cost_priv += c/GROUP_SIZE * warrior_codes[key]->cost;
			}
		    }
		}
	      if (band->size() > 0) hex->set(band);
	    }
	  if(den_t *den = dynamic_cast<den_t*>(hex))
	    {
	      if (json_object_object_get_ex(h, "pop", &tt))
		{
		  int c = json_object_get_int(tt);
		  c -= c%GROUP_SIZE;
		  den->pop = c;
		}
	      else
		{
		  den->pop = 30;
		}
	    }
	}
    }

    if (json_object_object_get_ex(js, "home", &t))
    {
      const char *home_id = json_object_get_string(t);
      if (map.count(home_id) == 0)
	{
	  misc_t::log(LOG_DEBUG, "FATAL ERROR: invalid HOME HEX.");
	  exit (EXIT_FAILURE);
	}
      misc_t::log(LOG_NOTICE, "Home Den is hex %s", home_id);
      home = map[home_id];
      den_t *home_den = dynamic_cast<den_t*>(home);
      if (home_den)
	home_den->home = 1;
    }

  if (json_object_object_get_ex(js, "active", &t) && map.count(json_object_get_string(t)))
      active = map[json_object_get_string(t)];
  else
    active = 0;
}

void template_t::load_file_js(layouts_t *layouts, const char *path)
{
  misc_t::log(LOG_DEBUG, "Loading template files %s", path);
  std::string s = misc_t::read_file(path);
  assert(s.length());
  read_js(layouts, s);
}


/* ----------------------------------------------------------------------
--
-- realize
--
---------------------------------------------------------------------- */

grid_t* template_t::realize()
{
  int x_min = layout->x_min;
  int y_min = layout->y_min;

  int x_max = layout->x_max;
  int y_max = layout->y_max;

  x_min -= abs(x_min%2);
  y_min -= abs(y_min%2);
  x_max += abs(x_max%2);
  y_max += abs(y_max%2);

  int width = x_max - x_min + 1;
  int height = y_max - y_min + 1;

  /* --------------------
     Fill-in any missing hexes
     -------------------- */
  
  for(auto const &i : layout->offsets)
    {
      layout_hex_t *lh = i.second;
      misc_t::log(LOG_DEBUG, "template::realize(%s)", lh->id.c_str());
      if(map.count(lh->id) == 0)
	{
	  hex_t *hex = new hex_t();
	  hex->xy = lh->xy;
	  hex->setid(lh->id);
	  map[lh->id] = hex;
	  misc_t::log(LOG_DEBUG, "template::realize(%d, %d)", lh->xy.x, lh->xy.y);
	}
    }
  misc_t::log(LOG_DEBUG, "Realize template with width = %d height = %d size %u", width, height, map.size());

  grid_t *grid = new grid_t(width, height);
  point_t o(-x_min, -y_min);
  grid->place_template(this, 0, o, 0);
  return grid;
}

/* ----------------------------------------------------------------------
--
-- json
--
---------------------------------------------------------------------- */

void template_t::save(const char *file)
{
  json_object* j_tmpl = json_object_new_object();

  json_object_object_add(j_tmpl, "layout", json_object_new_string(layout_id.c_str()));
  if (cost_priv) json_object_object_add(j_tmpl, "cost", json_object_new_int(cost_priv));
  if (home) json_object_object_add(j_tmpl, "home", json_object_new_string(home->getid().c_str()));
  if (active) json_object_object_add(j_tmpl, "active", json_object_new_string(active->getid().c_str()));

  json_object* j_hexes = json_object_new_array();
  json_object_object_add(j_tmpl, "hexes", j_hexes);

  for (auto const &i : map)
    {
      hex_t *h = i.second;
      json_object *j_hex = json_object_new_object();
      json_object_array_add(j_hexes, j_hex);
      json_object_object_add(j_hex, "id", json_object_new_string(h->getid().c_str()));

#ifdef FALSE
      if (den_t* den = dynamic_cast<den_t*>(h))
	{
	  if (pop_ids.count(den->type))
	    {
	      code_map *map = pop_ids[den->type];
	      json_object_object_add(j_hex, "type", json_object_new_string(map->code));
	    }
	}
      else
#endif
	if (hex_codes.count(typeid(*h).name()))
	{
	  code_map *map = hex_codes[typeid(*h).name()];
	  json_object_object_add(j_hex, "type", json_object_new_string(map->code));
	}
    }
  json_object_to_file_ext((char*)file, j_tmpl, JSON_C_TO_STRING_PLAIN);
}

/* ----------------------------------------------------------------------
--
-- find
--
---------------------------------------------------------------------- */

hex_t* template_t::setactive(const char *id)
{
  hex_t *a = active;
  if (a) a->active = 0;

  active = (map.count(id)) ? map[id] : 0;
  if (active)
    {
      active->active = 1;
      misc_t::log(LOG_DEBUG, "Hex %s set active %d", active->getid().c_str(), active->active);
    }
  return a;
}

hex_t* template_t::getactive()
{
  return active;
}

int template_t::cost()
{
  return cost_priv;
}

/* ----------------------------------------------------------------------
--
-- constructors
--
---------------------------------------------------------------------- */

template_t::template_t(layouts_t* layouts, int format, const char* path) : template_t()
{
  if (format == FORMAT_FILE_TXT)
    load_file_txt(layouts, path);
  else if (format == FORMAT_FILE_JS)
    load_file_js(layouts, path);
  else if (format == FORMAT_STRING_JS)
    read_js(layouts, std::string(path));
}

/* ----------------------------------------------------------------------
--
-- template_t()
--
---------------------------------------------------------------------- */

 template_t::template_t()
 {
   for (unsigned int k = 0; k < sizeof(hex_code_map)/sizeof(code_map); k++)
    hex_codes[hex_code_map[k].code] = &hex_code_map[k];
  
  for (unsigned int k = 0; k < sizeof(warrior_code_map)/sizeof(code_map); k++)
    warrior_codes[warrior_code_map[k].code] = &warrior_code_map[k];

  for (unsigned int k = 0; k < sizeof(pop_code_map)/sizeof(code_map); k++)
    pop_codes[pop_code_map[k].code] = &pop_code_map[k];
 }
