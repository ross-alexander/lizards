#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include "lizards.h"

debug_t::debug_t()
{
  logfilename = (char*)"debug.log";
  bannerstr = NULL;
  if ((stream = fopen(logfilename, "w")) == NULL)
    {
      fprintf(stderr, "Debugger failed to open log file %s\n", logfilename);
      exit(1);
    }
  log("Debugger (%s, %d) started.", __FILE__,__LINE__);
}

debug_t::~debug_t()
{
  log("Debugger closing.");
  fclose(stream);
}

void debug_t::banner(const char *s)
{
  bannerstr = strdup(s);
}

void debug_t::log(const char *s, ...)
{
  char timestr[256];
  time_t t;
  struct tm *tm;
  va_list ap;
  time(&t);
  tm = localtime(&t);
  strftime(timestr, 255, "%Y-%m-%d %H:%M", tm);
  fprintf(stream, "%s ", timestr);
  if (bannerstr)
    fprintf(stream,"%s ", bannerstr);
  va_start(ap, s);
  vfprintf(stream, s, ap);
  fprintf (stream, "\n");
  fflush (stream);
  va_end(ap);
}

