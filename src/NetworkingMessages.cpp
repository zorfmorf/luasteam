#include "NetworkingMessages.hpp"
#include "SteamNetworkingMessage.hpp"
#include "auto/auto.hpp"
#include <cassert>

// ========================================
// ======= SteamNetworkingMessages ========
// ========================================

// In C++:
// int ReceiveMessagesOnChannel(int nLocalChannel, SteamNetworkingMessage_t **ppOutMessages, int nMaxMessages)
// In Lua:
// (int, ppOutMessages: SteamNetworkingMessage_t[]) NetworkingMessages.ReceiveMessagesOnChannel(nLocalChannel: int, nMaxMessages: int)
static int luasteam_ReceiveMessagesOnChannel(lua_State *L, ISteamNetworkingMessages *iface) {
    int nLocalChannel = luaL_checkinteger(L, 1);
    int nMaxMessages = luaL_checkinteger(L, 2);

    std::vector<SteamNetworkingMessage_t *> msgs(nMaxMessages);
    int numMessages = iface->ReceiveMessagesOnChannel(nLocalChannel, msgs.data(), nMaxMessages);

    lua_pushinteger(L, numMessages);
    lua_createtable(L, numMessages, 0);
    for (int i = 0; i < numMessages; i++) {
        luasteam::push_SteamNetworkingMessage_t(L, msgs[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 2;
}

static int luasteam_ReceiveMessagesOnChannel_user(lua_State *L) { return luasteam_ReceiveMessagesOnChannel(L, SteamNetworkingMessages()); }
static int luasteam_ReceiveMessagesOnChannel_gs(lua_State *L) { return luasteam_ReceiveMessagesOnChannel(L, SteamGameServerNetworkingMessages()); }

namespace luasteam {

void add_NetworkingMessages(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_NetworkingMessages_auto(L, {
                                       {"ReceiveMessagesOnChannel", luasteam_ReceiveMessagesOnChannel_user},
                                   });
}

void add_GameServerNetworkingMessages(lua_State *L, std::initializer_list<luaL_Reg> extra_funcs) {
    assert(extra_funcs.size() == 0);
    add_GameServerNetworkingMessages_auto(L, {
                                                 {"ReceiveMessagesOnChannel", luasteam_ReceiveMessagesOnChannel_gs},
                                             });
}

void shutdown_NetworkingMessages(lua_State *L) { shutdown_NetworkingMessages_auto(L); }

void shutdown_GameServerNetworkingMessages(lua_State *L) { shutdown_GameServerNetworkingMessages_auto(L); }

} // namespace luasteam
