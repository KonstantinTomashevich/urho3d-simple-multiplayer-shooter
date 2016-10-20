namespace LogInToServer
{
    // *** Ids for ui buttons
    const int CONNECT_BUTTON = 0;
    const int UNKNOWN_BUTTON = -1;
    // ***
    
    class StateUi
    {
        // *** Protected variables
        protected IntVector2 lastScreenSize;
        protected XMLFile @styles_;
        protected UIElement @rootElement_;
        protected Sprite @background_;
            
        protected Text @inputAdressHint_;
        protected LineEdit @adressEdit_;
            
        protected Text @inputPortHint_;
        protected LineEdit @portEdit_;
            
        protected Text @inputNicknameHint_;
        protected LineEdit @nicknameEdit_;
            
        protected Button @connectButton_;
        protected Text @connectButtonText_;
        protected Text @connectingText_;
        float currentColorChangingDelta_ = 0.5f;
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
            
            background_ = rootElement_.CreateChild ("Sprite", "background");
            background_.texture = cache.GetResource ("Texture2D", "Textures/StoneDiffuse.dds");
            background_.color = Color (0.5, 0.5, 0.5, 1);
            
            inputAdressHint_ = rootElement_.CreateChild ("Text", "input_adress_hint");
            inputAdressHint_.text = "Input server adress:";
            inputAdressHint_.SetStyleAuto (styles_);
            
            adressEdit_ = rootElement_.CreateChild ("LineEdit", "adress_edit");
            adressEdit_.text = SharedGlobals::lastAdress;
            adressEdit_.SetStyleAuto (styles_);
            
            inputPortHint_ = rootElement_.CreateChild ("Text", "input_port_hint");
            inputPortHint_.text = "Input server port:";
            inputPortHint_.SetStyleAuto (styles_);
            
            portEdit_ = rootElement_.CreateChild ("LineEdit", "port_edit");
            portEdit_.text = "" + SharedGlobals::lastPort;
            portEdit_.SetStyleAuto (styles_);
            
            inputNicknameHint_ = rootElement_.CreateChild ("Text", "input_nickname_hint");
            inputNicknameHint_.text = "Input your nickname:";
            inputNicknameHint_.SetStyleAuto (styles_);
            
            nicknameEdit_ = rootElement_.CreateChild ("LineEdit", "nickname_edit");
            nicknameEdit_.text = SharedGlobals::lastNickname;
            nicknameEdit_.SetStyleAuto (styles_);
            
            connectButton_ = rootElement_.CreateChild ("Button", "connect_button");
            connectButton_.SetStyleAuto (styles_);
            
            connectButtonText_ = connectButton_.CreateChild ("Text", "text");
            connectButtonText_.text = "Connect";
            connectButtonText_.SetStyleAuto (styles_);
            connectButtonText_.SetAlignment (HA_CENTER, VA_CENTER);
            
            connectingText_ = rootElement_.CreateChild ("Text", "connectingText");
            connectingText_.text = "Connecting to \nXXX.XXX.XXX.XXX:XXXX...";
            connectingText_.SetStyleAuto (styles_);
            connectingText_.SetAlignment (HA_CENTER, VA_CENTER);
        }
        // ***
        
        // *** Calculate and set ui elements positions and sizes
        void Resize (int width, int height)
        {
            float aspectRatio = 1.0f * width / height;
            background_.SetPosition (0, 0);
            background_.SetSize (width, height);
            background_.imageRect = IntRect (
                            0, 0, background_.texture.width * aspectRatio, background_.texture.height);
            connectingText_.fontSize = height * 0.1f;
            
            inputAdressHint_.fontSize = height * 0.05f;                
            inputAdressHint_.SetPosition (width / 2 - inputAdressHint_.fontSize * 6, height * 0.1f);
            
            adressEdit_.textElement.fontSize = height * 0.05f;
            adressEdit_.SetPosition (width / 2 - inputAdressHint_.fontSize * 6, height * 0.2f);
            adressEdit_.SetSize (inputAdressHint_.fontSize * 12, height * 0.07f);
            
            inputPortHint_.fontSize = height * 0.05f;                
            inputPortHint_.SetPosition (width / 2 - inputPortHint_.fontSize * 6, height * 0.3f);
            
            portEdit_.textElement.fontSize = height * 0.05f;
            portEdit_.SetPosition (width / 2 - inputPortHint_.fontSize * 6, height * 0.4f);
            portEdit_.SetSize (inputPortHint_.fontSize * 12, height * 0.07f);
            
            inputNicknameHint_.fontSize = height * 0.05f;                
            inputNicknameHint_.SetPosition (width / 2 - inputNicknameHint_.fontSize * 6, height * 0.5f);
            
            nicknameEdit_.textElement.fontSize = height * 0.05f;
            nicknameEdit_.SetPosition (width / 2 - inputNicknameHint_.fontSize * 6, height * 0.6f);
            nicknameEdit_.SetSize (inputNicknameHint_.fontSize * 12, height * 0.07f);
            
            connectButtonText_.fontSize = height * 0.05f;
            connectButton_.SetPosition (width / 2 - inputNicknameHint_.fontSize * 6, height * 0.7f);
            connectButton_.SetSize (inputNicknameHint_.fontSize * 12, height * 0.1f);
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
        
            if (!isConnectingNow)
            {
                connectingText_.visible = false;
                Color color = connectButtonText_.colors [C_TOPLEFT];
                if (color.r >= 1.0f or color.r <= 0.5f)
                    currentColorChangingDelta_ = -currentColorChangingDelta_;
                
                color.r += currentColorChangingDelta_ * timeStep;
                color.g += currentColorChangingDelta_ * timeStep;
                color.b += currentColorChangingDelta_ * timeStep;
                connectButtonText_.color = color;
            }
            else
            {        
                connectingText_.visible = true;
                Color color = connectingText_.colors [C_TOPLEFT];
                if (color.r >= 1.0f or color.r <= 0.5f)
                    currentColorChangingDelta_ = -currentColorChangingDelta_;
                
                color.r += currentColorChangingDelta_ * timeStep;
                color.g += currentColorChangingDelta_ * timeStep;
                color.b += currentColorChangingDelta_ * timeStep;
                connectingText_.color = color;
                
                if (network.serverConnection !is null and network.serverConnection.connected)
                    connectingText_.text = "Loading scene... " + network.serverConnection.downloadProgress * 100.0f + "%.";
                else
                {
                    String adress = adressEdit_.text;
                    int port = portEdit_.text.ToInt ();
                    connectingText_.text = "Connecting to\n" + adress + ":" + port + "...";
                }
            }
        }
        // ***
        
        // *** Removes all ui elements
        void Clear ()
        {
            rootElement_.RemoveAllChildren ();
        }
        // ***
        
        void SetConnectionSetupFieldsVisible (bool visible)
        {
            inputAdressHint_.visible = visible;
            adressEdit_.visible = visible;
            inputPortHint_.visible = visible;
            portEdit_.visible = visible;
            inputNicknameHint_.visible = visible;
            nicknameEdit_.visible = visible;
            connectButton_.visible = visible;
        }
        
        String GetAdress ()
        {
            return adressEdit_.text;
        }
        
        int GetPort ()
        {
            return portEdit_.text.ToInt ();
        }
        
        String GetNickname ()
        {
            return nicknameEdit_.text;
        }
        
        int GetButtonId (Button @button)
        {
            if (button is connectButton_)
                return CONNECT_BUTTON;
            else
                return UNKNOWN_BUTTON;
        }
    };
}
