namespace Ingame
{
    class UiListener
    {
        UiListener ()
        {
            
        }
        
        ~UiListener ()
        {
            
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            if (stateUi.GetButtonId (eventData ["Element"].GetPtr ()) == SEND_MESSAGE_BUTTON)
                networkHandler.SendChatMessage ();
        }
    };
}
