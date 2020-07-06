#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"

#include "filefunc.h"
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

char data_path [80],
     disk_data_path [80],
     game_code [5];

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

int world_x,
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
void build_disk_world (int player, FILE *fptr);
see_t *build_see_list (int player, int *list_len);
int build_see_list_sort (const void *elem1, const void *elem2);
void see_wrap_hex (char *x, char *y);
void reveal_map_hex (int x, int y, int player, FILE *fptr);
void build_unit_list (void);
void put_disk_map_hex (FILE *fptr, unsigned int x, unsigned int y, hex_t *hex);
void save_reduced_player_list (char *filename);

int main (int argc, char *argv [])
{
  FILE *fptr;

  char filename [80],
       filename2 [80],
       string [256];

  unsigned int work,
	       x,
	       y;

  hex_t work_hex;

  show_title ();

  printf ("LIZARDS! Disk World Generator. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 5)
  {
    strcpy (data_path, argv [1]);
    strcpy (disk_data_path, argv [2]);
    turn_number = atoi (argv [3]);
    strncpy (game_code, argv [4], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path disk_data_path turn_number game.\n"
	     "                             e.g. DISKWORL C:\\ A:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_switch_list ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  /* pass through the player list, generating a 'sub-world' for each
     DISK or BBS player that consists of a world of the hexes and spys that
     only THEY can see */

  /* first build an 'empty world' that has no visible hexes */

  work_hex.terrain = -1;
  work_hex.hex_owner = -1;

  sprintf (filename, "%sNOVISMAP.%s", disk_data_path, game_code);

  if ((fptr = fopen (filename, "wb")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to '%s' file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  for (y = 0; y < world_y; y ++)
    for (x = 0; x < world_x; x ++)
      put_disk_map_hex (fptr, x, y, &work_hex);

  fclose (fptr);

  printf ("Building player unit lists...\n");

  build_unit_list ();

  for (work = 0; work < players_in_game + 1; work ++)
    if (strcmp (player_name [work], "****") != 0 &&
      strcmp (player_name [work], "DROP") != 0 &&
      (player_output_mode [work] == 'D' || player_output_mode [work] == 'B'))
    {
      unsigned int loop, loop2;

      printf ("Creating LIZARDS! 'Disk-World' for Player %d...\n", work);

      /* give player copy of blank map (zero visibility) file */

      sprintf (filename, "%sNOVISMAP.%s", disk_data_path, game_code);
      sprintf (filename2, "%sDWO%02d%03d.%s", disk_data_path, work, turn_number,
	game_code);
      sprintf (string, "COPY %s %s > NUL:", filename, filename2);

      if (system (string) == -1)
      {
	printf ("FATAL ERROR: Unable to copy '%s' file to '%s' file. Press SPACE.\n",
	  filename, filename2);

	getch ();

	exit (EXIT_FAILURE);
      }

      /* give player copy of world stat file */

      sprintf (filename, "%s%s%s", data_path, WORLD_STAT_F, game_code);
      sprintf (filename2, "%sDST%02d%03d.%s", disk_data_path, work, turn_number,
	game_code);
      sprintf (string, "COPY %s %s > NUL:", filename, filename2);

      if (system (string) == -1)
      {
	printf ("FATAL ERROR: Unable to copy '%s' file to '%s' file. Press SPACE.\n",
	  filename, filename2);

	getch ();

	exit (EXIT_FAILURE);
      }

      /* give player copy of world switch file */

      sprintf (filename, "%s%s%s", data_path, WORLD_SWITCH_F, game_code);
      sprintf (filename2, "%sDSW%02d%03d.%s", disk_data_path, work, turn_number,
	game_code);
      sprintf (string, "COPY %s %s > NUL:", filename, filename2);

      if (system (string) == -1)
      {
	printf ("FATAL ERROR: Unable to copy '%s' file to '%s' file. Press SPACE.\n",
	  filename, filename2);

	getch ();

	exit (EXIT_FAILURE);
      }

      /* give copy 'reduced' copy of LIZARDS player file */

      sprintf (filename, "%sDWP%02d%03d.%s", disk_data_path, work, turn_number,
	game_code);

      save_reduced_player_list (filename);

      /* build world vis map */

      sprintf (filename, "%sDWO%02d%03d.%s", disk_data_path, work, turn_number,
	game_code);

      if ((fptr = fopen (filename, "r+b")) == NULL)
      {
	printf ("FATAL ERROR: Unable to write to '%s' file. Press SPACE.\n", filename);

	getch ();

	exit (EXIT_FAILURE);
      }

      if (number_units [work] > 0)
	build_disk_world (work, fptr);

      fclose (fptr);

      /* build spys list for player */

      sprintf (filename, "%sDSP%02d%03d.%s", disk_data_path, work, turn_number,
	game_code);

      if ((fptr = fopen (filename, "wb")) == NULL)
      {
	printf ("FATAL ERROR: Unable to write to '%s' file. Press SPACE.\n", filename);

	getch ();

	exit (EXIT_FAILURE);
      }

      for (loop = 0, loop2 = 0; loop < number_spys; loop ++)
	if (spy_list [loop]->player == work)
	  put_rec (fptr, sizeof (spy_t), loop2++, spy_list [loop]);

      fclose (fptr);
    }

  sprintf (filename, "%sNOVISMAP.%s", disk_data_path, game_code);

  remove (filename);

  printf ("\nFinished!\n");

  return (EXIT_SUCCESS);
}

void build_disk_world (int player, FILE *fptr)
{
  int high_end,
      work;

  see_t *player_hex_see_list;

  if (number_units [player] == 0)
    return;

  player_hex_see_list = build_see_list (player, &high_end);

  for (work = 0; work < high_end; work ++)
    reveal_map_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
      player, fptr);

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

void reveal_map_hex (int x, int y, int player, FILE *fptr)
{
  char string [80];

  hex_t hex;

  get_hex (x, y, &hex);

  /* exercise those visibility rules so players don't even get info on
     lizards they can't see in the hexes they can see (so they can't gain
     extra info by 'hacking' bin file!) */

  if (hex.hex_owner != player)
    if (hex.terrain == HOME_DEN || hex.terrain == DEN ||
      ((hex.terrain == SCRUB || hex.terrain == RUIN) &&
      !player_spy_here (x, y, player)))
    {
      /* kill lizard stats in hex as player cannot see them! */

      hex.red_lizards = 0;
      hex.green_lizards = 0;
      hex.grey_lizards = 0;
      hex.black_lizards = 0;
      hex.yellow_lizards = 0;
      hex.lizard_pop = 0;
    } else {
      /* a hex where they can see the NUMBER of enemy lizards, but not
         the breakdown, so make them all red to stop hackers getting
         breakdowns by file editing! Don't worry about pop, as you can
         never see it with spys etc. */

      hex.red_lizards += (hex.green_lizards + hex.grey_lizards +
        hex.black_lizards + hex.yellow_lizards);

      hex.green_lizards = 0;
      hex.grey_lizards = 0;
      hex.black_lizards = 0;
      hex.yellow_lizards = 0;
    }

  put_disk_map_hex (fptr, x, y, &hex);
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

void put_disk_map_hex (FILE *fptr, unsigned int x, unsigned int y, hex_t *hex)
{
  fseek (fptr, ((long) sizeof (hex_t) * ((y * world_x) + x)), SEEK_SET);
  fwrite (hex, sizeof (hex_t), 1, fptr);
}

void save_reduced_player_list (char *filename)
{
  FILE *fptr;

  unsigned int player;

  if ((fptr = fopen (filename, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  fprintf (fptr, "%s\n", game_due_date);

  for (player = 1; player < MAX_PLAYERS + 1; player ++)
    if (player_clan_name [player][0] != NULL)
      fprintf (fptr, "%s,%s,%c\n", player_code [player],
	player_clan_name [player], player_output_mode [player]);

  fclose (fptr);
}
