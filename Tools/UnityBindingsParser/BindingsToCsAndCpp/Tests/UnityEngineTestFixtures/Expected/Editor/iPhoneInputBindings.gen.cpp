#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Graphics/MoviePlayback.h"
#include "Runtime/Math/Color.h"
#include "Runtime/Misc/SystemInfo.h"
#include "Runtime/Network/NetworkManager.h"
#include "Runtime/Input/GetInput.h"
#include "Runtime/Input/LocationService.h"
#include "Runtime/Input/OnScreenKeyboard.h"
#include "PlatformDependent/iPhonePlayer/iPhoneSettings.h"
#include "PlatformDependent/iPhonePlayer/APN.h"

using namespace Unity;

#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
iPhoneTouch SCRIPT_CALL_CONVENTION iPhoneInput_CUSTOM_GetTouch(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_CUSTOM_GetTouch)
    SCRIPTINGAPI_STACK_CHECK(GetTouch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTouch)
    
    #if ENABLE_NEW_EVENT_SYSTEM
    		if (index >= 0 && index < GetTouchCount ())
    		{
    			Touch* t = GetTouch(index);
    
    			if (t != NULL)
    			{
    				return *t;
    			}
    			else
    			{
    				RaiseMonoException ("GetTouch() failed!");
    			}
    		}
    		else
    		{
    			RaiseMonoException ("Index specified to GetTouch() is out of bounds! Must be less than Touch.touchCount.");
    		}
    		Touch dummy;
    		return dummy;
    #else
    		Touch touch;
    
    		if (index >= 0 && index < GetTouchCount ())
    		{
    			if (!GetTouch (index, touch))
    				RaiseMonoException ("Internal error.");
    		}
    		else
    			RaiseMonoException ("Index out of bounds.");
    		return touch;
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION iPhoneInput_Get_Custom_PropTouchCount()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_Get_Custom_PropTouchCount)
    SCRIPTINGAPI_STACK_CHECK(get_touchCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_touchCount)
    return GetTouchCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneInput_Get_Custom_PropMultiTouchEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_Get_Custom_PropMultiTouchEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_multiTouchEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_multiTouchEnabled)
    return IsMultiTouchEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneInput_Set_Custom_PropMultiTouchEnabled(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_Set_Custom_PropMultiTouchEnabled)
    SCRIPTINGAPI_STACK_CHECK(set_multiTouchEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_multiTouchEnabled)
     return SetMultiTouchEnabled (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
iPhoneAccelerationEvent SCRIPT_CALL_CONVENTION iPhoneInput_CUSTOM_GetAccelerationEvent(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_CUSTOM_GetAccelerationEvent)
    SCRIPTINGAPI_STACK_CHECK(GetAccelerationEvent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAccelerationEvent)
    
    		Acceleration acc;
    		if (index >= 0 && index < GetAccelerationCount ())
    			GetAcceleration (index, acc);
    		else
    			RaiseMonoException ("Index out of bounds.");
    		return acc;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION iPhoneInput_Get_Custom_PropAccelerationEventCount()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_Get_Custom_PropAccelerationEventCount)
    SCRIPTINGAPI_STACK_CHECK(get_accelerationEventCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_accelerationEventCount)
    return GetAccelerationCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneInput_CUSTOM_INTERNAL_get_acceleration(Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_CUSTOM_INTERNAL_get_acceleration)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_acceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_acceleration)
    { *returnValue =(GetAcceleration ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
iPhoneOrientation SCRIPT_CALL_CONVENTION iPhoneInput_Get_Custom_PropOrientation()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_Get_Custom_PropOrientation)
    SCRIPTINGAPI_STACK_CHECK(get_orientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_orientation)
    return GetOrientation();
}

SCRIPT_BINDINGS_EXPORT_DECL
LocationInfo SCRIPT_CALL_CONVENTION iPhoneInput_Get_Custom_PropLastLocation()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneInput_Get_Custom_PropLastLocation)
    SCRIPTINGAPI_STACK_CHECK(get_lastLocation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lastLocation)
    if (LocationService::GetLocationStatus() != kLocationServiceRunning)
    printf_console ("Location service updates are not enabled. Check Handheld.locationServiceStatus before querying last location.\n");
    return LocationService::GetLastLocation();
}

SCRIPT_BINDINGS_EXPORT_DECL
iPhoneScreenOrientation SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropScreenOrientation()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropScreenOrientation)
    SCRIPTINGAPI_STACK_CHECK(get_screenOrientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_screenOrientation)
    return GetScreenManager().GetScreenOrientation();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneSettings_Set_Custom_PropScreenOrientation(iPhoneScreenOrientation value)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Set_Custom_PropScreenOrientation)
    SCRIPTINGAPI_STACK_CHECK(set_screenOrientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_screenOrientation)
     GetScreenManager().RequestOrientation(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropVerticalOrientation()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropVerticalOrientation)
    SCRIPTINGAPI_STACK_CHECK(get_verticalOrientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_verticalOrientation)
    return GetScreenManager().GetScreenOrientation() == portrait;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneSettings_Set_Custom_PropVerticalOrientation(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Set_Custom_PropVerticalOrientation)
    SCRIPTINGAPI_STACK_CHECK(set_verticalOrientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_verticalOrientation)
     GetScreenManager().SetScreenOrientation(value ? portrait : landscapeLeft); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropScreenCanDarken()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropScreenCanDarken)
    SCRIPTINGAPI_STACK_CHECK(get_screenCanDarken)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_screenCanDarken)
    return IsIdleTimerEnabled();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneSettings_Set_Custom_PropScreenCanDarken(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Set_Custom_PropScreenCanDarken)
    SCRIPTINGAPI_STACK_CHECK(set_screenCanDarken)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_screenCanDarken)
     SetIdleTimerEnabled(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropUniqueIdentifier()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropUniqueIdentifier)
    SCRIPTINGAPI_STACK_CHECK(get_uniqueIdentifier)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_uniqueIdentifier)
    return CreateScriptingString(systeminfo::GetDeviceUniqueIdentifier());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropName()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropName)
    SCRIPTINGAPI_STACK_CHECK(get_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_name)
    return CreateScriptingString(systeminfo::GetDeviceName());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropModel()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropModel)
    SCRIPTINGAPI_STACK_CHECK(get_model)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_model)
    return CreateScriptingString(systeminfo::GetDeviceModel());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropSystemName()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropSystemName)
    SCRIPTINGAPI_STACK_CHECK(get_systemName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_systemName)
    return CreateScriptingString(systeminfo::GetDeviceSystemName());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropSystemVersion()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropSystemVersion)
    SCRIPTINGAPI_STACK_CHECK(get_systemVersion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_systemVersion)
    return CreateScriptingString(systeminfo::GetDeviceSystemVersion());
}

SCRIPT_BINDINGS_EXPORT_DECL
iPhoneNetworkReachability SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropInternetReachability()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropInternetReachability)
    SCRIPTINGAPI_STACK_CHECK(get_internetReachability)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_internetReachability)
    return GetInternetReachability();
}

SCRIPT_BINDINGS_EXPORT_DECL
iPhoneGeneration SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropGeneration()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropGeneration)
    SCRIPTINGAPI_STACK_CHECK(get_generation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_generation)
    return iphone::GetDeviceGeneration();
}

SCRIPT_BINDINGS_EXPORT_DECL
LocationServiceStatus SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropLocationServiceStatus()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropLocationServiceStatus)
    SCRIPTINGAPI_STACK_CHECK(get_locationServiceStatus)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_locationServiceStatus)
    return LocationService::GetLocationStatus();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneSettings_Get_Custom_PropLocationServiceEnabledByUser()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_Get_Custom_PropLocationServiceEnabledByUser)
    SCRIPTINGAPI_STACK_CHECK(get_locationServiceEnabledByUser)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_locationServiceEnabledByUser)
    return LocationService::IsServiceEnabledByUser();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneSettings_CUSTOM_StartLocationServiceUpdates(float desiredAccuracyInMeters, float updateDistanceInMeters)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_CUSTOM_StartLocationServiceUpdates)
    SCRIPTINGAPI_STACK_CHECK(StartLocationServiceUpdates)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StartLocationServiceUpdates)
    
    		LocationService::SetDesiredAccuracy(desiredAccuracyInMeters);
    		LocationService::SetDistanceFilter(updateDistanceInMeters);
    		LocationService::StartUpdatingLocation();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneSettings_CUSTOM_StopLocationServiceUpdates()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneSettings_CUSTOM_StopLocationServiceUpdates)
    SCRIPTINGAPI_STACK_CHECK(StopLocationServiceUpdates)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopLocationServiceUpdates)
    
    		LocationService::StopUpdatingLocation();
    	
}

#endif


struct MonoiPhoneKeyboard_InternalConstructorHelperArguments {
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
void SCRIPT_CALL_CONVENTION iPhoneKeyboard_CUSTOM_Destroy(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_CUSTOM_Destroy)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Destroy)
    
    		if (GET)
            {
    			delete GET;
    			GET=0;
            }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneKeyboard_CUSTOM_iPhoneKeyboard_InternalConstructorHelper(ICallType_ReadOnlyUnityEngineObject_Argument self_, iPhoneKeyboard_InternalConstructorHelperArguments arguments)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_CUSTOM_iPhoneKeyboard_InternalConstructorHelper)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(iPhoneKeyboard_InternalConstructorHelper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(iPhoneKeyboard_InternalConstructorHelper)
    
    		GET = new KeyboardOnScreen(scripting_cpp_string_for(arguments.text),
    			arguments.keyboardType, arguments.autocorrection, arguments.multiline, arguments.secure, arguments.alert,
    			scripting_cpp_string_for(arguments.textPlaceholder));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION iPhoneKeyboard_Get_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Get_Custom_PropText)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_text)
    if (GET) return CreateScriptingString(GET->getText());
    else return CreateScriptingString("");
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneKeyboard_Set_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Set_Custom_PropText)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_text)
    
    if (GET) GET->setText(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneKeyboard_Get_Custom_PropHideInput()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Get_Custom_PropHideInput)
    SCRIPTINGAPI_STACK_CHECK(get_hideInput)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hideInput)
    return KeyboardOnScreen::isInputHidden();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneKeyboard_Set_Custom_PropHideInput(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Set_Custom_PropHideInput)
    SCRIPTINGAPI_STACK_CHECK(set_hideInput)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hideInput)
    
    KeyboardOnScreen::setInputHidden(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneKeyboard_Get_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Get_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_active)
    if (GET) return (short)GET->isActive();
    else return false;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneKeyboard_Set_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Set_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_active)
    
    if (GET) GET->setActive(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneKeyboard_Get_Custom_PropDone(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Get_Custom_PropDone)
    ReadOnlyScriptingObjectOfType<iPhoneKeyboard> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_done)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_done)
    if (GET) return (short)GET->isDone();
    else return false;
}

