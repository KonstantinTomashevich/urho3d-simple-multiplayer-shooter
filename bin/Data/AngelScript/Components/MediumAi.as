class Ai : ScriptObject
{
    protected Vector2 normalizedMoveRequest_;
    protected bool tryToFireInNextFrame_;
    protected float timeFromLastScan_;
    protected float timeFromLastShotRaycasting_;
    
    protected void GoRightToNearestEnemyOrWaitIfNoEnemies ()
    {
        Array <Node @> sceneNodes = scene.GetChildren (true);
        Vector3 nearestVisibleEnemy = Vector3 (999, 999, 999);
        
        // Find position of nearest enemy.
        for (int index = 0; index < sceneNodes.length; index++)
        {
            Node @processingNode = sceneNodes [index];
            if (processingNode.id < FIRST_LOCAL_ID and
                processingNode.vars ["ObjectType"].GetInt () ==
                SerializationConstants__OBJECT_TYPE_PLAYER and
                (processingNode.position - node.position).length < 40.0f)
            {
                Vector3 localPosition = node.WorldToLocal (processingNode.worldPosition);
                if (localPosition.z > 0.0f and 
                    localPosition.length < nearestVisibleEnemy.length)
                    nearestVisibleEnemy = localPosition;
            }
        }
            
        if (nearestVisibleEnemy.length > 50.0f)
            // Wait for enemies.
            normalizedMoveRequest_ = Vector2 (1.0f, 1.0f);
        else
        {
            // Try to go to the nearest enemy.
            if (nearestVisibleEnemy.z > 30.0f)
                normalizedMoveRequest_.x = 1.0f;
            else if (nearestVisibleEnemy.z > 10.0f)
                normalizedMoveRequest_.x = 0.5f;
            else
                normalizedMoveRequest_.x = 0.0f;
                
            if (nearestVisibleEnemy.x > 35.0f)
                normalizedMoveRequest_.y = 0.25f;
            else if (nearestVisibleEnemy.z > 15.0f)
                normalizedMoveRequest_.y = 0.5f;
            else if (nearestVisibleEnemy.z > 0.0f) 
                normalizedMoveRequest_.y = 1.0f;
                    
            if (nearestVisibleEnemy.x < -35.0f)
                normalizedMoveRequest_.y = -0.25f;
            else if (nearestVisibleEnemy.z < -15.0f)
                normalizedMoveRequest_.y = -0.5f;
            else if (nearestVisibleEnemy.z < -0.0f) 
                normalizedMoveRequest_.y = -1.0f;
        }
    }
    
    protected void ShotIfItMakesSense ()
    {
        Ray ray;
        ray.origin = node.LocalToWorld (Vector3 (0, 0, 1));
        ray.direction = node.worldRotation * Vector3 (0, 0, 1);
        
        PhysicsRaycastResult result = 
        physicsWorld.RaycastSingle (ray, 40.0f);
            
        if (result.body !is null and 
            result.body.node.vars ["ObjectType"].GetInt () ==
            SerializationConstants__OBJECT_TYPE_PLAYER and
            not result.body.node.HasTag ("Died"))
                
            tryToFireInNextFrame_ = true; 
        else
            tryToFireInNextFrame_ = false;   
        
        timeFromLastShotRaycasting_ = 0.0f;
    }
    
    Ai ()
    {
        
    }
    
    ~Ai ()
    {
        
    }
    
    void Start ()
    {
        normalizedMoveRequest_ = Vector2 (0, 0);
        tryToFireInNextFrame_ = false;
        timeFromLastScan_ = 0.75f;
        timeFromLastShotRaycasting_ = 0.0f;
    }
    
    void Update (float timeStep)
    {
        timeFromLastScan_ += timeStep;
        timeFromLastShotRaycasting_ += timeStep;
        
        if (timeFromLastScan_ > 0.75f)
            GoRightToNearestEnemyOrWaitIfNoEnemies ();
        
        if (timeFromLastShotRaycasting_ >= 0.125f)
            ShotIfItMakesSense ();
    }
    
    void Stop ()
    {
        
    }
    
    void WriteAiCommands (AiCommands @output)
    {
        output.normalizedMoveRequest = normalizedMoveRequest_;
        output.tryToFireInNextFrame = tryToFireInNextFrame_;
    }
};
