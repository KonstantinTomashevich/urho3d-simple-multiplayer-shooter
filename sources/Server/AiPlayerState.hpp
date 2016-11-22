#pragma once
#include "PlayerState.hpp"
#include <Urho3D/AngelScript/Script.h>

class AiCommands : public Urho3D::Object
{
URHO3D_OBJECT (AiCommands, Object)
public:
    AiCommands (Urho3D::Context *context);
    virtual ~AiCommands ();

    Urho3D::Vector2 normalizedMoveRequest_;
    bool tryToFireInNextFrame_;

    static void BindToAngelScript (Urho3D::Script *script);
};

class AiPlayerState : public PlayerState
{
protected:
    AiCommands *aiCommands_;
    Urho3D::String scriptPath_;
public:
    AiPlayerState (PlayersManager *manager, Urho3D::String aiScriptPath);
    virtual ~AiPlayerState ();
    virtual void Update (float timeStep);
    virtual Urho3D::String GetScriptPath ();
};
