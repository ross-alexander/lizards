#include <libxml/tree.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

player_t::player_t()
{
  ref = code = clan = startup = NULL;
  fptr = NULL;
  status = 0;
  home_dens = 0;
}

void player_t::mesg(int level, const char *s, ...)
{
  FILE *stream;
  if (fptr)
    stream = fptr;
  else
    stream = stdout;

  va_list ap;

  va_start(ap, s);
  vfprintf(stream, s, ap);
  va_end(ap);
}
