#define LIZMENU_LOG_FILE "LIZMENU.LOG"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "compat/cio.h"

#define NUMERIC 0
#define ALPHA 1
#define ALPHANUMERIC 2

#define HELP_KEY 1
#define SAVE_KEY 2
#define CANCEL_KEY 3
#define FINISH_KEY 4

#define TITLE_WINDOW 1, 1, 78, 13, COLOR_RED, COLOR_BLACK
#define MENU_WINDOW 1, 12, 78, 23, COLOR_RED, COLOR_BLACK

#include "lizard.h"
#include "map.h"
#include "template.h"

#include "filefunc.h"
#include "strgfunc.h"
#include "tempfunc.h"

#include "lizgame.h"

char *game_code = 0;

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

char *disk_drive_path;

spy_t *spy_list [MAX_SPYS];
int number_spys;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
	     home_owned_worth = 0,
             home_victory_points = 0,
             give_switch = 0;

WINDOW *titleWindow, *menuWindow;

field_t **player_template;

unsigned int player_template_fields;

void show_lizmenu_title (void);
void base_menu (void);
void show_base_menu (void);
void create_game_menu (void);
void show_create_game_menu (void);
void run_game_menu (void);
void show_run_game_menu (void);
void edit_game_menu (void);
void show_edit_game_menu (void);
void game_menu (void);
void show_game_menu (void);
void create_random_game (void);
void create_handmade_game (void);
void create_custom_startup_game (void);
void custom_start_game (void);
void view_custom_startup_log (void);
void enter_orders (void);
void run_turn (void);
void output_to_log (int turn);
void print_turn (void);
int postscript_turn (int turn);
int ascii_turn (int turn);
void print_world_map (void);
int backup_game (int turn);
int unbackup_game (void);
void restore_game (void);
void edit_world (void);
void edit_spys (void);
void change_player (void);
void list_games (void);
int game_list_sort (const void *one, const void *two);
void delete_game (void);
void set_game_turn (void);
int select_game(char *prompt);
int select_lizard_colour (char *prompt);
int game_code_exists (void);
void get_game_player_list (void);
int get_world_turn (void);
void save_world_turn (int turn);
void wipe_player_list (void);
void show_player_list (void);
void show_player_form (void);
void full_screen (void);
void restore_screen (void);
void get_data_paths (const char*);

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

extern int optind;
extern char *optarg;

int main (int argc, char *argv[])
{
  char *pathfile = "LM_PATHS.TXT";
  int opt;
  
  while((opt = getopt(argc, argv, "p:h:")) != EOF)
    switch(opt)
      {
      case 'p':
	pathfile = optarg;
	break;
      case 'h':
	printf("Lizards: [-p data path]\n");
	exit(0);
      }

  get_data_paths ("LM_PATHS.TXT");
  player_template = load_template ("PLAYFORM.TEM", &player_template_fields);

  NcursesInit(1); /* use color = 1 */
  titleWindow = NcursesOpenWindow(TITLE_WINDOW, 0);
  cWindow = titleWindow;
  show_lizmenu_title ();
  menuWindow = NcursesOpenWindow (MENU_WINDOW, 0);
  cWindow = menuWindow;
  wrefresh(stdscr);
  wrefresh(titleWindow);

  base_menu ();
  NcursesExit(); 
  free_template (player_template);
  return (EXIT_SUCCESS);
}

void show_lizmenu_title (void)
{
  cprintf ( "        ::::    :::: ::::::   :::   ::::::. :::::   .::::.  ::  TM\n\r"
    "         ::      ::  ``  ::  :: ::   ::  ::  :: ::  ::  :: ::::\n\r"
    "         ::      ::     ::  ::   ::  ::  ::  ::  :: ::     ::::\n\r"
    "         ::      ::    ::   :::::::  :::::'  ::  :: `::::.  ::\n\r"
    "         ::   .  ::   ::    ::   ::  :: :::  ::  ::     ::  ::\n\r"
    "         ::  .:  ::   :: .. ::   ::  ::  ::  :: ::  ::  ::  ..\n\r"
    "        ::::::: :::: :::::: ::   :: :::  :: :::::   `::::'  ::\n\r\n\r"
    "                        (c) Copyright, 1991,92,93.\n\r"
    "                        LIZARDS! (TM), 1991,92,93.");
}

void base_menu ()
{
  int option;

  /* assumed to be in menu screen area! */

  show_base_menu ();
  wrefresh(menuWindow);
  do
  {
/*    gotoxy (6, 10); */
    wmove(menuWindow, 10, 6);
    cprintf ("Option Number: ");
    option = toupper (getch());

    switch (option)
    {
      case '1':
	create_game_menu ();
        show_base_menu ();
        break;
      case '2':
	run_game_menu ();
        show_base_menu ();
        break;
      case '3':
        edit_game_menu ();
        show_base_menu ();
        break;
      case '4':
        game_menu ();
        show_base_menu ();
        break;
    }
  } while (option != 'Q');
}

void show_base_menu (void)
{
  clrscr ();

  cprintf ("\n\r"
	   "     LIZARDS! Main Menu.\n\r\n\r"
	   "       1. Create Game (Random, Hand Made and Custom Startups).\n\r"
	   "       2. Run Game (Enter Orders,Run Turn,Print Reports and Print Map).\n\r"
	   "       3. Edit Game (Edit World, Edit Spies and Change Players).\n\r"
	   "       4. LIZARDS! Games (List Games, Delete Game, Restore Game & Set Turn).\n\r"
	   "       Q. Quit LIZARDS! Menu Program.\n\r");
}

void create_game_menu (void)
{
  char option;
  show_create_game_menu ();

  do
  {
/*    gotoxy (6, 11); */
    wmove(menuWindow, 11, 6);

    cprintf ("Option Number: ");

    option = getch ();

    switch (option)
    {
      case '1':
	create_random_game ();
	show_create_game_menu ();
	break;
      case '2':
	create_handmade_game ();
	show_create_game_menu ();
	break;
      case '3':
	create_custom_startup_game ();
	show_create_game_menu ();
	break;
      case '4':
	custom_start_game ();
	show_create_game_menu ();
	break;
      case '5':
	view_custom_startup_log ();
	show_create_game_menu ();
	break;
    }
  } while (option != '0');
}

void show_create_game_menu (void)
{
  clrscr ();

  cprintf ("\n\r"
	   "     LIZARDS! Create Game Menu.\n\r\n\r"
	   "       1. Create Random Game.\n\r"
	   "       2. Create Hand Made Game.\n\r"
	   "       3. Create Custom Startup Game.\n\r"
	   "       4. Enter Custom Startup Templates.\n\r"
	   "       5. View Custom Startup Log File.\n\r"
	   "       0. Quit Menu.\n\r");
}

void run_game_menu (void)
{
  char option;

  show_run_game_menu ();

  do
  {
    gotoxy (6, 10);

    cprintf ("Option Number: ");

    option = getche ();

    switch (option)
    {
      case '1':
        enter_orders ();
	show_run_game_menu ();
        break;
      case '2':
        run_turn ();
	show_run_game_menu ();
        break;
      case '3':
	print_turn ();
	show_run_game_menu ();
	break;
      case '4':
	print_world_map ();
	show_run_game_menu ();
	break;
    }
  } while (option != '0');
}

