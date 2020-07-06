#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>
#include <fcntl.h>

#include "lizard.h"
#include "lizgame.h"

extern int optind;
extern char* optarg;

int Execute(char*);
int Execute(char *program)
{
  int slen, error;
  char *scode;
  slen = strlen(program) + strlen(data_path) + strlen(game_code) + 4;

  scode = (char*)alloca(slen);
  sprintf(scode, "%s %s %s", program, data_path, game_code);

  if ((error = system(scode)))
    {
      fprintf(stderr, "Error %d occurred while trying to execute %s\n",
	      error, program);
      exit(1);
    }
  return 0;
}
extern int optind;

int main(int argc, char *argv[])
{
  int logfd, ch;

  while ((ch = getopt(argc, argv, "")) != EOF)
    ;
  if ((argc - optind) == 2)
  {
    data_path = LzCheckDir(argv[optind++], 0);
    game_code = (char*)malloc(strlen(argv[optind]) + 1);
    strcpy (game_code, argv[optind]);
    strupr (game_code);
  }
  else
    exit(1);

  world = (World*)malloc(sizeof(World));
  get_world(world);
  fprintf(stdout, "Game %s turn %d\n", game_code, world->turn);
  put_world(world);

  if((logfd = open("log", O_WRONLY|O_CREAT, 0644)) < 0)
    {
      fprintf(stderr, "Error opening log file.\n");
      exit(1);
    }

  if(dup2(logfd, 1) < 0)
    {
      fprintf(stderr, "Error in dup2\n");
      exit(1);
    }

  Execute("tordbord");
  Execute("terrmast");
  Execute("bordshuffle");
  Execute("worldopen");
  Execute("worldmast");
  Execute("worldshut");
  Execute("clanreport");
  Execute("clanrank");
  Execute("messapnd");
  close(logfd);

  get_world(world);
  world->turn++;
  put_world(world);
  return 0;
}
