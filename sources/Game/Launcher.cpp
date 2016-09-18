#include "Launcher.hpp"
#include <Urho3D/Input/Input.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
URHO3D_DEFINE_APPLICATION_MAIN (Launcher)

Launcher::Launcher (Urho3D::Context *context) : Urho3D::Application (context), scriptMain_ (0)
{

}

Launcher::~Launcher ()
{
    scriptObject_->Release ();
    delete scriptMain_;
}

void Launcher::Setup ()
{
    engineParameters_ ["FullScreen"] = false;
    engineParameters_ ["WindowResizable"] = true;
    engineParameters_ ["LogName"] = "Game.log";
    engineParameters_ ["WindowTitle"] = "Urho3D Simple Multiplayer Shooter Game";
}

void Launcher::Start ()
{
    // Set mouse to free mode
    Urho3D::Input *input = GetSubsystem <Urho3D::Input> ();
    input->SetMouseVisible (true);
    input->SetMouseMode (Urho3D::MM_FREE);

    // Register AngelScript subsystem.
    context_->RegisterSubsystem (new Urho3D::Script (context_));

    // Load main script file
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    scriptMain_ = resourceCache->GetResource <Urho3D::ScriptFile> ("AngelScript/GameApplication.as");

    // Init script object and call Start ()
    scriptObject_ = scriptMain_->CreateObject ("GameApplication");
    scriptMain_->Execute (scriptObject_, "void Start ()");

    // Subscribe to update.
    SubscribeToEvent (Urho3D::E_UPDATE, URHO3D_HANDLER (Launcher, Update));
}

void Launcher::Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::VariantVector parameters;
    parameters.Push (eventData [Urho3D::Update::P_TIMESTEP]);
    scriptMain_->Execute (scriptObject_, "void Update (float timeStep)", parameters);
}

void Launcher::Stop ()
{
    scriptMain_->Execute (scriptObject_, "void Stop ()");
}
