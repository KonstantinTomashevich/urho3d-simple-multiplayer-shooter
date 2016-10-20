#include "BuildConfiguration.hpp"
#include "Spawner.hpp"
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Physics/PhysicsWorld.h>

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/IO/Log.h>
#include <Shared/Constants.hpp>

Spawner::Spawner (Urho3D::Context *context) :
    Urho3D::Object (context),
    scene_ (0)
{

}

Spawner::~Spawner ()
{

}

void Spawner::SetScene (Urho3D::Scene *scene)
{
    scene_ = scene;
}

Urho3D::Scene *Spawner::GetScene ()
{
    return scene_;
}

Urho3D::PODVector <Urho3D::Vector3> *Spawner::GetPlacedObstaclesVectorPointer()
{
    return &placedObstacles_;
}

void Spawner::GenerateServerScene ()
{
    scene_->CreateComponent <Urho3D::Octree> ();
    scene_->CreateComponent <Urho3D::PhysicsWorld> ();
    AddStandartLight ();
    AddStandartZone ();
    AddStandartTerrain ();
    AddStandartTerrainBorders ();
    GenerateObstacles (Urho3D::Random (
                           ServerConstants::MINIMUM_OBSTACLES_COUNT, ServerConstants::MAXIMUM_OBSTACLES_COUNT));
}

void Spawner::AddStandartLight ()
{
    Urho3D::Node *lightNode = scene_->CreateChild ("light", Urho3D::REPLICATED);
    lightNode->SetRotation (Urho3D::Quaternion (45.0f, 25.0f, 0.0f));
    lightNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_WITHOUT_LOCALS);

    Urho3D::Light *light = lightNode->CreateComponent <Urho3D::Light> (Urho3D::REPLICATED);
    light->SetLightType (Urho3D::LIGHT_DIRECTIONAL);
    light->SetCastShadows (true);
    light->SetBrightness (0.65f);
    light->SetColor (Urho3D::Color (0.35f, 0.35f, 0.65f));
}

void Spawner::AddStandartZone ()
{
    Urho3D::Node *zoneNode = scene_->CreateChild ("zone", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    zoneNode->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> ("Objects/zone_for_server.xml")->GetRoot ());
}

void Spawner::AddStandartTerrain ()
{
    Urho3D::Node *terrainNode = scene_->CreateChild ("terrain", Urho3D::REPLICATED);
    terrainNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_TERRAIN);

    Urho3D::Node *terrainLocal = terrainNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    terrainLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (SceneConstants::TERRAIN_LOCAL_PREFAB)->GetRoot ());
}

void Spawner::AddStandartTerrainBorders ()
{
    for (float x = -52.5f; x <= 52.5f; x += 5.0f)
    {
        if (x == -52.5f || x == 52.5f)
        {
            for (float z = -52.5f; z <= 52.5f; z += 5)
                AddStandartObstacle (Urho3D::Vector3 (x, 2.5f, z), Urho3D::Quaternion ());
        }
        else
        {
            AddStandartObstacle (Urho3D::Vector3 (x, 2.5f, -52.5f), Urho3D::Quaternion ());
            AddStandartObstacle (Urho3D::Vector3 (x, 2.5f, 52.5f), Urho3D::Quaternion ());
        }
    }
}

float Spawner::GetMinimumDistanceBetween (Urho3D::Vector3 position, Urho3D::PODVector <Urho3D::Vector3> &others)
{
    if (others.Empty ())
        return -1.0f;

    float minimumDistance = (others.At (0) - position).Length ();
    for (int index = 1; index < others.Size (); index++)
    {
        float distance = (others.At (index) - position).Length ();
        if (distance < minimumDistance)
            minimumDistance = distance;
    }
    return minimumDistance;
}

void Spawner::GenerateObstacles (int count)
{
    for (int index = 0; index < count; index++)
    {
        Urho3D::Vector3 position;
        float minimumDistance;
        do
        {
            position = Urho3D::Vector3 (Urho3D::Random (-50.0f, 50.0f), 2.5f, Urho3D::Random (-50.0f, 50.0f));
            minimumDistance = GetMinimumDistanceBetween (position, placedObstacles_);
        }
        while (minimumDistance < ServerConstants::MINIMUM_DISTANCE_BETWEEN_OBSTACLES && minimumDistance > 0.0f);
        AddStandartObstacle (position, Urho3D::Quaternion (0, Urho3D::Random (360.0f), 0));
    }
}

void Spawner::AddStandartObstacle (Urho3D::Vector3 position, Urho3D::Quaternion rotation)
{
    Urho3D::Node *obstacleNode = scene_->CreateChild ("obstacle", Urho3D::REPLICATED);
    obstacleNode->SetPosition (position);
    obstacleNode->SetRotation (rotation);
    obstacleNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_OBSTACLE);

    Urho3D::Node *obstacleLocal = obstacleNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    obstacleLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (SceneConstants::OBSTACLE_LOCAL_PREFAB)->GetRoot ());
    placedObstacles_.Push (position);
}

unsigned Spawner::SpawnPlayer ()
{
    Urho3D::Vector3 position;
    float minimumDistance;
    do
    {
        position = Urho3D::Vector3 (Urho3D::Random (-30.0f, 30.0f), 2.5f, Urho3D::Random (-30.0f, 30.0f));
        minimumDistance = GetMinimumDistanceBetween (position, placedObstacles_);
    }
    while (minimumDistance < ServerConstants::MINIMUM_DISTANCE_BETWEEN_OBSTACLES && minimumDistance > 0.0f);

    Urho3D::Node *playerNode = scene_->CreateChild ("player", Urho3D::REPLICATED);
    playerNode->SetPosition (position);
    playerNode->SetRotation (Urho3D::Quaternion (0, Urho3D::Random (360.0f), 0));
    playerNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_PLAYER);

    Urho3D::Node *playerLocal = playerNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    playerLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (SceneConstants::PLAYER_LOCAL_PREFAB)->GetRoot ());

    Urho3D::SharedPtr <Urho3D::RigidBody> body (playerLocal->GetComponent <Urho3D::RigidBody> ());
    body->Remove ();
    playerNode->AddComponent (body.Get (), Urho3D::FIRST_LOCAL_ID, Urho3D::LOCAL);

    Urho3D::SharedPtr <Urho3D::CollisionShape> shape (playerLocal->GetComponent <Urho3D::CollisionShape> ());
    shape->Remove ();
    playerNode->AddComponent (shape.Get (), Urho3D::FIRST_LOCAL_ID, Urho3D::LOCAL);
    return playerNode->GetID ();
}

