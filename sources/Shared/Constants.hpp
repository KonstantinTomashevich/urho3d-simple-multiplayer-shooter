#pragma once
#include <Urho3D/Container/Str.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/AngelScript/Script.h>

void BindConstantsToAngelScript (Urho3D::Script *script);
namespace ServerConstants
{
const int PORT = 6391;
const float TIME_BEFORE_AUTOMATIC_NAMING = 1.0f;
const float MINIMUM_DISTANCE_BETWEEN_OBSTACLES = 10.0f;
const int MINIMUM_OBSTACLES_COUNT = 5;
const int MAXIMUM_OBSTACLES_COUNT = 25;
}

namespace GameplayConstants
{
const float RESPAWN_TIME = 20.0f;
const float HEALTH_REGENERATION = 2.5f;
const float BASIC_MAX_HEALTH = 100.0f;
const float DEAD_PLAYERS_REMOVE_TIME = 10.0f;
const float FIRE_COOLDOWN_TIME = 1.5f;

const float MOVE_IMPULSE = 50.0f;
const float ROTATION_IMPULSE = 2.0f;
const Urho3D::Vector3 SHELL_LINEAR_VELOCITY (0.0f, 3.0f, 40.0f);

const float MAX_HEALTH_INCREASE_PER_EXP = 0.05f;
const float SHELL_DAMAGE_INCREASE_PER_EXP = 0.1f;
const int MAX_EXP = 20;
const float BASIC_SHELL_DAMAGE = 30.0f;
}

namespace SerializationConstants
{
const Urho3D::StringHash HEALTH_VAR_HASH ("Health");
const Urho3D::StringHash NAME_VAR_HASH ("Name");
const Urho3D::StringHash OBJECT_TYPE_VAR_HASH ("ObjectType");
const Urho3D::StringHash EXP_VAR_HASH ("Exp");

const int OBJECT_TYPE_WITHOUT_LOCALS = -1;
const int OBJECT_TYPE_TERRAIN = 0;
const int OBJECT_TYPE_OBSTACLE = 1;
const int OBJECT_TYPE_PLAYER = 2;
const int OBJECT_TYPE_SHELL = 3;
const int OBJECT_TYPE_EXPLOSSION = 4;
}

namespace SceneConstants
{
const Urho3D::String TERRAIN_LOCAL_PREFAB ("Objects/terrain_local.xml");
const Urho3D::String OBSTACLE_LOCAL_PREFAB ("Objects/obstacle_local.xml");
const Urho3D::String PLAYER_LOCAL_PREFAB ("Objects/player_local.xml");
const Urho3D::String SHELL_LOCAL_PREFAB ("Objects/shell_local.xml");
const Urho3D::String EXPLOSSION_LOCAL_PREFAB ("Objects/explossion_local.xml");
}

namespace NetworkMessageIds
{
enum NetworkMessageId
{
    // Server to client messages

    STC_PLAYER_NAME_SETTED = 153, // Data: nameString
    STC_PLAYER_SPAWNED, // Data: nodeId (unsigned)
    STC_CHAT_MESSAGE, // Data: senderName (String), message (String)
    STC_SERVER_MESSAGE, // Data: message (String)
    STC_RETURN_TIME_UNTIL_SPAWN, // Data: time (float)

    // Client to server messages

    CTS_REQUEST_NAME, // Data: nameString
    CTS_SET_MOVE_REQUEST, // Data: normalizedMoveRequest (Vector2)
    CTS_REQUEST_FIRE, // Data: none
    CTS_REQUEST_CHAT_MESSAGE, // Data: message (String)
    CTS_GET_TIME_UNTIL_SPAWN // Data: empty
};
}
