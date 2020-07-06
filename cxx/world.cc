#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <alloca.h>
#include <assert.h>
#include <unistd.h>

#include <glib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml++/libxml++.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- Constructors
--
---------------------------------------------------------------------- */


world_t::world_t(int x, int y, const char *d, const char *n)
{
  misc_t::dbg.log("world_t::world_t(%d, %d, %s, %s) called.", x, y, d, n);
  turn = 0;
  game = strdup(n);
  dir = g_strdup(d);
  path = g_strdup_printf("%s/%s.xml", d, game);
  oldmaps = 0;
  if (x > 1 && y > 1)
    {
      map = new map_t(x, y);
      map->create_nowrap();
    }
}

world_t::world_t(const char *p)
{
  game = NULL;
  dir = NULL;
  path = strdup(p);
  load();
}

/* ----------------------------------------------------------------------
--
-- open_player_files
--
---------------------------------------------------------------------- */
int world_t::open_player_files(void)
{
  char tmp[256];
  for (int i = 0; i < num_players; i++)
    {
      sprintf(tmp, "%s%02d%02d.txt",
	      game,
	      turn,
	      i);
      if ((player[i]->fptr = fopen(tmp, "w")) == NULL)
	exit(1);
    }
  return 0;
}

int world_t::close_player_files(void)
{
  for (int i = 0; i < num_players; i++)
    if (player[i]->fptr != NULL)
      {
	fclose(player[i]->fptr);
	player[i]->fptr = NULL;
      }
  return 0;
}

/* ----------------------------------------------------------------------
--
-- get_world
--
---------------------------------------------------------------------- */
void world_t::load()
{
  xmlpp::DomParser parser(path);

  if (!parser)
    {
      fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
      exit(1);
    }

  xmlpp::Element *xn_swamp = parser.get_document()->get_root_node();
  if (xn_swamp->get_name() != "swamp")
    {
      fprintf(stderr, "Root element is not 'swamp'\n");
      exit(1);
    }

  assert(xn_swamp->get_attribute("game"));
  assert(xn_swamp->get_attribute("dir"));
  assert(xn_swamp->get_attribute("turn"));

  if (game) delete game;
  game = strdup(xn_swamp->get_attribute("game")->get_value().data());

  if (dir) delete dir;
  dir = strdup(xn_swamp->get_attribute("dir")->get_value().data());

  //  xmlDocPtr doc = xmlParseFile(path);
  //  xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
  //  xmlXPathObjectPtr res;

  turn = strtol(xn_swamp->get_attribute("dir")->get_value().data(), NULL, 10);

  printf("%s [%s] turn %d\n", game, dir, turn);

  xmlpp::Node::NodeSet ns = xn_swamp->find("players/player");

  num_players = ns.size() + 1;
  assert(num_players > 1);

  printf("Number of players is %d\n", num_players);

  player = new player_t*[num_players];

  player[0] = new player_t;
  player[0]->num = 0;
  player[0]->ref = strdup("CG-00");
  player[0]->clan = strdup("Circle Games");
  player[0]->code = strdup("C-G");

  for (unsigned int i = 0; i < ns.size(); i++)
    {
      xmlpp::Element *e = dynamic_cast<xmlpp::Element*>(ns[i]);
      assert(e != NULL);
      assert(e->get_attribute("clan"));
      assert(e->get_attribute("code"));
      assert(e->get_attribute("ref"));
      assert(e->get_attribute("indx"));

      int index = strtol((e->get_attribute("indx")->get_value().data()), NULL, 10);

      assert(index > 0 && index <= num_players);

      player[index] = new player_t;
      player[index]->num = index;
      player[index]->ref = strdup(e->get_attribute("ref")->get_value().data());
      player[index]->clan = strdup(e->get_attribute("clan")->get_value().data());
      player[index]->code = strdup(e->get_attribute("code")->get_value().data());
      player[index]->home_dens = 0;
      printf("%d %s [%s] %s\n",
	     player[index]->num,
	     player[index]->code,
	     player[index]->ref,
	     player[index]->clan);
    }

  /* -------------------- 
     oldmaps is sized turn+2 so fit both the current turns (since it
     is zero based) and any new turn
     -------------------- */

  oldmaps = new map_t*[turn + 2];
  for (int j = 0; j < turn+2; j++)
    oldmaps[j] = 0;

  ns = xn_swamp->find("turn");
  int turns = ns.size();

  for (int j = 0; j < turns; j++)
    {
      xmlpp::Element *e = dynamic_cast<xmlpp::Element*>(ns[j]);
      assert(e->get_attribute("turn") != NULL);

      int k = strtol(e->get_attribute("turn")->get_value().data(), NULL, 10);

      if (k > turn)
	{
	  fprintf(stderr, "Found <turn> with turn (%d) > game turn (%d), ignoring turn.\n", k, turn);
	}
      else
	{
	  xmlpp::Node::NodeSet map_ns = e->find("map");
	  assert(map_ns.size() == 1);
	  oldmaps[k] = new map_t(dynamic_cast<xmlpp::Element*>(map_ns[0]));
	}
    }
  map = new map_t(*oldmaps[turn]);
}

