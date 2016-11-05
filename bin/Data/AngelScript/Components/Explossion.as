class Explossion : ScriptObject
{
    protected float timeUntilRemove_;
    
    Explossion ()
    {
        
    }
    
    ~Explossion ()
    {
        
    }
    
    void Start ()
    {
        ParticleEmitter @emitter = node.GetComponent ("ParticleEmitter");
        ParticleEffect @effect = emitter.effect;
        timeUntilRemove_ = (effect.activeTime + effect.inactiveTime + effect.maxTimeToLive) * 1.5f;
    }
    
    void Update (float timeStep)
    {
        timeUntilRemove_ -= timeStep;
        if (timeUntilRemove_ <= 0.0f)
            node.parent.Remove ();
    }
    
    void Stop ()
    {
        
    }
};
