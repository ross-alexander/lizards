#include <io.h>
#include <conio.h>
#include <ctype.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "filefunc.h"
#include "strgfunc.h"

#include "lizhex.h"
#include "lizmisc.h"

#include "textwind.h"
#include "textwinf.h"

#include "hyplink.h"
#include "hyphelpf.h"

#define WORK_WINDOW 1, 1, 80, 22, LIGHTGRAY, BLACK
#define MENU_WINDOW 1, 23, 75, 25, LIGHTGRAY, BLACK

#define ctextattr(attr) if (!mono_switch) textattr ((attr))
#define ctextcolor(color) if (!mono_switch) textcolor ((color))
#define ctextbackground(color) if (!mono_switch) textbackground ((color))

int main (int argc, char *argv []);
void show_world (int *x, int *y, int x_offset, int y_offset);
void show_hex (int x, int y, int scr_x, int scr_y);
void update_hexes (int x, int y, int wrld_x, int wrld_y);
void show_hex_details (int x, int y, int x_offset, int y_offset);
void hex_code_attr (int x_offset, int y_offset, int attr);
void select_hex (int *x, int *y);
void get_disk_switch_list (void);
void get_disk_player_list (void);
void get_disk_world (void);
void get_disk_hex (int x, int y, hex_t *hex);
void get_disk_spy_list (void);
void draw_hex_screen (void);

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

char data_path [255],
     game_code [5];

int disk_player,
    turn_number,
    world_x,
    world_y,
    mono_switch = 0;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
	     victory_points_switch = 0,
	     home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

FILE *lizards_disk_world_fptr;

win_t menu_win,
      work_win;

