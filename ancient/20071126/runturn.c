#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>
#include <fcntl.h>

#include "lizards.h"
#include "lizgame.h"

extern int optind;
extern char* optarg;

static int Execute(char *path, char *game, char *program)
{
  int slen, error;
  char *scode;
  slen = strlen(program) + strlen(path) + strlen(game) + 4;

  scode = (char*)alloca(slen);
  sprintf(scode, "%s %s %s", program, path, game);

  if ((error = system(scode)))
    {
      fprintf(stderr, "Error %d occurred while trying to execute %s\n",
	      error, program);
      exit(1);
    }
  return 0;
}

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  world_t *world;
  int logfd, ch, postscript = 0;

  while ((ch = getopt(argc, argv, "p")) != EOF)
    switch(ch)
      {
      case 'p':
	postscript = 1;
	break;
      }
  if ((argc - optind) != 2)
    {
      fprintf(stderr, "%s: [-p postscript] datapath game\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  world = get_world(LzCheckDir(argv[optind], 0), argv[optind+1]);
  fprintf(stdout, "Game %s turn %d\n", world->game, world->turn);
  put_world(world);
  
  if((logfd = open("log", O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
    {
      fprintf(stderr, "Error opening log file.\n");
      exit(1);
    }

  if(dup2(logfd, 1) < 0)
    {
      fprintf(stderr, "Error in dup2\n");
      exit(1);
    }

  Execute(world->path, world->game, "worldturn");
  if (postscript)
    Execute(world->path, world->game, "pscenprn");
  close(logfd);

  world = get_world(LzCheckDir(argv[optind], 0), argv[optind+1]);
  world->turn++;
  put_world(world);
  return 0;
}
