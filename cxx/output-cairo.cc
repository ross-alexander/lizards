#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>

#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

#include "symbols/symbols.h"
#include "symbols/symbols.c"

/* ----------------------------------------------------------------------
--
-- SVG drawings
--
---------------------------------------------------------------------- */

struct svgmap_t {
  int output;
  const char *svgfile;
};

struct svgmap_t svgmap[OUTPUT_LAST] = {
  {OUTPUT_RAFT,       	"svg/raft.svg"},
  {OUTPUT_VOLCANO,     	"svg/volcano.svg"},
  {OUTPUT_BRIDGE,      	"svg/bridge.svg"},
  {OUTPUT_BUILDING,   	"svg/building.svg"},
  {OUTPUT_WHIRLPOOL,   	"svg/whirlpool.svg"},
  {OUTPUT_CURSED,   	"svg/cursed.svg"},
  {OUTPUT_SWAMP,       	"svg/swamp.svg"},
  {OUTPUT_SCRUB,       	"svg/scrub.svg"},
  {OUTPUT_BAND,       	"svg/band.svg"},
  {OUTPUT_FERTILE,	"svg/fertile.svg"},
  {OUTPUT_TEMPLE,	"svg/temple.svg"},
  {OUTPUT_PLAIN,	"svg/plains.svg"},
  {OUTPUT_PEAK,		"svg/peak.svg"},
  {OUTPUT_RUIN,		"svg/ruin.svg"},
  {OUTPUT_EMPTY_DEN,	"svg/den.svg"},
  {OUTPUT_RED_DEN,	"svg/den-red.svg"},
  {OUTPUT_BLACK_DEN,	"svg/den-black.svg"},
  {OUTPUT_YELLOW_DEN,	"svg/den-yellow.svg"},
  {OUTPUT_GREY_DEN,	"svg/den-grey.svg"},
  {OUTPUT_GREEN_DEN,	"svg/den-green.svg"},
  {OUTPUT_HOME_DEN,	"svg/den-home.svg"},
  {OUTPUT_LIZARD,	"svg/lizard.svg"},
  {0,			0},
};


cairo_tab_t::cairo_tab_t(output_cairo_t *par, int p)
{
  parent = par;
  world = par->world;
  hlist = new hexlist_t(world, p);
  edge = hlist->fcenter();

  border.left = border.right = border.top = border.bottom = 10.0;
  radius = 40.0;
  
  width = (int)((edge.right - edge.left) *  radius + border.left + border.right);
  height = (int)((edge.bottom - edge.top) *  radius + border.top + border.bottom);
}

static double fill[][3] = {
  {0.4,0.4,1.0}, /* water */
  {0.1,0.8,0.2}, /* plain */
  {0.7,0.4,0.1}, /* peak */
  {0.9,1.0,0.9}, /* swamp */
  {0.8,0.8,0.0}}; /* scrub */

RsvgHandle *cairo_svg_outputs[OUTPUT_LAST];

/* ----------------------------------------------------------------------
--
-- draw
--
---------------------------------------------------------------------- */

