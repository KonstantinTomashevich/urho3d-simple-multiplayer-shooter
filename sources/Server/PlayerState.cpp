#include "BuildConfiguration.hpp"
#include "PlayerState.hpp"
#include "PlayersManager.hpp"
#include <Shared/Constants.hpp>

PlayerState::PlayerState()
{
    // Empty constructor for normal using in Urho3D's containers.
}

PlayerState::PlayerState (PlayersManager *manager, Urho3D::Connection *connection)
{
    assert (manager);
    assert (connection);
    manager_ = manager;
    connection_ = connection;
    timeFromLastFire_ = 2 * ServerConstants::FIRE_COOLDOWN_TIME;

    name_ = Urho3D::String::EMPTY;
    timeBeforeAutomaticNaming_ = ServerConstants::TIME_BEFORE_AUTOMATIC_NAMING;

    node_ = 0;
    timeBeforeSpawn_ = ServerConstants::RESPAWN_TIME;

    deaths_ = 0;
    kills_ = 0;
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
                manager_->RequestRespawn (this);
        }
        else
        {
            float health = node_->GetVar (GameConstants::HEALTH_VAR_HASH).GetFloat ();
            if (health < 0)
            {
                // Server will remove all nodes with 'health < 0' after DEAD_PLAYERS_REMOVE_TIME.
                node_ = 0;
                IncrementDeaths ();
            }
            else
            {
                health += ServerConstants::HEALTH_REGENERATION * timeStep;
                if (health > 100.0f)
                    health = 100.0f;
                node_->SetVar (GameConstants::HEALTH_VAR_HASH, Urho3D::Variant (health));

                if (timeFromLastFire_ <= 2 * ServerConstants::FIRE_COOLDOWN_TIME)
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

