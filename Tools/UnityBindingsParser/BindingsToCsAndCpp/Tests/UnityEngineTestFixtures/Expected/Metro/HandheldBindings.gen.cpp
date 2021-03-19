#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Graphics/MoviePlayback.h"
#include "Runtime/Graphics/RenderTexture.h"
#include "Runtime/Input/GetInput.h"
#include "Runtime/Input/OnScreenKeyboard.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Misc/PlayerSettings.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Math/Vector2.h"
#include "Runtime/Shaders/Material.h"
#include "PlatformDependent/iPhonePlayer/iPhoneSettings.h"
#include "PlatformDependent/iPhonePlayer/iPhoneMisc.h"
#include "PlatformDependent/AndroidPlayer/AndroidSystemInfo.h"


#if PLATFORM_ANDROID
#include "PlatformDependent/AndroidPlayer/EntryPoint.h"
#elif PLATFORM_IPHONE
	void StartActivityIndicator();
	void StopActivityIndicator();
#endif

using namespace Unity;
#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Handheld_CUSTOM_INTERNAL_CALL_PlayFullScreenMovie(ICallType_String_Argument path_, const ColorRGBAf& bgColor, unsigned controlMode, unsigned scalingMode)
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_INTERNAL_CALL_PlayFullScreenMovie)
    ICallType_String_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_PlayFullScreenMovie)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_PlayFullScreenMovie)
    
    		return PlayFullScreenMovie (path, bgColor, controlMode, scalingMode);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Handheld_CUSTOM_Vibrate()
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_Vibrate)
    SCRIPTINGAPI_STACK_CHECK(Vibrate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Vibrate)
    
    		Vibrate ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Handheld_Get_Custom_PropUse32BitDisplayBuffer()
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_Get_Custom_PropUse32BitDisplayBuffer)
    SCRIPTINGAPI_STACK_CHECK(get_use32BitDisplayBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_use32BitDisplayBuffer)
    return GetPlayerSettings().GetUse32BitDisplayBuffer();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Handheld_Set_Custom_PropUse32BitDisplayBuffer(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_Set_Custom_PropUse32BitDisplayBuffer)
    SCRIPTINGAPI_STACK_CHECK(set_use32BitDisplayBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_use32BitDisplayBuffer)
    
    GetPlayerSettings().SetUse32BitDisplayBuffer(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Handheld_CUSTOM_SetActivityIndicatorStyleImpl(int style)
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_SetActivityIndicatorStyleImpl)
    SCRIPTINGAPI_STACK_CHECK(SetActivityIndicatorStyleImpl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetActivityIndicatorStyleImpl)
    
    	#if PLATFORM_IPHONE
    		GetPlayerSettings().SetIOSShowActivityIndicatorOnLoading(style);
    	#elif PLATFORM_ANDROID
    		GetPlayerSettings().SetAndroidShowActivityIndicatorOnLoading(style);
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Handheld_CUSTOM_GetActivityIndicatorStyle()
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_GetActivityIndicatorStyle)
    SCRIPTINGAPI_STACK_CHECK(GetActivityIndicatorStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetActivityIndicatorStyle)
    
    	#if PLATFORM_IPHONE
    		return GetPlayerSettings().GetIOSShowActivityIndicatorOnLoading();
    	#elif PLATFORM_ANDROID
    		return GetPlayerSettings().GetAndroidShowActivityIndicatorOnLoading();
    	#else
    		return -1;
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Handheld_CUSTOM_StartActivityIndicator()
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_StartActivityIndicator)
    SCRIPTINGAPI_STACK_CHECK(StartActivityIndicator)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StartActivityIndicator)
    
    	#if PLATFORM_IPHONE || PLATFORM_ANDROID
    		StartActivityIndicator();
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Handheld_CUSTOM_StopActivityIndicator()
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_StopActivityIndicator)
    SCRIPTINGAPI_STACK_CHECK(StopActivityIndicator)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopActivityIndicator)
    
    	#if PLATFORM_IPHONE || PLATFORM_ANDROID
    		StopActivityIndicator();
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Handheld_CUSTOM_ClearShaderCache()
{
    SCRIPTINGAPI_ETW_ENTRY(Handheld_CUSTOM_ClearShaderCache)
    SCRIPTINGAPI_STACK_CHECK(ClearShaderCache)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearShaderCache)
    
    	#if PLATFORM_ANDROID
    		extern void Internal_ClearShaderCache();
    		Internal_ClearShaderCache();
    	#endif
    	
}

