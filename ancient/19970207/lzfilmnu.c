#define MAX_GAMES_IN 100

#define VER_STRING "ALPHA"

#include <conio.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io_lib.h"
#include "doorbits.h"

char upload_data_path [80],
     download_data_path [80];

int main (void);
void get_data_paths (void);
void check_turn_downloads (char *player_name, int games_in,
			   char game_codes [MAX_GAMES_IN][4],
			   int player_numbers [MAX_GAMES_IN],
			   int player_turns [MAX_GAMES_IN]);
int get_games_player_in (char *player_name, char game_codes [MAX_GAMES_IN][4],
			 int player_numbers [MAX_GAMES_IN],
			 int player_turns [MAX_GAMES_IN],
			 float *player_credit);

int main (void)
{
  FILE *fptr;

  char player_name [80],
       player_games [MAX_GAMES_IN][4],
       code [80],
       filename [80],
       string [256],
       bbs_name [80],
       sysop_name [80];

  float player_credit;

  int dsz_status,
      element,
      loop,
      timeleft,
      games_in,
      player_turns [MAX_GAMES_IN],
      player_numbers [MAX_GAMES_IN];

  printf ("LIZARDS! BBS Order Entry Utility. %s Last compiled %s.\n",
	  VER_STRING, __DATE__);

  open_door (player_name, bbs_name, sysop_name, &timeleft,
    "lzfilmnu", "lzfilmnu.def");

  get_data_paths ();

  strupr (player_name);

  games_in = get_games_player_in (player_name, player_games, player_numbers,
				  player_turns, &player_credit);

  writeln ("Greetings %s. Welcome to Circle Games's LIZARDS! Door.%n%n", player_name);

  if (games_in == 0)
  {
    writeln ("Sorry, the program can't find you in any LIZARDS! games.%n"
	     "Please contact Circle Games for further details.%n"
	     "Press ENTER to return to the BBS.%n");

    NeedChar ();

    exit (EXIT_FAILURE);
  }

  if (player_credit < 0)
  {
    sprintf (string, "$%+0.2f", player_credit);

    writeln ("Sorry, your account has a credit of %s.%n"
	     "You can only receive turns and enter orders if your account is positive.%n"
	     "Please contact Circle Games for further details.%n"
	     "Press ENTER to return to the BBS.%n",
	     string);

    NeedChar ();

    exit (EXIT_FAILURE);
  }

  /* check for turns to download */

  check_turn_downloads (player_name, games_in, player_games, player_numbers,
			player_turns);

  while (1)
  {
    writeln ("%nYou're presently playing in the following LIZARDS! Games:%n%n");

    for (loop = 0; loop < games_in; loop ++)
    {
      writeln ("Player %u in Game %s (Turn %u)       ",
	player_numbers [loop], player_games [loop], player_turns [loop]);

      if ((loop + 1) % 2 == 0 && loop > 0) writeln ("%n");
    }

    if (loop % 2 != 0) writeln ("%n");

    writeln ("%n");

    writeln ("Select the Game Code you wish to UPLOAD orders for (ENTER = ABORT): ");

    input_line (code, 3);

    strupr (code);

    writeln ("%n");

    if (code [0] == NULL) break;

    for (loop = 0; loop < games_in; loop ++)
      if (strcmp (player_games [loop], code) == 0)
      {
	sprintf (string, "DOR%02d%03d.%s", player_numbers [loop],
	  player_turns [loop] + 1, player_games [loop]);

	writeln ("Ready to RECEIVE %s order via ZMODEM.%n",
	  filename);

	writeln ("PRESS ENTER KEY TO BEGIN ZMODEM TRANSFER TO BBS.%n");

	NeedChar ();

	sprintf (filename, "%sDOR%02d%03d.%s", upload_data_path,
	  player_numbers [loop], player_turns [loop] + 1,
	  player_games [loop]);

	sprintf (string, "rz %s%s", filename);

	dsz_status = spawnl (P_WAIT, "dsz.com", "dsz.com", string, NULL);

	if (dsz_status != 0)
	  writeln ("UPLOAD FAILED! File NOT saved.%n");
	else
	  writeln ("UPLOAD SUCCEEDED! File saved.%n");

	break;
      }

    if (loop == games_in)
      writeln ("Sorry! You're not in that game.%n");
  }

  writeln ("Returning to BBS...");

  exit (EXIT_SUCCESS);
}

