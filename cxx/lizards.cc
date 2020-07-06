#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <string.h>
#include <math.h>
#include <libxml++/libxml++.h>
#include <glib.h>
#include <cairo.h>
#include <librsvg/rsvg.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

// debug_t dbg;
// char *progname;

// ----------------------------------------------------------------------
//
// main
//
// ----------------------------------------------------------------------
int main(int argc, char *argv[])
{
  char *option = "version";
  int ch;
  int x = 32;
  int y = 32;
  char *playerfile = "game.ini";
  time_t now = time(NULL);
  char *game = "Test";
  misc_t::dbg.log("random time seed = %d", now);
  srandom(now);
  while ((ch = getopt(argc, argv, "x:y:p:r:o:")) != EOF)
    {
    switch (ch)
      {
      case 'x':
	x = atoi(optarg);
	break;
      case 'y':
	y = atoi(optarg);
	break;
      case 'p':
	playerfile = optarg;
	break;
      case 'r':
	srandom(atoi(optarg));
	break;
      case 'o':
	option = optarg;
	break;
      }
    }
  char *gamefile = g_strdup_printf("%s.xml", game);

  printf("Lizards! called with option %s.\n", option);

  if (!strcmp("version", option))
    {
      fprintf(stdout, "Lizards! (c++ version %s)\n.", LZ_VERSION);
      return EXIT_SUCCESS;
    }
  if (!strcmp("test", option))
    {
      world_t *world = new world_t(gamefile);
      delete world;
    }
  if (!strcmp("create", option))
    {
      world_t* world = new world_t(x, y, playerfile, argc, argv);
      world->save();
      world->open_player_files();
      turn_t t(world);
      t.clanreport();
      world->close_player_files();
      delete world;
    }
  if (!strcmp("reload", option))
    {
      world_t *world = new world_t(gamefile);
      world->save();
    }
  if (!strcmp("report", option))
    {
      world_t *world = new world_t(gamefile);
      world->open_player_files();
      turn_t(world).clanreport();
      world->close_player_files();
    }
#ifdef OldOutputs
  if (!strcmp("svg", option))
    {
      world_t *world = new world_t(gamefile);
      output_svg_t *output = new output_svg_t(world);
      output->display();
    }
  if (!strcmp("ps", option))
    {
      world_t *world = new world_t(gamefile);
      output_ps_t *output = new output_ps_t(world);
      output->display();
    }
  if (!strcmp("gtk", option))
    {
      world_t *world = new world_t(gamefile);
      output_gtk_t *output = new output_gtk_t(world, argc, argv);
      output->display();
    }
#endif
  if (!strcmp("cairo", option))
    {
      world_t *world = new world_t(gamefile);

      for (int x = 0; x < world->map->width; x++)
	for (int y = 0; y < world->map->height; y++)
	  {
	    hex_t *h = (*world->map)(x, y);
	    assert(h->map);
	  }

      output_cairo_t *output = new output_cairo_t(world, argc, argv);
      output->display();
    }
#ifdef HaveGL
  if (!strcmp("glx", option))
    {
      world_t *world = new world_t(gamefile);
      output_glx_t *output = new output_glx_t(world, argc, argv);
      output->display();
    }
  if (!strcmp("glu", option))
    {
      world_t *world = new world_t(gamefile);
      output_glu_t *output = new output_glu_t(world, argc, argv);
      output->display();
    }
#endif
  if (!strcmp("turn", option))
    {
      world_t *world = new world_t(gamefile);
      world->turn++;
      world->open_player_files();
      turn_t t(world);
      t.textorderconvert();
      t.terrain();
      t.ordershuffle();
      t.worldopen();
      t.worldmaster();
      t.worldclose();
      t.clanreport();
      world->close_player_files();
      world->save();
      output_cairo_t *output = new output_cairo_t(world, argc, argv);
      output->display();
    }
  return EXIT_SUCCESS;
}
