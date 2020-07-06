#include <io.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "strgfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

#include "textwind.h"
#include "textwinf.h"

#define WORK_WINDOW 1, 1, 80, 19, LIGHTGRAY, BLACK
#define MENU_WINDOW 1, 20, 75, 24, BLACK, LIGHTGRAY

int main (int argc, char *argv []);
void template_read (char *filename);
void show_world (int *x, int *y);
void show_hex (int x, int y, int scr_x, int scr_y);
void update_hexes (int x, int y, int wrld_x, int wrld_y);
void select_hex (int *x, int *y);

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

win_t menu_win,
      work_win;

int main (int argc, char *argv [])
{
  char work_string [255],
       filename [255];

  int work,
      work2,
      option,
      option2,
      work_x,
      work_y,
      world_show_x,
      world_show_y;

  hex_t world_hex;

  strcpy (player_code [0], "Free");

  randomize ();

  if (argc == 3)
  {
    strcpy (data_path, argv [1]);
    strncpy (game_code, argv [2], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    cprintf ("\n\rLIZARDS! World Editor. %s Last compiled %s.\n\r\n\r",
            VER_STRING, __DATE__);

    cprintf ("FATAL ERROR: Game Number not specified on command line!\n\r"
	     "             Command Line Format: EXEFILE data_path game.\n\r"
	     "                             e.g. WORLEDIT C:\\ A\n\r");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  get_player_list ();

  open_text_window (&work_win, WORK_WINDOW, NO_BORDER);

  cprintf ("        ____        ____        ____        ____        ____        ____\n\r");

  for (work = 0; work < 5; work ++)
  {

    cprintf ("\\      /    \\      /    \\      /    \\      /    \\      /    \\      /    \\\n\r");

    if (work == 4)
    {
      cprintf (" \\____/      \\____/      \\____/      \\____/      \\____/      \\____/      \\");
      break;
    } else
      cprintf (" \\____/      \\____/      \\____/      \\____/      \\____/      \\____/      \\\n\r");

    cprintf (" /    \\      /    \\      /    \\      /    \\      /    \\      /    \\      /\n\r");
    cprintf ("/      \\____/      \\____/      \\____/      \\____/      \\____/      \\____/\n\r");
  }

  open_text_window (&menu_win, MENU_WINDOW, SINGLE_BORDER);

  cprintf ("LIZARDS! World Editor. %s Last compiled %s.\n\r",
	  VER_STRING, __DATE__);

  show_world (0, 0);

  world_show_x = 0;
  world_show_y = 0;

  do
  {
    gotoxy (1, 3);

    cprintf ("H = Hex edit, W = Change world view, S = Search, Q = Quit.");

    gotoxy (1, 2);

    option = toupper (getch ());

    switch (option)
    {
      case 'H':
	select_hex (&work_x, &work_y);

	get_hex (work_x, work_y, &world_hex);

	if (world_hex.raft_here > 0)
	{
	  gotoxy (1, 3);
	  clreol ();

	  cprintf ("Raft here...sinks in %d turns.",
		  world_hex.raft_here);

	  gotoxy (1, 2);
	}

	cprintf ("Pla,Den,Hom,Rui,Tem,Fer,Swa,sCr,cUr,peK,Vol,Wat,whI,Bri,rAf,RET=Same? ");

	option2 = toupper (getch ());

	switch (option2)
	{
	  case 'A':
	    gotoxy (1, 2);
	    clreol ();

	    cprintf ("Raft here (Y/N)? ");

	    option2 = toupper (getche ());

	    if (option2 == 'N')
	      world_hex.raft_here = 0;
	    else
	      if (option2 == 'Y')
	      {
		gotoxy (1, 2);
		clreol ();

		cprintf ("Turns till sinkin'(1-5, 1 = End of this turn!)? ");

		work = atoi (gets (work_string));

		world_hex.raft_here = work;
	      }

	    break;
	  case 'B':
            gotoxy (1, 2);
	    clreol ();

	    cprintf ("Bridge here (Y/N)? ");

	    option2 = toupper (getche ());

	    if (option2 == 'Y')
	      world_hex.bridge_here = 1;
	    else
	      if (option2 == 'N')
		world_hex.bridge_here = 0;

	    break;
	  case 'T':
	    world_hex.terrain = TEMPLE;

	    break;
	  case 'C':
	    world_hex.terrain = SCRUB;

            break;
	  case 'U':
	    world_hex.terrain = CURSED;

	    break;
	  case 'P':
	    world_hex.terrain = PLAIN;

            break;
	  case 'D':
	  case 'H':
	    if (option2 == 'D')
	      world_hex.terrain = DEN;
	    else
	      world_hex.terrain = HOME_DEN;

            gotoxy (1, 2);
	    clreol ();

	    cprintf ("Den Type (Red,Grn,grEy,Black,Yellow,Unsettle,rAndom)? ");

            option2 = toupper (getch ());

	    switch (option2)
	    {
	      case 'R':
		world_hex.den_lizard_type = RED_LIZARD;

		break;
	      case 'G':
		world_hex.den_lizard_type = GREEN_LIZARD;

		break;
	      case 'E':
		world_hex.den_lizard_type = GREY_LIZARD;

		break;
	      case 'B':
		world_hex.den_lizard_type = BLACK_LIZARD;

		break;
	      case 'Y':
		world_hex.den_lizard_type = YELLOW_LIZARD;

		break;
	      case 'U':
		world_hex.den_lizard_type = 0;

		break;
	      default:
		world_hex.den_lizard_type = random (5) + 1;

		break;
	    }

	    break;
	  case 'R':
	    world_hex.terrain = RUIN;

            break;
	  case 'F':
	    world_hex.terrain = FERTILE;

	    break;
	  case 'S':
	    world_hex.terrain = SWAMP;

	    break;
	  case 'K':
	    world_hex.terrain = PEAK;

	    break;
	  case 'V':
	    world_hex.terrain = VOLCANO;

	    break;
	  case 'W':
	    world_hex.terrain = WATER;

	    break;
	  case 'I':
	    world_hex.terrain = WHIRLPOOL;

	    break;
	  default:
	    break;
	}

	do
	{
	  work2 = (world_hex.red_lizards + world_hex.green_lizards +
		   world_hex.grey_lizards + world_hex.black_lizards +
		   world_hex.yellow_lizards + world_hex.lizard_pop);

          gotoxy (1, 2);
	  clreol ();

	  cprintf ("Liz (Red,Grn,grEy,Blk,Yel,Imble,Pop,Strv,Cmbt,RET=Quit (%d here))? ", work2);

          option2 = toupper (getch ());

	  gotoxy (1, 2);
	  clreol ();

	  switch (option2)
	  {
	    case 'R':
	      cprintf ("%d Red Lizards here, How many? ", world_hex.red_lizards);

	      gets (work_string);

	      work = atoi (work_string);

	      if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.red_lizards = work;

	      break;
	    case 'G':
	      cprintf ("%d Green Lizards here, How many? ", world_hex.green_lizards);

	      gets (work_string);

	      work = atoi (work_string);

	      if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.green_lizards = work;

	      break;
	    case 'E':
	      cprintf ("%d Grey Lizards here, How many? ", world_hex.grey_lizards);

	      gets (work_string);

	      work = atoi (work_string);

	      if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.grey_lizards = work;

	      break;
	    case 'B':
	      cprintf ("%d Black Lizards here, How many? ", world_hex.black_lizards);

	      gets (work_string);

	      work = atoi (work_string);

	      if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.black_lizards = work;

	      break;
	    case 'Y':
	      cprintf ("%d Yellow Lizards here, How many? ", world_hex.yellow_lizards);

	      gets (work_string);

	      work = atoi (work_string);

	      if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.yellow_lizards = work;

	      break;
	    case 'P':
	      cprintf ("%d Population Lizards here (Dens only!), How many? ", world_hex.lizard_pop);

	      gets (work_string);

	      work = atoi (work_string);

              if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.lizard_pop = work;

	      break;
	    case 'I':
	      cprintf ("Lizards here lizards immobile=%d, Set to? ", world_hex.lizards_immobile);

	      gets (work_string);

	      work = atoi (work_string);

              if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.lizards_immobile = work;

	      break;
	    case 'S':
	      cprintf ("Lizards here hungry %d turns, How many? ", world_hex.turns_hungry);

	      gets (work_string);

	      work = atoi (work_string);

              if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.turns_hungry = work;

	      break;
	    case 'C':
	      cprintf ("Lizards here combat points %d, How many? ", world_hex.combat_points);

	      gets (work_string);

	      work = atoi (work_string);

	      if (work > 0 || (work == 0 && work_string [0] == '0'))
		world_hex.combat_points = work;

	      break;
	    default:
	      option2 = 'Q';

	      break;
	  }
	} while (option2 != 'Q');

	do
	{
	  gotoxy (1, 2);
	  clreol ();

	  cprintf ("Hex Owner (1-%d, 0=No Owner, RET=No change)? ",
		  players_in_game);

	  option2 = atoi (gets (work_string));

	  if (option2 < 0)
	    option2 = 0;

	  if ((option2 <= players_in_game && option2 > 0) ||
	      (option2 == 0 && work_string [0] == '0'))
	    world_hex.hex_owner = option2;
	} while (option2 > players_in_game);

	gotoxy (1, 2);
	clreol ();

	put_hex (work_x, work_y, &world_hex);

	update_hexes (work_x, work_y, world_show_x, world_show_y);

	break;
      case 'W':
	select_hex (&world_show_x, &world_show_y);

	world_show_x -= 5;
	world_show_y -= 2;

	show_world (&world_show_x, &world_show_y);

	break;
      /* no longer required due to custom startup
      case 'T':
	cprintf ("Template to read (1-4, ENTER = Abort)? ");

	option = atoi (gets (work_string));

	if (option > 0 && option < 5)
	{
	  strcpy (filename, "LIZGEN");
	  strcat (filename, work_string);
	  strcat (filename, ".TMP");

	  template_read (filename);

	  show_world (&world_show_x, &world_show_y);
	}

	break;
      */
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
	    get_hex (work_x, work_y, &world_hex);

	    if (world_hex.hex_owner == option2)
	      if (lizards_in_hex (&world_hex) > 0 ||
		world_hex.lizard_pop > 0)
	      {
		world_show_x = work_x;
		world_show_y = work_y;

		world_show_x -= 5;
		world_show_y -= 2;

		show_world (&world_show_x, &world_show_y);

		work_x = world_x;
		work_y = world_y;
	      }
	  }

	  work_y = 0;
	}

	break;
      case '8':
	if (world_show_y >= 1)
	{
	  world_show_y -= 1;

	  show_world (&world_show_x, &world_show_y);
	}

	break;
      case '6':
	if (world_show_x < world_x - 8)
	{
	  world_show_x += 2;

	  show_world (&world_show_x, &world_show_y);
	}

	break;
      case '2':
	if (world_show_y < world_y - 3)
	{
	  world_show_y += 1;

	  show_world (&world_show_x, &world_show_y);
	}

	break;
      case '4':
	if (world_show_x >= 2)
	{
	  world_show_x -= 2;

	  show_world (&world_show_x, &world_show_y);
	}
	break;
      default:
	break;
    }
  } while (option != 'Q');

  close_text_window (&work_win);
  close_text_window (&menu_win);

  return (EXIT_SUCCESS);
}

