#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Graphics/LightProbeGroup.h"
#if !UNITY_FLASH && !PLATFORM_WEBGL
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION LightProbeGroup_Get_Custom_PropProbePositions(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbeGroup_Get_Custom_PropProbePositions)
    ReadOnlyScriptingObjectOfType<LightProbeGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_probePositions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_probePositions)
    #if UNITY_EDITOR
    return CreateScriptingArray (self->GetPositions(), self->GetPositionsSize (), GetCommonScriptingClasses ().vector3);
    #else
    return CreateScriptingArray ((const Vector3f*)NULL, 0, GetCommonScriptingClasses ().vector3);
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightProbeGroup_Set_Custom_PropProbePositions(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbeGroup_Set_Custom_PropProbePositions)
    ReadOnlyScriptingObjectOfType<LightProbeGroup> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_probePositions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_probePositions)
    
    #if UNITY_EDITOR
    self->SetPositions (&GetMonoArrayElement<Vector3f> (value, 0), mono_array_length_safe(value));
    #endif
    
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if !UNITY_FLASH && !PLATFORM_WEBGL
void Register_UnityEngine_LightProbeGroup_get_probePositions()
{
    scripting_add_internal_call( "UnityEngine.LightProbeGroup::get_probePositions" , (gpointer)& LightProbeGroup_Get_Custom_PropProbePositions );
}

void Register_UnityEngine_LightProbeGroup_set_probePositions()
{
    scripting_add_internal_call( "UnityEngine.LightProbeGroup::set_probePositions" , (gpointer)& LightProbeGroup_Set_Custom_PropProbePositions );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_LightProbe_IcallNames [] =
{
#if !UNITY_FLASH && !PLATFORM_WEBGL
    "UnityEngine.LightProbeGroup::get_probePositions",    // -> LightProbeGroup_Get_Custom_PropProbePositions
    "UnityEngine.LightProbeGroup::set_probePositions",    // -> LightProbeGroup_Set_Custom_PropProbePositions
#endif
    NULL
};

static const void* s_LightProbe_IcallFuncs [] =
{
#if !UNITY_FLASH && !PLATFORM_WEBGL
    (const void*)&LightProbeGroup_Get_Custom_PropProbePositions,  //  <- UnityEngine.LightProbeGroup::get_probePositions
    (const void*)&LightProbeGroup_Set_Custom_PropProbePositions,  //  <- UnityEngine.LightProbeGroup::set_probePositions
#endif
    NULL
};

void ExportLightProbeBindings();
void ExportLightProbeBindings()
{
    for (int i = 0; s_LightProbe_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_LightProbe_IcallNames [i], s_LightProbe_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportLightProbeBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if !UNITY_FLASH && !PLATFORM_WEBGL
    SET_METRO_BINDING(LightProbeGroup_Get_Custom_PropProbePositions); //  <- UnityEngine.LightProbeGroup::get_probePositions
    SET_METRO_BINDING(LightProbeGroup_Set_Custom_PropProbePositions); //  <- UnityEngine.LightProbeGroup::set_probePositions
#endif
}

#endif