void show_run_game_menu (void)
{
  clrscr ();

  cprintf ("\n\r"
	   "     LIZARDS! Run Game Menu.\n\r\n\r"
	   "       1. Enter Orders.\n\r"
	   "       2. Run Turn.\n\r"
	   "       3. Print Reports.\n\r"
	   "       4. Print World Maps.\n\r"
	   "       0. Quit Menu.\n\r");
}

void edit_game_menu (void)
{
  int option;
  show_edit_game_menu ();
  do
  {
    gotoxy (6, 10);
    cprintf ("Option Number: ");

    option = getche ();
    option = getch();

    switch (option)
    {
      case '1':
        edit_world ();
        show_edit_game_menu ();
        break;
      case '2':
        edit_spys ();
        show_edit_game_menu ();
        break;
      case '3':
        change_player ();
        show_edit_game_menu ();
        break;
    }
  } while (option != '0');
}

void show_edit_game_menu (void)
{
  clrscr ();

  cprintf ("\n\r"
           "     LIZARDS! Edit Game Menu.\n\r\n\r"
           "       1. Edit World.\n\r"
           "       2. Edit Spies.\n\r"
           "       3. Change Player.\n\r\n\r"
           "       0. Quit Menu.\n\r");
}

void game_menu (void)
{
  char option;

  show_game_menu ();

  do
  {
    gotoxy (6, 10);

    cprintf ("Option Number: ");

    option = getche ();

    switch (option)
    {
      case '1':
        list_games ();
        show_game_menu ();
        break;
      case '2':
        delete_game ();
        show_game_menu ();
        break;
      case '3':
        restore_game ();
        show_game_menu ();
        break;
      case '4':
        set_game_turn ();
        show_game_menu ();
        break;
    }
  } while (option != '0');
}

void show_game_menu (void)
{
  clrscr ();

  cprintf ("\n\r"
           "     LIZARDS! Game Menu.\n\r\n\r"
           "       1. List Games.\n\r"
           "       2. Delete Game.\n\r"
           "       3. Restore Game.\n\r"
           "       4. Set Game Turn.\n\r"
           "       0. Quit Menu.\n\r");
}

void create_random_game (void)
{
  FILE *fptr;
  unsigned int world_x = 32,
  world_y = 32,
  errorlevel = 0,
  den_colour,
  warrior_colour;
  
  char string [256];
  clrscr ();
    cprintf ("\n\r     CREATE RANDOM GAME.\n\r\n\r     ");
  if (select_game ("Enter New Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s already exists! Press SPACE.\n\r",
	     game_code);
    getch ();
    return;
  }
  
  /* create game */

  clrscr ();
  cprintf ("\n\r"
           "     The LIZARDS! Menu Program is about to run a series of programs\n\r"
           "     to create a random world. Each program will prompt you for further\n\r"
           "     information.\n\r\n\r"
           "     Press SPACE to begin LIZARDS! Random World Creation.");
  getch ();

  full_screen ();

  sprintf (string, "%s -d %s -x %d -y %d %s", strWorldInit, data_path, world_x, world_y, game_code);
#ifdef DEBUG
  fprintf(stderr, "DEBUG: system %s\n", string);
#endif
  errorlevel = system(string);
/*
  errorlevel = spawnlp (P_WAIT, "WORLINIT.EXE", "WORLINIT.EXE", string, '\0');
*/

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLINIT program has failed with an error.\n\r"
             "     Random World Creation has been aborted.\n\r\n\r"
             "     Press SPACE to return to menu.");
    getch ();
    return;
  }

  sprintf (string, "%s -d %s -g %s", strWorldFfro, data_path, game_code);
  errorlevel = system(string);
  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLFFRO program has failed with an error.\n\r"
             "     Random World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE.");

    getch ();
    return;
  }

  sprintf (string, "%s %s %s", strWorldFill, data_path, game_code);
  errorlevel = system(string);
  if (errorlevel)
  {
    perror (strerror(errno));
    printf ("Press SPACE.\n");
    exit (EXIT_FAILURE);
  }
  restore_screen ();

  cprintf ("\n\r"
           "     The LIZARDS! Menu Program has successfully created the random world.\n\r\n\r"
	   "     Press SPACE to place PLAYERS into the game.");
  getch ();

  /* put players into game */

  wipe_player_list ();

  while (players_in_game == 0)
  {
    sprintf (string, "%s%s%s", data_path, PLAYER_F, game_code);

    if ((fptr = fopen (string, "rt")) != '\0')
    {
      fclose (fptr);

      get_player_list ();
    }

    get_game_player_list ();

    save_player_list ("01/01/1980");

    clear_error ();

    if (players_in_game == 0)
    {
      show_error ("\n\r     NO PLAYER INFORMATION ENTERED!");

      cprintf ("\n\r\n\r     Press SPACE to continue player entry.");

      getch ();
    }
  }

  /* randomly place players into world with selected den and warrior
    colours */

  den_colour = select_lizard_colour ("Select Player Starting Home Den Colour,");
  warrior_colour = select_lizard_colour ("Select Player Starting Warrior Lizard Colour,");

  full_screen ();

  sprintf (string, "%s %s %s %d %d", strWorldPop, data_path, game_code, den_colour, warrior_colour);
  errorlevel = system(string);
  
  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLPOP program has failed with an error.\n\r"
             "     Random World Creation has been aborted.\n\r\n\r"
             "     Press SPACE.");

    getch ();

    return;
  }

  sprintf (string, "%s %s %s", strWorldFix, data_path, game_code);
  errorlevel = system(string);
  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLFIX program has failed with an error.\n\r"
             "     Random World Creation has been aborted.\n\r\n\r"
             "     Press SPACE.");

    getch ();

    return;
  }

  restore_screen ();

  save_world_turn (0);

  cprintf ("\n\r"
           "     Random Game %s has been successfully created and is ready for play.\n\r\n\r"
           "     Press SPACE to return to menu.", game_code);

  getch ();
}