int main (int argc, char *argv [])
{
  FILE *help_fptr;

  char work_string [255],
       filename [255],
       **help_index;

  int work,
      work2,
      option,
      option2,
      work_x,
      work_y,
      world_show_x,
      world_show_y,
      world_hex_x_offset = 0,
      world_hex_y_offset = 0;

  hex_t world_hex;

  strcpy (player_code [0], "Free");

  randomize ();

  cprintf ("\n\rLIZARDS! World Show Utility. %s Last compiled %s.\n\r\n\r",
    VER_STRING, __DATE__);

  if (argc >= 5)
  {
    strcpy (data_path, argv [1]);
    turn_number = atoi (argv [2]);
    strncpy (game_code, argv [3], 3);
    game_code [3] = NULL;
    strupr (game_code);
    disk_player = atoi (argv [4]);

    if (argc > 5)
    {
      if (argv [5][0] == 'M') mono_switch = 1;
    }
  } else {
    cprintf ("FATAL ERROR: Game and Turn Number not specified on command line!\n\r"
	     "             Command Line Format: EXEFILE data_path turn_number game player.\n\r"
	     "                             e.g. WORLSHOW C:\\ 6 A 5\n\r");

    exit (EXIT_FAILURE);
  }

  help_index = open_help_file (&help_fptr, "WORLSHOW.HLP", "WORLSHOW.IDX");

  get_disk_world ();

  get_disk_switch_list ();

  get_disk_player_list ();

  get_disk_spy_list ();

  open_text_window (&work_win, WORK_WINDOW, NO_BORDER);

  draw_hex_screen ();

  open_text_window (&menu_win, MENU_WINDOW, NO_BORDER);

  cprintf ("LIZARDS! World Show Utility. %s Last compiled %s.\n\r",
	  VER_STRING, __DATE__);

  show_world (0, 0, world_hex_x_offset, world_hex_y_offset);

  world_show_x = 0;
  world_show_y = 0;

  do
  {
    gotoxy (1, 3);

    cprintf ("J-Jump to hex, S-Search, T-Turn Report, O-Orders, X-eXit, ?-HELP.");

    gotoxy (1, 2);

    option = toupper (getch ());

    switch (option)
    {
      case 'J':
	select_hex (&world_show_x, &world_show_y);

	world_show_x -= 5;
	world_show_y -= 2;

	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);

	break;
      case 'S':
	gotoxy (1, 2);
	clreol ();

	cprintf ("Search for player number: ");

	option2 = atoi (gets (work_string));

	gotoxy (1, 3);
	clreol ();

	cprintf ("Search from hex? ");

	select_hex (&work_x, &work_y);

	for (;work_x < world_x; work_x ++)
	{
	  for (;work_y < world_y; work_y ++)
	  {
	    get_disk_hex (work_x, work_y, &world_hex);

	    if (world_hex.hex_owner == option2)
	      if (lizards_in_hex (&world_hex) > 0 ||
		world_hex.lizard_pop > 0)
	      {
		world_show_x = work_x;
		world_show_y = work_y;

		world_show_x -= 5;
		world_show_y -= 2;

		show_world (&world_show_x, &world_show_y, world_hex_x_offset,
		  world_hex_y_offset);

		work_x = world_x;
		work_y = world_y;
	      }
	  }

	  work_y = 0;
	}

	break;
      case 'T': /* shell to the turn report show program */
	window (1, 1, 80, 25);
	clrscr ();

	sprintf (work_string, "%s %d %s %d", data_path, turn_number,
	  game_code, disk_player);

	spawnl (P_WAIT, "TURNSHOW.EXE", "TURNSHOW.EXE", work_string, NULL);

	/* make screen look the same, clumsy I know but what the hell */

	clrscr ();
	reopen_text_window (&work_win);
	draw_hex_screen ();
	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);
	reopen_text_window (&menu_win);
	cprintf ("LIZARDS! World Show Utility. %s Last compiled %s.\n\r",
	  VER_STRING, __DATE__);

	break;
      case 'O': /* shell to order entry program */
	window (1, 1, 80, 25);
	clrscr ();

	sprintf (work_string, "%s %d %s %d", data_path, turn_number,
	  game_code, disk_player);

	spawnl (P_WAIT, "DISKORD.EXE", "DISKORD.EXE", work_string, NULL);

	/* make screen look the same, clumsy I know but what the hell */

	clrscr ();
	reopen_text_window (&work_win);
	draw_hex_screen ();
	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);
	reopen_text_window (&menu_win);
	cprintf ("LIZARDS! World Show Utility. %s Last compiled %s.\n\r",
	  VER_STRING, __DATE__);

	break;
      case '?':
	window (1, 1, 80, 25);
	clrscr ();

	sprintf (work_string, "%s %d %s %d", data_path, turn_number,
	  game_code, disk_player);

	/* enter help */

	enter_help (help_fptr, help_index);

	/* make screen look the same, clumsy I know but what the hell */

	clrscr ();
	reopen_text_window (&work_win);
	draw_hex_screen ();
	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);
	reopen_text_window (&menu_win);
	cprintf ("LIZARDS! World Show Utility. %s Last compiled %s.\n\r",
	  VER_STRING, __DATE__);

	break;
      case '8': /* screen move */
	world_show_y -= 1;

	if (world_show_y + 2 < 0)
	  if (world_wrap_switch)
	    world_show_y += world_y;
	  else
	    world_show_y += 1;

	if (!world_wrap_switch)
	{
	  /* check if cursor in bounds */

	  if (world_show_y + 2 + world_hex_y_offset < 0)
	    world_hex_y_offset ++;
	}

	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);

	break;
      case '6':
	world_show_x += 2;

	if (world_show_x + 5 >= world_x)
	  if (world_wrap_switch)
	    world_show_x -= world_x;
	  else
	    world_show_x -= 2;

	if (!world_wrap_switch)
	{
	  /* check if cursor in bounds */

	  if (world_show_x + 5 + world_hex_x_offset >= world_x)
	    world_hex_x_offset -= 2;
	}

	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);

	break;
      case '2':
	world_show_y += 1;

	if (world_show_y + 2 >= world_x)
	  if (world_wrap_switch)
	    world_show_y -= world_y;
	  else
	    world_show_y -= 1;

	if (!world_wrap_switch)
	{
	  /* check if cursor in bounds */

	  if (world_show_y + 2 + world_hex_y_offset == world_y)
	    world_hex_y_offset --;
	}

	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);

	break;
      case '4':
	world_show_x -= 2;

	if (world_show_x + 5 < 0)
	  if (world_wrap_switch)
	    world_show_x += world_x;
	  else
	    world_show_x += 2;

	if (!world_wrap_switch)
	{
	  /* check if cursor in bounds */

	  if (world_show_x + 5 + world_hex_x_offset < 0)
	    world_hex_x_offset += 2;
	}

	show_world (&world_show_x, &world_show_y, world_hex_x_offset,
	  world_hex_y_offset);

	break;
      case 0:
	option = getch ();

	switch (option)
	{
	  case 76: /* cursor centre */
	    reopen_text_window (&work_win);
	    hex_code_attr (world_hex_x_offset, world_hex_y_offset,
	      LIGHTGRAY + (BLACK << 4));
	    world_hex_x_offset = world_hex_y_offset = 0;
	    show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
	    hex_code_attr (world_hex_x_offset, world_hex_y_offset,
	      BLINK + WHITE + (BLACK << 4));
	    reopen_text_window (&menu_win);
	    break;
	  case 72: /* cursor up */
	    if (world_hex_y_offset > -2)
	    {
	      if (!world_wrap_switch)
	      {
		/* check if we're trying to go off visible world edge. */

		if (world_show_y + 2 + world_hex_y_offset == 0)
		  break;
	      }

	      world_hex_y_offset --;
	      reopen_text_window (&work_win);
	      hex_code_attr (world_hex_x_offset, world_hex_y_offset + 1,
		LIGHTGRAY + (BLACK << 4));
	      show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
	      hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		BLINK + WHITE + (BLACK << 4));
	      reopen_text_window (&menu_win);
	    }
	    break;
	  case 80: /* cursor down */
	    if (world_hex_y_offset < 2)
	    {
	      if (!world_wrap_switch)
	      {
		/* check if we're trying to go off visible world edge. */

		if (world_show_y + 2 + world_hex_y_offset == world_y - 1)
		  break;
	      }

	      world_hex_y_offset ++;
	      reopen_text_window (&work_win);
	      hex_code_attr (world_hex_x_offset, world_hex_y_offset - 1,
		LIGHTGRAY + (BLACK << 4));
	      show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
	      hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		BLINK + WHITE + (BLACK << 4));
	      reopen_text_window (&menu_win);
	    }
	    break;
	  case 71:
	    if (abs (world_hex_x_offset % 2))
	    {
	      if (world_hex_x_offset > -5)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == 0)
		    break;
		}

		world_hex_x_offset --;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset + 1, world_hex_y_offset,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    } else {
	      if (world_hex_x_offset > -5 && world_hex_y_offset > -2)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == 0 ||
		    world_show_y + 2 + world_hex_y_offset == 0)
		    break;
		}

		world_hex_x_offset --;
		world_hex_y_offset --;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset + 1, world_hex_y_offset + 1,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    }
	    break;
	  case 73:
	    if (abs (world_hex_x_offset % 2))
	    {
	      if (world_hex_x_offset < 5)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == world_x - 1)
		    break;
		}

		world_hex_x_offset ++;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset - 1, world_hex_y_offset,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    } else {
	      if (world_hex_x_offset < 5 && world_hex_y_offset > -2)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == world_x - 1 ||
		    world_show_y + 2 + world_hex_y_offset == 0)
		    break;
		}

		world_hex_x_offset ++;
		world_hex_y_offset --;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset - 1, world_hex_y_offset + 1,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    }
	    break;
	  case 79:
	    if (abs (world_hex_x_offset % 2))
	    {
	      if (world_hex_x_offset > -5 && world_hex_y_offset < 2)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == 0 ||
		    world_show_y + 2 + world_hex_y_offset == world_y - 1)
		    break;
		}

		world_hex_x_offset --;
		world_hex_y_offset ++;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset + 1, world_hex_y_offset - 1,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    } else {
	      if (world_hex_x_offset > -5)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == 0)
		    break;
		}

		world_hex_x_offset --;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset + 1, world_hex_y_offset,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    }
	    break;
	  case 81:
	    if (abs (world_hex_x_offset % 2))
	    {
	      if (world_hex_x_offset < 5 && world_hex_y_offset < 2)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == world_x - 1 ||
		    world_show_y + 2 + world_hex_y_offset == world_y - 1)
		    break;
		}

		world_hex_x_offset ++;
		world_hex_y_offset ++;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset - 1, world_hex_y_offset - 1,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    } else {
	      if (world_hex_x_offset < 5)
	      {
		if (!world_wrap_switch)
		{
		  /* check if we're trying to go off visible world edge. */

		  if (world_show_x + 5 + world_hex_x_offset == world_x - 1)
		    break;
		}

		world_hex_x_offset ++;
		reopen_text_window (&work_win);
		hex_code_attr (world_hex_x_offset - 1, world_hex_y_offset,
		  LIGHTGRAY + (BLACK << 4));
		show_hex_details (world_show_x, world_show_y, world_hex_x_offset, world_hex_y_offset);
		hex_code_attr (world_hex_x_offset, world_hex_y_offset,
		  BLINK + WHITE + (BLACK << 4));
		reopen_text_window (&menu_win);
	      }
	    }
	    break;
	  }

	break;
      default:
	break;
    }
  } while (option != 'X');

  fcloseall ();

  close_text_window (&work_win);
  close_text_window (&menu_win);

  return (EXIT_SUCCESS);
}

