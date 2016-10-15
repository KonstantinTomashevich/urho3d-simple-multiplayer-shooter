#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>

class Spawner : public Urho3D::Object
{
URHO3D_OBJECT (Spawner, Object)
protected:
    Urho3D::Scene *scene_;
    Urho3D::PODVector <Urho3D::Vector3> placedObstacles_;
public:
    Spawner (Urho3D::Context *context);
    virtual ~Spawner ();

    void SetScene (Urho3D::Scene *scene);
    Urho3D::Scene *GetScene ();
    Urho3D::PODVector <Urho3D::Vector3> *GetPlacedObstaclesVectorPointer ();

    void GenerateServerScene ();
    void AddStandartLight ();
    void AddStandartZone ();
    void AddStandartTerrain ();

    float GetMinimumDistanceBetween (Urho3D::Vector3 position, Urho3D::PODVector <Urho3D::Vector3> &others);
    void GenerateObstacles (int count);
    void AddStandartObstacle (Urho3D::Vector3 position, Urho3D::Quaternion rotation);
};

