#include "NetworkingUtils.hpp"
#include "SteamNetworkingMessage.hpp"
#include "auto/auto.hpp"
#include <cassert>

// ======================================
// SetGlobalCallback_* — store a Lua function and pass a trampoline to Steam
// ======================================

namespace {
static int cb_SteamNetConnectionStatusChanged_ref = LUA_NOREF;
static int cb_SteamNetAuthenticationStatusChanged_ref = LUA_NOREF;
static int cb_SteamRelayNetworkStatusChanged_ref = LUA_NOREF;
static int cb_SteamNetworkingFakeIPResult_ref = LUA_NOREF;
static int cb_MessagesSessionRequest_ref = LUA_NOREF;
static int cb_MessagesSessionFailed_ref = LUA_NOREF;
} // namespace

// clang-format off
#define DEFINE_SET_GLOBAL_CALLBACK(lua_name, fn_type, cb_type, steam_method, ref_var)            \
    static void trampoline_##lua_name(cb_type *data) {                                           \
        if (ref_var == LUA_NOREF) return;                                                        \
        lua_State *L = luasteam::global_lua_state;                                               \
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref_var);                                              \
        luasteam::push_##cb_type(L, *data);                                                      \
        lua_call(L, 1, 0);                                                                       \
    }                                                                                            \
    EXTERN int lua_name(lua_State *L) {                                                          \
        luaL_unref(L, LUA_REGISTRYINDEX, ref_var);                                               \
        if (lua_isnil(L, 1)) {                                                                   \
            ref_var = LUA_NOREF;                                                                 \
            bool __ret = SteamNetworkingUtils_SteamAPI()->steam_method((fn_type) nullptr);       \
            lua_pushboolean(L, __ret);                                                           \
        } else {                                                                                 \
            luaL_checktype(L, 1, LUA_TFUNCTION);                                                 \
            lua_pushvalue(L, 1);                                                                 \
            ref_var = luaL_ref(L, LUA_REGISTRYINDEX);                                            \
            bool __ret = SteamNetworkingUtils_SteamAPI()->steam_method(trampoline_##lua_name);   \
            lua_pushboolean(L, __ret);                                                           \
        }                                                                                        \
        return 1;                                                                                \
    }
// clang-format on

DEFINE_SET_GLOBAL_CALLBACK(luasteam_NetworkingUtils_SetGlobalCallback_SteamNetConnectionStatusChanged, FnSteamNetConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t, SetGlobalCallback_SteamNetConnectionStatusChanged, cb_SteamNetConnectionStatusChanged_ref)

DEFINE_SET_GLOBAL_CALLBACK(luasteam_NetworkingUtils_SetGlobalCallback_SteamNetAuthenticationStatusChanged, FnSteamNetAuthenticationStatusChanged, SteamNetAuthenticationStatus_t, SetGlobalCallback_SteamNetAuthenticationStatusChanged, cb_SteamNetAuthenticationStatusChanged_ref)

DEFINE_SET_GLOBAL_CALLBACK(luasteam_NetworkingUtils_SetGlobalCallback_SteamRelayNetworkStatusChanged, FnSteamRelayNetworkStatusChanged, SteamRelayNetworkStatus_t, SetGlobalCallback_SteamRelayNetworkStatusChanged, cb_SteamRelayNetworkStatusChanged_ref)

DEFINE_SET_GLOBAL_CALLBACK(luasteam_NetworkingUtils_SetGlobalCallback_FakeIPResult, FnSteamNetworkingFakeIPResult, SteamNetworkingFakeIPResult_t, SetGlobalCallback_FakeIPResult, cb_SteamNetworkingFakeIPResult_ref)

DEFINE_SET_GLOBAL_CALLBACK(luasteam_NetworkingUtils_SetGlobalCallback_MessagesSessionRequest, FnSteamNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t, SetGlobalCallback_MessagesSessionRequest, cb_MessagesSessionRequest_ref)

DEFINE_SET_GLOBAL_CALLBACK(luasteam_NetworkingUtils_SetGlobalCallback_MessagesSessionFailed, FnSteamNetworkingMessagesSessionFailed, SteamNetworkingMessagesSessionFailed_t, SetGlobalCallback_MessagesSessionFailed, cb_MessagesSessionFailed_ref)

// SteamNetworkingUtils::AllocateMessage — create an outbound message with a pre-allocated buffer.
// Returns: SteamNetworkingMessage_t userdata. Set m_conn, m_nFlags, m_pData before passing to SendMessages.
EXTERN int luasteam_networkingUtils_AllocateMessage(lua_State *L) {
    SteamNetworkingMessage_t *msg = SteamNetworkingUtils_SteamAPI()->AllocateMessage(0);
    luasteam::push_SteamNetworkingMessage_t(L, msg);
    return 1;
}

namespace luasteam {

void add_NetworkingUtils(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_NetworkingUtils_auto(L, {
                                    {"SetGlobalCallback_SteamNetConnectionStatusChanged", luasteam_NetworkingUtils_SetGlobalCallback_SteamNetConnectionStatusChanged},
                                    {"SetGlobalCallback_SteamNetAuthenticationStatusChanged", luasteam_NetworkingUtils_SetGlobalCallback_SteamNetAuthenticationStatusChanged},
                                    {"SetGlobalCallback_SteamRelayNetworkStatusChanged", luasteam_NetworkingUtils_SetGlobalCallback_SteamRelayNetworkStatusChanged},
                                    {"SetGlobalCallback_FakeIPResult", luasteam_NetworkingUtils_SetGlobalCallback_FakeIPResult},
                                    {"SetGlobalCallback_MessagesSessionRequest", luasteam_NetworkingUtils_SetGlobalCallback_MessagesSessionRequest},
                                    {"SetGlobalCallback_MessagesSessionFailed", luasteam_NetworkingUtils_SetGlobalCallback_MessagesSessionFailed},
                                    {"AllocateMessage", luasteam_networkingUtils_AllocateMessage},
                                });
}

void shutdown_NetworkingUtils(lua_State *L) {
    shutdown_NetworkingUtils_auto(L);
    for (auto ptr : {&cb_SteamNetConnectionStatusChanged_ref, &cb_SteamNetAuthenticationStatusChanged_ref, &cb_SteamRelayNetworkStatusChanged_ref, &cb_SteamNetworkingFakeIPResult_ref, &cb_MessagesSessionRequest_ref, &cb_MessagesSessionFailed_ref}) {
        if (*ptr != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, *ptr);
            *ptr = LUA_NOREF;
        }
    }
}

} // namespace luasteam
