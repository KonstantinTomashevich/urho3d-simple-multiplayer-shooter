#pragma once
#include <Urho3D/Container/Str.h>
#include <Urho3D/Input/Input.h>

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
}

namespace Events
{
namespace ServerEvents
{
/// Signalize for player that server created node for this player.
URHO3D_EVENT (E_PLAYER_SPAWNED, PlayerSpawned)
{
    /// int.
    URHO3D_PARAM (P_NODE_ID, NodeId);
}

/// Signalize all players that one of players fires.
URHO3D_EVENT (E_PLAYER_FIRES, PlayerFires)
{
    /// int.
    URHO3D_PARAM (P_PLAYER_NODE_ID, PlayerNodeId);
    /// Urho3D::Vector3.
    URHO3D_PARAM (P_SHELL_POSITION, ShellPosition);
    /// Urho3D::Vector3.
    URHO3D_PARAM (P_SHELL_INITIAL_VELOCITY, ShellInitialVelocity);
}
}

namespace PlayerEvents
{
URHO3D_EVENT (E_SET_MOVE_REQUEST, SetMoveRequest)
{
    // Urho3D::Vector2.
    URHO3D_PARAM (P_MOVE_VECTOR, MoveVector);
}
}
}
