#include "BuildConfiguration.hpp"
#include "PlayerState.hpp"
#include "PlayersManager.hpp"
#include "Spawner.hpp"

#include <Shared/Constants.hpp>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/Context.h>

PlayerState::PlayerState (PlayersManager *manager, Urho3D::Connection *connection) :
    manager_ (manager),
    connection_ (connection),
    timeFromLastFire_ (GameplayConstants::FIRE_COOLDOWN_TIME),
    name_ (Urho3D::String::EMPTY),
    timeBeforeAutomaticNaming_ (ServerConstants::TIME_BEFORE_AUTOMATIC_NAMING),
    node_ (0),
    timeBeforeSpawn_ (ServerConstants::TIME_BEFORE_AUTOMATIC_NAMING * 2.0f),
    deaths_ (0),
    kills_ (0),
    isAi_ (false)
{
    assert (manager);
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
                manager_->RequestRespawn (this, isAi_);
                node_->SetVar (SerializationConstants::HEALTH_VAR_HASH, GameplayConstants::BASIC_MAX_HEALTH);
                node_->SetVar (SerializationConstants::NAME_VAR_HASH, Urho3D::Variant (name_));
                node_->SetVar (SerializationConstants::EXP_VAR_HASH, Urho3D::Variant (0));
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
                timeBeforeSpawn_ = GameplayConstants::RESPAWN_TIME;
            }
            else
            {
                health += GameplayConstants::HEALTH_REGENERATION * timeStep;
                int exp = node_->GetVar (SerializationConstants::EXP_VAR_HASH).GetInt ();
                if (health > GameplayConstants::BASIC_MAX_HEALTH * (1 + exp * GameplayConstants::MAX_HEALTH_INCREASE_PER_EXP))
                    health = GameplayConstants::BASIC_MAX_HEALTH * (1 + exp * GameplayConstants::MAX_HEALTH_INCREASE_PER_EXP);
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

void PlayerState::TryToFire ()
{
    Spawner *spawner = manager_->GetContext ()->GetSubsystem <Spawner> ();
    if (node_ && timeFromLastFire_ >= GameplayConstants::FIRE_COOLDOWN_TIME)
    {
        assert (spawner);
        spawner->SpawnShell (this);
        timeFromLastFire_ = 0.0f;
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
    assert (normalizedMoveRequest.Length () <= sqrtf (2) + 0.0001f);
    normalizedMoveRequest_ = normalizedMoveRequest;
}

int PlayerState::GetKills ()
{
    return kills_;
}

void PlayerState::IncrementKills ()
{
    kills_ ++;
    if (node_)
    {
        int exp = node_->GetVar (SerializationConstants::EXP_VAR_HASH).GetInt ();
        if (exp < GameplayConstants::MAX_EXP)
        {
            exp ++;
            node_->SetVar (SerializationConstants::EXP_VAR_HASH, Urho3D::Variant (exp));
        }
    }
}

int PlayerState::GetDeaths ()
{
    return deaths_;
}

void PlayerState::IncrementDeaths ()
{
    deaths_ ++;
}

void PlayerState::ClearStatistics()
{
    kills_ = 0;
    deaths_ = 0;
}

float PlayerState::GetShellDamage ()
{
    int exp = 0;
    if (node_)
        exp = node_->GetVar (SerializationConstants::EXP_VAR_HASH).GetInt ();
    return GameplayConstants::BASIC_SHELL_DAMAGE * (1 + exp * GameplayConstants::SHELL_DAMAGE_INCREASE_PER_EXP);
}

bool PlayerState::ApplyDamage (float damage)
{
    if (node_)
    {
        float health = node_->GetVar (SerializationConstants::HEALTH_VAR_HASH).GetFloat ();
        health -= damage;
        node_->SetVar (SerializationConstants::HEALTH_VAR_HASH, Urho3D::Variant (health));
        return (health >= 0.0f);
    }
    else
        return false;
}

Urho3D::String PlayerState::GetScriptPath ()
{
    return "";
}

