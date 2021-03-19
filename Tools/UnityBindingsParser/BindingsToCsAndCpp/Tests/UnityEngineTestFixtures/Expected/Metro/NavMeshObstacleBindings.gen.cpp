#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/NavMesh/NavMeshObstacle.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshObstacle_Get_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshObstacle_Get_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<NavMeshObstacle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_height)
    return self->GetHeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshObstacle_Set_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshObstacle_Set_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<NavMeshObstacle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_height)
    
    self->SetHeight (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshObstacle_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshObstacle_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<NavMeshObstacle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshObstacle_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshObstacle_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<NavMeshObstacle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshObstacle_CUSTOM_INTERNAL_get_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshObstacle_CUSTOM_INTERNAL_get_velocity)
    ReadOnlyScriptingObjectOfType<NavMeshObstacle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_velocity)
    *returnValue = self->GetVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshObstacle_CUSTOM_INTERNAL_set_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshObstacle_CUSTOM_INTERNAL_set_velocity)
    ReadOnlyScriptingObjectOfType<NavMeshObstacle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_velocity)
    
    self->SetVelocity (value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_NavMeshObstacle_get_height()
{
    scripting_add_internal_call( "UnityEngine.NavMeshObstacle::get_height" , (gpointer)& NavMeshObstacle_Get_Custom_PropHeight );
}

void Register_UnityEngine_NavMeshObstacle_set_height()
{
    scripting_add_internal_call( "UnityEngine.NavMeshObstacle::set_height" , (gpointer)& NavMeshObstacle_Set_Custom_PropHeight );
}

void Register_UnityEngine_NavMeshObstacle_get_radius()
{
    scripting_add_internal_call( "UnityEngine.NavMeshObstacle::get_radius" , (gpointer)& NavMeshObstacle_Get_Custom_PropRadius );
}

void Register_UnityEngine_NavMeshObstacle_set_radius()
{
    scripting_add_internal_call( "UnityEngine.NavMeshObstacle::set_radius" , (gpointer)& NavMeshObstacle_Set_Custom_PropRadius );
}

void Register_UnityEngine_NavMeshObstacle_INTERNAL_get_velocity()
{
    scripting_add_internal_call( "UnityEngine.NavMeshObstacle::INTERNAL_get_velocity" , (gpointer)& NavMeshObstacle_CUSTOM_INTERNAL_get_velocity );
}

void Register_UnityEngine_NavMeshObstacle_INTERNAL_set_velocity()
{
    scripting_add_internal_call( "UnityEngine.NavMeshObstacle::INTERNAL_set_velocity" , (gpointer)& NavMeshObstacle_CUSTOM_INTERNAL_set_velocity );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_NavMeshObstacle_IcallNames [] =
{
    "UnityEngine.NavMeshObstacle::get_height",    // -> NavMeshObstacle_Get_Custom_PropHeight
    "UnityEngine.NavMeshObstacle::set_height",    // -> NavMeshObstacle_Set_Custom_PropHeight
    "UnityEngine.NavMeshObstacle::get_radius",    // -> NavMeshObstacle_Get_Custom_PropRadius
    "UnityEngine.NavMeshObstacle::set_radius",    // -> NavMeshObstacle_Set_Custom_PropRadius
    "UnityEngine.NavMeshObstacle::INTERNAL_get_velocity",    // -> NavMeshObstacle_CUSTOM_INTERNAL_get_velocity
    "UnityEngine.NavMeshObstacle::INTERNAL_set_velocity",    // -> NavMeshObstacle_CUSTOM_INTERNAL_set_velocity
    NULL
};

static const void* s_NavMeshObstacle_IcallFuncs [] =
{
    (const void*)&NavMeshObstacle_Get_Custom_PropHeight   ,  //  <- UnityEngine.NavMeshObstacle::get_height
    (const void*)&NavMeshObstacle_Set_Custom_PropHeight   ,  //  <- UnityEngine.NavMeshObstacle::set_height
    (const void*)&NavMeshObstacle_Get_Custom_PropRadius   ,  //  <- UnityEngine.NavMeshObstacle::get_radius
    (const void*)&NavMeshObstacle_Set_Custom_PropRadius   ,  //  <- UnityEngine.NavMeshObstacle::set_radius
    (const void*)&NavMeshObstacle_CUSTOM_INTERNAL_get_velocity,  //  <- UnityEngine.NavMeshObstacle::INTERNAL_get_velocity
    (const void*)&NavMeshObstacle_CUSTOM_INTERNAL_set_velocity,  //  <- UnityEngine.NavMeshObstacle::INTERNAL_set_velocity
    NULL
};

void ExportNavMeshObstacleBindings();
void ExportNavMeshObstacleBindings()
{
    for (int i = 0; s_NavMeshObstacle_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_NavMeshObstacle_IcallNames [i], s_NavMeshObstacle_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportNavMeshObstacleBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(NavMeshObstacle_Get_Custom_PropHeight); //  <- UnityEngine.NavMeshObstacle::get_height
    SET_METRO_BINDING(NavMeshObstacle_Set_Custom_PropHeight); //  <- UnityEngine.NavMeshObstacle::set_height
    SET_METRO_BINDING(NavMeshObstacle_Get_Custom_PropRadius); //  <- UnityEngine.NavMeshObstacle::get_radius
    SET_METRO_BINDING(NavMeshObstacle_Set_Custom_PropRadius); //  <- UnityEngine.NavMeshObstacle::set_radius
    SET_METRO_BINDING(NavMeshObstacle_CUSTOM_INTERNAL_get_velocity); //  <- UnityEngine.NavMeshObstacle::INTERNAL_get_velocity
    SET_METRO_BINDING(NavMeshObstacle_CUSTOM_INTERNAL_set_velocity); //  <- UnityEngine.NavMeshObstacle::INTERNAL_set_velocity
}

#endif
