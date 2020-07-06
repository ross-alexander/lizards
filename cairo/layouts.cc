#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <assert.h>

#include <cairomm/context.h>
#include <librsvg/rsvg.h>

#include <json-c/json.h>
#include <sys/syslog.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
 --
 -- layouts
 --
 ---------------------------------------------------------------------- */

void layouts_t::load_file_txt(const char* f)
{
  FILE *stream = fopen(f, "r");
  assert(stream);
  char line[1024];

  while(fgets(line, 1024, stream))
    {
      int cost;
      int size;
      char id[10];
      if (sscanf(line, "LAYOUT %10s %d %d", id, &size, &cost))
	{
	  layout_t* t = new layout_t(size, cost);
	  for (int j = 0; fgets(line, 1024, stream) && j < size; j++)
	    {
	      char id[4];
	      int x, y;
	      if (sscanf(line, "%s %d %d", (char*)&id, &x, &y))
		{
		  point_t p = point_t(x, y);
		  layout_hex_t* lh = new layout_hex_t;
		  lh->id = id;
		  lh->xy = p;
		  (*t)[id] = lh;
		}
	    }
	  layouts[id] = t;
	  t->bounds();
	}
    }
  fclose(stream);
}

/* ----------------------------------------------------------------------
--
-- load_file_js
--
---------------------------------------------------------------------- */

void layouts_t::load_file_js(const char* f)
{
  std::string s = misc_t::read_file(f);
  if (s.length() == 0)
    {
      misc_t::log(LOG_ERR, "layouts_t::load_file_js(%s) failed", f);      
      return;
    }
  read_string_js(s);
}

void layouts_t::read_string_js(std::string s)
{
  enum json_tokener_error jerr;
  json_tokener *tok = json_tokener_new();

  json_object *js = json_tokener_parse_ex(tok, s.c_str(), s.length());

  assert((jerr = json_tokener_get_error(tok)) == json_tokener_success);
  assert(js != 0);
  assert(json_object_is_type(js, json_type_array));
  
  unsigned int n_layouts = json_object_array_length(js);

  misc_t::log(LOG_DEBUG, "LAYOUT with %d layouts", n_layouts);

  for (unsigned int i = 0; i < n_layouts; i++)
    {
      json_object *l = json_object_array_get_idx(js, i);
      assert(json_object_is_type(l, json_type_object));

      json_object *id_obj, *cost_obj;
      json_object_object_get_ex(l, "id", &id_obj);
      json_object_object_get_ex(l, "cost", &cost_obj);
      
      const char* id =  json_object_get_string(id_obj);
      int cost = json_object_get_int(cost_obj);
      
      json_object* hexes;
      json_object_object_get_ex(l, "hexes", &hexes);
      assert(json_object_is_type(hexes, json_type_array));
      int n_hexes = json_object_array_length(hexes);
      assert(n_hexes > 0);
 
      misc_t::log(LOG_DEBUG, "LAYOUT id=%s size=%d cost=%d", id, n_hexes, cost);

      layout_t* t = new layout_t(n_hexes, cost);
      for (int j = 0; j < n_hexes; j++)
	{
	  json_object *hexline = json_object_array_get_idx(hexes, j);
	  const char* hex_id = json_object_get_string(json_object_array_get_idx(hexline, 0));
	  int hex_x = json_object_get_int(json_object_array_get_idx(hexline, 1));
	  int hex_y = json_object_get_int(json_object_array_get_idx(hexline, 2));

	  misc_t::log(LOG_DEBUG, "%2s%3d%3d", hex_id, hex_x, hex_y);
	  point_t p = point_t(hex_x, hex_y);
	  layout_hex_t *lh = new layout_hex_t;
	  lh->id = hex_id;
	  lh->xy = p;
	  (*t)[hex_id] = lh;
	}
      layouts[id] = t;
      t->bounds();
    }
}

/* ----------------------------------------------------------------------
--
-- layouts_t::layouts_t
--
---------------------------------------------------------------------- */

layouts_t::layouts_t(int format, const char *f)
{
  if (format == FORMAT_FILE_TXT)
    load_file_txt(f);
  else if (format == FORMAT_FILE_JS)
    load_file_js(f);
  else if (format == FORMAT_STRING_JS)
    read_string_js(std::string(f));
}

layout_t* layouts_t::layout(std::string id)
{
  return layouts[id];
}

layout_t*& layouts_t::operator[](std::string id)
{
  return layouts[id];
}
