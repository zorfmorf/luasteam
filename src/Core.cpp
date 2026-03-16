#include "Core.hpp"
#include "Client.hpp"
#include "Extra.hpp"
#include "NetworkingMessages.hpp"
#include "NetworkingSockets.hpp"
#include "NetworkingUtils.hpp"
#include "SteamNetworkingMessage.hpp"
#include "Utils.hpp"
#include "auto/auto.hpp"
#include <vector>

// ========================
// ======= SteamAPI =======
// ========================

namespace {

using add_f = void (*)(lua_State *, std::initializer_list<luaL_Reg>);
using shutdown_f = void (*)(lua_State *);
using inter = std::pair<add_f, shutdown_f>;

// clang-format off
const std::vector<inter> user_interfaces = {
	std::make_pair(luasteam::add_Apps_auto, luasteam::shutdown_Apps_auto),
    {luasteam::add_Client, luasteam::shutdown_Client},
    {luasteam::add_Controller_auto, luasteam::shutdown_Controller_auto},
    {luasteam::add_Friends_auto, luasteam::shutdown_Friends_auto},
    {luasteam::add_HTMLSurface_auto, luasteam::shutdown_HTMLSurface_auto},
    {luasteam::add_HTTP_auto, luasteam::shutdown_HTTP_auto},
    {luasteam::add_Input_auto, luasteam::shutdown_Input_auto},
    {luasteam::add_Inventory_auto, luasteam::shutdown_Inventory_auto},
    {luasteam::add_MatchmakingServers_auto, luasteam::shutdown_MatchmakingServers_auto},
    {luasteam::add_Matchmaking_auto, luasteam::shutdown_Matchmaking_auto},
    {luasteam::add_Music_auto, luasteam::shutdown_Music_auto},
    {luasteam::add_NetworkingMessages, luasteam::shutdown_NetworkingMessages},
    {luasteam::add_NetworkingSockets, luasteam::shutdown_NetworkingSockets},
    {luasteam::add_NetworkingUtils, luasteam::shutdown_NetworkingUtils},
    {luasteam::add_Networking_auto, luasteam::shutdown_Networking_auto},
    {luasteam::add_ParentalSettings_auto, luasteam::shutdown_ParentalSettings_auto},
    {luasteam::add_Parties_auto, luasteam::shutdown_Parties_auto},
    {luasteam::add_RemotePlay_auto, luasteam::shutdown_RemotePlay_auto},
    {luasteam::add_RemoteStorage_auto, luasteam::shutdown_RemoteStorage_auto},
    {luasteam::add_Screenshots_auto, luasteam::shutdown_Screenshots_auto},
    {luasteam::add_Timeline_auto, luasteam::shutdown_Timeline_auto},
    {luasteam::add_UGC_auto, luasteam::shutdown_UGC_auto},
    {luasteam::add_UserStats_auto, luasteam::shutdown_UserStats_auto},
    {luasteam::add_User_auto, luasteam::shutdown_User_auto},
    {luasteam::add_Utils, luasteam::shutdown_Utils},
    {luasteam::add_Video_auto, luasteam::shutdown_Video_auto},
    {luasteam::add_callback_interfaces_auto, luasteam::shutdown_callback_interfaces_auto},
};

const std::vector<inter> gs_interfaces = {
    std::make_pair(luasteam::add_GameServerClient, luasteam::shutdown_GameServerClient),
    {luasteam::add_GameServerHTTP_auto, luasteam::shutdown_GameServerHTTP_auto},
    {luasteam::add_GameServerInventory_auto, luasteam::shutdown_GameServerInventory_auto},
    {luasteam::add_GameServerNetworkingMessages, luasteam::shutdown_GameServerNetworkingMessages},
    {luasteam::add_GameServerNetworkingSockets, luasteam::shutdown_GameServerNetworkingSockets},
    {luasteam::add_GameServerNetworking_auto, luasteam::shutdown_GameServerNetworking_auto},
    {luasteam::add_GameServerStats_auto, luasteam::shutdown_GameServerStats_auto},
    {luasteam::add_GameServerUGC_auto, luasteam::shutdown_GameServerUGC_auto},
    {luasteam::add_GameServerUtils_auto, luasteam::shutdown_GameServerUtils_auto},
    {luasteam::add_GameServer_auto, luasteam::shutdown_GameServer_auto},
};
// clang-format on

void common_shutdown(lua_State *L) {
    luasteam::shutdown_SteamNetworkingMessage_t(L);
    luasteam::shutdown_callback_interfaces_auto(L);
    luasteam::shutdown_structs_auto(L);
    // luasteam::shutdown_consts_auto(L);
    // luasteam::shutdown_enums_auto(L);
    luasteam::shutdown_Extra(L);
    luasteam::shutdown_Common(L);
    // luasteam::shutdown_Core(L);
}

} // namespace

