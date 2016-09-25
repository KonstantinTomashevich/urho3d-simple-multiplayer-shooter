#include "Ingame/LocalSceneManager.as"

namespace Ingame
{    
    // *** Game State globals
    LocalSceneManager @localSceneManager = LocalSceneManager ();
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
            localSceneManager.Setup ();
        }
        
        void Update (float timeStep)
        {
            localSceneManager.Update (timeStep);
        }
        
        void Dispose ()
        {
            localSceneManager.Clear ();
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            
        }
    };
}
