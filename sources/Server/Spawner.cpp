#include "BuildConfiguration.hpp"
#include "Spawner.hpp"
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Physics/PhysicsWorld.h>

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/AngelScript/ScriptInstance.h>
#include <Urho3D/AngelScript/ScriptFile.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/Context.h>
#include <Shared/Constants.hpp>

Spawner::Spawner (Urho3D::Context *context) :
    Urho3D::Object (context)
{
    SubscribeToEvent (Urho3D::StringHash ("Explossion"), URHO3D_HANDLER (Spawner, OnExplossion));
}

Spawner::~Spawner ()
{

}

Urho3D::PODVector <Urho3D::Vector3> *Spawner::GetPlacedObstaclesVectorPointer()
{
    return &placedObstacles_;
}

void Spawner::GenerateServerScene ()
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    scene->CreateComponent <Urho3D::Octree> ();
    scene->CreateComponent <Urho3D::PhysicsWorld> ();

    AddStandartLight ();
    AddStandartZone ();
    AddStandartTerrain ();
    AddStandartTerrainBorders ();
    AddStandartCamera ();
    GenerateObstacles (Urho3D::Random (
                           ServerConstants::MINIMUM_OBSTACLES_COUNT, ServerConstants::MAXIMUM_OBSTACLES_COUNT));


    Urho3D::ScriptInstance *serverCommandUiScript = scene->CreateComponent <Urho3D::ScriptInstance> (Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    serverCommandUiScript->CreateObject (resourceCache->GetResource <Urho3D::ScriptFile> (
                                             "AngelScript/Components/ServerCommandUi.as"), "ServerCommandUi");
}

void Spawner::AddStandartLight ()
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *lightNode = scene->CreateChild ("light", Urho3D::REPLICATED);
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
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *zoneNode = scene->CreateChild ("zone", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    zoneNode->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> ("Objects/zone_for_server.xml")->GetRoot ());
}

void Spawner::AddStandartTerrain ()
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *terrainNode = scene->CreateChild ("terrain", Urho3D::REPLICATED);
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

void Spawner::AddStandartCamera ()
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *cameraNode = scene->CreateChild ("server_local_camera", Urho3D::LOCAL);
    cameraNode->SetPosition (Urho3D::Vector3 (0, 50, 0));
    cameraNode->SetRotation (Urho3D::Quaternion (90, 0, 0));

    Urho3D::Camera *camera = cameraNode->CreateComponent <Urho3D::Camera> (Urho3D::LOCAL);
    camera->SetFarClip (300.0);
    camera->SetAutoAspectRatio (true);

    Urho3D::Renderer *renderer = GetSubsystem <Urho3D::Renderer> ();
    Urho3D::SharedPtr <Urho3D::Viewport> viewport (
                new Urho3D::Viewport (context_, scene, camera));
    renderer->SetViewport (0, viewport);
    renderer->SetShadowMapSize (1024);
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
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *obstacleNode = scene->CreateChild ("obstacle", Urho3D::REPLICATED);
    obstacleNode->SetPosition (position);
    obstacleNode->SetRotation (rotation);
    obstacleNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_OBSTACLE);

    Urho3D::Node *obstacleLocal = obstacleNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    obstacleLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (SceneConstants::OBSTACLE_LOCAL_PREFAB)->GetRoot ());
    placedObstacles_.Push (position);
}

unsigned Spawner::SpawnPlayer (bool isAi, Urho3D::String aiScriptPath)
{
    Urho3D::Vector3 position;
    float minimumDistance;
    do
    {
        position = Urho3D::Vector3 (Urho3D::Random (-30.0f, 30.0f), 2.5f, Urho3D::Random (-30.0f, 30.0f));
        minimumDistance = GetMinimumDistanceBetween (position, placedObstacles_);
    }
    while (minimumDistance < ServerConstants::MINIMUM_DISTANCE_BETWEEN_OBSTACLES && minimumDistance > 0.0f);

    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *playerNode = scene->CreateChild ("player", Urho3D::REPLICATED);
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

    if (isAi && !aiScriptPath.Empty ())
    {
        Urho3D::ScriptInstance *aiScript = playerNode->CreateComponent <Urho3D::ScriptInstance> (Urho3D::LOCAL);
        aiScript->CreateObject (resourceCache->GetResource <Urho3D::ScriptFile> (aiScriptPath), "Ai");
    }
    return playerNode->GetID ();
}

void Spawner::SpawnExplossion (Urho3D::Vector3 position)
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *explossionNode = scene->CreateChild ("explossion", Urho3D::REPLICATED);
    explossionNode->SetPosition (position);
    explossionNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_EXPLOSSION);

    Urho3D::Node *explossionLocal = explossionNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    explossionLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (SceneConstants::EXPLOSSION_LOCAL_PREFAB)->GetRoot ());
}

void Spawner::OnExplossion (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Vector3 position = eventData [Urho3D::StringHash ("Position")].GetVector3 ();
    SpawnExplossion (position);
}

void Spawner::SpawnShell (PlayerState *player)
{
    Urho3D::Scene *scene = context_->GetSubsystem <Urho3D::Scene> ();
    assert (scene);
    Urho3D::Node *shellNode = scene->CreateChild ("shell", Urho3D::REPLICATED);

    shellNode->SetPosition (player->GetNode ()->LocalToWorld (Urho3D::Vector3 (0.0f, 0.0f, 1.5f)));
    shellNode->SetVar (SerializationConstants::OBJECT_TYPE_VAR_HASH, SerializationConstants::OBJECT_TYPE_SHELL);
    shellNode->SetVar (Urho3D::StringHash ("ShooterName"), player->GetName ());

    Urho3D::Node *shellLocal = shellNode->CreateChild ("local", Urho3D::LOCAL);
    Urho3D::ResourceCache *resourceCache = GetSubsystem <Urho3D::ResourceCache> ();
    shellLocal->LoadXML (resourceCache->GetResource <Urho3D::XMLFile> (SceneConstants::SHELL_LOCAL_PREFAB)->GetRoot ());

    Urho3D::SharedPtr <Urho3D::RigidBody> body (shellLocal->GetComponent <Urho3D::RigidBody> ());
    body->Remove ();
    shellNode->AddComponent (body.Get (), Urho3D::FIRST_LOCAL_ID, Urho3D::LOCAL);
    body->SetLinearVelocity (player->GetNode ()->GetWorldRotation () * GameplayConstants::SHELL_LINEAR_VELOCITY);

    Urho3D::SharedPtr <Urho3D::CollisionShape> shape (shellLocal->GetComponent <Urho3D::CollisionShape> ());
    shape->Remove ();
    shellNode->AddComponent (shape.Get (), Urho3D::FIRST_LOCAL_ID, Urho3D::LOCAL);
}

