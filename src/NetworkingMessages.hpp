#ifndef LUASTEAM_NETWORKINGMESSAGES
#define LUASTEAM_NETWORKINGMESSAGES

#include "Common.hpp"

namespace luasteam {

// Adds functions from ISteamNetworkingMessages
void add_NetworkingMessages(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs);
void add_GameServerNetworkingMessages(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs);

void shutdown_NetworkingMessages(lua_State *L);
void shutdown_GameServerNetworkingMessages(lua_State *L);

} // namespace luasteam

#endif // LUASTEAM_NETWORKINGMESSAGES