#endif

 #undef GET
#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneKeyboard_CUSTOM_INTERNAL_get_area(Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_CUSTOM_INTERNAL_get_area)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_area)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_area)
    { *returnValue =(KeyboardOnScreen::GetRect()); return;};
}

#endif

#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneKeyboard_Get_Custom_PropVisible()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneKeyboard_Get_Custom_PropVisible)
    SCRIPTINGAPI_STACK_CHECK(get_visible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_visible)
    return KeyboardOnScreen::IsVisible();
}

#endif

#if UNITY_IPHONE_API || UNITY_ANDROID_API
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovie(ICallType_String_Argument path_, const ColorRGBAf& bgColor, int controlMode, int scalingMode)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovie)
    ICallType_String_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_PlayMovie)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_PlayMovie)
    
    		PlayFullScreenMovie (path, bgColor, (unsigned)controlMode, (unsigned)scalingMode);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovieURL(ICallType_String_Argument url_, const ColorRGBAf& bgColor, int controlMode, int scalingMode)
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovieURL)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_PlayMovieURL)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_PlayMovieURL)
    
    		PlayFullScreenMovie (url, bgColor, (unsigned)controlMode, (unsigned)scalingMode);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION iPhoneUtils_CUSTOM_Vibrate()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneUtils_CUSTOM_Vibrate)
    SCRIPTINGAPI_STACK_CHECK(Vibrate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Vibrate)
    
    		Vibrate ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneUtils_Get_Custom_PropIsApplicationGenuine()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneUtils_Get_Custom_PropIsApplicationGenuine)
    SCRIPTINGAPI_STACK_CHECK(get_isApplicationGenuine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isApplicationGenuine)
    return IsApplicationGenuine ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION iPhoneUtils_Get_Custom_PropIsApplicationGenuineAvailable()
{
    SCRIPTINGAPI_ETW_ENTRY(iPhoneUtils_Get_Custom_PropIsApplicationGenuineAvailable)
    SCRIPTINGAPI_STACK_CHECK(get_isApplicationGenuineAvailable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isApplicationGenuineAvailable)
    return IsApplicationGenuineAvailable();
}

#endif


 #define GET ExtractMonoObjectData<iPhoneLocalNotification*> (self)
#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION LocalNotification_CUSTOM_GetFireDate(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_CUSTOM_GetFireDate)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetFireDate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetFireDate)
    
    		return GET->GetFireDate();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_CUSTOM_SetFireDate(ICallType_Object_Argument self_, double dt)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_CUSTOM_SetFireDate)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetFireDate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFireDate)
    
    		GET->SetFireDate(dt);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropTimeZone(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropTimeZone)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_timeZone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_timeZone)
    const char *timeZone = GET->GetTimeZone();
    return (timeZone ? CreateScriptingString(timeZone) : 0);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropTimeZone(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropTimeZone)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_timeZone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_timeZone)
    
    GET->SetTimeZone(value.IsNull() ? 0 : value.ToUTF8().c_str());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
unsigned SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropRepeatInterval(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropRepeatInterval)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_repeatInterval)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_repeatInterval)
    return GET->GetRepeatInterval();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropRepeatInterval(ICallType_Object_Argument self_, unsigned value)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropRepeatInterval)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_repeatInterval)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_repeatInterval)
    
    GET->SetRepeatInterval(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
CalendarIdentifier SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropRepeatCalendar(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropRepeatCalendar)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_repeatCalendar)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_repeatCalendar)
    return GET->GetRepeatCalendar();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropRepeatCalendar(ICallType_Object_Argument self_, CalendarIdentifier value)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropRepeatCalendar)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_repeatCalendar)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_repeatCalendar)
    
    GET->SetRepeatCalendar(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropAlertBody(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropAlertBody)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alertBody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alertBody)
    const char *message = GET->GetAlertBody();
    return (message ? CreateScriptingString(message) : 0);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropAlertBody(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropAlertBody)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_alertBody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alertBody)
    
    GET->SetAlertBody(value.IsNull() ? 0 : value.ToUTF8().c_str());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropAlertAction(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropAlertAction)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alertAction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alertAction)
    const char *action = GET->GetAlertAction();
    return (action ? CreateScriptingString(action) : 0);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropAlertAction(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropAlertAction)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_alertAction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alertAction)
    
    GET->SetAlertAction(value.IsNull() ? 0 : value.ToUTF8().c_str());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropHasAction(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropHasAction)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hasAction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hasAction)
    return GET->HasAction();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropHasAction(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropHasAction)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_hasAction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hasAction)
    
    GET->HasAction(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropAlertLaunchImage(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropAlertLaunchImage)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alertLaunchImage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alertLaunchImage)
    const char *path = GET->GetAlertLaunchImage();
    return (path ? CreateScriptingString(path) : 0);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropAlertLaunchImage(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropAlertLaunchImage)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_alertLaunchImage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alertLaunchImage)
    
    GET->SetAlertLaunchImage(value.IsNull () ? 0 : value.ToUTF8().c_str());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropApplicationIconBadgeNumber(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropApplicationIconBadgeNumber)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_applicationIconBadgeNumber)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_applicationIconBadgeNumber)
    return GET->GetApplicationIconBadgeNumber();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropApplicationIconBadgeNumber(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropApplicationIconBadgeNumber)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_applicationIconBadgeNumber)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_applicationIconBadgeNumber)
    
    GET->SetApplicationIconBadgeNumber(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropSoundName(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropSoundName)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_soundName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_soundName)
    const char *path = GET->GetSoundName();
    return (path ? CreateScriptingString(path) : 0);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropSoundName(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropSoundName)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_soundName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_soundName)
    
    GET->SetSoundName(value.IsNull() ? 0 : value.ToUTF8().c_str());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropDefaultSoundName()
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropDefaultSoundName)
    SCRIPTINGAPI_STACK_CHECK(get_defaultSoundName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_defaultSoundName)
    const char *path = iPhoneLocalNotification::GetDefaultSoundName();
    return (path ? CreateScriptingString(path) : 0);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION LocalNotification_Get_Custom_PropUserInfo(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Get_Custom_PropUserInfo)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_userInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_userInfo)
    return GET->GetUserInfo();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_Set_Custom_PropUserInfo(ICallType_Object_Argument self_, ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_Set_Custom_PropUserInfo)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_Object_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_userInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_userInfo)
    
    GET->SetUserInfo(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_CUSTOM_Destroy(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_CUSTOM_Destroy)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Destroy)
    
    		if (GET)
    		{
    			delete GET;
    			GET = 0;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocalNotification_CUSTOM_InitWrapper(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocalNotification_CUSTOM_InitWrapper)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InitWrapper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitWrapper)
    
    		GET = new iPhoneLocalNotification;
    	
}

#endif

 #undef GET

 #define GET ExtractMonoObjectData<iPhoneRemoteNotification*> (self)
#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION RemoteNotification_Get_Custom_PropAlertBody(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RemoteNotification_Get_Custom_PropAlertBody)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alertBody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alertBody)
    const char *message = GET->GetAlertBody();
    return (message ? CreateScriptingString(message) : 0);
}

#endif

#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RemoteNotification_Get_Custom_PropHasAction(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RemoteNotification_Get_Custom_PropHasAction)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hasAction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hasAction)
    return GET->HasAction();
}

#endif

#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RemoteNotification_Get_Custom_PropApplicationIconBadgeNumber(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RemoteNotification_Get_Custom_PropApplicationIconBadgeNumber)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_applicationIconBadgeNumber)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_applicationIconBadgeNumber)
    return GET->GetApplicationIconBadgeNumber();
}

#endif

#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION RemoteNotification_Get_Custom_PropSoundName(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RemoteNotification_Get_Custom_PropSoundName)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_soundName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_soundName)
    const char *path = GET->GetSoundName();
    return (path ? CreateScriptingString(path) : 0);
}

#endif

#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION RemoteNotification_Get_Custom_PropUserInfo(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RemoteNotification_Get_Custom_PropUserInfo)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_userInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_userInfo)
    return GET->GetUserInfo();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RemoteNotification_CUSTOM_Destroy(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RemoteNotification_CUSTOM_Destroy)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Destroy)
    
    		if (GET)
    		{
    			delete GET;
    			GET = 0;
    		}
    	
}

#endif

 #undef GET
#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NotificationServices_Get_Custom_PropLocalNotificationCount()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_Get_Custom_PropLocalNotificationCount)
    SCRIPTINGAPI_STACK_CHECK(get_localNotificationCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_localNotificationCount)
    return GetLocalNotificationCount();
}

#endif

