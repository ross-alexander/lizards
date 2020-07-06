#include <stdio.h>
#include <stdlib.h>

#include "lizard.h"
#include "lizgame.h"
#include "lizmisc.h"
#include "lizhex.h"

int main(int argc, char *argv[])
{
  UnitList ulist[MAX_PLAYERS + 1];
  HexList *hexlist;
  hex_t hex;
  int i, j;

  if (argc == 3)
    {
      data_path = LzCheckDir(argv[1], 0);
      game_code = (char*)malloc(strlen(argv[2] + 1));
      strcpy (game_code, argv [2]);
      strupr (game_code);
    }
  else
    exit(1);

  get_world(&world);
  get_player_list();
  get_spy_list();

  printf("World(%d,%d, 0)\n", world.x, world.y);

  for (i = 0; i <= players_in_game; i++)
    {
      int spycount = 0;
      for (j = 0; j < number_spys; j++)
	spycount += (spy_list[j]->player == i) ? 1 : 0;

      ulist[i] = BuildUnitList(&world, i);
      hexlist = BuildHexList(i, ulist[i], 1, &world);
      printf("Player(%s, %d, %d, %d)\n",
	     player[i].code, ulist[i].cnt, spycount, hexlist->cnt);
      for (j = 0; j < ulist[i].cnt; j++)
	printf("Unit(%d, %d)\n", ulist[i].list[j].x, ulist[i].list[j].y);
      for (j = 0; j < hexlist->cnt; j++)
	{
	  int flags = 0x0000;
	  get_hex(hexlist->list[j].x, hexlist->list[j].y, &hex);
	  if (hex.bridge_here) 
	    flags |= LZ_BRIDGE_NS;
	  if (hex.raft_here)
	    flags |= LZ_RAFT_HERE;

	  fprintf(stdout,
		  "Hex(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",
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
      free(hexlist->list);
      free(hexlist);
    }
  return 0;
}
