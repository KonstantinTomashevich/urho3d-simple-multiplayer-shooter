#include "BuildConfiguration.hpp"
#include "PlayersManager.hpp"
#include "Spawner.hpp"
#include "AiPlayerState.hpp"

#include <Shared/Constants.hpp>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Math/MathDefs.h>

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

    if (playerState->GetConnection ())
    {
        Urho3D::VectorBuffer messageData;
        messageData.WriteString (resultingName);
        playerState->GetConnection ()->SendMessage (NetworkMessageIds::STC_PLAYER_NAME_SETTED, true, false, messageData);
    }
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
    Urho3D::String message = data.ReadString ();
    messageData.WriteString (message);
    messageData.WriteString (playerState->GetName ());

    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *player = players_.Values ().At (index);
        if (player && player->GetConnection ())
            player->GetConnection ()->SendMessage (NetworkMessageIds::STC_CHAT_MESSAGE, true, false, messageData);
    }

    Urho3D::VariantMap newChatMessageEventData;
    newChatMessageEventData [Urho3D::StringHash ("Sender")] = playerState->GetName ();
    newChatMessageEventData [Urho3D::StringHash ("Message")] = message;
    SendEvent (Urho3D::StringHash ("NewChatMessage"), newChatMessageEventData);
}

void PlayersManager::ProcessSetMoveRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data)
{
    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    Urho3D::Vector2 moveRequest = data.ReadVector2 ();
    playerState->SetNormalizedMoveRequest (moveRequest);
}

void PlayersManager::ProcessFireRequest (Urho3D::Connection *connection)
{
    PlayerState *playerState = players_ [Urho3D::StringHash (connection->ToString ())];
    playerState->TryToFire ();
}

void PlayersManager::SendServerMessage (Urho3D::String message)
{
    Urho3D::VectorBuffer messageData;
    messageData.WriteString (message);
    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *player = players_.Values ().At (index);
        if (player && player->GetConnection ())
            player->GetConnection ()->SendMessage (NetworkMessageIds::STC_SERVER_MESSAGE, true, false, messageData);
    }

    Urho3D::VariantMap newServerMessageEventData;
    newServerMessageEventData [Urho3D::StringHash ("Message")] = message;
    SendEvent (Urho3D::StringHash ("NewServerMessage"), newServerMessageEventData);
}

PlayerState *PlayersManager::GetPlayerByName (Urho3D::String name)
{
    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *player = players_.Values ().At (index);
        if (player && player->GetName () == name)
            return player;
    }
    return 0;
}

void PlayersManager::UpdateAllPlayers (float timeStep)
{
    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *playerState = players_.Values ().At (index);
        playerState->Update (timeStep);
    }
}

void PlayersManager::RemoveDiedPlayersNodes (float timeStep)
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::PODVector <Urho3D::Node *> nodes;
    scene->GetChildren (nodes, true);

    for (int index = 0; index < nodes.Size (); index++)
    {
        Urho3D::Node *node = nodes.At (index);
        if (node->GetID () < Urho3D::FIRST_LOCAL_ID &&
                node->GetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH).GetInt () ==
                SerializationConstants::OBJECT_TYPE_PLAYER &&
                node->GetVar (SerializationConstants::HEALTH_VAR_HASH).GetFloat () < 0.0f)
        {
            if (node->HasTag ("Died"))
            {
                float timeBeforeRemove = node->GetVar (Urho3D::StringHash ("timeBeforeRemove")).GetFloat ();
                timeBeforeRemove -= timeStep;
                if (timeBeforeRemove < 0)
                    node->Remove ();
                else
                    node->SetVar (Urho3D::StringHash ("timeBeforeRemove"), timeBeforeRemove);
            }
            else
            {
                node->AddTag ("Died");
                node->SetVar (Urho3D::StringHash ("timeBeforeRemove"), GameplayConstants::DEAD_PLAYERS_REMOVE_TIME);
            }
        }
    }
}

