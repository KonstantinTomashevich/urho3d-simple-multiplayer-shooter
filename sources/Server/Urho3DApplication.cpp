#include "BuildConfiguration.hpp"
#include "Urho3DApplication.hpp"
#include <Shared/Constants.hpp>

#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>
URHO3D_DEFINE_APPLICATION_MAIN (Urho3DApplication)

Urho3DApplication::Urho3DApplication (Urho3D::Context *context) : Urho3D::Application (context),
    scene_ (0), cameraManager_ (0), playersManager_ (0), spawner_ (0)
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

    // Add spawner and generate scene.
    Urho3D::SetRandomSeed (Urho3D::Time::GetTimeSinceEpoch ());
    spawner_ = new Spawner (context_);
    context_->RegisterSubsystem (spawner_);

    scene_ = new Urho3D::Scene (context_);
    spawner_->SetScene (scene_);
    spawner_->GenerateServerScene ();

    // Setup camera manager
    cameraManager_ = new ServerCameraManager (context_);
    cameraManager_->Setup (scene_);
    context_->RegisterSubsystem (cameraManager_);

    // Setup players manager
    playersManager_ = new PlayersManager (context_);
    playersManager_->Setup (scene_);
    context_->RegisterSubsystem (playersManager_);

    // Start server
    GetSubsystem <Urho3D::Network> ()->StartServer (ServerConstants::PORT);
}

void Urho3DApplication::Stop ()
{
    GetSubsystem <Urho3D::Network> ()->StopServer ();
    delete cameraManager_;
    delete playersManager_;
    delete spawner_;
    scene_->RemoveAllChildren ();
    delete scene_;
}
