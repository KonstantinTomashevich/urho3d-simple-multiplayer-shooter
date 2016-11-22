#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include "PlayerState.hpp"

class Spawner : public Urho3D::Object
{
URHO3D_OBJECT (Spawner, Object)
protected:
    Urho3D::PODVector <Urho3D::Vector3> placedObstacles_;
public:
    Spawner (Urho3D::Context *context);
    virtual ~Spawner ();
    Urho3D::PODVector <Urho3D::Vector3> *GetPlacedObstaclesVectorPointer ();

    void GenerateServerScene ();
    void AddStandartLight ();
    void AddStandartZone ();
    void AddStandartTerrain ();
    void AddStandartTerrainBorders ();
    void AddStandartCamera ();

    float GetMinimumDistanceBetween (Urho3D::Vector3 position, Urho3D::PODVector <Urho3D::Vector3> &others);
    void GenerateObstacles (int count);
    void AddStandartObstacle (Urho3D::Vector3 position, Urho3D::Quaternion rotation);
    unsigned SpawnPlayer (bool isAi, Urho3D::String aiScriptPath = "");

    void SpawnExplossion (Urho3D::Vector3 position);
    void OnExplossion (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void SpawnShell (PlayerState *player);
};

