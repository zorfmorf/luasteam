#ifndef LUASTEAM_NETWORKINGUTILS
#define LUASTEAM_NETWORKINGUTILS

#include "Common.hpp"

namespace luasteam {

// Adds functions from ISteamNetworkingSockets
void add_NetworkingUtils(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs);
void shutdown_NetworkingUtils(lua_State *L);
} // namespace luasteam

#endif // LUASTEAM_NETWORKINGUTILS
