#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "strgfunc.h"

#include "lizgame.h"

char cover_letter [80];

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

spy_t *spy_list [MAX_SPYS];

int generic_on,
    world_x,
    world_y,
    turn_number,
    number_spys;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

int main (int argc, char *argv []);
char *brace_string (char *string);
char *ps_hack_braces_string (char *string);
void break_str (char *string, char *string2, int length);

int main (int argc, char *argv [])
{
  FILE *in_fptr,
       *out_fptr;

  char work_string [255],
       filename [20],
       out_filename [20];

  char *cover_letter_text [100]; /* a hundred lines of text max */

  float credit = 0.0;

  int work,
      work2,
      message_lines;

  show_title ();

  printf ("LIZARDS! POSTSCRIPT Cover Page Printer. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc != 5)
  {
    printf ("FATAL ERROR: Turn, Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path turn_number game doc_file.\n"
	     "                             e.g. PSPLYPAG C:\\ 1 A ONEWEEK.TXT\n");

    exit (EXIT_FAILURE);
  }

  strcpy (data_path, argv [1]);
  turn_number = atoi (argv [2]);
  strncpy (game_code, argv [3], 3);
  game_code [3] = NULL;
  strupr (game_code);
  strcpy (cover_letter, argv [4]);

  get_world ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  strcpy (player_code [0], "C-G");

  get_switch_list ();

  get_spy_list ();

  sprintf (out_filename, "%sLZGTURN.%s", data_path, game_code);

  if ((out_fptr = fopen (out_filename, "at")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", out_filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  fprintf (out_fptr, "/Gameletter {(%s)} def\n", game_code);
  fprintf (out_fptr, "/Turnnumber {(%d)} def\n", turn_number);
  fprintf (out_fptr, "/NTN {(%d)} def\n", turn_number + 1);
  fprintf (out_fptr, "/Date {(%s)} def\n", game_due_date);

  printf (" \n");

  if ((in_fptr = fopen (cover_letter, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", cover_letter);

    getch ();

    exit (EXIT_FAILURE);
  }

  message_lines = 0;

  while (fgets (work_string, 255, in_fptr) != NULL && message_lines < 99)
  {
    work_string [strlen (work_string) - 1] = NULL;

    if ((cover_letter_text [message_lines] = malloc (strlen (work_string) + 1))
      == NULL)
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    strcpy (cover_letter_text [message_lines], work_string);
    cover_letter_text [strlen (work_string)] = NULL;

    message_lines ++;
  }

  fclose (in_fptr);

  for (work = 0; work < players_in_game + 1; work ++)
    if (strcmp (player_name [work], "****") != 0)
    {
      printf ("Postscripting Player %d...\n", work);

      fprintf (out_fptr, "/saveobj save def\n");
      fprintf (out_fptr, "/Shadow_Code {(%s)} def\n", player_code [work]);

      fprintf (out_fptr, "PageOne\n");
      fprintf (out_fptr, "(%d) Player_Number\n", work);

      credit = atof (player_cred [work]);

      fprintf (out_fptr, "($ %0.2f) Credit\n", credit);

      fprintf (out_fptr, "Address\n");
      fprintf (out_fptr, "(%s) -18 Text\n", player_name [work]);
      fprintf (out_fptr, "(%s) -18 Text\n", player_road [work]);
      fprintf (out_fptr, "(%s) -18 Text\n", player_burb [work]);
      fprintf (out_fptr, "(%s) -18 Text\n", player_city [work]);
      fprintf (out_fptr, "(%s) -18 Text\n", player_country [work]);

      fprintf (out_fptr, "(%s) Clan_Full_Name\n", player_clan_name [work]);

      fseek (in_fptr, 0, SEEK_SET);

      fprintf (out_fptr, "Write [\nLinef\n", work_string);

      for (work2 = 0; work2 < message_lines; work2 ++)
      {
        strcpy (work_string, cover_letter_text [work2]);

	fprintf (out_fptr, "%s Linef\n", brace_string (work_string));
      }

      fprintf (out_fptr, "] format\n");

      fprintf (out_fptr, "pr\n");

      remove (filename);

      fprintf (out_fptr, "saveobj restore\n");
    }

  fprintf (out_fptr, "\x04");

  fclose (out_fptr);

  printf ("\nFinished!\n");

  return (EXIT_SUCCESS);
}

char *brace_string (char *string)
{
  char work_string [400],
       *ptr;

  ps_hack_braces_string (string);

  strcpy (work_string, "(");

  strtok (string, " ");

  strcat (work_string, string);
  strcat (work_string, ")");

  while (ptr = strtok (NULL, " "))
  {
    strcat (work_string, "(");
    strcat (work_string, ptr);
    strcat (work_string, ")");
  }

  strcpy (string, work_string);
  return (string);
}

char *ps_hack_braces_string (char *string)
{
  char work_string [400];

  int work,
      work2;

  work_string [0] = NULL;

  for (work = 0, work2 = 0; work < strlen (string); work ++, work2 ++)
  {
    if (string [work] == '(' || string [work] == ')')
      work_string [work2 ++] = '\\';

    work_string [work2] = string [work];
  }

  work_string [work2] = NULL;

  strcpy (string, work_string);

  return (string);
}

void break_str (char *string, char *string2, int length)
{
  int work;

  for (work = length; work > -1; work --)
    if (isspace (string [work]) || ispunct (string [work]))
    {
      strsplt (string2, string, work + 1);

      string [work] = '\n';

      string [work + 1] = NULL;

      return;
    }

  string2 [0] = NULL;
}
