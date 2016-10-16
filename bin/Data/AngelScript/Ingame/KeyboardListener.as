namespace Ingame
{ 
    // *** Keyboard buttons constants
    int KEY_SWITCH_INPUT_CHAT_MESSAGE = KEY_TAB;
    int KEY_SEND_CHAT_MESSAGE = KEY_RETURN;
    // ***
    
    class KeyboardListener
    {
        KeyboardListener ()
        {
            
        }
        
        ~KeyboardListener ()
        {
            
        }
        
        void Update (float timeStep)
        {
            if (input.keyPress [KEY_SWITCH_INPUT_CHAT_MESSAGE])
            {
                stateUi.messageEdit_.selected = !stateUi.messageEdit_.selected;
                stateUi.messageEdit_.focus = !stateUi.messageEdit_.focus;
            }
            
            if (input.keyPress [KEY_SEND_CHAT_MESSAGE] and stateUi.messageEdit_.focus == true)
                networkHandler.SendChatMessage ();
        }
    };
}
