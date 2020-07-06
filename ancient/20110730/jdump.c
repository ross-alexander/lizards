#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lizard.h"
#include "lizgame.h"
#include "lizmisc.h"
#include "lizhex.h"

extern int optind;
extern char *optarg;

/* ----------------------------------------------------------------------
--
--
--
---------------------------------------------------------------------- */
int build_see_list_sort (const void *elem1, const void *elem2);

void PlaceHex(World *world, see_t work, int* seen, see_t *slist, int *sptr)
{
  int x, y;
 
  x = (work.x + world->x) % world->x;
  y = (work.y + world->y) % world->y;
  
  if (!seen[y * world->x + x])
    {
      slist[*sptr] = work;
      seen[y * world->x + x] = 1;
      (*sptr)++;
    }
}

/* ----------------------------------------------------------------------
--
--
--
---------------------------------------------------------------------- */
void DoPlayer(World *, int);
void DoPlayer(World *world, int player)
{
  int i, j, x, y;
  BandList ulist[MAX_PLAYERS + 1];
  HexList *hexlist;
  hex_t hex;
  int *map;
  int spycount = 0;

  printf("%d %d %d %d %d\n",
	 world->x, world->y, player, world->num_players, world->turn);

  for (i = 0; i < world->num_players; i++)
    printf("%s\n", world->player[i].clan_name);

  map = (int*)malloc(world->x * world->y * sizeof(int));

  for (j = 0; j < world->y; j++)
    for (i = 0; i < world->x; i++)
      map[j * world->x + i] = 0;

  for (j = 0; j < world->num_spies; j++)
    spycount += (world->spylist[j]->player == i) ? 1 : 0;
  
  ulist[player] = BuildBandList(world, player);
  hexlist = BuildHexList(world, player, ulist[player], 0);
  for (j = 0; j < hexlist->cnt; j++)
    {
      int flags = 0x0000;
      x = hexlist->list[j].x;
      y = hexlist->list[j].y;
      get_hex(world, x, y, &hex);

      map[y * world->x + x] = '*';

      if (hex.bridge_here) 
	flags |= LZ_BRIDGE_NS;
      if (hex.raft_here)
	flags |= LZ_RAFT_HERE;
      
      fprintf(stdout,
	      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	      hex.x,
	      hex.y,
	      flags,
	      hex.terrain,
	      hex.hex_owner,
	      hex.den_lizard_type,
	      hex.turns_hungry,
	      hex.combat_points,
	      hex.red_lizards,
	      hex.green_lizards,
	      hex.grey_lizards,
	      hex.black_lizards,
	      hex.yellow_lizards,
	      hex.lizard_pop,
	      0,
	      0);
    }
  /*
  for (j = 0; j < world->y; j++)
    {
      for (i = 0; i < world->x; i++)
	fputc(map[j * world->x + i] ? '*' : ' ', stdout);
      fputc('\n', stdout);
    }
*/
  free(hexlist->list);
  free(hexlist);
}

void DoAll(World *);
void DoAll(World *world)
{
  int i, j;
  hex_t hex;  
  for (j = 0; j < world->y; j++)
    for (i = 0; i < world->x; i++)
      {
	int flags = 0x0000;
	get_hex(world, i, j, &hex);
	if (hex.bridge_here) 
	  flags |= LZ_BRIDGE_NS;
	if (hex.raft_here)
	  flags |= LZ_RAFT_HERE;
      
	fprintf(stdout,
		"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
		hex.x,
		hex.y,
		flags,
		hex.terrain,
		hex.hex_owner,
		hex.den_lizard_type % 5,
		hex.turns_hungry,
		hex.combat_points,
		hex.red_lizards,
		hex.green_lizards,
		hex.grey_lizards,
		hex.black_lizards,
		hex.yellow_lizards,
		hex.lizard_pop,
		0,
		0);
      }
}

int main(int argc, char *argv[])
{
  int ch, ply = 0, all = 0;

  while((ch = getopt(argc, argv, "p:a")) != EOF)
    switch(ch)
    {
    case 'p':
      ply = atoi(optarg);
      break;
    case 'a':
      all = 1;
      break;
    }
  if ((argc - optind) == 2)
    {
      data_path = LzCheckDir(argv[optind++], 0);
      game_code = (char*)malloc(strlen(argv[optind] + 1));
      strcpy (game_code, argv [optind]);
      strupr (game_code);
    }
  else
    exit(1);

  world = get_world();

  if (all)
    DoAll(world);
  else if ((ply < 0) || (ply > world->num_players))
    return 0;
  else
    DoPlayer(world, ply);
  return 0;
}
