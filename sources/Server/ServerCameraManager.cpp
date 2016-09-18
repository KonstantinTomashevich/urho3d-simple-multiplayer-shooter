#include "BuildConfiguration.hpp"
#include "ServerCameraManager.hpp"
#include <Shared/Constants.hpp>

#include <Urho3D/Input/Input.h>
#include <Urho3D/Core/CoreEvents.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>

#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>

ServerCameraManager::ServerCameraManager (Urho3D::Context *context) : Urho3D::Object (context)
{

}

ServerCameraManager::~ServerCameraManager ()
{
    Reset ();
}

void ServerCameraManager::Setup (Urho3D::Scene *scene)
{
    assert (scene);
    scene_ = scene;
    camera_ = scene_->CreateChild ("server_local_camera", Urho3D::LOCAL);
    camera_->SetPosition (Urho3D::Vector3 (0, 50, 0));
    camera_->SetRotation (Urho3D::Quaternion (90, 0, 0));

    Urho3D::Camera *cameraComponent = camera_->CreateComponent <Urho3D::Camera> (Urho3D::LOCAL);
    cameraComponent->SetFarClip (300.0);
    cameraComponent->SetAutoAspectRatio (true);

    Urho3D::Renderer *renderer = GetSubsystem <Urho3D::Renderer> ();
    Urho3D::SharedPtr <Urho3D::Viewport> viewport (
                new Urho3D::Viewport (context_, scene_, cameraComponent));
    renderer->SetViewport (0, viewport);
    renderer->SetShadowMapSize (1024);

    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    Urho3D::Node *zoneNode = scene_->CreateChild ("zone", Urho3D::LOCAL);
    zoneNode->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> ("Objects/zone_for_server.xml")->GetRoot ());
    SubscribeToEvent (Urho3D::E_UPDATE, URHO3D_HANDLER (ServerCameraManager, Update));
}

void ServerCameraManager::Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    assert (camera_);
    float timestep = eventData [Urho3D::Update::P_TIMESTEP].GetFloat ();
    Urho3D::Input *input = GetSubsystem <Urho3D::Input> ();

    Urho3D::Vector3 move;
    if (input->GetKeyDown (ServerConstants::KEY_LEFT))
        move.x_ -= 1;

    if (input->GetKeyDown (ServerConstants::KEY_RIGHT))
        move.x_ += 1;

    if (input->GetKeyDown (ServerConstants::KEY_FORWARD))
        move.z_ += 1;

    if (input->GetKeyDown (ServerConstants::KEY_BACK))
        move.z_ -= 1;

    if (input->GetKeyDown (ServerConstants::KEY_UP))
        move.y_ += 1;

    if (input->GetKeyDown (ServerConstants::KEY_DOWN))
        move.y_ -= 1;

    if (input->GetKeyDown (ServerConstants::KEY_SPEED_UP))
        move *= ServerConstants::SPEED_UP_MODIFER;

    move *= ServerConstants::DEFAULT_CAMERA_MOVE_SPEED;
    move *= timestep;
    camera_->SetPosition (camera_->GetPosition () + move);
}

void ServerCameraManager::Reset ()
{
    scene_ = 0;
    camera_ = 0;
    UnsubscribeFromAllEvents ();
}

