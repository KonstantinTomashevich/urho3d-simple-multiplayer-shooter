#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

class CameraManager : public Urho3D::Object
{
URHO3D_OBJECT (CameraManager, Object)
protected:
    Urho3D::Node *camera_;
public:
    CameraManager (Urho3D::Context *context);
    virtual ~CameraManager ();

    void Setup ();
    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void Reset ();
};

