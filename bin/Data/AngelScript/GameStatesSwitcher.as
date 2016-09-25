// *** Interface for game state
interface GameStateAdapter
{
    void Setup ();
    void Update (float timeStep);
    void Dispose ();
    void HandleEvent (StringHash eventType, VariantMap &eventData);
}
// ***

// *** Namespace for GameStatesSwitcher functions and globals.
namespace GameStatesSwitcher
{
    // *** Globals section
    GameStateAdapter @gameState;
    // ***

    void SetupState (GameStateAdapter @newState)
    {
        DisposeCurrentState ();
        gameState = newState;
        if (gameState !is null)
            gameState.Setup ();
    }
    
    void DisposeCurrentState ()
    {
        if (gameState !is null)
            gameState.Dispose ();
        gameState = null;
    }
    
    void UpdateState (float timeStep)
    {
        if (gameState !is null)
            gameState.Update (timeStep);
    }
    
    void SendEventToCurrentState (StringHash eventType, VariantMap &eventData)
    {
        if (gameState !is null)
            gameState.HandleEvent (eventType, eventData);
    }
}
// ***
