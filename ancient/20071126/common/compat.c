#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "compat.h"

/* ----------------------------------------------------------------------
--
-- LzCheckDir
--
---------------------------------------------------------------------- */
char *LzCheckDir(char *dir, int create)
{
  struct stat statbuf;
  char *newdir;
  int err, len, addslash = 0;
  len = strlen(dir);
  addslash = (dir[len - 1] != '/');
  if (!addslash) len--;
  newdir = (char*)malloc(len + 2);
  strcpy(newdir, dir);
  if (!addslash) newdir[len] = '\0';
  if ((err = stat(newdir, &statbuf)) == -1)
    {
      if (create) {
	if ((err = mkdir(newdir, 0755)) != 0)
	  {
	    fprintf(stderr, "Error creating directory %s.\n", newdir);
	    exit(1);
	  }
      }
      else
	{
	  fprintf(stderr, "Data directory %s does not exist.\n", newdir);
	  exit(1);
	}
    }
  else
    {
      if (!S_ISDIR(statbuf.st_mode))
	{
	  fprintf(stderr, "%s is not a directory.\n", newdir);
	  exit(1);
	}
    }
  newdir[len] = '/';
  newdir[len + 1] = '\0';
  return newdir;
}

void randomize(void)
{
  srand(time(0));
}
char* itoa(int i, char *c, int base)
{
  sprintf(c, "%d", i);
  return c;
}
char* strupr(char *c)
{
  return c;
}

