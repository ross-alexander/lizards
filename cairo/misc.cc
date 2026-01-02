#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <getopt.h>
#include <glib.h>
#include <map>
#include <vector>
#include <typeinfo>

#include <sys/syslog.h>
#include <cairomm/context.h>

#include "lizards.h"

void misc_t::rseed(int i) { srand(i); };
int misc_t::uniform(int i) { return rand()%i; };

void misc_t::log(int level, const char *s, ...)
{
  char timestr[256];
  time_t t;
  struct tm *tm;
  va_list ap;
  FILE *stream = stderr;
  if (level > log_level)
    return;
  time(&t);
  tm = localtime(&t);
  strftime(timestr, 255, "%Y-%m-%d %H:%M", tm);
  fprintf(stream, "%s ", timestr);
  va_start(ap, s);
  vfprintf(stream, s, ap);
  fprintf (stream, "\n");
  fflush (stream);
  va_end(ap);
}

std::string misc_t::read_file(const char *f)
{
  FILE *stream;
  std::string s;
  if (!(stream = fopen(f, "r")))
    return s;

  char buf[1024];
  int len;
  while ((len = fread(buf, sizeof(char), sizeof(buf), stream)))
    s.append(buf, len);
  fclose(stream);
  return s;
}

std::map<int, std::string> misc_t::band_colours = {
  {RED,    "RED"},
  {GREEN,  "GRN"},
  {YELLOW, "YEL"},
  {BLACK,  "BLK"},
  {GREY,   "GRY"}
};

std::map<std::string, int> misc_t::formats = {
  {"FORMAT_JSON", FORMAT_FILE_JS},
  {"FORMAT_TXT",  FORMAT_FILE_TXT},
  {"FORMAT_XML",  FORMAT_FILE_XML},
};

int log_level = LOG_INFO;