#endif


struct MonoTouchScreenKeyboard_InternalConstructorHelperArguments {
	MonoString* text;
	MonoString* textPlaceholder;
	unsigned int keyboardType;
	unsigned int autocorrection;
	unsigned int multiline;
	unsigned int secure;
	unsigned int alert;
};

 #define GET ExtractMonoObjectData<KeyboardOnScreen*> (self)
#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TouchScreenKeyboard_CUSTOM_Destroy(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_CUSTOM_Destroy)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Destroy)
    
    		if (GET)
    		{
    			delete GET;
    			GET=0;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TouchScreenKeyboard_CUSTOM_TouchScreenKeyboard_InternalConstructorHelper(ICallType_Object_Argument self_, MonoTouchScreenKeyboard_InternalConstructorHelperArguments arguments)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_CUSTOM_TouchScreenKeyboard_InternalConstructorHelper)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(TouchScreenKeyboard_InternalConstructorHelper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TouchScreenKeyboard_InternalConstructorHelper)
    
    		GET = new KeyboardOnScreen(scripting_cpp_string_for(arguments.text),
    			arguments.keyboardType, arguments.autocorrection, arguments.multiline, arguments.secure, arguments.alert,
    			scripting_cpp_string_for(arguments.textPlaceholder));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Get_Custom_PropText(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Get_Custom_PropText)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_text)
    if (GET) return CreateScriptingString(GET->getText());
    else return CreateScriptingString("");
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Set_Custom_PropText(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Set_Custom_PropText)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_text)
    
    if (GET) GET->setText(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Get_Custom_PropHideInput()
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Get_Custom_PropHideInput)
    SCRIPTINGAPI_STACK_CHECK(get_hideInput)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hideInput)
    return KeyboardOnScreen::isInputHidden();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Set_Custom_PropHideInput(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Set_Custom_PropHideInput)
    SCRIPTINGAPI_STACK_CHECK(set_hideInput)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hideInput)
    
    KeyboardOnScreen::setInputHidden(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Get_Custom_PropActive(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Get_Custom_PropActive)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_active)
    if (GET) return (short)GET->isActive();
    else return false;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Set_Custom_PropActive(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Set_Custom_PropActive)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_active)
    
    if (GET) GET->setActive(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Get_Custom_PropDone(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Get_Custom_PropDone)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_done)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_done)
    if (GET) return (short)GET->isDone();
    else return false;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Get_Custom_PropWasCanceled(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Get_Custom_PropWasCanceled)
    ScriptingObjectWithIntPtrField<KeyboardOnScreen> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wasCanceled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wasCanceled)
    if (GET) return (short)GET->wasCanceled();
    else return false;
}

#endif

 #undef GET
#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TouchScreenKeyboard_CUSTOM_INTERNAL_get_area(Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_CUSTOM_INTERNAL_get_area)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_area)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_area)
    { *returnValue =(KeyboardOnScreen::GetRect()); return;};
}

#endif

#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TouchScreenKeyboard_Get_Custom_PropVisible()
{
    SCRIPTINGAPI_ETW_ENTRY(TouchScreenKeyboard_Get_Custom_PropVisible)
    SCRIPTINGAPI_STACK_CHECK(get_visible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_visible)
    return KeyboardOnScreen::IsVisible();
}

#endif
#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
iPhoneGeneration SCRIPT_CALL_CONVENTION iPhone_Get_Custom_PropGeneration()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhone_Get_Custom_PropGeneration)
    SCRIPTINGAPI_STACK_CHECK(get_generation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_generation)
    return iphone::GetDeviceGeneration ();
}

#endif

#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhone_CUSTOM_SetNoBackupFlag(ICallType_String_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhone_CUSTOM_SetNoBackupFlag)
    ICallType_String_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(SetNoBackupFlag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetNoBackupFlag)
    
    	#if PLATFORM_IPHONE
    		iphone::SetNoBackupFlag(path.ToUTF8().c_str());
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhone_CUSTOM_ResetNoBackupFlag(ICallType_String_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhone_CUSTOM_ResetNoBackupFlag)
    ICallType_String_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(ResetNoBackupFlag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ResetNoBackupFlag)
    
    	#if PLATFORM_IPHONE
    		iphone::ResetNoBackupFlag(path.ToUTF8().c_str());
    	#endif
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if UNITY_IPHONE_API || UNITY_ANDROID_API
void Register_UnityEngine_Handheld_INTERNAL_CALL_PlayFullScreenMovie()
{
    scripting_add_internal_call( "UnityEngine.Handheld::INTERNAL_CALL_PlayFullScreenMovie" , (gpointer)& Handheld_CUSTOM_INTERNAL_CALL_PlayFullScreenMovie );
}

