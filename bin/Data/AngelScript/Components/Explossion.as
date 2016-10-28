class Explossion : ScriptObject
{
    Explossion ()
    {
        
    }
    
    ~Explossion ()
    {
        
    }
    
    void Start ()
    {
        SubscribeToEvent ("ParticleEffectFinished", "HandleParticleEffectFinished");
    }
    
    void HandleParticleEffectFinished (StringHash eventType, VariantMap &eventData)
    {
        Node @effectNode = eventData ["Node"].GetPtr ();
        if (effectNode.parent !is null)
            effectNode.parent.Remove ();
        else
            effectNode.Remove ();
    }
};
