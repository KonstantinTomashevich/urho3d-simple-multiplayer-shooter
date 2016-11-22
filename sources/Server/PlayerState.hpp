#pragma once
#include <Urho3D/Container/Str.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Network/Connection.h>

class PlayersManager;
class Spawner;
class PlayerState
{
protected:
    PlayersManager *manager_;
    Urho3D::Connection *connection_;
    float timeFromLastFire_;

    Urho3D::String name_;
    float timeBeforeAutomaticNaming_;

    Urho3D::Node *node_;
    float timeBeforeSpawn_;
    Urho3D::Vector2 normalizedMoveRequest_;

    int deaths_;
    int kills_;
    bool isAi_;
public:
    PlayerState (PlayersManager *manager, Urho3D::Connection *connection);
    virtual ~PlayerState ();

    virtual void Update (float timeStep);
    void TryToFire ();
    Urho3D::Connection *GetConnection ();

    float GetTimeFromLastFire ();
    void SetTimeFromLastFire (float timeFromLastFire);
    float GetTimeBeforeSpawn ();

    Urho3D::String GetName ();
    void SetName (Urho3D::String name);

    Urho3D::Node *GetNode ();
    void SetNode (Urho3D::Node *node);

    Urho3D::Vector2 GetNormalizedMoveRequest ();
    void SetNormalizedMoveRequest (Urho3D::Vector2 normalizedMoveRequest);

    int GetKills ();
    void IncrementKills ();

    int GetDeaths ();
    void IncrementDeaths ();
    void ClearStatistics ();

    float GetShellDamage ();
    // Returns true if player survived, otherwise false.
    bool ApplyDamage (float damage);
    virtual Urho3D::String GetScriptPath ();
};
