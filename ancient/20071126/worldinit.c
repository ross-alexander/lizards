#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <obstack.h>

#include "lizards.h"
#include "lizgame.h"
#include "filefunc.h"
#include "create.h"

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

/* ----------------------------------------------------------------------
   --
   -- main.c
   --
   ---------------------------------------------------------------------- */
extern int optind;
extern char *optarg;

int main (int argc, char *argv[])
{
  world_t *world;
  int home_colour = GREEN_LIZARD, start_colour = RED_LIZARD, i, ch, count = 0;
  int usetemplates = 1;
  char *fillfile = NULL;
  char *playerfile = NULL;
  char *args[1];
  char *path = LIZ_DEFAULT_DATA;
  struct obstack players_os;
  FILE *stream;
  char line[256];

  args[0] = NULL;
  world = (world_t*)calloc(sizeof(world_t), 1);
  world->x = 32;
  world->y = 32;

  while ((ch = getopt (argc, argv, "d:h:s:x:y:p:t")) != EOF)
    switch (ch)
      {
      case 'd':
	path = optarg;
	break;
      case 'f':
	fillfile = optarg;
	break;
      case 'h':
	home_colour = atoi(optarg);
	break;
      case 's':
	start_colour = atoi(optarg);
	break;
      case 'x':
	world->x = atoi (optarg);
	break;
      case 'y':
	world->y = atoi (optarg);
	break;
      case 't':
	usetemplates = 1 - usetemplates;
	break;
      case 'p':
	playerfile = optarg;
	break;
      case 'v':
	show_title ();
	printf ("LIZARDS! World Initialise Utility. %s Last compiled %s.\n\n",
		VER_STRING, __DATE__);
	exit (0);
      }
  if ((argc - optind) < 1)
    {
      printf ("FATAL ERROR: Game Number not specified on command line!\n"
	      "             Command Line Format: %s [-d data_path] [-x] [-y] [-c (ON|off)] [-p (ON|off)] [-w (ON|off)] game.\n"
	      "             e.g. %s -d lizdata -x 40 -y 50 -c on -w off\n", argv[0], argv[0]);
      exit (EXIT_FAILURE);
    }

  world->game = argv[optind];
  world->path = LzCheckDir(path, 1);
  world->flags |= LIZ_VICTORY;

  obstack_init(&players_os);
  obstack_ptr_grow(&players_os, NULL);
  if (playerfile)
    {
      if ((stream = fopen(playerfile, "r")) == NULL)
	{
	  fprintf(stderr, "Cannot open file %s.\n", playerfile);
	  exit(1);
	}
      else
	{
	  fprintf(stdout, "Opening file %s for reading.\n", playerfile);
	}
    }
  else
    stream = stdin;
  while (fgets(line, 256, stream))
    {
      player_t *p;
      char *cr, *cc, *cn, *cs, *pstr = line;
      cr = strtok(pstr, ",");
      cc = strtok(NULL, ",");
      cn = strtok(NULL, ",");
      cs = strtok(NULL, ",\n");
      if (strlen(cc) > MAX_CLAN_CODE)
	{
	  fprintf(stderr, "Clan Code \'%s\' too long (maximum %d).\n", cc, MAX_CLAN_CODE);
	  exit(1);
	}
      if (strlen(cn) > MAX_CLAN_NAME)
	{
	  fprintf(stderr, "Clan Name \'%s\' too long (maximum %d).\n", cn, MAX_CLAN_NAME);
	  exit(1);
	}
      p = (player_t*)malloc(sizeof(player_t));
      p->ref = strdup(cr);
      p->code = strdup(cc);
      p->clan = strdup(cn);
      p->startup = strdup(cs);
      obstack_ptr_grow(&players_os, p);
    }
  if (playerfile) fclose(stream);

  obstack_ptr_grow(&players_os, NULL);
  world->num_players = obstack_object_size(&players_os) / sizeof(void*) - 1;
  world->player = (player_t**)obstack_finish(&players_os);
  
  printf ("Creating game '%s' in directory %s with world %d x %d with %d players (one is the system)\n",
	  world->game, world->path,world->x, world->y, world->num_players);
  for (i = 0; i < world->num_players; i++)
      if (world->player[i])
	{
	  world->player[i]->num = i;
	  world->player[i]->status = PLAYER_ACTIVE;
	  printf ("%6s %s\n", world->player[i]->code, world->player[i]->clan);
	}
  if (world->num_players < 1)
  {
    printf ("FATAL ERROR: Game %s has no players.\n", world->game);
    exit (EXIT_FAILURE);
  }
  world->due = time(NULL);
  world->turn = 0;
  world->num_spies = 0;
  randomize();
  WorldInit(world);
  WorldFill(world, fillfile, count, args);
  if (usetemplates)
    CustomStartup(world);
  else
    WorldPopulate(world, home_colour, start_colour);
  WorldFix(world);
  put_world (world);
  printf ("Finished!\n");
  return (EXIT_SUCCESS);
}
