#define MAX_GAMES_IN 100

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"

char upload_data_path [80],
     download_data_path [80],
     data_path [80];

int main (void);
void get_data_paths (void);
void show_title (void);
void get_sfdoors_file (char *fullname, int *timeleft);
int get_games_player_in (char *player_name, char game_codes [MAX_GAMES_IN][4],
			 int player_numbers [MAX_GAMES_IN],
			 int player_turns [MAX_GAMES_IN],
			 float *player_credit);

int main (void)
{
  char player_name [80],
       player_games [MAX_GAMES_IN][4],
       code [80],
       string [256];

  int loop,
      timeleft,
      games_in,
      player_turns [MAX_GAMES_IN],
      player_numbers [MAX_GAMES_IN],
      shell_f;

  float player_credit;

  printf ("LIZARDS! BBS Order Entry Utility. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  kickback:

  printf ("\x1B[2J");

  show_title ();

  get_sfdoors_file (player_name, &timeleft);

  get_data_paths ();

  games_in = get_games_player_in (player_name, player_games, player_numbers,
				  player_turns, &player_credit);

  printf ("Greetings %s. Welcome to Circle Games's LIZARDS! Order Entry Door.\n\n", player_name);

  if (games_in == 0)
  {
    printf ("Sorry, the Order Entry Utility can't find you in any LIZARDS! games.\n"
            "If you have an account with Circle, check it's in the positive.\n\n"
	    "Press ENTER to return to the BBS.");

    getch ();

    exit (EXIT_FAILURE);
  }

  printf ("You're presently playing in the following LIZARDS! Games:\n\n");

  for (loop = 0; loop < games_in; loop ++)
  {
    printf ("Player %02d in Game %-3s (Turn %02d)       ",
      player_numbers [loop], player_games [loop], player_turns [loop]);

    if ((loop + 1) % 2 == 0 && loop > 0) printf ("\n");
  }

  if (loop % 2 != 0) printf ("\n");

  shell_f = 0;

  while (1)
  {
    printf ("\n\x1B[s");

    printf ("Select the Game Code you wish to enter orders for (ENTER = ABORT): \x1B[K");

    strupr (gets (code));

    printf ("\n");

    if (code [0] == NULL) break;

    for (loop = 0; loop < games_in && !shell_f; loop ++)
      if (strcmp (player_games [loop], code) == 0)
      {
	FILE *fptr;

	if ((fptr = fopen ("ENTERORD.BAT", "wt")) == NULL)
	{
	  printf ("FATAL ERROR: Unable to write %s file. Press ENTER.\n",
	    "ENTERORD.BAT");

	  getch ();

	  exit (EXIT_FAILURE);
	}

	fprintf (fptr, "doorway SYS /A:OFF /B:MSZ /G:ON /M:30 /O:T /P:ORDERDOR.EXE %s %s %d %s %d",
	  data_path, upload_data_path, player_turns [loop], player_games [loop], player_numbers [loop]);

	fclose (fptr);

	exit (EXIT_SUCCESS);
      }

    printf ("Sorry! You're not in that game.");

    printf ("\x1B[u");
  }

  printf ("Returning to BBS...\x1B[K");

  exit (EXIT_SUCCESS);
}

void get_data_paths (void)
{
  FILE *fptr;

  if ((fptr = fopen ("LZ_PATHS.TXT", "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press ENTER.%n",
      "LZ_PATHS.TXT");

    getch ();

    exit (EXIT_FAILURE);
  }

  fscanf (fptr, "%70[^\n]\n%70[^\n]\n%70[^\n]\n", upload_data_path,
    download_data_path, data_path);

  fclose (fptr);
}

void show_title (void)
{
  printf ("           (c) Copyright, 1991,92,93. LIZARDS! (TM), 1991,92,93.\n"
    "                      Programmed by Stuart Nicholson.\n"
    "        Contact Circle Games, P O Box 105320 Auckland, New Zealand.\n"
    "                          Phone (09) 358 5378.\n\n");
}


void get_sfdoors_file (char *fullname, int *timeleft)
{
  FILE *fptr;

  char work_string [80];

  if ((fptr = fopen ("SFDOORS.DAT", "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press ENTER.\n", "SFDOORS.DAT");

    getch ();

    exit (EXIT_FAILURE);
  }

  fgets (work_string, 80, fptr); /* user number, ignore */
  fgets (fullname, 80, fptr);
  fgets (work_string, 80, fptr); /* password, ignore */
  fgets (work_string, 80, fptr); /* first name, ignore */
  fgets (work_string, 80, fptr); /* baud rate, ignore */
  fgets (work_string, 80, fptr); /* comm port, ignore */
  fgets (work_string, 80, fptr); /* time left */
  *timeleft = atoi (work_string);

  fullname [strlen (fullname) - 1] = NULL; /* strip return */
  strupr (fullname);

  fclose (fptr);
}

int get_games_player_in (char *player_name, char game_codes [MAX_GAMES_IN][4],
			 int player_numbers [MAX_GAMES_IN],
			 int player_turns [MAX_GAMES_IN],
			 float *player_credit)
{
  FILE *fptr;

  char work_string [256],
       first_name [80],
       last_name [80],
       full_name [256],
       game [80],
       player [80],
       turn [80],
       credit [80];

  int games_in = 0;

  if ((fptr = fopen ("LZ_DATA.BBS", "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press ENTER.\n", "LZ_DATA.BBS");

    getch ();

    exit (EXIT_FAILURE);
  }

  while (fscanf (fptr, "%35[^,], %*[^,] , %3[^,], %3[^,], %3[^,], %10[^\n]\n", full_name, game, player,
    turn, credit) != EOF)
  {
    strupr (full_name);

    if (strcmp (player_name, full_name) == 0)
    {
      strcpy (game_codes [games_in], game);
      player_numbers [games_in] = atoi (player);
      player_turns [games_in] = atoi (turn) + 1;
      *player_credit = atof (credit);

      games_in ++;
    }
  }

  fclose (fptr);

  return (games_in);
}
