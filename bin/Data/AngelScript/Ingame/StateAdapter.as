#include "Ingame/LocalSceneManager.as"
#include "Ingame/NetworkHandler.as"
#include "Ingame/StateUi.as"

namespace Ingame
{    
    // *** Game State globals
    LocalSceneManager @localSceneManager = LocalSceneManager ();
    NetworkHandler @networkHandler = NetworkHandler ();
    StateUi @stateUi = StateUi ();
    // ***
    
    class StateAdapter : GameStateAdapter
    {        
        StateAdapter ()
        {

        }
        
        ~StateAdapter ()
        {
            
        }
        
        void Setup ()
        {
            SubscribeToEvent ("NetworkMessage", "HandleEvent");
            stateUi.Setup ();
            localSceneManager.Setup ();
            networkHandler.SendNickname ();
        }
        
        void Update (float timeStep)
        {
            localSceneManager.Update (timeStep);
            stateUi.Update (timeStep);
        }
        
        void Dispose ()
        {
            stateUi.Clear ();
            localSceneManager.Clear ();
            UnsubscribeFromAllEvents ();
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            networkHandler.HandleEvent (eventType, eventData);
        }
    };
}
