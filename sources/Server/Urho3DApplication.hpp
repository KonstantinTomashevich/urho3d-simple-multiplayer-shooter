#pragma once
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include "ServerCameraManager.hpp"

class Urho3DApplication : public Urho3D::Application
{
URHO3D_OBJECT (Urho3DApplication, Application)
protected:
    Urho3D::Scene *scene_;
    ServerCameraManager *cameraManager_;
public:
    Urho3DApplication (Urho3D::Context *context);
    virtual ~Urho3DApplication ();

    virtual void Setup ();
    virtual void Start ();
    virtual void Stop ();
};
