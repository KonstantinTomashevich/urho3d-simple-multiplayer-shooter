class Ingame : GameState
{
// *** Variables for scene handling
    Scene @scene_;
// ***

// *** Variables for UI
    protected IntVector2 lastScreenSize;
// ***

// *** UI managing functions
    protected void SetupUi ()
    {

    }
    
    protected void ResizeUi (int width, int height)
    {

    }
// ***

// *** Constructor and destructor
    Ingame ()
    {
        
    }
    
    ~Ingame ()
    {
        UnsubscribeFromAllEvents ();
    }
// ***

// *** Set the server scene which downloaded while logging into server.
    void Configure (Scene @serverScene)
    {
        scene_ = serverScene;
    }
// ***

// *** Setups local camera and screen viewport
    void SetupScene ()
    {
        
    }
// ***
    
// *** State lifecycle functions
    void Setup ()
    {
        SetupScene ();
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
    }
    
    void Dispose ()
    {
        ui.root.RemoveAllChildren ();
    }
// ***
    
// *** Main events handling function
    void HandleEvent (StringHash eventType, VariantMap &eventData)
    {
        
    }
// ***

// *** UI events handling functions
// ***

// *** Keyboard handling function
// ***

// *** Network events handlers
// ***

// *** Network messages senders
// ***
};
