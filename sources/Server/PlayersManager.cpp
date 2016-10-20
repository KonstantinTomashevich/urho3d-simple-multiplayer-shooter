#include "BuildConfiguration.hpp"
#include "PlayersManager.hpp"
#include "Spawner.hpp"

#include <Shared/Constants.hpp>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Context.h>

Urho3D::String PlayersManager::CreateUniqueName (Urho3D::String requestedName)
{
    int addition = -1;
    Urho3D::String resultingName;
    bool isUniqueNameGenerated = false;

    while (!isUniqueNameGenerated)
    {
        addition ++;
        if (addition > 0)
            resultingName = requestedName + "_" + Urho3D::String (addition);
        else
            resultingName = requestedName;

        bool isFindedPlayerWithSameName = false;
        for (int index = 0; index < players_.Values ().Size (); index++)
            if (players_.Values ().At (index)->GetName () == resultingName)
                isFindedPlayerWithSameName = true;

        isUniqueNameGenerated = !isFindedPlayerWithSameName;
    }
    return resultingName;
}

void PlayersManager::ProcessNameRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data)
{
    Urho3D::String requestedName = data.ReadString ();
    Urho3D::String resultingName = CreateUniqueName (requestedName);

    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    playerState->SetName (resultingName);

    Urho3D::VectorBuffer messageData;
    messageData.WriteString (resultingName);
    playerState->GetConnection ()->SendMessage (NetworkMessageIds::STC_PLAYER_NAME_SETTED, true, false, messageData);
}

void PlayersManager::ProcessGetTimeUntilSpawn (Urho3D::Connection *connection)
{
    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    float timeBeforeSpawn = playerState->GetTimeBeforeSpawn ();
    Urho3D::VectorBuffer messageData;
    messageData.WriteFloat (timeBeforeSpawn);
    playerState->GetConnection ()->SendMessage (NetworkMessageIds::STC_RETURN_TIME_UNTIL_SPAWN, true, false, messageData);
}

void PlayersManager::ProcessChatMessageRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data)
{
    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    Urho3D::VectorBuffer messageData;
    messageData.WriteString (data.ReadString ());
    messageData.WriteString (playerState->GetName ());

    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *player = players_.Values ().At (index);
        if (player)
            player->GetConnection ()->SendMessage (NetworkMessageIds::STC_CHAT_MESSAGE, true, false, messageData);
    }
}

void PlayersManager::ProcessSetMoveRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data)
{
    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    Urho3D::Vector2 moveRequest = data.ReadVector2 ();
    playerState->SetNormalizedMoveRequest (moveRequest);
}

void PlayersManager::SendServerMessage (Urho3D::String message)
{
    Urho3D::VectorBuffer messageData;
    messageData.WriteString (message);
    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *player = players_.Values ().At (index);
        if (player)
            player->GetConnection ()->SendMessage (NetworkMessageIds::STC_SERVER_MESSAGE, true, false, messageData);
    }
}

PlayersManager::PlayersManager (Urho3D::Context *context) :
    Urho3D::Object (context),
    players_ (),
    scene_ (0)
{

}

PlayersManager::~PlayersManager ()
{
    Reset ();
}

Urho3D::HashMap <Urho3D::StringHash, PlayerState *> *PlayersManager::GetPlayers ()
{
    return &players_;
}

void PlayersManager::Setup (Urho3D::Scene *scene)
{
    assert (scene);
    SubscribeToEvent (Urho3D::E_CLIENTCONNECTED, URHO3D_HANDLER (PlayersManager, OnClientConnected));
    SubscribeToEvent (Urho3D::E_CLIENTDISCONNECTED, URHO3D_HANDLER (PlayersManager, OnClientDisconnected));
    SubscribeToEvent (Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER (PlayersManager, OnNetworkMessage));
    SubscribeToEvent (Urho3D::E_UPDATE, URHO3D_HANDLER (PlayersManager, Update));
    scene_ = scene;
}

void PlayersManager::Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    float timeStep = eventData [Urho3D::Update::P_TIMESTEP].GetFloat ();
    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *playerState = players_.Values ().At (index);
        playerState->Update (timeStep);
    }
}

void PlayersManager::Reset ()
{
    UnsubscribeFromAllEvents ();
    while (!players_.Empty ())
    {
        delete players_.Front ().second_;
        players_.Erase (players_.Front ().first_);
    }
    scene_ = 0;
}

void PlayersManager::RequestName (PlayerState *requester)
{
    assert (requester->GetName () == Urho3D::String::EMPTY);
    Urho3D::String requestedName = "UnnamedPlayer_" + Urho3D::String (requester->GetConnection ()->GetPort ());
    Urho3D::String resultingName = CreateUniqueName (requestedName);
    requester->SetName (resultingName);

    Urho3D::VectorBuffer messageData;
    messageData.WriteString (resultingName);
    requester->GetConnection ()->SendMessage (NetworkMessageIds::STC_PLAYER_NAME_SETTED, true, false, messageData);
}

void PlayersManager::RequestRespawn (PlayerState *requester)
{
    assert (!requester->GetNode ());
    assert (requester->GetTimeBeforeSpawn () <= 0);

    Spawner *spawner = context_->GetSubsystem <Spawner> ();
    unsigned id = spawner->SpawnPlayer ();
    requester->SetNode (scene_->GetNode (id));

    Urho3D::VectorBuffer messageData;
    messageData.WriteUInt (id);
    requester->GetConnection ()->SendMessage (NetworkMessageIds::STC_PLAYER_SPAWNED, true, false, messageData);
    SendServerMessage (requester->GetName () + " respawned!");
}

void PlayersManager::OnClientConnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection =
            (Urho3D::Connection *) eventData [Urho3D::ClientConnected::P_CONNECTION].GetPtr ();

    players_ [Urho3D::StringHash (connection->ToString ())] = new PlayerState (this, connection);
    connection->SetScene (scene_);
}

void PlayersManager::OnClientDisconnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection =
            (Urho3D::Connection *) eventData [Urho3D::ClientDisconnected::P_CONNECTION].GetPtr ();

    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    SendServerMessage (playerState->GetName () + " exited!");
    if (playerState->GetNode ())
        playerState->GetNode ()->Remove ();

    delete playerState;
    players_.Erase (Urho3D::StringHash (connection->ToString ()));
}

void PlayersManager::OnNetworkMessage (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection =
            (Urho3D::Connection *) eventData [Urho3D::NetworkMessage::P_CONNECTION].GetPtr ();
    int messageId = eventData [Urho3D::NetworkMessage::P_MESSAGEID].GetInt ();
    Urho3D::VectorBuffer data (eventData [Urho3D::NetworkMessage::P_DATA].GetBuffer ());
    data.Seek (0);

    if (messageId == NetworkMessageIds::CTS_REQUEST_NAME)
        ProcessNameRequest (connection, data);

    else if (messageId == NetworkMessageIds::CTS_GET_TIME_UNTIL_SPAWN)
        ProcessGetTimeUntilSpawn (connection);

    else if (messageId == NetworkMessageIds::CTS_REQUEST_CHAT_MESSAGE)
        ProcessChatMessageRequest (connection, data);

    else if (messageId == NetworkMessageIds::CTS_SET_MOVE_REQUEST)
        ProcessSetMoveRequest (connection, data);

    // TODO: Implement all messages processing.
}

