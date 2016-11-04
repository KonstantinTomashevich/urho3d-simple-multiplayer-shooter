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
public:
    AiPlayerState (PlayersManager *manager, int aiType);
    virtual ~AiPlayerState ();
    virtual void Update (float timeStep);
};