void cairo_tab_t::draw(cairo_t *cr)
{
  if (hlist->cnt == 0)
    return;

  double r = radius;
  hexlist_t *hexlist = hlist;

  /* --------------------
     Save and set base parameters
     -------------------- */

  cairo_save(cr);  // 1
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_select_font_face(cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 12.0);
  cairo_set_line_width(cr, 0.5);

  cairo_translate(cr, -edge.left * r + border.left, -edge.top * r + border.top);

  cairo_save(cr); // 2
  cairo_scale(cr, r, r);
  cairo_move_to(cr, edge.left, edge.top);
  cairo_line_to(cr, edge.right, edge.top);
  cairo_line_to(cr, edge.right, edge.bottom);
  cairo_line_to(cr, edge.left, edge.bottom);
  cairo_close_path(cr);
  //  cairo_stroke(cr);
  cairo_restore(cr); // 2
  
  double s3 = sqrt(3);
  double r2 = r/2.0;
  double s3r2 = s3 * r2;
  
  for (int work = 0; work < hexlist->cnt; work ++)
    {
      see_t *see = &hexlist->list[work];
      hex_t *hex = (*world->map)(see->x, see->y);

      double cx = see->mx * r;
      double cy = see->my * r;

      cairo_save(cr); // 2
      cairo_translate(cr, cx, cy);

      cairo_new_path(cr);
      cairo_move_to(cr, -r2, -s3r2);
      cairo_line_to(cr, +r2, -s3r2);
      cairo_line_to(cr, +r, 0.0);
      cairo_line_to(cr, +r2, +s3r2);
      cairo_line_to(cr, -r2, +s3r2);
      cairo_line_to(cr, -r, 0.0);
      cairo_close_path(cr);

      int color = hex->terrain;
      cairo_save(cr); // 3
      cairo_set_source_rgb(cr, fill[color][0], fill[color][1], fill[color][2]);
      cairo_fill_preserve(cr);
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_set_line_width(cr, 1.0);
      cairo_stroke(cr);
      cairo_restore(cr); // 3

      cairo_set_line_width(cr, 1.0);

      cairo_save(cr); // 3
      cairo_translate(cr, 0.0, r - r/3.0);

      int output = parent->order(hex);
      RsvgHandle *handle = parent->cairo_svg_outputs[output];
      
      if (handle)
	{
	  cairo_scale(cr, r / 150.0, r / 150.0);
	  cairo_translate(cr, -100, -200);
	  rsvg_handle_render_cairo(handle, cr);
	}
      cairo_restore(cr); // 3

	  /* Title */

      cairo_text_extents_t xt;
      cairo_text_extents(cr, hex->title, &xt);
      
      cairo_move_to(cr, -xt.width / 2, s3r2 - 2);
      cairo_show_text(cr, hex->title);
      
      if (hex->owner)
	{
	  player_t *p = world->get_player(hex->owner);
	  cairo_text_extents(cr, p->code, &xt);
	  cairo_move_to(cr, -xt.width / 2, -s3r2 + xt.height + 2);
	  cairo_show_text(cr, p->code);
	}
      
      if (hex->has_feature(BAND))
	{
	  char tmp[10];
	  
	  band_t *band = (band_t*)hex->has_feature(BAND);
	  cairo_move_to(cr, -20, -10);
	  snprintf(tmp, 10, "%d", band->size());
	  cairo_show_text(cr, tmp);
	  
	  cairo_save(cr); // 3
	  cairo_translate(cr, 10, -20);
	  cairo_scale(cr, 0.1, 0.1);
	  if (parent->cairo_svg_outputs[OUTPUT_BAND])
	    rsvg_handle_render_cairo(parent->cairo_svg_outputs[OUTPUT_BAND], cr);
	  cairo_restore(cr); // 3
	}
      cairo_restore(cr); // 2
    }
  cairo_restore(cr); // 1
  cairo_show_page(cr);
}

/* ----------------------------------------------------------------------
--
-- display
--
---------------------------------------------------------------------- */

void output_cairo_t::display()
{

  printf ("Building player maps...\n");
  for (int i = 1; i < world->num_players; i++)
    if (tabs[i])
      {
	char *f;
	cairo_tab_t *tab = tabs[i];
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, tab->width,tab->height);
	cairo_t *cairo = cairo_create(surface);
	tabs[i]->draw(cairo);
	f = g_strdup_printf("%s%02d%02d.png", world->game, world->turn, i);
	cairo_surface_write_to_png(surface, f);
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
	free(f);

	f = g_strdup_printf("%s%02d%02d.pdf", world->game, world->turn, i);
	surface = cairo_pdf_surface_create(f, tab->width, tab->height);
	cairo = cairo_create(surface);
	tabs[i]->draw(cairo);
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
	free(f);

#ifdef OutputPS
	
	f = g_strdup_printf("%s%02d%02d.ps", world->game, world->turn, i);
	surface = cairo_ps_surface_create(f, tab->width, tab->height);
	cairo = cairo_create(surface);
	tabs[i]->draw(cairo);
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
	free(f);

#endif

	f = g_strdup_printf("%s%02d%02d.svg", world->game, world->turn, i);
	surface = cairo_svg_surface_create(f, tab->width, tab->height);
	cairo = cairo_create(surface);
	tabs[i]->draw(cairo);
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
	free(f);
	delete tab;
      }
  printf("\nFinished!\n");
}

/* ----------------------------------------------------------------------
--
-- output_cairo_t
--
---------------------------------------------------------------------- */

output_cairo_t::output_cairo_t(world_t *w, int p_argc, char *p_argv[]) : output_t(w)
{
  argc = p_argc;
  argv = p_argv;

  /* --------------------
     load SVG icons
     -------------------- */

  rsvg_init();

  for (int i = 0; i < OUTPUT_LAST; i++)
    cairo_svg_outputs[i] = 0;

  for (int i = 0; (svgmap[i].svgfile != 0); i++)
    {
      int output = svgmap[i].output;
      printf("%d %s\n", i, svgmap[i].svgfile);
      cairo_svg_outputs[output] = rsvg_handle_new_from_file(svgmap[i].svgfile, 0);
      assert(cairo_svg_outputs[output]);
    }
  
  tabs = new cairo_tab_t*[world->num_players];

  for (int i = 0; i < world->num_players; i++)
    tabs[i] = NULL;

  for (int i = 1; i < world->num_players; i++)
    if (world->player[i])
      tabs[i] = new cairo_tab_t(this, i);
}
