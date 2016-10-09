#pragma once
#include <Urho3D/Core/Object.h>
#include "PlayerState.hpp"

class PlayersManager : public Urho3D::Object
{
URHO3D_OBJECT (PlayersManager, Object)
protected:
    Urho3D::Scene *scene_;
    Urho3D::HashMap <Urho3D::StringHash, PlayerState *> players_;

    Urho3D::String CreateUniqueName (Urho3D::String requestedName);
    void ProcessNameRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data);
    void ProcessGetTimeUntilSpawn (Urho3D::Connection *connection);
public:
    PlayersManager (Urho3D::Context *context);
    virtual ~PlayersManager ();

    Urho3D::HashMap<Urho3D::StringHash, PlayerState *> *GetPlayers();
    void Setup (Urho3D::Scene *scene);
    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void Reset ();

    void RequestName (PlayerState *requester);
    void RequestRespawn (PlayerState *requester);

    void OnClientConnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnClientDisconnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnNetworkMessage (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
};
