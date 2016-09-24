#include "GameStatesSwitcher.as"
#include "SharedGlobals.as"
#include "LogInToServer/StateAdapter.as"
#include "Ingame/StateAdapter.as"

// *** Application cycle functions
void Start ()
{
    SharedGlobals::syncedGameScene = Scene ();
    SharedGlobals::lastAdress = "localhost";
    SharedGlobals::lastPort = ServerConstants__SERVER_PORT;
    SharedGlobals::lastNickname = "UnknownPlayer";
    GameStatesSwitcher::SetupState (LogInToServer::StateAdapter ());
}

void Update (float timeStep)
{
    GameStatesSwitcher::UpdateState (timeStep);
}

void Stop ()
{
    GameStatesSwitcher::DisposeCurrentState ();
}
// ***

// *** Events resender
void HandleEvent (StringHash eventType, VariantMap &eventData)
{
    GameStatesSwitcher::SendEventToCurrentState (eventType, eventData);
}
// ***
