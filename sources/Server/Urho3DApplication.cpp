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

void Urho3DApplication::GenerateScene ()
{
    scene_->CreateComponent <Urho3D::Octree> ();
    GenerateLight ();
    GenerateTerrain ();
    GenerateZone ();
    GenerateObstacles (Urho3D::Random (5, 25));
}

void Urho3DApplication::GenerateLight ()
{
    Urho3D::Node *lightNode = scene_->CreateChild ("light", Urho3D::REPLICATED);
    lightNode->SetRotation (Urho3D::Quaternion (45.0f, 25.0f, 0.0f));
    Urho3D::Light *light = lightNode->CreateComponent <Urho3D::Light> (Urho3D::REPLICATED);
    light->SetLightType (Urho3D::LIGHT_DIRECTIONAL);
    light->SetCastShadows (true);
    light->SetBrightness (0.65f);
    light->SetColor (Urho3D::Color (0.35f, 0.35f, 0.65f));
}

void Urho3DApplication::GenerateTerrain ()
{
    Urho3D::Node *terrainNode = scene_->CreateChild ("terrain", Urho3D::REPLICATED);
    terrainNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_TERRAIN);
    Urho3D::Node *terrainLocal = terrainNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    terrainLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> ("Objects/terrain_local.xml")->GetRoot ());
}

void Urho3DApplication::GenerateZone ()
{
    Urho3D::Node *zoneNode = scene_->CreateChild ("zone", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    zoneNode->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> ("Objects/zone_for_server.xml")->GetRoot ());
}

float Urho3DApplication::GetNearestLength (Urho3D::Vector3 position, Urho3D::PODVector <Urho3D::Vector3> &others)
{
    if (others.Empty ())
        return -1.0f;

    float nearestLength = (others.At (0) - position).Length ();
    for (int index = 1; index < others.Size (); index++)
    {
        float length = (others.At (index) - position).Length ();
        if (length < nearestLength)
            nearestLength = length;
    }
    return nearestLength;
}

void Urho3DApplication::GenerateObstacles (int count)
{
    Urho3D::PODVector <Urho3D::Vector3> placed;
    for (int index = 0; index < count; index++)
    {
        Urho3D::Vector3 position;
        float nearestLength;
        do
        {
            position = Urho3D::Vector3 (Urho3D::Random (-50.0f, 50.0f), 2.5f, Urho3D::Random (-50.0f, 50.0f));
            nearestLength = GetNearestLength (position, placed);
        }
        while (nearestLength < 8.0f && nearestLength > 0.0f);
        placed.Push (position);
        GenerateObstacle (position);
    }
}

void Urho3DApplication::GenerateObstacle (Urho3D::Vector3 position)
{
    Urho3D::Node *obstacleNode = scene_->CreateChild ("obstacle", Urho3D::REPLICATED);
    obstacleNode->SetPosition (position);
    obstacleNode->SetRotation (Urho3D::Quaternion (0, Urho3D::Random (0.0f, 360.0f), 0));
    obstacleNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_OBSTACLE);
    Urho3D::Node *obstacleLocal = obstacleNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    obstacleLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> ("Objects/obstacle_local.xml")->GetRoot ());
}

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

    // Generate scene
    Urho3D::SetRandomSeed (Urho3D::Time::GetTimeSinceEpoch ());
    scene_ = new Urho3D::Scene (context_);
    GenerateScene ();

    // Setup camera manager
    cameraManager_ = new ServerCameraManager (context_);
    cameraManager_->Setup (scene_);

    // Setup players manager
    playersManager_ = new PlayersManager (context_);
    playersManager_->Setup (scene_);

    // Start server
    GetSubsystem <Urho3D::Network> ()->StartServer (ServerConstants::PORT);
}

void Urho3DApplication::Stop ()
{
    GetSubsystem <Urho3D::Network> ()->StopServer ();
    delete cameraManager_;
    delete playersManager_;
    scene_->RemoveAllChildren ();
    delete scene_;
}
