/* ----------------------------------------------------------------------
--
-- output.cc
--
---------------------------------------------------------------------- */

#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <assert.h>
#include <sys/syslog.h>

#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <cairomm/scaledfont.h>
#include <cairomm/fontface.h>
#include <librsvg/rsvg.h>

#include <json-c/json.h>

#include <fmt/format.h>
#include <fmt/printf.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- structures
--
---------------------------------------------------------------------- */

enum Background {
  BG_BLANK,
  BG_WATER,
  BG_LAND
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
  OUTPUT_HALF_DEN,
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

struct svgmap_t {
  int output;
  const char *type;
  const char *svgfile;
  RsvgHandle *handle;
};

class output_private_t {
public:
  output_t *output;
  void path_extents(Cairo::RefPtr<Cairo::Context> cr);
  void path_paint_hex(Cairo::RefPtr<Cairo::Context> cr, view_hex_t*);
  void path_paint(Cairo::RefPtr<Cairo::Context> cr);
  fbox_t paint(Cairo::RefPtr<Cairo::Context> cr);

  std::vector<view_hex_t*> hexes;
  void view(view_hex_t*);
  
  output_private_t(output_t*);
  static svgmap_t svgmap[OUTPUT_LAST];
  std::map<int, svgmap_t*> id2sym;
  std::map<std::string, svgmap_t*> type2sym;
  hex_t *hit(double, double, double, double);
};

#define min(a, b) (a < b ? a : b)

svgmap_t output_private_t::svgmap[OUTPUT_LAST] = {
  {OUTPUT_BLANK,     	BLANK,		0},
  {OUTPUT_WATER,     	WATER,		0},
  {OUTPUT_WHIRLPOOL,   	WHIRLPOOL,	"svg/whirlpool.svg"},
  {OUTPUT_PLAINS, 	PLAINS,		"svg/plains.svg"},
  {OUTPUT_FERTILE,	FERTILE,       	"svg/fertile.svg"},
  {OUTPUT_SWAMP,       	SWAMP,		"svg/swamp.svg"},
  {OUTPUT_SCRUB,       	SCRUB,		"svg/scrub.svg"},
  {OUTPUT_PEAK,		PEAK,		"svg/peak.svg"},
  {OUTPUT_VOLCANO,     	VOLCANO,	"svg/volcano.svg"},
  {OUTPUT_HALF_DEN,   	HALF_DEN,	"svg/half_den.svg"},
  {OUTPUT_EMPTY_DEN,	EMPTY_DEN,	"svg/den.svg"},
  {OUTPUT_RED_DEN,	RED_DEN,	"svg/den-red.svg"},
  {OUTPUT_GREEN_DEN,	GREEN_DEN,	"svg/den-green.svg"},
  {OUTPUT_GREY_DEN,	GREY_DEN,	"svg/den-grey.svg"},
  {OUTPUT_BLACK_DEN,	BLACK_DEN,	"svg/den-black.svg"},
  {OUTPUT_YELLOW_DEN,	YELLOW_DEN,	"svg/den-yellow.svg"},
  {OUTPUT_HOME_DEN,	0,		"svg/den-home.svg"},
  {OUTPUT_TEMPLE,	TEMPLE,		"svg/temple.svg"},
  {OUTPUT_CURSED,   	CURSED,		"svg/cursed.svg"},
  {OUTPUT_RUIN,		RUIN,		"svg/ruin.svg"},
  {OUTPUT_BAND,       	BAND,		"svg/band.svg"},
  {OUTPUT_BRIDGE,      	BRIDGE,		"svg/bridge.svg"},
  {OUTPUT_RAFT,       	RAFT,		"svg/raft.svg"},
  {OUTPUT_LIZARD,	0,		"svg/lizard.svg"},
};


/* ----------------------------------------------------------------------
--
-- place
--
-- Places a hex onto the cartesian plane, applying the vertical shift
-- and scaling the final output
--

      -0.5r,-sqrt(3)/2*r    
            +------+------+           
           / \     |     / \
          /   \    |    /   \   
         /    sqrt(3)/2*r    \  
        /       \  |  /       \ 
       /         \ | /         \
      /           \|/           \
      +---- r -----+-------------+
                 0,0            +r,0

-- System uses raster (top = 0) coordinates
--
---------------------------------------------------------------------- */



/* ----------------------------------------------------------------------
--
-- path_extents
--
---------------------------------------------------------------------- */

void output_private_t::path_extents(Cairo::RefPtr<Cairo::Context> cr)
{
  cr->begin_new_path();
  for (auto hex : hexes)
    {
      if (hex == nullptr)
	continue;

      cr->begin_new_sub_path();
      cr->move_to(hex->place.hull[0].x, hex->place.hull[0].y);
      for (int i = 1; i < 6; i++)
	cr->line_to(hex->place.hull[i].x, hex->place.hull[i].y);
      cr->close_path();
    }
}

/* ----------------------------------------------------------------------
--
-- paint
--
---------------------------------------------------------------------- */

fbox_t output_private_t::paint(Cairo::RefPtr<Cairo::Context> cr)
{
  fbox_t fb;

  cr->save();
  path_extents(cr);
  cr->set_line_width(0.4);
  cr->get_stroke_extents(fb.left, fb.top, fb.right, fb.bottom);
  cr->restore();

  misc_t::log(LOG_INFO, "stroke extents left = %f top = %f", fb.left, fb.top);

  cr->save();
  cr->set_source_rgb(1, 1, 1);
  cr->translate(-fb.left, -fb.top);
  path_paint(cr);
  cr->restore();
  return fb;
}

/* ----------------------------------------------------------------------
--
-- path_paint
--
---------------------------------------------------------------------- */

void output_private_t::path_paint(Cairo::RefPtr<Cairo::Context> cr)
{
  for (auto hex : hexes)
    {
      path_paint_hex(cr, hex);
    }
  for (auto hex : hexes)
    {
      cr->begin_new_path();
      cr->move_to(hex->place.hull[0].x, hex->place.hull[0].y);
      for (int i = 1; i < 6; i++)
	cr->line_to(hex->place.hull[i].x, hex->place.hull[i].y);
      cr->close_path();

      if (hex->hex->active)
	{
	  fprintf(stderr, "%s active\n", hex->hex->getid().c_str());
	  cr->set_source_rgb(1.0, 1.0, 0.0);
	  cr->set_line_width(2.0);
	  cr->stroke();
	}
      if ((output->player > 0) && (output->player == hex->hex->owner))
	{
	  cr->set_line_width(1.0);
	  cr->set_source_rgb(1.0, 0.0, 0.0);
	  cr->stroke();
	}
    }
}


/* ----------------------------------------------------------------------
--
-- path_paint_hex
--
---------------------------------------------------------------------- */

void output_private_t::path_paint_hex(Cairo::RefPtr<Cairo::Context> cr, view_hex_t* ovh)
{
  hex_t *hex = ovh->hex;
  if (hex == 0)
    return;
  
  int symbol;
  const char *trailer = nullptr;
  const char *header = nullptr;
  
  symbol = ovh->symbol;
  header = ovh->header_text.c_str();
  trailer = ovh->footer_text.c_str();

  misc_t::log(LOG_DEBUG, "header=%s trailer=%s symbol=%d", header, trailer, symbol);
  
/* --------------------
   Create path from hull
   -------------------- */

  cr->begin_new_path();
  cr->move_to(ovh->place.hull[0].x, ovh->place.hull[0].y);
  for (int i = 1; i < 6; i++)
    cr->line_to(ovh->place.hull[i].x, ovh->place.hull[i].y);
  cr->close_path();

  /* --------------------
     Set fill colour
     -------------------- */
  
  switch(ovh->background)
    {
    case BG_WATER:
      cr->set_source_rgb(output->water_colour.red, output->water_colour.green, output->water_colour.blue);
	break;
    case BG_BLANK:
      cr->set_source_rgb(0.8, 0.8, 0.8);
      break;
    case BG_LAND:
      cr->set_source_rgb(output->land_colour.red, output->land_colour.green, output->land_colour.blue);
      break;
    }
  cr->fill_preserve();

  /* --------------------
     Paint hex border
     -------------------- */
  
  cr->set_line_width(output->border_width);
  cr->set_source_rgb(0.0, 0.0, 0.0);
  if (ovh->hex->active)
    {
      fprintf(stderr, "%s active\n", ovh->hex->getid().c_str());
      cr->set_line_width(2.0);
      cr->set_source_rgb(1.0, 1.0, 0.0);
    }
  cr->stroke();
  
  /* --------------------
     Place icon (base size is 200x200) scaled by 0.20 to 40x40 so image is places at cx-20, cy-15 to cx+20, cy+25
     -------------------- */
  
  double cx = ovh->place.center.x;
  double cy = ovh->place.center.y;
  
  if (id2sym[symbol] && id2sym[symbol]->handle)
    {
      cr->save();
      cairo_t *c = (cairo_t*)(cr->cobj());
      RsvgRectangle viewport = {.x = cx - 20.0, .y = cy - 15.0, .width = 40, .height = 40};
      //      cr->translate(cx - 20.0, cy - 15.0);
      //      cr->scale(0.20, 0.20);
      //      rsvg_handle_render_cairo(id2sym[symbol]->handle, c);
      rsvg_handle_render_document(id2sym[symbol]->handle, c, &viewport, 0);
      cr->restore();
    }

  /* --------------------
     Trailer
     -------------------- */
  
  Cairo::TextExtents e;
  if (trailer)
    {
      cr->get_text_extents(trailer, e);
      cr->move_to(cx - e.width/2, cy + sqrt(3)/2*output->radius - 2);
      cr->set_source_rgb(0.0, 0.0, 0.0);
      cr->show_text(trailer);
    }

  /* --------------------
     Header
     -------------------- */
  
  cr->save();
  cr->set_source_rgb(1.0, 0.0, 0.0);
  if (header)
    {
      cr->get_text_extents(header, e);
      cr->move_to(cx - e.width/2, cy - sqrt(3)/2*output->radius + e.height + 2);
      cr->show_text(header);
    }
  cr->restore();
  
  /* --------------------
     Add band - visibility done in view function
     -------------------- */
  
  if (ovh->band)
    {
      char band[10];
      snprintf(band, 10, "%d", ovh->band);
      cr->get_text_extents(band, e);
      cr->move_to(cx - e.width, cy - 5);
      cr->show_text(band);

      //      cr->save();
      //      cr->translate(cx, cy - 25);
      //      cr->scale(0.15, 0.15);
      RsvgRectangle viewport = {.x = cx,.y = cy - 25,.width = 30,.height = 30 };
      rsvg_handle_render_document(id2sym[OUTPUT_LIZARD]->handle, (cairo_t*)(cr->cobj()), &viewport, 0);
      //      rsvg_handle_render_cairo(id2sym[OUTPUT_LIZARD]->handle, (cairo_t*)(cr->cobj()));
      //      cr->restore();
    }
  else if (!ovh->center_text.empty())
    {
      const char *center_str = ovh->center_text.c_str();
      cr->get_text_extents(center_str, e);
      cr->move_to(cx - e.width/2, cy+5);
      cr->show_text(center_str);
    }
}

/* ----------------------------------------------------------------------
--
-- output_t::view
--
---------------------------------------------------------------------- */

void output_private_t::view(view_hex_t *see)
{
  svgmap_t *smap = type2sym[see->hex->isa()];
  assert(smap);
  see->symbol = smap ? smap->output : 0;
  hex_t *hex = see->hex;
  
  den_t *den = dynamic_cast<den_t*>(hex);
  band_t *band = dynamic_cast<band_t*>(hex->get(BAND));
  
  see->footer_text = hex->getid();

  switch(see->symbol)
    {
    case OUTPUT_WATER:
    case OUTPUT_WHIRLPOOL:
    case OUTPUT_BRIDGE:
      see->background = BG_WATER;
      break;
    case OUTPUT_BLANK:
      see->background = BG_BLANK;
      break;
    default:
      see->background = BG_LAND;
      break;
    }

  if (den)
    {
      if (den->home)
	{
	  see->symbol = OUTPUT_HOME_DEN;
	  see->center_text = "HOME DEN";
	}
      if (hex->owner == 0)
	{
	  see->header_text = "Free";
	}
      else
	{
	  if (hex->owner == output->player)
	    see->band = band ? band->size() + den->pop : den->pop;
	  else
	    see->band = 0;
	  see->header_text = see->owner;
	  }
    }
  else
    { 
      if (band && ((see->hex->owner == output->player) || !(see->hex->isa(SCRUB) || see->hex->isa(RUIN))))
	{
	  see->band = band->size();
	  see->header_text = see->owner;
	}
      else
	see->band = 0;
    }
}

/* ----------------------------------------------------------------------
--
-- output_private_t(grid_t *g, int p)
--
---------------------------------------------------------------------- */

output_private_t::output_private_t(output_t *o)
{
  output = o;

  /* --------------------
     Load SVG icons
     -------------------- */
  
  int nmap = sizeof(svgmap)/sizeof(svgmap_t);
  for (int i = 0; i < nmap; i++)
    {
      if (svgmap[i].svgfile)
	svgmap[i].handle = rsvg_handle_new_from_file(svgmap[i].svgfile, 0);
      else
	svgmap[i].handle = nullptr;

      id2sym[svgmap[i].output] = &svgmap[i];
      if (svgmap[i].type)
	{
	  misc_t::log(LOG_DEBUG, "output_private_t::output_private_t(%s)", svgmap[i].type);
	  type2sym[svgmap[i].type] = &svgmap[i];
	}
    }

  for (auto kv : output->view->overlay)
    {
      hexes.push_back(kv.second);
      misc_t::log(LOG_DEBUG, "output_private_t(%s)", kv.second->hex->getid().c_str());
      view(kv.second);
    }
}


/* ----------------------------------------------------------------------
--
-- svg
--
---------------------------------------------------------------------- */

int output_t::svg(const char *file)
{
  /* --------------------
     borders are scaled because they are applied to the hex bounding box in place()
     -------------------- */

  int s_width = width * scalef; // + 2 * border;
  int s_height = height * scalef; // + 2 * border;

  Cairo::RefPtr<Cairo::SvgSurface> surface = Cairo::SvgSurface::create(file, s_width, s_height);
  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

  auto font = Cairo::ToyFontFace::create("Nimbus Sans", Cairo::ToyFontFace::Slant::NORMAL, Cairo::ToyFontFace::Weight::NORMAL);
  cr->set_font_face(font);
  cr->set_font_size(10);

  Cairo::FontOptions font_options;

  //  font_options.set_hint_style(Cairo::HINT_STYLE_NONE);
  //  font_options.set_hint_metrics(Cairo::HINT_METRICS_OFF);
  //  font_options.set_antialias(Cairo::ANTIALIAS_BEST);
  cr->set_font_options(font_options);

  /* --------------------
     translate for to fit in 0,0+width,height box
     -------------------- */

  misc_t::log(LOG_DEBUG, "output_t::svg(%f,%f+%f,%f)", edge.left,edge.top,edge.right,edge.bottom);

  cr->scale(scalef, scalef);
  cr->translate(-edge.left, -edge.top);
  
  view_hex_t *active = nullptr;

  priv = new output_private_t(this);

  for (auto hex : priv->hexes)
    {
      assert(hex);
      if (hex->hex->active)
	active = hex;
      else
	priv->path_paint_hex(cr, hex);
    }

  misc_t::log(LOG_DEBUG, "output_t::svg()");
  
  /* --------------------
     Output active last
     -------------------- */
  
  if (active) priv->path_paint_hex(cr, active);
  
  return 1;
}

/* ----------------------------------------------------------------------
--
-- output_t::json
--
---------------------------------------------------------------------- */

int output_t::json(const char *file)
{
  priv = new output_private_t(this);
  serial_map_t s;
  s.add("width", width);
  s.add("height", height);
  serial_map_t box;
  box.add("top", edge.top);
  box.add("left", edge.left);
  box.add("bottom", edge.bottom);
  box.add("right", edge.right);
  s.add("bounds", box);
  serial_map_t hexes;
  
  for (auto kv : view->overlay)
    {
      view_hex_t *view = kv.second;
      serial_map_t hex;

      serial_map_t center;
      center.add("x", view->place.center.x);
      center.add("y", view->place.center.y);
      hex.add("center", center);
      
      serial_array_t path;
      for (auto p : view->place.hull)
	{
	  serial_map_t xy;
	  xy.add("x", p.x);
	  xy.add("y", p.y);
	  path.add(xy);
	}

      svgmap_t *smap = priv->type2sym[view->hex->isa()];
      assert(smap);
      view->symbol = smap ? smap->output : 0;

      den_t *home = dynamic_cast<den_t*>(view->hex);
      if (home && home->home)
	view->symbol = OUTPUT_HOME_DEN;
	  
      switch(view->symbol)
	{
	case OUTPUT_WATER:
	case OUTPUT_WHIRLPOOL:
	case OUTPUT_BRIDGE:
	  hex.add("fill", "#CCF0FF");
	  break;
	case OUTPUT_BLANK:
	  hex.add("fill", "#CCCCCC");
	  break;
	default:
	  hex.add("fill", "#CCFFCC");
	  break;
	}
      hex.add("path", path);
      hexes.add(view->hex->getid(), hex);
      
      if (priv->id2sym[view->symbol]->svgfile)
	{
	  hex.add("symbol", priv->id2sym[view->symbol]->svgfile);
	  misc_t::log(LOG_DEBUG, "output_t::json(%s %s)", view->hex->getid().c_str(), priv->id2sym[view->symbol]->svgfile);
	}
    }
  s.add("hexes", hexes);
  s.to_file(file);
  return 0;
}

/* ----------------------------------------------------------------------
--
-- output_t::hit
--
---------------------------------------------------------------------- */
hex_t* output_t::hit(double x, double y, double w, double h)
{
  priv = new output_private_t(this);
  return priv->hit(x, y, w, h);
}


hex_t* output_private_t::hit(double x, double y, double w, double h)
{
  hex_t *hit = 0;
  /*
  double nx = x/w * width;
  double ny = y/h * height;

  misc_t::log(LOG_DEBUG, "nx,ny = %f,%f : width,height = %d,%d", nx, ny, width, height);

  Cairo::RefPtr<Cairo::SvgSurface> surface = Cairo::SvgSurface::create("/dev/null", width, height);
  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

  fbox_t fb;
  cr->save();
  path_extents(cr);
  cr->set_line_width(0.4);
  cr->get_stroke_extents(fb.left, fb.top, fb.right, fb.bottom);
  cr->restore();

  //  nx += fb.left - border;
  //  ny += fb.top - border;

  cr->save();
  //  cr->translate(-fb.left + border, -fb.top + border);
  for (auto ohex : hexes)
    {
      if (ohex == 0)
	continue;
      ohex->hex->active = 0;

      cr->begin_new_path();
      cr->move_to(ohex->place.hull[0].x, ohex->place.hull[0].y);
      for (int i = 1; i < 6; i++)
	cr->line_to(ohex->place.hull[i].x, ohex->place.hull[i].y);
      cr->close_path();
      if (cr->in_fill(nx, ny))
	{
	  hit = ohex->hex;
	  ohex->hex->active = 1;
	  misc_t::log(LOG_NOTICE, "Hit for hex %s", hit->getid());
	}
    }
  cr->restore();
  */
  return hit;
}

/* ----------------------------------------------------------------------
--
-- singles_t
--
---------------------------------------------------------------------- */
unsigned int singles_t::size()
{
  return (*this).singles.size();
}

/* ----------------------------------------------------------------------
--
-- place
--
---------------------------------------------------------------------- */

void output_t::place()
{
  view = new view_t(grid, player);

  std::vector<view_hex_t*> hexes;
  for (auto kv : view->overlay)
    hexes.push_back(kv.second);

  /* --------------------
     Make sure we have atleast one hex
     -------------------- */

  assert(hexes.size() > 0);
  
  /* --------------------
     Center as much as possible
     -------------------- */
  
  if (player > 0)
    {
      int min_weight = grid->width * grid->height * grid->width * grid->height;
      view_hex_t* hex_min_weight;

      for (auto a : hexes)
	{
	  int x = a->xy.x;
	  int y = a->xy.y;
	  
	  int total = 0;
	  for (auto b : hexes)
	    {
	      int xd = abs(b->xy.x - x);
	      int yd = abs(b->xy.y - y);
	      total += min(xd, grid->width - xd);
	      total += min(yd, grid->height - yd);
	    }
	  if (total < min_weight)
	    {
	      min_weight = total;
	      hex_min_weight = a;
	    }
	}
      point_t center = hex_min_weight->xy;
      center.x -= center.x%2;

      point_t middle(grid->width / 2, grid->height / 2);
      point_t diff = middle - center;
      assert(diff.x%2 == 0);

      misc_t::log(LOG_DEBUG, "Hex %s (%d,%d) has minimum weight %d", hex_min_weight->hex->getid().c_str(), hex_min_weight->xy.x, hex_min_weight->xy.y, min_weight);

      /* --------------------
	 Shift and wrap hexes
	 -------------------- */

      for (auto hex : hexes)
	{
	  point_t pt = hex->xy + diff;
	  hex->xy = pt.clip(grid->width, grid->height);
	}
    }

  /* --------------------
     Place hexes and expand edge
     -------------------- */
  
  for (auto hex : hexes)
    {
      point_t o = hex->xy;
      fplace_t b(o, radius, border);
      hex->place = b;
      edge.setunion(b.bounds);
      if (hex->hex->owner)
	{
	  if (game)
	    hex->owner = game->player(hex->hex->owner)->code;
	  else
	    hex->owner = fmt::sprintf("P%02d", hex->hex->owner);
	}
    }

  width = (edge.right - edge.left);
  height = (edge.bottom - edge.top);
}


/* ----------------------------------------------------------------------
--
-- output_t(grid_t*)
--
---------------------------------------------------------------------- */

output_t::output_t(grid_t *gd, int p)
{
  grid = gd;
  game = nullptr;
  scalef = 1.0;
  border = 5.0;
  radius = 40.0;
  player = p;
  water_colour = {.red = 0.80, .green = 0.94, .blue = 0.99};
  land_colour = {.red = 0.80, .green = 0.99, .blue = 0.80};
  border_width = 0.2;
  place();
}

/* ----------------------------------------------------------------------
--
-- output_t(game_t*)
--
---------------------------------------------------------------------- */

output_t::output_t(game_t *ga, int p)
{
  game = ga;
  grid = ga->grid;
  scalef = 1.0;
  border = 5.0;
  radius = 40.0;
  player = p;
  place();
}
