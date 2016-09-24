#include "BuildConfiguration.hpp"
#include "Urho3DApplication.hpp"
#include <Shared/Constants.hpp>

#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>

#include <Urho3D/Graphics/Octree.h>
URHO3D_DEFINE_APPLICATION_MAIN (Urho3DApplication)

Urho3DApplication::Urho3DApplication (Urho3D::Context *context) : Urho3D::Application (context),
    scene_ (0), cameraManager_ (0), playersManager_ (0)
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
    // Set mouse to free mode
    Urho3D::Input *input = GetSubsystem <Urho3D::Input> ();
    input->SetMouseVisible (true);
    input->SetMouseMode (Urho3D::MM_FREE);

    // Load default scene
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    scene_ = new Urho3D::Scene (context_);
    scene_->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (ServerConstants::DEFAULT_SERVER_SCENE)->GetRoot ());

    // Setup camera manager
    cameraManager_ = new ServerCameraManager (context_);
    cameraManager_->Setup (scene_);

    // Setup players manager
    playersManager_ = new PlayersManager (context_);
    playersManager_->Setup (scene_);

    // Start server
    GetSubsystem <Urho3D::Network> ()->StartServer (ServerConstants::SERVER_PORT);
}

void Urho3DApplication::Stop ()
{
    GetSubsystem <Urho3D::Network> ()->StopServer ();
    delete cameraManager_;
    delete playersManager_;
    scene_->RemoveAllChildren ();
    delete scene_;
}
