#include "Utils.hpp"
#include "Client.hpp"
#include "auto/auto.hpp"
#include <cassert>

// ======================================
// ======= ISteamUtils ==================
// ======================================

namespace luasteam {

void add_Utils(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_Utils_auto(L, {{"SetWarningMessageHook", luasteam_Client_SetWarningMessageHook}});
}

void add_GameServerUtils(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_GameServerUtils_auto(L, {{"SetWarningMessageHook", luasteam_GameServerClient_SetWarningMessageHook}});
}

void shutdown_Utils(lua_State *L) { shutdown_Utils_auto(L); }

void shutdown_GameServerUtils(lua_State *L) { shutdown_GameServerUtils_auto(L); }

} // namespace luasteam
