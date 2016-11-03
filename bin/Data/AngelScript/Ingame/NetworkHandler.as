namespace Ingame
{
    class NetworkHandler
    {
        protected bool waitingForSpawnSync_ = false;
        protected uint playerExpectedId_ = 0;
        
        protected void HandleNameSettedMessage (VariantMap &eventData)
        {
            VectorBuffer messageData = eventData ["Data"].GetBuffer ();
            stateUi.nickname_ = messageData.ReadString ();
            stateUi.isSpawned_ = false;
        }
        
        protected void HandleTimeUntilSpawnReturned (VariantMap &eventData)
        {
            VectorBuffer messageData = eventData ["Data"].GetBuffer ();
            stateUi.timeUntilSpawn_ = messageData.ReadFloat ();
        }
        
        protected void HandleChatMessage (VariantMap &eventData)
        {
            VectorBuffer messageData = eventData ["Data"].GetBuffer ();
            stateUi.AddChatMessage ("[" +  time.timeStamp.Substring (11, 8) +
                                    " " + messageData.ReadString () + "]" +
                                     " : " + messageData.ReadString ());
        }
        
        protected void HandleServerMessage (VariantMap &eventData)
        {
            VectorBuffer messageData = eventData ["Data"].GetBuffer ();
            stateUi.AddChatMessage ("{" + time.timeStamp.Substring (11, 8) + 
                                    " Server} : " + messageData.ReadString ());
        }
        
        protected void HandlePlayerSpawned (VariantMap &eventData)
        {
            VectorBuffer messageData = eventData ["Data"].GetBuffer ();
            playerExpectedId_ = messageData.ReadUInt ();
            waitingForSpawnSync_ = true;
        }
        
        NetworkHandler ()
        {
            
        }
        
        ~NetworkHandler ()
        {
            
        }
        
        void Update (float timeStep)
        {
            if (waitingForSpawnSync_)
            {
                localSceneManager.playerNodeId = playerExpectedId_;
                if (localSceneManager.playerNodeId == playerExpectedId_)
                {
                    waitingForSpawnSync_ = false;
                    stateUi.isSpawned_ = true;
                }
            }
        }
        
        void SendNickname ()
        {
            VectorBuffer messageData = VectorBuffer ();
            messageData.WriteString (SharedGlobals::lastNickname);
            network.serverConnection.SendMessage (NMID_CTS_REQUEST_NAME, true, false, messageData);
        }
        
        void SendChatMessage ()
        {
            String message = stateUi.messageEdit_.text;
            stateUi.messageEdit_.text = "";
            stateUi.messageEdit_.selected = false;
            stateUi.messageEdit_.focus = false;
            
            VectorBuffer messageData = VectorBuffer ();
            messageData.WriteString (message);
            network.serverConnection.SendMessage (NMID_CTS_REQUEST_CHAT_MESSAGE, true, false, messageData);
        }
        
        void RequestTimeUntilSpawnFromServer ()
        {
            VectorBuffer messageData = VectorBuffer ();
            if (network.serverConnection !is null)
                network.serverConnection.SendMessage (NMID_CTS_GET_TIME_UNTIL_SPAWN, true, false, messageData);
        }
        
        void SendMoveRequest (Vector2 request)
        {
            VectorBuffer messageData = VectorBuffer ();
            messageData.WriteVector2 (request);
            if (network.serverConnection !is null)
                network.serverConnection.SendMessage (NMID_CTS_SET_MOVE_REQUEST, true, false, messageData);
        }
        
        void SendFireRequest ()
        {
            if (network.serverConnection !is null)
                network.serverConnection.SendMessage (NMID_CTS_REQUEST_FIRE, true, false, VectorBuffer ());
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            if (eventData ["MessageID"] == NMID_STC_PLAYER_NAME_SETTED)
                HandleNameSettedMessage (eventData);
            else if (eventData ["MessageID"] == NMID_STC_RETURN_TIME_UNTIL_SPAWN)
                HandleTimeUntilSpawnReturned (eventData);
            else if (eventData ["MessageID"] == NMID_STC_CHAT_MESSAGE)
                HandleChatMessage (eventData);
            else if (eventData ["MessageID"] == NMID_STC_SERVER_MESSAGE)
                HandleServerMessage (eventData);
            else if (eventData ["MessageID"] == NMID_STC_PLAYER_SPAWNED)
                HandlePlayerSpawned (eventData);
        }
    };
}
