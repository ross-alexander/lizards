#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <sys/syslog.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

#include "lizards.h"

int main(int argc, char *argv[])
{
  misc_t::rseed(time(0));
  int ch;
  char *file = (char*)"cairo.svg";
  while ((ch = getopt(argc, argv, "d:o:")) > 0)
    switch (ch)
      {
      case 'd':
	chdir(optarg);
	break;
      case 'o':
	file = optarg;
	break;
      }
  game_t *game = new game_t(FORMAT_FILE_JS, "game.js");

  for(unsigned int i = 0; i <= game->nplayers; i++)
    {
      char ffile[1024];
      snprintf(ffile, 1024, "cairo-%02d.svg", i);  
      output_t(game, i).svg((const char*)ffile);
    }
  serial_t s = game->grid->serialize();
  s.to_file("grid.js");
  exit(0);
}
