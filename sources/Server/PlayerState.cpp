#include "BuildConfiguration.hpp"
#include "PlayerState.hpp"
#include "PlayersManager.hpp"
#include <Shared/Constants.hpp>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/IO/Log.h>

PlayerState::PlayerState (PlayersManager *manager, Urho3D::Connection *connection) :
    manager_ (manager),
    connection_ (connection),
    timeFromLastFire_ (GameplayConstants::FIRE_COOLDOWN_TIME),
    name_ (Urho3D::String::EMPTY),
    timeBeforeAutomaticNaming_ (ServerConstants::TIME_BEFORE_AUTOMATIC_NAMING),
    node_ (0),
    timeBeforeSpawn_ (GameplayConstants::RESPAWN_TIME),
    deaths_ (0),
    kills_ (0)
{
    assert (manager);
    assert (connection);
}

PlayerState::~PlayerState ()
{

}

void PlayerState::Update (float timeStep)
{
    if (name_ == Urho3D::String::EMPTY)
    {
        if (timeBeforeAutomaticNaming_ >= 0)
            timeBeforeAutomaticNaming_ -= timeStep;
        else
            manager_->RequestName (this);
    }
    else
    {
        if (!node_)
        {
            if (timeBeforeSpawn_ >= 0)
                timeBeforeSpawn_ -= timeStep;
            else
            {
                manager_->RequestRespawn (this);
                node_->SetVar (SerializationConstants::HEALTH_VAR_HASH, GameplayConstants::MAX_HEALTH);
            }
        }
        else
        {
            float health = node_->GetVar (SerializationConstants::HEALTH_VAR_HASH).GetFloat ();
            if (health < 0)
            {
                // Server will remove all nodes with 'health < 0' after DEAD_PLAYERS_REMOVE_TIME.
                node_ = 0;
                IncrementDeaths ();
            }
            else
            {
                health += GameplayConstants::HEALTH_REGENERATION * timeStep;
                if (health > 100.0f)
                    health = 100.0f;
                node_->SetVar (SerializationConstants::HEALTH_VAR_HASH, Urho3D::Variant (health));

                Urho3D::RigidBody *physicsBody = node_->GetComponent <Urho3D::RigidBody> ();
                physicsBody->SetLinearVelocity (
                            physicsBody->GetLinearVelocity () * Urho3D::Vector3 (0, 1, 0));
                physicsBody->SetAngularVelocity (Urho3D::Vector3 (0, 0, 0));

                physicsBody->ApplyImpulse (
                            node_->GetWorldDirection () * GameplayConstants::MOVE_IMPULSE * normalizedMoveRequest_.x_);

                physicsBody->ApplyTorqueImpulse (
                            Urho3D::Vector3::UP * GameplayConstants::ROTATION_IMPULSE * normalizedMoveRequest_.y_);

                if (timeFromLastFire_ <= GameplayConstants::FIRE_COOLDOWN_TIME)
                    timeFromLastFire_ += timeStep;
            }
        }
    }
}

Urho3D::Connection *PlayerState::GetConnection ()
{
    return connection_;
}

float PlayerState::GetTimeFromLastFire ()
{
    return timeFromLastFire_;
}

void PlayerState::SetTimeFromLastFire (float timeFromLastFire)
{
    timeFromLastFire_ = timeFromLastFire;
}

float PlayerState::GetTimeBeforeSpawn ()
{
    return timeBeforeSpawn_;
}

Urho3D::String PlayerState::GetName ()
{
    return name_;
}

void PlayerState::SetName (Urho3D::String name)
{
    assert (name_ == Urho3D::String::EMPTY);
    assert (name != Urho3D::String::EMPTY);
    name_ = name;
}

Urho3D::Node *PlayerState::GetNode ()
{
    return node_;
}

void PlayerState::SetNode (Urho3D::Node *node)
{
    assert (!node_);
    assert (node);
    node_ = node;
}

Urho3D::Vector2 PlayerState::GetNormalizedMoveRequest ()
{
    return normalizedMoveRequest_;
}

void PlayerState::SetNormalizedMoveRequest (Urho3D::Vector2 normalizedMoveRequest)
{
    assert (normalizedMoveRequest.Length () <= sqrtf (2));
    normalizedMoveRequest_ = normalizedMoveRequest;
}

int PlayerState::GetKills ()
{
    return kills_;
}

void PlayerState::IncrementKills ()
{
    kills_ ++;
}

int PlayerState::GetDeaths ()
{
    return deaths_;
}

void PlayerState::IncrementDeaths ()
{
    deaths_ ++;
}

