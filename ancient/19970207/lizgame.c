#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "filefunc.h"
#include "strgfunc.h"

#include "lizgame.h"

extern unsigned int spawn_turn_switch,
		    capture_switch,
		    volcano_dormant_switch,
		    peak_active_switch,
		    world_wrap_switch,
                    victory_points_switch,
                    home_owned_worth,
                    home_victory_points,
                    give_switch;

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
  strcat (filename, WORLD_SWITCH_F);
  strcat (filename, game_code);

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

void get_home_den_status (void)
{
  FILE *fptr;

  char filename [255],
       work_string [80];

  int work;

  sprintf (filename, "%s%s%s", data_path, HOME_STAT_F, game_code);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    player_home_den [work] = 0;

  for (work = 1; work < players_in_game + 1; work ++)
    if (fscanf (fptr, "%s", work_string) == NULL)
      break;
    else
      player_home_den [work] = atoi (work_string);

  fclose (fptr);
}

void put_home_den_status (void)
{
  FILE *fptr;

  char filename [255];

  int work;

  sprintf (filename, "%s%s%s", data_path, HOME_STAT_F, game_code);

  if ((fptr = fopen (filename, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  for (work = 1; work < players_in_game + 1; work ++)
    fprintf (fptr, "%d\n", player_home_den [work]);

  fclose (fptr);
}

void get_player_list (void)
{
  FILE *fptr;

  int loop;

  char filename [255],
       work_string [255],
       *next_comma,
       *last_comma;

  for (loop = 1; loop < MAX_PLAYERS + 1; loop ++)
    strcpy (player_name [loop], "****");

  sprintf (filename, "%s%s%s", data_path, PLAYER_F, game_code);

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
    strncpy (player_name [players_in_game], last_comma, 35);
    player_name [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_clan_name [players_in_game], last_comma, 35);
    player_clan_name [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_road [players_in_game], last_comma, 35);
    player_road [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_burb [players_in_game], last_comma, 35);
    player_burb [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_city [players_in_game], last_comma, 35);
    player_city [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_country [players_in_game], last_comma, 35);
    player_country [players_in_game][35] = NULL;

    last_comma = next_comma + 1;
    next_comma = strchr (last_comma, ',');
    next_comma [0] = NULL;
    strncpy (player_cred [players_in_game], last_comma, 9);
    player_cred [players_in_game][9] = NULL;

    last_comma = next_comma + 1;
    player_output_mode [players_in_game] = last_comma [0];

    players_in_game ++;
  }

  players_in_game --;

  fclose (fptr);
}

void save_player_list (char *turn_date)
{
  FILE *fptr;

  char filename [80];

  unsigned int player;

  players_in_game = 0;

  sprintf (filename, "%s%s%s", data_path, PLAYER_F, game_code);

  if ((fptr = fopen (filename, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  fprintf (fptr, "%s\n", turn_date);

  for (player = 1; player < MAX_PLAYERS + 1; player ++)
    if (player_clan_name [player][0] != NULL)
    {
      players_in_game ++;

      fprintf (fptr, "%s,%s,%s,%s,%s,%s,%s,%0s,%c\n", player_code [player],
	player_name [player], player_clan_name [player],
	player_road [player], player_burb [player], player_city [player],
	player_country [player], player_cred [player],
	player_output_mode [player]);
    }

  fclose (fptr);
}

void get_spy_list (void)
{
  FILE *fptr;

  char filename [255];

  int work;

  spy_t work_spy;

  sprintf (filename, "%s%s%s", data_path, SPY_F, game_code);

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

void put_spy_list (void)
{
  FILE *spy_fptr;

  char filename [255];

  int work,
      work2;

  sprintf (filename, "%s%s%s", data_path, SPY_F, game_code);

  if ((spy_fptr = fopen (filename, "wb")) == NULL)
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

void get_world (void)
{
  FILE *fptr;

  char filename [255];

  sprintf (filename, "%s%s%s", data_path, WORLD_STAT_F, game_code);

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
