#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "lizards.h"
#include "lizgame.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */
extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  world_t *world;
  int ch, save = 0;

  randomize ();
  show_title ();

  printf ("LIZARDS! World Turn. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  while((ch = getopt(argc, argv, "s")) != EOF)
    switch(ch)
      {
      case 's':
	save = 1;
	break;
      }
  if ((argc - optind) != 2)
    {
      fprintf(stderr, "%s: [data path] [game code]\n", argv[0]);
      exit(1);
    }
  world = get_world(LzCheckDir(argv[optind], 0), argv[optind + 1]);
  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", world->game);
      exit (EXIT_FAILURE);
    }
  OpenPlayerFiles(world);
  TextOrderConvert(world);
  Terrain(world);
  BinaryOrderShuffle(world);
  WorldOpen(world);
  WorldMast(world);
  WorldClose(world);
  ClanReport(world);
  ClanRank(world);
  MesgAppend(world);
  ClosePlayerFiles(world);
  if (save)
    {
      world->turn += 1;
      put_world(world);
    }
  printf ("Finished!\n");
  return (EXIT_SUCCESS);
}
