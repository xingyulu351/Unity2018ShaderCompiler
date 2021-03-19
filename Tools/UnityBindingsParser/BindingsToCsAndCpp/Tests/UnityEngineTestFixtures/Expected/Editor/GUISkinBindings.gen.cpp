#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Misc/InputEvent.h"
#include "Runtime/Scripting/ScriptingUtility.h"
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUISettings_CUSTOM_Internal_GetCursorFlashSpeed()
{
    SCRIPTINGAPI_ETW_ENTRY(GUISettings_CUSTOM_Internal_GetCursorFlashSpeed)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetCursorFlashSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetCursorFlashSpeed)
    
    		#if PLATFORM_WIN && !PLATFORM_WINRT
    		return 1000.0f / GetCaretBlinkTime();
    		#elif PLATFORM_IPHONE || PLATFORM_ANDROID || UNITY_PEPPER || PLATFORM_LINUX || UNITY_FLASH || PLATFORM_WEBGL || PLATFORM_WINRT || PLATFORM_OSX
    		return 2.0f;
    		#else
    		#error "Unknown platform"
    		#endif
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_GUISettings_Internal_GetCursorFlashSpeed()
{
    scripting_add_internal_call( "UnityEngine.GUISettings::Internal_GetCursorFlashSpeed" , (gpointer)& GUISettings_CUSTOM_Internal_GetCursorFlashSpeed );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GUISkin_IcallNames [] =
{
    "UnityEngine.GUISettings::Internal_GetCursorFlashSpeed",    // -> GUISettings_CUSTOM_Internal_GetCursorFlashSpeed
    NULL
};

static const void* s_GUISkin_IcallFuncs [] =
{
    (const void*)&GUISettings_CUSTOM_Internal_GetCursorFlashSpeed,  //  <- UnityEngine.GUISettings::Internal_GetCursorFlashSpeed
    NULL
};

void ExportGUISkinBindings();
void ExportGUISkinBindings()
{
    for (int i = 0; s_GUISkin_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GUISkin_IcallNames [i], s_GUISkin_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportGUISkinBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(GUISettings_CUSTOM_Internal_GetCursorFlashSpeed); //  <- UnityEngine.GUISettings::Internal_GetCursorFlashSpeed
}

#endif
