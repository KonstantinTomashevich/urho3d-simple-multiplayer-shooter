#include "BuildConfiguration.hpp"
#include "PlayersManager.hpp"

#include <Shared/Constants.hpp>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>

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
            if (players_.Values ().At (index).GetName () == resultingName)
                isFindedPlayerWithSameName = true;

        isUniqueNameGenerated = !isFindedPlayerWithSameName;
    }
    return resultingName;
}

void PlayersManager::ProcessNameRequest (Urho3D::Connection *connection, Urho3D::VectorBuffer &data)
{
    Urho3D::String requestedName = data.ReadString ();
    Urho3D::String resultingName = CreateUniqueName (requestedName);

    PlayerState *playerState = &(players_ [Urho3D::StringHash (connection->ToString ())]);
    playerState->SetName (resultingName);

    Urho3D::VectorBuffer messageData;
    messageData.WriteString (resultingName);
    playerState->GetConnection ()->SendMessage (NetworkMessageIds::STC_PLAYER_NAME_SETTED, true, false, messageData);
}

PlayersManager::PlayersManager (Urho3D::Context *context) : Urho3D::Object (context), players_ (), scene_ (0)
{

}

PlayersManager::~PlayersManager ()
{
    Reset ();
}

Urho3D::HashMap<Urho3D::StringHash, PlayerState> *PlayersManager::GetPlayers ()
{
    return &players_;
}

void PlayersManager::Setup (Urho3D::Scene *scene)
{
    assert (scene);
    SubscribeToEvent (Urho3D::E_CLIENTCONNECTED, URHO3D_HANDLER (PlayersManager, OnClientConnected));
    SubscribeToEvent (Urho3D::E_CLIENTDISCONNECTED, URHO3D_HANDLER (PlayersManager, OnClientDisconnected));
    SubscribeToEvent (Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER (PlayersManager, OnNetworkMessage));
    scene_ = scene;
}

void PlayersManager::Reset ()
{
    UnsubscribeFromAllEvents ();
    players_.Clear ();
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
    // TODO: Implement.
}

void PlayersManager::OnClientConnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection =
            (Urho3D::Connection *) eventData [Urho3D::ClientConnected::P_CONNECTION].GetPtr ();

    Urho3D::Log::Write (Urho3D::LOG_INFO, "New player connected. It's adress is " + connection->ToString () + ".");
    players_ [Urho3D::StringHash (connection->ToString ())] = PlayerState (this, connection);
    connection->SetScene (scene_);
}

void PlayersManager::OnClientDisconnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection =
            (Urho3D::Connection *) eventData [Urho3D::ClientDisconnected::P_CONNECTION].GetPtr ();

    PlayerState *playerState = &(players_ [Urho3D::StringHash (connection->ToString ())]);
    if (playerState->GetNode ())
        playerState->GetNode ()->Remove ();

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
    // TODO: Implement all messages processing.
}

