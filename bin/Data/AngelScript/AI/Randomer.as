class Ai : ScriptObject
{
    protected Vector2 normalizedMoveRequest_;
    protected bool tryToFireInNextFrame_;
    protected float timeFromLastMoveRandomization_;
    protected float timeFromLastShotRaycasting_;
    
    protected void RandomizeMoveRequest ()
    {
        normalizedMoveRequest_.x += Random (-1.5f, 1.5f);
        normalizedMoveRequest_.y += Random (-1.5f, 1.5f);
            
        // Invert move request if it's bigger then allowed.
        if (normalizedMoveRequest_.x > 1.0f)
                normalizedMoveRequest_.x = 1.0f - normalizedMoveRequest_.x;
        if (normalizedMoveRequest_.x < -1.0f)
            normalizedMoveRequest_.x = -1.0f - normalizedMoveRequest_.x;
                
        if (normalizedMoveRequest_.y > 1.0f)
            normalizedMoveRequest_.y = 1.0f - normalizedMoveRequest_.y;
        if (normalizedMoveRequest_.y < -1.0f)
            normalizedMoveRequest_.y = -1.0f - normalizedMoveRequest_.y;
                
        // Truncate move request if it's still bigger then allowed.
        if (normalizedMoveRequest_.x > 1.0f)
            normalizedMoveRequest_.x = 1.0f;
        if (normalizedMoveRequest_.x < -1.0f)
            normalizedMoveRequest_.x = -1.0f;
                
        if (normalizedMoveRequest_.y > 1.0f)
            normalizedMoveRequest_.y = 1.0f;
        if (normalizedMoveRequest_.y < -1.0f)
            normalizedMoveRequest_.y = -1.0f;
                
        timeFromLastMoveRandomization_ = 0.0f;
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
        timeFromLastMoveRandomization_ = 2.0f;
        timeFromLastShotRaycasting_ = 0.0f;
    }
    
    void Update (float timeStep)
    {
        if (not node.HasTag ("Died"))
        {
            timeFromLastMoveRandomization_ += timeStep;
            timeFromLastShotRaycasting_ += timeStep;
            
            if (timeFromLastMoveRandomization_ >= 2.0f)
                RandomizeMoveRequest ();
            
            if (timeFromLastShotRaycasting_ >= 0.25f)
                ShotIfItMakesSense ();
        }
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