void PlayersManager::KillPlayersIfTheyOutIfLevelBounds ()
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::PODVector <Urho3D::Node *> nodes;
    scene->GetChildren (nodes, true);

    for (int index = 0; index < nodes.Size (); index++)
    {
        Urho3D::Node *node = nodes.At (index);
        if (node->GetID () < Urho3D::FIRST_LOCAL_ID &&
                node->GetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH).GetInt () ==
                SerializationConstants::OBJECT_TYPE_PLAYER &&
                (node->GetWorldPosition ().y_ > 100.0f || node->GetWorldPosition ().y_ < -100.0f))
            node->SetVar (SerializationConstants::HEALTH_VAR_HASH, Urho3D::Variant (-1.0f));
    }
}

void PlayersManager::RecalculateLeaderboard ()
{
    // TODO: What about other points system? Example:
    // Points contained in PlayerState.
    // Each sucessfull attack (shot) gives +10 points.
    // Each kill gives +10 points (in addition to sucessfull shot points).
    // Each death gives -30 points.

    Urho3D::HashMap <HashableFloat, PlayerState *> playersPoints;
    for (int index = 0; index < players_.Values ().Size (); index++)
    {
        PlayerState *player = players_.Values ().At (index);
        float points = -player->GetDeaths ();

        if (player->GetDeaths () > 0)
            points += 500.0f * (player->GetKills () * 1.0f / (player->GetDeaths () + 1.0f));
        else
            points += 500.0f * (player->GetKills () / 0.7f);

        if (player->GetNode ())
            points += 25.0f * player->GetNode ()->GetVar (SerializationConstants::EXP_VAR_HASH).GetInt ();

        points += Urho3D::Random ();
        playersPoints [points] = player;
    }

    playersPoints.Sort ();
    Urho3D::VariantVector leaderboardData;
    // Iterate from end of map because numbers in hash map are sorted from smallest to biggest.
    for (int index = playersPoints.Values ().Size () - 1; index >= 0; index--)
    {
        PlayerState *player = playersPoints.Values ().At (index);
        float points = playersPoints.Keys ().At (index).value_;

        Urho3D::String informationString;
        informationString += player->GetName () + ";";
        informationString += Urho3D::String (player->GetKills ()) + ";";
        informationString += Urho3D::String (player->GetDeaths ()) + ";";
        if (player->GetNode ())
            informationString += Urho3D::String (player->GetNode ()->GetVar (SerializationConstants::EXP_VAR_HASH).GetInt ()) + ";";
        else
            informationString += "0;";
        informationString += Urho3D::String (floor (points)) + ";";
        leaderboardData.Push (informationString);
    }

    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    scene->SetVar (Urho3D::StringHash ("Leaderboard"), leaderboardData);
}

PlayersManager::PlayersManager (Urho3D::Context *context) :
    Urho3D::Object (context),
    players_ ()
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

void PlayersManager::Setup ()
{
    SubscribeToEvent (Urho3D::E_CLIENTCONNECTED, URHO3D_HANDLER (PlayersManager, OnClientConnected));
    SubscribeToEvent (Urho3D::E_CLIENTDISCONNECTED, URHO3D_HANDLER (PlayersManager, OnClientDisconnected));
    SubscribeToEvent (Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER (PlayersManager, OnNetworkMessage));
    SubscribeToEvent (Urho3D::E_UPDATE, URHO3D_HANDLER (PlayersManager, Update));
    SubscribeToEvent (Urho3D::StringHash ("PlayerShooted"), URHO3D_HANDLER (PlayersManager, OnPlayerShooted));
    SubscribeToEvent (Urho3D::StringHash ("RequestServerMessage"), URHO3D_HANDLER (PlayersManager, OnServerMessageRequest));
    SubscribeToEvent (Urho3D::StringHash ("CreateAiPlayer"), URHO3D_HANDLER (PlayersManager, OnCreateAiPlayerRequest));
    SubscribeToEvent (Urho3D::StringHash ("KickPlayer"), URHO3D_HANDLER (PlayersManager, OnKickPlayerRequest));
}

void PlayersManager::Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    float timeStep = eventData [Urho3D::Update::P_TIMESTEP].GetFloat ();
    UpdateAllPlayers (timeStep);
    RemoveDiedPlayersNodes (timeStep);
    KillPlayersIfTheyOutIfLevelBounds ();
    RecalculateLeaderboard ();
}

