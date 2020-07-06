#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "lizgame.h"

char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_clan_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char players_in_game;

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_cred [MAX_PLAYERS + 1][10];
char player_output_mode [MAX_PLAYERS + 1];
char game_due_date [20];

int turn_number,
    world_x,
    world_y;

unsigned int spawn_turn_switch = 0,
             capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

spy_t *spy_list [MAX_SPYS];
int number_spys;

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  FILE *fptr,
       *fptr2;

  char work_string [140],
       filename [20],
       number [20];

  int work;

  show_title ();

  printf ("LIZARDS! Message Append Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc != 4)
  {
    printf ("FATAL ERROR: Turn Number and Game not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path turn_number game.\n"
	     "                             e.g. MESSAPND C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  data_path = LzCheckDir(argv[1], 0);
  turn_number = atoi (argv[2]);
  game_code = (char*)malloc(strlen(argv[3] + 1));
  strcpy (game_code, argv[3]);
  strupr (game_code);

  get_player_list ();

  if (players_in_game == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  /* append 'the other bit' (if there is one) in player 0's message area to
     ALL the other players! */

  sprintf (filename, "%s%s00_%03d.%s", data_path, PLYPLY_MESG_F, turn_number,
    game_code);

  if ((fptr = fopen (filename, "rt")) != NULL)
  {
    for (work = 1; work < players_in_game + 1; work ++)
    {
      fseek (fptr, 0, SEEK_SET);

      printf ("Appending 'The Other Bit' to Player %d's Messages...\n", work);

      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F, work,
        turn_number, game_code);

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

  for (work = 0; work < players_in_game + 1; work ++)
  {
    printf ("Appending Player %d's Messages...\n", work);

    sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F, work,
      turn_number, game_code);

    if ((fptr = fopen (filename, "rt")) != NULL)
    {
      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, work,
        turn_number, game_code);

      if ((fptr2 = fopen (filename, "at")) == NULL)
      {
        fprintf (stderr, "FATAL ERROR: Unable to append to %s file.\n", filename);
	exit (EXIT_FAILURE);
      }

      fprintf (fptr2, "\n * Speaking to the Enemy in Turn %d:\n\n",
	       turn_number);

      while (fgets (work_string, 140, fptr) != NULL)
	fprintf (fptr2, "%s", work_string);

      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F, work,
        turn_number, game_code);

      remove (filename);
    }
  }

  for (work = 0; work < players_in_game + 1; work ++)
  {
    printf ("Appending Player %d's Errors...\n", work);

    sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYERR_MESG_F, work,
      turn_number, game_code);

    if ((fptr = fopen (filename, "rt")) != NULL)
    {
      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, work,
        turn_number, game_code);

      if ((fptr2 = fopen (filename, "at")) == NULL)
      {
	fprintf (stderr, "FATAL ERROR: Unable to append to %s file.\n", filename);
	exit (EXIT_FAILURE);
      }

      while (fgets (work_string, 140, fptr) != NULL)
	fprintf (fptr2, "%s", work_string);
      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYERR_MESG_F, work,
        turn_number, game_code);

      remove (filename);
    }
  }

  printf ("\nFinished!\n");

  return (EXIT_SUCCESS);
}
