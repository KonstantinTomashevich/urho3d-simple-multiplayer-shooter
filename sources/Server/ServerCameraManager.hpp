#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

class ServerCameraManager : public Urho3D::Object
{
URHO3D_OBJECT (ServerCameraManager, Object)
protected:
    Urho3D::Scene *scene_;
    Urho3D::Node *camera_;
public:
    ServerCameraManager (Urho3D::Context *context);
    virtual ~ServerCameraManager ();

    void Setup (Urho3D::Scene *scene);
    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void Reset ();
};