void show_world (int *x, int *y, int x_offset, int y_offset)
{
  int work,
      work2,
      scr_x,
      scr_y,
      work_y,
      work_x;

  reopen_text_window (&work_win);

  if (abs(*x % 2) == 1)
    if (*x >= (world_x / 2))
      *x -= 1;
    else
      *x += 1;

  for (scr_y = 0, work = *y; work < *y + 5; work ++, scr_y ++)
    for (scr_x = 0, work2 = *x; work2 < *x + 11; work2 ++, scr_x ++)
    {
      if ((work2 + 1) % 2 == 0)
	work_y = 1 + (4 * scr_y);
      else
	work_y = 3 + (4 * scr_y);

      work_x = 2 + (6 * scr_x);

      show_hex (work2, work, work_x, work_y);
    }

  /* show details of present hex */

  show_hex_details (*x, *y, x_offset, y_offset);

  hex_code_attr (x_offset, y_offset, BLINK + WHITE + (BLACK << 4));

  reopen_text_window (&menu_win);
}

void update_hexes (int x, int y, int wrld_x, int wrld_y)
{
  int work,
      work2,
      scr_x,
      scr_y,
      work_y,
      work_x;

  if (x < wrld_x || x > (wrld_x + 11) ||
      y < wrld_y || y > (wrld_y + 3))
    return;

  reopen_text_window (&work_win);

  for (scr_y = 0, work = wrld_y; work < wrld_y + 4; work ++, scr_y ++)
    for (scr_x = 0, work2 = wrld_x; work2 < wrld_x + 12; work2 ++, scr_x ++)
    {
      if ((work2 + 1) % 2 == 0)
	work_y = 1 + (4 * scr_y);
      else
	work_y = 3 + (4 * scr_y);

      work_x = 2 + (6 * scr_x);

      if (work2 == x && work == y)
	show_hex (work2, work, work_x, work_y);
    }

  reopen_text_window (&menu_win);
}

