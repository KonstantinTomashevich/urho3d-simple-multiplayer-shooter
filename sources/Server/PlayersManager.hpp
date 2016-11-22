#pragma once
#include <Urho3D/Core/Object.h>
#include "PlayerState.hpp"

class HashableFloat
{
public:
    float value_;

    HashableFloat (float value = 0.0f);
    virtual ~HashableFloat ();

    unsigned ToHash () const;
    bool operator > (const HashableFloat &other) const;
    bool operator < (const HashableFloat &other) const;
    bool operator == (const HashableFloat &other) const;
    bool operator != (const HashableFloat &other) const;
};

class PlayersManager : public Urho3D::Object
{
URHO3D_OBJECT (PlayersManager, Object)
protected:
    Urho3D::HashMap <Urho3D::StringHash, PlayerState *> players_;

    Urho3D::String CreateUniqueName (Urho3D::String requestedName);
    void ProcessNameRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data);
    void ProcessGetTimeUntilSpawn (Urho3D::Connection *connection);

    void ProcessChatMessageRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data);
    void ProcessSetMoveRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data);
    void ProcessFireRequest (Urho3D::Connection *connection);

    void SendServerMessage (Urho3D::String message);
    PlayerState *GetPlayerByName (Urho3D::String name);

    void UpdateAllPlayers (float timeStep);
    void RemoveDiedPlayersNodes (float timeStep);
    void KillPlayersIfTheyOutIfLevelBounds ();
    void RecalculateLeaderboard ();
public:
    PlayersManager (Urho3D::Context *context);
    virtual ~PlayersManager ();

    Urho3D::HashMap <Urho3D::StringHash, PlayerState *> *GetPlayers();
    void Setup ();
    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void Reset ();

    void RequestName (PlayerState *requester);
    void RequestRespawn (PlayerState *requester, bool isAi);

    void OnClientConnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnClientDisconnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnNetworkMessage (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnPlayerShooted (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnServerMessageRequest (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnKickPlayerRequest (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void OnCreateAiPlayerRequest (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
};
