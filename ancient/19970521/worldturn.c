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

#include "lizard.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- Prototypes
--
---------------------------------------------------------------------- */

int main (int argc, char *argv []);

extern void TextOrderConvert(World*);
extern void Terrain(World*);
extern void BinaryOrderShuffle(World *world);
extern void WorldOpen(World *);
extern void WorldMast(World *);
extern void WorldClose(World *);
extern void ClanReport(World *);
extern void ClanRank(World *);
extern void MesgAppend(World *world);

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */
extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  int ch;

  randomize ();
  show_title ();

  printf ("LIZARDS! World Turn. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  while((ch = getopt(argc, argv, "")) != EOF)
    ;
  if ((argc - optind) == 2)
    {
      data_path = LzCheckDir(argv[optind++], 0);
      game_code = (char*)malloc(strlen(argv[optind]) + 1);
      strcpy (game_code, argv[optind]);
    }
  else
    {
      fprintf(stderr, "%s: [data path] [game code]\n", argv[0]);
      exit(1);
    }
  world = get_world();
  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
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
  put_world(world);

  printf ("\nFinished!\n");
  return (EXIT_SUCCESS);
}
