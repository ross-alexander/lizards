#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/syslog.h>

#include <fmt/format.h>
#include <fmt/printf.h>

#include "lizards.h"

game_t::game_t(int format, const char *path)
{
  if (format != FORMAT_FILE_JS)
    {
      misc_t::log(LOG_ERR, "Only JS format supported");
      exit(1);
    }
 
  serial_t s = serial_t::from_file(path);

  s.get("game", id);

  serial_array_t pl;
  if (s.get("players", pl))
    {
      nplayers = pl.size();
    }
  else
    {
      nplayers = 0;
    }
  if (nplayers == 0)
    misc_t::log(LOG_NOTICE, "No players, generated world only");
  else
    misc_t::log(LOG_NOTICE, "Number of players is %d", nplayers);
    
  players.reserve(nplayers + 1);
  players.push_back(nullptr);
  
  for (unsigned int i = 0; i < nplayers; i++)
    {
      player_t *pp = new player_t(pl[i]);
      misc_t::log(LOG_NOTICE, "Player %d (%s [%s]) with template %s format %d", i+1, pp->clan.c_str(), pp->code.c_str(), pp->startup.c_str(), pp->format);
      players.push_back(pp);
    }

  serial_map_t boot;
  if (s.get("grid", boot))
    {
      grid = new grid_t(boot);
    }
  else if (s.get("initial", boot))
    {
      int w, h;
      boot.get("width", w);
      boot.get("height", h);
      grid = new grid_t(w, h);
      grid->generate_typical();
      
      misc_t::log(LOG_DEBUG, "Typical world generated");
      
      layouts_t* layouts = new layouts_t(FORMAT_FILE_JS, "LAYOUT.JS");

      unsigned int w8 = grid->width/8;
      unsigned int h8 = grid->height/8;

      unsigned int nplaces = w8 * h8;

      misc_t::log(LOG_DEBUG, "World has %d places", nplaces);
      
      if (nplayers > nplaces)
	{
	  misc_t::log(LOG_ERR, "Got %d players but only %d places", nplayers, nplaces);
	  exit(1);
	}

      point_t places[nplaces];
      nplaces = 0;
      for (unsigned int i = 0; i < w8; i++)
	for (unsigned int j = 0; j < h8; j++)
	  places[nplaces++] = point_t(i * 8 + 4, j * 8 + 4);
      
      for (int i = 0; i < 8; i++)
	{
	  int x = misc_t::uniform(nplaces);
	  int y = misc_t::uniform(nplaces);
	  point_t p = places[x];
	  places[x] = places[y];
	  places[y] = p;
	}
      for (unsigned int i = 1; i <= nplayers; i++)
	{
	  template_t* t = new template_t(layouts, player(i)->format, player(i)->startup.c_str());
	  misc_t::log(LOG_DEBUG, "Placing template at %d,%d", places[i-1].x, places[i-1].y);
	  grid->place_template(t, i, places[i-1], 1);
	}
    }
  misc_t::log(LOG_DEBUG, "Game %s loaded", path);
}

grid_t * game_t::bootstrap()
{
  grid_t *grid = new grid_t(16, 16);
  grid->generate_typical();

  int np = (grid->width * grid->height) >> 8;
  point_t places[np];

  for (int i = 0; i < grid->width / 8; i++)
    for (int j = 0; j < grid->height / 8; j++)
      places[grid->width/8 * i + j] = point_t(i*8+4, j*8+4);

  srandom(time(0));
  for (int i = 0; i < grid->width * grid->height; i++)
    {
      int a = random() % np;
      int b = random() % np;
      point_t p = places[b];
      places[b] = places[a];
      places[a] = p;
    }

  for (unsigned int i = 1; i <= nplayers; i++)
    {
      layouts_t *layouts = new layouts_t(FORMAT_FILE_JS, "LAYOUT.JS");
      template_t *tmpl = new template_t(layouts, player(i)->format, player(i)->startup.c_str());
      grid->place_template(tmpl, i, places[i-1], 1);
    }
  return grid;
}

game_t::game_t(player_t ** _players)
{
  nplayers = 0;
  for (int i = 0; _players[i]; i++)
    nplayers++;
  nplayers++;
  for (int i = 0; _players[i]; i++)
    players[i+1] = _players[i];
}

player_t *game_t::player(unsigned int p)
{
  assert(p > 0 && p <= nplayers);
  return players[p];
}

/* ----------------------------------------------------------------------
--
-- report
--
---------------------------------------------------------------------- */
void game_t::report()
{
  //  int work, x, y;

  printf ("Messaging Players...\n");

  for (unsigned int i = 1; i <= nplayers; i++)
    {
      player_t *p = players[i];
      fprintf (stdout,
	     "Clan Report for clan %s (%s)\n",
	       p->clan.c_str(),
	       p->code.c_str());
  }

  printf (" \nReporting Clans and Dens...\n");

  /* 
  for (y = 0; y < world->map->height; y ++)
    for (x = 0; x < world->map->width; x ++)
      ReportPass1(world, x, y);
  for (y = 0; y < world->map->height; y ++)
    for (x = 0; x < world->map->width; x ++)
      ReportPass2(world, x, y);
  */
  /*

  printf (" \nReporting Spys...");

  for (work = 0; work < world->num_spies; work ++)
  {
    if (world->spylist[work]->player > 0)
    {
      xy_to_string (world->spylist [work]->x_hex,
		    world->spylist [work]->y_hex,
		    work_string);

      fprintf (world->player[world->spylist[work]->player].fptr,
	       "Spy(%s, %d)\n",
	       work_string,
	       world->spylist[work]->turns_alone);
    }
  }
  */
}

/* ----------------------------------------------------------------------
--
-- describe
--
---------------------------------------------------------------------- */

std::string game_t::tostring()
{
  std::string s = fmt::sprintf("%s with %d players", id, nplayers);
  return s;
}

/* ----------------------------------------------------------------------
   --
   -- serialize
   --
   ---------------------------------------------------------------------- */

serial_t game_t::serialize()
{
  serial_map_t g;
  g.add("game", id);
  serial_array_t players_ser;
  for (unsigned int i = 1; i < players.size(); i++)
    {
      players_ser.add(players[i]->serialize());
    }
  g.add("players", players_ser);
  g.add("grid", grid->serialize());
  return g;
}
