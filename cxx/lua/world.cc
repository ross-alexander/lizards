#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml++/libxml++.h>

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
}

#include <lizards.h>
#include "local.h"

/* ----------------------------------------------------------------------
--
-- world
--
---------------------------------------------------------------------- */

world_t *checkWorld(lua_State *L, int index)
{
  world_t **bar;
  luaL_checktype(L, index, LUA_TUSERDATA);
  bar = (world_t**)luaL_checkudata(L, index, WORLD);
  return *bar;
}

world_t* toWorld(lua_State *L, int index)
{
  world_t **bar = (world_t**)lua_touserdata(L, index);
  if (bar == NULL) luaL_typeerror(L, index, WORLD);
  return *bar;
}

world_t* pushWorld(lua_State *L)
{
  world_t **bar = (world_t**)lua_newuserdata(L, sizeof(world_t*));
  luaL_getmetatable(L, WORLD);
  lua_setmetatable(L, -2);
  return *bar;
}

static int World_game(lua_State *L)
{
  printf("Topstack = %d\n", lua_gettop(L));
  world_t *w = checkWorld(L, 1);
  printf("w = %p\n", w);
  lua_pushstring(L, w->game);
  return 1;
}

int World_map(lua_State *L)
{
  world_t *w = checkWorld(L, 1);
  map_t **mapp = (map_t**)lua_newuserdata(L, sizeof(map_t*));
  *mapp = w->map;
  luaL_getmetatable(L, MAP);
  lua_setmetatable(L, -2);
  return 1;
}

static int World_new(lua_State *L)
{
  const char *s = luaL_optstring(L, 1, 0);
  assert(s != NULL);

  world_t *w = new world_t(s);
  world_t **bar = (world_t**)lua_newuserdata(L, sizeof(world_t*));
  *bar = w;
  printf("World_new(%s, %p)\n", s, w);
  luaL_getmetatable(L, WORLD);
  lua_setmetatable(L, -2);
  checkWorld(L, -1);
  return 1;
}

static int World_gc (lua_State *L)
{
  printf("bye, bye, bar\n");
  return 0;
}

static int World_tostring (lua_State *L)
{
  world_t *w = toWorld(L, 1);
  lua_pushfstring(L, "World(%s)", w->game);
  return 1;
}

int World_players(lua_State *L)
{
  world_t *w = toWorld(L, 1);
  lua_newtable(L);
  int t = lua_gettop(L);

  int j;
  for (j = 1; j < w->num_players; j++)
    {
      lua_pushnumber(L, j);
      player_t *p = w->get_player(j);
      player_t **mapp = (player_t**)lua_newuserdata(L, sizeof(player_t*));
      *mapp = p;
      luaL_getmetatable(L, "Player");
      lua_setmetatable(L, -2);
      lua_settable(L, t);
    }
  return 1;
}

void World_register(lua_State *L)
{
  static const luaL_Reg World_methods[] = {
    {"game",	World_game},
    {"map",	World_map},
    {"players", World_players},
    {0, 0},
  };

  static const luaL_Reg World_meta[] = {
    {"__gc",       World_gc},
    {"__tostring", World_tostring},
    {0, 0},
  };
  static const luaL_Reg World_globals[] = {
    {"new",	World_new},
    {0, 0},
  };
  Meta_register(L, WORLD, World_methods, World_meta, World_globals);
}
