#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lizard.h"
#include "lizhex.h"
#include "lizgame.h"
#include "filefunc.h"

char *lzOnStr = "on";
char *lzOffStr = "off";

void show_init_title (void);

/* ----------------------------------------------------------------------
   --
   -- main.c
   --
   ---------------------------------------------------------------------- */
extern int optind;
extern char *optarg;

int main (int argc, char *argv[])
{
  FILE *fptr;
  char filename[255];

  hex_t empty_hex;
  int x, y, ch, loop;
  world = (World*)malloc(sizeof(World));
  world->x = 32;
  world->y = 32;
  data_path = "DATA";

  while ((ch = getopt (argc, argv, "vc:w:p:x:y:")) != EOF)
    switch (ch)
      {
      case 'c':
	if (!strcmp (optarg, "on") || !strcmp (optarg, "ON"))
	  world->flags |= LZ_CAPTURE;
	else if (!strcmp (optarg, "off") || !strcmp (optarg, "OFF"))
	  world->flags &= ~LZ_CAPTURE;
	else
	  fprintf (stderr, "Illegal value to capture switch.\n");
	break;

      case 'p':
	if (!strcmp (optarg, "on") || !strcmp (optarg, "ON"))
	  world->flags |= LZ_VICTORY;
	else if (!strcmp (optarg, "off") || !strcmp (optarg, "OFF"))
	  world->flags &= ~LZ_VICTORY;
	else
	  fprintf (stderr, "Illegal value to victory points switch.\n");
	break;

      case 'w':
	if (!strcmp (optarg, "on") || !strcmp (optarg, "ON"))
	  world->flags |= LZ_WRAP;
	else if (!strcmp (optarg, "off") || !strcmp (optarg, "OFF"))
	  world->flags &= ~LZ_WRAP;
	else
	  fprintf (stderr, "Illegal value to world wrap switch.\n");
	break;
      case 'x':
	world->x = atoi (optarg);
	break;
      case 'y':
	world->x = atoi (optarg);
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
  printf ("World: %d x %d\n", world->x, world->y);
  printf ("Capture: %s\n", world->flags & LZ_CAPTURE ? lzOnStr : lzOffStr);
  printf ("Wrapmap: %s\n", world->flags & LZ_WRAP ? lzOnStr : lzOffStr);
  printf ("Victory points: %s\n", world->flags & LZ_VICTORY ? lzOnStr : lzOffStr);

  world->due_date = "1/1/1990";
  world->turn = 0;
  world->num_players = world->num_spies = 0;
  world->hexes = (hex_t*)malloc(sizeof(hex_t) * world->x * world->y);
  world->player = (Player*)malloc(sizeof(Player) * (MAX_PLAYERS + 1));

  for (loop = 0; loop < MAX_PLAYERS + 1; loop ++)
    strcpy (world->player[loop].code, "****");

  for (y = 0; y < world->y; y++)
    {
      for (x = 0; x < world->x; x++)
	{
	  get_hex(world, x, y, &empty_hex);
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
	  empty_hex.tmpl_hex = 0;

	  put_hex(world,x,y,&empty_hex);
	}
    }

  put_world (world);
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