void create_handmade_game (void)
{
  FILE *fptr;

  unsigned int world_x = 32,
	       world_y = 32,
	       errorlevel = 0,
               den_colour,
               warrior_colour;

  char string [80],
       filename [13],
       overflow_char = 0;

  struct ffblk ffblk;

  clrscr ();

  cprintf ("\n\r     CREATE HAND MADE GAME.\n\r\n\r     ");

  if (select_game ("Enter New Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s already exists! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  /* select map file */

  clrscr ();

  cprintf ("\n\r     CREATE HAND MADE GAME.\n\r\n\r     ");

  cprintf ("Please enter the filename of hand made map: ");
  cprintf ("____________");
  gotoxy (wherex () - 12, wherey ());

  filename [0] = '\0';

  get_string (filename, 12, ALPHANUMERIC, FALSE, FALSE, FALSE, &overflow_char);

  /* check file exists and rename to appropriate lizards file */
/*
  if (findfirst (filename,&ffblk,0) == -1)
  {
    clrscr ();
    cprintf ("\n\r"
	     "     Unable to find hand made map file '%s' in the present directory.\n\r"
             "     Hand Made World Creation has been aborted.\n\r\n\r"
             "     Press SPACE to return to menu.", filename);

    getch ();
    return;
  }
*/

  /* rename file */

  sprintf (string, "COPY %s%s %sLHANDMAP.%s > NUL:", data_path, filename,
    data_path, game_code);

  system (string);

  /* create game */

  clrscr ();
  cprintf ("\n\r"
           "     The LIZARDS! Menu Program is about to run a series of programs\n\r"
	   "     to create a HAND MADE world. Each program will prompt you for further\n\r"
           "     information.\n\r\n\r"
	   "     Press SPACE to begin LIZARDS! Hand Made World Creation.");
  getch ();

  full_screen ();

  sprintf (string, "%s %s %d %d", data_path, game_code, world_x, world_y);
  errorlevel = spawnlp (P_WAIT, "WORLINIT.EXE", "WORLINIT.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
	     "     The WORLINIT program has failed with an error.\n\r"
	     "     Hand Made World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE to return to menu.");

    getch ();

    return;
  }

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "WORLTRAN.EXE", "WORLTRAN.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLTRAN program has failed with an error.\n\r"
	     "     Hand Made World Creation has been aborted.\n\r\n\r"
             "     Press SPACE.");

    getch ();

    return;
  }

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "WORLFIX.EXE", "WORLFIX.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLFIX program has failed with an error.\n\r"
             "     Hand Made World Creation has been aborted.\n\r\n\r"
             "     Press SPACE.");

    getch ();

    return;
  }

  restore_screen ();

  cprintf ("\n\r"
           "     The LIZARDS! Menu Program has successfully created the hand made world.\n\r\n\r"
           "     Press SPACE to place PLAYERS into the game.");
  getch ();

  /* put players into game */

  wipe_player_list ();

  while (players_in_game == 0)
  {
    sprintf (string, "%s%s%s", data_path, PLAYER_F, game_code);

    if ((fptr = fopen (string, "rt")) != '\0')
    {
      fclose (fptr);

      get_player_list ();
    }

    get_game_player_list ();

    save_player_list ("01/01/1980");

    clear_error ();

    if (players_in_game == 0)
      show_error ("NO PLAYER INFORMATION ENTERED!");
  }

  /* place players into world with selected den and warrior colours */

  den_colour = select_lizard_colour ("Select Player Starting Home Den Colour,");
  warrior_colour = select_lizard_colour ("Select Player Starting Warrior Lizard Colour,");

  full_screen ();

  sprintf (string, "%s %s %d %d", data_path, game_code, den_colour, warrior_colour);
  errorlevel = spawnlp (P_WAIT, "WORLHPOP.EXE", "WORLHPOP.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
             "     The WORLHPOP program has failed with an error.\n\r"
             "     Hand Made World Creation has been aborted.\n\r\n\r"
             "     Press SPACE.");

    getch ();

    return;
  }

  restore_screen ();

  save_world_turn (0);

  cprintf ("\n\r"
           "     Hand Made Game %s has been successfully created and is ready for play.\n\r\n\r"
	   "     Press SPACE to return to menu.", game_code);

  getch ();
}

void create_custom_startup_game (void)
{
  FILE *fptr;

  unsigned int world_x = 32,
	       world_y = 32,
	       errorlevel = 0,
	       option;

  char string [80];

  clrscr ();

  cprintf ("\n\r     CREATE CUSTOM GAME.\n\r\n\r     ");

  if (select_game ("Enter New Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s already exists! Sure you want to continue (Y/N)?\n\r",
      game_code);

    option = toupper (getch ());

    if (option != 'Y')
      return;
  }

  /* create game */

  clrscr ();
  cprintf ("\n\r"
	   "     The LIZARDS! Menu Program is about to run a series of programs\n\r"
	   "     to create a custom world. Each program will prompt you for further\n\r"
	   "     information.\n\r\n\r"
	   "     Press SPACE to begin LIZARDS! Custom World Creation.");
  getch ();

  full_screen ();

  sprintf (string, "%s %s %d %d", data_path, game_code, world_x, world_y);
  errorlevel = spawnlp (P_WAIT, "WORLINIT.EXE", "WORLINIT.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
	     "     The WORLINIT program has failed with an error.\n\r"
	     "     Custom World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE to return to menu.");

    getch ();

    return;
  }

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "WORLFFRO.EXE", "WORLFFRO.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
	     "     The WORLFFRO program has failed with an error.\n\r"
	     "     Custom World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE.");

    getch ();

    return;
  }

  restore_screen ();

  cprintf ("\n\r"
	   "     The LIZARDS! Menu Program has successfully initialised the custom world.\n\r\n\r"
	   "     Press SPACE to place PLAYERS into the game.");
  getch ();

  /* put players into game */

  wipe_player_list ();

  while (players_in_game == 0)
  {
    sprintf (string, "%s%s%s", data_path, PLAYER_F, game_code);

    if ((fptr = fopen (string, "rt")) != '\0')
    {
      fclose (fptr);

      get_player_list ();
    }

    get_game_player_list ();

    save_player_list ("01/01/1980");

    clear_error ();

    if (players_in_game == 0)
      show_error ("NO PLAYER INFORMATION ENTERED!");
  }

  restore_screen ();

  save_world_turn (0);

  cprintf ("\n\r"
	   "     Custom Game %s has been successfully initialised.\n\r"
	   "     The Game will **NOT** be ready to play until you place players\n\r"
	   "     in the game with their custom startups by using the NEXT option,\n\r"
	   "     '4. Enter Custom Startup Templates.' on the 'Create Games' Menu.\n\r\n\r"
	   "     Press SPACE to return to menu.", game_code);

  getch ();
}

void custom_start_game (void)
{
  FILE *fptr;

  unsigned int world_x = 32,
	       world_y = 32,
	       errorlevel = 0,
	       option;

  char string [80];

  clrscr ();

  cprintf ("\n\r     CUSTOM STARTUP TEMPLATES.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s already exists! Sure you want to continue (Y/N)?\n\r",
      game_code);

    option = toupper (getch ());

    if (option != 'Y')
      return;
  }

  clrscr ();

  cprintf ("\n\r"
	   "     You are now required to enter the player's CUSTOM STARTUP\n\r"
	   "     FORMS so that they can be placed in the game world.\n\r\n\r"
	   "     Press SPACE to enter PLAYERS' CUSTOM STARTUP FORMS into the game.");
  getch ();

  full_screen ();

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "CUSTENT.EXE", "CUSTENT.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
	     "     The CUSTENT program has failed with an error\n\r"
	     "     (unless you deliberately aborted CUSTENT with the F5 key).\n\r"
	     "     Custom World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE.");

    getch ();

    return;
  }

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "CUSTSTAR.EXE", "CUSTSTAR.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
	     "     The CUSTSTAR program has failed with an error.\n\r"
	     "     Custom World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE.");

    getch ();

    return;
  }

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "WORLFIX.EXE", "WORLFIX.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
  {
    restore_screen ();

    cprintf ("\n\r"
	     "     The WORLFIX program has failed with an error.\n\r"
	     "     Custom World Creation has been aborted.\n\r\n\r"
	     "     Press SPACE.");

    getch ();

    return;
  }

  restore_screen ();
}

