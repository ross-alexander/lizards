#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>

#include "lizard.h"
#include "map.h"

#include "filefunc.h"
#include "strgfunc.h"

#include "lizgame.h"

void show_title (void)
{
  printf ("\n          ::::    :::: ::::::   :::   ::::::. :::::   .::::.  ::  TM\n"
    "           ::      ::  ``  ::  :: ::   ::  ::  :: ::  ::  :: ::::\n"
    "           ::      ::     ::  ::   ::  ::  ::  ::  :: ::     ::::\n"
    "           ::      ::    ::   :::::::  :::::'  ::  :: `::::.  ::\n"
    "           ::   .  ::   ::    ::   ::  :: :::  ::  ::     ::  ::\n"
    "           ::  .:  ::   :: .. ::   ::  ::  ::  :: ::  ::  ::  ..\n"
    "          ::::::: :::: :::::: ::   :: :::  :: :::::   `::::'  ::\n\n"
    "                        (c) Copyright, 1991,92,93.\n"
    "                        LIZARDS! (TM), 1991,92,93.\n"
    "                      Programmed by Stuart Nicholson.\n"
    "                          Contact Circle Games,\n"
    "                  P O Box 105320 Auckland, New Zealand.\n"
    "                           Phone (09) 358 5378.\n\n");
}

/* ----------------------------------------------------------------------
--
-- get_switch_list
--
---------------------------------------------------------------------- */
void get_switch_list (void)
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

  strcpy (filename, data_path);
  strcat (filename, game_code);
  strcat (filename, ".");
  strcat (filename, WORLD_SWITCH_F);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  while (fgets (work_string, 255, fptr) != NULL)
  {
    work_string [strlen (work_string) - 1] = '\0';

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
          printf ("FATAL ERROR: HOME_OWN switch used with no parameter.\n");
          exit (EXIT_FAILURE);
        }

        break;
      case 7: /* HOME POINTS switch, indicates points per home den */
        home_victory_points = atoi (work_string);

        if (home_victory_points == 0)
        {
          printf ("FATAL ERROR: HOME_VIC switch used with no parameter.\n");
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

void get_home_den_status (void)
{
  FILE *fptr;

  char filename [255],
       work_string [80];

  int work;

  sprintf (filename, "%s%s.%s", data_path,game_code,  HOME_STAT_F);

  if ((fptr = fopen (filename, "r")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    player[work].home_den = 0;

  for (work = 1; work < players_in_game + 1; work ++)
    if (fscanf (fptr, "%s", work_string) == 0)
      break;
    else
      player [work].home_den = atoi (work_string);

  fclose (fptr);
}

void put_home_den_status (void)
{
  FILE *fptr;

  char filename [255];

  int work;

  sprintf (filename, "%s%s.%s", data_path, game_code, HOME_STAT_F);

  if ((fptr = fopen (filename, "w")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  for (work = 1; work < players_in_game + 1; work ++)
    fprintf (fptr, "%d\n", player[work].home_den);

  fclose (fptr);
}

/*----------------------------------------------------------------------
--
-- get_player_list
--
---------------------------------------------------------------------- */
void get_player_list (void)
{
  FILE *fptr;

  int loop;

  char filename [255],
       work_string [255],
       *next_comma,
       *last_comma;

  for (loop = 1; loop < MAX_PLAYERS + 1; loop ++)
    strcpy (player[loop].code, "****");

  sprintf (filename, "%s%s.%s", data_path, game_code, PLAYER_F);

  if ((fptr = fopen (filename, "r")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  players_in_game = 1;

  while (fgets (work_string, 255, fptr) != '\0' &&
    players_in_game < MAX_PLAYERS + 1)
  {
    work_string [strlen (work_string)] = '\0';

    last_comma = work_string;
    next_comma = strchr (work_string, ',');
    next_comma [0] = '\0';
    strncpy (player[players_in_game].code, last_comma, 6);
    player[players_in_game].code[5] = '\0';

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = '\0';
    strncpy (player[players_in_game].clan_name , last_comma, 35);
    player[players_in_game].clan_name [35] = '\0';

    players_in_game ++;
  }

  players_in_game --;

  fclose (fptr);
}

void save_player_list (char *turn_date)
{
  FILE *fptr;
  char filename [80];
  int i;

  players_in_game = 0;

  sprintf (filename, "%s%s.%s", data_path, game_code, PLAYER_F);

  if ((fptr = fopen (filename, "w")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  fprintf (fptr, "%s\n", turn_date);

  for (i = 1; i < MAX_PLAYERS + 1; i++)
    if (player[i].clan_name [0] != '\0')
    {
      players_in_game ++;

      fprintf (fptr, "%s,%s\n",
	       player[i].code ,
	       player[i].clan_name);
    }

  fclose (fptr);
}

void get_spy_list (void)
{
  FILE *fptr;

  char filename [255];

  int work;

  spy_t work_spy;

  sprintf (filename, "%s%s.%s", data_path, game_code,  SPY_F);

  if ((fptr = fopen (filename, "rb")) == NULL)
  {
    number_spys = 0;
    return;
  }

  number_spys = num_records (fptr, sizeof (spy_t));

  for (work = 0; work < number_spys; work ++)
  {
    get_rec (fptr, sizeof (spy_t), work, &work_spy);

    if ((spy_list [work] = malloc (sizeof (spy_t))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory.\n");
      exit (EXIT_FAILURE);
    }

    *(spy_list [work]) = work_spy;
  }

  fclose (fptr);
}

void put_spy_list (void)
{
  FILE *spy_fptr;

  char filename [255];

  int work,
      work2;

  sprintf (filename, "%s%s.%s", data_path, game_code, SPY_F);

  if ((spy_fptr = fopen (filename, "wb")) == '\0')
  {
    printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  for (work = 0, work2 = 0; work < number_spys; work ++)
  {
    if (spy_list [work]->player > 0)
    {
      put_rec (spy_fptr, sizeof (spy_t), work2, spy_list [work]);

      work2 ++;
    }
  }

  fclose (spy_fptr);
}

/* ----------------------------------------------------------------------
--
-- get_world
--
---------------------------------------------------------------------- */
void get_world (World *world)
{
  FILE *fptr;
  int length;
  char *filename;
  length = strlen(data_path) + strlen(WORLD_STAT_F) + strlen(game_code);
  filename = (char*)alloca(length + 1);
  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_STAT_F);

  if ((fptr = fopen (filename, "r")) == 0)
  {
    printf ("FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }
  world->due_date = (char*)malloc(80);

  fscanf (fptr, "%d\n", &world->x);
  fscanf (fptr, "%d\n", &world->y);
  fscanf (fptr, "%d\n", &world->turn);
  fscanf (fptr, "%s\n", world->due_date);
  fclose (fptr);
}

/* ----------------------------------------------------------------------
--
-- get_world
--
---------------------------------------------------------------------- */
void put_world (World *world)
{
  FILE *fptr;
  int length;
  char *filename;
  length = strlen(data_path) + strlen(WORLD_STAT_F) + strlen(game_code);
  filename = (char*)alloca(length + 1);
  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_STAT_F);

  if ((fptr = fopen (filename, "w")) == 0)
  {
    printf ("FATAL ERROR: Unable to write %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  fprintf (fptr, "%d\n", world->x);
  fprintf (fptr, "%d\n", world->y);
  fprintf (fptr, "%d\n", world->turn);
  fprintf (fptr, "%s\n", world->due_date);
  fclose (fptr);
}
