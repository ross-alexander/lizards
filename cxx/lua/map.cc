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
-- map
--
---------------------------------------------------------------------- */
map_t* toMap(lua_State *L, int index)
{
  map_t **mapp = (map_t**)lua_touserdata(L, index);
  if (mapp == NULL) luaL_typerror(L, index, MAP);
  return *mapp;
}

int Map_width(lua_State *L)
{
  map_t *map = toMap(L, 1);
  lua_pushnumber(L, map->width);
  return 1;
}

int Map_height(lua_State *L)
{
  map_t *map = toMap(L, 1);
  lua_pushnumber(L, map->height);
  return 1;
}


int Map_hexes(lua_State *L)
{
  map_t *map = toMap(L, 1);
  lua_newtable(L);
  int t = lua_gettop(L);

  int i, j, k = 0;
  for (j = 0; j < map->height; j++)
    for (i = 0; i < map->width; i++)
      {
	lua_pushnumber(L, k);
	hex_t *hex = (*map)(i, j);
	hex_t **mapp = (hex_t**)lua_newuserdata(L, sizeof(hex_t*));
	*mapp = hex;
	luaL_getmetatable(L, HEX);
	lua_setmetatable(L, -2);
	lua_settable(L, t);
	k++;
      }
  return 1;
}

int Map_hex(lua_State *L)
{
  hex_t *hex;
  if (lua_gettop(L) != 3)
    return 0;
  map_t *map = toMap(L, 1);
  int x = lua_tonumber(L, 2);
  int y = lua_tonumber(L, 3);

  if ((hex = (*map)(x,y)))
    {
      hex_t **mapp = (hex_t**)lua_newuserdata(L, sizeof(hex_t*));
      *mapp = hex;
      luaL_getmetatable(L, HEX);
      lua_setmetatable(L, -2);
      return 1;
    }
  return 0;
}

static int Map_tostring (lua_State *L)
{
  map_t *map = toMap(L, 1);
  lua_pushfstring(L, "Map(%d x %d)", map->width, map->height);
  return 1;
}

void Map_register(lua_State *L)
{
  static const luaL_reg Map_methods[] = {
    {"width",		Map_width},
    {"height",		Map_height},
    {"hexes",		Map_hexes},
    {"hex",		Map_hex},
    {0, 0},
  };
  
  static const luaL_reg Map_meta[] = {
    {"__tostring", Map_tostring},
    {0, 0}
  };

  Meta_register(L, MAP, Map_methods, Map_meta, 0);
}

