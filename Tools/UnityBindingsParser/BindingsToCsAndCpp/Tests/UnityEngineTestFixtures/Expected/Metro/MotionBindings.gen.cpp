#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoScript.h"
#include "Runtime/Animation/Motion.h"
#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Motion_CUSTOM_ValidateIfRetargetable(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool showWarning)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_CUSTOM_ValidateIfRetargetable)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ValidateIfRetargetable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ValidateIfRetargetable)
    return self->ValidateIfRetargetable(showWarning);
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Motion_Get_Custom_PropAverageDuration(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_Get_Custom_PropAverageDuration)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_averageDuration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_averageDuration)
    return self->GetAverageDuration();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Motion_Get_Custom_PropAverageAngularSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_Get_Custom_PropAverageAngularSpeed)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_averageAngularSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_averageAngularSpeed)
    return self->GetAverageAngularSpeed();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Motion_CUSTOM_INTERNAL_get_averageSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_CUSTOM_INTERNAL_get_averageSpeed)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_averageSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_averageSpeed)
    { *returnValue =(self->GetAverageSpeed()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Motion_Get_Custom_PropApparentSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_Get_Custom_PropApparentSpeed)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_apparentSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_apparentSpeed)
    return self->GetApparentSpeed();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Motion_Get_Custom_PropIsLooping(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_Get_Custom_PropIsLooping)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isLooping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isLooping)
    return self->IsLooping();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Motion_Get_Custom_PropIsAnimatorMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_Get_Custom_PropIsAnimatorMotion)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isAnimatorMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isAnimatorMotion)
    return self->IsAnimatorMotion();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Motion_Get_Custom_PropIsHumanMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Motion_Get_Custom_PropIsHumanMotion)
    ReadOnlyScriptingObjectOfType<Motion> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isHumanMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isHumanMotion)
    return self->IsHumanMotion();
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if UNITY_EDITOR
void Register_UnityEngine_Motion_ValidateIfRetargetable()
{
    scripting_add_internal_call( "UnityEngine.Motion::ValidateIfRetargetable" , (gpointer)& Motion_CUSTOM_ValidateIfRetargetable );
}

void Register_UnityEngine_Motion_get_averageDuration()
{
    scripting_add_internal_call( "UnityEngine.Motion::get_averageDuration" , (gpointer)& Motion_Get_Custom_PropAverageDuration );
}

void Register_UnityEngine_Motion_get_averageAngularSpeed()
{
    scripting_add_internal_call( "UnityEngine.Motion::get_averageAngularSpeed" , (gpointer)& Motion_Get_Custom_PropAverageAngularSpeed );
}

void Register_UnityEngine_Motion_INTERNAL_get_averageSpeed()
{
    scripting_add_internal_call( "UnityEngine.Motion::INTERNAL_get_averageSpeed" , (gpointer)& Motion_CUSTOM_INTERNAL_get_averageSpeed );
}

void Register_UnityEngine_Motion_get_apparentSpeed()
{
    scripting_add_internal_call( "UnityEngine.Motion::get_apparentSpeed" , (gpointer)& Motion_Get_Custom_PropApparentSpeed );
}

void Register_UnityEngine_Motion_get_isLooping()
{
    scripting_add_internal_call( "UnityEngine.Motion::get_isLooping" , (gpointer)& Motion_Get_Custom_PropIsLooping );
}

void Register_UnityEngine_Motion_get_isAnimatorMotion()
{
    scripting_add_internal_call( "UnityEngine.Motion::get_isAnimatorMotion" , (gpointer)& Motion_Get_Custom_PropIsAnimatorMotion );
}

void Register_UnityEngine_Motion_get_isHumanMotion()
{
    scripting_add_internal_call( "UnityEngine.Motion::get_isHumanMotion" , (gpointer)& Motion_Get_Custom_PropIsHumanMotion );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Motion_IcallNames [] =
{
#if UNITY_EDITOR
    "UnityEngine.Motion::ValidateIfRetargetable",    // -> Motion_CUSTOM_ValidateIfRetargetable
    "UnityEngine.Motion::get_averageDuration",    // -> Motion_Get_Custom_PropAverageDuration
    "UnityEngine.Motion::get_averageAngularSpeed",    // -> Motion_Get_Custom_PropAverageAngularSpeed
    "UnityEngine.Motion::INTERNAL_get_averageSpeed",    // -> Motion_CUSTOM_INTERNAL_get_averageSpeed
    "UnityEngine.Motion::get_apparentSpeed" ,    // -> Motion_Get_Custom_PropApparentSpeed
    "UnityEngine.Motion::get_isLooping"     ,    // -> Motion_Get_Custom_PropIsLooping
    "UnityEngine.Motion::get_isAnimatorMotion",    // -> Motion_Get_Custom_PropIsAnimatorMotion
    "UnityEngine.Motion::get_isHumanMotion" ,    // -> Motion_Get_Custom_PropIsHumanMotion
#endif
    NULL
};

static const void* s_Motion_IcallFuncs [] =
{
#if UNITY_EDITOR
    (const void*)&Motion_CUSTOM_ValidateIfRetargetable    ,  //  <- UnityEngine.Motion::ValidateIfRetargetable
    (const void*)&Motion_Get_Custom_PropAverageDuration   ,  //  <- UnityEngine.Motion::get_averageDuration
    (const void*)&Motion_Get_Custom_PropAverageAngularSpeed,  //  <- UnityEngine.Motion::get_averageAngularSpeed
    (const void*)&Motion_CUSTOM_INTERNAL_get_averageSpeed ,  //  <- UnityEngine.Motion::INTERNAL_get_averageSpeed
    (const void*)&Motion_Get_Custom_PropApparentSpeed     ,  //  <- UnityEngine.Motion::get_apparentSpeed
    (const void*)&Motion_Get_Custom_PropIsLooping         ,  //  <- UnityEngine.Motion::get_isLooping
    (const void*)&Motion_Get_Custom_PropIsAnimatorMotion  ,  //  <- UnityEngine.Motion::get_isAnimatorMotion
    (const void*)&Motion_Get_Custom_PropIsHumanMotion     ,  //  <- UnityEngine.Motion::get_isHumanMotion
#endif
    NULL
};

void ExportMotionBindings();
void ExportMotionBindings()
{
    for (int i = 0; s_Motion_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Motion_IcallNames [i], s_Motion_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportMotionBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if UNITY_EDITOR
    SET_METRO_BINDING(Motion_CUSTOM_ValidateIfRetargetable); //  <- UnityEngine.Motion::ValidateIfRetargetable
    SET_METRO_BINDING(Motion_Get_Custom_PropAverageDuration); //  <- UnityEngine.Motion::get_averageDuration
    SET_METRO_BINDING(Motion_Get_Custom_PropAverageAngularSpeed); //  <- UnityEngine.Motion::get_averageAngularSpeed
    SET_METRO_BINDING(Motion_CUSTOM_INTERNAL_get_averageSpeed); //  <- UnityEngine.Motion::INTERNAL_get_averageSpeed
    SET_METRO_BINDING(Motion_Get_Custom_PropApparentSpeed); //  <- UnityEngine.Motion::get_apparentSpeed
    SET_METRO_BINDING(Motion_Get_Custom_PropIsLooping); //  <- UnityEngine.Motion::get_isLooping
    SET_METRO_BINDING(Motion_Get_Custom_PropIsAnimatorMotion); //  <- UnityEngine.Motion::get_isAnimatorMotion
    SET_METRO_BINDING(Motion_Get_Custom_PropIsHumanMotion); //  <- UnityEngine.Motion::get_isHumanMotion
#endif
}

#endif
