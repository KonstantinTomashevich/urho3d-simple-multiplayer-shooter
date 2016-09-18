class GameApplication : ScriptObject
{
    GameApplication ()
    {
        log.Info ("Application constructed!");
    }
    ~GameApplication () 
    {
        
    }
    
    void Start ()
    {
        log.Info ("Application started!");
    }
    
    void Update (float timeStep)
    {
        log.Info ("Application updated!");
    }
    
    void Stop ()
    {
        log.Info ("Application stopped!");
    }
};
