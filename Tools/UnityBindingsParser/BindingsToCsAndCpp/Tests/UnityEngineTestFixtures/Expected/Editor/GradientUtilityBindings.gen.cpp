#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Animation/Animation.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/Animation/AnimationManager.h"
#include "Runtime/Animation/AnimationState.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Animation/AnimationCurveUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoScript.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Math/Gradient.h"

using namespace Unity;

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GradientUtility_IcallNames [] =
{
    NULL
};

static const void* s_GradientUtility_IcallFuncs [] =
{
    NULL
};

void ExportGradientUtilityBindings();
void ExportGradientUtilityBindings()
{
    for (int i = 0; s_GradientUtility_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GradientUtility_IcallNames [i], s_GradientUtility_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportGradientUtilityBindings()
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
