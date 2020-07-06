#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "lizgame.h"

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  FILE *fptr,
       *fptr2;

  char work_string [140],
       filename [20];

  int work;

  show_title ();

  printf ("LIZARDS! Message Append Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc != 3)
  {
    printf ("FATAL ERROR: Turn Number and Game not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path game.\n"
	     "                             e.g. MESSAPND DATA 001\n");

    exit (EXIT_FAILURE);
  }

  data_path = LzCheckDir(argv[1], 0);
  game_code = (char*)malloc(strlen(argv[2] + 1));
  strcpy (game_code, argv[2]);
  strupr (game_code);

  world = (World*)malloc(sizeof(World));
  get_world(world);

  if (world->num_players == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  /* append 'the other bit' (if there is one) in player 0's message area to
     ALL the other players! */

  sprintf (filename, "%s%s00_%03d.%s", data_path, PLYPLY_MESG_F, world->turn,
    game_code);

  if ((fptr = fopen (filename, "rt")) != NULL)
  {
    for (work = 1; work < world->num_players + 1; work ++)
    {
      fseek (fptr, 0, SEEK_SET);

      printf ("Appending 'The Other Bit' to Player %d's Messages...\n", work);

      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F, work,
        world->turn, game_code);

      if ((fptr2 = fopen (filename, "at")) == NULL)
	if ((fptr2 = fopen (filename, "wt")) == NULL)
	{
	  fprintf (stderr, "FATAL ERROR: Unable to write to '%s' file.\n", filename);
	  exit (EXIT_FAILURE);
	}

      while (fgets (work_string, 140, fptr) != NULL)
	fprintf (fptr2, "%s", work_string);

      fclose (fptr2);
    }

    fclose (fptr);
  }

  for (work = 0; work < world->num_players + 1; work ++)
  {
    printf ("Appending Player %d's Messages...\n", work);

    sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F, work,
      world->turn, game_code);

    if ((fptr = fopen (filename, "rt")) != NULL)
    {
      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, work,
        world->turn, game_code);

      if ((fptr2 = fopen (filename, "at")) == NULL)
      {
        fprintf (stderr, "FATAL ERROR: Unable to append to %s file.\n", filename);
	exit (EXIT_FAILURE);
      }

      fprintf (fptr2, "\n * Speaking to the Enemy in Turn %d:\n\n",
	       world->turn);

      while (fgets (work_string, 140, fptr) != NULL)
	fprintf (fptr2, "%s", work_string);

      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F, work,
        world->turn, game_code);

      remove (filename);
    }
  }

  for (work = 0; work < world->num_players + 1; work ++)
  {
    printf ("Appending Player %d's Errors...\n", work);

    sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYERR_MESG_F, work,
      world->turn, game_code);

    if ((fptr = fopen (filename, "rt")) != NULL)
    {
      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, work,
        world->turn, game_code);

      if ((fptr2 = fopen (filename, "at")) == NULL)
      {
	fprintf (stderr, "FATAL ERROR: Unable to append to %s file.\n", filename);
	exit (EXIT_FAILURE);
      }

      while (fgets (work_string, 140, fptr) != NULL)
	fprintf (fptr2, "%s", work_string);
      sprintf (filename, "%s%s.%s.%02d_%03d", game_code,  PLYERR_MESG_F,
	       data_path, work, world->turn);

      remove (filename);
    }
  }

  printf ("\nFinished!\n");

  return (EXIT_SUCCESS);
}
