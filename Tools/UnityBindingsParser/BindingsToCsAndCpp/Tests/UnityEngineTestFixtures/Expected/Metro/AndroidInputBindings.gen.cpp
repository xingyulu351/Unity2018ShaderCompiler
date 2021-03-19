#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Input/GetInput.h"
#include "Runtime/Mono/MonoBehaviour.h"

#if PLATFORM_ANDROID
	#include <PlatformDependent/AndroidPlayer/AndroidInput.h>
	#include <android/input.h>
#endif
#if PLATFORM_ANDROID || UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidInput_CUSTOM_INTERNAL_CALL_GetSecondaryTouch(int index, Touch& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidInput_CUSTOM_INTERNAL_CALL_GetSecondaryTouch)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetSecondaryTouch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetSecondaryTouch)
    
    		Touch touch;
    #if PLATFORM_ANDROID
    		if (index >= 0 && index < GetTouchCount ((int)AINPUT_SOURCE_TOUCHPAD))
    		{
    			if (!GetTouch ((int)AINPUT_SOURCE_TOUCHPAD, index, touch))
    				RaiseMonoException ("Internal error.");
    		}
    		else
    			RaiseMonoException ("Index out of bounds.");
    #endif			
    		{ returnValue =(touch); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidInput_Get_Custom_PropTouchCountSecondary()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidInput_Get_Custom_PropTouchCountSecondary)
    SCRIPTINGAPI_STACK_CHECK(get_touchCountSecondary)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_touchCountSecondary)
    #if PLATFORM_ANDROID
    return GetTouchCount((int)AINPUT_SOURCE_TOUCHPAD);
    #else
    return 0;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidInput_Get_Custom_PropSecondaryTouchEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidInput_Get_Custom_PropSecondaryTouchEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_secondaryTouchEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_secondaryTouchEnabled)
    #if PLATFORM_ANDROID
    return IsInputDeviceEnabled((int)AINPUT_SOURCE_TOUCHPAD);
    #else
    return false;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidInput_Get_Custom_PropSecondaryTouchWidth()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidInput_Get_Custom_PropSecondaryTouchWidth)
    SCRIPTINGAPI_STACK_CHECK(get_secondaryTouchWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_secondaryTouchWidth)
    #if PLATFORM_ANDROID
    return GetTouchpadWidth();
    #else
    return 0;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidInput_Get_Custom_PropSecondaryTouchHeight()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidInput_Get_Custom_PropSecondaryTouchHeight)
    SCRIPTINGAPI_STACK_CHECK(get_secondaryTouchHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_secondaryTouchHeight)
    #if PLATFORM_ANDROID
    return GetTouchpadHeight();
    #else
    return 0;
    #endif
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if PLATFORM_ANDROID || UNITY_EDITOR
void Register_UnityEngine_AndroidInput_INTERNAL_CALL_GetSecondaryTouch()
{
    scripting_add_internal_call( "UnityEngine.AndroidInput::INTERNAL_CALL_GetSecondaryTouch" , (gpointer)& AndroidInput_CUSTOM_INTERNAL_CALL_GetSecondaryTouch );
}

void Register_UnityEngine_AndroidInput_get_touchCountSecondary()
{
    scripting_add_internal_call( "UnityEngine.AndroidInput::get_touchCountSecondary" , (gpointer)& AndroidInput_Get_Custom_PropTouchCountSecondary );
}

void Register_UnityEngine_AndroidInput_get_secondaryTouchEnabled()
{
    scripting_add_internal_call( "UnityEngine.AndroidInput::get_secondaryTouchEnabled" , (gpointer)& AndroidInput_Get_Custom_PropSecondaryTouchEnabled );
}

void Register_UnityEngine_AndroidInput_get_secondaryTouchWidth()
{
    scripting_add_internal_call( "UnityEngine.AndroidInput::get_secondaryTouchWidth" , (gpointer)& AndroidInput_Get_Custom_PropSecondaryTouchWidth );
}

void Register_UnityEngine_AndroidInput_get_secondaryTouchHeight()
{
    scripting_add_internal_call( "UnityEngine.AndroidInput::get_secondaryTouchHeight" , (gpointer)& AndroidInput_Get_Custom_PropSecondaryTouchHeight );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_AndroidInput_IcallNames [] =
{
#if PLATFORM_ANDROID || UNITY_EDITOR
    "UnityEngine.AndroidInput::INTERNAL_CALL_GetSecondaryTouch",    // -> AndroidInput_CUSTOM_INTERNAL_CALL_GetSecondaryTouch
    "UnityEngine.AndroidInput::get_touchCountSecondary",    // -> AndroidInput_Get_Custom_PropTouchCountSecondary
    "UnityEngine.AndroidInput::get_secondaryTouchEnabled",    // -> AndroidInput_Get_Custom_PropSecondaryTouchEnabled
    "UnityEngine.AndroidInput::get_secondaryTouchWidth",    // -> AndroidInput_Get_Custom_PropSecondaryTouchWidth
    "UnityEngine.AndroidInput::get_secondaryTouchHeight",    // -> AndroidInput_Get_Custom_PropSecondaryTouchHeight
#endif
    NULL
};

static const void* s_AndroidInput_IcallFuncs [] =
{
#if PLATFORM_ANDROID || UNITY_EDITOR
    (const void*)&AndroidInput_CUSTOM_INTERNAL_CALL_GetSecondaryTouch,  //  <- UnityEngine.AndroidInput::INTERNAL_CALL_GetSecondaryTouch
    (const void*)&AndroidInput_Get_Custom_PropTouchCountSecondary,  //  <- UnityEngine.AndroidInput::get_touchCountSecondary
    (const void*)&AndroidInput_Get_Custom_PropSecondaryTouchEnabled,  //  <- UnityEngine.AndroidInput::get_secondaryTouchEnabled
    (const void*)&AndroidInput_Get_Custom_PropSecondaryTouchWidth,  //  <- UnityEngine.AndroidInput::get_secondaryTouchWidth
    (const void*)&AndroidInput_Get_Custom_PropSecondaryTouchHeight,  //  <- UnityEngine.AndroidInput::get_secondaryTouchHeight
#endif
    NULL
};

void ExportAndroidInputBindings();
void ExportAndroidInputBindings()
{
    for (int i = 0; s_AndroidInput_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_AndroidInput_IcallNames [i], s_AndroidInput_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportAndroidInputBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if PLATFORM_ANDROID || UNITY_EDITOR
    SET_METRO_BINDING(AndroidInput_CUSTOM_INTERNAL_CALL_GetSecondaryTouch); //  <- UnityEngine.AndroidInput::INTERNAL_CALL_GetSecondaryTouch
    SET_METRO_BINDING(AndroidInput_Get_Custom_PropTouchCountSecondary); //  <- UnityEngine.AndroidInput::get_touchCountSecondary
    SET_METRO_BINDING(AndroidInput_Get_Custom_PropSecondaryTouchEnabled); //  <- UnityEngine.AndroidInput::get_secondaryTouchEnabled
    SET_METRO_BINDING(AndroidInput_Get_Custom_PropSecondaryTouchWidth); //  <- UnityEngine.AndroidInput::get_secondaryTouchWidth
    SET_METRO_BINDING(AndroidInput_Get_Custom_PropSecondaryTouchHeight); //  <- UnityEngine.AndroidInput::get_secondaryTouchHeight
#endif
}

#endif
