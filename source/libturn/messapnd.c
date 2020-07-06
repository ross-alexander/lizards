#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizards.h"

void MesgAppend(world_t *world)
{
  FILE *fptr,
       *fptr2;

  char work_string [140],
       filename [20];

  int work;

  sprintf (filename, "%s%s00_%03d.%s", world->path, PLYPLY_MESG_F, world->turn,
	   world->game);
  
  if ((fptr = fopen (filename, "r")) != NULL)
    {
      for (work = 1; work < world->num_players + 1; work ++)
	{
	  fseek (fptr, 0, SEEK_SET);
	  printf ("Appending 'The Other Bit' to Player %d's Messages...\n",
		  work);
	  sprintf (filename, "%s%s%02d_%03d.%s", world->path, PLYPLY_MESG_F,
		   work, world->turn, world->game);
	  
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
  
  for (work = 0; work < world->num_players; work ++)
    {
      printf ("Appending Player %d's Messages...\n", work);
      
      sprintf (filename, "%s%s%02d_%03d.%s", world->path, PLYPLY_MESG_F, work,
	       world->turn, world->game);
      if ((fptr = fopen (filename, "r")) != NULL)
	{
	  player_mesg (world, work,
		   "\n * Speaking to the Enemy in Turn %d:\n\n",
		   world->turn);
	  while (fgets (work_string, 140, fptr) != NULL)
	    player_mesg (world, work, "%s", work_string);
	  remove (filename);
	}
    }
  for (work = 0; work < world->num_players; work ++)
  {
    printf ("Appending Player %d's Errors...\n", work);

    sprintf (filename, "%s%s.%s%02d_%03d", world->path, world->game,
	     PLYERR_MESG_F, work, world->turn);
    
    if ((fptr = fopen (filename, "rt")) != NULL)
      {
	while (fgets (work_string, 140, fptr) != NULL)
	  player_mesg (world, work, "%s", work_string);
	fclose(fptr);
	remove (filename);
      }
    player_mesg (world, work, "(trailer)))\n");
  }
}
