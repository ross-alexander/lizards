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

player_t* toPlayer(lua_State *L, int index)
{
  player_t **mapp = (player_t**)lua_touserdata(L, index);
  if (mapp == NULL) luaL_typeerror(L, index, PLAYER);
  return *mapp;
}

static int Player_tostring(lua_State *L)
{
  player_t *p = toPlayer(L, 1);
  lua_pushfstring(L, "%s", p->clan);
  return 1;
}

static int Player_num(lua_State *L)
{
  player_t *p = toPlayer(L, 1);
  lua_pushinteger(L, p->num);
  return 1;
}


void Player_register(lua_State *L)
{
  static const luaL_Reg methods[] = {
    {"num",	Player_num},
    {0, 0},
  };
  
  static const luaL_Reg meta[] = {
    {"__tostring", Player_tostring},
    {0, 0}
  };

  static const luaL_Reg globals[] = {
    {0, 0}
  };

  Meta_register(L, "Player", methods, meta, globals);
}

