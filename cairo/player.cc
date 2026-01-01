#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <assert.h>
#include <sys/syslog.h>

#include "lizards.h"

player_t::player_t()
{
}

player_t::player_t(serial_t p)
{
  const char* startup_s = "UNDEF";
  if (p.get("startup", startup_s))
    startup = startup_s;

  const char *code_s;
  if (p.get("code", code_s))
      code = code_s;

  const char *clan_s;
  if (p.get("clan", clan_s))
    clan = clan_s;

  const char *format_s = "FORMAT_UNSPEC";
  p.get("format", format_s);

  assert(misc_t::formats.count(format_s) > 0);
  format = misc_t::formats[format_s];
}

serial_t player_t::serialize(void)
{
  serial_map_t player_m;
  player_m.add("code", code);
  player_m.add("clan", clan);
  player_m.add("format", "FORMAT_JSON");
  return player_m;
}

std::string player_t::tostring(void)
{
  return fmt::sprintf("%s [%s]", clan, code);
}
