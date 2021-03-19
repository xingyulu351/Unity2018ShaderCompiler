#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "UnityPrefix.h"
#include "Runtime/Mono/MonoManager.h"
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_RuntimeAnimatorController_IcallNames [] =
{
    NULL
};

static const void* s_RuntimeAnimatorController_IcallFuncs [] =
{
    NULL
};

void ExportRuntimeAnimatorControllerBindings();
void ExportRuntimeAnimatorControllerBindings()
{
    for (int i = 0; s_RuntimeAnimatorController_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_RuntimeAnimatorController_IcallNames [i], s_RuntimeAnimatorController_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportRuntimeAnimatorControllerBindings()
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
