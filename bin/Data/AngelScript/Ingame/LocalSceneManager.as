namespace Ingame
{
    class LocalSceneManager
    {
        protected Node @cameraNode_;
        protected Node @playerNode_;
        protected Array <Node @> otherPlayersNodes_;
        
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
            Camera @camera = cameraNode_.CreateComponent ("Camera", LOCAL);
            camera.farClip = 75.0f;
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
        
        protected void ScanForOtherPlayers ()
        {
            otherPlayersNodes_.Clear ();
            Array <Node @> nodes = SharedGlobals::syncedGameScene.GetChildren (true);
            
            for (int index = 0; index < nodes.length; index++)
            {
                Node @otherPlayerNode = nodes [index];
                // Is it node of any player? Isn't it node of our player?
                if (otherPlayerNode.id < FIRST_LOCAL_ID and 
                    otherPlayerNode.vars [SerializationConstants__OBJECT_TYPE_VAR_HASH].GetInt () == 
                    SerializationConstants__OBJECT_TYPE_PLAYER and
                    otherPlayerNode !is playerNode_)
                {
                    Camera @camera = cameraNode_.GetComponent ("Camera");
                    // It's not too far?
                    if ((otherPlayerNode.position - cameraNode_.position).length < camera.farClip)
                    {
                        Vector2 screenPosition = camera.WorldToScreenPoint (otherPlayerNode.position);
                        // Is it inside screen?
                        if (screenPosition.x > 0.0f and screenPosition.x < 1.0f and
                            screenPosition.y > 0.0f and screenPosition.y < 1.0f)
                        {
                            Octree @octree = SharedGlobals::syncedGameScene.GetComponent ("Octree");
                            Ray ray = camera.GetScreenRay (screenPosition.x, screenPosition.y);
                            RayQueryResult result = octree.RaycastSingle (ray, RAY_TRIANGLE,
                                                                          camera.farClip,
                                                                          DRAWABLE_GEOMETRY);
                            // Is other player visible?
                            if (result.node.parent.parent is otherPlayerNode)
                                otherPlayersNodes_.Push (otherPlayerNode);
                        }
                    }
                }
            }
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
            ScanForOtherPlayers ();
            
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
        
        Node @get_cameraNode ()
        {
            return cameraNode_;
        }
        
        Array <Node @> get_otherPlayersNodes ()
        {
            return otherPlayersNodes_;
        }
    };
}
