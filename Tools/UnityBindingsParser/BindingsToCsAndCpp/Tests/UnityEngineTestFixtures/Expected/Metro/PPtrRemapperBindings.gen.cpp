#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
#include "Runtime/Mono/MonoBehaviourSerialization_ByCodeGeneration.h"
#endif
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION PPtrRemapper_CUSTOM_INTERNAL_GetNewInstanceToReplaceOldInstance(int instance_id)
{
    SCRIPTINGAPI_ETW_ENTRY(PPtrRemapper_CUSTOM_INTERNAL_GetNewInstanceToReplaceOldInstance)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_GetNewInstanceToReplaceOldInstance)
    
    		return NativeExt_MonoBehaviourSerialization_GetNewInstanceToReplaceOldInstance(instance_id);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
void Register_UnityEngine_Serialization_PPtrRemapper_INTERNAL_GetNewInstanceToReplaceOldInstance()
{
    scripting_add_internal_call( "UnityEngine.Serialization.PPtrRemapper::INTERNAL_GetNewInstanceToReplaceOldInstance" , (gpointer)& PPtrRemapper_CUSTOM_INTERNAL_GetNewInstanceToReplaceOldInstance );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_PPtrRemapper_IcallNames [] =
{
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    "UnityEngine.Serialization.PPtrRemapper::INTERNAL_GetNewInstanceToReplaceOldInstance",    // -> PPtrRemapper_CUSTOM_INTERNAL_GetNewInstanceToReplaceOldInstance
#endif
    NULL
};

static const void* s_PPtrRemapper_IcallFuncs [] =
{
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    (const void*)&PPtrRemapper_CUSTOM_INTERNAL_GetNewInstanceToReplaceOldInstance,  //  <- UnityEngine.Serialization.PPtrRemapper::INTERNAL_GetNewInstanceToReplaceOldInstance
#endif
    NULL
};

void ExportPPtrRemapperBindings();
void ExportPPtrRemapperBindings()
{
    for (int i = 0; s_PPtrRemapper_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_PPtrRemapper_IcallNames [i], s_PPtrRemapper_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportPPtrRemapperBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    SET_METRO_BINDING(PPtrRemapper_CUSTOM_INTERNAL_GetNewInstanceToReplaceOldInstance); //  <- UnityEngine.Serialization.PPtrRemapper::INTERNAL_GetNewInstanceToReplaceOldInstance
#endif
}

#endif