/* ----------------------------------------------------------------------
--
-- put_world
--
---------------------------------------------------------------------- */
void world_t::save()
{
  const char *ns = "";
  xmlpp::Document doc("1.0");
  xmlpp::Element *xn_swamp = doc.create_root_node("swamp", "", ns);
  xn_swamp->set_attribute("game", game);
  xn_swamp->set_attribute("dir", dir);
  xn_swamp->set_attribute("turn", Glib::ustring::compose("%1", turn));

  //  xn_swamp->set_attribute("width", Glib::ustring::compose("%1", map->width));
  //  xn_swamp->set_attribute("height", Glib::ustring::compose("%1", map->height));

  xmlpp::Element *xn_players = xn_swamp->add_child_element("players", ns);

  for (int i = 1; i < num_players; i++)
    {
      xmlpp::Element *node = xn_players->add_child_element("player");
      node->set_attribute("ref", player[i]->ref);
      node->set_attribute("indx", Glib::ustring::compose("%1", player[i]->num));
      node->set_attribute("clan", player[i]->clan);
      node->set_attribute("code", player[i]->code);
    }
  /* --------------------

     the oldmaps array is originally sized turn+2 so in general the
     last entry should be NULL.  If it isn't then we delete the
     existing entry and update with the current map.  If turn has been
     increased then there is space anyway.

     -------------------- */

  if (oldmaps == NULL)
    {
      oldmaps = new map_t*[turn+1];
      for (int k = 0; k < turn+1; k++)
	oldmaps[k] = 0;
    }

  if (oldmaps[turn])
    delete oldmaps[turn];

  oldmaps[turn] = new map_t(*map);

  for (int i = 0; i <= turn; i++)
    if (oldmaps[i])
      {
	xmlpp::Element *xn_turn = xn_swamp->add_child_element("turn");
	xn_turn->set_attribute("turn", Glib::ustring::compose("%1", turn));
	oldmaps[i]->save_xml(xn_turn);
      }
  doc.write_to_file_formatted(path);
}

world_t::~world_t()
{
  misc_t::dbg.log("world_t::~world_t() entered.");
}

void world_t::debug(FILE *stream)
{
  fprintf(stream, "Swamp(game=%s turn=%d)\n", game, turn);
  map->debug(stream);
}

player_t *world_t::get_player(int p)
{
  assert(p >= 0 && p <= num_players);
  return player[p];
}

