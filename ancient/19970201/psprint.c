#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "strgfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

typedef struct
{
  int x,
      y,
      spy,
      peak;
} unit_t;

typedef struct
{
  char x,
       y;
} see_t;

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

int number_units [MAX_PLAYERS + 1];
unit_t *player_units [MAX_PLAYERS + 1][500];

int main (int argc, char *argv []);
void ps_show_world (int player, FILE *fptr);
see_t *build_see_list (int player, int *list_len);
int build_see_list_sort (const void *elem1, const void *elem2);
void see_wrap_hex (char *x, char *y);
void ps_show_hex (int x, int y, int player, FILE *fptr);
int spy_at (int x, int y, int player);
char *brace_string (char *string);
char *ps_hack_braces_string (char *string);
void break_str (char *string, char *string2, int length);
void build_unit_list (void);

int main (int argc, char *argv [])
{
  FILE *in_fptr,
       *out_fptr,
       *disk_out_fptr;

  char work_string [255],
       filename [20],
       out_filename [20],
       number [20];

  int work;

  show_title ();

  printf ("LIZARDS! POSTSCRIPT Print Utility. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc != 6)
  {
    printf ("FATAL ERROR: Turn, Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path disk_data_path turn_number game generic.\n"
	     "                             e.g. PSPRINT C:\\ D:\\ 1 A N\n");

    exit (EXIT_FAILURE);
  }

  data_path = LzCheckDir(argv[1], 0);
  disk_data_path = LzCheckDir(argv[2], 0);
  turn_number = atoi (argv[3]);
  game_code = (char*)malloc(strlen(argv[4]) + 1);
  strcpy (game_code, argv[4]);
  strupr (game_code);

  if (toupper (argv [5][0]) == 'N')
    generic_on = LZ_FALSE ;
  else
    generic_on = LZ_TRUE;

  get_world ();

  get_player_list ();

  if (players_in_game == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  strcpy (player_code [0], "C-G");

  get_switch_list ();

  get_spy_list ();

  sprintf (out_filename, "%s%s%03d.%s", data_path, POSTSCRIPT_F, turn_number, game_code);

  if ((out_fptr = fopen (out_filename, "at")) == NULL)
  {
    fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
    exit (EXIT_FAILURE);
  }

  fprintf (out_fptr, "/Gameletter {(%s)} def\n", game_code);
  fprintf (out_fptr, "/Turnnumber {(%d)} def\n", turn_number);
  fprintf (out_fptr, "/NTN {(%d)} def\n", turn_number + 1);
  fprintf (out_fptr, "/Date {(%s)} def\n", game_due_date);

  printf ("Building player maps...");

  build_unit_list ();

  printf (" \n");

  for (work = 0; work < players_in_game + 1; work ++)
    if (strcmp (player_name [work], "****") != 0 &&
      strcmp (player_name [work], "DROP") != 0)
    {
      switch (player_output_mode [work])
      {
	case 'D':
	case 'B':
	  /* pass through their turn report, burning the postscript header
	     and replacing an ASCII text one */

	  printf ("Translating Player %d...\n", work);

	  sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F,
            work, turn_number, game_code);

	  if ((in_fptr = fopen (filename, "rt")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
	    exit (EXIT_FAILURE);
	  }

	  sprintf (filename, "%sDPL%02d%03d.%s", disk_data_path, work, turn_number,
	    game_code);

	  if ((disk_out_fptr = fopen (filename, "wt")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", filename);
	    exit (EXIT_FAILURE);
	  }

	  /* get strings until we find 'WRITE' marker */

	  while (fgets (work_string, 255, in_fptr) != NULL)
	    if (strncmp (work_string, "Write", 5) == 0) break;

	  /* generate own player data header */

	  fprintf (disk_out_fptr, "\n * TURN REPORT INFORMATION:\n\n");

	  fprintf (disk_out_fptr, " - LIZARDS! Turn Report for Player %02d, Turn %02d in Game %s.\n",
	    work, turn_number, game_code);
	  fprintf (disk_out_fptr, " - %s you control Clan '%s' (%s).\n\n",
	    player_name [work], player_clan_name [work], player_code [work]);
	  fprintf (disk_out_fptr, " - Your present account balance is $%0.2f.\n\n",
	    atof (player_cred [work]));
	  fprintf (disk_out_fptr, " - Your present postal address is,\n");
	  fprintf (disk_out_fptr, "     %s\n", player_road [work]);
	  fprintf (disk_out_fptr, "     %s\n", player_burb [work]);
	  fprintf (disk_out_fptr, "     %s\n", player_city [work]);
	  fprintf (disk_out_fptr, "     %s\n\n", player_country [work]);

	  /* flush rest of file into disk player file */

	  while (fgets (work_string, 255, in_fptr) != NULL)
	    fprintf (disk_out_fptr, "%s", work_string);

	  fclose (in_fptr);
	  fclose (disk_out_fptr);

	  sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F,
            work, turn_number, game_code);

	  remove (filename);

	  break;
	default:
	  printf ("Postscripting Player %d...\n", work);

	  sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F,
            work, turn_number, game_code);

	  if ((in_fptr = fopen (filename, "r")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
	    exit (EXIT_FAILURE);
	  }

	  fprintf (out_fptr, "/saveobj save def\n");
	  fprintf (out_fptr, "/Shadow_Code {(%s)} def\n", player_code [work]);

	  while (fgets (work_string, 255, in_fptr) != NULL &&
	    strcmp (work_string, "Write\n") != 0)
	    fprintf (out_fptr, "%s", work_string);

	  fprintf (out_fptr, "%s[\n", work_string);

	  while (fgets (work_string, 255, in_fptr) != NULL)
	  {
	    work_string [strlen (work_string) - 1] = NULL;

	    if (strncmp (work_string, " * ", 3) == 0 && work > 0)
	    {
	      /* to activate large phase headers! */

	      fprintf (out_fptr, "Startf (%s) Endf\n",
	      ps_hack_braces_string (&work_string [2]));
	    } else {
	      fprintf (out_fptr, "%s Linef\n", brace_string (work_string));
	    }
	  }

	  fprintf (out_fptr, "] format\n");

	  fprintf (out_fptr, "pr\n");

	  fclose (in_fptr);

	  remove (filename);

	  if (number_units [work] > 0)
	    ps_show_world (work, out_fptr);

	  fprintf (out_fptr, "saveobj restore\n");
      }
    } else {
      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, work,
        turn_number, game_code);

      remove (filename);
    }

  fprintf (out_fptr, "\x04");

  fclose (out_fptr);

  printf ("\nFinished!\n");

  return (EXIT_SUCCESS);
}

void ps_show_world (int player, FILE *fptr)
{
  char text_string [255];

  int page_one_offset,
      page_x,
      page_y,
      range,
      work,
      high_end,
      hex_per_page;

  see_t *player_hex_see_list;

  if (number_units [player] == 0)
    return;

  if ((world_y / 2) + 1 > 16)
    hex_per_page = 18;
  else
    hex_per_page = 16;

  player_hex_see_list = build_see_list (player, &high_end);

  if ((range = player_hex_see_list [high_end - 1].y -
       player_hex_see_list [0].y + 1) > hex_per_page)
  {
    if (hex_per_page > 16)
    {
      page_one_offset = 18;

      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_Full_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location_Full\n");
      }
    } else {
      page_one_offset = 16;

      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location\n");
      }
    }

    fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 2. World size 1A to %s) Map_Page_Num\n",
		    player_clan_name [player], game_code, turn_number,
		    xy_to_string (world_x - 1, world_y - 1, text_string));

    for (work = 0; player_hex_see_list [work].y < page_one_offset; work ++)
    {
      page_x = (player_hex_see_list [work].x * 35) + 25;

      if (player_hex_see_list [work].x % 2 == 1)
	page_y = 775 - (player_hex_see_list [work].y * 50);
      else
	page_y = 750 - (player_hex_see_list [work].y * 50);

      fprintf (fptr, "%d %d ", page_x, page_y);

      ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
		   player, fptr);
    }

    fprintf (fptr, "printing\n");

    if ((player_hex_see_list [high_end - 1].y -
	 hex_per_page + 1) > 16)
    {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_Full_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location_Full\n");
      }
    } else {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location\n");
      }
    }

    fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 2 of 2. World size 1A to %s) Map_Page_Num\n",
		    player_clan_name [player], game_code, turn_number,
		    xy_to_string (world_x - 1, world_y - 1, text_string));

    for (; work < high_end; work ++)
      if (player_hex_see_list [work].y >= page_one_offset)
      {
	page_x = (player_hex_see_list [work].x * 35) + 25;

	if (player_hex_see_list [work].x % 2 == 1)
	  page_y = 775 - ((player_hex_see_list [work].y - page_one_offset) * 50);
	else
	  page_y = 750 - ((player_hex_see_list [work].y - page_one_offset) * 50);

	fprintf (fptr, "%d %d ", page_x, page_y);

	ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
		     player, fptr);
      }

      fprintf (fptr, "printing\n");
  } else {
    page_one_offset = player_hex_see_list [0].y;

    if (player_hex_see_list [0].y != 0 ||
	player_hex_see_list [0].y != hex_per_page)
    {
      if (hex_per_page > 16)
      {
	if (range > 16)
	{
	  if (world_wrap_switch)
	  {
	    fprintf (fptr, "Map_Page_R\n");
	    fprintf (fptr, "-1190 140 Location_Full_R\n");
	  } else {
	    fprintf (fptr, "Map_Page\n");
	    fprintf (fptr, "-1169 140 Location_Full\n");
	  }
	} else {
	  if (world_wrap_switch)
	  {
	    fprintf (fptr, "Map_Page_R\n");
	    fprintf (fptr, "-1190 140 Location_R\n");
	  } else {
	    fprintf (fptr, "Map_Page\n");
	    fprintf (fptr, "-1169 140 Location\n");
	  }
	}
      } else {
	if (world_wrap_switch)
	{
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_R\n");
	} else {
	  fprintf (fptr, "Map_Page\n");
	  fprintf (fptr, "-1169 140 Location\n");
	}
      }

      fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 1. World size 1A to %s) Map_Page_Num\n",
		      player_clan_name [player], game_code, turn_number,
		      xy_to_string (world_x - 1, world_y - 1, text_string));

      for (work = 0; work < high_end; work ++)
      {
	page_x = (player_hex_see_list [work].x * 35) + 25;

	if (player_hex_see_list [work].x % 2 == 1)
	  page_y = 775 - ((player_hex_see_list [work].y - page_one_offset) * 50);
	else
	  page_y = 750 - ((player_hex_see_list [work].y - page_one_offset) * 50);

	fprintf (fptr, "%d %d ", page_x, page_y);

	ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
		     player, fptr);
      }

      fprintf (fptr, "printing\n");
    } else {
      if (hex_per_page > 16)
      {
	if (world_wrap_switch)
	{
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_Full_R\n");
	} else {
	  fprintf (fptr, "Map_Page\n");
	  fprintf (fptr, "-1169 140 Location_Full\n");
	}
      } else {
	if (world_wrap_switch)
	{
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_R\n");
	} else {
	  fprintf (fptr, "Map_Page\n");
	  fprintf (fptr, "-1169 140 Location\n");
	}
      }

      fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 1. World size 1A to %s) Map_Page_Num\n",
		      player_clan_name [player], game_code, turn_number,
		      xy_to_string (world_x - 1, world_y - 1, text_string));

      for (work = 0; work < high_end; work ++)
      {
	page_x = (player_hex_see_list [work].x * 35) + 25;

	if (player_hex_see_list [work].x % 2 == 1)
	  page_y = 775 - ((player_hex_see_list [work].y - page_one_offset) * 50);
	else
	  page_y = 750 - ((player_hex_see_list [work].y - page_one_offset) * 50);

	fprintf (fptr, "%d %d ", page_x, page_y);

	ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
		     player, fptr);
      }

      fprintf (fptr, "printing\n");
    }
  }

  free (player_hex_see_list);
}

