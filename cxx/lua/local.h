#define WORLD "World"
#define MAP "Map"
#define HEX "Hex"
#define PLAYER "Player"
#define FEATURE "Feature"

int Meta_register(lua_State *L, const char *name, const luaL_Reg methods[], const luaL_Reg meta[], const luaL_Reg globals[]);

void World_register(lua_State *L);
void Player_register(lua_State *L);
void Map_register(lua_State *L);
void Hex_register(lua_State *L);
void Feature_register(lua_State *L);