#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_GetLocalNotification(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_GetLocalNotification)
    SCRIPTINGAPI_STACK_CHECK(GetLocalNotification)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetLocalNotification)
    
    		if (index >= 0 && index < GetLocalNotificationCount())
    		{
    			MonoClass *classLocalNotification;
    			MonoObject *localNotification;
    
    			classLocalNotification = GetMonoManager().GetBuiltinMonoClass("LocalNotification");
    			localNotification = mono_object_new(mono_domain_get(), classLocalNotification);
    
    			ExtractMonoObjectData<iPhoneLocalNotification*>(localNotification) = CopyLocalNotification(index);
    
    			return localNotification;
    		}
    
    		RaiseMonoException("Index out of bounds.");
    		return 0;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_ScheduleLocalNotification(ICallType_Object_Argument notification_)
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_ScheduleLocalNotification)
    ICallType_Object_Local notification(notification_);
    UNUSED(notification);
    SCRIPTINGAPI_STACK_CHECK(ScheduleLocalNotification)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ScheduleLocalNotification)
    
    		MonoRaiseIfNull(notification);
    		ExtractMonoObjectData<iPhoneLocalNotification*>(notification)->Schedule();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_PresentLocalNotificationNow(ICallType_Object_Argument notification_)
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_PresentLocalNotificationNow)
    ICallType_Object_Local notification(notification_);
    UNUSED(notification);
    SCRIPTINGAPI_STACK_CHECK(PresentLocalNotificationNow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PresentLocalNotificationNow)
    
    		MonoRaiseIfNull(notification);
    		ExtractMonoObjectData<iPhoneLocalNotification*>(notification)->PresentNow();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_CancelLocalNotification(ICallType_Object_Argument notification_)
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_CancelLocalNotification)
    ICallType_Object_Local notification(notification_);
    UNUSED(notification);
    SCRIPTINGAPI_STACK_CHECK(CancelLocalNotification)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CancelLocalNotification)
    
    		MonoRaiseIfNull(notification);
    		ExtractMonoObjectData<iPhoneLocalNotification*>(notification)->Cancel();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_CancelAllLocalNotifications()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_CancelAllLocalNotifications)
    SCRIPTINGAPI_STACK_CHECK(CancelAllLocalNotifications)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CancelAllLocalNotifications)
    
    		iPhoneLocalNotification::CancelAll();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION NotificationServices_Get_Custom_PropScheduledLocalNotifications()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_Get_Custom_PropScheduledLocalNotifications)
    SCRIPTINGAPI_STACK_CHECK(get_scheduledLocalNotifications)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_scheduledLocalNotifications)
    std::vector<iPhoneLocalNotification*> notifications = iPhoneLocalNotification::GetScheduled();
    int count = notifications.size();
    MonoClass *classLocalNotification = GetMonoManager().GetBuiltinMonoClass("LocalNotification");
    MonoArray *monoNotifications = mono_array_new(mono_domain_get(), classLocalNotification, count);
    for (int index = 0; index < count; ++index)
    {
    MonoObject *notif = mono_object_new(mono_domain_get(), classLocalNotification);
    ExtractMonoObjectData<iPhoneLocalNotification*>(notif) = notifications[index];
    SetScriptingArrayElement(monoNotifications, index, notif);
    }
    return monoNotifications;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NotificationServices_Get_Custom_PropRemoteNotificationCount()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_Get_Custom_PropRemoteNotificationCount)
    SCRIPTINGAPI_STACK_CHECK(get_remoteNotificationCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_remoteNotificationCount)
    return GetRemoteNotificationCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_GetRemoteNotification(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_GetRemoteNotification)
    SCRIPTINGAPI_STACK_CHECK(GetRemoteNotification)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetRemoteNotification)
    
    		if (index >= 0 && index < GetRemoteNotificationCount())
    		{
    			MonoClass *classRemoteNotification;
    			MonoObject *remoteNotification;
    
    			classRemoteNotification = GetMonoManager().GetBuiltinMonoClass("RemoteNotification");
    			remoteNotification = mono_object_new(mono_domain_get(), classRemoteNotification);
    
    			ExtractMonoObjectData<iPhoneRemoteNotification*>(remoteNotification) = CopyRemoteNotification(index);
    
    			return remoteNotification;
    		}
    
    		RaiseMonoException("Index out of bounds.");
    		return 0;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_ClearLocalNotifications()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_ClearLocalNotifications)
    SCRIPTINGAPI_STACK_CHECK(ClearLocalNotifications)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearLocalNotifications)
    
    		ClearLocalNotifications();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_ClearRemoteNotifications()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_ClearRemoteNotifications)
    SCRIPTINGAPI_STACK_CHECK(ClearRemoteNotifications)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearRemoteNotifications)
    
    		ClearRemoteNotifications();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_RegisterForRemoteNotificationTypes(RemoteNotificationType notificationTypes)
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_RegisterForRemoteNotificationTypes)
    SCRIPTINGAPI_STACK_CHECK(RegisterForRemoteNotificationTypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RegisterForRemoteNotificationTypes)
    
    		iPhoneRemoteNotification::Register(notificationTypes);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NotificationServices_CUSTOM_UnregisterForRemoteNotifications()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_CUSTOM_UnregisterForRemoteNotifications)
    SCRIPTINGAPI_STACK_CHECK(UnregisterForRemoteNotifications)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UnregisterForRemoteNotifications)
    
    		iPhoneRemoteNotification::Unregister();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
RemoteNotificationType SCRIPT_CALL_CONVENTION NotificationServices_Get_Custom_PropEnabledRemoteNotificationTypes()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_Get_Custom_PropEnabledRemoteNotificationTypes)
    SCRIPTINGAPI_STACK_CHECK(get_enabledRemoteNotificationTypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabledRemoteNotificationTypes)
    return iPhoneRemoteNotification::GetEnabledTypes();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION NotificationServices_Get_Custom_PropDeviceToken()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_Get_Custom_PropDeviceToken)
    SCRIPTINGAPI_STACK_CHECK(get_deviceToken)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceToken)
    const char *deviceToken = iPhoneRemoteNotification::GetDeviceToken();
    if (!deviceToken)
    return 0;
    int count = iPhoneRemoteNotification::GetDeviceTokenLength();
    MonoArray *monoDeviceToken = mono_array_new(mono_domain_get(), mono_get_byte_class(), count);
    for (int index = 0; index < count; ++index)
    {
    SetScriptingArrayElement(monoDeviceToken, index, deviceToken[index]);
    }
    return monoDeviceToken;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NotificationServices_Get_Custom_PropRegistrationError()
{
    SCRIPTINGAPI_ETW_ENTRY(NotificationServices_Get_Custom_PropRegistrationError)
    SCRIPTINGAPI_STACK_CHECK(get_registrationError)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_registrationError)
    const char *error = iPhoneRemoteNotification::GetError();
    return (error ? CreateScriptingString(error) : 0);
}

#endif

	extern void CrashedCheckBellowForHintsWhy();
#if UNITY_IPHONE_API
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION UnhandledExceptionHandler_CUSTOM_HandleUnhandledException(ICallType_Object_Argument sender_, ICallType_Object_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(UnhandledExceptionHandler_CUSTOM_HandleUnhandledException)
    ICallType_Object_Local sender(sender_);
    UNUSED(sender);
    ICallType_Object_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(HandleUnhandledException)
    
    		#if UNITY_IOS
    		if ( Thread::CurrentThreadIsMainThread() )
    		{
    			CrashedCheckBellowForHintsWhy();
    		}
    		#endif
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if UNITY_IPHONE_API || UNITY_ANDROID_API
void Register_UnityEngine_iPhoneInput_GetTouch()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::GetTouch" , (gpointer)& iPhoneInput_CUSTOM_GetTouch );
}

void Register_UnityEngine_iPhoneInput_get_touchCount()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::get_touchCount" , (gpointer)& iPhoneInput_Get_Custom_PropTouchCount );
}

void Register_UnityEngine_iPhoneInput_get_multiTouchEnabled()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::get_multiTouchEnabled" , (gpointer)& iPhoneInput_Get_Custom_PropMultiTouchEnabled );
}

void Register_UnityEngine_iPhoneInput_set_multiTouchEnabled()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::set_multiTouchEnabled" , (gpointer)& iPhoneInput_Set_Custom_PropMultiTouchEnabled );
}

void Register_UnityEngine_iPhoneInput_GetAccelerationEvent()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::GetAccelerationEvent" , (gpointer)& iPhoneInput_CUSTOM_GetAccelerationEvent );
}

void Register_UnityEngine_iPhoneInput_get_accelerationEventCount()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::get_accelerationEventCount" , (gpointer)& iPhoneInput_Get_Custom_PropAccelerationEventCount );
}

void Register_UnityEngine_iPhoneInput_INTERNAL_get_acceleration()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::INTERNAL_get_acceleration" , (gpointer)& iPhoneInput_CUSTOM_INTERNAL_get_acceleration );
}

void Register_UnityEngine_iPhoneInput_get_orientation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::get_orientation" , (gpointer)& iPhoneInput_Get_Custom_PropOrientation );
}

void Register_UnityEngine_iPhoneInput_get_lastLocation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneInput::get_lastLocation" , (gpointer)& iPhoneInput_Get_Custom_PropLastLocation );
}

void Register_UnityEngine_iPhoneSettings_get_screenOrientation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_screenOrientation" , (gpointer)& iPhoneSettings_Get_Custom_PropScreenOrientation );
}

void Register_UnityEngine_iPhoneSettings_set_screenOrientation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::set_screenOrientation" , (gpointer)& iPhoneSettings_Set_Custom_PropScreenOrientation );
}

void Register_UnityEngine_iPhoneSettings_get_verticalOrientation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_verticalOrientation" , (gpointer)& iPhoneSettings_Get_Custom_PropVerticalOrientation );
}

void Register_UnityEngine_iPhoneSettings_set_verticalOrientation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::set_verticalOrientation" , (gpointer)& iPhoneSettings_Set_Custom_PropVerticalOrientation );
}

void Register_UnityEngine_iPhoneSettings_get_screenCanDarken()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_screenCanDarken" , (gpointer)& iPhoneSettings_Get_Custom_PropScreenCanDarken );
}

void Register_UnityEngine_iPhoneSettings_set_screenCanDarken()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::set_screenCanDarken" , (gpointer)& iPhoneSettings_Set_Custom_PropScreenCanDarken );
}

void Register_UnityEngine_iPhoneSettings_get_uniqueIdentifier()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_uniqueIdentifier" , (gpointer)& iPhoneSettings_Get_Custom_PropUniqueIdentifier );
}

void Register_UnityEngine_iPhoneSettings_get_name()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_name" , (gpointer)& iPhoneSettings_Get_Custom_PropName );
}

void Register_UnityEngine_iPhoneSettings_get_model()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_model" , (gpointer)& iPhoneSettings_Get_Custom_PropModel );
}

void Register_UnityEngine_iPhoneSettings_get_systemName()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_systemName" , (gpointer)& iPhoneSettings_Get_Custom_PropSystemName );
}

void Register_UnityEngine_iPhoneSettings_get_systemVersion()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_systemVersion" , (gpointer)& iPhoneSettings_Get_Custom_PropSystemVersion );
}

void Register_UnityEngine_iPhoneSettings_get_internetReachability()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_internetReachability" , (gpointer)& iPhoneSettings_Get_Custom_PropInternetReachability );
}

void Register_UnityEngine_iPhoneSettings_get_generation()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_generation" , (gpointer)& iPhoneSettings_Get_Custom_PropGeneration );
}

void Register_UnityEngine_iPhoneSettings_get_locationServiceStatus()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_locationServiceStatus" , (gpointer)& iPhoneSettings_Get_Custom_PropLocationServiceStatus );
}

void Register_UnityEngine_iPhoneSettings_get_locationServiceEnabledByUser()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::get_locationServiceEnabledByUser" , (gpointer)& iPhoneSettings_Get_Custom_PropLocationServiceEnabledByUser );
}