void show_hex (int x, int y, int scr_x, int scr_y)
{
  char den_colour [6][4] = { "un-", "red", "grn", "gry", "blk", "yel" },
       den_text_colour [6] = { LIGHTGRAY,
			       LIGHTRED,
			       LIGHTGREEN,
			       LIGHTGRAY,
			       BLUE,
			       YELLOW},
       work_string [80],
       number [10];

  int work;

  hex_t world_hex;

  if ((x < 0 || y < 0 || x >= world_x || y >= world_y) && world_wrap_switch)
  {
    /* adjust for wrapped world */

    if (x < 0)
      x += world_x;
    else
      if (x >= world_x)
	x -= world_x;

    if (y < 0)
      y += world_y;
    else
      if (y >= world_y)
	y -= world_y;
  }

  get_disk_hex (x, y, &world_hex);

  if (world_hex.terrain == -1 || x < 0 || y < 0 || x >= world_x ||
    y >= world_y)
  {
    ctextattr (LIGHTGRAY + (BLACK << 4));
  } else
    switch (world_hex.terrain)
    {
      case PLAIN:
	ctextbackground (BROWN);
	break;
      case DEN:
      case HOME_DEN:
      case RUIN:
      case TEMPLE:
	ctextbackground (BLACK);
	break;
      case FERTILE:
      case SCRUB:
	ctextbackground (GREEN);
	break;
      case SWAMP:
	ctextbackground (MAGENTA);
	break;
      case PEAK:
	ctextbackground (LIGHTGRAY);
	break;
      case VOLCANO:
	ctextbackground (RED);
	break;
      case WATER:
      case WHIRLPOOL:
	ctextbackground (BLUE);
	break;
      case CURSED:
	ctextbackground (LIGHTGRAY);
	break;
    }

  gotoxy (scr_x + 1, scr_y);
  cprintf ("    ");

  gotoxy (scr_x, scr_y + 1);
  cprintf ("      ");

  gotoxy (scr_x, scr_y + 2);
  cprintf ("      ");

  if (x < 0 || y < 0 || x >= world_x || y >= world_y)
  {
    gotoxy (scr_x + 1, scr_y + 3);
    cprintf ("____");

    return;
  }

  gotoxy (scr_x + 1, scr_y);

  if (world_hex.terrain > -1) /* visible hex! */
  {
    if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN ||
	world_hex.bridge_here || world_hex.terrain == TEMPLE ||
	world_hex.red_lizards > 0 || world_hex.green_lizards > 0 ||
	world_hex.grey_lizards > 0 || world_hex.black_lizards > 0 ||
	world_hex.yellow_lizards > 0 ||	world_hex.lizard_pop > 0)
    {
      if (world_hex.hex_owner > 0)
      {
	if (world_hex.hex_owner == disk_player)
	{
	  ctextcolor (WHITE);
	} else {
	  ctextcolor (LIGHTRED);
	}

	cprintf ("%s", player_code [world_hex.hex_owner]);

	if ((world_hex.hex_owner == disk_player ||
	  world_hex.terrain != RUIN || world_hex.terrain != SCRUB) &&
	  (world_hex.red_lizards > 0 || world_hex.green_lizards > 0 ||
	  world_hex.grey_lizards > 0 || world_hex.black_lizards > 0 ||
	  world_hex.yellow_lizards > 0 || world_hex.lizard_pop > 0))
	{
	  gotoxy (scr_x, scr_y + 1);

	  work = (world_hex.red_lizards +
		  world_hex.green_lizards +
		  world_hex.grey_lizards +
		  world_hex.black_lizards +
		  world_hex.yellow_lizards +
		  world_hex.lizard_pop);

	  cprintf ("%3dliz", work);
	}
      } else
	if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN)
	{
	  ctextcolor (YELLOW);
	  cprintf ("Free");
	}
    } else {
      cprintf ("    ");
      gotoxy (scr_x, scr_y + 1);
      cprintf ("      ");
    }

    gotoxy (scr_x, scr_y + 2);

    if (world_hex.bridge_here == 1)
    {
      ctextcolor (LIGHTGRAY);
      cprintf ("%s", BRIDGE_C);
    } else
      if (world_hex.raft_here > 0)
      {
	ctextcolor (BROWN);
	cprintf ("%s", RAFT_C);
      } else
	switch (world_hex.terrain)
	{
	  case PLAIN:
	    ctextcolor (YELLOW);
	    cprintf ("%s", PLAIN_C);
	    break;
	  case DEN:
	    ctextcolor (den_text_colour [world_hex.den_lizard_type]);
	    cprintf ("%s%s", den_colour [world_hex.den_lizard_type], DEN_C);
	    break;
	  case HOME_DEN:
	    ctextcolor (den_text_colour [world_hex.den_lizard_type]);
	    cprintf ("%s%s", den_colour [world_hex.den_lizard_type], HOME_DEN_C);
	    break;
	  case RUIN:
	    ctextcolor (LIGHTGRAY);
	    cprintf ("%s", RUIN_C);
	    break;
	  case FERTILE:
	    ctextcolor (LIGHTGREEN);
	    cprintf ("%s", FERTILE_C);
	    break;
	  case SWAMP:
	    ctextcolor (LIGHTMAGENTA);
	    cprintf ("%s", SWAMP_C);
	    break;
	  case PEAK:
	    ctextcolor (WHITE);
	    cprintf ("%s", PEAK_C);
	    break;
	  case VOLCANO:
	    ctextcolor (LIGHTRED);
	    cprintf ("%s", VOLCANO_C);
	   break;
	  case WATER:
	    ctextcolor (LIGHTBLUE);
	    cprintf ("%s", WATER_C);
	    break;
	  case WHIRLPOOL:
	    ctextcolor (LIGHTCYAN);
	    cprintf ("%s", WHIRLPOOL_C);
	    break;
	  case SCRUB:
	    ctextcolor (LIGHTGREEN);
	    cprintf ("%s", SCRUB_C);
	    break;
	  case TEMPLE:
	    ctextcolor (WHITE);
	    cprintf ("%s", TEMPLE_C);
	    break;
	  case CURSED:
	    ctextcolor (BLACK);
	    cprintf ("%s", CURSED_C);
	    break;
	}
  } else {
    ctextattr (LIGHTGRAY + (BLACK << 4));

    gotoxy (scr_x + 1, scr_y);
    cprintf ("    ");

    gotoxy (scr_x, scr_y + 1);
    cprintf ("      ");

    gotoxy (scr_x, scr_y + 2);
    cprintf ("      ");

    gotoxy (scr_x + 1, scr_y + 3);
    cprintf ("____");
  }

  ctextattr (LIGHTGRAY + (BLACK << 4));

  strcpy (work_string, itoa (y + 1, number, 10));
  strcat (work_string, loc_to_string (x, number));

  gotoxy (scr_x + 1, scr_y + 3);

  cprintf ("%s", work_string);

  for (work = 0; work < 6; work ++)
    number [work] = NULL;

  strncpy (number, "____", 4 - strlen (work_string));

  cprintf ("%s", number);
}

