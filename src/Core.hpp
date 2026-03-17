#ifndef LUASTEAM_CORE
#define LUASTEAM_CORE

#include "Common.hpp"

namespace luasteam {

// Adds functions SteamAPI_*
void add_Core(lua_State *L);
void add_interface_placeholders(lua_State *L);

} // namespace luasteam

#endif // LUASTEAM_CORE
