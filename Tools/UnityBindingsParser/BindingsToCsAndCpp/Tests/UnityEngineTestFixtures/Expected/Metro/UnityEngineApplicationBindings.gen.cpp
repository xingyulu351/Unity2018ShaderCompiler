#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Input/InputManager.h"
#include "Runtime/Input/TimeManager.h"
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/BaseClasses/Tags.h"
#include "Runtime/Misc/DebugUtility.h"
#include "Runtime/Misc/PlayerSettings.h"
#include "Runtime/GameCode/CloneObject.h"
#include "Runtime/Math/Random/Random.h"
#include "Runtime/Misc/PreloadManager.h"
#include "Runtime/Animation/Animation.h"
#include "Runtime/Math/Color.h"
#include "Runtime/Utilities/PlayerPrefs.h"
#include "Runtime/Camera/Camera.h"
#include "Runtime/Dynamics/Rigidbody.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Filters/Misc/TextMesh.h"
#include "Runtime/Dynamics/ConstantForce.h"
#include "Runtime/Filters/Renderer.h"
#include "Runtime/Misc/SaveAndLoadHelper.h"
#include "Runtime/Network/NetworkView.h"
#include "Runtime/Camera/RenderLayers/GUIText.h"
#include "Runtime/Camera/RenderLayers/GUITexture.h"
#include "Runtime/Dynamics/Collider.h"
#include "Runtime/Dynamics/HingeJoint.h"
#include "Runtime/Misc/AssetBundleUtility.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/BaseClasses/IsPlaying.h"
#include "Runtime/Misc/CaptureScreenshot.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Misc/Plugins.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/File.h"
#include "Runtime/Network/NetworkManager.h"
#include <ctime>
#include "Runtime/Input/GetInput.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Animation/AnimationManager.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/BaseClasses/RefCounted.h"
#include "Runtime/Misc/GOCreation.h"
#include "Runtime/Utilities/URLUtility.h"
#include "Runtime/Graphics/ScreenManager.h"
#include "Runtime/Serialize/PersistentManager.h"
#include "Runtime/Shaders/GraphicsCaps.h"
#include "Runtime/Misc/SystemInfo.h"
#include "Runtime/Utilities/FileUtilities.h"
#include "Runtime/Misc/GraphicsDevicesDB.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include "Runtime/File/ApplicationSpecificPersistentDataPath.h"
#include "Runtime/Mono/Coroutine.h"
#include "Runtime/Utilities/UserAuthorizationManager.h"
#if WEBPLUG
#include "PlatformDependent/CommonWebPlugin/UnityWebStream.h"
#include "PlatformDependent/CommonWebPlugin/WebScripting.h"
#endif
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingManager.h"

#if UNITY_EDITOR
	#include "Editor/Src/EditorSettings.h"
	#include "Editor/Src/EditorUserBuildSettings.h"
	#include "Editor/Mono/MonoEditorUtility.h"
#endif

#if PLATFORM_ANDROID
	#include "PlatformDependent/AndroidPlayer/EntryPoint.h"
#endif



using namespace Unity;
using namespace std;


SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_Quit()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_Quit)
    SCRIPTINGAPI_STACK_CHECK(Quit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Quit)
    
    		GetInputManager ().QuitApplication ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_CancelQuit()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_CancelQuit)
    SCRIPTINGAPI_STACK_CHECK(CancelQuit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CancelQuit)
    
    		GetInputManager ().CancelQuitApplication ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_Get_Custom_PropLoadedLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropLoadedLevel)
    SCRIPTINGAPI_STACK_CHECK(get_loadedLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_loadedLevel)
    return PlayerGetLoadedSceneIndex ();
}


SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropLoadedLevelName()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropLoadedLevelName)
    SCRIPTINGAPI_STACK_CHECK(get_loadedLevelName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_loadedLevelName)
    return CreateScriptingString(PlayerGetLoadedSceneName());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Application_CUSTOM_LoadLevelAsync(ICallType_String_Argument monoLevelName_, int index, ScriptingBool additive, ScriptingBool mustCompleteNextFrame)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_LoadLevelAsync)
    ICallType_String_Local monoLevelName(monoLevelName_);
    UNUSED(monoLevelName);
    SCRIPTINGAPI_STACK_CHECK(LoadLevelAsync)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadLevelAsync)
    
    		string levelName = monoLevelName;
    		string levelPath;
    		string assetPath;
    
    		if (!GetSceneAndAssetPath (levelName, index, &levelPath, &assetPath, &index))
    			return SCRIPTING_NULL;
    
    		LoadSceneOperation::LoadingMode mode = additive ? LoadSceneOperation::kLoadSceneAdditive : LoadSceneOperation::kLoadScene;
    		AsyncOperation* result = LoadSceneOperation::LoadScene(levelPath, assetPath, index, mode, mustCompleteNextFrame);
    		#if UNITY_FLASH
    		
    		result->Release();
    		
    		return SCRIPTING_NULL;
    		#else
    		ScriptingObjectPtr o = scripting_object_new(GetCommonScriptingClasses ().asyncOperation);
    		ScriptingObjectWithIntPtrField<AsyncOperation>(o).SetPtr(result);
    		return o;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropIsLoadingLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropIsLoadingLevel)
    SCRIPTINGAPI_STACK_CHECK(get_isLoadingLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isLoadingLevel)
    return IsLoadingScene();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_Get_Custom_PropLevelCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropLevelCount)
    SCRIPTINGAPI_STACK_CHECK(get_levelCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_levelCount)
    return PlayerGetSceneCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Application_CUSTOM_GetStreamProgressForLevelByName(ICallType_String_Argument levelName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_GetStreamProgressForLevelByName)
    ICallType_String_Local levelName(levelName_);
    UNUSED(levelName);
    SCRIPTINGAPI_STACK_CHECK(GetStreamProgressForLevelByName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetStreamProgressForLevelByName)
    
    		#if WEBPLUG
    		return GetUnityWebStream().GetProgressForScene(GetBuildSettings().GetSceneIndexChecked (levelName));
    		#else
    		return 1.0F;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Application_CUSTOM_GetStreamProgressForLevel(int levelIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_GetStreamProgressForLevel)
    SCRIPTINGAPI_STACK_CHECK(GetStreamProgressForLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetStreamProgressForLevel)
    
    		#if WEBPLUG
    		return GetUnityWebStream().GetProgressForScene(levelIndex);
    		#else
    		if (levelIndex >= 0 && levelIndex < GetBuildSettings().levels.size())
    			return 1.0F;
    		else
    			return 0.0F;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_Get_Custom_PropStreamedBytes()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropStreamedBytes)
    SCRIPTINGAPI_STACK_CHECK(get_streamedBytes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_streamedBytes)
    #if WEBPLUG
    return GetUnityWebStream().GetDownloadedBytes();
    #else
    return 0;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_CUSTOM_CanStreamedLevelBeLoadedByName(ICallType_String_Argument levelName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_CanStreamedLevelBeLoadedByName)
    ICallType_String_Local levelName(levelName_);
    UNUSED(levelName);
    SCRIPTINGAPI_STACK_CHECK(CanStreamedLevelBeLoadedByName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CanStreamedLevelBeLoadedByName)
    
    		string cppName = levelName;
    		#if WEBPLUG
    		return GetUnityWebStream().CanSceneBeLoaded(GetBuildSettings().GetSceneIndex (cppName)) || GetHasLateBoundSceneFromAssetBundle(cppName);
    		#else
    		return GetBuildSettings().GetSceneIndex (cppName) != -1 || GetHasLateBoundSceneFromAssetBundle(cppName);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_CUSTOM_CanStreamedLevelBeLoaded(int levelIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_CanStreamedLevelBeLoaded)
    SCRIPTINGAPI_STACK_CHECK(CanStreamedLevelBeLoaded)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CanStreamedLevelBeLoaded)
    
    		#if WEBPLUG
    		return GetUnityWebStream().CanSceneBeLoaded(levelIndex);
    		#else
    		return levelIndex >= 0 && levelIndex < GetBuildSettings().levels.size();
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropIsPlaying()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropIsPlaying)
    SCRIPTINGAPI_STACK_CHECK(get_isPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPlaying)
    return IsWorldPlaying ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropIsEditor()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropIsEditor)
    SCRIPTINGAPI_STACK_CHECK(get_isEditor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isEditor)
    #if UNITY_EDITOR
    return true;
    #else
    return false;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropIsWebPlayer()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropIsWebPlayer)
    SCRIPTINGAPI_STACK_CHECK(get_isWebPlayer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isWebPlayer)
    int platform = systeminfo::GetRuntimePlatform();
    return platform == WindowsWebPlayer || platform == OSXWebPlayer;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_Get_Custom_PropPlatform()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropPlatform)
    SCRIPTINGAPI_STACK_CHECK(get_platform)
    return systeminfo::GetRuntimePlatform();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_CaptureScreenshot(ICallType_String_Argument filename_, int superSize)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_CaptureScreenshot)
    ICallType_String_Local filename(filename_);
    UNUSED(filename);
    SCRIPTINGAPI_STACK_CHECK(CaptureScreenshot)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CaptureScreenshot)
    
    		#if CAPTURE_SCREENSHOT_AVAILABLE
    		QueueScreenshot (filename, superSize);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropRunInBackground()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropRunInBackground)
    SCRIPTINGAPI_STACK_CHECK(get_runInBackground)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_runInBackground)
    return GetPlayerRunInBackground();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_Set_Custom_PropRunInBackground(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Set_Custom_PropRunInBackground)
    SCRIPTINGAPI_STACK_CHECK(set_runInBackground)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_runInBackground)
     SetPlayerRunInBackground(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_CUSTOM_HasProLicense()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_HasProLicense)
    SCRIPTINGAPI_STACK_CHECK(HasProLicense)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasProLicense)
    
    		return GetBuildSettings().hasPROVersion;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_CUSTOM_HasAdvancedLicense()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_HasAdvancedLicense)
    SCRIPTINGAPI_STACK_CHECK(HasAdvancedLicense)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasAdvancedLicense)
    
    		return GetBuildSettings().hasAdvancedVersion;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_DontDestroyOnLoad(ICallType_ReadOnlyUnityEngineObject_Argument mono_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_DontDestroyOnLoad)
    ReadOnlyScriptingObjectOfType<Object> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(DontDestroyOnLoad)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DontDestroyOnLoad)
    
    		Object* o = mono;
    		if (o)
    			DontDestroyOnLoad (*o);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropDataPath()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropDataPath)
    SCRIPTINGAPI_STACK_CHECK(get_dataPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dataPath)
    return CreateScriptingString ( GetAppDataPath() );
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropStreamingAssetsPath()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropStreamingAssetsPath)
    SCRIPTINGAPI_STACK_CHECK(get_streamingAssetsPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_streamingAssetsPath)
    return CreateScriptingString ( GetStreamingAssetsPath() );
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropPersistentDataPath()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropPersistentDataPath)
    SCRIPTINGAPI_STACK_CHECK(get_persistentDataPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_persistentDataPath)
    return CreateScriptingString ( GetPersistentDataPathApplicationSpecific() );
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropTemporaryCachePath()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropTemporaryCachePath)
    SCRIPTINGAPI_STACK_CHECK(get_temporaryCachePath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_temporaryCachePath)
    return CreateScriptingString ( GetTemporaryCachePathApplicationSpecific() );
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropSrcValue()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropSrcValue)
    SCRIPTINGAPI_STACK_CHECK(get_srcValue)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_srcValue)
    return CreateScriptingString (GetPlayerSettings().srcValue);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropAbsoluteURL()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropAbsoluteURL)
    SCRIPTINGAPI_STACK_CHECK(get_absoluteURL)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_absoluteURL)
    return CreateScriptingString (GetPlayerSettings().absoluteURL);
}

#if UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_Internal_ExternalCall_Flash(ICallType_String_Argument script_, ICallType_Object_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_Internal_ExternalCall_Flash)
    ICallType_String_Local script(script_);
    UNUSED(script);
    ICallType_Object_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(Internal_ExternalCall_Flash)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ExternalCall_Flash)
    
    		ScriptingObjectPtr arg = args;
    		if(args){
    			Ext_ExternalCall(script.ToUTF8().c_str(), arg);
    		}
    	
}

