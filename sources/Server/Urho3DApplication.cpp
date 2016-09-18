#include "Urho3DApplication.hpp"
#include <Urho3D/Input/Input.h>
URHO3D_DEFINE_APPLICATION_MAIN (Urho3DApplication)

Urho3DApplication::Urho3DApplication (Urho3D::Context *context) : Urho3D::Application (context)
{

}

Urho3DApplication::~Urho3DApplication ()
{

}

void Urho3DApplication::Setup ()
{
    engineParameters_ ["FullScreen"] = false;
    engineParameters_ ["WindowResizable"] = true;
    engineParameters_ ["LogName"] = "Server.log";
    engineParameters_ ["WindowTitle"] = "Urho3D Simple Multiplayer Shooter Server";
    engineParameters_ ["WindowWidth"] = 800;
    engineParameters_ ["WindowHeight"] = 600;
}

void Urho3DApplication::Start ()
{
    Urho3D::Input *input = GetSubsystem <Urho3D::Input> ();
    input->SetMouseVisible (true);
    input->SetMouseMode (Urho3D::MM_FREE);
}

void Urho3DApplication::Stop ()
{

}
