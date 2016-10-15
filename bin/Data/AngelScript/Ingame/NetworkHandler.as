namespace Ingame
{
    class NetworkHandler
    {
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
            stateUi.AddChatMessage (messageData.ReadString () + ": " + messageData.ReadString ());
        }
        
        NetworkHandler ()
        {
            
        }
        
        ~NetworkHandler ()
        {
            
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
        
        void GetTimeUntilSpawn ()
        {
            VectorBuffer messageData = VectorBuffer ();
            network.serverConnection.SendMessage (NMID_CTS_GET_TIME_UNTIL_SPAWN, true, false, messageData);
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            if (eventData ["MessageID"] == NMID_STC_PLAYER_NAME_SETTED)
                HandleNameSettedMessage (eventData);
            else if (eventData ["MessageID"] == NMID_STC_RETURN_TIME_UNTIL_SPAWN)
                HandleTimeUntilSpawnReturned (eventData);
            else if (eventData ["MessageID"] == NMID_STC_CHAT_MESSAGE)
                HandleChatMessage (eventData);
        }
    };
}
