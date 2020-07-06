#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <obstack.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#include "lizards.h"
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

/*----------------------------------------------------------------------
--
-- get_player_list
--
---------------------------------------------------------------------- */
static void get_player_list (world_t *world, FILE *fptr)
{
  char work_string [255];
  char *str;
  int i, nplayers;

  fscanf(fptr, "%d\n", &nplayers);
  world->num_players = nplayers; /* this includes player 0 */
  world->player = (player_t**)calloc(sizeof(player_t*), world->num_players);
  for (i = 1; i < nplayers && fgets (work_string, 255, fptr); i++)
    {
      player_t *player = (player_t*)calloc(sizeof(player_t), 1);
      player->num = i;
      str = strtok(work_string, ","); player->ref = strdup(str);
      str = strtok(NULL, ","); player->num = atoi(str);
      str = strtok(NULL, ","); player->code = strdup(str);
      str = strtok(NULL, ","); player->clan = strdup(str);
      str = strtok(NULL, "\n"); player->status = atoi(str);
      world->player[i] = player;
  }

  world->player[0] = (player_t*)malloc(sizeof(player_t));
  world->player[0]->num = 0;
  world->player[0]->ref = strdup("CG-00");
  world->player[0]->clan = strdup("Circle Games");
  world->player[0]->code = strdup("C-G");
}

static void save_player_list (world_t *world, FILE *fptr)
{
  int i;
  fprintf (fptr, "%d\n", world->num_players);

  for (i = 1; i < world->num_players; i++)
    fprintf (fptr, "%s,%d,%s,%s,%d\n",
	     world->player[i]->ref,
	     world->player[i]->num,
	     world->player[i]->code,
	     world->player[i]->clan,
	     world->player[i]->status);
}

/* ----------------------------------------------------------------------
--
-- get_world
--
---------------------------------------------------------------------- */
world_t* get_world (char *path, char *game)
{
  world_t *w;
  FILE *fptr;
  int i, x, y, length, nfeatures;
  char flag_str[200], title[40];
  char *filename;
  hex_t *hex;
  w = (world_t*)calloc(1,sizeof(world_t));

  if ((w->debug = fopen("debug.log", "w")) == NULL)
    {
      fprintf(stderr, "Cannot open debug.log file.\n");
      exit(EXIT_FAILURE);
    }
  length = strlen(path) + strlen(WORLD_STAT_F) + strlen(game);
  filename = (char*)alloca(length + 1);
  sprintf (filename, "%s%s.%s", path, game, WORLD_STAT_F);

  if ((fptr = fopen (filename, "r")) == NULL)
    {
      printf ("FATAL ERROR: Unable to read %s file.\n", filename);
      exit (EXIT_FAILURE);
    }
  w->path = strdup(path);
  w->game = strdup(game);

  debug(w, "Opened file %s for reading.", filename);

  fscanf (fptr, "%d\n", &w->x);
  fscanf (fptr, "%d\n", &w->y);
  fscanf (fptr, "%d\n", &w->turn);
  fscanf (fptr, "%ld\n", &w->due);
  fscanf (fptr, "%200s\n", flag_str);
  fscanf (fptr, "%d\n", &w->num_spies);
  fscanf (fptr, "%d\n", &nfeatures);
  get_player_list(w, fptr);

  debug(w, "World (%d %d %d %ld) opened.", w->x, w->y, w->turn, w->due);
  w->hexes = (hex_t*)calloc(sizeof(hex_t), w->x * w->y);

  for (y = 0; y < w->y; y++)
    for (x = 0; x < w->x; x++)
      {
	coord xy; xy.x = x; xy.y = y;
	hex = get_hex(w, xy);
	hex->xy.invalid = 0;
	fscanf(fptr, "%d %d %s %d %d ", &hex->xy.x, &hex->xy.y, title,
	       &hex->terrain, &hex->owner);
	hex->title = strdup(title);
	if (!coord_eq(hex->xy, xy))
	  fprintf(stderr, "hex (%d, %d) != file (%d, %d)\n",
		  x, y, hex->xy.x, hex->xy.y);
	for (i = 0; i < 6; i++)
	  fscanf(fptr, "%d %d %d ", &hex->edge[i].invalid,
		 &hex->edge[i].x, &hex->edge[i].y);
	fscanf(fptr, "\n");
	hex->chain = NULL;
      }
  for (i = 0; i < nfeatures; i++)
    {
      feature_t* f = (feature_t*)calloc(sizeof(feature_t), 1);
      fscanf(fptr, "%d %d %d %d", &f->type, &f->owner, &f->xy.x, &f->xy.y);
      switch(f->type)
	{
	case DEN:
	  fscanf(fptr, "%d %d %d %d", &f->f.den.pop, &f->f.den.hunger,
		  &f->f.den.type, &f->f.den.home);
	  break;
	case BAND:
	  fscanf(fptr, "%d %d %d %d %d %d %d",
		  &f->f.band.type[RED], &f->f.band.type[GREEN],
		  &f->f.band.type[GREY], &f->f.band.type[BLACK],
		  &f->f.band.type[YELLOW],
		  &f->f.band.hunger, &f->f.band.immobile);
	  break;
	case RAFT:
	  fscanf(fptr, "%d", &f->f.raft.age);
	  break;
	}
      hex = get_hex(w, f->xy);
      add_feature(w, f, hex);
    }
  w->spylist = (spy_t**)calloc(sizeof(spy_t*), w->num_spies);
  for (i = 0; i < w->num_spies; i++)
    if (w->spylist[i])
      fscanf(fptr, "%d %d %d %d %d %d\n",
	      &w->spylist[i]->x_hex,
	      &w->spylist[i]->y_hex,
	      &w->spylist[i]->player,
	      &w->spylist[i]->lizard_type,
	      &w->spylist[i]->turns_alone,
	      &w->spylist[i]->mired);
  fclose(fptr);
  return w;
}

