#pragma once
#include <Urho3D/Engine/Application.h>

class Launcher : public Urho3D::Application
{
URHO3D_OBJECT (Launcher, Application)
public:
    Launcher (Urho3D::Context *context);
    virtual ~Launcher ();

    virtual void Setup ();
    virtual void Start ();
    virtual void Stop ();
};
