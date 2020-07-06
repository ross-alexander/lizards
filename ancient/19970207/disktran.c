#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"

#include "lizgame.h"

char data_path [80],
     disk_data_path [80],
     email_data_path [80],
     disk_drive_path [80],
     game_code [5];

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

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
	     victory_points_switch = 0,
	     home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

int world_x,
    world_y,
    turn_number,
    number_spys;

spy_t *spy_list [MAX_SPYS];

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  char filename [80],
       string [256];

  unsigned int player;

  show_title ();

  printf ("LIZARDS! Transfer to Disk Utility. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 7)
  {
    strcpy (data_path, argv [1]);
    strcpy (disk_data_path, argv [2]);
    strcpy (email_data_path, argv [3]);
    strcpy (disk_drive_path, argv [4]);
    turn_number = atoi (argv [5]);
    strncpy (game_code, argv [6], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path disk_data_path\n"
	     "                                          email_data_path disk_drive_path\n"
	     "                                          turn_number game.\n"
	     "                             e.g. DISKTRAN C:\\ C:\\ D:\\ A:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  for (player = 1; player < players_in_game + 1; player ++)
    if (strcmp (player_name [player], "****") != 0)
      switch (player_output_mode [player])
      {
	case 'D':
	  printf ("Disk Player %s, Player %02d, Turn %03d in Game %s.\n",
	    player_name [player], player, turn_number, game_code);
	  printf ("Please Insert this Player's Disk into Drive %s.\n",
	    disk_drive_path);
	  printf ("Press SPACE when ready to transfer files to disk.\n\n");

	  getch ();

	  sprintf (filename, "DWO%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    disk_drive_path, filename);

	  printf ("Copying %s file to %s\n", filename, disk_drive_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DST%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    disk_drive_path, filename);

	  printf ("Copying %s file to %s\n", filename, disk_drive_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DSW%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    disk_drive_path, filename);

	  printf ("Copying %s file to %s\n", filename, disk_drive_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DSP%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    disk_drive_path, filename);

	  printf ("Copying %s file to %s\n", filename, disk_drive_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DWP%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    disk_drive_path, filename);

	  printf ("Copying %s file to %s\n", filename, disk_drive_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DPL%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    disk_drive_path, filename);

	  printf ("Copying %s file to %s\n", filename, disk_drive_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  printf ("\n");

	  break;
	case 'B':
	  printf ("Email Player %s, Player %02d, Turn %03d in Game %s.\n",
	    player_name [player], player, turn_number, game_code);

	  sprintf (filename, "DWO%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    email_data_path, filename);

	  printf ("Copying %s file to %s\n", filename, email_data_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DST%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    email_data_path, filename);

	  printf ("Copying %s file to %s\n", filename, email_data_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DSW%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    email_data_path, filename);

	  printf ("Copying %s file to %s\n", filename, email_data_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DSP%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    email_data_path, filename);

	  printf ("Copying %s file to %s\n", filename, email_data_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DWP%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    email_data_path, filename);

	  printf ("Copying %s file to %s\n", filename, email_data_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "DPL%02d%03d.%s", player, turn_number, game_code);
	  sprintf (string, "COPY %s%s %s%s > NUL:", disk_data_path, filename,
	    email_data_path, filename);

	  printf ("Copying %s file to %s\n", filename, email_data_path);

	  if (system (string) == -1)
	  {
	    printf ("FATAL ERROR: Unable to copy %s file to disk. Press SPACE.\n", filename);

	    getch ();

	    exit (EXIT_FAILURE);
	  }

	  printf ("\n");

	  break;
      }

  printf ("\nFinished!\n");

  exit (EXIT_SUCCESS);
}