/* ----------------------------------------------------------------------
--
-- put_world
--
---------------------------------------------------------------------- */
void put_world (world_t *w)
{
  FILE *fptr;
  int length, x, y, i, nspies, nfeatures;
  hex_t *hex;
  feature_t *f;
  char *filename;

  nfeatures = 0;
  for (y = 0; y < w->y; y++)
    for (x = 0; x < w->x; x++)
      {
	coord xy; xy.x = x; xy.y = y;
	hex = get_hex(w, xy);
	for (f = hex->chain; f; f = f->chain)
	  nfeatures++;
      }
  for (nspies = i = 0; i < w->num_spies; i++)
    nspies += w->spylist[i] ? 1 : 0;

  length = strlen(w->path) + strlen(WORLD_STAT_F) + strlen(w->game);
  filename = (char*)alloca(length + 1);
  sprintf (filename, "%s%s.%s", w->path, w->game, WORLD_STAT_F);

  if ((fptr = fopen (filename, "w")) == 0)
  {
    printf ("FATAL ERROR: Unable to write %s file.\n", filename);
    exit (EXIT_FAILURE);
  }
  fprintf (fptr, "%d\n", w->x);
  fprintf (fptr, "%d\n", w->y);
  fprintf (fptr, "%d\n", w->turn);
  fprintf (fptr, "%ld\n", w->due);
  fprintf (fptr, "%s\n", w->flags & LIZ_VICTORY ? "VICTORY" : "");
  fprintf (fptr, "%d\n", nspies);
  fprintf (fptr, "%d\n", nfeatures);
  save_player_list(w, fptr);

  nfeatures = 0;
  for (y = 0; y < w->y; y++)
    for (x = 0; x < w->x; x++)
      {
	coord xy; xy.x = x; xy.y = y;
	hex = get_hex(w, xy);
	fprintf(fptr, "%d %d %s %d %d ", hex->xy.x, hex->xy.y, hex->title,
		hex->terrain, hex->owner);
	for (i = 0; i < 6; i++)
	  fprintf(fptr, "%d %d %d ", hex->edge[i].invalid,
		 hex->edge[i].x, hex->edge[i].y);
	fprintf(fptr, "\n");
      }
  for (y = 0; y < w->y; y++)
    for (x = 0; x < w->x; x++)
      {
	coord xy; xy.x = x; xy.y = y;
	hex = get_hex(w, xy);
	for (f = hex->chain; f; f = f->chain)
	  {
	    fprintf(fptr, "%d %d %d %d ", f->type, f->owner,
		    f->xy.x, f->xy.y);
	    switch (f->type)
	      {
	      case DEN:
		fprintf(fptr, "%d %d %d %d", f->f.den.pop, f->f.den.hunger,
		       f->f.den.type, f->f.den.home);
		break;
	      case BAND:
		fprintf(fptr, "%d %d %d %d %d %d %d",
			f->f.band.type[RED], f->f.band.type[GREEN],
			f->f.band.type[GREY], f->f.band.type[BLACK],
			f->f.band.type[YELLOW],
			f->f.band.hunger, f->f.band.immobile);
		break;
	      case RAFT:
		fprintf(fptr, "%d", f->f.raft.age);
		break;
	      }
	    fprintf(fptr,"\n");
	    nfeatures++;
	  }
      }
  for (i = 0; i < w->num_spies; i++)
    if (w->spylist[i])
      fprintf(fptr, "%d %d %d %d %d %d\n",
	      w->spylist[i]->x_hex,
	      w->spylist[i]->y_hex,
	      w->spylist[i]->player,
	      w->spylist[i]->lizard_type,
	      w->spylist[i]->turns_alone,
	      w->spylist[i]->mired);
  fclose(fptr);
}

/* ----------------------------------------------------------------------
--
-- OpenPlayerFiles
--
---------------------------------------------------------------------- */
void OpenPlayerFiles(world_t *w)
{
  int i;
  int j;
  char *fname;

  j = strlen(w->path) + strlen(w->game) + strlen("LP_") + 10;
  fname = (char*)alloca(j);

  for (i = 0; i < w->num_players; i++)
    if (w->player[i])
      {
	sprintf(fname, "%s%s.LP%02d_%03d", w->path, w->game,
		i, w->turn);
	if ((w->player[i]->stream = fopen(fname, "w")) == NULL)
	  {
	    fprintf(stderr, "Error opening file %s.\n", fname);
	    exit(1);
	  }
      }
}

void ClosePlayerFiles(world_t *w)
{
  int i;
  for (i = 0; i < w->num_players; i++)
    fclose(w->player[i]->stream);
}