void view_custom_startup_log (void)
{
  char string [256];

  int errorlevel,
      game_turn;

  clrscr ();

  cprintf ("\n\r     VIEW CUSTOM STARTUP LOG.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  full_screen ();

  sprintf (string, "VIEWNOW.BAT %sSTARTLOG.%s > NUL:", data_path, game_code);

  if (system (string) == -1)
  {
    cprintf ("\n\r     VIEW CUSTOM STARTUP LOG.\n\r\n\r");

    cprintf ("     A problem has occured with the VIEWNOW.BAT file.\n\r"
             "     The custom startup log could not be viewed.\n\r\n\r");

      cprintf ("     Press SPACE to return to menu.");

      getch ();

      return;
  }

  restore_screen ();
}

/* ----------------------------------------------------------------------
--
-- enter_orders
--
---------------------------------------------------------------------- */
void enter_orders (void)
{
  char string [256];

  int errorlevel,
      game_turn;

  clrscr ();

  cprintf ("\n\r     ENTER ORDERS.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  game_turn = get_world_turn ();

  full_screen ();

  sprintf(string, "%s %s %s %s %d %s", strOrderEnt, data_path, disk_drive_path,
	  email_data_path, game_turn, game_code);

  errorlevel = system(string);

  restore_screen ();

  if (errorlevel != EXIT_SUCCESS)
  {
    cprintf ("\n\r"
	     "     The ORDERENT program has failed with an error\n\r"
	     "     (unless you deliberately aborted ORDERENT with the F5 key).\n\r"
             "     THE PLAYER ORDERS MAY NOT HAVE BEEN SAVED.\n\r\n\r"
             "     Press SPACE to return to menu.");

    getch ();

    return;
  }
}

void run_turn (void)
{
  FILE *fptr;

  char string [80],
       due_date [12] = { "\x0" },
       *ptr,
       option,
       overflow_char = 0;

  int game_turn,
      result,
      printout = 'P';

  clrscr ();

  cprintf ("\n\r     RUN GAME TURN.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  game_turn = get_world_turn ();

  /* see if we can find orders */

  sprintf (string, "%s%s%03d.%s", data_path, TEXT_ORDS_F, game_turn, game_code);

  if ((fptr = fopen (string, "rt")) == '\0')
  {
    clrscr ();

    cprintf ("\n\r"
	     "     No orders found for Game %s, Turn %d!\n\r\n\r"
	     "     Press SPACE to return to menu.", game_code, game_turn);

    getch ();

    return;
  }

  fclose (fptr);

  get_player_list ();

  strcpy (due_date, game_due_date);

  cprintf ("\n\r\n\r     Due date in file is: %-12s\n\r", game_due_date);
  cprintf ("     Enter this turn's due date: ____________");

  gotoxy (wherex () - 12, wherey ());

  cprintf ("%s", due_date);

  get_string (due_date, 12, ALPHANUMERIC, TRUE, FALSE, FALSE, &overflow_char);

  cprintf ("\n\r\n\r     Ascii or Postscript output (A/P, Default = Postscript)? ");

  option = toupper (getche ());

  if (option == 'A')
    printout = 'A';

  cprintf ("\n\r\n\r     SURE YOU WANT TO RUN THIS TURN (Y/N, ENTER=YES)? ");

  option = toupper (getche ());

  if (option != '\r' && option != 'Y') return;

  save_player_list (due_date);

  full_screen ();

  if (printout == 'P')
    result = postscript_turn (game_turn);
  else
    result = ascii_turn (game_turn);

  restore_screen ();

  if (result != 1)
  {
    unbackup_game ();

    cprintf ("\n\r"
             "     A problem has occured with the turn.\n\r"
             "     THE TURN MAY NOT HAVE BEEN PROCESSED CORRECTLY.\n\r"
             "     The game should have been restored.\n\r"
             "     Press SPACE to return to menu.");

    getch ();

    return;
  }

  save_world_turn (game_turn + 1);

  output_to_log (game_turn);

  cprintf ("\n\r"
           "     The turn has been processed correctly.\n\r\n\r"
           "     Press SPACE to return to menu.");

  getch ();

  get_player_list ();

  return;
}

void output_to_log (int turn)
{
  FILE *fptr;

  int player;

  if ((fptr = fopen (LIZMENU_LOG_FILE, "at")) == '\0')
    if ((fptr = fopen (LIZMENU_LOG_FILE, "wt")) == '\0')
    {
      printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", LIZMENU_LOG_FILE);

      getch ();

      exit (EXIT_FAILURE);
    }

  fprintf (fptr, "GAME %s\nTURN %d\n", game_code, turn);

  for (player = 1; player < MAX_PLAYERS + 1; player ++)
    if (strcmp (player_name [player], "****") != 0 &&
      strcmp (player_name [player], "DROP") != 0)
      fprintf (fptr, "%s\n", player_name [player]);

  fclose (fptr);
}

void print_turn (void)
{
  FILE *fptr;

  char string [256],
       option;

  int errorlevel,
      game_turn,
      result,
      loop,
      printout = 'P';

  clrscr ();

  cprintf ("\n\r     PRINT REPORTS.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  game_turn = get_world_turn () - 1;

  if (game_turn < 0)
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s has no been run yet! Press SPACE.");

    getch ();

    return;
  }

  cprintf ("\n\r\n\r     Ascii or Postscript output (A/P, Default = Postscript)? ");

  option = toupper (getche ());

  if (option == 'A')
    printout = 'A';

  if (printout == 'P')
  {
    /* postscript output, just print the postscript output file. */

    sprintf (string, "%s%s%03d.%s", data_path, POSTSCRIPT_F, game_turn,
      game_code);

    /* first see if we can find it in present directory */

    if ((fptr = fopen (string, "rt")) == '\0')
    {
      clrscr ();

      cprintf ("\n\r     PRINT REPORTS.\n\r\n\r");

      cprintf ("     Unable to find the required print file '%s'.\n\r"
               "     There is no turn report to print.\n\r\n\r",
               string);

      cprintf ("     Press SPACE to return to menu.");

      getch ();

      return;
    }

    fclose (fptr);

    sprintf (string, "printnow.bat %s%s%03d.%s", data_path, POSTSCRIPT_F,
      game_turn, game_code);

    full_screen ();

    if (system (string) == -1)
    {
      restore_screen ();

      cprintf ("\n\r     PRINT REPORTS.\n\r\n\r");

      cprintf ("     A problem has occured with the PRINTNOW.BAT file.\n\r"
               "     The turn report may not print.\n\r\n\r");

      cprintf ("     Press SPACE to return to menu.");

      getch ();

      return;
    }

    /* transfer any players required to disk */

    sprintf (string, "%s %s %s %s %d %s", data_path, disk_data_path,
      email_data_path, disk_drive_path, game_turn, game_code);
    errorlevel = spawnlp (P_WAIT, "DISKTRAN.EXE", "DISKTRAN.EXE", string, '\0');

    if (errorlevel != EXIT_SUCCESS)
    {
      restore_screen ();

      cprintf ("\n\r"
	       "     The DISKTRAN program has failed with an error.\n\r"
	       "     Transfer of Players to Disk has been aborted.\n\r\n\r"
	       "     Press SPACE to return to menu.");

      getch ();

      return;
    }
  } else {
    /* ascii output, print EACH live players report page. */

    get_player_list ();

    /* only print the reports of the live-un's */

    full_screen ();

    for (loop = 0; loop < MAX_PLAYERS + 1; loop ++)
      if (loop == 0 || (strcmp (player_name [loop], "****") != 0 &&
        strcmp (player_name [loop], "DROP") != 0))
      {
        /* print this player */

	sprintf (string, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, loop,
	  game_turn, game_code);

        /* first see if we can find it in present directory */

        if ((fptr = fopen (string, "rt")) == '\0')
        {
          restore_screen ();

          clrscr ();

          cprintf ("\n\r     PRINT REPORTS.\n\r\n\r");

          cprintf ("     Unable to find the required print file '%s'.\n\r"
                   "     Full turn reports may not have printed.\n\r\n\r",
                   string);

          cprintf ("     Press SPACE to return to menu.");

          getch ();

          return;
        }

        fclose (fptr);

	sprintf (string, "printnow.bat %s%s%02d_%03d.%s", data_path,
	  PLAYER_MESG_F, loop, game_turn, game_code);

	if (system (string) == -1)
	{
	  restore_screen ();

	  cprintf ("\n\r     PRINT REPORTS.\n\r\n\r");

	  cprintf ("     A problem has occured with the PRINTNOW.BAT file.\n\r"
		   "     All player turn reports may not have printed.\n\r\n\r");

	  cprintf ("     Press SPACE to return to menu.");

	  getch ();

	  return;
	}
      }
  }

  restore_screen ();

  cprintf ("\n\r     PRINT REPORTS.\n\r\n\r");

  cprintf ("     The DOS PRINT utility should be printing player reports now.\n\r\n\r");

  cprintf ("     Press SPACE to return to menu.");

  getch ();
}

int postscript_turn (int turn)
{
  char string [80];

  int errorlevel;

  if (backup_game (turn) == -1)
    return (-1);

  sprintf (string, "DEL %sLP??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "DEL %sLM??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "DEL %sLC??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "DEL %sLE??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "%s %d %s", data_path, turn, game_code);
  errorlevel = spawnlp (P_WAIT, "TORDBORD.EXE", "TORDBORD.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "TERRMAST.EXE", "TERRMAST.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "BORDSHUF.EXE", "BORDSHUF.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "PSWRLOPN.EXE", "PSWRLOPN.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "WORLMAST.EXE", "WORLMAST.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "WORLSHUT.EXE", "WORLSHUT.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "CLANREPO.EXE", "CLANREPO.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "CLANRANK.EXE", "CLANRANK.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "MESSAPND.EXE", "MESSAPND.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  sprintf (string, "COPY LZHEADER.PS %s%s%03d.%s > NUL:", data_path,
    POSTSCRIPT_F, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "%s %s %d %s Y", data_path, disk_data_path, turn, game_code);

  errorlevel = spawnlp (P_WAIT, "PSCENPRN.EXE", "PSCENPRN.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  sprintf (string, "%s %s %d %s", data_path, disk_data_path, turn, game_code);

  errorlevel = spawnlp (P_WAIT, "DISKWORL.EXE", "DISKWORL.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  return (1);
}

int ascii_turn (int turn)
{
  char string [80];

  int errorlevel;

  if (backup_game (turn) == -1)
    return (-1);

  sprintf (string, "DEL %sLP??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "DEL %sLM??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "DEL %sLC??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "DEL %sLE??_%03d.%s > NUL:", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "%s %d %s", data_path, turn, game_code);
  errorlevel = spawnlp (P_WAIT, "TORDBORD.EXE", "TORDBORD.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "TERRMAST.EXE", "TERRMAST.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "BORDSHUF.EXE", "BORDSHUF.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "WORLOPEN.EXE", "WORLOPEN.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "WORLMAST.EXE", "WORLMAST.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "WORLSHUT.EXE", "WORLSHUT.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "CLANREPO.EXE", "CLANREPO.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "CLANRANK.EXE", "CLANRANK.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "MESSAPND.EXE", "MESSAPND.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  errorlevel = spawnlp (P_WAIT, "CLANVIEW.EXE", "CLANVIEW.EXE", string, '\0');

  if (errorlevel != EXIT_SUCCESS)
    return (-1);

  return (1);
}

void print_world_map (void)
{
  char string [80] = "\x0",
       overflow_char = 0,
       option;

  int copies = 0,
      loop;

  clrscr ();

  cprintf ("\n\r     PRINT WORLD MAPS.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  cprintf ("\n\r\n\r     Number of Maps to Print: __");
  gotoxy (wherex () - 2, wherey ());
  cprintf ("%s", string);

  get_string (string, 2, NUMERIC, FALSE, FALSE, FALSE, &overflow_char);

  copies = atoi (string);

  if (copies == 0) return;

  full_screen ();

  sprintf (string, "worldmap.bat %s %s", data_path, game_code);

  if (system (string) == -1)
  {
    restore_screen ();

    cprintf ("\n\r     PRINT WORLD MAPS.\n\r\n\r");

    cprintf ("     A problem has occured with the WORLDMAP.BAT file.\n\r"
	     "     No world maps may be printed.\n\r\n\r");

    cprintf ("     Press SPACE to return to menu.");

    getch ();

    return;
  }

  sprintf (string, "PRINTNOW.BAT %sLZPSMAP.%s", data_path, game_code);

  for (loop = 0; loop < copies; loop ++)
    if (system (string) == -1)
    {
      restore_screen ();

      cprintf ("\n\r     PRINT WORLD MAPS.\n\r\n\r");

      cprintf ("     A problem has occured with the PRINTNOW.BAT file.\n\r"
	       "     Not all world maps may have been printed.\n\r\n\r");

      cprintf ("     Press SPACE to return to menu.");

      getch ();

      return;
    }

  restore_screen ();
}

int backup_game (int turn)
{
  char string [80];

  sprintf (string, "COPY %s*.%s %s*.BAK > NUL:", data_path, game_code,
    data_path);

  if (system (string) == -1)
    return (-1);

  sprintf (string, "lzbackup.bat %s %03d %s", data_path, turn, game_code);

  if (system (string) == -1)
    return (-1);

  return (1);
}

int unbackup_game (void)
{
  char string [80];

  sprintf (string, "COPY %s*.BAK %s*.%s > NUL:", data_path, data_path,
    game_code);

  if (system (string) == -1)
    return (-1);

  return (1);
}

void restore_game (void)
{
  char string [80],
       overflow_char = 0,
       option;

  int game_turn;

  clrscr ();

  cprintf ("\n\r     RESTORE GAME.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  game_turn = get_world_turn ();

  itoa (game_turn, string, 10);

  cprintf ("\n\r\n\r     Present Game Turn: ___");

  gotoxy (wherex () - 3, wherey ());
  cprintf ("%s", string);

  itoa (game_turn - 1, string, 10);

  cprintf ("\n\r\n\r     Enter Game Turn to Restore: ___");
  gotoxy (wherex () - 3, wherey ());
  cprintf ("%s", string);

  get_string (string, 3, NUMERIC, FALSE, FALSE, FALSE, &overflow_char);

  game_turn = atoi (string);

  cprintf ("\n\r\n\r     SURE YOU WANT TO RESTORE GAME %s TURN %d (Y/N)? ",
    game_code, game_turn);

  option = toupper (getche ());

  if (option == 'Y')
  {
    sprintf (string, "lzunback.bat %s %03d %s", data_path,
      game_turn, game_code);

    full_screen ();

    if (system (string) == -1)
    {
      restore_screen ();

      cprintf ("\n\r\n\r     A problem has occured. Game may have not been restored.\n\r\n\r");
      cprintf ("     Press SPACE to return to menu.");

      getch ();
    } else
      restore_screen ();
  }
}

void edit_world (void)
{
  char string [256];

  int errorlevel;

  clrscr ();

  cprintf ("\n\r     EDIT GAME WORLD.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  full_screen ();

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "WORLEDIT.EXE", "WORLEDIT.EXE", string, '\0');

  restore_screen ();

  if (errorlevel != EXIT_SUCCESS)
  {
    cprintf ("\n\r"
             "     A problem has occured with the WORLEDIT program.\n\r"
             "     GAME %s MAY HAVE BEEN CORRUPTED.\n\r\n\r"
             "     Press SPACE to return to menu.", game_code);

    getch ();

    return;
  }
}

void edit_spys (void)
{
  char string [256];

  int errorlevel;

  clrscr ();

  cprintf ("\n\r     EDIT GAME SPIES.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  full_screen ();

  sprintf (string, "%s %s", data_path, game_code);
  errorlevel = spawnlp (P_WAIT, "SPYEDIT.EXE", "SPYEDIT.EXE", string, '\0');

  restore_screen ();

  if (errorlevel != EXIT_SUCCESS)
  {
    cprintf ("\n\r"
             "     A problem has occured with the SPYEDIT program.\n\r"
             "     GAME %s SPIES MAY HAVE BEEN CORRUPTED.\n\r\n\r"
             "     Press SPACE to return to menu.", game_code);

    getch ();

    return;
  }
}

void change_player (void)
{
  char number [3] = { "\x0" },
       string [255],
       overflow_char = 0;

  char **player_data;

  int player;

  unsigned int field_in;

  clrscr ();

  cprintf ("\n\r     CHANGE PLAYER.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  wipe_player_list ();

  get_player_list ();

  if (players_in_game == 0)
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s HAS NO PLAYERS! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  full_screen ();

  clrscr ();

  cprintf ("\n\r     CHANGE PLAYER IN GAME.\n\r\n\r");

  show_player_list ();

  cprintf ("\n\r     Enter player number to change (ENTER = Abort): __");

  gotoxy (wherex () - 2, wherey ());

  get_string (number, 2, NUMERIC, FALSE, FALSE, FALSE, &overflow_char);

  player = atoi (number);

  if (player == 0)
  {
    restore_screen ();

    return;
  }

  if (player > MAX_PLAYERS + 1)
  {
    restore_screen ();

    cprintf ("\n\r     CHANGE PLAYER.\n\r\n\r     ");

    cprintf ("INVALID player number entered. Press SPACE.");

    getch ();

    return;
  }

  if (strcmp (player_name [player], "****") == 0)
  {
    restore_screen ();

    cprintf ("\n\r     CHANGE PLAYER.\n\r\n\r     ");

    cprintf ("NON-EXISTANT or REMOVED player number entered. Press SPACE.");

    getch ();

    return;
  }

  if ((player_data = calloc (player_template_fields + 1,
    sizeof (char *))) == '\0')
  {
    printf ("FATAL ERROR: Out of memory for calloc. Press SPACE.\n");

    getch ();

    exit (EXIT_FAILURE);
  }

  set_field (player_template, player_data, 0, game_code);
  set_field (player_template, player_data, 1, itoa (player, string, 10));
  set_field (player_template, player_data, 2, player_clan_name [player]);
  set_field (player_template, player_data, 3, player_code [player]);
  set_field (player_template, player_data, 4, player_name [player]);
  set_field (player_template, player_data, 5, player_road [player]);
  set_field (player_template, player_data, 6, player_burb [player]);
  set_field (player_template, player_data, 7, player_city [player]);
  set_field (player_template, player_data, 8, player_country [player]);
  string [0] = player_output_mode [player];
  string [1] = '\0';
  set_field (player_template, player_data, 9, string);

  clrscr ();

  show_player_form ();

  show_template (player_template, player_data);

  while (enter_template (player_template, player_data, 2, &field_in, '\0', '\0', '\0') == ERASE_KEY)
  {
    free_data (player_template, player_data);

    if ((player_data = calloc (player_template_fields + 1,
      sizeof (char *))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory for calloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    set_field (player_template, player_data, 0, game_code);
    set_field (player_template, player_data, 1, itoa (player, string, 10));

    show_template (player_template, player_data);
  }

  show_template (player_template, player_data);

  cprintf ("\n\r\n\rSave this player (Y/N)? ");

  if (toupper (getche ()) == 'Y')
  {
    if (player_data [2] != '\0')
      strcpy (player_clan_name [player], player_data [2]);
    else
      player_clan_name [player][0] = '\0';

    if (player_data [3] != '\0')
      strcpy (player_code [player], player_data [3]);
    else
      player_code [player][0] = '\0';

    if (player_data [4] != '\0')
      strcpy (player_name [player], player_data [4]);
    else
      player_name [player][0] = '\0';

    if (player_data [5] != '\0')
      strcpy (player_road [player], player_data [5]);
    else
      player_road [player][0] = '\0';

    if (player_data [6] != '\0')
      strcpy (player_burb [player], player_data [6]);
    else
      player_burb [player][0] = '\0';

    if (player_data [7] != '\0')
      strcpy (player_city [player], player_data [7]);
    else
      player_city [player][0] = '\0';

    if (player_data [8] != '\0')
      strcpy (player_country [player], player_data [8]);
    else
      player_country [player][0] = '\0';

    if (player_data [9] != '\0')
      player_output_mode [player] = player_data [9][0];
    else
      player_output_mode [player] = '\0';

    save_player_list (game_due_date);

    restore_screen ();

    cprintf ("\n\r     CHANGE PLAYER.\n\r\n\r     ");

    cprintf ("Player number %02d CHANGED in Game %s. Press SPACE.",
      player, game_code);

    getch ();

    free_data (player_template, player_data);

    return;
  } else {
    restore_screen ();

    cprintf ("\n\r     CHANGE PLAYER.\n\r\n\r     ");

    cprintf ("Player number %02d WAS NOT CHANGED in Game %s. Press SPACE.",
      player, game_code);

    getch ();

    free_data (player_template, player_data);

    return;
  }
}

void list_games (void)
{
  struct ffblk ffblk;

  char *ptr,
       *games_list = '\0';

  int done,
      loop,
      games = 0,
      lines = 3;

  clrscr ();

  cprintf ("\n\r     LIST GAMES.\n\r\n\r");
/*
  done = findfirst("LWLDATA.*",&ffblk,0);
*/
  while (!done)
  {
    ptr = strchr ((const char *) &ffblk.ff_name, '.');

    if (strcmp (ptr, ".BAK") != 0)
    {
      /* add name to malloced list */

      if (games_list == '\0')
      {
        if ((games_list = malloc (sizeof (char) * 4)) == '\0')
        {
          printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

          getch ();

          exit (EXIT_FAILURE);
        }
      } else
        if ((games_list = realloc (games_list, sizeof (char) * 4 * (games + 1))) == '\0')
        {
          printf ("FATAL ERROR: Out of memory for realloc. Press SPACE.\n");

          getch ();

          exit (EXIT_FAILURE);
        }

      strcpy (&games_list [games * 4], ++ ptr);

      games ++;
    }
/*
    done = findnext(&ffblk);
*/
  }

  /* quick sort the game names */

  qsort ((void *)games_list, games, sizeof (char) * 4, game_list_sort);

  for (loop = 0; loop < games; loop ++)
  {
    if (lines > 10)
    {
      cprintf ("\n\r     Press SPACE to continue.");

      getch ();

      lines = 3;

      clrscr ();

      cprintf ("\n\r     LIST GAMES.\n\r\n\r");
    }

    cprintf("     LIZARDS! Game %s\n\r", &games_list [loop * 4]);

    lines ++;
  }

  cprintf ("\n\r     End of Games. Press SPACE to return to menu.");

  getch ();

  if (games_list != '\0')
    free (games_list);
}

int game_list_sort (const void *one, const void *two)
{
  return (strcmp (one, two));
}

void delete_game (void)
{
  char option,
       string [80];

  int game_turn;

  clrscr ();

  cprintf ("\n\r     DELETE LIZARDS GAME.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  game_turn = get_world_turn ();

  cprintf ("\n\r\n\r     LIZARDS! Game %s is in Turn %d.\n\r\n\r",
    game_code, game_turn);

  cprintf ("     SURE YOU WANT TO DELETE THIS GAME FROM THE HARD DISK (Y/N)? ");

  option = toupper (getche ());

  if (option == 'Y')
  {
    cprintf ("\n\r\n\r     Press SPACE to DELETE GAME %s or ANY OTHER KEY to abort.",
      game_code);

    option = getch ();

    if (option == ' ')
    {
      sprintf (string, "del %s*.%s > NUL:", data_path, game_code);

      if (system (string) != 0)
      {
        clrscr ();

        cprintf ("\n\r\n\r     A problem has occured. Game may have not been deleted.\n\r\n\r");
        cprintf ("     Press SPACE to return to menu.");

        getch ();
      } else {
        clrscr ();

        cprintf ("\n\r\n\r     LIZARDS! Game %s has been removed from the hard disk.\n\r\n\r", game_code);
        cprintf ("     Press SPACE to return to menu.");

        getch ();
      }
    }
  }
}

void set_game_turn (void)
{
  char string [10],
       overflow_char = '\0';

  int game_turn;

  clrscr ();

  cprintf ("\n\r     SET GAME TURN.\n\r\n\r     ");

  if (select_game ("Enter Game Code (3 alphanumeric, ENTER=Abort):") == -1)
    return;

  if (!game_code_exists ())
  {
    cprintf ("\n\r\n\r     LIZARDS! Game %s does NOT exist! Press SPACE.\n\r",
      game_code);

    getch ();

    return;
  }

  game_turn = get_world_turn ();

  itoa (game_turn, string, 10);

  cprintf ("\n\r\n\r      Enter Game Turn: ___");
  gotoxy (wherex () - 3, wherey ());
  cprintf ("%s", string);

  get_string (string, LZ_GAMECODELEN, NUMERIC, FALSE, FALSE, FALSE, &overflow_char);

  game_turn = atoi (string);

  save_world_turn (game_turn);
}

/* ----------------------------------------------------------------------
--
-- select_game
--
---------------------------------------------------------------------- */
int select_game (char *prompt)
{
  char overflow_char = '\0';
  char buf[LZ_GAMECODELEN + 1];
  int x,y;
  cprintf ("%s ", prompt);

  if (game_code)
    cprintf ("[%s] ", game_code);
  else
    {
      game_code = (char*)malloc(LZ_GAMECODELEN + 1);
      game_code[0] = '\0';
    }
/*
  echo();
*/
  get_string (game_code, 3, ALPHANUMERIC, FALSE, FALSE, FALSE, &overflow_char);
/*
  wgetnstr(menuWindow, buf, LZ_GAMECODELEN);
  noecho();
*/
/*  if (strlen(buf))
    strcpy(game_code, buf);
*/
  strupr (game_code);
  return (strlen(game_code)) ? 1 : -1;
}

int select_lizard_colour (char *prompt)
{
  char option = ' ';

  while (option != 'R' && option != 'G' && option != 'E' && option != 'Y' &&
    option != 'B')
  {
    clrscr ();

    cprintf ("\n\r     %s\n\r"
             "     (Red, Green, grEy, Yellow, Black): ", prompt);

    option = toupper (getche ());
  }

  switch (option)
  {
    case 'R':
      return (RED_LIZARD);
    case 'G':
      return (GREEN_LIZARD);
    case 'E':
      return (GREY_LIZARD);
    case 'Y':
      return (YELLOW_LIZARD);
    case 'B':
      return (BLACK_LIZARD);
  }

  return (0);
}
/* ----------------------------------------------------------------------
--
-- game_code_exists
--
---------------------------------------------------------------------- */

int game_code_exists (void)
{
  char filename [256];
  struct stat statbuf;
  snprintf (filename, 255, "%s/%s%s", data_path, WORLD_STAT_F, game_code);
  if (stat(filename, &statbuf) == -1)
    return FALSE;
  else
    return TRUE;
}

/* ----------------------------------------------------------------------
--
-- get_game_player_list
--
---------------------------------------------------------------------- */

void get_game_player_list (void)
{
  char **player_data = '\0',
       string [256];

  unsigned int player = 1,
	       field_in;

  full_screen ();

  while (player < MAX_PLAYERS + 1)
  {
    free_data (player_template, player_data);

    if ((player_data = calloc (player_template_fields + 1,
      sizeof (char *))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory for calloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    set_field (player_template, player_data, 0, game_code);
    set_field (player_template, player_data, 1, itoa (player, string, 10));
    set_field (player_template, player_data, 2, player_clan_name [player]);
    set_field (player_template, player_data, 3, player_code [player]);
    set_field (player_template, player_data, 4, player_name [player]);
    set_field (player_template, player_data, 5, player_road [player]);
    set_field (player_template, player_data, 6, player_burb [player]);
    set_field (player_template, player_data, 7, player_city [player]);
    set_field (player_template, player_data, 8, player_country [player]);
    string [0] = player_output_mode [player];
    string [1] = '\0';
    set_field (player_template, player_data, 9, string);

    show_player_form ();

    show_template (player_template, player_data);

    switch (enter_template (player_template, player_data, 2, &field_in, '\0', '\0', '\0'))
    {
      case ERASE_KEY:
	/* do nothing as erased at beginning of next while loop */

        break;
      case FINISH_KEY:
        /* put player form data into player table */

        if (player_data [2] != '\0')
          strcpy (player_clan_name [player], player_data [2]);
        else
          player_clan_name [player][0] = '\0';

	if (player_data [3] != '\0')
          strcpy (player_code [player], player_data [3]);
	else
          player_code [player][0] = '\0';

        if (player_data [4] != '\0')
          strcpy (player_name [player], player_data [4]);
        else
          player_name [player][0] = '\0';

        if (player_data [5] != '\0')
	  strcpy (player_road [player], player_data [5]);
	else
	  player_road [player][0] = '\0';

	if (player_data [6] != '\0')
	  strcpy (player_burb [player], player_data [6]);
	else
	  player_burb [player][0] = '\0';

	if (player_data [7] != '\0')
	  strcpy (player_city [player], player_data [7]);
	else
	  player_city [player][0] = '\0';

	if (player_data [8] != '\0')
	  strcpy (player_country [player], player_data [8]);
	else
	  player_country [player][0] = '\0';

	if (player_data [9] != '\0')
	  player_output_mode [player] = player_data [9][0];
	else
	  player_output_mode [player] = '\0';

	player ++;

	break;
      case ESCAPE_KEY:
        /* put player form data into player table */

        if (player_data [2] != '\0')
          strcpy (player_clan_name [player], player_data [2]);
        else
          player_clan_name [player][0] = '\0';

        if (player_data [3] != '\0')
	  strcpy (player_code [player], player_data [3]);
	else
	  player_code [player][0] = '\0';

        if (player_data [4] != '\0')
	  strcpy (player_name [player], player_data [4]);
	else
	  player_name [player][0] = '\0';

	if (player_data [5] != '\0')
	  strcpy (player_road [player], player_data [5]);
	else
	  player_road [player][0] = '\0';

	if (player_data [6] != '\0')
	  strcpy (player_burb [player], player_data [6]);
	else
	  player_burb [player][0] = '\0';

	if (player_data [7] != '\0')
	  strcpy (player_city [player], player_data [7]);
	else
	  player_city [player][0] = '\0';

	if (player_data [8] != '\0')
	  strcpy (player_country [player], player_data [8]);
	else
	  player_country [player][0] = '\0';

	if (player_data [9] != '\0')
	  player_output_mode [player] = player_data [9][0];
	else
	  player_output_mode [player] = '\0';

	player = 17;

	break;
    }
  }

  restore_screen ();
}

int get_world_turn (void)
{
  FILE *fptr;

  char filename [80];

  int turn;

  sprintf (filename, "%s%s%s", data_path, WORLD_TURN_F, game_code);

  if ((fptr = fopen (filename, "rt")) == '\0')
    return (0);

  fscanf (fptr, "%d\n", &turn);

  fclose (fptr);

  return (turn);
}

void save_world_turn (int turn)
{
  FILE *fptr;

  char filename [80];

  sprintf (filename, "%s%s%s", data_path, WORLD_TURN_F, game_code);

  if ((fptr = fopen (filename, "wt")) == '\0')
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n");

    getch ();

    exit (EXIT_FAILURE);
  }

  fprintf (fptr, "%d\n", turn);

  fclose (fptr);
}

void wipe_player_list (void)
{
  unsigned int loop;

  players_in_game = 0;

  for (loop = 0; loop < MAX_PLAYERS + 1; loop ++)
  {
    player_code [loop][0] = '\0';
    player_name [loop][0] = '\0';
    player_clan_name [loop][0] = '\0';

    player_road [loop][0] = '\0';
    player_burb [loop][0] = '\0';
    player_city [loop][0] = '\0';
    player_country [loop][0] = '\0';
    player_cred [loop][0] = '\0';
    player_output_mode [loop] = '\0';
  }
}

void show_player_list (void)
{
  int loop;

  for (loop = 1; loop < MAX_PLAYERS + 1; loop ++)
    if (strcmp (player_name [loop], "****") != 0)
    {
      cprintf ("%02d-%-30s(%-3s),%-35s\n\r",
        loop, player_clan_name [loop], player_code [loop],
        player_name [loop]);
    }
}

void show_player_form (void)
{
  clrscr ();

  cprintf ("LIZARDS! PLAYER FORM.\n\r\n\r"
	   "Game Code ___                    Player Number __\n\r"
	   "Clan Name ____________________   Clan Code ___\n\r"
	   "Player Name   ____________________________\n\r"
	   "Player Road   ______________________________           F3 = ERASE FORM\n\r"
	   "Player Suburb ______________________________           F4 = SAVE PLAYER\n\r"
	   "Player City   ______________________________           F5 = ALL FINISHED!\n\r"
	   "Playr Country ______________________________\n\r"
	   "Output Type   _ (P=Paper, B=BBS, D=Disk).\n\r");

}

void full_screen (void)
{
/*
  window (1, 1, 80, 25);
*/
  endwin();
  clrscr ();
}

void restore_screen (void)
{
  /*
     clrscr ();
  reopen_text_window (&title_win);
  show_lizmenu_title ();
  reopen_text_window (&menu_win);
  clrscr ();
  */
  
}

/* ----------------------------------------------------------------------
--
-- get_data_paths
--
---------------------------------------------------------------------- */
void get_data_paths (const char *paths)
{
  FILE *fptr;
  struct stat statbuf;
  char *odata_path = (char*) malloc(LZ_MAXPATH);
  char *odd_path = (char*) malloc(LZ_MAXPATH);
  char *oemail_path = (char*) malloc(LZ_MAXPATH);
  char *odisk_path = (char*) malloc(LZ_MAXPATH);
  if ((fptr = fopen (paths, "rt")) == 0)
  {
    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", paths);
    exit (EXIT_FAILURE);
  }
  fscanf (fptr, "%70[^\n]\n%70[^\n]\n%70[^\n]\n%70[^\n]\n", odata_path,
    odd_path, oemail_path, odisk_path);

  fclose (fptr);
  data_path = LzCheckDir(odata_path, 1);
  disk_data_path = LzCheckDir(odd_path, 1);
  email_data_path = LzCheckDir(oemail_path, 1);
  disk_drive_path = LzCheckDir(odisk_path, 1);
  free(odata_path);
  free(odd_path);
  free(oemail_path);
  free(odisk_path);
}
