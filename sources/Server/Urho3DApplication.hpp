#pragma once
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include "ServerCameraManager.hpp"
#include "PlayersManager.hpp"

class Urho3DApplication : public Urho3D::Application
{
URHO3D_OBJECT (Urho3DApplication, Application)
protected:
    Urho3D::Scene *scene_;
    ServerCameraManager *cameraManager_;
    PlayersManager *playersManager_;

    void GenerateScene ();
    void GenerateLight ();
    void GenerateTerrain ();
    void GenerateZone ();
    float GetNearestLength (Urho3D::Vector3 position, Urho3D::PODVector <Urho3D::Vector3> &others);
    void GenerateObstacles (int count);
    void GenerateObstacle (Urho3D::Vector3 position);
public:
    Urho3DApplication (Urho3D::Context *context);
    virtual ~Urho3DApplication ();

    virtual void Setup ();
    virtual void Start ();
    virtual void Stop ();
};
