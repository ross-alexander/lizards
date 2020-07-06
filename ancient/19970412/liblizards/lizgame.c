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
void get_switch_list (World *world)
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
	world->flags |= LZ_SPAWN;
	break;

      case 1: /* CAPTURE switch */
	world->flags |= LZ_CAPTURE;
	break;

      case 2: /* VOLCANO DORMANT switch */
	world->flags |= LZ_VOLCANO;
	break;

      case 3: /* PEAK ACTIVE switch */
	world->flags |= LZ_PEAK;
	break;

      case 4: /* WORLD WRAP switch */
	world->flags |= LZ_WRAP;
	break;

      case 5: /* VICTORY POINTS switch */
	world->flags |= LZ_VICTORY;
        world->home_owned_worth = 2000;
        world->home_victory_points = 1000;
	break;

      case 6: /* HOME RANK switch, indicates what home dens are worth */
        world->home_owned_worth = atoi (work_string);
        if (world->home_owned_worth == 0)
        {
          printf ("FATAL ERROR: HOME_OWN switch used with no parameter.\n");
          exit (EXIT_FAILURE);
        }
        break;

      case 7: /* HOME POINTS switch, indicates points per home den */
        world->home_victory_points = atoi (work_string);

        if (world->home_victory_points == 0)
        {
          printf ("FATAL ERROR: HOME_VIC switch used with no parameter.\n");
          exit (EXIT_FAILURE);
        }
        break;

      case 8: /* GIVE order allowed */
	world->flags |= LZ_GIVE;
        break;

      default:
	break;
    }
  }
  fclose (fptr);
}
void put_switch_list(World *world)
{
  char *filename;
  FILE *fptr;

  filename = alloca(strlen(data_path) + strlen(game_code) +
		    strlen(WORLD_SWITCH_F) + 3);
  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_SWITCH_F);

  if ((fptr = fopen (filename, "w")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
      exit (EXIT_FAILURE);
    }

  fprintf (fptr, "LIZARDS! Game %s Switches:\n", game_code);

  if (world->flags & LZ_CAPTURE)
    fprintf (fptr, "CAPTURE\n");
  if (world->flags & LZ_WRAP)
    fprintf (fptr, "WRAP\n");
  if (world->flags & LZ_VICTORY)
    fprintf (fptr, "V_POINTS\n");
  fclose(fptr);
}

/* ----------------------------------------------------------------------
--
-- get_home_den_status
--
---------------------------------------------------------------------- */
void get_home_den_status (World *world)
{
  FILE *fptr;

  char filename [255],
       work_string [80];

  int work;

  sprintf (filename, "%s%s.%s", data_path,game_code,  HOME_STAT_F);

  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    world->player[work].home_den = 0;

  if ((fptr = fopen (filename, "r")) != NULL)
  {
    for (work = 1; work < world->num_players + 1; work ++)
      if (fscanf (fptr, "%s", work_string) == 0)
	break;
      else
	world->player[work].home_den = atoi (work_string);
    fclose (fptr);
  }
}

void put_home_den_status (World *world)
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

  for (work = 1; work < world->num_players + 1; work ++)
    fprintf (fptr, "%d\n", world->player[work].home_den);

  fclose (fptr);
}

/*----------------------------------------------------------------------
--
-- get_player_list
--
---------------------------------------------------------------------- */
void get_player_list (World *world)
{
  FILE *fptr;

  int loop;

  char filename [255],
       work_string [255],
       *next_comma,
       *last_comma;

  for (loop = 1; loop < MAX_PLAYERS + 1; loop ++)
    strcpy (world->player[loop].code, "****");

  sprintf (filename, "%s%s.%s", data_path, game_code, PLAYER_F);

  if ((fptr = fopen (filename, "r")) == NULL)
  {
    world->num_players = 0;
    return;
  }

  world->num_players = 1;

  while (fgets (work_string, 255, fptr) != '\0' &&
    world->num_players < MAX_PLAYERS + 1)
  {
    work_string [strlen (work_string)] = '\0';

    last_comma = work_string;
    next_comma = strchr (work_string, ',');
    next_comma [0] = '\0';
    strncpy (world->player[world->num_players].code, last_comma, 6);
    world->player[world->num_players].code[5] = '\0';

    last_comma = next_comma + 1;
    strncpy (world->player[world->num_players].clan_name, last_comma, 35);
    world->player[world->num_players].clan_name [strlen(world->player[world->num_players].clan_name) - 1] = '\0';

    world->num_players ++;
  }

  world->num_players --;
  strcpy(world->player[0].clan_name, "Circle Games");
  strcpy(world->player[0].code, "C-G");

  fclose (fptr);
}

