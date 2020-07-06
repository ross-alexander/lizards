#define SECLEV_LINE 10

#define VER_STRING "ALPHA"

#define VALIDATED "VALIDU.BBS"
#define NOT_VALIDATED "INVALIDU.BBS"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "io_lib.h"
#include "doorbits.h"

extern int ANSI;

char path_to_sfdoor [80];

int main (int argc, char **argv);
int get_games_player_in (char *filename, char *player_name, char *player_pin);
void validate_user (char *sec_lev);
int open_vanilla_door (char *users_name, char *bbs_name, char *sysops_name,
	int *time_left, char *intro_name, char *def_name);
char **add_to_strlist ( char **strlist, char *entry );
char **free_strlist ( char **strlist );
int strlist_len ( char **strlist );

int main (int argc, char **argv)
{
  char player_name [80],
       pin [80],
       entered_pin [80],
       bbs_name [80],
       sysop_name [80];

  int security,
      timeleft,
      games_player_in = 0;

  printf ("Circle Games VALIDATE CUSTOMER Utility. %s Last compiled %s.\n",
	  VER_STRING, __DATE__);

  security = open_vanilla_door (player_name, bbs_name, sysop_name, &timeleft,
    "CGVALID", "CGVALID.DEF");

  strupr (player_name);

  writeln ("Greetings %s. Welcome to Circle Games Customer Validation.%n%n", player_name);

  if (security > 5)
  {
    writeln ("You are already validated as a FULL USER.%n%n");

    exit (EXIT_SUCCESS);
  }

  writeln ("Hang on, checking you're a Circle Games customer...");

  games_player_in += get_games_player_in ("AK_DATA.BBS", player_name, pin);
  games_player_in += get_games_player_in ("LZ_DATA.BBS", player_name, pin);
  games_player_in += get_games_player_in ("SL_DATA.BBS", player_name, pin);

  if (games_player_in > 0)
  {
    writeln ("found you!%n%nPlease enter your PIN number for validation:");

    input_line (entered_pin, 5);

    if (entered_pin [strlen (entered_pin) - 1] == '\n')
      entered_pin [strlen (entered_pin) - 1] = NULL;

    if (strcmp (pin, entered_pin) == 0)
    {
      validate_user (argv [1]);

      show_file (VALIDATED);

      tap_key ("Press SPACE.");
    } else {
      show_file (NOT_VALIDATED);

      tap_key ("Press SPACE.");
    }
  } else {
    writeln ("NOT FOUND.%n%n");

    show_file (NOT_VALIDATED);

    tap_key ("Press SPACE.");
  }

  exit (EXIT_SUCCESS);
}

int get_games_player_in (char *filename, char *player_name, char *player_pin)
{
  FILE *fptr;

  char work_string [256],
       full_name [256],
       game [80],
       player [80],
       turn [80],
       pin [80],
       credit [12];

  int games_in = 0;

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    writeln ("FATAL ERROR: Unable to read %s file. Press SPACE.%n",
      filename);

    NeedChar ();

    exit (EXIT_FAILURE);
  }

  while (fscanf (fptr, "%35[^,], %10[^,] , %3[^,], %3[^,], %3[^,], %10[^\n]\n",
    full_name, pin, game, player, turn, credit) != EOF)
  {
    strupr (full_name);

    if (strcmp (player_name, full_name) == 0)
    {
      strcpy (player_pin, pin);
      games_in ++;
    }
  }

  fclose (fptr);

  return (games_in);
}