void Register_UnityEngine_iPhoneSettings_StartLocationServiceUpdates()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::StartLocationServiceUpdates" , (gpointer)& iPhoneSettings_CUSTOM_StartLocationServiceUpdates );
}

void Register_UnityEngine_iPhoneSettings_StopLocationServiceUpdates()
{
    scripting_add_internal_call( "UnityEngine.iPhoneSettings::StopLocationServiceUpdates" , (gpointer)& iPhoneSettings_CUSTOM_StopLocationServiceUpdates );
}

void Register_UnityEngine_iPhoneKeyboard_Destroy()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::Destroy" , (gpointer)& iPhoneKeyboard_CUSTOM_Destroy );
}

void Register_UnityEngine_iPhoneKeyboard_iPhoneKeyboard_InternalConstructorHelper()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::iPhoneKeyboard_InternalConstructorHelper" , (gpointer)& iPhoneKeyboard_CUSTOM_iPhoneKeyboard_InternalConstructorHelper );
}

void Register_UnityEngine_iPhoneKeyboard_get_text()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::get_text" , (gpointer)& iPhoneKeyboard_Get_Custom_PropText );
}

void Register_UnityEngine_iPhoneKeyboard_set_text()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::set_text" , (gpointer)& iPhoneKeyboard_Set_Custom_PropText );
}

void Register_UnityEngine_iPhoneKeyboard_get_hideInput()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::get_hideInput" , (gpointer)& iPhoneKeyboard_Get_Custom_PropHideInput );
}

void Register_UnityEngine_iPhoneKeyboard_set_hideInput()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::set_hideInput" , (gpointer)& iPhoneKeyboard_Set_Custom_PropHideInput );
}

void Register_UnityEngine_iPhoneKeyboard_get_active()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::get_active" , (gpointer)& iPhoneKeyboard_Get_Custom_PropActive );
}

void Register_UnityEngine_iPhoneKeyboard_set_active()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::set_active" , (gpointer)& iPhoneKeyboard_Set_Custom_PropActive );
}

void Register_UnityEngine_iPhoneKeyboard_get_done()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::get_done" , (gpointer)& iPhoneKeyboard_Get_Custom_PropDone );
}

void Register_UnityEngine_iPhoneKeyboard_INTERNAL_get_area()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::INTERNAL_get_area" , (gpointer)& iPhoneKeyboard_CUSTOM_INTERNAL_get_area );
}

void Register_UnityEngine_iPhoneKeyboard_get_visible()
{
    scripting_add_internal_call( "UnityEngine.iPhoneKeyboard::get_visible" , (gpointer)& iPhoneKeyboard_Get_Custom_PropVisible );
}

void Register_UnityEngine_iPhoneUtils_INTERNAL_CALL_PlayMovie()
{
    scripting_add_internal_call( "UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovie" , (gpointer)& iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovie );
}

void Register_UnityEngine_iPhoneUtils_INTERNAL_CALL_PlayMovieURL()
{
    scripting_add_internal_call( "UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovieURL" , (gpointer)& iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovieURL );
}

void Register_UnityEngine_iPhoneUtils_Vibrate()
{
    scripting_add_internal_call( "UnityEngine.iPhoneUtils::Vibrate" , (gpointer)& iPhoneUtils_CUSTOM_Vibrate );
}

void Register_UnityEngine_iPhoneUtils_get_isApplicationGenuine()
{
    scripting_add_internal_call( "UnityEngine.iPhoneUtils::get_isApplicationGenuine" , (gpointer)& iPhoneUtils_Get_Custom_PropIsApplicationGenuine );
}

void Register_UnityEngine_iPhoneUtils_get_isApplicationGenuineAvailable()
{
    scripting_add_internal_call( "UnityEngine.iPhoneUtils::get_isApplicationGenuineAvailable" , (gpointer)& iPhoneUtils_Get_Custom_PropIsApplicationGenuineAvailable );
}

#endif
#if UNITY_IPHONE_API
void Register_UnityEngine_LocalNotification_GetFireDate()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::GetFireDate" , (gpointer)& LocalNotification_CUSTOM_GetFireDate );
}

void Register_UnityEngine_LocalNotification_SetFireDate()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::SetFireDate" , (gpointer)& LocalNotification_CUSTOM_SetFireDate );
}

void Register_UnityEngine_LocalNotification_get_timeZone()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_timeZone" , (gpointer)& LocalNotification_Get_Custom_PropTimeZone );
}

void Register_UnityEngine_LocalNotification_set_timeZone()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_timeZone" , (gpointer)& LocalNotification_Set_Custom_PropTimeZone );
}

void Register_UnityEngine_LocalNotification_get_repeatInterval()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_repeatInterval" , (gpointer)& LocalNotification_Get_Custom_PropRepeatInterval );
}

void Register_UnityEngine_LocalNotification_set_repeatInterval()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_repeatInterval" , (gpointer)& LocalNotification_Set_Custom_PropRepeatInterval );
}

void Register_UnityEngine_LocalNotification_get_repeatCalendar()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_repeatCalendar" , (gpointer)& LocalNotification_Get_Custom_PropRepeatCalendar );
}

void Register_UnityEngine_LocalNotification_set_repeatCalendar()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_repeatCalendar" , (gpointer)& LocalNotification_Set_Custom_PropRepeatCalendar );
}

void Register_UnityEngine_LocalNotification_get_alertBody()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_alertBody" , (gpointer)& LocalNotification_Get_Custom_PropAlertBody );
}

void Register_UnityEngine_LocalNotification_set_alertBody()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_alertBody" , (gpointer)& LocalNotification_Set_Custom_PropAlertBody );
}

void Register_UnityEngine_LocalNotification_get_alertAction()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_alertAction" , (gpointer)& LocalNotification_Get_Custom_PropAlertAction );
}

void Register_UnityEngine_LocalNotification_set_alertAction()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_alertAction" , (gpointer)& LocalNotification_Set_Custom_PropAlertAction );
}

void Register_UnityEngine_LocalNotification_get_hasAction()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_hasAction" , (gpointer)& LocalNotification_Get_Custom_PropHasAction );
}

void Register_UnityEngine_LocalNotification_set_hasAction()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_hasAction" , (gpointer)& LocalNotification_Set_Custom_PropHasAction );
}

void Register_UnityEngine_LocalNotification_get_alertLaunchImage()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_alertLaunchImage" , (gpointer)& LocalNotification_Get_Custom_PropAlertLaunchImage );
}

void Register_UnityEngine_LocalNotification_set_alertLaunchImage()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_alertLaunchImage" , (gpointer)& LocalNotification_Set_Custom_PropAlertLaunchImage );
}

void Register_UnityEngine_LocalNotification_get_applicationIconBadgeNumber()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_applicationIconBadgeNumber" , (gpointer)& LocalNotification_Get_Custom_PropApplicationIconBadgeNumber );
}

void Register_UnityEngine_LocalNotification_set_applicationIconBadgeNumber()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_applicationIconBadgeNumber" , (gpointer)& LocalNotification_Set_Custom_PropApplicationIconBadgeNumber );
}

void Register_UnityEngine_LocalNotification_get_soundName()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_soundName" , (gpointer)& LocalNotification_Get_Custom_PropSoundName );
}

void Register_UnityEngine_LocalNotification_set_soundName()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_soundName" , (gpointer)& LocalNotification_Set_Custom_PropSoundName );
}

void Register_UnityEngine_LocalNotification_get_defaultSoundName()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_defaultSoundName" , (gpointer)& LocalNotification_Get_Custom_PropDefaultSoundName );
}

void Register_UnityEngine_LocalNotification_get_userInfo()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::get_userInfo" , (gpointer)& LocalNotification_Get_Custom_PropUserInfo );
}

void Register_UnityEngine_LocalNotification_set_userInfo()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::set_userInfo" , (gpointer)& LocalNotification_Set_Custom_PropUserInfo );
}

void Register_UnityEngine_LocalNotification_Destroy()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::Destroy" , (gpointer)& LocalNotification_CUSTOM_Destroy );
}

void Register_UnityEngine_LocalNotification_InitWrapper()
{
    scripting_add_internal_call( "UnityEngine.LocalNotification::InitWrapper" , (gpointer)& LocalNotification_CUSTOM_InitWrapper );
}

void Register_UnityEngine_RemoteNotification_get_alertBody()
{
    scripting_add_internal_call( "UnityEngine.RemoteNotification::get_alertBody" , (gpointer)& RemoteNotification_Get_Custom_PropAlertBody );
}

void Register_UnityEngine_RemoteNotification_get_hasAction()
{
    scripting_add_internal_call( "UnityEngine.RemoteNotification::get_hasAction" , (gpointer)& RemoteNotification_Get_Custom_PropHasAction );
}

void Register_UnityEngine_RemoteNotification_get_applicationIconBadgeNumber()
{
    scripting_add_internal_call( "UnityEngine.RemoteNotification::get_applicationIconBadgeNumber" , (gpointer)& RemoteNotification_Get_Custom_PropApplicationIconBadgeNumber );
}

void Register_UnityEngine_RemoteNotification_get_soundName()
{
    scripting_add_internal_call( "UnityEngine.RemoteNotification::get_soundName" , (gpointer)& RemoteNotification_Get_Custom_PropSoundName );
}

void Register_UnityEngine_RemoteNotification_get_userInfo()
{
    scripting_add_internal_call( "UnityEngine.RemoteNotification::get_userInfo" , (gpointer)& RemoteNotification_Get_Custom_PropUserInfo );
}

void Register_UnityEngine_RemoteNotification_Destroy()
{
    scripting_add_internal_call( "UnityEngine.RemoteNotification::Destroy" , (gpointer)& RemoteNotification_CUSTOM_Destroy );
}

void Register_UnityEngine_NotificationServices_get_localNotificationCount()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::get_localNotificationCount" , (gpointer)& NotificationServices_Get_Custom_PropLocalNotificationCount );
}

void Register_UnityEngine_NotificationServices_GetLocalNotification()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::GetLocalNotification" , (gpointer)& NotificationServices_CUSTOM_GetLocalNotification );
}

void Register_UnityEngine_NotificationServices_ScheduleLocalNotification()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::ScheduleLocalNotification" , (gpointer)& NotificationServices_CUSTOM_ScheduleLocalNotification );
}

void Register_UnityEngine_NotificationServices_PresentLocalNotificationNow()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::PresentLocalNotificationNow" , (gpointer)& NotificationServices_CUSTOM_PresentLocalNotificationNow );
}