void show_hex_details (int x, int y, int x_offset, int y_offset)
{
  unsigned int loop,
	       loop2,
	       spys;

  hex_t world_hex;

  char den_colour [6][10] = { "Unstld", "Red", "Green", "Grey", "Black",
			      "Yellow" },
       liz_cp_desc [MAX_COMBAT_POINTS + 1][10] = { "Average",
						 "Good",
						 "Dangerous",
						 "Ferocious",
						 "Fearsome",
						 "Veteran" },
       liz_hunger_desc [MAX_HUNGER_POINTS + 1][10] = { "Sated",
						     "Peckish",
						     "Hungry",
						     "Famished",
						     "Starved",
						     "STARVING" },
       work_string [80],
       number [20];

  x += 5 + x_offset;
  y += 2 + y_offset;

  if ((x < 0 || y < 0 || x >= world_x || y >= world_y) && world_wrap_switch)
  {
    /* adjust for wrapped world */

    if (x < 0)
      x += world_x;
    else
      if (x >= world_x)
	x -= world_x;

    if (y < 0)
      y += world_y;
    else
      if (y >= world_y)
	y -= world_y;
  }

  get_disk_hex (x, y, &world_hex);

  /* assuming map text window open */

  /* display hex number */

  gotoxy (69,1);
  strcpy (work_string, itoa (y + 1, number, 10));
  strcat (work_string, loc_to_string (x, number));
  cprintf ("HEX#: %s  ", work_string);

  /* display hex owner line */

  gotoxy (69,2);

  if (world_hex.terrain > - 1)
  {
    if (world_hex.bridge_here == 1)
    {
      if (world_hex.hex_owner > 0)
	cprintf ("CLAN: %s", player_code [world_hex.hex_owner]);
    } else
      if (world_hex.raft_here > 0)
      {
	if (world_hex.hex_owner > 0)
	  cprintf ("CLAN: %s", player_code [world_hex.hex_owner]);
      } else {
	switch (world_hex.terrain)
	{
	  case DEN:
	    if (world_hex.hex_owner > 0)
	      cprintf ("CLAN: %s", player_code [world_hex.hex_owner]);
	    else
	      cprintf ("FREE DEN");
	    break;
	  case HOME_DEN:
	    if (world_hex.hex_owner > 0)
	      cprintf ("CLAN: %s", player_code [world_hex.hex_owner]);
	    else
	      cprintf ("FREE HOME");
	    break;
	  case TEMPLE:
	    if (world_hex.hex_owner > 0)
	      cprintf ("CLAN: %s", player_code [world_hex.hex_owner]);
	    break;
	  default:
	    if (lizards_in_hex (&world_hex) > 0)
	      cprintf ("CLAN: %s", player_code [world_hex.hex_owner]);
	    break;
	}
      }
  }

  clreol ();

  /* display hex terrain */

  gotoxy (69,3);

  if (world_hex.terrain > -1)
  {
    if (world_hex.bridge_here == 1)
      cprintf ("Bridge ");
    else
      if (world_hex.raft_here > 0)
	cprintf ("Raft");
      else
	switch (world_hex.terrain)
	{
	  case PLAIN:
	    cprintf ("Plains");
	    break;
	  case DEN:
	    cprintf ("%s Den", den_colour [world_hex.den_lizard_type]);
	    break;
	  case HOME_DEN:
	    cprintf ("%s Home", den_colour [world_hex.den_lizard_type]);
	    break;
	  case RUIN:
	    cprintf ("Ruin");
	    break;
	  case FERTILE:
	    cprintf ("Fertile");
	    break;
	  case SWAMP:
	    cprintf ("Swamp");
	    break;
	  case PEAK:
	    cprintf ("Peak");
	    break;
	  case VOLCANO:
	    cprintf ("Volcano");
	    break;
	  case WATER:
	    cprintf ("Water");
	    break;
	  case WHIRLPOOL:
	    cprintf ("Whirlpool");
	    break;
	  case SCRUB:
	    cprintf ("Scrub");
	    break;
	  case TEMPLE:
	    cprintf ("Temple");
	    break;
	  case CURSED:
	    cprintf ("Cursed");
	    break;
	}
  }

  clreol ();

  /* display lizards in hex */

  /* wipe lizards space */

  for (loop = 0; loop < 13; loop ++)
  {
    gotoxy (69, 4 + loop);
    clreol ();
  }

  if (world_hex.terrain > -1)
  {
    loop = 0;

    if (world_hex.red_lizards > 0 || world_hex.green_lizards > 0 ||
      world_hex.grey_lizards > 0 || world_hex.black_lizards > 0 ||
      world_hex.yellow_lizards > 0 || world_hex.lizard_pop > 0)
    {
      if (world_hex.hex_owner == disk_player)
      {
	if (world_hex.lizard_pop > 0)
	{
	  loop ++;
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%03d Pop", world_hex.lizard_pop);
	  clreol ();
	}

	if (lizards_in_hex (&world_hex) > 0) loop ++; /* jam in blank line */

	if (world_hex.red_lizards > 0)
	{
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%03d Red", world_hex.red_lizards);
	  clreol ();
	}

	if (world_hex.green_lizards > 0)
	{
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%03d Green", world_hex.green_lizards);
	  clreol ();
	}

	if (world_hex.grey_lizards > 0)
	{
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%03d Grey", world_hex.grey_lizards);
	  clreol ();
	}

	if (world_hex.yellow_lizards > 0)
	{
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%03d Yellow", world_hex.yellow_lizards);
	  clreol ();
	}

	if (world_hex.black_lizards > 0)
	{
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%03d Black", world_hex.black_lizards);
	  clreol ();
	}

	if (lizards_in_hex (&world_hex) > 0)
	{
	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%s", liz_hunger_desc [world_hex.turns_hungry]);
	  clreol ();

	  gotoxy (69, 4 + (loop ++));
	  cprintf ("%s", liz_cp_desc [world_hex.combat_points]);
	  clreol ();

	  if (world_hex.lizards_immobile > 0)
	  {
	    gotoxy (69, 4 + (loop ++));

	    if (world_hex.lizards_immobile > 2)
	      cprintf ("FROZEN");
	    else
	      cprintf ("MIRED");
	  }
	}
      } else {
	gotoxy (69,4);
	cprintf ("%03d LIZ", world_hex.red_lizards + world_hex.green_lizards +
	  world_hex.grey_lizards + world_hex.black_lizards +
	  world_hex.yellow_lizards + world_hex.lizard_pop);
	clreol ();
	loop ++;
      }
    }

    /* count spys and display if > 0 */

    for (loop2 = 0, spys = 0; loop2 < number_spys; loop2 ++)
      if (spy_list [loop2]->x_hex == x && spy_list [loop2]->y_hex == y)
	spys ++;

    if (spys > 0)
    {
      loop ++;

      gotoxy (69, 4 + (loop ++));

      if (spys > 1)
	cprintf ("%d SPYS HERE", spys);
      else
	cprintf ("SPY HERE");
    }
  }
}

