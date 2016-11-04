#include "BuildConfiguration.hpp"
#include "AiPlayerState.hpp"
#include "PlayersManager.hpp"
#include <AngelScript/angelscript.h>
#include <Urho3D/AngelScript/APITemplates.h>

AiPlayerState::AiPlayerState (PlayersManager *manager, int aiType) :
    PlayerState (manager, 0),
    aiCommands_ (new AiCommands (manager->GetContext ()))
{
    isAi_ = true;
    aiType_ = aiType;
    timeBeforeSpawn_ = 0.0f;
}

AiPlayerState::~AiPlayerState ()
{

}

void AiPlayerState::Update (float timeStep)
{
    if (node_)
    {
        Urho3D::ScriptInstance *aiScript = node_->GetComponent <Urho3D::ScriptInstance> ();
        assert (aiScript);
        Urho3D::VariantVector parameters;
        parameters.Push (aiCommands_);
        aiCommands_->AddRef ();
        aiScript->Execute ("void WriteAiCommands (AiCommands@)", parameters);
    }

    SetNormalizedMoveRequest (aiCommands_->normalizedMoveRequest_);
    PlayerState::Update (timeStep);
    if (aiCommands_->tryToFireInNextFrame_)
        TryToFire ();
}

AiCommands::AiCommands (Urho3D::Context *context) : Urho3D::Object (context),
    normalizedMoveRequest_ (0, 0),
    tryToFireInNextFrame_ (false)
{

}

AiCommands::~AiCommands ()
{

}


#ifdef NDEBUG
    #define CHECK_ANGELSCRIPT_RETURN(expression) expression
#else
    #define CHECK_ANGELSCRIPT_RETURN(expression) assert ((expression) >= 0)
#endif

#define GETTER(type, from, what) \
    type get_ ## from ## _ ## what (from *obj) \
    { return obj->what ; }

GETTER (Urho3D::Vector2, AiCommands, normalizedMoveRequest_)
GETTER (bool, AiCommands, tryToFireInNextFrame_)

#define SETTER(type, from, what) \
    void set_ ## from ## _ ## what (from *obj, type toSet) \
    { obj->what = toSet; }

SETTER (Urho3D::Vector2, AiCommands, normalizedMoveRequest_)
SETTER (bool, AiCommands, tryToFireInNextFrame_)

void AiCommands::BindToAngelScript (Urho3D::Script *script)
{
    asIScriptEngine *engine = script->GetScriptEngine ();
    Urho3D::RegisterObject <AiCommands> (engine, "AiCommands");

    CHECK_ANGELSCRIPT_RETURN (engine->RegisterObjectMethod ("AiCommands",
                                                            "Vector2 get_normalizedMoveRequest ()",
                                                            asFUNCTION (get_AiCommands_normalizedMoveRequest_),
                                                            asCALL_CDECL_OBJFIRST));

    CHECK_ANGELSCRIPT_RETURN (engine->RegisterObjectMethod ("AiCommands",
                                                            "bool get_tryToFireInNextFrame ()",
                                                            asFUNCTION (get_AiCommands_tryToFireInNextFrame_),
                                                            asCALL_CDECL_OBJFIRST));

    CHECK_ANGELSCRIPT_RETURN (engine->RegisterObjectMethod ("AiCommands",
                                                            "void set_normalizedMoveRequest (Vector2)",
                                                            asFUNCTION (set_AiCommands_normalizedMoveRequest_),
                                                            asCALL_CDECL_OBJFIRST));

    CHECK_ANGELSCRIPT_RETURN (engine->RegisterObjectMethod ("AiCommands",
                                                            "void set_tryToFireInNextFrame (bool)",
                                                            asFUNCTION (set_AiCommands_tryToFireInNextFrame_),
                                                            asCALL_CDECL_OBJFIRST));
}