see_t *build_see_list (int player, int *list_len)
{
  /* in y,x order! */

  char one_hex_even  [6][2] = { -1,0,-1,-1,-1,1,0,-1,0,1,1,0 },
       one_hex_odd [6][2] = { -1,0,0,-1,0,1,1,-1,1,1,1,0 },
       two_hex_even  [12][2] = { -2,0,-2,-1,-2,1,-1,-2,-1,2,0,-2,0,2,1,-2,
				1,2,1,-1,1,1,2,0 },
       two_hex_odd [12][2] = { -2,0,-1,-1,-1,1,-1,-2,-1,2,0,-2,0,2,1,-2,
				1,2,2,-1,2,1,2,0 },
       three_hex_even [18][2] = { -3,0,-3,-1,-3,1,-2,-2,-2,2,-2,-3,-2,3,
				  -1,-3,-1,3,0,-3,0,3,1,-3,1,3,2,-2,2,2,
				  2,-1,2,1,3,0 },
       three_hex_odd [18][2] = { -3,0,-2,-1,-2,1,-2,-2,-2,2,-1,-3,-1,3,
				  0,-3,0,3,1,-3,1,3,2,-3,2,3,2,-2,2,2,3,-1,
				  3,1,3,0 };

  int work,
      work2;

  see_t *see_ptr,
	 work_see;

  unsigned int total_see_hex,
	       see_hex;

  total_see_hex = 0;
  see_hex = 0;

  for (work = 0; work < number_units [player]; work ++)
    if (player_units [player][work]->peak)
      total_see_hex += 37;
    else
      total_see_hex += 19;

  if ((see_ptr = malloc (sizeof (see_t) * total_see_hex)) == NULL)
  {
    printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

    getch ();

    exit (EXIT_FAILURE);
  }

  for (work = 0; work < number_units [player]; work ++)
  {
    work_see.x = player_units [player][work]->x;
    work_see.y = player_units [player][work]->y;

    see_ptr [see_hex] = work_see;

    see_hex ++;

    if (player_units [player][work]->peak)
    {
      if (player_units [player][work]->x % 2 == 0)
      {
	for (work2 = 0; work2 < 6; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       one_hex_odd [work2][1];
	  work_see.y = player_units [player][work]->y +
		       one_hex_odd [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}

	for (work2 = 0; work2 < 12; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       two_hex_odd [work2][1];
	  work_see.y = player_units [player][work]->y +
		       two_hex_odd [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}

	for (work2 = 0; work2 < 18; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
			 three_hex_odd [work2][1];
	  work_see.y = player_units [player][work]->y +
		       three_hex_odd [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}
      } else {
	for (work2 = 0; work2 < 6; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		     one_hex_even [work2][1];
	  work_see.y = player_units [player][work]->y +
		     one_hex_even [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}

	for (work2 = 0; work2 < 12; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       two_hex_even [work2][1];
	  work_see.y = player_units [player][work]->y +
		       two_hex_even [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}

	for (work2 = 0; work2 < 18; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       three_hex_even [work2][1];
	  work_see.y = player_units [player][work]->y +
		       three_hex_even [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}
      }
    } else {
      if (player_units [player][work]->x % 2 == 0)
      {
	for (work2 = 0; work2 < 6; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       one_hex_odd [work2][1];
	  work_see.y = player_units [player][work]->y +
		       one_hex_odd [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}

	for (work2 = 0; work2 < 12; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       two_hex_odd [work2][1];
	  work_see.y = player_units [player][work]->y +
		       two_hex_odd [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}
      } else {
	for (work2 = 0; work2 < 6; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       one_hex_even [work2][1];
	  work_see.y = player_units [player][work]->y +
		       one_hex_even [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}

	for (work2 = 0; work2 < 12; work2 ++)
	{
	  work_see.x = player_units [player][work]->x +
		       two_hex_even [work2][1];
	  work_see.y = player_units [player][work]->y +
		       two_hex_even [work2][0];

	  see_wrap_hex (&work_see.x, &work_see.y);

	  if (work_see.x > -1 && work_see.x < world_x &&
	      work_see.y > -1 && work_see.y < world_y)
	  {
	    see_ptr [see_hex] = work_see;

	    see_hex ++;
	  }
	}
      }
    }
  }

  *list_len = see_hex;

  for (work = 0; work < see_hex; work ++)
    for (work2 = 0; work2 < see_hex; work2 ++)
      if (see_ptr [work].x == see_ptr [work2].x &&
	  see_ptr [work].y == see_ptr [work2].y &&
	  work != work2)
      {
	see_ptr [work2].x = 127;
	see_ptr [work2].y = 127;
      }

  qsort (see_ptr, see_hex, sizeof (see_t), build_see_list_sort);

  for (work = 0; work < see_hex; work ++)
    if (see_ptr [work].y == 127)
    {
      *list_len = work;
      break;
    }

  return (see_ptr);
}

void see_wrap_hex (char *x, char *y)
{
  if (world_wrap_switch)
  {
    if (*x < 0)
      *x += world_x;
    else
      if (*x >= world_x)
	*x -= world_x;

    if (*y < 0)
      *y += world_y;
    else
      if (*y >= world_y)
	*y -= world_y;
  }
}

int build_see_list_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->y < ((see_t *)elem2)->y)
    return (-1);
  else
    if (((see_t *)elem1)->y > ((see_t *)elem2)->y)
      return (1);
    else
      return (0);
}

void ps_show_hex (int x, int y, int player, FILE *fptr)
{
  char string [80];

  hex_t hex;

  get_hex (x, y, &hex);

  if (hex.bridge_here == 1 || hex.raft_here > 0 ||
      (hex.turns_undisturbed > 0 && hex.terrain != HOME_DEN))
  {
    if (hex.bridge_here == 1)
      fprintf (fptr, BRIDGE_PC);
    else
      if (hex.turns_undisturbed > 0)
	fprintf (fptr, HALF_DEN_PC);
      else
	fprintf (fptr, RAFT_PC);
  } else {
    switch (hex.terrain)
    {
      case PLAIN:
	fprintf (fptr, PLAIN_PC);
	break;
      case DEN:
	switch (hex.den_lizard_type)
	{
	  case 0:
	    fprintf (fptr, EMPTY_DEN_PC);
	    break;
	  case RED_LIZARD:
	    fprintf (fptr, RED_DEN_PC);
	    break;
	  case GREEN_LIZARD:
	    fprintf (fptr, GRN_DEN_PC);
	    break;
	  case GREY_LIZARD:
	    fprintf (fptr, GRY_DEN_PC);
	    break;
	  case BLACK_LIZARD:
	    fprintf (fptr, BLK_DEN_PC);
	    break;
	  case YELLOW_LIZARD:
	    fprintf (fptr, YEL_DEN_PC);
	    break;
	}
	break;
      case HOME_DEN:
	fprintf (fptr, HOME_DEN_PC);
	break;
      case RUIN:
	fprintf (fptr, RUIN_PC);
	break;
      case FERTILE:
	fprintf (fptr, FERTILE_PC);
	break;
      case SWAMP:
	fprintf (fptr, SWAMP_PC);
	break;
      case PEAK:
	fprintf (fptr, PEAK_PC);
	break;
      case VOLCANO:
	fprintf (fptr, VOLCANO_PC);
	break;
      case WATER:
	fprintf (fptr, WATER_PC);
	break;
      case WHIRLPOOL:
	fprintf (fptr, WHIRLPOOL_PC);
	break;
      case SCRUB:
	fprintf (fptr, SCRUB_PC);
	break;
      case TEMPLE:
	fprintf (fptr, TEMPLE_PC);
	break;
      case CURSED:
	fprintf (fptr, CURSED_PC);
	break;
    }
  }

  if (generic_on)
  {
    xy_to_string (x, y, string);

    fprintf (fptr, "(%s) ", string);
  } else
    fprintf (fptr, "() ");

  if (hex.terrain == DEN || hex.terrain == HOME_DEN)
  {
    if (hex.hex_owner == 0)
    {
      fprintf (fptr, "Small (Free) Owner\n");
    } else {
      if (hex.hex_owner == player)
      {
	fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		 player_code [hex.hex_owner], lizards_in_hex (&hex) + hex.lizard_pop);
      } else {
	fprintf (fptr, "Small (%s) Owner\n", player_code [hex.hex_owner]);
      }
    }
  } else {
    if (lizards_in_hex (&hex) > 0)
    {
      if ((hex.terrain != SCRUB && hex.terrain != RUIN) ||
	  hex.hex_owner == player)
      {
	fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		 player_code [hex.hex_owner], lizards_in_hex (&hex));
      } else {
        if (spy_at (x, y, player))
	{
	  fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		   player_code [hex.hex_owner], lizards_in_hex (&hex));
	} else {
	  fprintf (fptr, "Small\n");
	}
      }
    } else {
      if (spy_at (x, y, player))
      {
	fprintf (fptr, "Small (%s) Owner (SPY) Troop_Number\n",
		 player_code [player]);
      } else {
	if ((hex.bridge_here == 1 || hex.terrain == TEMPLE) &&
	  hex.hex_owner > 0)
	{
	  fprintf (fptr, "Small (%s) Owner\n", player_code [hex.hex_owner]);
	} else {
	  fprintf (fptr, "Small\n");
	}
      }
    }
  }
}

int spy_at (int x, int y, int player)
{
  int work;

  for (work = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player == player)
      return (1);

  return (0);
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

void build_unit_list (void)
{
  int work,
      work2,
      work3,
      work4,
      x,
      y;

  hex_t world_hex;

  unit_t work_unit;

  for (work = 1; work < players_in_game + 1; work ++)
    number_units [work] = 0;

  for (x = 0; x < world_x; x ++)
  {
    for (y = 0; y < world_y; y ++)
    {
      get_hex (x, y, &world_hex);

      if (world_hex.hex_owner > 0 && lizards_in_hex (&world_hex) > 0)
      {
	work = world_hex.hex_owner;

	for (work2 = 0, work3 = 0; work2 < number_units [work]; work2 ++)
	  if (player_units [work][work2]->x == x &&
	      player_units [work][work2]->y == y)
	  {
	    work3 = 1;

	    break;
	  }

	if (work3 == 0)
	{
	  work_unit.x = x;
	  work_unit.y = y;
	  work_unit.spy = 0;

	  if (world_hex.terrain == PEAK)
	    work_unit.peak = 1;
	  else
	    work_unit.peak = 0;

	  if ((player_units [work][number_units [work]] =
	       malloc (sizeof (unit_t))) == NULL)
	  {
            printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

            getch ();

	    exit (EXIT_FAILURE);
	  }

	  *player_units [work][number_units [work]] = work_unit;

	  number_units [work] ++;
	}
      }
    }
  }

  for (work = 0; work < number_spys; work ++)
  {
    if (spy_list [work]->player > 0)
    {
      work2 = spy_list [work]->player;

      for (work3 = 0, work4 = 0; work3 < number_units [work2]; work3 ++)
	if (player_units [work2][work3]->x == spy_list [work]->x_hex &&
	    player_units [work2][work3]->y == spy_list [work]->y_hex)
	{
	  work4 = 1;

	  break;
	}

      if (work4 == 0)
      {
	work_unit.x = spy_list [work]->x_hex;
	work_unit.y = spy_list [work]->y_hex;
	work_unit.spy = 1;

	get_hex (work_unit.x, work_unit.y, &world_hex);

	if (world_hex.terrain == PEAK)
	  work_unit.peak = 1;
	else
	  work_unit.peak = 0;

	if ((player_units [work2][number_units [work2]] =
	    malloc (sizeof (unit_t))) == NULL)
	{
          printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	*player_units [work2][number_units [work2]] = work_unit;

	number_units [work2] ++;
      }
    }
  }
}
