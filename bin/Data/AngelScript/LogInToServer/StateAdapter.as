#include "LogInToServer/StateUi.as"
namespace LogInToServer
{
    // *** Game State globals
    StateUi @stateUi = StateUi ();
    bool isConnectingNow = false;
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
            SubscribeToEvent ("UIMouseClickEnd", "HandleEvent");
            SubscribeToEvent ("ConnectFailed", "HandleEvent");
            SubscribeToEvent ("NetworkSceneLoadFailed", "HandleEvent");
            stateUi.Setup ();
        }
        
        void Update (float timeStep)
        {
            stateUi.Update (timeStep);
            if (network.serverConnection !is null and network.serverConnection.sceneLoaded
                and SharedGlobals::syncedGameScene.GetChildren ().length > 0)
                HandleSuccessfullConnection ();
        }
        
        void Dispose ()
        {
            stateUi.Clear ();
            UnsubscribeFromAllEvents ();
        }
        
        void HandleEvent (StringHash eventType, VariantMap &eventData)
        {
            if (eventType == StringHash ("UIMouseClickEnd") and 
                stateUi.GetButtonId (eventData ["Element"].GetPtr ()) == CONNECT_BUTTON)
                HandleConnectButtonClick (eventData);
                
            else if (eventType == StringHash ("ConnectFailed"))
                HandleConnectFailed (eventData);
                
            else if (eventType == StringHash ("NetworkSceneLoadFailed"))
                HandleSceneLoadingError (eventData);
        }

        // *** UI events handling functions
        protected void HandleConnectButtonClick (VariantMap &eventData)
        {
            String nickname = stateUi.nickname;
            nickname.Replace ('!', '_');
            nickname.Replace ('(', '_');
            nickname.Replace (')', '_');
            nickname.Replace ('[', '_');
            nickname.Replace (']', '_');
            nickname.Replace ('\\', '_');
            nickname.Replace ('/', '_');
            nickname.Replace ('{', '_');
            nickname.Replace ('}', '_');
            nickname.Replace ('*', '_');
            nickname.Replace ('<', '_');
            nickname.Replace ('>', '_');
            
            SharedGlobals::syncedGameScene.RemoveAllChildren ();
            network.Connect (stateUi.adress, stateUi.port, SharedGlobals::syncedGameScene);
            isConnectingNow = true;
            stateUi.SetConnectionSetupFieldsVisible (false);
            
            SharedGlobals::lastAdress = stateUi.adress;
            SharedGlobals::lastPort = stateUi.port;
            SharedGlobals::lastNickname = nickname;
        }
        // ***

        // *** Network events handling functions
        void HandleConnectFailed (VariantMap &eventData)
        {
            ErrorDialog ("Connect failed!", "Connection to " + stateUi.adress + ":" + 
                         stateUi.port + " failed!");
            engine.Exit ();
        }
        
        void HandleSuccessfullConnection ()
        {
            GameStatesSwitcher::SetupState (Ingame::StateAdapter ());
        }
        
        void HandleSceneLoadingError (VariantMap &eventData)
        {
            ErrorDialog ("Error loading scene!", "Connection to " + stateUi.adress + ":" +
                          stateUi.port + " failed!\n" + "Can't load server scene!");
            engine.Exit ();
        }
        // ***
    };
}
