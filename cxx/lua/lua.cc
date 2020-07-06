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


void dump_stack(lua_State *L)
{
  int top = lua_gettop(L);
  for (int i = top; i > 0; i--)
    {
      printf("%02d: ", i);
      if (lua_isnil(L, i))
	printf("Nil");
      else if (lua_isuserdata(L, i))
	printf("Userdata");
      else if (lua_istable(L, i))
	printf("Table");
      else
	printf("'%s'", lua_tostring(L, i));
      printf("\n");
    }
}

/* ----------------------------------------------------------------------
--
-- Meta_register
--
---------------------------------------------------------------------- */


int Meta_register(lua_State *L, const char *name, const luaL_reg methods[], const luaL_reg meta[], int global)
{
  lua_pushlstring(L, name, strlen(name));           /* name of Image table */
  lua_newtable(L);             /* Methods table */
  lua_gettop(L);
  lua_newtable(L);             /* Metatable */
  lua_gettop(L);

  /*
    03: Metatable
    02: Methods
    01: "World"
  */
  
  lua_pushliteral(L, "__index");	/* add index event to metatable */
  lua_pushvalue(L, 2);
  lua_settable(L, 3);          	 /* metatable.__index = methods */

  //  lua_pushliteral(L, "__metatable");    /* hide metatable */
  //  lua_pushvalue(L, 2);
  //  lua_settable(L, 3);       	    /* metatable.__metatable = methods */

  lua_pushvalue(L, 2);
  luaL_register(L, 0, methods);	/* fill methods table */
  lua_pop(L, 1);
  
  luaL_register(L, 0, meta);	/* fill metatable */
  
  lua_pushvalue(L, 1);
  lua_pushvalue(L, 3);
  lua_settable(L, LUA_REGISTRYINDEX);
  if (global)
    {
      lua_pop(L, 1);
      lua_settable(L, LUA_GLOBALSINDEX);
    }
  else
    {
      lua_pop(L, 3);
    }
  return 0;
}

/* ----------------------------------------------------------------------

main
---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
  lua_State *L = lua_open();
  luaL_openlibs(L);

  World_register(L);
  Hex_register(L);
  Map_register(L);
  Player_register(L);
  Feature_register(L);

  if(argc > 1)
    {
      int ret = luaL_dofile(L, argv[1]);
      if (ret != 0)
	{
	  printf("%s\n", lua_tostring(L, -1));
	}
      assert(ret == 0);
    }
  lua_close(L);
  return 0;
}
