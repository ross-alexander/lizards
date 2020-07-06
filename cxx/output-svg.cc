#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

struct svgbox_t {
  point_t tl;
  point_t br;
  double width;
  double height;
};

/* ----------------------------------------------------------------------
--
-- svg_show_hex
--
---------------------------------------------------------------------- */
static void svg_show_hex(world_t *world, double cx, double cy, double radius,
			 int x, int y, int player, FILE *fptr)
{
  hex_t *hex = (*world->map)(x, y);
  feature_t *feature;
  char *fill = "none";
  double s3 = sqrt(3);
  double r2 = radius/2.0;

  switch (hex->terrain)
    {
    case PLAIN:
    case WATER:
      fill = "#8080ff";
      break;
    }
  fprintf (fptr, "<path stroke=\"black\" fill=\"%s\" stroke-width=\"1\" d=\"M%d %d L%d %d L %d %d L %d %d %d %d L %d %d Z\"/>\n",
	   fill,
	   (int)(cx - r2), (int)(cy - s3 * r2),
	   (int)(cx + r2), (int)(cy - s3 * r2),
	   (int)(cx + radius), (int)(cy),
	   (int)(cx + r2), (int)(cy + s3 * r2),
	   (int)(cx - r2), (int)(cy + s3 * r2),
	   (int)(cx - radius), (int)(cy));

  fprintf(fptr, "<text x=\"%d\" y=\"%d\" font-family=\"Helvetica\" font-size=\"12\">%s</text>\n",
	  (int)cx - 10, (int)(cy + radius - 10), hex->title);

  if ((feature = hex->has_feature(DEN)))
    {
      fprintf(stdout, "Den @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#den\" x=\"%d\" y=\"%d\" style=\"stroke: black; fill: white;\"/>\n",
      	      (int)(cx),
     	      (int)(cy + radius - 20));
    }
  else if ((feature = hex->has_feature(FERTILE)))
    {
      fprintf(stdout, "Fertile @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#fertile\" x=\"%d\" y=\"%d\" style=\"stroke: black; fill: yellow;\"/>\n",
	      (int)(cx),
	      (int)(cy + radius - 20));
    }
  else if (hex->has_feature(SWAMP))
    {
      fprintf(stdout, "Swamp @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#swamp\" x=\"%d\" y=\"%d\" stroke=\"black\" fill=\"none\"/>\n",
	      (int)(cx),
      	      (int)(cy + radius - 20));
    }
  else if (hex->has_feature(SCRUB))
    {
      fprintf(fptr, "<use xlink:href=\"#scrub\" x=\"%d\" y=\"%d\" style=\"stroke: black; fill: white;\"/>\n",
      	      (int)(cx),
      	      (int)(cy + radius - 20));
    }
  else if ((feature = hex->has_feature(TEMPLE)))
    {
      fprintf(fptr, "<use xlink:href=\"#temple\" x=\"%d\" y=\"%d\" stroke=\"black\" fill=\"none\"/>\n",
      	      (int)(cx),
      	      (int)(cy + radius - 20));
    }
  else if ((feature = hex->has_feature(RUIN)))
    {
      fprintf(fptr, "<use xlink:href=\"#ruin\" x=\"%d\" y=\"%d\" stroke=\"black\" fill=\"none\"/>\n",
      	      (int)(cx),
      	      (int)(cy + radius - 20));
    }
  else if (hex->terrain == PLAIN)
    {
      fprintf(stdout, "Plain @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#plains\" x=\"%d\" y=\"%d\" style=\"stroke: green; fill: none;\"/>\n",
      	      (int)(cx),
      	      (int)(cy + radius - 20));
    }

}

/* ----------------------------------------------------------------------
--
-- svg_show_world
--
---------------------------------------------------------------------- */
static void svg_show_world (world_t *world, int player, char *defs, FILE *fptr)
{
  double page_x, page_y,
    min_x = 2 << 20,
    min_y = 2 << 20,
    max_x = -2 << 20,
    max_y = -2 << 20;
  int work;
  int radius = 50;
  FILE *stream;

  bandlist_t bandlist(world, player);

  if (bandlist.cnt == 0)
    return;

  hexlist_t* hexlist = new hexlist_t(world, &bandlist);

  //  bbox_t box = hexlist->centre();
  bbox_t box = hexlist->zero();

  printf("%d %d %d %d\n", box.left, box.top, box.right, box.bottom);

  int range = box.bottom - box.top + 1;

  for (int i = 0; i < hexlist->cnt; i++)
    {
      see_t *see = &hexlist->list[i];
      see->mx = 1.5 * see->cx + 1;
      see->my = sqrt(3) * ((see->cy - range) - (abs(see->cx%2))/2.0);

      page_x = see->mx * radius;
      page_y = see->my * radius;

      printf("%02d ++ %02d %02d %02d %02d : %8.2f %8.2f %f %f %f %f\n",  i,
	   hexlist->list[i].x,
	   hexlist->list[i].y,
	   hexlist->list[i].cx,
	   hexlist->list[i].cy,
	   page_x, page_y, min_x, min_y, max_x, max_y);

      if (page_x < min_x) min_x = page_x;
      if (page_y < min_y) min_y = page_y;
      if (page_x > max_x) max_x = page_x;
      if (page_y > max_y) max_y = page_y;
    }
  min_x -= 2 * radius;
  min_y -= 2 * radius;
  max_x += 2 * radius;
  max_y += 2 * radius;

  fprintf(fptr, "<?xml version=\"1.0\" standalone=\"no\"?>\n");
  fprintf(fptr, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");

  fprintf(fptr, "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" width=\"%d\" height=\"%d\" background=\"white\">\n",
	  (int)(max_x - min_x),
	  (int)(max_y - min_y));

  if ((stream = fopen(defs, "r")))
    {
      while (!feof(stream))
	{
	  char buf[256];
	  int count = fread(buf, sizeof(char), 256, stream);
	  if (count) fwrite(buf, sizeof(char), count, fptr);
	}
      fclose(stream);
    }


  fprintf(fptr, "<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"white\" stroke=\"white\"/>\n",
	  (int)(max_x - min_x),
	  (int)(max_y - min_y));

  for (work = 0; work < hexlist->cnt; work ++)
    {
      see_t *see = &hexlist->list[work];
      page_x = see->mx * radius;
      page_y = see->my * radius;

      svg_show_hex(world,
		   page_x - min_x, page_y - min_y, radius,
		   see->x, see->y,
		   player,
		   fptr);
      
    }
  fprintf (fptr, "</svg>\n");
}

void output_svg_t::display(void)
{
  int work;
  FILE *out_fptr;
  char out_filename[200];

  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", world->game);
      exit (EXIT_FAILURE);
    }

  printf ("Building player maps...\n");
  for (work = 1; (work < world->num_players) && (work < 2); work ++)
    if (world->player[work])
      {
	printf ("SVGing Player %d...\n", work);
	sprintf (out_filename, "%s%02d%02d%s",
		 world->game,
		 world->turn, work,
		 ".svg");

	if ((out_fptr = fopen (out_filename, "w")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
	    exit (EXIT_FAILURE);
	  }
	svg_show_world (world, work, "defs.svg", out_fptr);
	fclose (out_fptr);
      }
  printf ("\nFinished!\n");
}

output_svg_t::output_svg_t(world_t*w) : output_t(w)
{
}
