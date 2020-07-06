#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"

#include "hyplink.h"
#include "hyphelpf.h"

char data_path [255],
     game_code [5];

int disk_player,
    world.turn;

int main (int argc, char **argv);
int find_start_marker (FILE *fptr, char *start_marker);
void view_file (FILE *fptr, char *end_marker);

int main (int argc, char **argv)
{
  FILE *fptr,
       *help_fptr;

  char string [256],
       head_line [256],
       **help_index;

  int option;

  unsigned int lines_so_far = 2;

  clrscr ();

  cprintf ("\n\rLIZARDS! Disk Turn Show Utility. %s Last compiled %s.\n\r",
    VER_STRING, __DATE__);

  if (argc == 5)
  {
    strcpy (data_path, argv [1]);
    world.turn = atoi (argv [2]);
    strncpy (game_code, argv [3], 3);
    game_code [3] = NULL;
    strupr (game_code);
    disk_player = atoi (argv [4]);
  } else {
    cprintf ("FATAL ERROR: Game and Turn Number not specified on command line!\n\r"
	     "             Command Line Format: EXEFILE data_path world.turn game player.\n\r"
	     "                             e.g. TURNSHOW A:\\ 6 A 5\n\r");

    exit (EXIT_FAILURE);
  }

  sprintf (string, "%sDPL%02d%03d.%s", data_path, disk_player, world.turn,
    game_code);

  if ((fptr = fopen (string, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file.\n", string);

    

    exit (EXIT_FAILURE);
  }

  help_index = open_help_file (&help_fptr, "TURNSHOW.HLP", "TURNSHOW.IDX");

  do
  {
    clrscr ();

    cprintf ("\n\rLIZARDS! Disk Turn Show Utility. %s Last compiled %s.\n\r\n\r",
      VER_STRING, __DATE__);

    cprintf ("  Viewing Turn Report for Player %02d, Turn %03d in Game %s.\n\r\n\r",
      disk_player, world.turn, game_code);

    cprintf ("  Select Show Method:\n\r\n\r");
    cprintf ("    T - Turn Report Information Section.\n\r");
    cprintf ("    W - World Phase Section.\n\r");
    cprintf ("    A - Artillery Phase Section.\n\r");
    cprintf ("    M - Movement Phase Section.\n\r");
    cprintf ("    D - Development Phase Section.\n\r");
    cprintf ("    C - Clan Listing Section.\n\r");
    cprintf ("    R - Clan Ranking Section.\n\r");
    cprintf ("    S - Speaking to the Enemy Section.\n\r");
    cprintf ("    E - Error Section.\n\r");
    cprintf ("    V - View ENTIRE Report.\n\r");
    cprintf ("    ? - Help.\n\r");
    cprintf ("    X - Exit Show Utility.\n\r\n\r");
    cprintf ("  Option: ");

    option = toupper (getche ());

    switch (option)
    {
      case 'T':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * TURN REPORT INFORMATION:"))
	  view_file (fptr, " * WORLD PHASE (all Den Growth and Settling).");
	break;
      case 'W':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * WORLD PHASE (all Den Growth and Settling)."))
	  view_file (fptr, " * ARTILLERY PHASE (all Chants, Lobs and Free orders).");
	break;
      case 'A':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * ARTILLERY PHASE (all Chants, Lobs and Free orders)."))
	  view_file (fptr, " * MOVEMENT PHASE (all Moves, Splits, Sails and Sneaks orders).");
	break;
      case 'M':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * MOVEMENT PHASE (all Moves, Splits, Sails and Sneaks orders)."))
	  view_file (fptr, " * DEVELOPMENT PHASE (all Builds, Destroy, Recruit and Give orders).");
	break;
      case 'D':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * DEVELOPMENT PHASE (all Builds, Destroy, Recruit and Give orders)."))
	  view_file (fptr, " * Clan Listing for");
	break;
      case 'C':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * Clan Listing for"))
	  view_file (fptr, " * Clans Ranking at end of Turn");
	break;
      case 'R':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * Clans Ranking at end of Turn"))
	  view_file (fptr, " * Speaking to the Enemy in Turn");
	break;
      case 'S':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * Speaking to the Enemy in Turn"))
	  view_file (fptr, " * LIZARDS! ORDER ERRORS");
	break;
      case 'E':
	fseek (fptr, 0, SEEK_SET);
	if (find_start_marker (fptr, " * LIZARDS! ORDER ERRORS"))
	  view_file (fptr, NULL);
	break;
      case 'V':
	fseek (fptr, 0, SEEK_SET);
	view_file (fptr, NULL);
	break;
      case '?':
	/* enter help */

	enter_help (help_fptr, help_index);

	break;
    }
  } while (option != 'X');

  fclose (fptr);

  exit (EXIT_SUCCESS);
}

int find_start_marker (FILE *fptr, char *start_marker)
{
  char string [256];

  long string_marker = 0;

  string_marker = ftell (fptr);

  while (fgets (string, 255, fptr) != NULL)
  {
    if (strncmp (string, start_marker, strlen (start_marker)) == 0)
    {
      fseek (fptr, string_marker, SEEK_SET);

      return (1);
    }

    string_marker = ftell (fptr);
  }

  return (0);
}

void view_file (FILE *fptr, char *end_marker)
{
  char string [256],
       head_line [80];

  int option;

  unsigned int lines_so_far = 0;

  clrscr ();

  while (fgets (string, 255, fptr) != NULL)
  {
    if (end_marker != NULL)
      if (strncmp (string, end_marker, strlen (end_marker)) == 0) break;

    if (strncmp (string, " * ", 3) == NULL)
    {
      memset (head_line, (int) '-', strlen (string) - 4);
      head_line [strlen (string) - 4] = NULL;

      cprintf ("%s\n\r%s\r%s\n\r", head_line, &string [3], head_line);

      lines_so_far += 3;
    } else {
      if (strlen (string) > 80)
      {
	char *back_ptr;

	back_ptr = &string [79];

	while (*back_ptr != ' ' && back_ptr != string) back_ptr --;

	if (back_ptr != string)
	{
	  back_ptr [0] = NULL;

	  cprintf ("%s\n\r", string);
	  cprintf ("   %s\r", &back_ptr [1]);

	  lines_so_far += 2;
	} else {
	  cprintf ("%s\r", string);

	  lines_so_far +=2;
	}
      } else {
	cprintf ("%s\r", string);

	lines_so_far ++;
      }
    }

    if (lines_so_far > 21)
    {
      textattr (BLACK + (LIGHTGRAY << 4));
      cprintf ("-- MORE --to continue, 'M' for menu.");
      textattr (LIGHTGRAY + (BLACK << 4));

      option = toupper (getch ());

      gotoxy (1, wherey ());
      clreol ();

      lines_so_far = 0;

      if (option == 'M') return;
    }
  }

  if (end_marker == NULL)
  {
    textattr (BLACK + (LIGHTGRAY << 4));
    cprintf ("END OF TURN REPORT.for menu.");
    textattr (LIGHTGRAY + (BLACK << 4));

    
  } else {
    textattr (BLACK + (LIGHTGRAY << 4));
    cprintf ("END OF SECTION.for menu.");
    textattr (LIGHTGRAY + (BLACK << 4));

    
  }
}