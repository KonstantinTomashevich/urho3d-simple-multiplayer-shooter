namespace Ingame
{
    class LocalSceneManager
    {
        protected Node @cameraNode_;
        
        protected void CreateCamera ()
        {
            cameraNode_ = SharedGlobals::syncedGameScene.CreateChild ("camera", LOCAL);
            cameraNode_.CreateComponent ("SoundListener", LOCAL);
            cameraNode_.CreateComponent ("Camera", LOCAL);
        };
        
        protected void CreateZone ()
        {
            Node @zoneNode = SharedGlobals::syncedGameScene.CreateChild ("zone", LOCAL);
            zoneNode.LoadXML (cast <XMLFile> (
                                              cache.GetResource ("XMLFile", "Objects/zone_for_players.xml")).
                                              GetRoot ());
        }
        
        protected void SetupViewport ()
        {
            Viewport @viewport = Viewport (SharedGlobals::syncedGameScene, cameraNode_.GetComponent ("Camera"));
            renderer.viewports [0] = viewport;
        }
        
        LocalSceneManager ()
        {
            
        }
        
        ~LocalSceneManager ()
        {
            
        }
        
        void Setup ()
        {
            for (int index = 0; index < SharedGlobals::syncedGameScene.GetChildren (true).length; index++)
            {
                Node @childNode = SharedGlobals::syncedGameScene.GetChildren (true) [index];
                log.Info ("Node [Id: " + childNode.id + "; Name: " + childNode.name + "]");
            }
            
            CreateZone ();
            CreateCamera ();
            SetupViewport ();
        }
        
        void Update (float timeStep)
        {
            cameraNode_.position = Vector3 (0, 20, 0);
            cameraNode_.rotation = Quaternion (90, 0, 0);
        }
        
        void Clear ()
        {
            
        }
    };
}