void template_read (char *filename)
{
  FILE *fptr;

  char text_string [255],
       tok_string [255];

  char template_orders [][3] = { "GH", /* Get hex location */
				 "GP", /* Get player number */
				 "HT", /* Change hex terrain */
				 "HL", /* Change hex lizards */
				 "MO", /* Relative move */
				 "PO", /* Player owns hex */
				 NULL };

  int work,
      template_command,
      hex_x = -1,
      hex_y = -1,
      new_x,
      new_y,
      player = 0;

  hex_t world_hex;

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    cprintf ("FATAL ERROR: Unable to read '%s' file. Press SPACE.\n\r", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  while (fgets (text_string, 255, fptr) != NULL)
  {
    text_string [strlen (text_string) - 1] = NULL;

    strupr (text_string);

    strtoken (tok_string, text_string, " ");

    work = 0;
    template_command = -1;

    while (template_orders [work] && template_command == -1)
    {
      if (strcmp (tok_string, template_orders [work]) == 0)
	template_command = work;

      work ++;
    }

    switch (template_command)
    {
      case 0: /* GH */
	gotoxy (1, 2);
	clreol ();

	select_hex (&hex_x, &hex_y);

	break;
      case 1: /* GP */
	gotoxy (1, 2);
	clreol ();

	cprintf ("Enter player number (1-%u)? ", players_in_game);

	player = atoi (gets (text_string));

	break;
      case 2: /* HT */
	if (hex_x == -1 || hex_y == -1)
        {
          cprintf ("TEMPLATE FATAL ERROR: Hex Terrain with no selected hex. Press SPACE.\n\r");

          getch ();

	  exit (EXIT_FAILURE);
	}

	switch (text_string [0])
	{
	  case 'L':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = PLAIN;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'R':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = RUIN;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'F':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = FERTILE;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'S':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = SWAMP;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'P':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = PEAK;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'V':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = VOLCANO;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case '.':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = WATER;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'W':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = WHIRLPOOL;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'C':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = SCRUB;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'T':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = TEMPLE;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'X':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = CURSED;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case 'H':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = HOME_DEN;
            world_hex.template_hex = TRUE;

            strtoken (tok_string, text_string, " ");

	    world_hex.den_lizard_type = atoi (text_string);

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case '1':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = DEN;
	    world_hex.den_lizard_type = RED_LIZARD;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case '2':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = DEN;
	    world_hex.den_lizard_type = GREEN_LIZARD;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case '3':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = DEN;
	    world_hex.den_lizard_type = GREY_LIZARD;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case '4':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = DEN;
	    world_hex.den_lizard_type = BLACK_LIZARD;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
	  case '5':
	    get_hex (hex_x, hex_y, &world_hex);

	    world_hex.terrain = DEN;
	    world_hex.den_lizard_type = YELLOW_LIZARD;
            world_hex.template_hex = TRUE;

	    put_hex (hex_x, hex_y, &world_hex);

	    break;
          default:
            cprintf ("TEMPLATE FATAL ERROR: Hex Terrain with unknown terrain code, '%c'. Press SPACE.\n\r", text_string [0]);

            getch ();

	    exit (EXIT_FAILURE);
	}

	break;
      case 3: /* HL */
	if (hex_x == -1 || hex_y == -1)
	{
          cprintf ("TEMPLATE FATAL ERROR: Hex Lizards with no selected hex. Press SPACE.\n\r");

          getch ();

	  exit (EXIT_FAILURE);
	}

	get_hex (hex_x, hex_y, &world_hex);

	world_hex.red_lizards = atoi (text_string);

        strtoken (tok_string, text_string, " ");
	world_hex.green_lizards = atoi (text_string);

        strtoken (tok_string, text_string, " ");
	world_hex.grey_lizards = atoi (text_string);

        strtoken (tok_string, text_string, " ");
	world_hex.black_lizards = atoi (text_string);

        strtoken (tok_string, text_string, " ");
	world_hex.yellow_lizards = atoi (text_string);

        strtoken (tok_string, text_string, " ");
	world_hex.lizard_pop = atoi (text_string);
        world_hex.template_hex = TRUE;

	put_hex (hex_x, hex_y, &world_hex);

	break;
      case 4: /* MO */
	if (hex_x == -1 || hex_y == -1)
	{
          cprintf ("TEMPLATE FATAL ERROR: Relative Move with no selected hex. Press SPACE.\n\r");

          getch ();

	  exit (EXIT_FAILURE);
	}

	move_coords (atoi (text_string), hex_x, hex_y, &new_x, &new_y);

	hex_x = new_x;
	hex_y = new_y;

	break;
      case 5: /* PO */
	if (player == 0)
	{
          cprintf ("TEMPLATE FATAL ERROR: Player Own with no selected player. Press SPACE.\n\r");

          getch ();

	  exit (EXIT_FAILURE);
	}

	get_hex (hex_x, hex_y, &world_hex);

	world_hex.hex_owner = player;
        world_hex.template_hex = TRUE;

	put_hex (hex_x, hex_y, &world_hex);

	break;
    }
  }

  fclose (fptr);
}

void show_world (int *x, int *y)
{
  int work,
      work2,
      scr_x,
      scr_y,
      work_y,
      work_x;

  reopen_text_window (&work_win);

  if (abs(*x % 2) == 1)
    *x -= 1;

  for (scr_y = 0, work = *y; work < *y + 4; work ++, scr_y ++)
    for (scr_x = 0, work2 = *x; work2 < *x + 12; work2 ++, scr_x ++)
    {
      if ((work2 + 1) % 2 == 0)
	work_y = 2 + (4 * scr_y);
      else
	work_y = 4 + (4 * scr_y);

      work_x = 2 + (6 * scr_x);

      show_hex (work2, work, work_x, work_y);
    }

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
	work_y = 2 + (4 * scr_y);
      else
	work_y = 4 + (4 * scr_y);

      work_x = 2 + (6 * scr_x);

      if (work2 == x && work == y)
	show_hex (work2, work, work_x, work_y);
    }

  reopen_text_window (&menu_win);
}