void PlayersManager::Reset ()
{
    UnsubscribeFromAllEvents ();
    while (!players_.Empty ())
    {
        delete players_.Front ().second_;
        players_.Erase (players_.Front ().first_);
    }
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

void PlayersManager::RequestRespawn (PlayerState *requester, bool isAi)
{
    assert (!requester->GetNode ());
    assert (requester->GetTimeBeforeSpawn () <= 0);

    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Spawner *spawner = context_->GetSubsystem <Spawner> ();
    unsigned id = spawner->SpawnPlayer (isAi, requester->GetScriptPath ());
    requester->SetNode (scene->GetNode (id));

    Urho3D::VectorBuffer messageData;
    messageData.WriteUInt (id);
    if (requester->GetConnection ())
        requester->GetConnection ()->SendMessage (NetworkMessageIds::STC_PLAYER_SPAWNED, true, false, messageData);
    SendServerMessage (requester->GetName () + " respawned!");
}

void PlayersManager::OnClientConnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection =
            (Urho3D::Connection *) eventData [Urho3D::ClientConnected::P_CONNECTION].GetPtr ();

    players_ [Urho3D::StringHash (connection->ToString ())] = new PlayerState (this, connection);
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    connection->SetScene (scene);
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

    else if (messageId == NetworkMessageIds::CTS_SET_MOVE_REQUEST)
        ProcessSetMoveRequest (connection, data);

    else if (messageId == NetworkMessageIds::CTS_REQUEST_FIRE)
        ProcessFireRequest (connection);

    else if (messageId == NetworkMessageIds::CTS_REQUEST_CHAT_MESSAGE)
        ProcessChatMessageRequest (connection, data);

    else if (messageId == NetworkMessageIds::CTS_GET_TIME_UNTIL_SPAWN)
        ProcessGetTimeUntilSpawn (connection);
}

void PlayersManager::OnPlayerShooted (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    PlayerState *attacker = GetPlayerByName (
                eventData [Urho3D::StringHash ("AttackerPlayerName")].GetString ());
    PlayerState *damaged = GetPlayerByName (
                eventData [Urho3D::StringHash ("DamagedPlayerName")].GetString ());

    assert (damaged);
    if (attacker && !damaged->ApplyDamage (attacker->GetShellDamage ()))
    {
        attacker->IncrementKills ();
        SendServerMessage (damaged->GetName () + " killed by " + attacker->GetName () + "!");
    }
}

void PlayersManager::OnServerMessageRequest (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::String message = eventData [Urho3D::StringHash ("Message")].GetString ();
    SendServerMessage (message);
}

void PlayersManager::OnKickPlayerRequest (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    // TODO: This event will be defended from hack.
    Urho3D::String name = eventData [Urho3D::StringHash ("Name")].GetString ();
    PlayerState *player = GetPlayerByName (name);
    if (player->GetConnection ())
        player->GetConnection ()->Disconnect ();
    else
    {
        SendServerMessage (player->GetName () + " (AI) exited!");
        if (player->GetNode ())
            player->GetNode ()->Remove ();

        delete player;
        players_.Erase (Urho3D::StringHash (name));
    }
}

void PlayersManager::OnCreateAiPlayerRequest (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::String aiScriptPath = eventData [Urho3D::StringHash ("AiScriptPath")].GetString ();
    Urho3D::String requestedName = eventData [Urho3D::StringHash ("Name")].GetString ();
    AiPlayerState *playerState = new AiPlayerState (this, aiScriptPath);

    Urho3D::String resultingName = CreateUniqueName (requestedName);
    playerState->SetName (resultingName);
    players_ [Urho3D::StringHash (resultingName)] = playerState;
}


HashableFloat::HashableFloat (float value) : value_ (value)
{

}

HashableFloat::~HashableFloat ()
{

}

unsigned HashableFloat::ToHash () const
{
    return ((unsigned) floor (value_ * 1000));
}

bool HashableFloat::operator > (const HashableFloat &other) const
{
    return value_ > other.value_;
}

bool HashableFloat::operator < (const HashableFloat &other) const
{
    return value_ < other.value_;
}

bool HashableFloat::operator == (const HashableFloat &other) const
{
    return value_ == other.value_;
}

bool HashableFloat::operator != (const HashableFloat &other) const
{
    return value_ != other.value_;
}
