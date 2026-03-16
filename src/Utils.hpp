#ifndef LUASTEAM_UTILS
#define LUASTEAM_UTILS

#include "Common.hpp"

namespace luasteam {

void add_Utils(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs);
void shutdown_Utils(lua_State *L);

void add_GameServerUtils(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs);
void shutdown_GameServerUtils(lua_State *L);

} // namespace luasteam

#endif // LUASTEAM_UTILS