void hex_code_attr (int x_offset, int y_offset, int attr)
{
  char buffer [10];

  int scr_x,
      scr_y,
      loop;

  if (x_offset % 2 == 0)
    scr_y = 4 + (4 * (y_offset + 2));
  else
    scr_y = 6 + (4 * (y_offset + 2));

  scr_x = 3 + (6 * (x_offset + 5));

  gotoxy (scr_x, scr_y);

  gettext (scr_x, scr_y, scr_x + 4, scr_y, buffer);

  textattr (attr);

  for (loop = 0; buffer [loop] != '_' && loop < 8; loop +=2)
    cprintf ("%c", buffer [loop]);

  textattr (LIGHTGRAY + (BLACK << 4));
}

void select_hex (int *x, int *y)
{
  char work_string [256],
       temp_string [256];

  do
  {
    gotoxy (1, 2);
    clreol ();
    gotoxy (1, 2);

    cprintf ("Enter hex: ");

    gets (work_string);

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

  gotoxy (1, 2);
  clreol ();
}

void get_disk_switch_list (void)
{
  FILE *fptr;

  char filename [255],
       work_string [255],
       temp_string [255];

  int work;

  const char known_switches [10][10] = { "SPAWN", "CAPTURE", "V_DORMANT",
					 "P_ACTIVE", "WRAP", "V_POINTS",
					 "HOME_OWN", "HOME_VIC", "GIVE",
					 "" },
	     num_known_switches = 9;

  sprintf (filename, "%sDSW%02d%03d.%s", data_path, disk_player, turn_number,
    game_code);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  while (fgets (work_string, 255, fptr) != NULL)
  {
    work_string [strlen (work_string) - 1] = NULL;

    strupr (work_string);

    strtoken (temp_string, work_string, "=");

    for (work = 0; work < num_known_switches; work ++)
      if (strcmp (temp_string, known_switches [work]) == 0)
	break;

    switch (work)
    {
      case 0: /* SPAWN switch */
	spawn_turn_switch = 1;

	break;
      case 1: /* CAPTURE switch */
	capture_switch = 1;

	break;
      case 2: /* VOLCANO DORMANT switch */
	volcano_dormant_switch = 1;

	break;
      case 3: /* PEAK ACTIVE switch */
	peak_active_switch = 1;

	break;
      case 4: /* WORLD WRAP switch */

	world_wrap_switch = 1;

	break;
      case 5: /* VICTORY POINTS switch */
	victory_points_switch = 1;

        home_owned_worth = 2000;
        home_victory_points = 1000;

	break;
      case 6: /* HOME RANK switch, indicates what home dens are worth */
        home_owned_worth = atoi (work_string);

        if (home_owned_worth == 0)
        {
          printf ("FATAL ERROR: HOME_OWN switch used with no parameter. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
        }

	break;
      case 7: /* HOME POINTS switch, indicates points per home den */
	home_victory_points = atoi (work_string);

        if (home_victory_points == 0)
        {
	  printf ("FATAL ERROR: HOME_VIC switch used with no parameter. Press SPACE.\n");

          getch ();

          exit (EXIT_FAILURE);
        }

        break;
      case 8: /* GIVE order allowed */
	give_switch = 1;

	break;
      default:
	break;
    }
  }

  fclose (fptr);
}

void get_disk_player_list (void)
{
  FILE *fptr;

  int loop;

  char filename [255],
       work_string [255],
       *next_comma,
       *last_comma;

  for (loop = 1; loop < MAX_PLAYERS + 1; loop ++)
    strcpy (player_name [loop], "****");

  sprintf (filename, "%sDWP%02d%03d.%s", data_path, disk_player, turn_number,
    game_code);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  players_in_game = 1;

  fgets (game_due_date, 20, fptr);

  game_due_date [strlen (game_due_date) - 1] = NULL;

  while (fgets (work_string, 255, fptr) != NULL &&
    players_in_game < MAX_PLAYERS + 1)
  {
    work_string [strlen (work_string) - 1] = NULL;

    last_comma = work_string;
    next_comma = strchr (work_string, ',');
    next_comma [0] = NULL;
    strncpy (player_code [players_in_game], last_comma, 6);
    player_code [players_in_game][5] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_clan_name [players_in_game], last_comma, 35);
    player_clan_name [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    player_output_mode [players_in_game] = last_comma [0];

    players_in_game ++;
  }

  players_in_game --;

  fclose (fptr);
}


void get_disk_world (void)
{
  FILE *fptr;

  char filename [255];

  sprintf (filename, "%sDST%02d%03d.%s", data_path, disk_player, turn_number,
    game_code);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  fscanf (fptr, "%d\n", &world_x);
  fscanf (fptr, "%d\n", &world_y);

  fclose (fptr);
}

void get_disk_hex (int x, int y, hex_t *hex)
{
  char filename [255];

  if (lizards_disk_world_fptr == NULL)
  {
    sprintf (filename, "%sDWO%02d%03d.%s", data_path, disk_player, turn_number,
      game_code);

    if ((lizards_disk_world_fptr = fopen (filename, "r+b")) == NULL)
    {
      printf ("FATAL ERROR: Unable to read/write %s file. Press SPACE.\n", filename);

      getch ();

      exit (EXIT_FAILURE);
    }
  }

  get_rec (lizards_disk_world_fptr, sizeof (hex_t), (y * world_x) + x, hex);
}

void get_disk_spy_list (void)
{
  FILE *fptr;

  char filename [255];

  int work;

  spy_t work_spy;

  sprintf (filename, "%sDSP%02d%03d.%s", data_path, disk_player, turn_number,
    game_code);

  if ((fptr = fopen (filename, "rb")) == NULL)
  {
    number_spys = 0;
    return;
  }

  number_spys = num_records (fptr, sizeof (spy_t));

  for (work = 0; work < number_spys; work ++)
  {
    get_rec (fptr, sizeof (spy_t), work, &work_spy);

    if ((spy_list [work] = malloc (sizeof (spy_t))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    *(spy_list [work]) = work_spy;
  }

  fclose (fptr);
}

void draw_hex_screen (void)
{
  int work;

  for (work = 0; work < 6; work ++)
  {
    cprintf ("\\      /    \\      /    \\      /    \\      /    \\      /    \\      /\n\r");

    if (work == 5)
    {
      cprintf (" \\____/      \\____/      \\____/      \\____/      \\____/      \\____/");
      break;
    } else
      cprintf (" \\____/      \\____/      \\____/      \\____/      \\____/      \\____/\n\r");

    cprintf (" /    \\      /    \\      /    \\      /    \\      /    \\      /    \\\n\r");
    cprintf ("/      \\____/      \\____/      \\____/      \\____/      \\____/      \\\n\r");
  }
}
