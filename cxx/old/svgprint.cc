#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <libxml/tree.h>

#include "lizards.h"
#include "hexlist.h"

/* ----------------------------------------------------------------------
--
-- svg_show_hex
--
---------------------------------------------------------------------- */
static void svg_show_hex(world_t *world, double center_x, double center_y, double radius,
			 int x, int y, int player, FILE *fptr)
{
  hex_t *hex = world->gethex(x, y);
  feature_t *feature;
  char *fill = "none";

  switch (hex->terrain)
    {
    case SWAMP:
    case PLAIN:
    case SCRUB:
      fill = "#80ff80";
      break;
    case WATER:
      fill = "#8080ff";
      break;
    case PEAK:
      fill = "brown";
      break;
    }
  fprintf (fptr, "<path stroke=\"black\" fill=\"%s\" stroke-width=\"1\" d=\"M%d %d L%d %d L %d %d L %d %d %d %d L %d %d Z\"/>\n",
	   fill,
	   (int)(center_x - sqrt(3) / 3 * radius), (int)(center_y - radius),
	   (int)(center_x + sqrt(3) / 3 * radius), (int)(center_y - radius),
	   (int)(center_x + sqrt(3) / 3 * 2 * radius), (int)(center_y),
	   (int)(center_x + sqrt(3) / 3 * radius), (int)(center_y + radius),
	   (int)(center_x - sqrt(3) / 3 * radius), (int)(center_y + radius),
	   (int)(center_x - sqrt(3) / 3 * 2 * radius), (int)(center_y));

  fprintf(fptr, "<text x=\"%d\" y=\"%d\" font-family=\"Helvetica\" font-size=\"12\">%s</text>\n",
	  (int)center_x - 10, (int)(center_y + radius - 2), hex->title);

  if ((feature = hex->has_feature(DEN)))
    {
      fprintf(stdout, "Den @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#den\" x=\"%d\" y=\"%d\" style=\"stroke: black; fill: white;\"/>\n",
      	      (int)(center_x),
     	      (int)(center_y + radius - 15));
    }
  else if ((feature = hex->has_feature(FERTILE)))
    {
      fprintf(stdout, "Fertile @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#fertile\" x=\"%d\" y=\"%d\" style=\"stroke: black; fill: yellow;\"/>\n",
	      (int)(center_x),
	      (int)(center_y + radius - 15));
    }
  else if (hex->terrain == SWAMP)
    {
      fprintf(stdout, "Swamp @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#swamp\" x=\"%d\" y=\"%d\" stroke=\"black\" fill=\"none\"/>\n",
	      (int)(center_x),
      	      (int)(center_y + radius - 15));
    }
  else if (hex->terrain == SCRUB)
    {
      fprintf(fptr, "<use xlink:href=\"#scrub\" x=\"%d\" y=\"%d\" style=\"stroke: black; fill: white;\"/>\n",
      	      (int)(center_x),
      	      (int)(center_y + radius - 15));
    }
  else if ((feature = hex->has_feature(TEMPLE)))
    {
      fprintf(fptr, "<use xlink:href=\"#temple\" x=\"%d\" y=\"%d\" stroke=\"black\" fill=\"none\"/>\n",
      	      (int)(center_x),
      	      (int)(center_y + radius - 15));
    }
  else if ((feature = hex->has_feature(RUIN)))
    {
      fprintf(fptr, "<use xlink:href=\"#ruin\" x=\"%d\" y=\"%d\" stroke=\"black\" fill=\"none\"/>\n",
      	      (int)(center_x),
      	      (int)(center_y + radius - 15));
    }
  else if (hex->terrain == PLAIN)
    {
      fprintf(stdout, "Plain @ %s\n", hex->title);
      fprintf(fptr, "<use xlink:href=\"#plains\" x=\"%d\" y=\"%d\" style=\"stroke: green; fill: none;\"/>\n",
      	      (int)(center_x),
      	      (int)(center_y + radius - 15));
    }

}

/* ----------------------------------------------------------------------
--
-- svg_show_world
--
---------------------------------------------------------------------- */
static void svg_show_world (world_t *world, int player, char *defs, FILE *fptr)
{
  int page_one_offset = 0;
  double page_x, page_y,
    min_x = 2 << 20,
    min_y = 2 << 20,
    max_x = -2 << 20,
    max_y = -2 << 20;
  int 
    range,
    work;
  //  int scale = 25;
  int scale = 50;
  FILE *stream;

  bandlist_t bandlist(world, player);

  if (bandlist.cnt == 0)
    return;

  hexlist_t* hexlist = new hexlist_t(world, &bandlist);

  hexlist->centre();

  range = hexlist->list[hexlist->cnt-1].cy - hexlist->list[0].cy + 1;

  for (int i = 0; i < hexlist->cnt; i++)
    printf("%02d ++ %02d %02d %02d %02d\n", i,
	   hexlist->list[i].x,
	   hexlist->list[i].y,
	   hexlist->list[i].cx,
	   hexlist->list[i].cy);

  for (work = 0; work < hexlist->cnt; work ++)
    {
      see_t *see = &hexlist->list[work];
      page_x = (see->cx * sqrt(3) * scale) + scale;
      page_y = ((see->cy - page_one_offset - range) * 2 * scale);
      page_y += see->cx % 2 ? 0 : scale;

      printf("%f %f %f %f %f %f\n", page_x, page_y, min_x, min_y, max_x, max_y);

      if (page_x < min_x) min_x = page_x;
      if (page_y < min_y) min_y = page_y;
      if (page_x > max_x) max_x = page_x;
      if (page_y > max_y) max_y = page_y;
    }
  min_x -= 2 * scale;
  min_y -= 2 * scale;
  max_x += 2 * scale;
  max_y += 2 * scale;

  fprintf(fptr, "<?xml version=\"1.0\" standalone=\"no\"?>\n");
  fprintf(fptr, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");

  fprintf(fptr, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\" background=\"white\">\n",
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
      page_x = (see->cx * sqrt(3) * scale) + scale;
      page_y = ((see->cy - page_one_offset - range) * 2 * scale);
      page_y += see->cx % 2 ? 0 : scale;

      svg_show_hex(world,
		   page_x - min_x, page_y - min_y, scale,
		   see->x, see->y,
		   player,
		   fptr);
      
    }
  fprintf (fptr, "</svg>\n");
}

int SVGPrint(world_t *world)
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
	sprintf (out_filename, "%s/%s%02d%02d%s",
		 world->path, world->game,
		 world->turn, work,
		 ".svg");

	debug.log("SVGPrint outputfile %s", out_filename);
	if ((out_fptr = fopen (out_filename, "w")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
	    exit (EXIT_FAILURE);
	  }
	svg_show_world (world, work, "defs.svg", out_fptr);
	fclose (out_fptr);
      }
  printf ("\nFinished!\n");
  return 0;
}
