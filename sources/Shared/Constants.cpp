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
GETTER (float, DEFAULT_CAMERA_MOVE_SPEED)
GETTER (float, SPEED_UP_MODIFER)
GETTER (int, PORT)

GETTER (int, KEY_SPEED_UP)
GETTER (int, KEY_LEFT)
GETTER (int, KEY_RIGHT)
GETTER (int, KEY_FORWARD)
GETTER (int, KEY_BACK)
GETTER (int, KEY_UP)
GETTER (int, KEY_DOWN)
GETTER (int, TIME_BEFORE_AUTOMATIC_NAMING)

GETTER (float, MINIMUM_DISTANCE_BETWEEN_OBSTACLES)
GETTER (int, MINIMUM_OBSTACLES_COUNT)
GETTER (int, MAXIMUM_OBSTACLES_COUNT)
}

namespace GameplayConstants
{

GETTER (int, RESPAWN_TIME)
GETTER (int, HEALTH_REGENERATION)
GETTER (int, MAX_HEALTH)
GETTER (int, DEAD_PLAYERS_REMOVE_TIME)
GETTER (float, FIRE_COOLDOWN_TIME)
}

namespace SerializationConstants
{
GETTER (Urho3D::StringHash, HEALTH_VAR_HASH)
GETTER (Urho3D::StringHash, OBJECT_TYPE_VAR_HASH)
GETTER (int, OBJECT_TYPE_WITHOUT_LOCALS)
GETTER (int, OBJECT_TYPE_TERRAIN)
GETTER (int, OBJECT_TYPE_OBSTACLE)
GETTER (int, OBJECT_TYPE_PLAYER)
GETTER (int, OBJECT_TYPE_SHELL)
}

namespace SceneConstants
{
GETTER (Urho3D::String, TERRAIN_LOCAL_PREFAB)
GETTER (Urho3D::String, OBSTACLE_LOCAL_PREFAB)
GETTER (Urho3D::String, PLAYER_LOCAL_PREFAB)
GETTER (Urho3D::String, SHELL_LOCAL_PREFAB)
}

void BindConstantsToAngelScript (Urho3D::Script *script)
{
    asIScriptEngine *engine = script->GetScriptEngine ();
    BIND_CONSTANT (engine, float, ServerConstants, DEFAULT_CAMERA_MOVE_SPEED);
    BIND_CONSTANT (engine, float, ServerConstants, SPEED_UP_MODIFER);
    BIND_CONSTANT (engine, int, ServerConstants, PORT);

    BIND_CONSTANT (engine, int, ServerConstants, KEY_SPEED_UP);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_LEFT);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_RIGHT);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_FORWARD);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_BACK);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_UP);
    BIND_CONSTANT (engine, int, ServerConstants, KEY_DOWN);
    BIND_CONSTANT (engine, int, ServerConstants, TIME_BEFORE_AUTOMATIC_NAMING);

    BIND_CONSTANT (engine, float, ServerConstants, MINIMUM_DISTANCE_BETWEEN_OBSTACLES);
    BIND_CONSTANT (engine, int, ServerConstants, MINIMUM_OBSTACLES_COUNT);
    BIND_CONSTANT (engine, int, ServerConstants, MAXIMUM_OBSTACLES_COUNT);

    BIND_CONSTANT (engine, int, GameplayConstants, RESPAWN_TIME);
    BIND_CONSTANT (engine, int, GameplayConstants, HEALTH_REGENERATION);
    BIND_CONSTANT (engine, int, GameplayConstants, MAX_HEALTH);
    BIND_CONSTANT (engine, int, GameplayConstants, DEAD_PLAYERS_REMOVE_TIME);
    BIND_CONSTANT (engine, float, GameplayConstants, FIRE_COOLDOWN_TIME);

    BIND_CONSTANT (engine, StringHash, SerializationConstants, HEALTH_VAR_HASH);
    BIND_CONSTANT (engine, StringHash, SerializationConstants, OBJECT_TYPE_VAR_HASH);
    BIND_CONSTANT (engine, int, SerializationConstants, OBJECT_TYPE_WITHOUT_LOCALS);
    BIND_CONSTANT (engine, int, SerializationConstants, OBJECT_TYPE_TERRAIN);
    BIND_CONSTANT (engine, int, SerializationConstants, OBJECT_TYPE_OBSTACLE);
    BIND_CONSTANT (engine, int, SerializationConstants, OBJECT_TYPE_PLAYER);
    BIND_CONSTANT (engine, int, SerializationConstants, OBJECT_TYPE_SHELL);

    BIND_CONSTANT (engine, String, SceneConstants, TERRAIN_LOCAL_PREFAB);
    BIND_CONSTANT (engine, String, SceneConstants, OBSTACLE_LOCAL_PREFAB);
    BIND_CONSTANT (engine, String, SceneConstants, PLAYER_LOCAL_PREFAB);
    BIND_CONSTANT (engine, String, SceneConstants, SHELL_LOCAL_PREFAB);

    engine->RegisterEnum ("NetworkMessageId");
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_PLAYER_NAME_SETTED", NetworkMessageIds::STC_PLAYER_NAME_SETTED);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_PLAYER_SPAWNED", NetworkMessageIds::STC_PLAYER_SPAWNED);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_PLAYER_DIE", NetworkMessageIds::STC_PLAYER_DIE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_CHAT_MESSAGE", NetworkMessageIds::STC_CHAT_MESSAGE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_SERVER_MESSAGE", NetworkMessageIds::STC_SERVER_MESSAGE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_STC_RETURN_TIME_UNTIL_SPAWN", NetworkMessageIds::STC_RETURN_TIME_UNTIL_SPAWN);

    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_REQUEST_NAME", NetworkMessageIds::CTS_REQUEST_NAME);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_SET_MOVE_VELOCITY_REQUEST", NetworkMessageIds::CTS_SET_MOVE_VELOCITY_REQUEST);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_SET_ROTATION_VELOCITY_REQUEST", NetworkMessageIds::CTS_SET_ROTATION_VELOCITY_REQUEST);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_REQUEST_FIRE", NetworkMessageIds::CTS_REQUEST_FIRE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_REQUEST_CHAT_MESSAGE", NetworkMessageIds::CTS_REQUEST_CHAT_MESSAGE);
    engine->RegisterEnumValue ("NetworkMessageId", "NMID_CTS_GET_TIME_UNTIL_SPAWN", NetworkMessageIds::CTS_GET_TIME_UNTIL_SPAWN);
}
