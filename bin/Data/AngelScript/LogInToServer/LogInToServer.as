class LogInToServer : GameState
{
// *** Variables for connection process
    Scene @scene_;
    float timeFromConnectionStart_ = -1.0f;
// ***   

// *** Variables for UI
    protected IntVector2 lastScreenSize;
    protected XMLFile @styles_;
    protected Sprite @background_;
    
    protected Text @inputIpHint_;
    protected LineEdit @ipEdit_;
    
    protected Text @inputPortHint_;
    protected LineEdit @portEdit_;
    
    protected Text @inputNicknameHint_;
    protected LineEdit @nicknameEdit_;
    
    protected Button @connectButton_;
    protected Text @connectButtonText_;
    protected Text @connectingText_;
    float currentColorChangingDelta_ = 0.5f;
// ***
  
// *** UI managing functions
    protected void SetupUi ()
    {
        styles_ = cache.GetResource ("XMLFile", "UI/DefaultStyle.xml");
        ui.root.RemoveAllChildren ();
        
        background_ = ui.root.CreateChild ("Sprite", "background");
        background_.texture = cache.GetResource ("Texture2D", "Textures/StoneDiffuse.dds");
        background_.color = Color (0.5, 0.5, 0.5, 1);
        
        inputIpHint_ = ui.root.CreateChild ("Text", "input_ip_hint");
        inputIpHint_.text = "Input server IP:";
        inputIpHint_.SetStyleAuto (styles_);
        
        ipEdit_ = ui.root.CreateChild ("LineEdit", "ip_edit");
        ipEdit_.text = "127.0.0.1";
        ipEdit_.SetStyleAuto (styles_);
        
        inputPortHint_ = ui.root.CreateChild ("Text", "input_port_hint");
        inputPortHint_.text = "Input server port:";
        inputPortHint_.SetStyleAuto (styles_);
        
        portEdit_ = ui.root.CreateChild ("LineEdit", "port_edit");
        portEdit_.text = "" + ServerConstants__SERVER_PORT;
        portEdit_.SetStyleAuto (styles_);
        
        inputNicknameHint_ = ui.root.CreateChild ("Text", "input_nickname_hint");
        inputNicknameHint_.text = "Input your nickname:";
        inputNicknameHint_.SetStyleAuto (styles_);
        
        nicknameEdit_ = ui.root.CreateChild ("LineEdit", "nickname_edit");
        nicknameEdit_.text = "AnonymousPlayer";
        nicknameEdit_.SetStyleAuto (styles_);
        
        connectButton_ = ui.root.CreateChild ("Button", "connect_button");
        connectButton_.SetStyleAuto (styles_);
        
        connectButtonText_ = connectButton_.CreateChild ("Text", "text");
        connectButtonText_.text = "Connect";
        connectButtonText_.SetStyleAuto (styles_);
        connectButtonText_.SetAlignment (HA_CENTER, VA_CENTER);
        
        connectingText_ = ui.root.CreateChild ("Text", "connectingText");
        connectingText_.text = "Connecting to \nXXX.XXX.XXX.XXX:XXXX...";
        connectingText_.SetStyleAuto (styles_);
        connectingText_.SetAlignment (HA_CENTER, VA_CENTER);
    }
    
    protected void ResizeUi (int width, int height)
    {
        float aspectRatio = 1.0f * width / height;
        background_.SetPosition (0, 0);
        background_.SetSize (width, height);
        background_.imageRect = IntRect (
                        0, 0, background_.texture.width * aspectRatio, background_.texture.height);
        connectingText_.fontSize = height * 0.1f;
        
        inputIpHint_.fontSize = height * 0.05f;                
        inputIpHint_.SetPosition (width / 2 - inputIpHint_.fontSize * 6, height * 0.1f);
        
        ipEdit_.textElement.fontSize = height * 0.05f;
        ipEdit_.SetPosition (width / 2 - inputIpHint_.fontSize * 6, height * 0.2f);
        ipEdit_.SetSize (inputIpHint_.fontSize * 12, height * 0.07f);
        
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
    
    protected void UpdateUi (float timeStep)
    {
        if (timeFromConnectionStart_ < 0.0f)
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
            timeFromConnectionStart_ += timeStep;
            connectingText_.visible = true;
            
            Color color = connectingText_.colors [C_TOPLEFT];
            if (color.r >= 1.0f or color.r <= 0.5f)
                currentColorChangingDelta_ = -currentColorChangingDelta_;
            
            color.r += currentColorChangingDelta_ * timeStep;
            color.g += currentColorChangingDelta_ * timeStep;
            color.b += currentColorChangingDelta_ * timeStep;
            connectingText_.color = color;
        }
    }
// ***

// *** Constructor and destructor
    LogInToServer ()
    {
        SubscribeToEvent ("UIMouseClickEnd", "HandleEvent");
    }
    
    ~LogInToServer () 
    {
        UnsubscribeFromAllEvents ();
    }
// ***
  
// *** State lifecycle functions
    void Setup ()
    {
        SetupUi ();
        lastScreenSize.x = graphics.width;
        lastScreenSize.y = graphics.height;
        ResizeUi (lastScreenSize.x, lastScreenSize.y);
    }
    
    void Update (float timeStep)
    {
        if (lastScreenSize.x != graphics.width or lastScreenSize.y != graphics.height)
        {
            lastScreenSize.x = graphics.width;
            lastScreenSize.y = graphics.height;
            ResizeUi (lastScreenSize.x, lastScreenSize.y);
        }
        UpdateUi (timeStep);
        
        if (network.serverConnection !is null and network.serverConnection.connected)
            SwitchToIngameState ();
        else if (timeFromConnectionStart_ > GameConstants__CONNECTION_TIMEOUT)
        {
            String ip = ipEdit_.text;
            int port = portEdit_.text.ToInt ();
            ErrorDialog ("Connection timeout!", "Connection to " + ip + ":" + port + " failed!");
            engine.Exit ();
        }
    }
    
    void Dispose ()
    {
        ui.root.RemoveAllChildren ();
    }
// ***

// *** Main events handling function
    void HandleEvent (StringHash eventType, VariantMap &eventData)
    {
        if (eventType == StringHash ("UIMouseClickEnd") and eventData ["Element"].GetPtr () is connectButton_)
            HandleConnectButtonClick (eventData);
    }
// ***

// *** UI events handling functions
    protected void HandleConnectButtonClick (VariantMap &eventData)
    {
        String ip = ipEdit_.text;
        int port = portEdit_.text.ToInt ();
        
        scene_ = Scene ();
        network.Connect (ip, port, scene_);
        timeFromConnectionStart_ = 0.0f;
        connectingText_.text = "Connecting to\n" + ip + ":" + port + "...";
        
        inputIpHint_.visible = false;
        ipEdit_.visible = false;
        inputPortHint_.visible = false;
        portEdit_.visible = false;
        inputNicknameHint_.visible = false;
        nicknameEdit_.visible = false;
        connectButton_.visible = false;
    }
// ***

// *** Switch to Ingame state on successfull connection
    protected void SwitchToIngameState ()
    {
        Dispose ();
        Ingame @ingameState = Ingame ();
        ingameState.Configure (scene_);
        ingameState.Setup ();
        currentGameState = ingameState;
    }
// ***
};
