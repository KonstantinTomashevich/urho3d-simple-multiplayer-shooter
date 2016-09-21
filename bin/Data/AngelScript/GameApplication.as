// *** Interface which will be implemented by our game states.
interface GameState
{
    void Setup ();
    void Update (float timeStep);
    void Dispose ();
    void HandleEvent (StringHash eventType, VariantMap &eventData);
}
// ***

#include "LogInToServer/LogInToServer.as"
#include "Ingame/Ingame.as"

// *** Global vartiables
GameState @currentGameState;
// ***

// *** Application cycle functions
void Start ()
{
    currentGameState = LogInToServer ();
    currentGameState.Setup ();
}

void Update (float timeStep)
{
    currentGameState.Update (timeStep);
}

void Stop ()
{
    currentGameState.Dispose ();
}
// ***

// *** Events resender
void HandleEvent (StringHash eventType, VariantMap &eventData)
{
    currentGameState.HandleEvent (eventType, eventData);
}
// ***
