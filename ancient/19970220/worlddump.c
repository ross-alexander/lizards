#include <stdio.h>
#include <stdlib.h>

#include "lizard.h"
#include "lizgame.h"
#include "lizmisc.h"

int main(int argc, char *argv[])
{
  UnitList ulist[MAX_PLAYERS + 1];
  int i;

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

  printf("World(%d,%d)\n", world.x, world.y);

  for (i = 0; i <= players_in_game; i++)
    {
      ulist[i] = BuildUnitList(&world, i);
      printf("Player %d has %d units\n", i, ulist[i].cnt);
    }
  return 0;
}
