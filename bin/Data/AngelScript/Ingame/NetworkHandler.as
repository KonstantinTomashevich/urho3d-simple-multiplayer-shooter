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
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            if (eventData ["MessageID"] == NMID_STC_PLAYER_NAME_SETTED)
                HandleNameSettedMessage (eventData);
        }
    };
}