// ----------------------------------------------------------------------
//
// CreateWorld
//
// ----------------------------------------------------------------------
world_t::world_t(int x, int y, const char *playerfile, int argc, char *argv[])
{

  /* round x & y up to nearest 4 */

  x += (4 - (x % 4)) % 4; 
  y += (4 - (y % 4)) % 4;

  assert(x >=0 && y >= 0);

  turn = 0;
  game = strdup("Test");
  dir = g_strdup(".");
  path = g_strdup_printf("%s/%s.xml", dir, game);
  oldmaps = 0;

  if (x > 1 && y > 1)
    {
      map = new map_t(x, y);
      map->create_nowrap();
    }

  misc_t::dbg.banner(game);

#ifdef NotUseKeyFile
  GArray *pa = g_array_new(1, 1, sizeof(player_t*));
  FILE *stream;
  if (playerfile)
    {
      if ((stream = fopen(playerfile, "r")) == NULL)
	{
	  fprintf(stderr, "Cannot open file %s.\n", playerfile);
	  exit(1);
	}
      else
	{
	  fprintf(stdout, "Opening file %s for reading.\n", playerfile);
	}
    }
  else
    stream = stdin;

  char line[256];
  while (fgets(line, 256, stream))
    {
      player_t *p;
      char *cr, *cc, *cn, *cs, *pstr = line;
      cr = strtok(pstr, ",");
      cc = strtok(NULL, ",");
      cn = strtok(NULL, ",");
      cs = strtok(NULL, ",\n");
      if (strlen(cc) > MAX_CLAN_CODE)
	{
	  fprintf(stderr, "Clan Code \'%s\' too long (maximum %d).\n", cc, MAX_CLAN_CODE);
	  exit(EXIT_FAILURE);
	}
      if (strlen(cn) > MAX_CLAN_NAME)
	{
	  fprintf(stderr, "Clan Name \'%s\' too long (maximum %d).\n", cn, MAX_CLAN_NAME);
	  exit(EXIT_FAILURE);
	}
      p = new player_t;
      p->ref = strdup(cr);
      p->code = strdup(cc);
      p->clan = strdup(cn);
      p->startup = strdup(cs);
      g_array_append_val(pa, p);
    }
  if (playerfile) fclose(stream);

  num_players = pa->len + 1;
  player = (player_t**)calloc(sizeof(player_t*), num_players + 1);
  for(int k = 1; k < num_players; k++)
    player[k] = g_array_index(pa, player_t*, k-1);
  g_array_free(pa, 0);

#else
  GError *gerror = 0;
  GKeyFile *kf = g_key_file_new();
  if (!g_key_file_load_from_file(kf, playerfile, G_KEY_FILE_NONE, &gerror))
    {
      fprintf(stderr, "Could not load configuration file %s: %s.\n", playerfile, gerror->message);
      exit(1);
    }

  if (!g_key_file_has_group(kf, "Players"))
    {
      fprintf(stderr, "[Players] missing from %s\n", playerfile);
      exit(1);
    }

  gsize size;
  char **keys = g_key_file_get_keys(kf, "Players", &size, &gerror);

  num_players = size + 1;
  player = (player_t**)calloc(sizeof(player_t*), num_players + 1);
  for (unsigned int i = 0; i < size; i++)
    {
      gchar *value = g_key_file_get_value(kf, "Players", keys[i], &gerror);
      gchar **values = g_strsplit(value, ",", 4);

      printf("%s\n", value);

      player_t *p = new player_t;
      p->num = i+1;
      p->ref = strdup(values[0]);
      p->code = strdup(values[1]);
      p->clan = strdup(values[2]);
      p->startup = strdup(values[3]);
      player[i+1] = p;
      g_strfreev(values);
    }

#endif

  if ((map->size / 64) < (num_players - 1))
    {
      fprintf(stderr, "Each player must have atleast 64 hexes each to start (normally 8 x 8).\n");
      exit(EXIT_FAILURE);
    }
  if (num_players < 1)
  {
    printf ("FATAL ERROR: Game %s has no players.\n", game);
    exit (EXIT_FAILURE);
  }

  printf ("Creating game '%s' in directory \"%s\" with world %d x %d with %d players.\n",
	  game, path, map->width, map->height, num_players - 1);

  player[0] = new player_t;
  player[0]->num = 0;
  player[0]->ref = strdup("CG-00");
  player[0]->clan = strdup("Circle Games");
  player[0]->code = strdup("C-G");

  for (int i = 1; i < num_players; i++)
      if (player[i])
	{
	  player[i]->num = i;
	  player[i]->status = PLAYER_ACTIVE;
	  printf ("%6s %s\n", player[i]->code, player[i]->clan);
	}

  fill(NULL, optind, argc, argv);
  //  populate(GREEN, RED);
  custom_startup();
  fix();
}
