local Steam = require("luasteam")

local ok = Steam.GameServerInit(0, 0, 0, Steam.eServerModeNoAuthentication, "1.0.0.0")
assert(ok, "Failed to initialize game server")
print("GameServerInit OK")

Steam.GameServer.SetDedicatedServer(true)
Steam.GameServer.LogOnAnonymous()
local connections = {}

function Steam.GameServer.OnSteamServersConnected()
    print("SteamServersConnected: " .. tostring(Steam.GameServer.GetSteamID()))

    Steam.GameServerNetworkingSockets.InitAuthentication()
end

function Steam.GameServerNetworkingSockets.OnSteamNetConnectionStatusChangedCallback(data)
    if data.m_info.m_eState == Steam.k_ESteamNetworkingConnectionState_Connecting then
        print("Accepting connection from " .. data.m_hConn)
        Steam.GameServerNetworkingSockets.AcceptConnection(data.m_hConn)
    end
    if  data.m_info.m_eState == Steam.k_ESteamNetworkingConnectionState_Connected then
        print("Connected to", data.m_hConn)
        connections[data.m_hConn] = true
        local message = "Hello client! This is the game server! Welcome!"
        Steam.GameServerNetworkingSockets.SendMessageToConnection(data.m_hConn, message, #message, Steam.k_nSteamNetworkingSend_Reliable)
    end
    if data.m_info.m_eState == Steam.k_ESteamNetworkingConnectionState_ClosedByPeer or data.m_info.m_eState == Steam.k_ESteamNetworkingConnectionState_ProblemDetectedLocally then
        print("Connection closed: " .. data.m_hConn)
        Steam.GameServerNetworkingSockets.CloseConnection(data.m_hConn, 0, "", false)
        connections[data.m_hConn] = nil
    end
end

function Steam.GameServerNetworkingSockets.OnSteamNetAuthenticationStatus(data)
    if data.m_eAvail == 100 and not connection then
        local addr = Steam.newSteamNetworkingIPAddr {}
        addr:ParseString("127.0.0.1:55556")
        server = Steam.GameServerNetworkingSockets.CreateListenSocketIP(addr, 0, nil)
        print("Listening on port 55556")
    end
end

local run = true
while run do
    Steam.GameServerRunCallbacks()

    for conn,_ in pairs(connections) do
        local n, messages = Steam.GameServerNetworkingSockets.ReceiveMessagesOnConnection(conn, 32)
        if n > 0 then
            for j,m in ipairs(messages) do
                print("Received message from connection " .. conn .. ": " .. m:GetData())
                run = false
                m:Release()
            end
        end
    end
end

print("Closing connections...")
for conn,_ in pairs(connections) do
    Steam.GameServerNetworkingSockets.FlushMessagesOnConnection(conn)
    Steam.GameServerNetworkingSockets.CloseConnection(conn, 0, "", false)
end
print("Closing server...")
Steam.GameServerNetworkingSockets.CloseListenSocket(server)
Steam.GameServerShutdown()