// Manually implemented because it's a core initialization function
// bool SteamAPI_Init();
EXTERN int luasteam_init(lua_State *L) {
    bool success = SteamAPI_Init();
    if (success) {
        luasteam::push_steam_table(L);
        for (auto [add, _] : user_interfaces)
            add(L, {});
        lua_pop(L, 1);
    } else {
        fprintf(stderr, "Couldn't connect to steam...\nPlease ensure that the following conditions are met:\n* Do you have Steam turned on?\n* If not running from steam, do you have a correct steam_appid.txt file?\n* Is the application running under the same user context as steam?\n* Is a license for the App ID present in your active steam account?\n* Is your App ID correctly set up, i.e. not in ``Release State: Unavailable`` and not missing default packages?\n");
    }
    lua_pushboolean(L, success);
    return 1;
}

// Manually implemented because it's a core shutdown function
// void SteamAPI_Shutdown();
EXTERN int luasteam_shutdown(lua_State *L) {
    SteamAPI_Shutdown();
    for (auto it = user_interfaces.rbegin(); it != user_interfaces.rend(); ++it) {
        auto [_, shutdown] = *it;
        shutdown(L);
    }
    common_shutdown(L);
    return 0;
}

// Manually implemented because it's a core function
// void SteamAPI_RunCallbacks();
EXTERN int luasteam_runCallbacks(lua_State *L) {
    SteamAPI_RunCallbacks();
    return 0;
}

// Manually implemented because it's a core initialization function
// bool SteamGameServer_Init()
EXTERN int luasteam_init_server(lua_State *L) {
    uint32 ip = luaL_checkinteger(L, 1);
    uint16 usGamePort = luaL_checkinteger(L, 2);
    uint16 usQueryPort = luaL_checkinteger(L, 3);
    EServerMode eServerMode = static_cast<EServerMode>(luaL_checkinteger(L, 4));
    const char *version = (char *)luaL_checkstring(L, 5);
    bool success = SteamGameServer_Init(ip, usGamePort, usQueryPort, eServerMode, version);
    if (success) {
        luasteam::push_steam_table(L);
        for (auto [add, _] : gs_interfaces) {
            add(L, {});
        }
        lua_pop(L, 1);
    } else {
        fprintf(stderr, "Couldn't init game server...\nDo you have a correct steam_appid.txt file?\n");
    }
    lua_pushboolean(L, success);
    return 1;
}

// Manually implemented because it's a core function
// void SteamGameServer_RunCallbacks()
EXTERN int luasteam_runCallbacks_server(lua_State *L) {
    SteamGameServer_RunCallbacks();
    return 0;
}

// Manually implemented because it's a core shutdown function
// void SteamGameServer_Shutdown()
EXTERN int luasteam_shutdown_server(lua_State *L) {
    SteamGameServer_Shutdown();
    for (auto it = gs_interfaces.rbegin(); it != gs_interfaces.rend(); ++it) {
        auto [_, shutdown] = *it;
        shutdown(L);
    }
    common_shutdown(L);
    return 0;
}

namespace luasteam {

void add_Core(lua_State *L) {
    add_func(L, "Init", luasteam_init);
    add_func(L, "Shutdown", luasteam_shutdown);
    add_func(L, "RunCallbacks", luasteam_runCallbacks);
    add_func(L, "GameServerInit", luasteam_init_server);
    add_func(L, "GameServerShutdown", luasteam_shutdown_server);
    add_func(L, "GameServerRunCallbacks", luasteam_runCallbacks_server);
}

} // namespace luasteam