void Register_UnityEngine_NotificationServices_CancelLocalNotification()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::CancelLocalNotification" , (gpointer)& NotificationServices_CUSTOM_CancelLocalNotification );
}

void Register_UnityEngine_NotificationServices_CancelAllLocalNotifications()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::CancelAllLocalNotifications" , (gpointer)& NotificationServices_CUSTOM_CancelAllLocalNotifications );
}

void Register_UnityEngine_NotificationServices_get_scheduledLocalNotifications()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::get_scheduledLocalNotifications" , (gpointer)& NotificationServices_Get_Custom_PropScheduledLocalNotifications );
}

void Register_UnityEngine_NotificationServices_get_remoteNotificationCount()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::get_remoteNotificationCount" , (gpointer)& NotificationServices_Get_Custom_PropRemoteNotificationCount );
}

void Register_UnityEngine_NotificationServices_GetRemoteNotification()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::GetRemoteNotification" , (gpointer)& NotificationServices_CUSTOM_GetRemoteNotification );
}

void Register_UnityEngine_NotificationServices_ClearLocalNotifications()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::ClearLocalNotifications" , (gpointer)& NotificationServices_CUSTOM_ClearLocalNotifications );
}

void Register_UnityEngine_NotificationServices_ClearRemoteNotifications()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::ClearRemoteNotifications" , (gpointer)& NotificationServices_CUSTOM_ClearRemoteNotifications );
}

void Register_UnityEngine_NotificationServices_RegisterForRemoteNotificationTypes()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::RegisterForRemoteNotificationTypes" , (gpointer)& NotificationServices_CUSTOM_RegisterForRemoteNotificationTypes );
}

void Register_UnityEngine_NotificationServices_UnregisterForRemoteNotifications()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::UnregisterForRemoteNotifications" , (gpointer)& NotificationServices_CUSTOM_UnregisterForRemoteNotifications );
}

void Register_UnityEngine_NotificationServices_get_enabledRemoteNotificationTypes()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::get_enabledRemoteNotificationTypes" , (gpointer)& NotificationServices_Get_Custom_PropEnabledRemoteNotificationTypes );
}

void Register_UnityEngine_NotificationServices_get_deviceToken()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::get_deviceToken" , (gpointer)& NotificationServices_Get_Custom_PropDeviceToken );
}

void Register_UnityEngine_NotificationServices_get_registrationError()
{
    scripting_add_internal_call( "UnityEngine.NotificationServices::get_registrationError" , (gpointer)& NotificationServices_Get_Custom_PropRegistrationError );
}

