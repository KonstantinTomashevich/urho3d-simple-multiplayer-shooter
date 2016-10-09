namespace Ingame
{
    // *** Ids for ui buttons
    int UNKNOWN_BUTTON = -1;
    // ***
    
    class StateUi
    {
        // *** Protected variables
        protected IntVector2 lastScreenSize;
        protected XMLFile @styles_;
        protected UIElement @rootElement_;
        protected Text @infoText_;
        // ***
        
        // *** Public variables
        String nickname_ = "...";
        bool isSpawned_ = false;
        float timeUntilSpawn_ = 0.0;
        int lives_ = 0;
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
        }
        // ***
        
        // *** Calculate and set ui elements positions and sizes
        void Resize (int width, int height)
        {
            float aspectRatio = 1.0f * width / height;
            infoText_.SetPosition (height * 0.03f, height * 0.03f);
            infoText_.fontSize = height * 0.05f;
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
                info += lives_ + "/100 HP.";
            else
                info += Floor (timeUntilSpawn_) + "s until respawn.";
            infoText_.text = info;
            
            if (!isSpawned_)
                networkHandler.GetTimeUntilSpawn ();
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
            return UNKNOWN_BUTTON;
        }
    };
}
