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


HexList *LBuildHexList(World *world, int player, BandList ulist, int wrap)
{
  /* in y,x order! */

  int one_hex_odd[6][2] = {{-1, 0}, {-1, -1},{-1, 1},{0, -1}, {0, 1},{ 1, 0}};
  int one_hex_even[6][2] = {{-1, 0},{ 0, -1}, {0, 1}, {1, -1},{ 1, 1}, {1, 0}};
  int two_hex_odd[12][2] = {{-2, 0},{-2,-1},{-2, 1},{-1,-2},{-1, 2}, {0,-2},
			     {0, 2}, {1, -2},{1, 2}, {1, -1},{ 1, 1},{ 2, 0}};
  int two_hex_even[12][2] = {{-2, 0},{-1, -1},{-1, 1},{-1,-2},{ -1, 2},{0, -2},
			    { 0, 2},{1, -2},{  1, 2,},{2, -1},{ 2, 1},{ 2, 0}};
  int three_hex_odd[18][2] = {{-3,0},{-3,-1},{-3, 1},{-2,-2},{-2,2},{-2,-3},
			       {-2,3},{-1,-3},{-1,3},{0,-3},{0,3},{1,-3},
			       {1,3},{2,-2},{2,2},{2,-1},{2,1},{3,0}};
  int three_hex_even[18][2] = {{-3,0},{-2,-1},{-2,1},{-2,-2},{-2,2},{-1,-3},
			      {-1,3},{0,-3},{0,3},{1,-3},{1,3},{2,-3},
			      {2,3},{2,-2},{2,2},{3,-1},{3, 1},{3, 0}};

  int work, work2, odd, i, j, *seen, x, y;
  int see_hex, total_see_hex;
  int min_x, min_y, max_x, max_y, shift_x, shift_y;

  HexList *hexlist;
  see_t *see_ptr, work_see;

  total_see_hex = see_hex = 0;

  for (work = 0; work < ulist.cnt; work++)
    if (ulist.list[work].peak)
      total_see_hex += 37;
    else
      total_see_hex += 19;

  if ((see_ptr = malloc (sizeof (see_t) * total_see_hex)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory line __LINE__ of __FILE__.\n");
      exit (EXIT_FAILURE);
    }

  if ((seen = malloc (sizeof (int) * world->x * world->y)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory line __LINE__ of __FILE__.\n");
      exit (EXIT_FAILURE);
    }
  for (j = 0; j < world->y; j++)
    for (i = 0; i < world->x; i++)
      seen[j * world->x + i] = 0;

  for (work = 0; work < ulist.cnt; work++)
    {
      x = work_see.x = ulist.list[work].x;
      y = work_see.y = ulist.list[work].y;
      odd = x % 2;
      PlaceHex(world, work_see, seen, see_ptr, &see_hex);

      for (work2 = 0; work2 < 6; work2++)
	{
	  work_see.x = x+(odd?one_hex_odd[work2][1] : one_hex_even[work2][1]);
	  work_see.y = y+(odd?one_hex_odd[work2][0] : one_hex_even[work2][0]);
	  PlaceHex(world, work_see, seen, see_ptr, &see_hex);
	}
      for (work2 = 0; work2 < 12; work2++)
	{
	  work_see.x = x+(odd?two_hex_odd[work2][1] : two_hex_even[work2][1]);
	  work_see.y = y+(odd?two_hex_odd[work2][0] : two_hex_even[work2][0]);
	  PlaceHex(world, work_see, seen, see_ptr, &see_hex);
	}
    }
  max_x = max_y = 0;
  min_x = world->x;
  min_y = world->y;
  for (i = 0; i < see_hex; i++)
    {
      if (see_ptr[i].x > max_x) max_x = see_ptr[i].x;
      if (see_ptr[i].y > max_y) max_y = see_ptr[i].y;
      if (see_ptr[i].x < min_x) min_x = see_ptr[i].x;
      if (see_ptr[i].y < min_y) min_y = see_ptr[i].y;
    }
  shift_x = (world->x - max_x + min_x) / 2;
  shift_y = (world->y - max_y + min_y) / 2;

  for (i = 0; i < see_hex; i++)
    {
      see_ptr[i].centred_x = see_ptr[i].x - shift_x;
      see_ptr[i].centred_y = see_ptr[i].y - shift_y;
      see_ptr[i].x = (see_ptr[i].x + world->x) % world->x;
      see_ptr[i].y = (see_ptr[i].y + world->y) % world->y;
    }

  if (!(hexlist = (HexList*)malloc(sizeof(HexList))))
    {
      fprintf(stderr, "malloc failed in line __LINE__ of file __FILE__\n");
      exit(EXIT_FAILURE);
    }
  free(seen);
  hexlist->cnt = see_hex;
  hexlist->list = see_ptr;

  qsort (see_ptr, see_hex, sizeof (see_t), build_see_list_sort);
  return hexlist;
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

  map = (int*)malloc(world->x * world->y * sizeof(int));

  for (j = 0; j < world->y; j++)
    for (i = 0; i < world->x; i++)
      map[j * world->x + i] = 0;

  for (j = 0; j < world->num_spies; j++)
    spycount += (world->spylist[j]->player == i) ? 1 : 0;
  
  ulist[player] = BuildBandList(world, player);
  hexlist = LBuildHexList(world, player, ulist[player], 0);
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

  world = (World*)malloc(sizeof(World));
  get_world(world);

  if (all)
    DoAll(world);
  else if ((ply < 0) || (ply > world->num_players))
    return 0;
  else
    DoPlayer(world, ply);
  return 0;
}