void Register_UnityEngine_UnhandledExceptionHandler_HandleUnhandledException()
{
    scripting_add_internal_call( "UnityEngine.UnhandledExceptionHandler::HandleUnhandledException" , (gpointer)& UnhandledExceptionHandler_CUSTOM_HandleUnhandledException );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_iPhoneInput_IcallNames [] =
{
#if UNITY_IPHONE_API || UNITY_ANDROID_API
    "UnityEngine.iPhoneInput::GetTouch"     ,    // -> iPhoneInput_CUSTOM_GetTouch
    "UnityEngine.iPhoneInput::get_touchCount",    // -> iPhoneInput_Get_Custom_PropTouchCount
    "UnityEngine.iPhoneInput::get_multiTouchEnabled",    // -> iPhoneInput_Get_Custom_PropMultiTouchEnabled
    "UnityEngine.iPhoneInput::set_multiTouchEnabled",    // -> iPhoneInput_Set_Custom_PropMultiTouchEnabled
    "UnityEngine.iPhoneInput::GetAccelerationEvent",    // -> iPhoneInput_CUSTOM_GetAccelerationEvent
    "UnityEngine.iPhoneInput::get_accelerationEventCount",    // -> iPhoneInput_Get_Custom_PropAccelerationEventCount
    "UnityEngine.iPhoneInput::INTERNAL_get_acceleration",    // -> iPhoneInput_CUSTOM_INTERNAL_get_acceleration
    "UnityEngine.iPhoneInput::get_orientation",    // -> iPhoneInput_Get_Custom_PropOrientation
    "UnityEngine.iPhoneInput::get_lastLocation",    // -> iPhoneInput_Get_Custom_PropLastLocation
    "UnityEngine.iPhoneSettings::get_screenOrientation",    // -> iPhoneSettings_Get_Custom_PropScreenOrientation
    "UnityEngine.iPhoneSettings::set_screenOrientation",    // -> iPhoneSettings_Set_Custom_PropScreenOrientation
    "UnityEngine.iPhoneSettings::get_verticalOrientation",    // -> iPhoneSettings_Get_Custom_PropVerticalOrientation
    "UnityEngine.iPhoneSettings::set_verticalOrientation",    // -> iPhoneSettings_Set_Custom_PropVerticalOrientation
    "UnityEngine.iPhoneSettings::get_screenCanDarken",    // -> iPhoneSettings_Get_Custom_PropScreenCanDarken
    "UnityEngine.iPhoneSettings::set_screenCanDarken",    // -> iPhoneSettings_Set_Custom_PropScreenCanDarken
    "UnityEngine.iPhoneSettings::get_uniqueIdentifier",    // -> iPhoneSettings_Get_Custom_PropUniqueIdentifier
    "UnityEngine.iPhoneSettings::get_name"  ,    // -> iPhoneSettings_Get_Custom_PropName
    "UnityEngine.iPhoneSettings::get_model" ,    // -> iPhoneSettings_Get_Custom_PropModel
    "UnityEngine.iPhoneSettings::get_systemName",    // -> iPhoneSettings_Get_Custom_PropSystemName
    "UnityEngine.iPhoneSettings::get_systemVersion",    // -> iPhoneSettings_Get_Custom_PropSystemVersion
    "UnityEngine.iPhoneSettings::get_internetReachability",    // -> iPhoneSettings_Get_Custom_PropInternetReachability
    "UnityEngine.iPhoneSettings::get_generation",    // -> iPhoneSettings_Get_Custom_PropGeneration
    "UnityEngine.iPhoneSettings::get_locationServiceStatus",    // -> iPhoneSettings_Get_Custom_PropLocationServiceStatus
    "UnityEngine.iPhoneSettings::get_locationServiceEnabledByUser",    // -> iPhoneSettings_Get_Custom_PropLocationServiceEnabledByUser
    "UnityEngine.iPhoneSettings::StartLocationServiceUpdates",    // -> iPhoneSettings_CUSTOM_StartLocationServiceUpdates
    "UnityEngine.iPhoneSettings::StopLocationServiceUpdates",    // -> iPhoneSettings_CUSTOM_StopLocationServiceUpdates
    "UnityEngine.iPhoneKeyboard::Destroy"   ,    // -> iPhoneKeyboard_CUSTOM_Destroy
    "UnityEngine.iPhoneKeyboard::iPhoneKeyboard_InternalConstructorHelper",    // -> iPhoneKeyboard_CUSTOM_iPhoneKeyboard_InternalConstructorHelper
    "UnityEngine.iPhoneKeyboard::get_text"  ,    // -> iPhoneKeyboard_Get_Custom_PropText
    "UnityEngine.iPhoneKeyboard::set_text"  ,    // -> iPhoneKeyboard_Set_Custom_PropText
    "UnityEngine.iPhoneKeyboard::get_hideInput",    // -> iPhoneKeyboard_Get_Custom_PropHideInput
    "UnityEngine.iPhoneKeyboard::set_hideInput",    // -> iPhoneKeyboard_Set_Custom_PropHideInput
    "UnityEngine.iPhoneKeyboard::get_active",    // -> iPhoneKeyboard_Get_Custom_PropActive
    "UnityEngine.iPhoneKeyboard::set_active",    // -> iPhoneKeyboard_Set_Custom_PropActive
    "UnityEngine.iPhoneKeyboard::get_done"  ,    // -> iPhoneKeyboard_Get_Custom_PropDone
    "UnityEngine.iPhoneKeyboard::INTERNAL_get_area",    // -> iPhoneKeyboard_CUSTOM_INTERNAL_get_area
    "UnityEngine.iPhoneKeyboard::get_visible",    // -> iPhoneKeyboard_Get_Custom_PropVisible
    "UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovie",    // -> iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovie
    "UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovieURL",    // -> iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovieURL
    "UnityEngine.iPhoneUtils::Vibrate"      ,    // -> iPhoneUtils_CUSTOM_Vibrate
    "UnityEngine.iPhoneUtils::get_isApplicationGenuine",    // -> iPhoneUtils_Get_Custom_PropIsApplicationGenuine
    "UnityEngine.iPhoneUtils::get_isApplicationGenuineAvailable",    // -> iPhoneUtils_Get_Custom_PropIsApplicationGenuineAvailable
#endif
#if UNITY_IPHONE_API
    "UnityEngine.LocalNotification::GetFireDate",    // -> LocalNotification_CUSTOM_GetFireDate
    "UnityEngine.LocalNotification::SetFireDate",    // -> LocalNotification_CUSTOM_SetFireDate
    "UnityEngine.LocalNotification::get_timeZone",    // -> LocalNotification_Get_Custom_PropTimeZone
    "UnityEngine.LocalNotification::set_timeZone",    // -> LocalNotification_Set_Custom_PropTimeZone
    "UnityEngine.LocalNotification::get_repeatInterval",    // -> LocalNotification_Get_Custom_PropRepeatInterval
    "UnityEngine.LocalNotification::set_repeatInterval",    // -> LocalNotification_Set_Custom_PropRepeatInterval
    "UnityEngine.LocalNotification::get_repeatCalendar",    // -> LocalNotification_Get_Custom_PropRepeatCalendar
    "UnityEngine.LocalNotification::set_repeatCalendar",    // -> LocalNotification_Set_Custom_PropRepeatCalendar
    "UnityEngine.LocalNotification::get_alertBody",    // -> LocalNotification_Get_Custom_PropAlertBody
    "UnityEngine.LocalNotification::set_alertBody",    // -> LocalNotification_Set_Custom_PropAlertBody
    "UnityEngine.LocalNotification::get_alertAction",    // -> LocalNotification_Get_Custom_PropAlertAction
    "UnityEngine.LocalNotification::set_alertAction",    // -> LocalNotification_Set_Custom_PropAlertAction
    "UnityEngine.LocalNotification::get_hasAction",    // -> LocalNotification_Get_Custom_PropHasAction
    "UnityEngine.LocalNotification::set_hasAction",    // -> LocalNotification_Set_Custom_PropHasAction
    "UnityEngine.LocalNotification::get_alertLaunchImage",    // -> LocalNotification_Get_Custom_PropAlertLaunchImage
    "UnityEngine.LocalNotification::set_alertLaunchImage",    // -> LocalNotification_Set_Custom_PropAlertLaunchImage
    "UnityEngine.LocalNotification::get_applicationIconBadgeNumber",    // -> LocalNotification_Get_Custom_PropApplicationIconBadgeNumber
    "UnityEngine.LocalNotification::set_applicationIconBadgeNumber",    // -> LocalNotification_Set_Custom_PropApplicationIconBadgeNumber
    "UnityEngine.LocalNotification::get_soundName",    // -> LocalNotification_Get_Custom_PropSoundName
    "UnityEngine.LocalNotification::set_soundName",    // -> LocalNotification_Set_Custom_PropSoundName
    "UnityEngine.LocalNotification::get_defaultSoundName",    // -> LocalNotification_Get_Custom_PropDefaultSoundName
    "UnityEngine.LocalNotification::get_userInfo",    // -> LocalNotification_Get_Custom_PropUserInfo
    "UnityEngine.LocalNotification::set_userInfo",    // -> LocalNotification_Set_Custom_PropUserInfo
    "UnityEngine.LocalNotification::Destroy",    // -> LocalNotification_CUSTOM_Destroy
    "UnityEngine.LocalNotification::InitWrapper",    // -> LocalNotification_CUSTOM_InitWrapper
    "UnityEngine.RemoteNotification::get_alertBody",    // -> RemoteNotification_Get_Custom_PropAlertBody
    "UnityEngine.RemoteNotification::get_hasAction",    // -> RemoteNotification_Get_Custom_PropHasAction
    "UnityEngine.RemoteNotification::get_applicationIconBadgeNumber",    // -> RemoteNotification_Get_Custom_PropApplicationIconBadgeNumber
    "UnityEngine.RemoteNotification::get_soundName",    // -> RemoteNotification_Get_Custom_PropSoundName
    "UnityEngine.RemoteNotification::get_userInfo",    // -> RemoteNotification_Get_Custom_PropUserInfo
    "UnityEngine.RemoteNotification::Destroy",    // -> RemoteNotification_CUSTOM_Destroy
    "UnityEngine.NotificationServices::get_localNotificationCount",    // -> NotificationServices_Get_Custom_PropLocalNotificationCount
    "UnityEngine.NotificationServices::GetLocalNotification",    // -> NotificationServices_CUSTOM_GetLocalNotification
    "UnityEngine.NotificationServices::ScheduleLocalNotification",    // -> NotificationServices_CUSTOM_ScheduleLocalNotification
    "UnityEngine.NotificationServices::PresentLocalNotificationNow",    // -> NotificationServices_CUSTOM_PresentLocalNotificationNow
    "UnityEngine.NotificationServices::CancelLocalNotification",    // -> NotificationServices_CUSTOM_CancelLocalNotification
    "UnityEngine.NotificationServices::CancelAllLocalNotifications",    // -> NotificationServices_CUSTOM_CancelAllLocalNotifications
    "UnityEngine.NotificationServices::get_scheduledLocalNotifications",    // -> NotificationServices_Get_Custom_PropScheduledLocalNotifications
    "UnityEngine.NotificationServices::get_remoteNotificationCount",    // -> NotificationServices_Get_Custom_PropRemoteNotificationCount
    "UnityEngine.NotificationServices::GetRemoteNotification",    // -> NotificationServices_CUSTOM_GetRemoteNotification
    "UnityEngine.NotificationServices::ClearLocalNotifications",    // -> NotificationServices_CUSTOM_ClearLocalNotifications
    "UnityEngine.NotificationServices::ClearRemoteNotifications",    // -> NotificationServices_CUSTOM_ClearRemoteNotifications
    "UnityEngine.NotificationServices::RegisterForRemoteNotificationTypes",    // -> NotificationServices_CUSTOM_RegisterForRemoteNotificationTypes
    "UnityEngine.NotificationServices::UnregisterForRemoteNotifications",    // -> NotificationServices_CUSTOM_UnregisterForRemoteNotifications
    "UnityEngine.NotificationServices::get_enabledRemoteNotificationTypes",    // -> NotificationServices_Get_Custom_PropEnabledRemoteNotificationTypes
    "UnityEngine.NotificationServices::get_deviceToken",    // -> NotificationServices_Get_Custom_PropDeviceToken
    "UnityEngine.NotificationServices::get_registrationError",    // -> NotificationServices_Get_Custom_PropRegistrationError
    "UnityEngine.UnhandledExceptionHandler::HandleUnhandledException",    // -> UnhandledExceptionHandler_CUSTOM_HandleUnhandledException
#endif
    NULL
};

static const void* s_iPhoneInput_IcallFuncs [] =
{
#if UNITY_IPHONE_API || UNITY_ANDROID_API
    (const void*)&iPhoneInput_CUSTOM_GetTouch             ,  //  <- UnityEngine.iPhoneInput::GetTouch
    (const void*)&iPhoneInput_Get_Custom_PropTouchCount   ,  //  <- UnityEngine.iPhoneInput::get_touchCount
    (const void*)&iPhoneInput_Get_Custom_PropMultiTouchEnabled,  //  <- UnityEngine.iPhoneInput::get_multiTouchEnabled
    (const void*)&iPhoneInput_Set_Custom_PropMultiTouchEnabled,  //  <- UnityEngine.iPhoneInput::set_multiTouchEnabled
    (const void*)&iPhoneInput_CUSTOM_GetAccelerationEvent ,  //  <- UnityEngine.iPhoneInput::GetAccelerationEvent
    (const void*)&iPhoneInput_Get_Custom_PropAccelerationEventCount,  //  <- UnityEngine.iPhoneInput::get_accelerationEventCount
    (const void*)&iPhoneInput_CUSTOM_INTERNAL_get_acceleration,  //  <- UnityEngine.iPhoneInput::INTERNAL_get_acceleration
    (const void*)&iPhoneInput_Get_Custom_PropOrientation  ,  //  <- UnityEngine.iPhoneInput::get_orientation
    (const void*)&iPhoneInput_Get_Custom_PropLastLocation ,  //  <- UnityEngine.iPhoneInput::get_lastLocation
    (const void*)&iPhoneSettings_Get_Custom_PropScreenOrientation,  //  <- UnityEngine.iPhoneSettings::get_screenOrientation
    (const void*)&iPhoneSettings_Set_Custom_PropScreenOrientation,  //  <- UnityEngine.iPhoneSettings::set_screenOrientation
    (const void*)&iPhoneSettings_Get_Custom_PropVerticalOrientation,  //  <- UnityEngine.iPhoneSettings::get_verticalOrientation
    (const void*)&iPhoneSettings_Set_Custom_PropVerticalOrientation,  //  <- UnityEngine.iPhoneSettings::set_verticalOrientation
    (const void*)&iPhoneSettings_Get_Custom_PropScreenCanDarken,  //  <- UnityEngine.iPhoneSettings::get_screenCanDarken
    (const void*)&iPhoneSettings_Set_Custom_PropScreenCanDarken,  //  <- UnityEngine.iPhoneSettings::set_screenCanDarken
    (const void*)&iPhoneSettings_Get_Custom_PropUniqueIdentifier,  //  <- UnityEngine.iPhoneSettings::get_uniqueIdentifier
    (const void*)&iPhoneSettings_Get_Custom_PropName      ,  //  <- UnityEngine.iPhoneSettings::get_name
    (const void*)&iPhoneSettings_Get_Custom_PropModel     ,  //  <- UnityEngine.iPhoneSettings::get_model
    (const void*)&iPhoneSettings_Get_Custom_PropSystemName,  //  <- UnityEngine.iPhoneSettings::get_systemName
    (const void*)&iPhoneSettings_Get_Custom_PropSystemVersion,  //  <- UnityEngine.iPhoneSettings::get_systemVersion
    (const void*)&iPhoneSettings_Get_Custom_PropInternetReachability,  //  <- UnityEngine.iPhoneSettings::get_internetReachability
    (const void*)&iPhoneSettings_Get_Custom_PropGeneration,  //  <- UnityEngine.iPhoneSettings::get_generation
    (const void*)&iPhoneSettings_Get_Custom_PropLocationServiceStatus,  //  <- UnityEngine.iPhoneSettings::get_locationServiceStatus
    (const void*)&iPhoneSettings_Get_Custom_PropLocationServiceEnabledByUser,  //  <- UnityEngine.iPhoneSettings::get_locationServiceEnabledByUser
    (const void*)&iPhoneSettings_CUSTOM_StartLocationServiceUpdates,  //  <- UnityEngine.iPhoneSettings::StartLocationServiceUpdates
    (const void*)&iPhoneSettings_CUSTOM_StopLocationServiceUpdates,  //  <- UnityEngine.iPhoneSettings::StopLocationServiceUpdates
    (const void*)&iPhoneKeyboard_CUSTOM_Destroy           ,  //  <- UnityEngine.iPhoneKeyboard::Destroy
    (const void*)&iPhoneKeyboard_CUSTOM_iPhoneKeyboard_InternalConstructorHelper,  //  <- UnityEngine.iPhoneKeyboard::iPhoneKeyboard_InternalConstructorHelper
    (const void*)&iPhoneKeyboard_Get_Custom_PropText      ,  //  <- UnityEngine.iPhoneKeyboard::get_text
    (const void*)&iPhoneKeyboard_Set_Custom_PropText      ,  //  <- UnityEngine.iPhoneKeyboard::set_text
    (const void*)&iPhoneKeyboard_Get_Custom_PropHideInput ,  //  <- UnityEngine.iPhoneKeyboard::get_hideInput
    (const void*)&iPhoneKeyboard_Set_Custom_PropHideInput ,  //  <- UnityEngine.iPhoneKeyboard::set_hideInput
    (const void*)&iPhoneKeyboard_Get_Custom_PropActive    ,  //  <- UnityEngine.iPhoneKeyboard::get_active
    (const void*)&iPhoneKeyboard_Set_Custom_PropActive    ,  //  <- UnityEngine.iPhoneKeyboard::set_active
    (const void*)&iPhoneKeyboard_Get_Custom_PropDone      ,  //  <- UnityEngine.iPhoneKeyboard::get_done
    (const void*)&iPhoneKeyboard_CUSTOM_INTERNAL_get_area ,  //  <- UnityEngine.iPhoneKeyboard::INTERNAL_get_area
    (const void*)&iPhoneKeyboard_Get_Custom_PropVisible   ,  //  <- UnityEngine.iPhoneKeyboard::get_visible
    (const void*)&iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovie,  //  <- UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovie
    (const void*)&iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovieURL,  //  <- UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovieURL
    (const void*)&iPhoneUtils_CUSTOM_Vibrate              ,  //  <- UnityEngine.iPhoneUtils::Vibrate
    (const void*)&iPhoneUtils_Get_Custom_PropIsApplicationGenuine,  //  <- UnityEngine.iPhoneUtils::get_isApplicationGenuine
    (const void*)&iPhoneUtils_Get_Custom_PropIsApplicationGenuineAvailable,  //  <- UnityEngine.iPhoneUtils::get_isApplicationGenuineAvailable
#endif
#if UNITY_IPHONE_API
    (const void*)&LocalNotification_CUSTOM_GetFireDate    ,  //  <- UnityEngine.LocalNotification::GetFireDate
    (const void*)&LocalNotification_CUSTOM_SetFireDate    ,  //  <- UnityEngine.LocalNotification::SetFireDate
    (const void*)&LocalNotification_Get_Custom_PropTimeZone,  //  <- UnityEngine.LocalNotification::get_timeZone
    (const void*)&LocalNotification_Set_Custom_PropTimeZone,  //  <- UnityEngine.LocalNotification::set_timeZone
    (const void*)&LocalNotification_Get_Custom_PropRepeatInterval,  //  <- UnityEngine.LocalNotification::get_repeatInterval
    (const void*)&LocalNotification_Set_Custom_PropRepeatInterval,  //  <- UnityEngine.LocalNotification::set_repeatInterval
    (const void*)&LocalNotification_Get_Custom_PropRepeatCalendar,  //  <- UnityEngine.LocalNotification::get_repeatCalendar
    (const void*)&LocalNotification_Set_Custom_PropRepeatCalendar,  //  <- UnityEngine.LocalNotification::set_repeatCalendar
    (const void*)&LocalNotification_Get_Custom_PropAlertBody,  //  <- UnityEngine.LocalNotification::get_alertBody
    (const void*)&LocalNotification_Set_Custom_PropAlertBody,  //  <- UnityEngine.LocalNotification::set_alertBody
    (const void*)&LocalNotification_Get_Custom_PropAlertAction,  //  <- UnityEngine.LocalNotification::get_alertAction
    (const void*)&LocalNotification_Set_Custom_PropAlertAction,  //  <- UnityEngine.LocalNotification::set_alertAction
    (const void*)&LocalNotification_Get_Custom_PropHasAction,  //  <- UnityEngine.LocalNotification::get_hasAction
    (const void*)&LocalNotification_Set_Custom_PropHasAction,  //  <- UnityEngine.LocalNotification::set_hasAction
    (const void*)&LocalNotification_Get_Custom_PropAlertLaunchImage,  //  <- UnityEngine.LocalNotification::get_alertLaunchImage
    (const void*)&LocalNotification_Set_Custom_PropAlertLaunchImage,  //  <- UnityEngine.LocalNotification::set_alertLaunchImage
    (const void*)&LocalNotification_Get_Custom_PropApplicationIconBadgeNumber,  //  <- UnityEngine.LocalNotification::get_applicationIconBadgeNumber
    (const void*)&LocalNotification_Set_Custom_PropApplicationIconBadgeNumber,  //  <- UnityEngine.LocalNotification::set_applicationIconBadgeNumber
    (const void*)&LocalNotification_Get_Custom_PropSoundName,  //  <- UnityEngine.LocalNotification::get_soundName
    (const void*)&LocalNotification_Set_Custom_PropSoundName,  //  <- UnityEngine.LocalNotification::set_soundName
    (const void*)&LocalNotification_Get_Custom_PropDefaultSoundName,  //  <- UnityEngine.LocalNotification::get_defaultSoundName
    (const void*)&LocalNotification_Get_Custom_PropUserInfo,  //  <- UnityEngine.LocalNotification::get_userInfo
    (const void*)&LocalNotification_Set_Custom_PropUserInfo,  //  <- UnityEngine.LocalNotification::set_userInfo
    (const void*)&LocalNotification_CUSTOM_Destroy        ,  //  <- UnityEngine.LocalNotification::Destroy
    (const void*)&LocalNotification_CUSTOM_InitWrapper    ,  //  <- UnityEngine.LocalNotification::InitWrapper
    (const void*)&RemoteNotification_Get_Custom_PropAlertBody,  //  <- UnityEngine.RemoteNotification::get_alertBody
    (const void*)&RemoteNotification_Get_Custom_PropHasAction,  //  <- UnityEngine.RemoteNotification::get_hasAction
    (const void*)&RemoteNotification_Get_Custom_PropApplicationIconBadgeNumber,  //  <- UnityEngine.RemoteNotification::get_applicationIconBadgeNumber
    (const void*)&RemoteNotification_Get_Custom_PropSoundName,  //  <- UnityEngine.RemoteNotification::get_soundName
    (const void*)&RemoteNotification_Get_Custom_PropUserInfo,  //  <- UnityEngine.RemoteNotification::get_userInfo
    (const void*)&RemoteNotification_CUSTOM_Destroy       ,  //  <- UnityEngine.RemoteNotification::Destroy
    (const void*)&NotificationServices_Get_Custom_PropLocalNotificationCount,  //  <- UnityEngine.NotificationServices::get_localNotificationCount
    (const void*)&NotificationServices_CUSTOM_GetLocalNotification,  //  <- UnityEngine.NotificationServices::GetLocalNotification
    (const void*)&NotificationServices_CUSTOM_ScheduleLocalNotification,  //  <- UnityEngine.NotificationServices::ScheduleLocalNotification
    (const void*)&NotificationServices_CUSTOM_PresentLocalNotificationNow,  //  <- UnityEngine.NotificationServices::PresentLocalNotificationNow
    (const void*)&NotificationServices_CUSTOM_CancelLocalNotification,  //  <- UnityEngine.NotificationServices::CancelLocalNotification
    (const void*)&NotificationServices_CUSTOM_CancelAllLocalNotifications,  //  <- UnityEngine.NotificationServices::CancelAllLocalNotifications
    (const void*)&NotificationServices_Get_Custom_PropScheduledLocalNotifications,  //  <- UnityEngine.NotificationServices::get_scheduledLocalNotifications
    (const void*)&NotificationServices_Get_Custom_PropRemoteNotificationCount,  //  <- UnityEngine.NotificationServices::get_remoteNotificationCount
    (const void*)&NotificationServices_CUSTOM_GetRemoteNotification,  //  <- UnityEngine.NotificationServices::GetRemoteNotification
    (const void*)&NotificationServices_CUSTOM_ClearLocalNotifications,  //  <- UnityEngine.NotificationServices::ClearLocalNotifications
    (const void*)&NotificationServices_CUSTOM_ClearRemoteNotifications,  //  <- UnityEngine.NotificationServices::ClearRemoteNotifications
    (const void*)&NotificationServices_CUSTOM_RegisterForRemoteNotificationTypes,  //  <- UnityEngine.NotificationServices::RegisterForRemoteNotificationTypes
    (const void*)&NotificationServices_CUSTOM_UnregisterForRemoteNotifications,  //  <- UnityEngine.NotificationServices::UnregisterForRemoteNotifications
    (const void*)&NotificationServices_Get_Custom_PropEnabledRemoteNotificationTypes,  //  <- UnityEngine.NotificationServices::get_enabledRemoteNotificationTypes
    (const void*)&NotificationServices_Get_Custom_PropDeviceToken,  //  <- UnityEngine.NotificationServices::get_deviceToken
    (const void*)&NotificationServices_Get_Custom_PropRegistrationError,  //  <- UnityEngine.NotificationServices::get_registrationError
    (const void*)&UnhandledExceptionHandler_CUSTOM_HandleUnhandledException,  //  <- UnityEngine.UnhandledExceptionHandler::HandleUnhandledException
#endif
    NULL
};

void ExportiPhoneInputBindings();
void ExportiPhoneInputBindings()
{
    for (int i = 0; s_iPhoneInput_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_iPhoneInput_IcallNames [i], s_iPhoneInput_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportiPhoneInputBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if UNITY_IPHONE_API || UNITY_ANDROID_API
    SET_METRO_BINDING(iPhoneInput_CUSTOM_GetTouch); //  <- UnityEngine.iPhoneInput::GetTouch
    SET_METRO_BINDING(iPhoneInput_Get_Custom_PropTouchCount); //  <- UnityEngine.iPhoneInput::get_touchCount
    SET_METRO_BINDING(iPhoneInput_Get_Custom_PropMultiTouchEnabled); //  <- UnityEngine.iPhoneInput::get_multiTouchEnabled
    SET_METRO_BINDING(iPhoneInput_Set_Custom_PropMultiTouchEnabled); //  <- UnityEngine.iPhoneInput::set_multiTouchEnabled
    SET_METRO_BINDING(iPhoneInput_CUSTOM_GetAccelerationEvent); //  <- UnityEngine.iPhoneInput::GetAccelerationEvent
    SET_METRO_BINDING(iPhoneInput_Get_Custom_PropAccelerationEventCount); //  <- UnityEngine.iPhoneInput::get_accelerationEventCount
    SET_METRO_BINDING(iPhoneInput_CUSTOM_INTERNAL_get_acceleration); //  <- UnityEngine.iPhoneInput::INTERNAL_get_acceleration
    SET_METRO_BINDING(iPhoneInput_Get_Custom_PropOrientation); //  <- UnityEngine.iPhoneInput::get_orientation
    SET_METRO_BINDING(iPhoneInput_Get_Custom_PropLastLocation); //  <- UnityEngine.iPhoneInput::get_lastLocation
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropScreenOrientation); //  <- UnityEngine.iPhoneSettings::get_screenOrientation
    SET_METRO_BINDING(iPhoneSettings_Set_Custom_PropScreenOrientation); //  <- UnityEngine.iPhoneSettings::set_screenOrientation
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropVerticalOrientation); //  <- UnityEngine.iPhoneSettings::get_verticalOrientation
    SET_METRO_BINDING(iPhoneSettings_Set_Custom_PropVerticalOrientation); //  <- UnityEngine.iPhoneSettings::set_verticalOrientation
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropScreenCanDarken); //  <- UnityEngine.iPhoneSettings::get_screenCanDarken
    SET_METRO_BINDING(iPhoneSettings_Set_Custom_PropScreenCanDarken); //  <- UnityEngine.iPhoneSettings::set_screenCanDarken
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropUniqueIdentifier); //  <- UnityEngine.iPhoneSettings::get_uniqueIdentifier
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropName); //  <- UnityEngine.iPhoneSettings::get_name
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropModel); //  <- UnityEngine.iPhoneSettings::get_model
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropSystemName); //  <- UnityEngine.iPhoneSettings::get_systemName
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropSystemVersion); //  <- UnityEngine.iPhoneSettings::get_systemVersion
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropInternetReachability); //  <- UnityEngine.iPhoneSettings::get_internetReachability
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropGeneration); //  <- UnityEngine.iPhoneSettings::get_generation
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropLocationServiceStatus); //  <- UnityEngine.iPhoneSettings::get_locationServiceStatus
    SET_METRO_BINDING(iPhoneSettings_Get_Custom_PropLocationServiceEnabledByUser); //  <- UnityEngine.iPhoneSettings::get_locationServiceEnabledByUser
    SET_METRO_BINDING(iPhoneSettings_CUSTOM_StartLocationServiceUpdates); //  <- UnityEngine.iPhoneSettings::StartLocationServiceUpdates
    SET_METRO_BINDING(iPhoneSettings_CUSTOM_StopLocationServiceUpdates); //  <- UnityEngine.iPhoneSettings::StopLocationServiceUpdates
    SET_METRO_BINDING(iPhoneKeyboard_CUSTOM_Destroy); //  <- UnityEngine.iPhoneKeyboard::Destroy
    SET_METRO_BINDING(iPhoneKeyboard_CUSTOM_iPhoneKeyboard_InternalConstructorHelper); //  <- UnityEngine.iPhoneKeyboard::iPhoneKeyboard_InternalConstructorHelper
    SET_METRO_BINDING(iPhoneKeyboard_Get_Custom_PropText); //  <- UnityEngine.iPhoneKeyboard::get_text
    SET_METRO_BINDING(iPhoneKeyboard_Set_Custom_PropText); //  <- UnityEngine.iPhoneKeyboard::set_text
    SET_METRO_BINDING(iPhoneKeyboard_Get_Custom_PropHideInput); //  <- UnityEngine.iPhoneKeyboard::get_hideInput
    SET_METRO_BINDING(iPhoneKeyboard_Set_Custom_PropHideInput); //  <- UnityEngine.iPhoneKeyboard::set_hideInput
    SET_METRO_BINDING(iPhoneKeyboard_Get_Custom_PropActive); //  <- UnityEngine.iPhoneKeyboard::get_active
    SET_METRO_BINDING(iPhoneKeyboard_Set_Custom_PropActive); //  <- UnityEngine.iPhoneKeyboard::set_active
    SET_METRO_BINDING(iPhoneKeyboard_Get_Custom_PropDone); //  <- UnityEngine.iPhoneKeyboard::get_done
    SET_METRO_BINDING(iPhoneKeyboard_CUSTOM_INTERNAL_get_area); //  <- UnityEngine.iPhoneKeyboard::INTERNAL_get_area
    SET_METRO_BINDING(iPhoneKeyboard_Get_Custom_PropVisible); //  <- UnityEngine.iPhoneKeyboard::get_visible
    SET_METRO_BINDING(iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovie); //  <- UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovie
    SET_METRO_BINDING(iPhoneUtils_CUSTOM_INTERNAL_CALL_PlayMovieURL); //  <- UnityEngine.iPhoneUtils::INTERNAL_CALL_PlayMovieURL
    SET_METRO_BINDING(iPhoneUtils_CUSTOM_Vibrate); //  <- UnityEngine.iPhoneUtils::Vibrate
    SET_METRO_BINDING(iPhoneUtils_Get_Custom_PropIsApplicationGenuine); //  <- UnityEngine.iPhoneUtils::get_isApplicationGenuine
    SET_METRO_BINDING(iPhoneUtils_Get_Custom_PropIsApplicationGenuineAvailable); //  <- UnityEngine.iPhoneUtils::get_isApplicationGenuineAvailable
