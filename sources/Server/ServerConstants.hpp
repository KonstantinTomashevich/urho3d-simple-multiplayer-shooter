#pragma once
#include <Urho3D/Container/Str.h>
#include <Urho3D/Input/Input.h>

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
