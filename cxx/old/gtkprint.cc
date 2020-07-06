#include <stdlib.h>
#include <libxml/tree.h>
#include "lizards.h"
#include "hexlist.h"

/* ----------------------------------------------------------------------
--
-- gtk_show_world
--
---------------------------------------------------------------------- */
static void gtk_show_world (world_t *world, int player, char *defs, FILE *fptr)
{
  bandlist_t bandlist(world, player);
  char *map = new char[world->map.width * world->map.height];
  int lb, ub;
  int lx = world->map.width;
  int ux = 0;

  if (bandlist.cnt == 0)
    return;

  hexlist_t* hexlist = new hexlist_t(world, &bandlist);
  hexlist->centre();
  
  for (int i = 0; i < hexlist->cnt; i++)
    {
      lx = lx > hexlist->list[i].cx ? hexlist->list[i].cx : lx;
      ux = ux < hexlist->list[i].cx ? hexlist->list[i].cx : ux;
    }

  for (int j = 0; j < world->map.height; j++)
    for (int i = 0; i < world->map.width; i++) 
      map[j * world->map.width + i] = '.';


  for (int i = 0; i < hexlist->cnt; i++)
    {
      hex_t *hex = world->gethex(hexlist->list[i].x, hexlist->list[i].y);
      switch(hex->terrain)
	{
	case WATER:
	  map[hexlist->list[i].cy * world->map.width + hexlist->list[i].cx] = '#';
	  break;
	default:
	  map[hexlist->list[i].cy * world->map.width + hexlist->list[i].cx] = '@';
	  break;
	}
    }
  lb = hexlist->list[0].cy ? hexlist->list[0].cy - 1 : 0;
  ub = hexlist->list[hexlist->cnt-1].cy - 1 == world->map.height ? world->map.height - 1 : hexlist->list[hexlist->cnt-1].cy + 1;

  fprintf(fptr, "%d %d %d %d\n", lx, ux, lb, ub);

  for (int j = lb; j <= ub; j++)
    {
      for (int i = lx; i <= ux; i++) 
	fprintf(fptr, "%c", map[j * world->map.width + i]);
      fprintf(fptr, "\n");
    }
}
int GTKPrint(world_t *world)
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
	printf ("GTKing Player %d...\n", work);
	sprintf (out_filename, "%s/%s%02d%02d%s",
		 world->path, world->game,
		 world->turn, work,
		 ".gtk");

	debug.log("GTKPrint outputfile %s", out_filename);
	if ((out_fptr = fopen (out_filename, "w")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
	    exit (EXIT_FAILURE);
	  }
	gtk_show_world (world, work, "defs.svg", out_fptr);
	fclose (out_fptr);
      }
  printf ("\nFinished!\n");
  return 0;
}