#endif
#if UNITY_IPHONE_API
    SET_METRO_BINDING(LocalNotification_CUSTOM_GetFireDate); //  <- UnityEngine.LocalNotification::GetFireDate
    SET_METRO_BINDING(LocalNotification_CUSTOM_SetFireDate); //  <- UnityEngine.LocalNotification::SetFireDate
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropTimeZone); //  <- UnityEngine.LocalNotification::get_timeZone
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropTimeZone); //  <- UnityEngine.LocalNotification::set_timeZone
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropRepeatInterval); //  <- UnityEngine.LocalNotification::get_repeatInterval
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropRepeatInterval); //  <- UnityEngine.LocalNotification::set_repeatInterval
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropRepeatCalendar); //  <- UnityEngine.LocalNotification::get_repeatCalendar
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropRepeatCalendar); //  <- UnityEngine.LocalNotification::set_repeatCalendar
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropAlertBody); //  <- UnityEngine.LocalNotification::get_alertBody
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropAlertBody); //  <- UnityEngine.LocalNotification::set_alertBody
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropAlertAction); //  <- UnityEngine.LocalNotification::get_alertAction
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropAlertAction); //  <- UnityEngine.LocalNotification::set_alertAction
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropHasAction); //  <- UnityEngine.LocalNotification::get_hasAction
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropHasAction); //  <- UnityEngine.LocalNotification::set_hasAction
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropAlertLaunchImage); //  <- UnityEngine.LocalNotification::get_alertLaunchImage
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropAlertLaunchImage); //  <- UnityEngine.LocalNotification::set_alertLaunchImage
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropApplicationIconBadgeNumber); //  <- UnityEngine.LocalNotification::get_applicationIconBadgeNumber
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropApplicationIconBadgeNumber); //  <- UnityEngine.LocalNotification::set_applicationIconBadgeNumber
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropSoundName); //  <- UnityEngine.LocalNotification::get_soundName
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropSoundName); //  <- UnityEngine.LocalNotification::set_soundName
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropDefaultSoundName); //  <- UnityEngine.LocalNotification::get_defaultSoundName
    SET_METRO_BINDING(LocalNotification_Get_Custom_PropUserInfo); //  <- UnityEngine.LocalNotification::get_userInfo
    SET_METRO_BINDING(LocalNotification_Set_Custom_PropUserInfo); //  <- UnityEngine.LocalNotification::set_userInfo
    SET_METRO_BINDING(LocalNotification_CUSTOM_Destroy); //  <- UnityEngine.LocalNotification::Destroy
    SET_METRO_BINDING(LocalNotification_CUSTOM_InitWrapper); //  <- UnityEngine.LocalNotification::InitWrapper
    SET_METRO_BINDING(RemoteNotification_Get_Custom_PropAlertBody); //  <- UnityEngine.RemoteNotification::get_alertBody
    SET_METRO_BINDING(RemoteNotification_Get_Custom_PropHasAction); //  <- UnityEngine.RemoteNotification::get_hasAction
    SET_METRO_BINDING(RemoteNotification_Get_Custom_PropApplicationIconBadgeNumber); //  <- UnityEngine.RemoteNotification::get_applicationIconBadgeNumber
    SET_METRO_BINDING(RemoteNotification_Get_Custom_PropSoundName); //  <- UnityEngine.RemoteNotification::get_soundName
    SET_METRO_BINDING(RemoteNotification_Get_Custom_PropUserInfo); //  <- UnityEngine.RemoteNotification::get_userInfo
    SET_METRO_BINDING(RemoteNotification_CUSTOM_Destroy); //  <- UnityEngine.RemoteNotification::Destroy
    SET_METRO_BINDING(NotificationServices_Get_Custom_PropLocalNotificationCount); //  <- UnityEngine.NotificationServices::get_localNotificationCount
    SET_METRO_BINDING(NotificationServices_CUSTOM_GetLocalNotification); //  <- UnityEngine.NotificationServices::GetLocalNotification
    SET_METRO_BINDING(NotificationServices_CUSTOM_ScheduleLocalNotification); //  <- UnityEngine.NotificationServices::ScheduleLocalNotification
    SET_METRO_BINDING(NotificationServices_CUSTOM_PresentLocalNotificationNow); //  <- UnityEngine.NotificationServices::PresentLocalNotificationNow
    SET_METRO_BINDING(NotificationServices_CUSTOM_CancelLocalNotification); //  <- UnityEngine.NotificationServices::CancelLocalNotification
    SET_METRO_BINDING(NotificationServices_CUSTOM_CancelAllLocalNotifications); //  <- UnityEngine.NotificationServices::CancelAllLocalNotifications
    SET_METRO_BINDING(NotificationServices_Get_Custom_PropScheduledLocalNotifications); //  <- UnityEngine.NotificationServices::get_scheduledLocalNotifications
    SET_METRO_BINDING(NotificationServices_Get_Custom_PropRemoteNotificationCount); //  <- UnityEngine.NotificationServices::get_remoteNotificationCount
    SET_METRO_BINDING(NotificationServices_CUSTOM_GetRemoteNotification); //  <- UnityEngine.NotificationServices::GetRemoteNotification
    SET_METRO_BINDING(NotificationServices_CUSTOM_ClearLocalNotifications); //  <- UnityEngine.NotificationServices::ClearLocalNotifications
    SET_METRO_BINDING(NotificationServices_CUSTOM_ClearRemoteNotifications); //  <- UnityEngine.NotificationServices::ClearRemoteNotifications
    SET_METRO_BINDING(NotificationServices_CUSTOM_RegisterForRemoteNotificationTypes); //  <- UnityEngine.NotificationServices::RegisterForRemoteNotificationTypes
    SET_METRO_BINDING(NotificationServices_CUSTOM_UnregisterForRemoteNotifications); //  <- UnityEngine.NotificationServices::UnregisterForRemoteNotifications
    SET_METRO_BINDING(NotificationServices_Get_Custom_PropEnabledRemoteNotificationTypes); //  <- UnityEngine.NotificationServices::get_enabledRemoteNotificationTypes
    SET_METRO_BINDING(NotificationServices_Get_Custom_PropDeviceToken); //  <- UnityEngine.NotificationServices::get_deviceToken
    SET_METRO_BINDING(NotificationServices_Get_Custom_PropRegistrationError); //  <- UnityEngine.NotificationServices::get_registrationError
    SET_METRO_BINDING(UnhandledExceptionHandler_CUSTOM_HandleUnhandledException); //  <- UnityEngine.UnhandledExceptionHandler::HandleUnhandledException
#endif
}

#endif
