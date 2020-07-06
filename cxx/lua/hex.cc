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
-- hex
--
---------------------------------------------------------------------- */

hex_t* toHex(lua_State *L, int index)
{
  hex_t **mapp = (hex_t**)lua_touserdata(L, index);
  if (mapp == NULL) luaL_typerror(L, index, HEX);
  return *mapp;
}

static int Hex_tostring (lua_State *L)
{
  hex_t *hex = toHex(L, 1);
  lua_pushfstring(L, "%s", hex->title);
  return 1;
}

static int Hex_owner(lua_State *L)
{
  hex_t *hex = toHex(L, 1);
  lua_pushnumber(L, hex->owner);
  return 1;
}

int Hex_features(lua_State *L)
{
  hex_t *hex = toHex(L, 1);
  lua_newtable(L);
  int t = lua_gettop(L);

  for (int j = 0; j < hex->num_features(); j++)
    {
      lua_pushnumber(L, j);
      feature_t *f = hex->feature(j);
      feature_t **ff = (feature_t**)lua_newuserdata(L, sizeof(feature_t*));
      *ff = f;
      luaL_getmetatable(L, FEATURE);
      lua_setmetatable(L, -2);
      lua_settable(L, t);
    }
  return 1;
}


void Hex_register(lua_State *L)
{
  static const luaL_reg methods[] = {
    {"owner",	Hex_owner},
    {"features", Hex_features},
    {0, 0}
  };

  static const luaL_reg meta[] = {
    {"__tostring", Hex_tostring},
    {0, 0}
  };

  Meta_register(L, HEX, methods, meta, 0);
}
