local Steam = require 'luasteam'

local connection = nil

function love.load()
   math.randomseed(os.time())

   Steam.init()
   --Steam.networkingUtils.initRelayNetworkAccess()
   Steam.networkingSockets.initAuthentication()
   --socket = Steam.networkingSockets.connectByIPAddress("127.0.0.1:55556")
   --Steam.networkingSockets.connectP2P(Steam.extra.parseUint64("85568392926268238"), 0, { Unencrypted = 2, IP_AllowWithoutAuth = 1})
end

function Steam.networkingSockets.onConnectionChanged(data)
	print ('Connection changed', data.connection, data.state, data.state_old, data.endReason, data.debug)
   --print(Steam.networkingSockets.getConnectionInfo(data.connection))
   if  data.state == "Connected" then
      print(connection, data.connection)
   else
      --connection = nil
   end
   if data.state == "ClosedByPeer" or data.state == "ProblemDetectedLocally" then
      connection = Steam.networkingSockets.closeConnection(data.connection)
   end
end

function Steam.networkingSockets.onAuthenticationStatus(data)
   print("onAuthenticationStatus", data.status, data.debugMsg)
   if data.status == 100 and not connection then
		print(Steam.networkingSockets.getIdentity())
      connection = Steam.networkingSockets.connectByIPAddress("127.0.0.1:55556")
   end
end

function love.update(dt)
   Steam.runCallbacks()
   if connection then
      local n, messages = Steam.networkingSockets.receiveMessagesOnConnection(connection)
      
      if n > -1 then
         for j,m in ipairs(messages) do
            print("received message", j, m, "from connection", connection, type(j))
            print(Steam.networkingSockets.getConnectionInfo(connection))
            --Steam.networkingSockets.sendMessageToConnection(connection, , Steam.networkingSockets.flags.Send_Reliable)
            --Steam.networkingSockets.sendMessageToConnection(connection, , Steam.networkingSockets.flags.Send_Reliable)
            local sendM = {}
            sendM[1] = { conn = connection, msg = "Hello server! This is the client! Thank you!", flag = Steam.networkingSockets.flags.Send_Reliable }
            sendM[2] = { conn = connection, msg = "Immediately one more message!", flag = Steam.networkingSockets.flags.Send_Reliable }
            local result = Steam.networkingSockets.sendMessages(#sendM, sendM)
            print("sendMessages result", result[1], result[2])
         end
      end
   end
end


function love.draw()
   love.graphics.print("Client", 400, 300)
end

function love.quit()
   if connection then
      Steam.networkingSockets.closeConnection(connection)
   end
   Steam.shutdown()
end
