#pragma once
#include <Urho3D/Engine/Application.h>
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <AngelScript/angelscript.h>

class Launcher : public Urho3D::Application
{
URHO3D_OBJECT (Launcher, Application)
protected:
    Urho3D::ScriptFile *scriptMain_;
public:
    Launcher (Urho3D::Context *context);
    virtual ~Launcher ();

    virtual void Setup ();
    virtual void Start ();
    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    virtual void Stop ();
};
