class Ai : ScriptObject
{
    protected Vector2 normalizedMoveRequest_;
    protected bool tryToFireInNextFrame_;
    protected float timeFromLastScan_;
    protected float timeFromLastShotRaycasting_;
    protected float timeFromLastRunAwayCalculating_;
    protected float lastDecisionPoints_;
    protected Vector3 weakerEnemyPosition_;
    
    protected Array <Node @> @GetEnemiesNear ()
    {
        Array <Node @> sceneNodes = scene.GetChildren (true);
        Array <Node @> @enemiesNear = Array <Node @> ();
        
        for (int index = 0; index < sceneNodes.length; index++)
        {
            Node @processingNode = sceneNodes [index];
            if (processingNode.id < FIRST_LOCAL_ID and
                processingNode.id != node.id and
                processingNode.vars ["ObjectType"].GetInt () ==
                SerializationConstants__OBJECT_TYPE_PLAYER and
                not processingNode.HasTag ("Died") and
                (processingNode.position - node.position).length < 25.0f)
                
                enemiesNear.Push (processingNode);
        }
        return enemiesNear;
    }
    
    protected void RunAway ()
    {
        float randomNumber = Random ();            
        if (randomNumber < 0.5f)
            normalizedMoveRequest_.x = -1.0f;
        else
            normalizedMoveRequest_.x = 1.0f;
        
        randomNumber = Random ();            
        if (randomNumber < 1.0f / 3.0f)
            normalizedMoveRequest_.y = -1.0f;
        else if (randomNumber < 2.0f / 3.0f)
            normalizedMoveRequest_.y = 0.0f;
        else
            normalizedMoveRequest_.y = 1.0f;
    }
    
    protected void GoRightToPosition (Vector3 position)
    {
        if (position.z > 30.0f)
            normalizedMoveRequest_.x = 1.0f;
        else if (position.z > 10.0f)
            normalizedMoveRequest_.x = 0.5f;
        else
            normalizedMoveRequest_.x = 0.0f;
            
        if (position.x > 5.0f)
            normalizedMoveRequest_.y = 1.0f;
        else if (position.x < -5.0f) 
            normalizedMoveRequest_.y = -1.0f;
        else
            normalizedMoveRequest_.y = 0.0f;
    }
    
    protected float CalculateRunAwayDecisionPoints (Array <Node @> @enemiesNear)
    {        
        if (enemiesNear.empty)
            return 1.0f;
        
        float runAwayDecisionPoints = 0.0f;
        for (int index = 0; index < enemiesNear.length; index++)
        {
            Node @processingEnemy = enemiesNear [index];
            Ray ray;
            ray.origin = processingEnemy.LocalToWorld (Vector3 (0, 0, 1));
            ray.direction = processingEnemy.worldRotation * Vector3 (0, 0, 1);
            
            PhysicsRaycastResult result = 
                physicsWorld.RaycastSingle (ray, 30.0f);
                
            if (result.body !is null and 
                result.body.node.id == node.id)
            {
                float myHealth = node.vars ["Health"].GetFloat ();
                int enemyExp = processingEnemy.vars ["Exp"].GetInt ();
                runAwayDecisionPoints += 4.0f * ((GameplayConstants__BASIC_SHELL_DAMAGE * 
                                                   (1.0f + enemyExp * 1.0f * GameplayConstants__SHELL_DAMAGE_INCREASE_PER_EXP)
                                                    / myHealth));
            }
        }
        return runAwayDecisionPoints;
    }
    
    protected float CalculateGoRightToWeakerEnemyPoints (Array <Node @> @enemiesNear)
    {
        if (enemiesNear.empty)
            return -100.0f;
            
        Array <float> points;
        int myExp = node.vars ["Exp"].GetInt ();
                    
        for (int index = 0; index < enemiesNear.length; index++)
        {
            Node @processingEnemy = enemiesNear [index];
            float decisionPoints = 0.0f;
            decisionPoints += 1000.0f * (GameplayConstants__BASIC_SHELL_DAMAGE *
                                        (1.0f + GameplayConstants__SHELL_DAMAGE_INCREASE_PER_EXP) /
                                        processingEnemy.vars ["Health"].GetFloat ()) ;
              
            Vector3 localPosition = node.WorldToLocal (processingEnemy.worldPosition);
            points.Push (decisionPoints);
        }
        
        float maxPoints = 0.0f;
        int maxPointsIndex = -1;
        for (int index = 0; index < points.length; index++)
            if (points [index] > maxPoints)
            {
                maxPoints = points [index];
                maxPointsIndex = index;
            }
        
        weakerEnemyPosition_ = enemiesNear [maxPointsIndex].position;
        return maxPoints;
    }
    
    protected void ShotIfItMakesSense ()
    {
        Ray ray;
        ray.origin = node.LocalToWorld (Vector3 (0, 0, 1));
        ray.direction = node.worldRotation * Vector3 (0, 0, 1);
        
        PhysicsRaycastResult result = 
            physicsWorld.RaycastSingle (ray, 30.0f);
            
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
        timeFromLastScan_ = 0.4f;
        timeFromLastShotRaycasting_ = 1.0f;
        timeFromLastRunAwayCalculating_ = 1.0f;
    }
    
    void Update (float timeStep)
    {
        if (not node.HasTag ("Died"))
        {
            timeFromLastScan_ += timeStep;
            timeFromLastShotRaycasting_ += timeStep;
            timeFromLastRunAwayCalculating_ += timeStep;
            
            if (timeFromLastScan_ > 0.25f)
            {
                Array <Node @> @enemies = GetEnemiesNear ();
                if (lastDecisionPoints_ <= CalculateGoRightToWeakerEnemyPoints (enemies))
                    GoRightToPosition (weakerEnemyPosition_);
                timeFromLastScan_ = 0.0f;
            }
            
            if (timeFromLastRunAwayCalculating_ > 0.75f)
            {
                Array <Node @> @enemies = GetEnemiesNear ();
                float runAwayPoints = CalculateRunAwayDecisionPoints (enemies);
                if (runAwayPoints >= lastDecisionPoints_)
                    RunAway ();
                timeFromLastRunAwayCalculating_ = 0.0f;
            }
            
            if (timeFromLastShotRaycasting_ >= 0.05f)
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

