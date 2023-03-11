local Steam = require 'luasteam'
local server = nil
local connections = {}
local poll = nil

function love.load()
   math.randomseed(os.time())
   local gamePort = 55557
   local queryPort = 55006
   local result = Steam.gameServer.init(0, gamePort, queryPort, Steam.gameServer.mode.Authentication, "0.0.1")
   print(Steam.networkingSockets.getIdentity())
   --local result = Steam.gameServer.init(0, gamePort, queryPort, Steam.gameServer.mode.NoAuthentication, "0.0.1")
   if result then
      Steam.networkingSockets.initAuthentication()
      --Steam.gameServer.setDedicatedServer(true)
      Steam.gameServer.logOn("03CACA95744ED41414F49362C67DD042")
      --Steam.networkingUtils.initRelayNetworkAccess()
      --server = Steam.networkingSockets.createListenSocketP2P(0, { Unencrypted = 2, IP_AllowWithoutAuth = 1})
   end

   -- Steam.init()
   -- server = Steam.networkingSockets.createListenSocketIP("[::]:55556")
end

function Steam.networkingSockets.onConnectionChanged(data)
   print ('Connection changed', data.connection, data.state, data.state_old, data.endReason, data.debug)
   print(Steam.networkingSockets.getConnectionInfo(data.connection))
   if data.state == "Connecting" then
      Steam.networkingSockets.acceptConnection(data.connection)
   end
   if data.state == "Connected" then
      connections[data.connection] = true
      print("Adding connection " .. data.connection .. " to poll group " .. tostring(poll) .. " result = " .. tostring(Steam.networkingSockets.setConnectionPollGroup(data.connection, poll)))
      Steam.networkingSockets.sendMessageToConnection(data.connection, "Hello client! This is the server! Welcome!", Steam.networkingSockets.flags.Send_Reliable)
   else
      connections[data.connection] = nil
   end
   if data.state == "ClosedByPeer" or data.state == "ProblemDetectedLocally" then
      print(data.connection, data.state, data.endReason, data.debug)
      Steam.networkingSockets.closeConnection(data.connection)
   end
end

function Steam.networkingSockets.onAuthenticationStatus(data)
   print("onAuthenticationStatus", data.status, data.debugMsg)
   print(Steam.networkingSockets.getIdentity())
end

function Steam.gameServer.onSteamServersConnected()
   print("SteamServersConnected")
end

function Steam.gameServer.onSteamServersDisconnected(data)
   print("onSteamServersDisconnected", data.result)
end

function Steam.gameServer.onSteamServerConnectFailure(data)
   print("onSteamServerConnectFailure", data.result, data.stillRetrying)
end

local i = 0
function love.update(dt)
   i = i + 1
   Steam.gameServer.runCallbacks()
   -- Steam.runCallbacks()
   --for conn,_ in pairs(connections) do
   --local messages = Steam.networkingSockets.receiveMessagesOnConnection(conn)
   if poll then
      local messages = Steam.networkingSockets.receiveMessagesOnPollGroup(poll)
      for j,m in ipairs(messages) do
         print("received message", j, m.msg, "from connection", m.conn, type(m))
         --print("received message", j, m, "from connection", conn, type(j))
         print("Logged on:", Steam.gameServer.bLoggedOn())
         print("Secure:", Steam.gameServer.bSecure())
      end
   end
   
   if i == 1000 then
      print("Logged on:", Steam.gameServer.bLoggedOn())
      print("Secure:", Steam.gameServer.bSecure())
      print("SteamID of Server:", Steam.gameServer.getSteamID())
      server = Steam.networkingSockets.createListenSocketIP("[::]:55556")
      poll = Steam.networkingSockets.createPollGroup()
   end
end


function love.draw()
   love.graphics.print("Server", 400, 300)
end

function love.quit()
   for conn,_ in pairs(connections) do
      Steam.networkingSockets.closeConnection(conn)
   end
   if poll then
      Steam.networkingSockets.destroyPollGroup(poll)
   end
   if Steam.gameServer.bLoggedOn() then
     Steam.gameServer.logOff()
   end
   Steam.gameServer.shutdown()
end
