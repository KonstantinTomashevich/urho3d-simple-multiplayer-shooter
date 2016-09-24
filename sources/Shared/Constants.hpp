#pragma once
#include <Urho3D/Container/Str.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/AngelScript/Script.h>

void BindConstantsToAngelScript (Urho3D::Script *script);
namespace ServerConstants
{
const Urho3D::String DEFAULT_SERVER_SCENE ("Maps/basic.xml");
const float DEFAULT_CAMERA_MOVE_SPEED = 10.0f;
const float SPEED_UP_MODIFER = 3.0f;
const int SERVER_PORT = 6391;

const int KEY_SPEED_UP = Urho3D::KEY_SHIFT;
const int KEY_LEFT = Urho3D::KEY_A;
const int KEY_RIGHT = Urho3D::KEY_D;
const int KEY_FORWARD = Urho3D::KEY_W;
const int KEY_BACK = Urho3D::KEY_S;
const int KEY_UP = Urho3D::KEY_Q;
const int KEY_DOWN = Urho3D::KEY_E;

const int TIME_BEFORE_AUTOMATIC_NAMING = 1.0f;
const int RESPAWN_TIME = 10.0f;

const int HEALTH_REGENERATION = 3.0f;
const int MAX_HEALTH = 100.0f;
const int DEAD_PLAYERS_REMOVE_TIME = 10.0f;
const float FIRE_COOLDOWN_TIME = 1.0f;
}

namespace GameConstants
{
const Urho3D::StringHash HEALTH_VAR_HASH ("Health");
}

namespace NetworkMessageIds
{
enum NetworkMessageId
{
    // Server to client messages

    STC_PLAYER_NAME_SETTED = 101, // Data: nameString
    STC_PLAYER_SPAWNED = 102, // Data: nodeId (unsigned)
    STC_PLAYER_FIRES = 103, // Data: nodeId (unsigned), shellPosition (Vector3), shellInitialVelocity (Vector3), shellSpawnTime (long)
    STC_CHAT_MESSAGE = 104, // Data: message (String)
    STC_SERVER_MESSAGE = 105, // Data: message (String)

    // Client to server messages

    CTS_REQUEST_NAME = 201, // Data: nameString
    CTS_SET_MOVE_VELOCITY_REQUEST = 202, // Data: moveVelocity (float)
    CTS_SET_ROTATION_VELOCITY_REQUEST = 203, // Data: rotationVelocity (float)
    CTS_REQUEST_FIRE = 204, // Data: none
    CTS_REQUEST_CHAT_MESSAGE = 205 // Data: message (String)
};
}
