#include <ctype.h>
#include <conio.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "map.h"
#include "order.h"
#include "lizard.h"

#include "textwind.h"
#include "textwinf.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

#define TITLE_WINDOW 32, 1, 80, 18, LIGHTGRAY, BLACK
#define LIST_WINDOW 1, 1, 31, 18, LIGHTGRAY, BLACK
#define WORK_WINDOW 1, 19, 80, 24, LIGHTGRAY, BLACK

spy_t *spy_list [MAX_SPYS];

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

char data_path [255],
     game_code [5];

int turn_number,
    number_spys,
    world_x,
    world_y;

win_t title_win,
      list_win,
      work_win;

int main (int argc, char *argv []);
void add_spy (int *spy_now_shown);
void change_spy (int *spy_now_shown);
void delete_spy (int *spy_now_shown);
void search_by_player (int *spy_now_shown);
void show_spys (int start);
void select_hex (int *x, int *y);

int main (int argc, char *argv [])
{
  char work_string [80],
       option;

  int work,
      present_show_spy;

  randomize ();

  open_text_window (&title_win, TITLE_WINDOW, NO_BORDER);

  cprintf ("\n\r\n\r\n\r\n\r"
    "                  LIZARDS!\n\r\n\r"
    "        (c) Copyright, 1991,92,93.\n\r"
    "        LIZARDS! (TM), 1991,92,93.\n\r"
    "      Programmed by Stuart Nicholson.\n\r"
    "           Contact Circle Games,\n\r"
    "    P O Box 105320 Auckland, New Zealand.\n\r"
    "           Phone (09) 358 5378.\n\r\n\r");

  open_text_window (&list_win, LIST_WINDOW, SINGLE_BORDER);

  open_text_window (&work_win, WORK_WINDOW, SINGLE_BORDER);

  cprintf ("LIZARDS! Spy Lizard Editor. %s Last compiled %s.\n\r",
          VER_STRING, __DATE__);

  if (argc == 3)
  {
    strcpy (data_path, argv [1]);
    strncpy (game_code, argv [2], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    cprintf ("FATAL ERROR: Game Number not specified on command line!\n\r"
	     "             Command Line Format: EXEFILE data_path game.\n\r"
	     "                             e.g. SPYEDIT C:\\ A\n\r");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  show_spys (present_show_spy = 0);

  gotoxy (1, 4);
  cprintf ("A=Add, C=Change, D=Delete spy, S=Show, E=sEarch by player, Q=Quit");

  do
  {
    gotoxy (1, 3);
    clreol ();

    option = toupper (getch ());

    switch (option)
    {
      case 'A':
	add_spy (&present_show_spy);
	break;
      case 'C':
	change_spy (&present_show_spy);
	break;
      case 'S':
	gotoxy (1, 3);

	cprintf ("Enter starting spy (0-%d)? ", number_spys - 1);

	work = atoi (gets (work_string));

	if (work >-1 && work < number_spys)
	{
	  present_show_spy = work;

	  show_spys (present_show_spy);
	}

	break;
      case 'D':
	delete_spy (&present_show_spy);
	break;
      case 'E':
	search_by_player (&present_show_spy);
	break;
      default:
	break;
    }
  } while (option != 'Q');

  put_spy_list ();

  for (work = 0; work < number_spys; work ++)
    free (spy_list [work]);

  cprintf ("Finished!\n\r");

  close_text_window (&list_win);
  close_text_window (&work_win);
  close_text_window (&title_win);

  return (EXIT_SUCCESS);
}

void add_spy (int *spy_now_shown)
{
  char work_string [80],
       option;

  int work,
      x,
      y;

  spy_t work_spy;

  do
  {
    gotoxy (1, 3);
    clreol ();

    cprintf ("Enter Player No (1-%d)? ", players_in_game);

    work = atoi (gets (work_string));
  } while (work < 1 || work > players_in_game);

  work_spy.player = work;

  do
  {
    gotoxy (1, 3);
    clreol ();

    select_hex (&x, &y);
  } while (x < 0 || x > world_x || y < 0 || y  > world_y);

  work_spy.x_hex = x;
  work_spy.y_hex = y;

  do
  {
    gotoxy (1, 3);
    clreol ();

    cprintf ("Enter Lizard Type (Red/Green/grEy/Black/Yellow)? ");

    option = toupper (getche ());
  } while (option != 'R' && option != 'G' && option != 'E' && option != 'B' &&
	   option != 'Y');

  switch (option)
  {
    case 'R':
      work_spy.lizard_type = RED_LIZARD;
      break;
    case 'G':
      work_spy.lizard_type = GREEN_LIZARD;
      break;
    case 'E':
      work_spy.lizard_type = GREY_LIZARD;
      break;
    case 'B':
      work_spy.lizard_type = BLACK_LIZARD;
      break;
    case 'Y':
      work_spy.lizard_type = YELLOW_LIZARD;
      break;
  }

  do
  {
    gotoxy (1, 3);
    clreol ();

    cprintf ("Spy Lizard for ? Turns (max %d):", SPY_BUSH_TURNS);

    work = atoi (gets (work_string));
  } while (work < 0 || work > SPY_BUSH_TURNS);

  work_spy.turns_alone = work;

  do
  {
    gotoxy (1, 3);
    clreol ();

    cprintf ("Spy Lizard Mired (Yes/No)? ");

    option = toupper(getche ());
  } while (option != 'Y' && option != 'N');

  if (option == 'Y')
    work_spy.mired = 1;
  else
    work_spy.mired = 0;

  gotoxy (1, 2);
  clreol ();

  xy_to_string (work_spy.x_hex, work_spy.y_hex, work_string);

  cprintf ("%03d-P%02d/%3s/%4s/%d old",
	   work, work_spy.player, player_code [work_spy.player],
	   work_string, work_spy.turns_alone);

  if (work_spy.mired)
    cprintf ("/MIRED\n\r");
  else
    cprintf ("\n\r");

  do
  {
    gotoxy (1, 3);
    clreol ();

    cprintf ("Okay to Add (Yes/No)? ");

    option = toupper(getche ());
  } while (option != 'Y' && option != 'N');

  if (option == 'Y')
  {
    if ((spy_list [number_spys] = malloc (sizeof (spy_t))) == NULL)
    {
      cprintf ("FATAL ERROR: Out of memory. Press SPACE.\n\r");

      getch ();

      exit (EXIT_FAILURE);
    }

    gotoxy (1, 2);
    clreol ();

    cprintf ("Spy added.\n\r");

    *spy_list [number_spys] = work_spy;

    number_spys ++;

    show_spys (*spy_now_shown);
  } else {
    gotoxy (1, 2);
    clreol ();
  }
}

void change_spy (int *spy_now_shown)
{
  char work_string [80],
       option;

  int work,
      select_spy,
      x,
      y;

  spy_t work_spy;

  gotoxy (1, 3);
  clreol ();

  cprintf ("Enter Spy # to Change (0-%d)? ", number_spys - 1);

  work = atoi (gets (work_string));

  if (work < 0 || work >= number_spys)
  {
    gotoxy (1, 2);
    clreol ();

    cprintf ("Spy Lizard NOT Changed.");
  } else {
    select_spy = work;

    work_spy = *spy_list [work];

    do
    {
      gotoxy (1, 2);
      clreol ();
      cprintf ("Present Player No:%d", work_spy.player);

      gotoxy (1, 3);
      clreol ();
      cprintf ("New Player No (1-%d, ENTER=No Change)? ", players_in_game);

      gets (work_string);

      if (work_string [0] == NULL)
	break;
      else
	work = atoi (work_string);
     } while (work < 1 || work > players_in_game);

    if (work_string [0] != NULL)
      work_spy.player = work;

    do
    {
      xy_to_string (work_spy.x_hex, work_spy.y_hex, work_string);

      gotoxy (1, 2);
      clreol ();

      cprintf ("Old hex:%s", work_string);

      gotoxy (1, 3);
      clreol ();

      select_hex (&x, &y);

      if (x == -1 && y == -1)
	break;
    } while (x < 0 || x > world_x || y < 0 || y  > world_y);

    if (x != -1 && y != -1)
    {
      work_spy.x_hex = x;
      work_spy.y_hex = y;
    }

    do
    {
      gotoxy (1, 2);
      clreol ();

      switch (work_spy.lizard_type)
      {
	case RED_LIZARD:
	  cprintf ("Lizard Type: Red");
	  break;
	case GREEN_LIZARD:
	  cprintf ("Lizard Type: Green");
	  break;
	case GREY_LIZARD:
	  cprintf ("Lizard Type: Grey");
	  break;
	case BLACK_LIZARD:
	  cprintf ("Lizard Type: Black");
	  break;
	case YELLOW_LIZARD:
	  cprintf ("Lizard Type: Yellow");
	  break;
      }

      gotoxy (1, 3);
      clreol ();

      cprintf ("Enter New Lizard Type (Red/Green/grEy/Black/Yellow, ENTER=No Change)? ");

      option = toupper (getche ());
    } while (option != 'R' && option != 'G' && option != 'E' && option != 'B' &&
	     option != 'Y' && option != '\r');

    switch (option)
    {
      case 'R':
	work_spy.lizard_type = RED_LIZARD;
	break;
      case 'G':
	work_spy.lizard_type = GREEN_LIZARD;
	break;
      case 'E':
	work_spy.lizard_type = GREY_LIZARD;
	break;
      case 'B':
	work_spy.lizard_type = BLACK_LIZARD;
	break;
      case 'Y':
	work_spy.lizard_type = YELLOW_LIZARD;
	break;
      default:
	break;
    }

    do
    {
      gotoxy (1, 2);
      clreol ();

      cprintf ("Spy Lizard for %d Turns", work_spy.turns_alone);

      gotoxy (1, 3);
      clreol ();

      cprintf ("Spy Lizard for ? Turns (max %d, ENTER=No Change):", SPY_BUSH_TURNS);

      gets (work_string);

      if (work_string [0] == NULL)
	break;
      else
	work = atoi (work_string);
    } while (work < 0 || work > SPY_BUSH_TURNS);

    if (work_string [0] != NULL)
      work_spy.turns_alone = work;

    do
    {
      gotoxy (1, 2);
      clreol ();

      if (work_spy.mired)
	cprintf ("Spy Lizard IS Mired.");
      else
	cprintf ("Spy Lizard is NOT Mired.");

      gotoxy (1, 3);
      clreol ();

      cprintf ("Spy Lizard Mired (Yes/No, ENTER=No Change)? ");

      option = toupper(getche ());
    } while (option != 'Y' && option != 'N' && option != '\r');

    if (option == 'Y')
      work_spy.mired = 1;
    else
      if (option == 'N')
      work_spy.mired = 0;

    gotoxy (1, 2);
    clreol ();

    xy_to_string (work_spy.x_hex, work_spy.y_hex, work_string);

    cprintf ("%03d-P%02d/%3s/%4s/%d old",
	     work, work_spy.player, player_code [work_spy.player],
	     work_string, work_spy.turns_alone);

    if (work_spy.mired)
      cprintf ("/MIRED\n\r");
    else
      cprintf ("\n\r");

    do
    {
      gotoxy (1, 3);
      clreol ();

      cprintf ("Okay to Change (Yes/No)? ");

      option = toupper(getche ());
    } while (option != 'Y' && option != 'N');

    if (option == 'Y')
    {
      *spy_list [select_spy] = work_spy;

      gotoxy (1, 2);
      clreol ();

      cprintf ("Spy Lizard Changed.");

      show_spys (*spy_now_shown);
    } else {
      gotoxy (1, 2);
      clreol ();

      cprintf ("Spy Lizard NOT Changed.");
    }
  }
}

void delete_spy (int *spy_now_shown)
{
  char work_string [80],
       option;

  int work;

  gotoxy (1, 3);
  clreol ();

  cprintf ("Enter Spy # to Delete (0-%d)? ", number_spys - 1);

  work = atoi (gets (work_string));

  if (work < 0 || work >= number_spys)
  {
    gotoxy (1, 2);
    clreol ();

    cprintf ("Spy Lizard NOT Deleted.");
  } else {
    gotoxy (1, 2);
    clreol ();

    xy_to_string (spy_list [work]->x_hex, spy_list [work]->y_hex, work_string);

    cprintf ("%03d-P%02d/%3s/%4s/%d old",
	     work, spy_list [work]->player, player_code [spy_list [work]->player],
	     work_string, spy_list [work]->turns_alone);

    if (spy_list [work]->mired)
      cprintf ("/MIRED\n\r");
    else
      cprintf ("\n\r");

    do
    {
      gotoxy (1, 3);
      clreol ();

      cprintf ("Okay to Delete (Yes/No)? ");

      option = toupper(getche ());
    } while (option != 'Y' && option != 'N');

    if (option == 'Y')
    {
      spy_list [work]->player = 0;

      gotoxy (1, 2);
      clreol ();

      cprintf ("Spy Lizard Deleted.");

      if (work == *spy_now_shown)
	*spy_now_shown -= 1;

      show_spys (*spy_now_shown);
    } else {
      gotoxy (1, 2);
      clreol ();

      cprintf ("Spy Lizard NOT Deleted.");
    }
  }
}

void search_by_player (int *spy_now_shown)
{
  char work_string [80];

  int work,
      player,
      spy_search_from;

  gotoxy (1, 3);
  clreol ();

  cprintf ("Enter Player # for Search (0-%d)? ", players_in_game);

  player = atoi (gets (work_string));

  if (player < 0 || player > players_in_game)
  {
    gotoxy (1, 2);
    clreol ();

    cprintf ("Invalid Player #.");
  } else {
    gotoxy (1, 3);
    clreol ();

    cprintf ("Search from Beginning of list or from Present point (B/p)? ");

    work = toupper (getche ());

    if (work == 'B' || work == '\r')
      spy_search_from = 0;
    else
      spy_search_from = (*spy_now_shown) + 1;

    for (work = spy_search_from; work < number_spys; work ++)
      if (spy_list [work]->player == player)
      {
	*spy_now_shown = work;

	show_spys (*spy_now_shown);

	break;
      }
  }
}

void show_spys (int start)
{
  char work_string [80];

  int work,
      work2;

  reopen_text_window (&list_win);

  clrscr ();

  for (work = start, work2 = 0; work < number_spys && work2 < 16; work ++)
    if (spy_list [work]->player > 0)
    {
      xy_to_string (spy_list [work]->x_hex, spy_list [work]->y_hex, work_string);

      cprintf ("%03d-P%02d/%3s/%4s/%d old",
	       work, spy_list [work]->player, player_code [spy_list [work]->player],
	       work_string, spy_list [work]->turns_alone);

      if (spy_list [work]->mired)
       	cprintf ("/MIRED");

      if (work2 < 15)
        cprintf ("\n\r");

      work2 ++;
    }

  reopen_text_window (&work_win);
}

void select_hex (int *x, int *y)
{
  char work_string [256],
       temp_string [256];

  *x = -1;
  *y = -1;

  do
  {
    gotoxy (1, 3);
    clreol ();
    gotoxy (1, 3);

    cprintf ("Enter hex (ENTER=No Change)? ");

    gets (work_string);

    if (!isdigit (work_string [0])) return;

    *y = atoi (work_string);

    if (*y < 10)
      strcpy (temp_string, &(work_string [1]));
    else
      strcpy (temp_string, &(work_string [2]));

    strupr (temp_string);

    if (strlen (temp_string) > 1)
    {
      *x = temp_string [1] - 64;
      *x += ((temp_string [0] - 64) * 26);
    } else
      *x = temp_string [0] - 64;
  } while (*x <= 0 || *y <= 0 || *x > world_x || *y > world_y);

  *x -= 1;
  *y -= 1;

  if (*y > world_y)
    *y = world_y - 1;

  if (*x > world_x)
    *x = world_x - 1;

  if (*y < 0)
    *y = 0;

  if (*x < 0)
    *x = 0;

  gotoxy (1, 3);
  clreol ();
}
