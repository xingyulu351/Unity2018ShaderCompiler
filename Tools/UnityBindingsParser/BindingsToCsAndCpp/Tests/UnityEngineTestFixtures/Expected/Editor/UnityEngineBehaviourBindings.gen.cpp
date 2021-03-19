#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"

using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Behaviour_Get_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Behaviour_Get_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<Behaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Behaviour_Set_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Behaviour_Set_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<Behaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    self->SetEnabled (value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Behaviour_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Behaviour::get_enabled" , (gpointer)& Behaviour_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Behaviour_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Behaviour::set_enabled" , (gpointer)& Behaviour_Set_Custom_PropEnabled );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineBehaviour_IcallNames [] =
{
    "UnityEngine.Behaviour::get_enabled"    ,    // -> Behaviour_Get_Custom_PropEnabled
    "UnityEngine.Behaviour::set_enabled"    ,    // -> Behaviour_Set_Custom_PropEnabled
    NULL
};

static const void* s_UnityEngineBehaviour_IcallFuncs [] =
{
    (const void*)&Behaviour_Get_Custom_PropEnabled        ,  //  <- UnityEngine.Behaviour::get_enabled
    (const void*)&Behaviour_Set_Custom_PropEnabled        ,  //  <- UnityEngine.Behaviour::set_enabled
    NULL
};

void ExportUnityEngineBehaviourBindings();
void ExportUnityEngineBehaviourBindings()
{
    for (int i = 0; s_UnityEngineBehaviour_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineBehaviour_IcallNames [i], s_UnityEngineBehaviour_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineBehaviourBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Behaviour_Get_Custom_PropEnabled); //  <- UnityEngine.Behaviour::get_enabled
    SET_METRO_BINDING(Behaviour_Set_Custom_PropEnabled); //  <- UnityEngine.Behaviour::set_enabled
}

#endif