void Register_UnityEngine_Handheld_Vibrate()
{
    scripting_add_internal_call( "UnityEngine.Handheld::Vibrate" , (gpointer)& Handheld_CUSTOM_Vibrate );
}

void Register_UnityEngine_Handheld_get_use32BitDisplayBuffer()
{
    scripting_add_internal_call( "UnityEngine.Handheld::get_use32BitDisplayBuffer" , (gpointer)& Handheld_Get_Custom_PropUse32BitDisplayBuffer );
}

void Register_UnityEngine_Handheld_set_use32BitDisplayBuffer()
{
    scripting_add_internal_call( "UnityEngine.Handheld::set_use32BitDisplayBuffer" , (gpointer)& Handheld_Set_Custom_PropUse32BitDisplayBuffer );
}

void Register_UnityEngine_Handheld_SetActivityIndicatorStyleImpl()
{
    scripting_add_internal_call( "UnityEngine.Handheld::SetActivityIndicatorStyleImpl" , (gpointer)& Handheld_CUSTOM_SetActivityIndicatorStyleImpl );
}

void Register_UnityEngine_Handheld_GetActivityIndicatorStyle()
{
    scripting_add_internal_call( "UnityEngine.Handheld::GetActivityIndicatorStyle" , (gpointer)& Handheld_CUSTOM_GetActivityIndicatorStyle );
}

void Register_UnityEngine_Handheld_StartActivityIndicator()
{
    scripting_add_internal_call( "UnityEngine.Handheld::StartActivityIndicator" , (gpointer)& Handheld_CUSTOM_StartActivityIndicator );
}

void Register_UnityEngine_Handheld_StopActivityIndicator()
{
    scripting_add_internal_call( "UnityEngine.Handheld::StopActivityIndicator" , (gpointer)& Handheld_CUSTOM_StopActivityIndicator );
}

void Register_UnityEngine_Handheld_ClearShaderCache()
{
    scripting_add_internal_call( "UnityEngine.Handheld::ClearShaderCache" , (gpointer)& Handheld_CUSTOM_ClearShaderCache );
}

void Register_UnityEngine_TouchScreenKeyboard_Destroy()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::Destroy" , (gpointer)& TouchScreenKeyboard_CUSTOM_Destroy );
}

void Register_UnityEngine_TouchScreenKeyboard_TouchScreenKeyboard_InternalConstructorHelper()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::TouchScreenKeyboard_InternalConstructorHelper" , (gpointer)& TouchScreenKeyboard_CUSTOM_TouchScreenKeyboard_InternalConstructorHelper );
}

void Register_UnityEngine_TouchScreenKeyboard_get_text()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::get_text" , (gpointer)& TouchScreenKeyboard_Get_Custom_PropText );
}

void Register_UnityEngine_TouchScreenKeyboard_set_text()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::set_text" , (gpointer)& TouchScreenKeyboard_Set_Custom_PropText );
}

void Register_UnityEngine_TouchScreenKeyboard_get_hideInput()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::get_hideInput" , (gpointer)& TouchScreenKeyboard_Get_Custom_PropHideInput );
}

void Register_UnityEngine_TouchScreenKeyboard_set_hideInput()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::set_hideInput" , (gpointer)& TouchScreenKeyboard_Set_Custom_PropHideInput );
}

void Register_UnityEngine_TouchScreenKeyboard_get_active()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::get_active" , (gpointer)& TouchScreenKeyboard_Get_Custom_PropActive );
}

void Register_UnityEngine_TouchScreenKeyboard_set_active()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::set_active" , (gpointer)& TouchScreenKeyboard_Set_Custom_PropActive );
}

void Register_UnityEngine_TouchScreenKeyboard_get_done()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::get_done" , (gpointer)& TouchScreenKeyboard_Get_Custom_PropDone );
}

void Register_UnityEngine_TouchScreenKeyboard_get_wasCanceled()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::get_wasCanceled" , (gpointer)& TouchScreenKeyboard_Get_Custom_PropWasCanceled );
}

