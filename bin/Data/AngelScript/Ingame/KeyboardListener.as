namespace Ingame
{ 
    // *** Keyboard buttons constants
    const int KEY_SWITCH_INPUT_CHAT_MESSAGE = KEY_TAB;
    const int KEY_SEND_CHAT_MESSAGE = KEY_RETURN;
    
    const int KEY_MOVE_FORWARD = KEY_W;
    const int KEY_MOVE_BACK= KEY_S;
    const int KEY_ROTATE_RIGHT = KEY_D;
    const int KEY_ROTATE_LEFT = KEY_A;
    // ***
    
    // *** Constants for keyboard listener
    const float TIME_BETWEEN_MOVE_REQUESTS_SENDING = 1.0f / 25.0f;
    // ***
    
    class KeyboardListener
    {
        protected float timeFromLastMoveRequest = 0.0f;
        
        KeyboardListener ()
        {
            
        }
        
        ~KeyboardListener ()
        {
            
        }
        
        void Update (float timeStep)
        {
            timeFromLastMoveRequest += timeStep;
            if (input.keyPress [KEY_SWITCH_INPUT_CHAT_MESSAGE])
            {
                stateUi.messageEdit_.selected = !stateUi.messageEdit_.selected;
                stateUi.messageEdit_.focus = !stateUi.messageEdit_.focus;
            }
            
            if (input.keyPress [KEY_SEND_CHAT_MESSAGE] and stateUi.messageEdit_.focus == true)
                networkHandler.SendChatMessage ();
             
            if (localSceneManager.playerNode !is null 
                and timeFromLastMoveRequest >= TIME_BETWEEN_MOVE_REQUESTS_SENDING)
            {
                Vector2 moveRequest = Vector2 (0, 0);
                if (stateUi.messageEdit_.focus == false)
                {
                    if (input.keyDown [KEY_MOVE_FORWARD])
                        moveRequest.x += 1;
                    if (input.keyDown [KEY_MOVE_BACK])
                        moveRequest.x -= 1;
                    
                    if (input.keyDown [KEY_ROTATE_RIGHT])
                        moveRequest.y += 1;
                    if (input.keyDown [KEY_ROTATE_LEFT])
                        moveRequest.y -= 1;
                }
                networkHandler.SendMoveRequest (moveRequest);
                timeFromLastMoveRequest = 0.0f;
            }
                
            if (localSceneManager.playerNode is null)
                timeFromLastMoveRequest = 0.0f;
        }
    };
}
