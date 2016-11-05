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
        // TODO: I'm not sure that finished particle effects are removed.
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