#endif
#if ENABLE_MONO
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_Internal_ExternalCall(ICallType_String_Argument script_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_Internal_ExternalCall)
    ICallType_String_Local script(script_);
    UNUSED(script);
    SCRIPTINGAPI_STACK_CHECK(Internal_ExternalCall)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ExternalCall)
    
    		#if WEBPLUG
    		WebScripting::Get().ExternalCall(script);
    		#else
    		LogString(Format ("External Call: %s", script.ToUTF8().c_str()));
    		#endif
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropUnityVersion()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropUnityVersion)
    SCRIPTINGAPI_STACK_CHECK(get_unityVersion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_unityVersion)
    return CreateScriptingString(UNITY_VERSION);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_CUSTOM_GetBuildUnityVersion()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_GetBuildUnityVersion)
    SCRIPTINGAPI_STACK_CHECK(GetBuildUnityVersion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBuildUnityVersion)
    
    		return GetBuildSettings().GetIntVersion();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_CUSTOM_GetNumericUnityVersion(ICallType_String_Argument version_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_GetNumericUnityVersion)
    ICallType_String_Local version(version_);
    UNUSED(version);
    SCRIPTINGAPI_STACK_CHECK(GetNumericUnityVersion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNumericUnityVersion)
    
    		return GetNumericVersion (version);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropWebSecurityEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropWebSecurityEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_webSecurityEnabled)
    #if WEBPLUG
    return true;
    #endif
    #if UNITY_EDITOR
    return (GetBuildTargetGroup( GetEditorUserBuildSettings().GetActiveBuildTarget ()) == kPlatformWebPlayer);
    #endif
    return false;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Application_Get_Custom_PropWebSecurityHostUrl()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropWebSecurityHostUrl)
    SCRIPTINGAPI_STACK_CHECK(get_webSecurityHostUrl)
    #if UNITY_EDITOR
    return CreateScriptingString(GetEditorSettings().GetWebSecurityEmulationHostUrl());
    #elif WEBPLUG
    return CreateScriptingString(GetPlayerSettings().absoluteURL);
    #else
    return CreateScriptingString("");
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_OpenURL(ICallType_String_Argument url_)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_OpenURL)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(OpenURL)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(OpenURL)
    
    	#if WEBPLUG
    	#if PLATFORM_OSX
    		if (systeminfo::IsRunningInDashboardWidget ())
    			WebScripting::Get().ExternalCall("goToUrl(\"" + url.ToUTF8() + "\");");
    		else
    	#endif
    			WebScripting::Get().ExternalCall("location.href=\"" + url.ToUTF8() + "\";");
    	#elif UNITY_FLASH
    		Ext_OpenURL(url.ToUTF8().c_str());
    	#elif UNITY_WP8
    		printf("ERROR: OpenURL not supported\n"); exit(-1);
    	#else
    		OpenURL (url);
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_CommitSuicide(int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_CommitSuicide)
    SCRIPTINGAPI_STACK_CHECK(CommitSuicide)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CommitSuicide)
    
    		if (mode == 0)
    		{
    			printf_console("Committing suicide -- Intentionally Dereferencing NULL pointer\n");
    			int* p = NULL;
    			*p = 5;
    		}
    		else if (mode == 1)
    		{
    			FatalErrorString("Intentionally caused fatal error");
    		}
    		else if (mode == 2)
    		{
    			abort();
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_Get_Custom_PropTargetFrameRate()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropTargetFrameRate)
    SCRIPTINGAPI_STACK_CHECK(get_targetFrameRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_targetFrameRate)
    return GetTargetFrameRateFromScripting();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_Set_Custom_PropTargetFrameRate(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Set_Custom_PropTargetFrameRate)
    SCRIPTINGAPI_STACK_CHECK(set_targetFrameRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_targetFrameRate)
    
    SetTargetFrameRate(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
SystemLanguage SCRIPT_CALL_CONVENTION Application_Get_Custom_PropSystemLanguage()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropSystemLanguage)
    SCRIPTINGAPI_STACK_CHECK(get_systemLanguage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_systemLanguage)
    return (SystemLanguage)systeminfo::GetSystemLanguage();
}


 static void LogCallbackImplementation(const std::string& condition, const std::string &stackTrace, int type)
	{
#if ENABLE_MONO
		if ( !GetMonoManagerPtr() )
			return;
		
		ScriptingInvocation invocation(GetCommonScriptingClasses ().callLogCallback);
		invocation.AddString(condition.c_str());
		invocation.AddString(stackTrace.c_str());
		invocation.AddInt(type);
		invocation.Invoke<ScriptingObjectPtr>();
#endif
	}
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_SetLogCallbackDefined(ScriptingBool defined, ScriptingBool threaded)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_SetLogCallbackDefined)
    SCRIPTINGAPI_STACK_CHECK(SetLogCallbackDefined)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLogCallbackDefined)
    
    		RegisterLogCallback(defined ? LogCallbackImplementation : NULL, threaded);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ThreadPriority SCRIPT_CALL_CONVENTION Application_Get_Custom_PropBackgroundLoadingPriority()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropBackgroundLoadingPriority)
    SCRIPTINGAPI_STACK_CHECK(get_backgroundLoadingPriority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_backgroundLoadingPriority)
    return GetPreloadManager().GetThreadPriority();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_Set_Custom_PropBackgroundLoadingPriority(ThreadPriority value)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Set_Custom_PropBackgroundLoadingPriority)
    SCRIPTINGAPI_STACK_CHECK(set_backgroundLoadingPriority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_backgroundLoadingPriority)
    
    GetPreloadManager().SetThreadPriority(value);
    
}


SCRIPT_BINDINGS_EXPORT_DECL
NetworkReachability SCRIPT_CALL_CONVENTION Application_Get_Custom_PropInternetReachability()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropInternetReachability)
    SCRIPTINGAPI_STACK_CHECK(get_internetReachability)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_internetReachability)
    return GetInternetReachability ();
}



SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropGenuine()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropGenuine)
    SCRIPTINGAPI_STACK_CHECK(get_genuine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_genuine)
    return IsApplicationGenuine ();
}


SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_Get_Custom_PropGenuineCheckAvailable()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_Get_Custom_PropGenuineCheckAvailable)
    SCRIPTINGAPI_STACK_CHECK(get_genuineCheckAvailable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_genuineCheckAvailable)
    return IsApplicationGenuineAvailable ();
}


	
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Application_CUSTOM_RequestUserAuthorization(int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_RequestUserAuthorization)
    SCRIPTINGAPI_STACK_CHECK(RequestUserAuthorization)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RequestUserAuthorization)
    
    		#if ENABLE_MONO
    		AsyncOperation* result = GetUserAuthorizationManager().RequestUserAuthorization (mode);
    		MonoObject* mono = mono_object_new(mono_domain_get(), GetCommonScriptingClasses ().asyncOperation);
    		ExtractMonoObjectData<AsyncOperation*>(mono) = result;
    		return mono;
    		#else
    		return SCRIPTING_NULL;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Application_CUSTOM_HasUserAuthorization(int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_HasUserAuthorization)
    SCRIPTINGAPI_STACK_CHECK(HasUserAuthorization)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasUserAuthorization)
    
    		return GetUserAuthorizationManager().HasUserAuthorization (mode);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Application_CUSTOM_ReplyToUserAuthorizationRequest(ScriptingBool reply, ScriptingBool remember)
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_ReplyToUserAuthorizationRequest)
    SCRIPTINGAPI_STACK_CHECK(ReplyToUserAuthorizationRequest)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReplyToUserAuthorizationRequest)
    
    		return GetUserAuthorizationManager().ReplyToUserAuthorizationRequest (reply, remember);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Application_CUSTOM_GetUserAuthorizationRequestMode_Internal()
{
    SCRIPTINGAPI_ETW_ENTRY(Application_CUSTOM_GetUserAuthorizationRequestMode_Internal)
    SCRIPTINGAPI_STACK_CHECK(GetUserAuthorizationRequestMode_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetUserAuthorizationRequestMode_Internal)
    
    		return GetUserAuthorizationManager().GetAuthorizationRequest();
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Application_Quit()
{
    scripting_add_internal_call( "UnityEngine.Application::Quit" , (gpointer)& Application_CUSTOM_Quit );
}

void Register_UnityEngine_Application_CancelQuit()
{
    scripting_add_internal_call( "UnityEngine.Application::CancelQuit" , (gpointer)& Application_CUSTOM_CancelQuit );
}

void Register_UnityEngine_Application_get_loadedLevel()
{
    scripting_add_internal_call( "UnityEngine.Application::get_loadedLevel" , (gpointer)& Application_Get_Custom_PropLoadedLevel );
}

void Register_UnityEngine_Application_get_loadedLevelName()
{
    scripting_add_internal_call( "UnityEngine.Application::get_loadedLevelName" , (gpointer)& Application_Get_Custom_PropLoadedLevelName );
}

void Register_UnityEngine_Application_LoadLevelAsync()
{
    scripting_add_internal_call( "UnityEngine.Application::LoadLevelAsync" , (gpointer)& Application_CUSTOM_LoadLevelAsync );
}

void Register_UnityEngine_Application_get_isLoadingLevel()
{
    scripting_add_internal_call( "UnityEngine.Application::get_isLoadingLevel" , (gpointer)& Application_Get_Custom_PropIsLoadingLevel );
}

void Register_UnityEngine_Application_get_levelCount()
{
    scripting_add_internal_call( "UnityEngine.Application::get_levelCount" , (gpointer)& Application_Get_Custom_PropLevelCount );
}

void Register_UnityEngine_Application_GetStreamProgressForLevelByName()
{
    scripting_add_internal_call( "UnityEngine.Application::GetStreamProgressForLevelByName" , (gpointer)& Application_CUSTOM_GetStreamProgressForLevelByName );
}

void Register_UnityEngine_Application_GetStreamProgressForLevel()
{
    scripting_add_internal_call( "UnityEngine.Application::GetStreamProgressForLevel" , (gpointer)& Application_CUSTOM_GetStreamProgressForLevel );
}

void Register_UnityEngine_Application_get_streamedBytes()
{
    scripting_add_internal_call( "UnityEngine.Application::get_streamedBytes" , (gpointer)& Application_Get_Custom_PropStreamedBytes );
}

void Register_UnityEngine_Application_CanStreamedLevelBeLoadedByName()
{
    scripting_add_internal_call( "UnityEngine.Application::CanStreamedLevelBeLoadedByName" , (gpointer)& Application_CUSTOM_CanStreamedLevelBeLoadedByName );
}

void Register_UnityEngine_Application_CanStreamedLevelBeLoaded()
{
    scripting_add_internal_call( "UnityEngine.Application::CanStreamedLevelBeLoaded" , (gpointer)& Application_CUSTOM_CanStreamedLevelBeLoaded );
}

void Register_UnityEngine_Application_get_isPlaying()
{
    scripting_add_internal_call( "UnityEngine.Application::get_isPlaying" , (gpointer)& Application_Get_Custom_PropIsPlaying );
}

void Register_UnityEngine_Application_get_isEditor()
{
    scripting_add_internal_call( "UnityEngine.Application::get_isEditor" , (gpointer)& Application_Get_Custom_PropIsEditor );
}

void Register_UnityEngine_Application_get_isWebPlayer()
{
    scripting_add_internal_call( "UnityEngine.Application::get_isWebPlayer" , (gpointer)& Application_Get_Custom_PropIsWebPlayer );
}

void Register_UnityEngine_Application_get_platform()
{
    scripting_add_internal_call( "UnityEngine.Application::get_platform" , (gpointer)& Application_Get_Custom_PropPlatform );
}

void Register_UnityEngine_Application_CaptureScreenshot()
{
    scripting_add_internal_call( "UnityEngine.Application::CaptureScreenshot" , (gpointer)& Application_CUSTOM_CaptureScreenshot );
}

void Register_UnityEngine_Application_get_runInBackground()
{
    scripting_add_internal_call( "UnityEngine.Application::get_runInBackground" , (gpointer)& Application_Get_Custom_PropRunInBackground );
}

void Register_UnityEngine_Application_set_runInBackground()
{
    scripting_add_internal_call( "UnityEngine.Application::set_runInBackground" , (gpointer)& Application_Set_Custom_PropRunInBackground );
}

void Register_UnityEngine_Application_HasProLicense()
{
    scripting_add_internal_call( "UnityEngine.Application::HasProLicense" , (gpointer)& Application_CUSTOM_HasProLicense );
}

void Register_UnityEngine_Application_HasAdvancedLicense()
{
    scripting_add_internal_call( "UnityEngine.Application::HasAdvancedLicense" , (gpointer)& Application_CUSTOM_HasAdvancedLicense );
}

void Register_UnityEngine_Application_DontDestroyOnLoad()
{
    scripting_add_internal_call( "UnityEngine.Application::DontDestroyOnLoad" , (gpointer)& Application_CUSTOM_DontDestroyOnLoad );
}

void Register_UnityEngine_Application_get_dataPath()
{
    scripting_add_internal_call( "UnityEngine.Application::get_dataPath" , (gpointer)& Application_Get_Custom_PropDataPath );
}

void Register_UnityEngine_Application_get_streamingAssetsPath()
{
    scripting_add_internal_call( "UnityEngine.Application::get_streamingAssetsPath" , (gpointer)& Application_Get_Custom_PropStreamingAssetsPath );
}

void Register_UnityEngine_Application_get_persistentDataPath()
{
    scripting_add_internal_call( "UnityEngine.Application::get_persistentDataPath" , (gpointer)& Application_Get_Custom_PropPersistentDataPath );
}

void Register_UnityEngine_Application_get_temporaryCachePath()
{
    scripting_add_internal_call( "UnityEngine.Application::get_temporaryCachePath" , (gpointer)& Application_Get_Custom_PropTemporaryCachePath );
}

void Register_UnityEngine_Application_get_srcValue()
{
    scripting_add_internal_call( "UnityEngine.Application::get_srcValue" , (gpointer)& Application_Get_Custom_PropSrcValue );
}

void Register_UnityEngine_Application_get_absoluteURL()
{
    scripting_add_internal_call( "UnityEngine.Application::get_absoluteURL" , (gpointer)& Application_Get_Custom_PropAbsoluteURL );
}

#if UNITY_FLASH
void Register_UnityEngine_Application_Internal_ExternalCall_Flash()
{
    scripting_add_internal_call( "UnityEngine.Application::Internal_ExternalCall_Flash" , (gpointer)& Application_CUSTOM_Internal_ExternalCall_Flash );
}

#endif
#if ENABLE_MONO
void Register_UnityEngine_Application_Internal_ExternalCall()
{
    scripting_add_internal_call( "UnityEngine.Application::Internal_ExternalCall" , (gpointer)& Application_CUSTOM_Internal_ExternalCall );
}

#endif
void Register_UnityEngine_Application_get_unityVersion()
{
    scripting_add_internal_call( "UnityEngine.Application::get_unityVersion" , (gpointer)& Application_Get_Custom_PropUnityVersion );
}

void Register_UnityEngine_Application_GetBuildUnityVersion()
{
    scripting_add_internal_call( "UnityEngine.Application::GetBuildUnityVersion" , (gpointer)& Application_CUSTOM_GetBuildUnityVersion );
}

void Register_UnityEngine_Application_GetNumericUnityVersion()
{
    scripting_add_internal_call( "UnityEngine.Application::GetNumericUnityVersion" , (gpointer)& Application_CUSTOM_GetNumericUnityVersion );
}

void Register_UnityEngine_Application_get_webSecurityEnabled()
{
    scripting_add_internal_call( "UnityEngine.Application::get_webSecurityEnabled" , (gpointer)& Application_Get_Custom_PropWebSecurityEnabled );
}

void Register_UnityEngine_Application_get_webSecurityHostUrl()
{
    scripting_add_internal_call( "UnityEngine.Application::get_webSecurityHostUrl" , (gpointer)& Application_Get_Custom_PropWebSecurityHostUrl );
}

void Register_UnityEngine_Application_OpenURL()
{
    scripting_add_internal_call( "UnityEngine.Application::OpenURL" , (gpointer)& Application_CUSTOM_OpenURL );
}

void Register_UnityEngine_Application_CommitSuicide()
{
    scripting_add_internal_call( "UnityEngine.Application::CommitSuicide" , (gpointer)& Application_CUSTOM_CommitSuicide );
}

void Register_UnityEngine_Application_get_targetFrameRate()
{
    scripting_add_internal_call( "UnityEngine.Application::get_targetFrameRate" , (gpointer)& Application_Get_Custom_PropTargetFrameRate );
}

void Register_UnityEngine_Application_set_targetFrameRate()
{
    scripting_add_internal_call( "UnityEngine.Application::set_targetFrameRate" , (gpointer)& Application_Set_Custom_PropTargetFrameRate );
}

void Register_UnityEngine_Application_get_systemLanguage()
{
    scripting_add_internal_call( "UnityEngine.Application::get_systemLanguage" , (gpointer)& Application_Get_Custom_PropSystemLanguage );
}

void Register_UnityEngine_Application_SetLogCallbackDefined()
{
    scripting_add_internal_call( "UnityEngine.Application::SetLogCallbackDefined" , (gpointer)& Application_CUSTOM_SetLogCallbackDefined );
}

void Register_UnityEngine_Application_get_backgroundLoadingPriority()
{
    scripting_add_internal_call( "UnityEngine.Application::get_backgroundLoadingPriority" , (gpointer)& Application_Get_Custom_PropBackgroundLoadingPriority );
}

void Register_UnityEngine_Application_set_backgroundLoadingPriority()
{
    scripting_add_internal_call( "UnityEngine.Application::set_backgroundLoadingPriority" , (gpointer)& Application_Set_Custom_PropBackgroundLoadingPriority );
}

void Register_UnityEngine_Application_get_internetReachability()
{
    scripting_add_internal_call( "UnityEngine.Application::get_internetReachability" , (gpointer)& Application_Get_Custom_PropInternetReachability );
}

void Register_UnityEngine_Application_get_genuine()
{
    scripting_add_internal_call( "UnityEngine.Application::get_genuine" , (gpointer)& Application_Get_Custom_PropGenuine );
}

void Register_UnityEngine_Application_get_genuineCheckAvailable()
{
    scripting_add_internal_call( "UnityEngine.Application::get_genuineCheckAvailable" , (gpointer)& Application_Get_Custom_PropGenuineCheckAvailable );
}

void Register_UnityEngine_Application_RequestUserAuthorization()
{
    scripting_add_internal_call( "UnityEngine.Application::RequestUserAuthorization" , (gpointer)& Application_CUSTOM_RequestUserAuthorization );
}

void Register_UnityEngine_Application_HasUserAuthorization()
{
    scripting_add_internal_call( "UnityEngine.Application::HasUserAuthorization" , (gpointer)& Application_CUSTOM_HasUserAuthorization );
}

void Register_UnityEngine_Application_ReplyToUserAuthorizationRequest()
{
    scripting_add_internal_call( "UnityEngine.Application::ReplyToUserAuthorizationRequest" , (gpointer)& Application_CUSTOM_ReplyToUserAuthorizationRequest );
}

void Register_UnityEngine_Application_GetUserAuthorizationRequestMode_Internal()
{
    scripting_add_internal_call( "UnityEngine.Application::GetUserAuthorizationRequestMode_Internal" , (gpointer)& Application_CUSTOM_GetUserAuthorizationRequestMode_Internal );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineApplication_IcallNames [] =
{
    "UnityEngine.Application::Quit"         ,    // -> Application_CUSTOM_Quit
    "UnityEngine.Application::CancelQuit"   ,    // -> Application_CUSTOM_CancelQuit
    "UnityEngine.Application::get_loadedLevel",    // -> Application_Get_Custom_PropLoadedLevel
    "UnityEngine.Application::get_loadedLevelName",    // -> Application_Get_Custom_PropLoadedLevelName
    "UnityEngine.Application::LoadLevelAsync",    // -> Application_CUSTOM_LoadLevelAsync
    "UnityEngine.Application::get_isLoadingLevel",    // -> Application_Get_Custom_PropIsLoadingLevel
    "UnityEngine.Application::get_levelCount",    // -> Application_Get_Custom_PropLevelCount
    "UnityEngine.Application::GetStreamProgressForLevelByName",    // -> Application_CUSTOM_GetStreamProgressForLevelByName
    "UnityEngine.Application::GetStreamProgressForLevel",    // -> Application_CUSTOM_GetStreamProgressForLevel
    "UnityEngine.Application::get_streamedBytes",    // -> Application_Get_Custom_PropStreamedBytes
    "UnityEngine.Application::CanStreamedLevelBeLoadedByName",    // -> Application_CUSTOM_CanStreamedLevelBeLoadedByName
    "UnityEngine.Application::CanStreamedLevelBeLoaded",    // -> Application_CUSTOM_CanStreamedLevelBeLoaded
    "UnityEngine.Application::get_isPlaying",    // -> Application_Get_Custom_PropIsPlaying
    "UnityEngine.Application::get_isEditor" ,    // -> Application_Get_Custom_PropIsEditor
    "UnityEngine.Application::get_isWebPlayer",    // -> Application_Get_Custom_PropIsWebPlayer
    "UnityEngine.Application::get_platform" ,    // -> Application_Get_Custom_PropPlatform
    "UnityEngine.Application::CaptureScreenshot",    // -> Application_CUSTOM_CaptureScreenshot
    "UnityEngine.Application::get_runInBackground",    // -> Application_Get_Custom_PropRunInBackground
    "UnityEngine.Application::set_runInBackground",    // -> Application_Set_Custom_PropRunInBackground
    "UnityEngine.Application::HasProLicense",    // -> Application_CUSTOM_HasProLicense
    "UnityEngine.Application::HasAdvancedLicense",    // -> Application_CUSTOM_HasAdvancedLicense
    "UnityEngine.Application::DontDestroyOnLoad",    // -> Application_CUSTOM_DontDestroyOnLoad
    "UnityEngine.Application::get_dataPath" ,    // -> Application_Get_Custom_PropDataPath
    "UnityEngine.Application::get_streamingAssetsPath",    // -> Application_Get_Custom_PropStreamingAssetsPath
    "UnityEngine.Application::get_persistentDataPath",    // -> Application_Get_Custom_PropPersistentDataPath
    "UnityEngine.Application::get_temporaryCachePath",    // -> Application_Get_Custom_PropTemporaryCachePath
    "UnityEngine.Application::get_srcValue" ,    // -> Application_Get_Custom_PropSrcValue
    "UnityEngine.Application::get_absoluteURL",    // -> Application_Get_Custom_PropAbsoluteURL
#if UNITY_FLASH
    "UnityEngine.Application::Internal_ExternalCall_Flash",    // -> Application_CUSTOM_Internal_ExternalCall_Flash
#endif
#if ENABLE_MONO
    "UnityEngine.Application::Internal_ExternalCall",    // -> Application_CUSTOM_Internal_ExternalCall
#endif
    "UnityEngine.Application::get_unityVersion",    // -> Application_Get_Custom_PropUnityVersion
    "UnityEngine.Application::GetBuildUnityVersion",    // -> Application_CUSTOM_GetBuildUnityVersion
    "UnityEngine.Application::GetNumericUnityVersion",    // -> Application_CUSTOM_GetNumericUnityVersion
    "UnityEngine.Application::get_webSecurityEnabled",    // -> Application_Get_Custom_PropWebSecurityEnabled
    "UnityEngine.Application::get_webSecurityHostUrl",    // -> Application_Get_Custom_PropWebSecurityHostUrl
    "UnityEngine.Application::OpenURL"      ,    // -> Application_CUSTOM_OpenURL
    "UnityEngine.Application::CommitSuicide",    // -> Application_CUSTOM_CommitSuicide
    "UnityEngine.Application::get_targetFrameRate",    // -> Application_Get_Custom_PropTargetFrameRate
    "UnityEngine.Application::set_targetFrameRate",    // -> Application_Set_Custom_PropTargetFrameRate
    "UnityEngine.Application::get_systemLanguage",    // -> Application_Get_Custom_PropSystemLanguage
    "UnityEngine.Application::SetLogCallbackDefined",    // -> Application_CUSTOM_SetLogCallbackDefined
    "UnityEngine.Application::get_backgroundLoadingPriority",    // -> Application_Get_Custom_PropBackgroundLoadingPriority
    "UnityEngine.Application::set_backgroundLoadingPriority",    // -> Application_Set_Custom_PropBackgroundLoadingPriority
    "UnityEngine.Application::get_internetReachability",    // -> Application_Get_Custom_PropInternetReachability
    "UnityEngine.Application::get_genuine"  ,    // -> Application_Get_Custom_PropGenuine
    "UnityEngine.Application::get_genuineCheckAvailable",    // -> Application_Get_Custom_PropGenuineCheckAvailable
    "UnityEngine.Application::RequestUserAuthorization",    // -> Application_CUSTOM_RequestUserAuthorization
    "UnityEngine.Application::HasUserAuthorization",    // -> Application_CUSTOM_HasUserAuthorization
    "UnityEngine.Application::ReplyToUserAuthorizationRequest",    // -> Application_CUSTOM_ReplyToUserAuthorizationRequest
    "UnityEngine.Application::GetUserAuthorizationRequestMode_Internal",    // -> Application_CUSTOM_GetUserAuthorizationRequestMode_Internal
    NULL
};

static const void* s_UnityEngineApplication_IcallFuncs [] =
{
    (const void*)&Application_CUSTOM_Quit                 ,  //  <- UnityEngine.Application::Quit
    (const void*)&Application_CUSTOM_CancelQuit           ,  //  <- UnityEngine.Application::CancelQuit
    (const void*)&Application_Get_Custom_PropLoadedLevel  ,  //  <- UnityEngine.Application::get_loadedLevel
    (const void*)&Application_Get_Custom_PropLoadedLevelName,  //  <- UnityEngine.Application::get_loadedLevelName
    (const void*)&Application_CUSTOM_LoadLevelAsync       ,  //  <- UnityEngine.Application::LoadLevelAsync
    (const void*)&Application_Get_Custom_PropIsLoadingLevel,  //  <- UnityEngine.Application::get_isLoadingLevel
    (const void*)&Application_Get_Custom_PropLevelCount   ,  //  <- UnityEngine.Application::get_levelCount
    (const void*)&Application_CUSTOM_GetStreamProgressForLevelByName,  //  <- UnityEngine.Application::GetStreamProgressForLevelByName
    (const void*)&Application_CUSTOM_GetStreamProgressForLevel,  //  <- UnityEngine.Application::GetStreamProgressForLevel
    (const void*)&Application_Get_Custom_PropStreamedBytes,  //  <- UnityEngine.Application::get_streamedBytes
    (const void*)&Application_CUSTOM_CanStreamedLevelBeLoadedByName,  //  <- UnityEngine.Application::CanStreamedLevelBeLoadedByName
    (const void*)&Application_CUSTOM_CanStreamedLevelBeLoaded,  //  <- UnityEngine.Application::CanStreamedLevelBeLoaded
    (const void*)&Application_Get_Custom_PropIsPlaying    ,  //  <- UnityEngine.Application::get_isPlaying
    (const void*)&Application_Get_Custom_PropIsEditor     ,  //  <- UnityEngine.Application::get_isEditor
    (const void*)&Application_Get_Custom_PropIsWebPlayer  ,  //  <- UnityEngine.Application::get_isWebPlayer
    (const void*)&Application_Get_Custom_PropPlatform     ,  //  <- UnityEngine.Application::get_platform
    (const void*)&Application_CUSTOM_CaptureScreenshot    ,  //  <- UnityEngine.Application::CaptureScreenshot
    (const void*)&Application_Get_Custom_PropRunInBackground,  //  <- UnityEngine.Application::get_runInBackground
    (const void*)&Application_Set_Custom_PropRunInBackground,  //  <- UnityEngine.Application::set_runInBackground
    (const void*)&Application_CUSTOM_HasProLicense        ,  //  <- UnityEngine.Application::HasProLicense
    (const void*)&Application_CUSTOM_HasAdvancedLicense   ,  //  <- UnityEngine.Application::HasAdvancedLicense
    (const void*)&Application_CUSTOM_DontDestroyOnLoad    ,  //  <- UnityEngine.Application::DontDestroyOnLoad
    (const void*)&Application_Get_Custom_PropDataPath     ,  //  <- UnityEngine.Application::get_dataPath
    (const void*)&Application_Get_Custom_PropStreamingAssetsPath,  //  <- UnityEngine.Application::get_streamingAssetsPath
    (const void*)&Application_Get_Custom_PropPersistentDataPath,  //  <- UnityEngine.Application::get_persistentDataPath
    (const void*)&Application_Get_Custom_PropTemporaryCachePath,  //  <- UnityEngine.Application::get_temporaryCachePath
    (const void*)&Application_Get_Custom_PropSrcValue     ,  //  <- UnityEngine.Application::get_srcValue
    (const void*)&Application_Get_Custom_PropAbsoluteURL  ,  //  <- UnityEngine.Application::get_absoluteURL
#if UNITY_FLASH
    (const void*)&Application_CUSTOM_Internal_ExternalCall_Flash,  //  <- UnityEngine.Application::Internal_ExternalCall_Flash
#endif
#if ENABLE_MONO
    (const void*)&Application_CUSTOM_Internal_ExternalCall,  //  <- UnityEngine.Application::Internal_ExternalCall
#endif
    (const void*)&Application_Get_Custom_PropUnityVersion ,  //  <- UnityEngine.Application::get_unityVersion
    (const void*)&Application_CUSTOM_GetBuildUnityVersion ,  //  <- UnityEngine.Application::GetBuildUnityVersion
    (const void*)&Application_CUSTOM_GetNumericUnityVersion,  //  <- UnityEngine.Application::GetNumericUnityVersion
    (const void*)&Application_Get_Custom_PropWebSecurityEnabled,  //  <- UnityEngine.Application::get_webSecurityEnabled
    (const void*)&Application_Get_Custom_PropWebSecurityHostUrl,  //  <- UnityEngine.Application::get_webSecurityHostUrl
    (const void*)&Application_CUSTOM_OpenURL              ,  //  <- UnityEngine.Application::OpenURL
    (const void*)&Application_CUSTOM_CommitSuicide        ,  //  <- UnityEngine.Application::CommitSuicide
    (const void*)&Application_Get_Custom_PropTargetFrameRate,  //  <- UnityEngine.Application::get_targetFrameRate
    (const void*)&Application_Set_Custom_PropTargetFrameRate,  //  <- UnityEngine.Application::set_targetFrameRate
    (const void*)&Application_Get_Custom_PropSystemLanguage,  //  <- UnityEngine.Application::get_systemLanguage
    (const void*)&Application_CUSTOM_SetLogCallbackDefined,  //  <- UnityEngine.Application::SetLogCallbackDefined
    (const void*)&Application_Get_Custom_PropBackgroundLoadingPriority,  //  <- UnityEngine.Application::get_backgroundLoadingPriority
    (const void*)&Application_Set_Custom_PropBackgroundLoadingPriority,  //  <- UnityEngine.Application::set_backgroundLoadingPriority
    (const void*)&Application_Get_Custom_PropInternetReachability,  //  <- UnityEngine.Application::get_internetReachability
    (const void*)&Application_Get_Custom_PropGenuine      ,  //  <- UnityEngine.Application::get_genuine
    (const void*)&Application_Get_Custom_PropGenuineCheckAvailable,  //  <- UnityEngine.Application::get_genuineCheckAvailable
    (const void*)&Application_CUSTOM_RequestUserAuthorization,  //  <- UnityEngine.Application::RequestUserAuthorization
    (const void*)&Application_CUSTOM_HasUserAuthorization ,  //  <- UnityEngine.Application::HasUserAuthorization
    (const void*)&Application_CUSTOM_ReplyToUserAuthorizationRequest,  //  <- UnityEngine.Application::ReplyToUserAuthorizationRequest
    (const void*)&Application_CUSTOM_GetUserAuthorizationRequestMode_Internal,  //  <- UnityEngine.Application::GetUserAuthorizationRequestMode_Internal
    NULL
};

void ExportUnityEngineApplicationBindings();
void ExportUnityEngineApplicationBindings()
{
    for (int i = 0; s_UnityEngineApplication_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineApplication_IcallNames [i], s_UnityEngineApplication_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineApplicationBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Application_CUSTOM_Quit); //  <- UnityEngine.Application::Quit
    SET_METRO_BINDING(Application_CUSTOM_CancelQuit); //  <- UnityEngine.Application::CancelQuit
    SET_METRO_BINDING(Application_Get_Custom_PropLoadedLevel); //  <- UnityEngine.Application::get_loadedLevel
    SET_METRO_BINDING(Application_Get_Custom_PropLoadedLevelName); //  <- UnityEngine.Application::get_loadedLevelName
    SET_METRO_BINDING(Application_CUSTOM_LoadLevelAsync); //  <- UnityEngine.Application::LoadLevelAsync
    SET_METRO_BINDING(Application_Get_Custom_PropIsLoadingLevel); //  <- UnityEngine.Application::get_isLoadingLevel
    SET_METRO_BINDING(Application_Get_Custom_PropLevelCount); //  <- UnityEngine.Application::get_levelCount
    SET_METRO_BINDING(Application_CUSTOM_GetStreamProgressForLevelByName); //  <- UnityEngine.Application::GetStreamProgressForLevelByName
    SET_METRO_BINDING(Application_CUSTOM_GetStreamProgressForLevel); //  <- UnityEngine.Application::GetStreamProgressForLevel
    SET_METRO_BINDING(Application_Get_Custom_PropStreamedBytes); //  <- UnityEngine.Application::get_streamedBytes
    SET_METRO_BINDING(Application_CUSTOM_CanStreamedLevelBeLoadedByName); //  <- UnityEngine.Application::CanStreamedLevelBeLoadedByName
    SET_METRO_BINDING(Application_CUSTOM_CanStreamedLevelBeLoaded); //  <- UnityEngine.Application::CanStreamedLevelBeLoaded
    SET_METRO_BINDING(Application_Get_Custom_PropIsPlaying); //  <- UnityEngine.Application::get_isPlaying
    SET_METRO_BINDING(Application_Get_Custom_PropIsEditor); //  <- UnityEngine.Application::get_isEditor
    SET_METRO_BINDING(Application_Get_Custom_PropIsWebPlayer); //  <- UnityEngine.Application::get_isWebPlayer
    SET_METRO_BINDING(Application_Get_Custom_PropPlatform); //  <- UnityEngine.Application::get_platform
    SET_METRO_BINDING(Application_CUSTOM_CaptureScreenshot); //  <- UnityEngine.Application::CaptureScreenshot
    SET_METRO_BINDING(Application_Get_Custom_PropRunInBackground); //  <- UnityEngine.Application::get_runInBackground
    SET_METRO_BINDING(Application_Set_Custom_PropRunInBackground); //  <- UnityEngine.Application::set_runInBackground
    SET_METRO_BINDING(Application_CUSTOM_HasProLicense); //  <- UnityEngine.Application::HasProLicense
    SET_METRO_BINDING(Application_CUSTOM_HasAdvancedLicense); //  <- UnityEngine.Application::HasAdvancedLicense
    SET_METRO_BINDING(Application_CUSTOM_DontDestroyOnLoad); //  <- UnityEngine.Application::DontDestroyOnLoad
    SET_METRO_BINDING(Application_Get_Custom_PropDataPath); //  <- UnityEngine.Application::get_dataPath
    SET_METRO_BINDING(Application_Get_Custom_PropStreamingAssetsPath); //  <- UnityEngine.Application::get_streamingAssetsPath
    SET_METRO_BINDING(Application_Get_Custom_PropPersistentDataPath); //  <- UnityEngine.Application::get_persistentDataPath
    SET_METRO_BINDING(Application_Get_Custom_PropTemporaryCachePath); //  <- UnityEngine.Application::get_temporaryCachePath
    SET_METRO_BINDING(Application_Get_Custom_PropSrcValue); //  <- UnityEngine.Application::get_srcValue
    SET_METRO_BINDING(Application_Get_Custom_PropAbsoluteURL); //  <- UnityEngine.Application::get_absoluteURL
#if UNITY_FLASH
    SET_METRO_BINDING(Application_CUSTOM_Internal_ExternalCall_Flash); //  <- UnityEngine.Application::Internal_ExternalCall_Flash
#endif
#if ENABLE_MONO
    SET_METRO_BINDING(Application_CUSTOM_Internal_ExternalCall); //  <- UnityEngine.Application::Internal_ExternalCall
#endif
    SET_METRO_BINDING(Application_Get_Custom_PropUnityVersion); //  <- UnityEngine.Application::get_unityVersion
    SET_METRO_BINDING(Application_CUSTOM_GetBuildUnityVersion); //  <- UnityEngine.Application::GetBuildUnityVersion
    SET_METRO_BINDING(Application_CUSTOM_GetNumericUnityVersion); //  <- UnityEngine.Application::GetNumericUnityVersion
    SET_METRO_BINDING(Application_Get_Custom_PropWebSecurityEnabled); //  <- UnityEngine.Application::get_webSecurityEnabled
    SET_METRO_BINDING(Application_Get_Custom_PropWebSecurityHostUrl); //  <- UnityEngine.Application::get_webSecurityHostUrl
    SET_METRO_BINDING(Application_CUSTOM_OpenURL); //  <- UnityEngine.Application::OpenURL
    SET_METRO_BINDING(Application_CUSTOM_CommitSuicide); //  <- UnityEngine.Application::CommitSuicide
    SET_METRO_BINDING(Application_Get_Custom_PropTargetFrameRate); //  <- UnityEngine.Application::get_targetFrameRate
    SET_METRO_BINDING(Application_Set_Custom_PropTargetFrameRate); //  <- UnityEngine.Application::set_targetFrameRate
    SET_METRO_BINDING(Application_Get_Custom_PropSystemLanguage); //  <- UnityEngine.Application::get_systemLanguage
    SET_METRO_BINDING(Application_CUSTOM_SetLogCallbackDefined); //  <- UnityEngine.Application::SetLogCallbackDefined
    SET_METRO_BINDING(Application_Get_Custom_PropBackgroundLoadingPriority); //  <- UnityEngine.Application::get_backgroundLoadingPriority
    SET_METRO_BINDING(Application_Set_Custom_PropBackgroundLoadingPriority); //  <- UnityEngine.Application::set_backgroundLoadingPriority
    SET_METRO_BINDING(Application_Get_Custom_PropInternetReachability); //  <- UnityEngine.Application::get_internetReachability
    SET_METRO_BINDING(Application_Get_Custom_PropGenuine); //  <- UnityEngine.Application::get_genuine
    SET_METRO_BINDING(Application_Get_Custom_PropGenuineCheckAvailable); //  <- UnityEngine.Application::get_genuineCheckAvailable
    SET_METRO_BINDING(Application_CUSTOM_RequestUserAuthorization); //  <- UnityEngine.Application::RequestUserAuthorization
    SET_METRO_BINDING(Application_CUSTOM_HasUserAuthorization); //  <- UnityEngine.Application::HasUserAuthorization
    SET_METRO_BINDING(Application_CUSTOM_ReplyToUserAuthorizationRequest); //  <- UnityEngine.Application::ReplyToUserAuthorizationRequest
    SET_METRO_BINDING(Application_CUSTOM_GetUserAuthorizationRequestMode_Internal); //  <- UnityEngine.Application::GetUserAuthorizationRequestMode_Internal
}

#endif
