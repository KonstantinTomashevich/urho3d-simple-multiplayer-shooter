// Enables assertion
#undef NDEBUG
//

#include "Constants.hpp"
#include <AngelScript/angelscript.h>

#ifdef NDEBUG
    #define CHECK_ANGELSCRIPT_RETURN(expression) expression
#else
    #define CHECK_ANGELSCRIPT_RETURN(expression) assert ((expression) >= 0)
#endif

// Macro for easy defining of constants getters for AngelScript.
#define GETTER(type, constant) \
    type Get ## constant () \
    {\
        return constant; \
    } \

// Macro for easy binding of constants to AngelScript.
#define BIND_CONSTANT(asEngine, asType, constant_namespace, constant) \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterGlobalFunction ( \
                        (#asType + Urho3D::String (" get_") + #constant_namespace + Urho3D::String ("__")\
                            + #constant + Urho3D::String (" ()")).CString (), \
                        asFUNCTION (constant_namespace::Get ## constant), asCALL_CDECL) \
                ) \

namespace ServerConstants
{
GETTER (Urho3D::String, DEFAULT_SERVER_SCENE)
GETTER (float, DEFAULT_CAMERA_MOVE_SPEED)
GETTER (float, SPEED_UP_MODIFER)
GETTER (int, SERVER_PORT)

GETTER (int, KEY_SPEED_UP)
GETTER (int, KEY_LEFT)
GETTER (int, KEY_RIGHT)
GETTER (int, KEY_FORWARD)
GETTER (int, KEY_BACK)
GETTER (int, KEY_UP)
GETTER (int, KEY_DOWN)

GETTER (int, TIME_BEFORE_AUTOMATIC_NAMING)
GETTER (int, RESPAWN_TIME)

GETTER (int, HEALTH_REGENERATION)
GETTER (int, MAX_HEALTH)
GETTER (int, DEAD_PLAYERS_REMOVE_TIME)
GETTER (float, FIRE_COOLDOWN_TIME)
}

namespace GameConstants
{
GETTER (Urho3D::StringHash, HEALTH_VAR_HASH)
}

void BindConstantsToAngelScript (Urho3D::Script *script)
{
    asIScriptEngine *engine = script->GetScriptEngine ();
    BIND_CONSTANT (engine, String, ServerConstants, DEFAULT_SERVER_SCENE);
    BIND_CONSTANT (engine, float, ServerConstants, DEFAULT_CAMERA_MOVE_SPEED);
    BIND_CONSTANT (engine, float, ServerConstants, SPEED_UP_MODIFER);
    BIND_CONSTANT (engine, int, ServerConstants, SERVER_PORT);

    BIND_CONSTANT (engine, int, ServerConstants, KEY_SPEED_UP);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_LEFT);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_RIGHT);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_FORWARD);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_BACK);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_UP);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_DOWN);

    BIND_CONSTANT (engine, int, ServerConstants, TIME_BEFORE_AUTOMATIC_NAMING);
    BIND_CONSTANT (engine, int, ServerConstants, RESPAWN_TIME);

    BIND_CONSTANT (engine, int, ServerConstants, HEALTH_REGENERATION);
    BIND_CONSTANT (engine, int, ServerConstants, MAX_HEALTH);
    BIND_CONSTANT (engine, int, ServerConstants, DEAD_PLAYERS_REMOVE_TIME);
    BIND_CONSTANT (engine, float, ServerConstants, FIRE_COOLDOWN_TIME);

    BIND_CONSTANT (engine, StringHash, GameConstants, HEALTH_VAR_HASH);

    engine->RegisterEnum ("NetworkMessageId");
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_PLAYER_NAME_SETTED", NetworkMessageIds::STC_PLAYER_NAME_SETTED);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_PLAYER_SPAWNED", NetworkMessageIds::STC_PLAYER_SPAWNED);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_PLAYER_FIRES", NetworkMessageIds::STC_PLAYER_FIRES);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_CHAT_MESSAGE", NetworkMessageIds::STC_CHAT_MESSAGE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_SERVER_MESSAGE", NetworkMessageIds::STC_SERVER_MESSAGE);

    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_REQUEST_NAME", NetworkMessageIds::CTS_REQUEST_NAME);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_SET_MOVE_VELOCITY_REQUEST", NetworkMessageIds::CTS_SET_MOVE_VELOCITY_REQUEST);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_SET_ROTATION_VELOCITY_REQUEST", NetworkMessageIds::CTS_SET_ROTATION_VELOCITY_REQUEST);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_REQUEST_FIRE", NetworkMessageIds::CTS_REQUEST_FIRE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_REQUEST_CHAT_MESSAGE", NetworkMessageIds::CTS_REQUEST_CHAT_MESSAGE);
}