void get_data_paths (void)
{
  FILE *fptr;

  if ((fptr = fopen ("LF_PATHS.TXT", "rt")) == NULL)
  {
    writeln ("FATAL ERROR: Unable to read %s file. Press ENTER.%n",
      "LF_PATHS.TXT");

    NeedChar ();

    exit (EXIT_FAILURE);
  }

  fscanf (fptr, "%70[^\n]\n%70[^\n]\n", upload_data_path,
    download_data_path);

  fclose (fptr);
}

void check_turn_downloads (char *player_name, int games_in,
			   char game_codes [MAX_GAMES_IN][4],
			   int player_numbers [MAX_GAMES_IN],
			   int player_turns [MAX_GAMES_IN])
{
  FILE *fptr;

  char filename [80],
       string [256];

  int loop,
      loop_two,
      element,
      dsz_status;

  writeln ("Checking for LIZARDS! turn reports to send you...%n");

  for (loop = 0; loop < games_in; loop ++)
  {
    sprintf (string, "%sDPL%02d%03d.%s", download_data_path,
      player_numbers [loop], player_turns [loop] + 1, game_codes [loop]);

    if ((fptr = fopen (string, "rt")) != NULL)
    {
      fclose (fptr);

      writeln ("Found files waiting for you! Sending via Zmodem.%n");

      writeln ("PRESS ENTER KEY TO BEGIN ZMODEM FILE TRANSFER TO YOU!%n");

      NeedChar ();

      /* archive the files */

      sprintf (string, "PKZIP %sLPZ%02d%03d.%s %s???%02d%03d.%s",
	download_data_path, player_numbers [loop], player_turns [loop] + 1,
	game_codes [loop], download_data_path, player_numbers [loop],
	player_turns [loop] + 1, game_codes [loop]);

      if ((system (string)) == -1)
      {
	writeln ("FAILED TO ARCHIVE FILES CORRECTLY. Download Aborted.%n");

	sprintf (string, "%sLPZ%02d%03d.%s", download_data_path,
	  player_numbers [loop], player_turns [loop] + 1, game_codes [loop]);

	remove (string);
      } else {
	sprintf (filename, "%sLPZ%02d%03d.%s", download_data_path,
	  player_numbers [loop], player_turns [loop] + 1, game_codes [loop]);

	sprintf (string, "sz %s", filename);

	dsz_status = spawnl (P_WAIT, "dsz.com", "dsz.com", string, NULL);

	if (dsz_status == 0)
	{
	  writeln ("Successfully transmitted archive file. Deleting files off BBS.%n");

	  sprintf (string, "DEL %s???%02d%03d.%s > NUL:",
	    download_data_path, player_numbers [loop],
	    player_turns [loop] + 1, game_codes [loop]);

	  system (string);
	} else {
	  writeln ("FAILED TO TRANSMIT FILES CORRECTLY. Leaving file on BBS.%n");
	}
      }
    }
  }
}

int get_games_player_in (char *player_name, char game_codes [MAX_GAMES_IN][4],
			 int player_numbers [MAX_GAMES_IN],
			 int player_turns [MAX_GAMES_IN],
			 float *player_credit)
{
  FILE *fptr;

  char work_string [256],
       full_name [256],
       game [80],
       player [80],
       turn [80],
       credit [12];

  int games_in = 0;

  if ((fptr = fopen ("LZ_DATA.BBS", "rt")) == NULL)
  {
    writeln ("FATAL ERROR: Unable to read %s file. Press ENTER.%n", "LZ_DATA.BBS");

    NeedChar ();

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
      player_turns [games_in] = atoi (turn);
      *player_credit = atof (credit);

      games_in ++;
    }
  }

  fclose (fptr);

  return (games_in);
}
