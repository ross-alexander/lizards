#include <unistd.h>
#include "Lizard.h"

extern int optind;
extern char *optarg;

LzPaths::LzPaths(int argc, char *argv[])
{
  char ch;
  while ((ch = getopt(argc, argv, "d:g:")) != EOF)
    switch(ch)
      {
      case 'd':
	dataPath = LzCheckDir(optarg, 0);
	break;
      case 'g':
	gameCode = optarg;
	break;
      }
}

World::World(void) 
{
  x = y = 0;
  spawn_turn_switch = 0;
  capture_switch = 0;
  volcano_dormant_switch = 0;
  peak_active_switch = 0;
  world_wrap_switch = 0;
  victory_points_switch = 0;
  home_owned_worth = 0;
  home_victory_points = 0;
  give_switch = 0;
};
