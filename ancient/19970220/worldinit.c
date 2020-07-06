#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lizard.h"
#include "map.h"
#include "lizgame.h"
#include "filefunc.h"

char *lzOnStr = "on";
char *lzOffStr = "off";
char *data_path = "DATA";
char *game_code;

void show_init_title (void);

/* ----------------------------------------------------------------------
   --
   -- main.c
   --
   ---------------------------------------------------------------------- */
extern int optind;
extern char *optarg;

int 
main (int argc, char *argv[])
{
  FILE *fptr;
  char filename[255];

  hex_t empty_hex;
  int x, y, ch;

  world.x = 32;
  world.y = 32;

  while ((ch = getopt (argc, argv, "vc:w:p:x:y:")) != EOF)
    switch (ch)
      {
      case 'c':
	if (!strcmp (optarg, "on") || !strcmp (optarg, "ON"))
	  capture_switch = 1;
	else if (!strcmp (optarg, "off") || !strcmp (optarg, "OFF"))
	  capture_switch = 0;
	else
	  fprintf (stderr, "Illegal value to capture switch.\n");
	break;

      case 'p':
	if (!strcmp (optarg, "on") || !strcmp (optarg, "ON"))
	  victory_points_switch = 1;
	else if (!strcmp (optarg, "off") || !strcmp (optarg, "OFF"))
	  victory_points_switch = 0;
	else
	  fprintf (stderr, "Illegal value to victory points switch.\n");
	break;

      case 'w':
	if (!strcmp (optarg, "on") || !strcmp (optarg, "ON"))
	  world_wrap_switch = 1;
	else if (!strcmp (optarg, "off") || !strcmp (optarg, "OFF"))
	  world_wrap_switch = 0;
	else
	  fprintf (stderr, "Illegal value to world wrap switch.\n");
	break;
      case 'x':
	world.x = atoi (optarg);
	break;
      case 'y':
	world.x = atoi (optarg);
	break;
      case 'v':
	show_init_title ();
	printf ("LIZARDS! World Initialise Utility. %s Last compiled %s.\n\n",
		VER_STRING, __DATE__);
	exit (0);
      }
  if ((argc - optind) < 2)
    {
      printf ("FATAL ERROR: Game Number not specified on command line!\n"
	      "             Command Line Format: %s [-d data_path] [-x] [-y] [-c (ON|off)] [-p (ON|off)] [-w (ON|off)] game.\n"
	      "             e.g. %s -d lizdata -x 40 -y 50 -c on -w off\n", argv[0], argv[0]);
      exit (EXIT_FAILURE);
    }
  else
    {
      data_path = LzCheckDir (argv[optind++], 1);
      game_code = argv[optind];
      strupr (game_code);
    }

  printf ("Datapath: %s\n", data_path);
  printf ("Gamecode: %s\n", game_code);
  printf ("World: %d x %d\n", world.x, world.y);
  printf ("Capture: %s\n", capture_switch ? lzOnStr : lzOffStr);
  printf ("Wrapmap: %s\n", world_wrap_switch ? lzOnStr : lzOffStr);
  printf ("Victorypoints: %s\n", victory_points_switch ? lzOnStr : lzOffStr);

  world.due_date = "1/1/1990";
  world.turn = 0;
  put_world (&world);
  printf ("Initialising world file...\n");

  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_DATA_F);
  printf ("Initialising %s file...", filename);

  if ((fptr = fopen (filename, "wb")) == NULL)
    {
      printf ("\nFATAL ERROR: Unable to write to %s file.\n", filename);
      exit (EXIT_FAILURE);
    }

  for (y = 0; y < world.y; y++)
    {
      for (x = 0; x < world.x; x++)
	{
	  empty_hex.x = x;
	  empty_hex.y = y;
	  empty_hex.terrain = WATER;
	  empty_hex.hex_owner = 0;
	  empty_hex.bridge_here = 0;
 	  empty_hex.den_lizard_type = 0;
	  empty_hex.turns_hungry = 0;
	  empty_hex.turns_den_hungry = 0;
	  empty_hex.combat_points = 0;
	  empty_hex.turns_undisturbed = 0;
	  empty_hex.lizards_immobile = 0;
	  empty_hex.red_lizards = 0;
	  empty_hex.green_lizards = 0;
	  empty_hex.grey_lizards = 0;
	  empty_hex.black_lizards = 0;
	  empty_hex.yellow_lizards = 0;
	  empty_hex.lizard_pop = 0;
	  empty_hex.bridge_dir = 0;
	  empty_hex.raft_here = 0;
	  empty_hex.template_hex = 0;

	  put_rec (fptr, sizeof (hex_t), (x * world.y) + y, &empty_hex);
	}
    }

  fclose (fptr);

  sprintf (filename, "%s%s.%s", data_path, game_code, PLAYER_F);

  printf (" \nInitialising %s file...\n", filename);

  if ((fptr = fopen (filename, "r")) == NULL)
    {
      if ((fptr = fopen (filename, "w")) == NULL)
	{
	  printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
	  exit (EXIT_FAILURE);
	}
      fclose (fptr);
    }
  else
    fclose (fptr);

  sprintf (filename, "%s%s.%s", data_path, game_code, SPY_F);
  printf ("Initialising %s file...\n", filename);

  remove (filename);

  sprintf (filename, "%s%s.%s", data_path, game_code, HOME_STAT_F);

  printf ("Initialising %s file...\n", filename);

  if ((fptr = fopen (filename, "w")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
      exit (EXIT_FAILURE);
    }

  fclose (fptr);

  sprintf (filename, "%s%s.%s", data_path, game_code, REAL_WORLD_SWITCH_F);

  printf ("Initialising %s file...\n", filename);

  if ((fptr = fopen (filename, "wt")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
      exit (EXIT_FAILURE);
    }

  fprintf (fptr, "LIZARDS! Game %s Switches:\n", game_code);

  if (capture_switch)
    fprintf (fptr, "CAPTURE\n");
  if (world_wrap_switch)
    fprintf (fptr, "WRAP\n");
  if (victory_points_switch)
    fprintf (fptr, "V_POINTS\n");

  fclose (fptr);

  sprintf (filename, "%s%s.%s", data_path, game_code, WORLD_SWITCH_F);
  printf ("Initialising %s file...\n", filename);

  if ((fptr = fopen (filename, "w")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file.\n", filename);
      exit (EXIT_FAILURE);
    }

  fprintf (fptr, "LIZARDS! Game %s Switches:\n", game_code);

  if (capture_switch)
    fprintf (fptr, "CAPTURE\n");
  if (world_wrap_switch)
    fprintf (fptr, "WRAP\n");
  if (victory_points_switch)
    fprintf (fptr, "V_POINTS\n");

  fclose (fptr);

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}

void 
show_init_title (void)
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
