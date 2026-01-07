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

base_t* check(lua_State *L, int index, char* t)
{
  luaL_checktype(L, index, LUA_TUSERDATA);
  return *((base_t**)luaL_checkudata(L, index, t));
}

base_t* to(lua_State *L, int index, const char *t)
{
  base_t **bar = (base_t**)lua_touserdata(L, index);
  if (bar == NULL) luaL_typeerror(L, index, t);
  return *bar;
}

base_t* push(lua_State *L, char *t)
{
  base_t **bar = (base_t**)lua_newuserdata(L, sizeof(base_t*));
  luaL_getmetatable(L, t);
  lua_setmetatable(L, -2);
  return *bar;
}

int Feature_desc(lua_State *L)
{
  feature_t *f = dynamic_cast<feature_t*>(to(L, 1, FEATURE));
  lua_pushfstring(L, "%s", f->describe());
  return 1;
}

void Feature_register(lua_State *L)
{
  static const luaL_Reg methods[] = {
    {"desc",	Feature_desc},
    {0, 0},
  };
  
  static const luaL_Reg meta[] = {
    {"__tostring", Feature_desc},
    {0, 0}
  };

  static const luaL_Reg globals[] = {
    {0, 0}
  };

  Meta_register(L, FEATURE, methods, meta, globals);
}
