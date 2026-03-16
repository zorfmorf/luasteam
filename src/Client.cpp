#include "Client.hpp"
#include "auto/auto.hpp"
#include <cassert>

// ======================================
// ======= ISteamClient =================
// ======================================

namespace {
static int warning_hook_ref = LUA_NOREF;
static int gs_warning_hook_ref = LUA_NOREF;

void S_CALLTYPE warning_trampoline(int nSeverity, const char *pchDebugText) {
    if (warning_hook_ref == LUA_NOREF)
        return;
    lua_State *L = luasteam::global_lua_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, warning_hook_ref);
    lua_pushinteger(L, nSeverity);
    lua_pushstring(L, pchDebugText);
    lua_call(L, 2, 0);
}

void S_CALLTYPE gs_warning_trampoline(int nSeverity, const char *pchDebugText) {
    if (gs_warning_hook_ref == LUA_NOREF)
        return;
    lua_State *L = luasteam::global_lua_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, gs_warning_hook_ref);
    lua_pushinteger(L, nSeverity);
    lua_pushstring(L, pchDebugText);
    lua_call(L, 2, 0);
}
} // namespace

// void SetWarningMessageHook(SteamAPIWarningMessageHook_t pFunction)
// Lua: Steam.Client.SetWarningMessageHook(function(severity, message) ... end) or nil to clear
EXTERN int luasteam_Client_SetWarningMessageHook(lua_State *L) {
    if (warning_hook_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, warning_hook_ref);
        warning_hook_ref = LUA_NOREF;
    }
    if (lua_isnil(L, 1)) {
        SteamClient()->SetWarningMessageHook(nullptr);
    } else {
        luaL_checktype(L, 1, LUA_TFUNCTION);
        lua_pushvalue(L, 1);
        warning_hook_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        SteamClient()->SetWarningMessageHook(warning_trampoline);
    }
    return 0;
}

EXTERN int luasteam_GameServerClient_SetWarningMessageHook(lua_State *L) {
    if (gs_warning_hook_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, gs_warning_hook_ref);
        gs_warning_hook_ref = LUA_NOREF;
    }
    if (lua_isnil(L, 1)) {
        SteamGameServerClient()->SetWarningMessageHook(nullptr);
    } else {
        luaL_checktype(L, 1, LUA_TFUNCTION);
        lua_pushvalue(L, 1);
        gs_warning_hook_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        SteamGameServerClient()->SetWarningMessageHook(gs_warning_trampoline);
    }
    return 0;
}

namespace luasteam {

void add_Client(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_Client_auto(L, {{"SetWarningMessageHook", luasteam_Client_SetWarningMessageHook}});
}

void add_GameServerClient(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_GameServerClient_auto(L, {{"SetWarningMessageHook", luasteam_GameServerClient_SetWarningMessageHook}});
}

void shutdown_Client(lua_State *L) {
    shutdown_Client_auto(L);
    if (warning_hook_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, warning_hook_ref);
        warning_hook_ref = LUA_NOREF;
    }
}

void shutdown_GameServerClient(lua_State *L) {
    shutdown_GameServerClient_auto(L);
    if (gs_warning_hook_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, gs_warning_hook_ref);
        gs_warning_hook_ref = LUA_NOREF;
    }
}

} // namespace luasteam