void show_hex (int x, int y, int scr_x, int scr_y)
{
  char den_colour [6][4] = { "un-", "red", "grn", "gry", "blk", "yel" },
       work_string [80],
       number [10];

  int work;

  hex_t world_hex;

  if (x < 0 || y < 0 || x >= world_x || y >= world_y)
  {
    gotoxy (scr_x + 1, scr_y);
    cprintf ("    ");

    gotoxy (scr_x, scr_y + 1);
    cprintf ("      ");

    gotoxy (scr_x, scr_y + 2);
    cprintf ("      ");

    gotoxy (scr_x + 1, scr_y + 3);
    cprintf ("____");

    return;
  }

  get_hex (x, y, &world_hex);

  gotoxy (scr_x + 1, scr_y);

  if (world_hex.red_lizards > 0 ||
      world_hex.green_lizards > 0 ||
      world_hex.grey_lizards > 0 ||
      world_hex.black_lizards > 0 ||
      world_hex.yellow_lizards > 0 ||
      world_hex.lizard_pop > 0)
  {
    if (world_hex.hex_owner > 0)
      cprintf ("%s", player_code [world_hex.hex_owner]);
    else
      if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN)
        cprintf ("Free");

    gotoxy (scr_x, scr_y + 1);

    work = (world_hex.red_lizards +
	    world_hex.green_lizards +
	    world_hex.grey_lizards +
	    world_hex.black_lizards +
	    world_hex.yellow_lizards +
	    world_hex.lizard_pop);

    cprintf ("%3dliz", work);
  } else {
    cprintf ("    ");
    gotoxy (scr_x, scr_y + 1);
    cprintf ("      ");
  }

  gotoxy (scr_x, scr_y + 2);

  if (world_hex.bridge_here == 1)
    cprintf ("%s", BRIDGE_C);
  else
    if (world_hex.raft_here > 0)
      cprintf ("%s", RAFT_C);
    else
      switch (world_hex.terrain)
      {
	case PLAIN:
	  cprintf ("%s", PLAIN_C);
	  break;
	case DEN:
	  cprintf ("%s%s", den_colour [world_hex.den_lizard_type], DEN_C);
	  break;
	case HOME_DEN:
	  cprintf ("%s%s", den_colour [world_hex.den_lizard_type], HOME_DEN_C);
	  break;
	case RUIN:
	  cprintf ("%s", RUIN_C);
	  break;
	case FERTILE:
	  cprintf ("%s", FERTILE_C);
	  break;
	case SWAMP:
	  cprintf ("%s", SWAMP_C);
	  break;
	case PEAK:
	  cprintf ("%s", PEAK_C);
	  break;
	case VOLCANO:
	  cprintf ("%s", VOLCANO_C);
	 break;
	case WATER:
	  cprintf ("%s", WATER_C);
	  break;
	case WHIRLPOOL:
	  cprintf ("%s", WHIRLPOOL_C);
	  break;
	case SCRUB:
	  cprintf ("%s", SCRUB_C);
	  break;
	case TEMPLE:
	  cprintf ("%s", TEMPLE_C);
	  break;
	case CURSED:
	  cprintf ("%s", CURSED_C);
	  break;
      }

  strcpy (work_string, itoa (y + 1, number, 10));
  strcat (work_string, loc_to_string (x, number));

  gotoxy (scr_x + 1, scr_y + 3);

  cprintf ("%s", work_string);

  for (work = 0; work < 6; work ++)
    number [work] = NULL;

  strncpy (number, "____", 4 - strlen (work_string));

  cprintf ("%s", number);
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
