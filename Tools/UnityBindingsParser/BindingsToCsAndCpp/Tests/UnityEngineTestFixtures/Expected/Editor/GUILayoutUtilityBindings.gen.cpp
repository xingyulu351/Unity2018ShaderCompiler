#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Math/Rect.h"
#include "Runtime/IMGUI/GUIWindows.h"
#include "Runtime/Scripting/ScriptingUtility.h"

using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_GetWindowRect(int windowID, Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_GetWindowRect)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_GetWindowRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_GetWindowRect)
    
    		{ returnValue =(IMGUI::GetWindowRect (GetGUIState (), windowID)); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_MoveWindow(int windowID, const Rectf& r)
{
    SCRIPTINGAPI_ETW_ENTRY(GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_MoveWindow)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_MoveWindow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_MoveWindow)
    
    		IMGUI::MoveWindowFromLayout (GetGUIState (), windowID, r);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUILayoutUtility_CUSTOM_INTERNAL_CALL_GetWindowsBounds(Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUILayoutUtility_CUSTOM_INTERNAL_CALL_GetWindowsBounds)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetWindowsBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetWindowsBounds)
    
    		{ returnValue =(IMGUI::GetWindowsBounds (GetGUIState ())); return; }
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_GUILayoutUtility_INTERNAL_CALL_Internal_GetWindowRect()
{
    scripting_add_internal_call( "UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_GetWindowRect" , (gpointer)& GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_GetWindowRect );
}

void Register_UnityEngine_GUILayoutUtility_INTERNAL_CALL_Internal_MoveWindow()
{
    scripting_add_internal_call( "UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_MoveWindow" , (gpointer)& GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_MoveWindow );
}

void Register_UnityEngine_GUILayoutUtility_INTERNAL_CALL_GetWindowsBounds()
{
    scripting_add_internal_call( "UnityEngine.GUILayoutUtility::INTERNAL_CALL_GetWindowsBounds" , (gpointer)& GUILayoutUtility_CUSTOM_INTERNAL_CALL_GetWindowsBounds );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GUILayoutUtility_IcallNames [] =
{
    "UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_GetWindowRect",    // -> GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_GetWindowRect
    "UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_MoveWindow",    // -> GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_MoveWindow
    "UnityEngine.GUILayoutUtility::INTERNAL_CALL_GetWindowsBounds",    // -> GUILayoutUtility_CUSTOM_INTERNAL_CALL_GetWindowsBounds
    NULL
};

static const void* s_GUILayoutUtility_IcallFuncs [] =
{
    (const void*)&GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_GetWindowRect,  //  <- UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_GetWindowRect
    (const void*)&GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_MoveWindow,  //  <- UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_MoveWindow
    (const void*)&GUILayoutUtility_CUSTOM_INTERNAL_CALL_GetWindowsBounds,  //  <- UnityEngine.GUILayoutUtility::INTERNAL_CALL_GetWindowsBounds
    NULL
};

void ExportGUILayoutUtilityBindings();
void ExportGUILayoutUtilityBindings()
{
    for (int i = 0; s_GUILayoutUtility_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GUILayoutUtility_IcallNames [i], s_GUILayoutUtility_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportGUILayoutUtilityBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_GetWindowRect); //  <- UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_GetWindowRect
    SET_METRO_BINDING(GUILayoutUtility_CUSTOM_INTERNAL_CALL_Internal_MoveWindow); //  <- UnityEngine.GUILayoutUtility::INTERNAL_CALL_Internal_MoveWindow
    SET_METRO_BINDING(GUILayoutUtility_CUSTOM_INTERNAL_CALL_GetWindowsBounds); //  <- UnityEngine.GUILayoutUtility::INTERNAL_CALL_GetWindowsBounds
}

#endif
