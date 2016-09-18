#pragma once
#include <Urho3D/Engine/Application.h>

class Urho3DApplication : public Urho3D::Application
{
URHO3D_OBJECT (Urho3DApplication, Application)
public:
    Urho3DApplication (Urho3D::Context *context);
    virtual ~Urho3DApplication ();

    virtual void Setup ();
    virtual void Start ();
    virtual void Stop ();
};
