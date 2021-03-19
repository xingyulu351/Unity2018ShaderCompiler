#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Input/InputManager.h"
#include "Runtime/Input/TimeManager.h"
#include "Runtime/Misc/DeveloperConsole.h"
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Misc/DebugUtility.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Misc/DeveloperConsole.h"
#include "Runtime/Scripting/ScriptingUtility.h"

#if UNITY_EDITOR
	#include "Editor/Src/EditorSettings.h"
	#include "Editor/Src/EditorUserBuildSettings.h"
	#include "Editor/Mono/MonoEditorUtility.h"
	#include "Editor/Src/AssetPipeline/MonoCompilationPipeline.h"
#endif
	
using namespace Unity;

using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_INTERNAL_CALL_DrawLine(const Vector3f& start, const Vector3f& end, const ColorRGBAf& color, float duration, ScriptingBool depthTest)
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_INTERNAL_CALL_DrawLine)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawLine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawLine)
     DebugDrawLine (start, end, color, duration, depthTest); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_Break()
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_Break)
    SCRIPTINGAPI_STACK_CHECK(Break)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Break)
     PauseEditor (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_DebugBreak()
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_DebugBreak)
    SCRIPTINGAPI_STACK_CHECK(DebugBreak)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DebugBreak)
     
    		#if DEBUGMODE && (PLATFORM_WIN && !PLATFORM_WINRT)
    		if( IsDebuggerPresent() )
    			::DebugBreak();
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_Internal_Log(int level, ICallType_String_Argument msg_, ICallType_Object_Argument obj_)
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_Internal_Log)
    ICallType_String_Local msg(msg_);
    UNUSED(msg);
    ScriptingObjectOfType<Object> obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(Internal_Log)
    
    		DebugStringToFile (msg.ToUTF8().c_str(), 0, __FILE__, __LINE__, (level==0?kScriptingLog:level==1?kScriptingWarning:kScriptingError) | kMayIgnoreLineNumber, obj.GetInstanceID());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_Internal_LogException(ICallType_Exception_Argument exception_, ICallType_Object_Argument obj_)
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_Internal_LogException)
    ICallType_Exception_Local exception(exception_);
    UNUSED(exception);
    ScriptingObjectOfType<Object> obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(Internal_LogException)
    
    		LogException (exception, obj.GetInstanceID());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_ClearDeveloperConsole()
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_ClearDeveloperConsole)
    SCRIPTINGAPI_STACK_CHECK(ClearDeveloperConsole)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearDeveloperConsole)
     
    	#if UNITY_HAS_DEVELOPER_CONSOLE
    		if (GetDeveloperConsolePtr() != NULL) GetDeveloperConsolePtr()->Clear(); 
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Debug_Get_Custom_PropDeveloperConsoleVisible()
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_Get_Custom_PropDeveloperConsoleVisible)
    SCRIPTINGAPI_STACK_CHECK(get_developerConsoleVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_developerConsoleVisible)
    #if UNITY_HAS_DEVELOPER_CONSOLE
    if (GetDeveloperConsolePtr() != NULL) return GetDeveloperConsolePtr()->IsVisible();
    #endif
    return false;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_Set_Custom_PropDeveloperConsoleVisible(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_Set_Custom_PropDeveloperConsoleVisible)
    SCRIPTINGAPI_STACK_CHECK(set_developerConsoleVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_developerConsoleVisible)
    
    #if UNITY_HAS_DEVELOPER_CONSOLE
    if (GetDeveloperConsolePtr() != NULL) return GetDeveloperConsolePtr()->SetOpen(value);
    #endif
    
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_LogPlayerBuildError(ICallType_String_Argument message_, ICallType_String_Argument file_, int line, int column)
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_LogPlayerBuildError)
    ICallType_String_Local message(message_);
    UNUSED(message);
    ICallType_String_Local file(file_);
    UNUSED(file);
    SCRIPTINGAPI_STACK_CHECK(LogPlayerBuildError)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LogPlayerBuildError)
     
    		DebugStringToFilePostprocessedStacktrace (message.ToUTF8().c_str(), "", "", 1, file.ToUTF8().c_str(), line, kScriptingError | kDontExtractStacktrace, 0, GetBuildErrorIdentifier());
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Debug_Get_Custom_PropIsDebugBuild()
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_Get_Custom_PropIsDebugBuild)
    SCRIPTINGAPI_STACK_CHECK(get_isDebugBuild)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isDebugBuild)
    return GetBuildSettings().isDebugBuild;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Debug_CUSTOM_OpenConsoleFile()
{
    SCRIPTINGAPI_ETW_ENTRY(Debug_CUSTOM_OpenConsoleFile)
    SCRIPTINGAPI_STACK_CHECK(OpenConsoleFile)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(OpenConsoleFile)
     DeveloperConsole_OpenConsoleFile(); 
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Debug_INTERNAL_CALL_DrawLine()
{
    scripting_add_internal_call( "UnityEngine.Debug::INTERNAL_CALL_DrawLine" , (gpointer)& Debug_CUSTOM_INTERNAL_CALL_DrawLine );
}

void Register_UnityEngine_Debug_Break()
{
    scripting_add_internal_call( "UnityEngine.Debug::Break" , (gpointer)& Debug_CUSTOM_Break );
}

void Register_UnityEngine_Debug_DebugBreak()
{
    scripting_add_internal_call( "UnityEngine.Debug::DebugBreak" , (gpointer)& Debug_CUSTOM_DebugBreak );
}

void Register_UnityEngine_Debug_Internal_Log()
{
    scripting_add_internal_call( "UnityEngine.Debug::Internal_Log" , (gpointer)& Debug_CUSTOM_Internal_Log );
}

void Register_UnityEngine_Debug_Internal_LogException()
{
    scripting_add_internal_call( "UnityEngine.Debug::Internal_LogException" , (gpointer)& Debug_CUSTOM_Internal_LogException );
}

void Register_UnityEngine_Debug_ClearDeveloperConsole()
{
    scripting_add_internal_call( "UnityEngine.Debug::ClearDeveloperConsole" , (gpointer)& Debug_CUSTOM_ClearDeveloperConsole );
}

void Register_UnityEngine_Debug_get_developerConsoleVisible()
{
    scripting_add_internal_call( "UnityEngine.Debug::get_developerConsoleVisible" , (gpointer)& Debug_Get_Custom_PropDeveloperConsoleVisible );
}

void Register_UnityEngine_Debug_set_developerConsoleVisible()
{
    scripting_add_internal_call( "UnityEngine.Debug::set_developerConsoleVisible" , (gpointer)& Debug_Set_Custom_PropDeveloperConsoleVisible );
}

#if UNITY_EDITOR
void Register_UnityEngine_Debug_LogPlayerBuildError()
{
    scripting_add_internal_call( "UnityEngine.Debug::LogPlayerBuildError" , (gpointer)& Debug_CUSTOM_LogPlayerBuildError );
}

#endif
void Register_UnityEngine_Debug_get_isDebugBuild()
{
    scripting_add_internal_call( "UnityEngine.Debug::get_isDebugBuild" , (gpointer)& Debug_Get_Custom_PropIsDebugBuild );
}

void Register_UnityEngine_Debug_OpenConsoleFile()
{
    scripting_add_internal_call( "UnityEngine.Debug::OpenConsoleFile" , (gpointer)& Debug_CUSTOM_OpenConsoleFile );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineDebug_IcallNames [] =
{
    "UnityEngine.Debug::INTERNAL_CALL_DrawLine",    // -> Debug_CUSTOM_INTERNAL_CALL_DrawLine
    "UnityEngine.Debug::Break"              ,    // -> Debug_CUSTOM_Break
    "UnityEngine.Debug::DebugBreak"         ,    // -> Debug_CUSTOM_DebugBreak
    "UnityEngine.Debug::Internal_Log"       ,    // -> Debug_CUSTOM_Internal_Log
    "UnityEngine.Debug::Internal_LogException",    // -> Debug_CUSTOM_Internal_LogException
    "UnityEngine.Debug::ClearDeveloperConsole",    // -> Debug_CUSTOM_ClearDeveloperConsole
    "UnityEngine.Debug::get_developerConsoleVisible",    // -> Debug_Get_Custom_PropDeveloperConsoleVisible
    "UnityEngine.Debug::set_developerConsoleVisible",    // -> Debug_Set_Custom_PropDeveloperConsoleVisible
#if UNITY_EDITOR
    "UnityEngine.Debug::LogPlayerBuildError",    // -> Debug_CUSTOM_LogPlayerBuildError
#endif
    "UnityEngine.Debug::get_isDebugBuild"   ,    // -> Debug_Get_Custom_PropIsDebugBuild
    "UnityEngine.Debug::OpenConsoleFile"    ,    // -> Debug_CUSTOM_OpenConsoleFile
    NULL
};

static const void* s_UnityEngineDebug_IcallFuncs [] =
{
    (const void*)&Debug_CUSTOM_INTERNAL_CALL_DrawLine     ,  //  <- UnityEngine.Debug::INTERNAL_CALL_DrawLine
    (const void*)&Debug_CUSTOM_Break                      ,  //  <- UnityEngine.Debug::Break
    (const void*)&Debug_CUSTOM_DebugBreak                 ,  //  <- UnityEngine.Debug::DebugBreak
    (const void*)&Debug_CUSTOM_Internal_Log               ,  //  <- UnityEngine.Debug::Internal_Log
    (const void*)&Debug_CUSTOM_Internal_LogException      ,  //  <- UnityEngine.Debug::Internal_LogException
    (const void*)&Debug_CUSTOM_ClearDeveloperConsole      ,  //  <- UnityEngine.Debug::ClearDeveloperConsole
    (const void*)&Debug_Get_Custom_PropDeveloperConsoleVisible,  //  <- UnityEngine.Debug::get_developerConsoleVisible
    (const void*)&Debug_Set_Custom_PropDeveloperConsoleVisible,  //  <- UnityEngine.Debug::set_developerConsoleVisible
#if UNITY_EDITOR
    (const void*)&Debug_CUSTOM_LogPlayerBuildError        ,  //  <- UnityEngine.Debug::LogPlayerBuildError
#endif
    (const void*)&Debug_Get_Custom_PropIsDebugBuild       ,  //  <- UnityEngine.Debug::get_isDebugBuild
    (const void*)&Debug_CUSTOM_OpenConsoleFile            ,  //  <- UnityEngine.Debug::OpenConsoleFile
    NULL
};

void ExportUnityEngineDebugBindings();
void ExportUnityEngineDebugBindings()
{
    for (int i = 0; s_UnityEngineDebug_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineDebug_IcallNames [i], s_UnityEngineDebug_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineDebugBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Debug_CUSTOM_INTERNAL_CALL_DrawLine); //  <- UnityEngine.Debug::INTERNAL_CALL_DrawLine
    SET_METRO_BINDING(Debug_CUSTOM_Break); //  <- UnityEngine.Debug::Break
    SET_METRO_BINDING(Debug_CUSTOM_DebugBreak); //  <- UnityEngine.Debug::DebugBreak
    SET_METRO_BINDING(Debug_CUSTOM_Internal_Log); //  <- UnityEngine.Debug::Internal_Log
    SET_METRO_BINDING(Debug_CUSTOM_Internal_LogException); //  <- UnityEngine.Debug::Internal_LogException
    SET_METRO_BINDING(Debug_CUSTOM_ClearDeveloperConsole); //  <- UnityEngine.Debug::ClearDeveloperConsole
    SET_METRO_BINDING(Debug_Get_Custom_PropDeveloperConsoleVisible); //  <- UnityEngine.Debug::get_developerConsoleVisible
    SET_METRO_BINDING(Debug_Set_Custom_PropDeveloperConsoleVisible); //  <- UnityEngine.Debug::set_developerConsoleVisible
#if UNITY_EDITOR
    SET_METRO_BINDING(Debug_CUSTOM_LogPlayerBuildError); //  <- UnityEngine.Debug::LogPlayerBuildError
#endif
    SET_METRO_BINDING(Debug_Get_Custom_PropIsDebugBuild); //  <- UnityEngine.Debug::get_isDebugBuild
    SET_METRO_BINDING(Debug_CUSTOM_OpenConsoleFile); //  <- UnityEngine.Debug::OpenConsoleFile
}

#endif
