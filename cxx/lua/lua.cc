#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml++/libxml++.h>

#include <lua.hpp>
#ifdef X
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#endif

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


int Meta_register(lua_State *L, const char *name, const luaL_Reg methods[], const luaL_Reg meta[], const luaL_Reg globals[])
{
  printf("register %s\n", name);
  luaL_newmetatable(L, name);             /* Metatable */
  luaL_setfuncs(L, meta, 0);	/* fill metatable */

  lua_newtable(L);             /* Methods table */
  luaL_setfuncs(L, methods, 0);	/* fill methods table */

  /*
    01: Methods
    02: Metatable
  */

  // Set meta.__index == methods, which removes methods from stack
  
  lua_setfield(L, -2, "__index");

  //  lua_pushliteral(L, "__metatable");    /* hide metatable */
  //  lua_pushvalue(L, 2);
  //  lua_settable(L, 3);       	    /* metatable.__metatable = methods */

  lua_pop(L, 1);
  printf("-- %d\n", lua_gettop(L));

  if (globals != nullptr)
    {
      lua_newtable(L);
      luaL_setfuncs(L, globals, 0);
      lua_setglobal(L, name);
      printf("Setting global %s\n", name);
    }
  return 0;
}

/* ----------------------------------------------------------------------

main
---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  World_register(L);
  Hex_register(L);
  //  Map_register(L);
  //  Player_register(L);
  //  Feature_register(L);

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