void save_player_list (World *world)
{
  FILE *fptr;
  char filename [80];
  int i;

  world->num_players = 0;

  sprintf (filename, "%s%s.%s", data_path, game_code, PLAYER_F);

  if ((fptr = fopen (filename, "w")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  for (i = 1; i < MAX_PLAYERS + 1; i++)
    if (world->player[i].clan_name [0] != '\0')
    {
      world->num_players ++;
      fprintf (fptr, "%s,%s\n",
	       world->player[i].code ,
	       world->player[i].clan_name);
    }
  fclose (fptr);
}

void get_spy_list (World *world)
{
  FILE *fptr;

  char filename [255];

  int work;

  spy_t work_spy;

  sprintf (filename, "%s%s.%s", data_path, game_code,  SPY_F);

  if ((fptr = fopen (filename, "rb")) == NULL)
  {
    world->num_spies = 0;
    return;
  }

  world->num_spies = num_records (fptr, sizeof (spy_t));

  for (work = 0; work < world->num_spies; work ++)
  {
    get_rec (fptr, sizeof (spy_t), work, &work_spy);

    if ((world->spylist[work] = malloc (sizeof (spy_t))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory.\n");
      exit (EXIT_FAILURE);
    }

    *(world->spylist [work]) = work_spy;
  }

  fclose (fptr);
}

void put_spy_list (World *world)
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

  for (work = 0, work2 = 0; work < world->num_spies; work ++)
  {
    if (world->spylist[work]->player > 0)
    {
      put_rec (spy_fptr, sizeof (spy_t), work2, world->spylist [work]);
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
  char *filename, *hfilename;
  
  length = strlen(data_path) + strlen(WORLD_STAT_F) + strlen(game_code);
  filename = (char*)alloca(length + 1);
  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_STAT_F);

  if ((fptr = fopen (filename, "r")) == 0)
  {
    printf ("FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }
  fprintf(stderr, "Opening world file.\n");

  world->due_date = (char*)malloc(80);

  fscanf (fptr, "%d\n", &world->x);
  fscanf (fptr, "%d\n", &world->y);
  fscanf (fptr, "%d\n", &world->turn);
  fscanf (fptr, "%s\n", world->due_date);
  fclose (fptr);

  fprintf(stderr, "Opening switch list.\n");
  get_switch_list(world);

  fprintf(stderr, "Opening world data file.\n");

  length = strlen(data_path) + strlen(WORLD_DATA_F) + strlen(game_code);
  hfilename = (char*)alloca(length + 1);
  sprintf (hfilename, "%s%s.%s", data_path, game_code, WORLD_DATA_F);

  if ((fptr = fopen (hfilename, "rb")) == NULL)
    {
      printf ("FATAL ERROR: Unable to read/write %s file.\n", filename);
      exit (EXIT_FAILURE);
    }
  world->hexes = (hex_t*)malloc(sizeof(hex_t) * world->x * world->y);
  fread(world->hexes, sizeof(hex_t), world->x * world->y, fptr);
  fclose(fptr);

  world->player = (Player*)malloc(sizeof(Player) * (MAX_PLAYERS + 1));
  fprintf(stderr, "Opening player list.\n");
  get_player_list(world);

  fprintf(stderr, "Opening homeden status.\n");
  get_home_den_status(world);

  fprintf(stderr, "Opening spy list.\n");
  get_spy_list(world);
}

/* ----------------------------------------------------------------------
--
-- put_world
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

  fprintf(stderr, "Closing switch list.\n");
  put_switch_list(world);

  fprintf(stderr, "Closing world data file.\n");

  length = strlen(data_path) + strlen(WORLD_DATA_F) + strlen(game_code);
  filename = (char*)alloca(length + 1);
  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_DATA_F);

  if ((fptr = fopen (filename, "wb")) == NULL)
    {
      printf ("FATAL ERROR: Unable to read/write %s file.\n", filename);
      exit (EXIT_FAILURE);
    }
  fwrite(world->hexes, sizeof(hex_t), world->x * world->y, fptr);
  fclose (fptr);

  fprintf(stderr, "Closing player list.\n");
  save_player_list(world);

  fprintf(stderr, "Closing homeden status.\n");
  put_home_den_status(world);

  fprintf(stderr, "Closing spy list.\n");
  put_spy_list(world);
}
