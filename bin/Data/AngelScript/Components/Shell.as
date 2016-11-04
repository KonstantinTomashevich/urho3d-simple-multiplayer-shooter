class Shell : ScriptObject
{
    Shell ()
    {
        
    }
    
    ~Shell ()
    {
        
    }
    
    void Start ()
    {
        SubscribeToEvent ("PhysicsCollisionStart", "HandlePhysicsCollisionStart");
    }
    
    void Stop ()
    {
        
    }
    
    void HandlePhysicsCollisionStart (StringHash eventType, VariantMap &eventData)
    {
        Node @nodeA = eventData ["NodeA"].GetPtr ();
        Node @nodeB = eventData ["NodeB"].GetPtr ();
        
        if (nodeA is node.parent or nodeB is node.parent)
        {
            VariantMap explossionEventData;
            explossionEventData ["Position"] = node.parent.position;
            SendEvent ("Explossion", explossionEventData);
            
            Node @otherNode;
            if (nodeA is node)
                otherNode = nodeB;
            else
                otherNode = nodeA;
            
            if (otherNode !is null and 
                otherNode.vars ["ObjectType"].GetInt () == 
                SerializationConstants__OBJECT_TYPE_PLAYER and
                not otherNode.HasTag ("Died"))
            {
                VariantMap playerShootedEventData;
                playerShootedEventData ["AttackerPlayerName"] = 
                    node.parent.vars ["Name"].GetString ();
                playerShootedEventData ["DamagedPlayerName"] =
                    otherNode.vars ["Name"].GetString ();
                SendEvent ("PlayerShooted", playerShootedEventData);
            }
            node.parent.Remove ();
        }
    }
}