void validate_user (char *sec_lev)
{
  FILE *fptr;

  char **line_list = NULL,
       string [256];

  int loop = 0;

  if (path_to_sfdoor [strlen (path_to_sfdoor) - 1] == '\n')
    path_to_sfdoor [strlen (path_to_sfdoor) - 1] = NULL;

  if ((fptr = fopen (path_to_sfdoor, "rt")) == NULL)
  {
    writeln ("FATAL ERROR: Unable to read %s file. Press SPACE.%n",
      path_to_sfdoor);

    NeedChar ();

    exit (EXIT_FAILURE);
  }

  /* load in SFDOORS.DAT */

  while (fgets (string, 255, fptr) != NULL)
    line_list = add_to_strlist ( line_list, string );

  fclose (fptr);

  /* write out SFDOORS.DAT, supplanting our own security level */

  if ((fptr = fopen (path_to_sfdoor, "wt")) == NULL)
  {
    writeln ("FATAL ERROR: Unable to write %s file. Press SPACE.%n",
      path_to_sfdoor);

    NeedChar ();

    exit (EXIT_FAILURE);
  }

  for (loop = 0; line_list [loop] != NULL; loop ++)
  {
    if (loop == SECLEV_LINE)
      fprintf (fptr, "%s\n", sec_lev);
    else
      fprintf (fptr, "%s", line_list [loop]);
  }

  fclose (fptr);

  /* free the table */

  line_list = free_strlist (line_list);

  fclose (fptr);
}

int open_vanilla_door (char *users_name, char *bbs_name, char *sysops_name,
	int *time_left, char *intro_name, char *def_name)
{
  char path_to_bulletin[65], work_string [13];

  int security;

  get_door_def (bbs_name, path_to_sfdoor, sysops_name, path_to_bulletin,
		def_name);

  security = get_user_sfdoors (users_name, time_left, path_to_sfdoor);

  if (*bbs_name != NULL) bbs_name[strlen(bbs_name)-1] = '\0';
  if (*sysops_name != NULL) sysops_name[strlen(sysops_name)-1] = '\0';

  if (ANSI)
  {
    fnmerge (work_string,"","",intro_name, ".CLR");
    show_file (work_string);
  }else{
    fnmerge (work_string, "", "", intro_name, ".BBS");
    show_file (work_string);
  }

  return (security);
}

/*
   add_to_strlist

   adds the passed entry string to the passed strlist (a dynamically
   allocated list of pointers to dynamically allocated strings) by either
   reallocing an existing list and adding the string, or mallocing a new
   list and adding the string. Passing a strlist of NULL starts a new list.

   eg. list = add_to_strlist (list, "TESTING..."); adds to existing list

   eg. list = add_to_strlist (NULL, "TESTING..."); creates new list

   add_to_strlist returns a pointer to the modified list.
*/

char **add_to_strlist ( char **strlist, char *entry )
{
  int elements;

  if ( strlist == NULL ) /* put entry in NEW table */
  {
    elements = 1;

    if (( strlist = malloc ( sizeof ( char * ) * 2 )) == NULL )
      return ( NULL ); /* failed to malloc new list, give them NULL back */
  } else {
    elements = strlist_len ( strlist ) + 1;

    if (( strlist = realloc ( strlist, sizeof ( char * ) * ( elements + 1))) == NULL )
      return ( NULL ); /* failed to realloc list, give them NULL back */
  }

  if (( strlist [ elements - 1 ] = malloc ( sizeof ( char ) *
    ( strlen ( entry ) + 1 ))) == NULL )
    return ( NULL ); /* unable to malloc space in list for new entry */

  strcpy ( strlist [ elements - 1 ], entry );

  strlist [ elements ] = NULL; /* table terminating NULL */

  return ( strlist ); /* give them back new copy of strlist */
}

/*
   strlist_len

   counts and returns the number of entries in the strlist passed to it.
*/

int strlist_len ( char **strlist )
{
  int element;

  for ( element = 0; strlist [ element ] != NULL; element ++ );

  return ( element );
}

/*
   free_strlist

   frees the memory used to hold each string in the strlist as well as the
   memory used by the strlist itself. Returns NULL so you can easily kill
   lists.

   eg. list = free_strlist (list);
*/

char **free_strlist ( char **strlist )
{
  int element = 0;

  if ( strlist == NULL ) return ( NULL ); /* do nothing as given empty list */

  for ( element = 0; strlist [ element ] != NULL; element ++ )
  {
    free ( strlist [ element ] ); /* free each element */

    strlist [ element ] = NULL;
  }

  free ( strlist ); /* free list itself */

  return ( NULL );
}