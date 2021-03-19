#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"

#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineMetro_IcallNames [] =
{
    NULL
};

static const void* s_UnityEngineMetro_IcallFuncs [] =
{
    NULL
};

void ExportUnityEngineMetroBindings();
void ExportUnityEngineMetroBindings()
{
    for (int i = 0; s_UnityEngineMetro_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineMetro_IcallNames [i], s_UnityEngineMetro_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineMetroBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
}

#endif
