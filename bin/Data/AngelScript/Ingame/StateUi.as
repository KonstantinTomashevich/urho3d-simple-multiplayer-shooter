namespace Ingame
{
    // *** Ids for ui buttons
    int UNKNOWN_BUTTON = -1;
    int SEND_MESSAGE_BUTTON = 0;
    // ***
    
    class StateUi
    {
        // *** Protected variables
        protected IntVector2 lastScreenSize;
        protected XMLFile @styles_;
        protected UIElement @rootElement_;
        protected Text @infoText_;
        protected Array <String> chatHistory_;
        protected Text @chatHistoryUI_;
        protected Button @sendMessageButton_;
        protected Text @sendMessageButtonText_;
        // ***
        
        // *** Public variables
        LineEdit @messageEdit_;
        String nickname_ = "...";
        bool isSpawned_ = false;
        float timeUntilSpawn_ = 0.0;
        // ***
        
        StateUi ()
        {
            
        }
        
        ~StateUi ()
        {
            
        }
        
        // *** Add all ui elements as childs of specified elements or to ui root if argument not specified.
        void Setup (UIElement @rootElement = null)
        {
            styles_ = cache.GetResource ("XMLFile", "UI/DefaultStyle.xml");
            if (rootElement is null)
                rootElement_ = ui.root;
            else
                rootElement_ = rootElement;
            
            infoText_ = rootElement_.CreateChild ("Text", "info_text");
            infoText_.text = "...\n...";
            infoText_.SetStyleAuto (styles_);
            
            chatHistory_.Clear ();
            chatHistoryUI_ = rootElement_.CreateChild ("Text", "chat");
            chatHistoryUI_.text = "No chat messages yet...";
            chatHistoryUI_.SetStyleAuto (styles_);
            
            messageEdit_ = rootElement_.CreateChild ("LineEdit", "message_edit");
            messageEdit_.SetStyleAuto (styles_);
            messageEdit_.textElement.SetAlignment (HA_LEFT, VA_CENTER);
            
            sendMessageButton_ = rootElement_.CreateChild ("Button", "send_message");
            sendMessageButton_.SetStyleAuto (styles_);
            
            sendMessageButtonText_ = sendMessageButton_.CreateChild ("Text", "text");
            sendMessageButtonText_.text = "Send";
            sendMessageButtonText_.SetStyleAuto (styles_);
            sendMessageButtonText_.SetAlignment (HA_CENTER, VA_CENTER);
        }
        // ***
        
        // *** Calculate and set ui elements positions and sizes
        void Resize (int width, int height)
        {
            float aspectRatio = 1.0f * width / height;
            infoText_.SetPosition (height * 0.03f, height * 0.03f);
            infoText_.fontSize = height * 0.05f;
            
            chatHistoryUI_.SetPosition (height * 0.03f, height * 0.7f);
            chatHistoryUI_.fontSize = height * 0.02f;
            
            sendMessageButton_.SetPosition (width - height * 0.13f, height * 0.93f);
            sendMessageButton_.SetSize (height * 0.1f, height * 0.04f);
            sendMessageButtonText_.fontSize = height * 0.02f;
            
            messageEdit_.SetPosition (width - height * 0.63f, height * 0.93f);
            messageEdit_.SetSize (height * 0.5f, height * 0.04f);
            messageEdit_.textElement.fontSize = height * 0.02f;
        }
        // ***
        
        // *** Update ui layout
        void Update (float timeStep)
        {
            if (lastScreenSize.x != graphics.width or lastScreenSize.y != graphics.height)
            {
                lastScreenSize.x = graphics.width;
                lastScreenSize.y = graphics.height;
                Resize (lastScreenSize.x, lastScreenSize.y);
            }
            
            String info = nickname_ + "\n";
            if (isSpawned_)
                info += Floor (localSceneManager.playerLives) + "/100 HP.";
            else
                info += Floor (timeUntilSpawn_) + "s until respawn.";
            infoText_.text = info;
            
            if (!isSpawned_)
                networkHandler.GetTimeUntilSpawn ();
                
            
            if (chatHistory_.length == 0)
                chatHistoryUI_.text = "No chat messages yet...";
            else
            {
                chatHistoryUI_.text = "";
                while (chatHistory_.length > 10)
                    chatHistory_.Erase (0);
                for (int index = 0; index < chatHistory_.length; index++)
                    chatHistoryUI_.text = chatHistoryUI_.text + "\n" + chatHistory_ [index];
            }

        }
        // ***
        
        // *** Removes all ui elements
        void Clear ()
        {
            rootElement_.RemoveAllChildren ();
        }
        // ***
        
        int GetButtonId (Button @button)
        {
            if (button is sendMessageButton_)
                return SEND_MESSAGE_BUTTON;
            else
                return UNKNOWN_BUTTON;
        }
        
        void AddChatMessage (String message)
        {
            chatHistory_.Push (message);
        }
    };
}
