#include "Launcher.hpp"
#include <Urho3D/Input/Input.h>
URHO3D_DEFINE_APPLICATION_MAIN (Launcher)

Launcher::Launcher (Urho3D::Context *context) : Urho3D::Application (context)
{

}

Launcher::~Launcher ()
{

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

}

void Launcher::Stop ()
{

}
