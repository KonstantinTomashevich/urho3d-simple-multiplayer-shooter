#pragma once
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include "PlayersManager.hpp"
#include "Spawner.hpp"

class Urho3DApplication : public Urho3D::Application
{
URHO3D_OBJECT (Urho3DApplication, Application)
protected:
    Urho3D::Scene *scene_;
    PlayersManager *playersManager_;
    Spawner *spawner_;
public:
    Urho3DApplication (Urho3D::Context *context);
    virtual ~Urho3DApplication ();

    virtual void Setup ();
    virtual void Start ();
    virtual void Stop ();
};
