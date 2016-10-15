#include "Ingame/LocalSceneManager.as"
#include "Ingame/NetworkHandler.as"
#include "Ingame/StateUi.as"
#include "Ingame/KeyboardListener.as"
#include "Ingame/UiListener.as"

namespace Ingame
{    
    // *** Game State globals
    LocalSceneManager @localSceneManager = LocalSceneManager ();
    NetworkHandler @networkHandler = NetworkHandler ();
    StateUi @stateUi = StateUi ();
    KeyboardListener @keyboardListener = KeyboardListener ();
    UiListener @uiListener = UiListener ();
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
            SubscribeToEvent ("UIMouseClickEnd", "HandleEvent");
            
            stateUi.Setup ();
            localSceneManager.Setup ();
            networkHandler.SendNickname ();
        }
        
        void Update (float timeStep)
        {
            localSceneManager.Update (timeStep);
            stateUi.Update (timeStep);
            keyboardListener.Update (timeStep);
        }
        
        void Dispose ()
        {
            stateUi.Clear ();
            localSceneManager.Clear ();
            UnsubscribeFromAllEvents ();
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            if (eventType == StringHash ("NetworkMessage"))
                networkHandler.HandleEvent (eventType, eventData);
            else if (eventType == StringHash ("UIMouseClickEnd"))
                uiListener.HandleEvent (eventType, eventData);
        }
    };
}
