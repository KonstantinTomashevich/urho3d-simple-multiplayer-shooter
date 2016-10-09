namespace Ingame
{
    class LocalSceneManager
    {
        protected Node @cameraNode_;
        
        protected void CreateLocals ()
        {
            Array <Node@> nodes;
            nodes = SharedGlobals::syncedGameScene.GetChildren (true);
            for (int index = 0; index < nodes.length; index++)
            {
                Node @childNode = nodes [index];
                if (childNode.id < FIRST_LOCAL_ID and childNode.GetChild ("local") is null)
                    CreateLocalFor (childNode);
            }
        }
        
        protected void CreateLocalFor (Node @replicatedNode)
        {
            int nodeLocalType = replicatedNode.vars [SerializationConstants__OBJECT_TYPE_VAR_HASH].GetInt ();
            Node @localNode = replicatedNode.CreateChild ("local", LOCAL);
            
            if (nodeLocalType == SerializationConstants__OBJECT_TYPE_TERRAIN)
            {
                localNode.LoadXML ((cast <XMLFile> (cache.GetResource ("XMLFile",
                                                                       SceneConstants__TERRAIN_LOCAL_PREFAB))).
                                   GetRoot ());
                // Physics will be calculated on server
                localNode.RemoveComponents ("RigidBody");
                localNode.RemoveComponents ("CollsionShape");
            }
            
            else if (nodeLocalType == SerializationConstants__OBJECT_TYPE_OBSTACLE)
            {
                localNode.LoadXML ((cast <XMLFile> (cache.GetResource ("XMLFile",
                                                                       SceneConstants__OBSTACLE_LOCAL_PREFAB))).
                                   GetRoot ());
                // Physics will be calculated on server
                localNode.RemoveComponents ("RigidBody");
                localNode.RemoveComponents ("CollsionShape");
            }
            // TODO: Implement shell and player.
            
            localNode.name = "local";
        }
        
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
            CreateLocals ();
            CreateZone ();
            CreateCamera ();
            SetupViewport ();
        }
        
        void Update (float timeStep)
        {
            // This is temporary. 
            // TODO: Add normal camera handling after adding players spawn.
            cameraNode_.position = Vector3 (0, 40, 0);
            cameraNode_.rotation = Quaternion (90, 0, 0);
            
            // Creates locals for new objects (old have "local" child and will not be affected).
            CreateLocals ();
        }
        
        void Clear ()
        {
            
        }
    };
}