void Register_UnityEngine_TouchScreenKeyboard_INTERNAL_get_area()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::INTERNAL_get_area" , (gpointer)& TouchScreenKeyboard_CUSTOM_INTERNAL_get_area );
}

void Register_UnityEngine_TouchScreenKeyboard_get_visible()
{
    scripting_add_internal_call( "UnityEngine.TouchScreenKeyboard::get_visible" , (gpointer)& TouchScreenKeyboard_Get_Custom_PropVisible );
}

#endif
#if UNITY_IPHONE_API
void Register_UnityEngine_iPhone_get_generation()
{
    scripting_add_internal_call( "UnityEngine.iPhone::get_generation" , (gpointer)& iPhone_Get_Custom_PropGeneration );
}

void Register_UnityEngine_iPhone_SetNoBackupFlag()
{
    scripting_add_internal_call( "UnityEngine.iPhone::SetNoBackupFlag" , (gpointer)& iPhone_CUSTOM_SetNoBackupFlag );
}

void Register_UnityEngine_iPhone_ResetNoBackupFlag()
{
    scripting_add_internal_call( "UnityEngine.iPhone::ResetNoBackupFlag" , (gpointer)& iPhone_CUSTOM_ResetNoBackupFlag );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Handheld_IcallNames [] =
{
#if UNITY_IPHONE_API || UNITY_ANDROID_API
    "UnityEngine.Handheld::INTERNAL_CALL_PlayFullScreenMovie",    // -> Handheld_CUSTOM_INTERNAL_CALL_PlayFullScreenMovie
    "UnityEngine.Handheld::Vibrate"         ,    // -> Handheld_CUSTOM_Vibrate
    "UnityEngine.Handheld::get_use32BitDisplayBuffer",    // -> Handheld_Get_Custom_PropUse32BitDisplayBuffer
    "UnityEngine.Handheld::set_use32BitDisplayBuffer",    // -> Handheld_Set_Custom_PropUse32BitDisplayBuffer
    "UnityEngine.Handheld::SetActivityIndicatorStyleImpl",    // -> Handheld_CUSTOM_SetActivityIndicatorStyleImpl
    "UnityEngine.Handheld::GetActivityIndicatorStyle",    // -> Handheld_CUSTOM_GetActivityIndicatorStyle
    "UnityEngine.Handheld::StartActivityIndicator",    // -> Handheld_CUSTOM_StartActivityIndicator
    "UnityEngine.Handheld::StopActivityIndicator",    // -> Handheld_CUSTOM_StopActivityIndicator
    "UnityEngine.Handheld::ClearShaderCache",    // -> Handheld_CUSTOM_ClearShaderCache
    "UnityEngine.TouchScreenKeyboard::Destroy",    // -> TouchScreenKeyboard_CUSTOM_Destroy
    "UnityEngine.TouchScreenKeyboard::TouchScreenKeyboard_InternalConstructorHelper",    // -> TouchScreenKeyboard_CUSTOM_TouchScreenKeyboard_InternalConstructorHelper
    "UnityEngine.TouchScreenKeyboard::get_text",    // -> TouchScreenKeyboard_Get_Custom_PropText
    "UnityEngine.TouchScreenKeyboard::set_text",    // -> TouchScreenKeyboard_Set_Custom_PropText
    "UnityEngine.TouchScreenKeyboard::get_hideInput",    // -> TouchScreenKeyboard_Get_Custom_PropHideInput
    "UnityEngine.TouchScreenKeyboard::set_hideInput",    // -> TouchScreenKeyboard_Set_Custom_PropHideInput
    "UnityEngine.TouchScreenKeyboard::get_active",    // -> TouchScreenKeyboard_Get_Custom_PropActive
    "UnityEngine.TouchScreenKeyboard::set_active",    // -> TouchScreenKeyboard_Set_Custom_PropActive
    "UnityEngine.TouchScreenKeyboard::get_done",    // -> TouchScreenKeyboard_Get_Custom_PropDone
    "UnityEngine.TouchScreenKeyboard::get_wasCanceled",    // -> TouchScreenKeyboard_Get_Custom_PropWasCanceled
    "UnityEngine.TouchScreenKeyboard::INTERNAL_get_area",    // -> TouchScreenKeyboard_CUSTOM_INTERNAL_get_area
    "UnityEngine.TouchScreenKeyboard::get_visible",    // -> TouchScreenKeyboard_Get_Custom_PropVisible
#endif
#if UNITY_IPHONE_API
    "UnityEngine.iPhone::get_generation"    ,    // -> iPhone_Get_Custom_PropGeneration
    "UnityEngine.iPhone::SetNoBackupFlag"   ,    // -> iPhone_CUSTOM_SetNoBackupFlag
    "UnityEngine.iPhone::ResetNoBackupFlag" ,    // -> iPhone_CUSTOM_ResetNoBackupFlag
#endif
    NULL
};

static const void* s_Handheld_IcallFuncs [] =
{
#if UNITY_IPHONE_API || UNITY_ANDROID_API
    (const void*)&Handheld_CUSTOM_INTERNAL_CALL_PlayFullScreenMovie,  //  <- UnityEngine.Handheld::INTERNAL_CALL_PlayFullScreenMovie
    (const void*)&Handheld_CUSTOM_Vibrate                 ,  //  <- UnityEngine.Handheld::Vibrate
    (const void*)&Handheld_Get_Custom_PropUse32BitDisplayBuffer,  //  <- UnityEngine.Handheld::get_use32BitDisplayBuffer
    (const void*)&Handheld_Set_Custom_PropUse32BitDisplayBuffer,  //  <- UnityEngine.Handheld::set_use32BitDisplayBuffer
    (const void*)&Handheld_CUSTOM_SetActivityIndicatorStyleImpl,  //  <- UnityEngine.Handheld::SetActivityIndicatorStyleImpl
    (const void*)&Handheld_CUSTOM_GetActivityIndicatorStyle,  //  <- UnityEngine.Handheld::GetActivityIndicatorStyle
    (const void*)&Handheld_CUSTOM_StartActivityIndicator  ,  //  <- UnityEngine.Handheld::StartActivityIndicator
    (const void*)&Handheld_CUSTOM_StopActivityIndicator   ,  //  <- UnityEngine.Handheld::StopActivityIndicator
    (const void*)&Handheld_CUSTOM_ClearShaderCache        ,  //  <- UnityEngine.Handheld::ClearShaderCache
    (const void*)&TouchScreenKeyboard_CUSTOM_Destroy      ,  //  <- UnityEngine.TouchScreenKeyboard::Destroy
    (const void*)&TouchScreenKeyboard_CUSTOM_TouchScreenKeyboard_InternalConstructorHelper,  //  <- UnityEngine.TouchScreenKeyboard::TouchScreenKeyboard_InternalConstructorHelper
    (const void*)&TouchScreenKeyboard_Get_Custom_PropText ,  //  <- UnityEngine.TouchScreenKeyboard::get_text
    (const void*)&TouchScreenKeyboard_Set_Custom_PropText ,  //  <- UnityEngine.TouchScreenKeyboard::set_text
    (const void*)&TouchScreenKeyboard_Get_Custom_PropHideInput,  //  <- UnityEngine.TouchScreenKeyboard::get_hideInput
    (const void*)&TouchScreenKeyboard_Set_Custom_PropHideInput,  //  <- UnityEngine.TouchScreenKeyboard::set_hideInput
    (const void*)&TouchScreenKeyboard_Get_Custom_PropActive,  //  <- UnityEngine.TouchScreenKeyboard::get_active
    (const void*)&TouchScreenKeyboard_Set_Custom_PropActive,  //  <- UnityEngine.TouchScreenKeyboard::set_active
    (const void*)&TouchScreenKeyboard_Get_Custom_PropDone ,  //  <- UnityEngine.TouchScreenKeyboard::get_done
    (const void*)&TouchScreenKeyboard_Get_Custom_PropWasCanceled,  //  <- UnityEngine.TouchScreenKeyboard::get_wasCanceled
    (const void*)&TouchScreenKeyboard_CUSTOM_INTERNAL_get_area,  //  <- UnityEngine.TouchScreenKeyboard::INTERNAL_get_area
    (const void*)&TouchScreenKeyboard_Get_Custom_PropVisible,  //  <- UnityEngine.TouchScreenKeyboard::get_visible
#endif
#if UNITY_IPHONE_API
    (const void*)&iPhone_Get_Custom_PropGeneration        ,  //  <- UnityEngine.iPhone::get_generation
    (const void*)&iPhone_CUSTOM_SetNoBackupFlag           ,  //  <- UnityEngine.iPhone::SetNoBackupFlag
    (const void*)&iPhone_CUSTOM_ResetNoBackupFlag         ,  //  <- UnityEngine.iPhone::ResetNoBackupFlag
#endif
    NULL
};

void ExportHandheldBindings();
void ExportHandheldBindings()
{
    for (int i = 0; s_Handheld_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Handheld_IcallNames [i], s_Handheld_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportHandheldBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if UNITY_IPHONE_API || UNITY_ANDROID_API
    SET_METRO_BINDING(Handheld_CUSTOM_INTERNAL_CALL_PlayFullScreenMovie); //  <- UnityEngine.Handheld::INTERNAL_CALL_PlayFullScreenMovie
    SET_METRO_BINDING(Handheld_CUSTOM_Vibrate); //  <- UnityEngine.Handheld::Vibrate
    SET_METRO_BINDING(Handheld_Get_Custom_PropUse32BitDisplayBuffer); //  <- UnityEngine.Handheld::get_use32BitDisplayBuffer
    SET_METRO_BINDING(Handheld_Set_Custom_PropUse32BitDisplayBuffer); //  <- UnityEngine.Handheld::set_use32BitDisplayBuffer
    SET_METRO_BINDING(Handheld_CUSTOM_SetActivityIndicatorStyleImpl); //  <- UnityEngine.Handheld::SetActivityIndicatorStyleImpl
    SET_METRO_BINDING(Handheld_CUSTOM_GetActivityIndicatorStyle); //  <- UnityEngine.Handheld::GetActivityIndicatorStyle
    SET_METRO_BINDING(Handheld_CUSTOM_StartActivityIndicator); //  <- UnityEngine.Handheld::StartActivityIndicator
    SET_METRO_BINDING(Handheld_CUSTOM_StopActivityIndicator); //  <- UnityEngine.Handheld::StopActivityIndicator
    SET_METRO_BINDING(Handheld_CUSTOM_ClearShaderCache); //  <- UnityEngine.Handheld::ClearShaderCache
    SET_METRO_BINDING(TouchScreenKeyboard_CUSTOM_Destroy); //  <- UnityEngine.TouchScreenKeyboard::Destroy
    SET_METRO_BINDING(TouchScreenKeyboard_CUSTOM_TouchScreenKeyboard_InternalConstructorHelper); //  <- UnityEngine.TouchScreenKeyboard::TouchScreenKeyboard_InternalConstructorHelper
    SET_METRO_BINDING(TouchScreenKeyboard_Get_Custom_PropText); //  <- UnityEngine.TouchScreenKeyboard::get_text
    SET_METRO_BINDING(TouchScreenKeyboard_Set_Custom_PropText); //  <- UnityEngine.TouchScreenKeyboard::set_text
    SET_METRO_BINDING(TouchScreenKeyboard_Get_Custom_PropHideInput); //  <- UnityEngine.TouchScreenKeyboard::get_hideInput
    SET_METRO_BINDING(TouchScreenKeyboard_Set_Custom_PropHideInput); //  <- UnityEngine.TouchScreenKeyboard::set_hideInput
    SET_METRO_BINDING(TouchScreenKeyboard_Get_Custom_PropActive); //  <- UnityEngine.TouchScreenKeyboard::get_active
    SET_METRO_BINDING(TouchScreenKeyboard_Set_Custom_PropActive); //  <- UnityEngine.TouchScreenKeyboard::set_active
    SET_METRO_BINDING(TouchScreenKeyboard_Get_Custom_PropDone); //  <- UnityEngine.TouchScreenKeyboard::get_done
    SET_METRO_BINDING(TouchScreenKeyboard_Get_Custom_PropWasCanceled); //  <- UnityEngine.TouchScreenKeyboard::get_wasCanceled
    SET_METRO_BINDING(TouchScreenKeyboard_CUSTOM_INTERNAL_get_area); //  <- UnityEngine.TouchScreenKeyboard::INTERNAL_get_area
    SET_METRO_BINDING(TouchScreenKeyboard_Get_Custom_PropVisible); //  <- UnityEngine.TouchScreenKeyboard::get_visible
#endif
#if UNITY_IPHONE_API
    SET_METRO_BINDING(iPhone_Get_Custom_PropGeneration); //  <- UnityEngine.iPhone::get_generation
    SET_METRO_BINDING(iPhone_CUSTOM_SetNoBackupFlag); //  <- UnityEngine.iPhone::SetNoBackupFlag
    SET_METRO_BINDING(iPhone_CUSTOM_ResetNoBackupFlag); //  <- UnityEngine.iPhone::ResetNoBackupFlag
#endif
}

#endif
