namespace Ingame
{
    class LocalSceneManager
    {
        protected Node @cameraNode_;
        protected Node @playerNode_;
        
        protected void CreateLocals ()
        {
            Array <Node @> nodes = SharedGlobals::syncedGameScene.GetChildren (true);
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
            
            else if (nodeLocalType == SerializationConstants__OBJECT_TYPE_PLAYER)
            {
                localNode.LoadXML ((cast <XMLFile> (cache.GetResource ("XMLFile",
                                                                       SceneConstants__PLAYER_LOCAL_PREFAB))).
                                   GetRoot ());
                // Physics will be calculated on server
                localNode.RemoveComponents ("RigidBody");
                localNode.RemoveComponents ("CollsionShape");
            }
            
            // TODO: Implement shell.
            
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
            
            cameraNode_.position = Vector3 (0, 40, 0);
            cameraNode_.rotation = Quaternion (90, 0, 0);
        }
        
        void Update (float timeStep)
        {           
            // Creates locals for new objects (old have "local" child and will not be affected).
            CreateLocals ();
            
            if (playerNode_ !is null)
            {
                cameraNode_.position = playerNode_.LocalToWorld (Vector3 (0, 4, -8));
                cameraNode_.LookAt (playerNode_.position);
            }
            
            if (playerNode_ !is null and playerLives < 0.0f)
            {
                playerNode_ = null;
                stateUi.isSpawned_ = true;
            }
        }
        
        void Clear ()
        {
            
        }
        
        Node @get_playerNode ()
        {
            return playerNode_;
        }
        
        uint get_playerNodeId ()
        {
            if (playerNode_ !is null)
                return playerNode_.id;
            else
                return 0;
        }
        
        void set_playerNodeId (uint id)
        {            
            Array <Node @>nodes = SharedGlobals::syncedGameScene.GetChildren (true);
            for (int index = 0; index < nodes.length; index++)
            {
                Node @scanningNode = nodes [index];
                if (scanningNode.id == id)
                {
                    playerNode_ = scanningNode;
                    return;
                }
            }
        }
        
        float get_playerLives ()
        {
            if (playerNode_ !is null)
                return playerNode_.vars [SerializationConstants__HEALTH_VAR_HASH].GetFloat ();
            else
                return -1;
        }
    };
}
