#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Input/InputManager.h"
#include "Runtime/Input/GetInput.h"
#include "Runtime/Input/LocationService.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Scripting/ScriptingUtility.h"

#if SUPPORT_REPRODUCE_LOG
#include "Runtime/Misc/ReproductionLog.h"
#endif

using namespace Unity;

using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Touch_CUSTOM_Internal_GetPtr(ICallType_IntPtr_Argument ptr_)
{
    SCRIPTINGAPI_ETW_ENTRY(Touch_CUSTOM_Internal_GetPtr)
    void* ptr(ptr_);
    UNUSED(ptr);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetPtr)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetPtr)
     return Scripting::ScriptingWrapperFor((GameObject*)ptr); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_INTERNAL_CALL_rotationRate_Internal(int idx, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_INTERNAL_CALL_rotationRate_Internal)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_rotationRate_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_rotationRate_Internal)
     { returnValue =(GetGyroRotationRate(idx)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_INTERNAL_CALL_rotationRateUnbiased_Internal(int idx, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_INTERNAL_CALL_rotationRateUnbiased_Internal)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_rotationRateUnbiased_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_rotationRateUnbiased_Internal)
     { returnValue =(GetGyroRotationRateUnbiased(idx)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_INTERNAL_CALL_gravity_Internal(int idx, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_INTERNAL_CALL_gravity_Internal)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_gravity_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_gravity_Internal)
     { returnValue =(GetGravity(idx)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_INTERNAL_CALL_userAcceleration_Internal(int idx, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_INTERNAL_CALL_userAcceleration_Internal)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_userAcceleration_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_userAcceleration_Internal)
     { returnValue =(GetUserAcceleration(idx)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_INTERNAL_CALL_attitude_Internal(int idx, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_INTERNAL_CALL_attitude_Internal)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_attitude_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_attitude_Internal)
     { returnValue =(GetAttitude(idx)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_getEnabled_Internal(int idx)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_getEnabled_Internal)
    SCRIPTINGAPI_STACK_CHECK(getEnabled_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(getEnabled_Internal)
     return IsGyroEnabled(idx); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_setEnabled_Internal(int idx, ScriptingBool enabled)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_setEnabled_Internal)
    SCRIPTINGAPI_STACK_CHECK(setEnabled_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(setEnabled_Internal)
     SetGyroEnabled(idx, enabled); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_getUpdateInterval_Internal(int idx)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_getUpdateInterval_Internal)
    SCRIPTINGAPI_STACK_CHECK(getUpdateInterval_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(getUpdateInterval_Internal)
     return GetGyroUpdateInterval(idx); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gyroscope_CUSTOM_setUpdateInterval_Internal(int idx, float interval)
{
    SCRIPTINGAPI_ETW_ENTRY(Gyroscope_CUSTOM_setUpdateInterval_Internal)
    SCRIPTINGAPI_STACK_CHECK(setUpdateInterval_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(setUpdateInterval_Internal)
     SetGyroUpdateInterval(idx, interval); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION LocationService_Get_Custom_PropIsEnabledByUser(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocationService_Get_Custom_PropIsEnabledByUser)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isEnabledByUser)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isEnabledByUser)
    return LocationService::IsServiceEnabledByUser ();
}

SCRIPT_BINDINGS_EXPORT_DECL
LocationServiceStatus SCRIPT_CALL_CONVENTION LocationService_Get_Custom_PropStatus(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocationService_Get_Custom_PropStatus)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_status)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_status)
    return LocationService::GetLocationStatus ();
}

SCRIPT_BINDINGS_EXPORT_DECL
LocationInfo SCRIPT_CALL_CONVENTION LocationService_Get_Custom_PropLastData(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocationService_Get_Custom_PropLastData)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lastData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lastData)
    if (LocationService::GetLocationStatus () != kLocationServiceRunning)
    printf_console ("Location service updates are not enabled. Check Handheld.locationServiceStatus before querying last location.\n");
    return LocationService::GetLastLocation ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocationService_CUSTOM_Start(ICallType_Object_Argument self_, float desiredAccuracyInMeters, float updateDistanceInMeters)
{
    SCRIPTINGAPI_ETW_ENTRY(LocationService_CUSTOM_Start)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Start)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Start)
    
    		LocationService::SetDesiredAccuracy (desiredAccuracyInMeters);
    		LocationService::SetDistanceFilter (updateDistanceInMeters);
    		LocationService::StartUpdatingLocation ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LocationService_CUSTOM_Stop(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LocationService_CUSTOM_Stop)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Stop)
    
    		LocationService::StopUpdatingLocation ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Compass_Get_Custom_PropMagneticHeading(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Compass_Get_Custom_PropMagneticHeading)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_magneticHeading)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_magneticHeading)
    return LocationService::GetLastHeading().magneticHeading;
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Compass_Get_Custom_PropTrueHeading(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Compass_Get_Custom_PropTrueHeading)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_trueHeading)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_trueHeading)
    return LocationService::GetLastHeading().trueHeading;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Compass_CUSTOM_INTERNAL_get_rawVector(ICallType_Object_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Compass_CUSTOM_INTERNAL_get_rawVector)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rawVector)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rawVector)
    { *returnValue =(LocationService::GetLastHeading().raw); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION Compass_Get_Custom_PropTimestamp(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Compass_Get_Custom_PropTimestamp)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_timestamp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_timestamp)
    return LocationService::GetLastHeading().timestamp;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Compass_Get_Custom_PropEnabled(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Compass_Get_Custom_PropEnabled)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return LocationService::IsHeadingUpdatesEnabled();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Compass_Set_Custom_PropEnabled(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Compass_Set_Custom_PropEnabled)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    LocationService::SetHeadingUpdatesEnabled(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Input_CUSTOM_mainGyroIndex_Internal()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_mainGyroIndex_Internal)
    SCRIPTINGAPI_STACK_CHECK(mainGyroIndex_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(mainGyroIndex_Internal)
     return GetGyro(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetKeyInt(int key)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetKeyInt)
    SCRIPTINGAPI_STACK_CHECK(GetKeyInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetKeyInt)
    
    		if (key > 0 && key < kKeyAndJoyButtonCount)
    			return GetInputManager ().GetKey (key);
    		else if (key == 0)
    			return false;
    		else
    		{
    			RaiseMonoException ("Invalid KeyCode enum.");
    			return false;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetKeyString(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetKeyString)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetKeyString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetKeyString)
    
    		string cname = name;
    		int key = StringToKey (cname);
    		if (key != 0)
    			return GetInputManager ().GetKey (key);
    		else
    		{
    			RaiseMonoException ("Input Key named: %s is unknown", cname.c_str());
    			return false;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetKeyUpInt(int key)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetKeyUpInt)
    SCRIPTINGAPI_STACK_CHECK(GetKeyUpInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetKeyUpInt)
    
    		if (key > 0 && key < kKeyAndJoyButtonCount)
    			return GetInputManager ().GetKeyUp (key);
    		else if (key == 0)
    			return false;
    		else
    		{
    			RaiseMonoException ("Invalid KeyCode enum.");
    			return false;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetKeyUpString(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetKeyUpString)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetKeyUpString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetKeyUpString)
    
    		string cname = name;
    		int key = StringToKey (cname);
    		if (key != 0)
    			return GetInputManager ().GetKeyUp (key);
    		else
    		{
    			RaiseMonoException ("Input Key named: %s is unknown", cname.c_str());
    			return false;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetKeyDownInt(int key)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetKeyDownInt)
    SCRIPTINGAPI_STACK_CHECK(GetKeyDownInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetKeyDownInt)
    
    		if (key > 0 && key < kKeyAndJoyButtonCount)
    			return GetInputManager ().GetKeyDown (key);
    		else if (key == 0)
    			return false;
    		else
    		{
    			RaiseMonoException ("Invalid KeyCode enum.");
    			return false;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetKeyDownString(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetKeyDownString)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetKeyDownString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetKeyDownString)
    
    		string cname = name;
    		int key = StringToKey (cname);
    		if (key != 0)
    			return GetInputManager ().GetKeyDown (key);
    		else
    		{
    			RaiseMonoException ("Input Key named: %s is unknown", cname.c_str());
    			return false;
    		}
    	
}


	static void CheckInputAxis (const string& name, bool button)
	{
		#if UNITY_EDITOR
		if (!GetInputManager ().HasAxisOrButton (name))
		{
			if (button)
				RaiseMonoException ("Input Button %s is not setup.\n To change the input settings use: Edit -> Project Settings -> Input", name.c_str());	
			else
				RaiseMonoException ("Input Axis %s is not setup.\n To change the input settings use: Edit -> Project Settings -> Input", name.c_str());	
		}
			
		#endif
	}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Input_CUSTOM_GetAxis(ICallType_String_Argument axisName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetAxis)
    ICallType_String_Local axisName(axisName_);
    UNUSED(axisName);
    SCRIPTINGAPI_STACK_CHECK(GetAxis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAxis)
    
    		string name = axisName;
    		CheckInputAxis (name, false);
    		return GetInputManager ().GetAxis (name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Input_CUSTOM_GetAxisRaw(ICallType_String_Argument axisName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetAxisRaw)
    ICallType_String_Local axisName(axisName_);
    UNUSED(axisName);
    SCRIPTINGAPI_STACK_CHECK(GetAxisRaw)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAxisRaw)
    
    		string name = axisName;
    		CheckInputAxis (name, false);
    		return GetInputManager ().GetAxisRaw (name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetButton(ICallType_String_Argument buttonName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetButton)
    ICallType_String_Local buttonName(buttonName_);
    UNUSED(buttonName);
    SCRIPTINGAPI_STACK_CHECK(GetButton)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetButton)
    
    		string name = buttonName;
    		CheckInputAxis (name, true);
    		return GetInputManager ().GetButton (name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropCompensateSensors()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropCompensateSensors)
    SCRIPTINGAPI_STACK_CHECK(get_compensateSensors)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_compensateSensors)
    return IsCompensatingSensors();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_Set_Custom_PropCompensateSensors(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Set_Custom_PropCompensateSensors)
    SCRIPTINGAPI_STACK_CHECK(set_compensateSensors)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_compensateSensors)
     SetCompensatingSensors(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropIsGyroAvailable()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropIsGyroAvailable)
    SCRIPTINGAPI_STACK_CHECK(get_isGyroAvailable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isGyroAvailable)
    return IsGyroAvailable();
}


SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetButtonDown(ICallType_String_Argument buttonName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetButtonDown)
    ICallType_String_Local buttonName(buttonName_);
    UNUSED(buttonName);
    SCRIPTINGAPI_STACK_CHECK(GetButtonDown)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetButtonDown)
    
    		string name = buttonName;
    		CheckInputAxis (name, true);
    		return GetInputManager ().GetButtonDown (name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetButtonUp(ICallType_String_Argument buttonName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetButtonUp)
    ICallType_String_Local buttonName(buttonName_);
    UNUSED(buttonName);
    SCRIPTINGAPI_STACK_CHECK(GetButtonUp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetButtonUp)
    
    		string name = buttonName;
    		CheckInputAxis (name, true);
    		return GetInputManager ().GetButtonUp (name);
    	
}

#if ENABLE_MONO || PLATFORM_METRO
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Input_CUSTOM_GetJoystickNames()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetJoystickNames)
    SCRIPTINGAPI_STACK_CHECK(GetJoystickNames)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetJoystickNames)
    
    		std::vector<std::string> names;
    		
    		#if !PLATFORM_IPHONE
    		GetJoystickNames(names);
    		#endif
    		return StringVectorToScripting(names);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetMouseButton(int button)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetMouseButton)
    SCRIPTINGAPI_STACK_CHECK(GetMouseButton)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMouseButton)
    
    		return GetInputManager ().GetMouseButton (button);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetMouseButtonDown(int button)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetMouseButtonDown)
    SCRIPTINGAPI_STACK_CHECK(GetMouseButtonDown)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMouseButtonDown)
    
    		return GetInputManager ().GetMouseButtonDown (button);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_CUSTOM_GetMouseButtonUp(int button)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_GetMouseButtonUp)
    SCRIPTINGAPI_STACK_CHECK(GetMouseButtonUp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMouseButtonUp)
    
    		return GetInputManager ().GetMouseButtonUp (button);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_ResetInputAxes()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_ResetInputAxes)
    SCRIPTINGAPI_STACK_CHECK(ResetInputAxes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ResetInputAxes)
     ResetInput(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_INTERNAL_get_mousePosition(Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_INTERNAL_get_mousePosition)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_mousePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_mousePosition)
    Vector2f p = GetInputManager ().GetMousePosition();
    { *returnValue =(Vector3f (p.x, p.y, 0.0F)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropMousePresent()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropMousePresent)
    SCRIPTINGAPI_STACK_CHECK(get_mousePresent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mousePresent)
    #if PLATFORM_METRO
    return GetMousePresent();
    #else
    return true;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropSimulateMouseWithTouches()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropSimulateMouseWithTouches)
    SCRIPTINGAPI_STACK_CHECK(get_simulateMouseWithTouches)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_simulateMouseWithTouches)
    return GetInputManager ().GetSimulateMouseWithTouches();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_Set_Custom_PropSimulateMouseWithTouches(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Set_Custom_PropSimulateMouseWithTouches)
    SCRIPTINGAPI_STACK_CHECK(set_simulateMouseWithTouches)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_simulateMouseWithTouches)
    
    GetInputManager ().SetSimulateMouseWithTouches(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropAnyKey()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropAnyKey)
    SCRIPTINGAPI_STACK_CHECK(get_anyKey)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anyKey)
    return GetInputManager ().GetAnyKey ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropAnyKeyDown()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropAnyKeyDown)
    SCRIPTINGAPI_STACK_CHECK(get_anyKeyDown)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anyKeyDown)
    return GetInputManager ().GetAnyKeyThisFrame ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Input_Get_Custom_PropInputString()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropInputString)
    SCRIPTINGAPI_STACK_CHECK(get_inputString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_inputString)
    return CreateScriptingString (GetInputManager ().GetInputString ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_INTERNAL_get_acceleration(Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_INTERNAL_get_acceleration)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_acceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_acceleration)
    { *returnValue =(GetAcceleration ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_INTERNAL_CALL_GetAccelerationEvent(int index, Acceleration& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_INTERNAL_CALL_GetAccelerationEvent)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetAccelerationEvent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetAccelerationEvent)
    
    		Acceleration acc;
    		if (index >= 0 && index < GetAccelerationCount ())
    			GetAcceleration (index, acc);
    		else
    			RaiseMonoException ("Index out of bounds.");
    		{ returnValue =(acc); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Input_Get_Custom_PropAccelerationEventCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropAccelerationEventCount)
    SCRIPTINGAPI_STACK_CHECK(get_accelerationEventCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_accelerationEventCount)
    return GetAccelerationCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_INTERNAL_CALL_GetTouch(int index, Touch& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_INTERNAL_CALL_GetTouch)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetTouch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetTouch)
    
    	#if ENABLE_NEW_EVENT_SYSTEM
    		if (index >= 0 && index < GetTouchCount ())
    		{
    			Touch* t = GetTouch(index);
    
    			if (t != NULL)
    			{
    				{ returnValue =(*t); return; }
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
    		{ returnValue =(dummy); return; }
    	#else
    		Touch touch;
    
    		if (index >= 0 && index < GetTouchCount ())
    		{
    			if (!GetTouch (index, touch))
    				RaiseMonoException ("Internal error.");
    		}
    		else
    			RaiseMonoException ("Index out of bounds.");
    		{ returnValue =(touch); return; }
    	#endif 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Input_Get_Custom_PropTouchCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropTouchCount)
    SCRIPTINGAPI_STACK_CHECK(get_touchCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_touchCount)
    return GetTouchCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropEatKeyPressOnTextFieldFocus()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropEatKeyPressOnTextFieldFocus)
    SCRIPTINGAPI_STACK_CHECK(get_eatKeyPressOnTextFieldFocus)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_eatKeyPressOnTextFieldFocus)
    return GetInputManager().GetEatKeyPressOnTextFieldFocus();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_Set_Custom_PropEatKeyPressOnTextFieldFocus(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Set_Custom_PropEatKeyPressOnTextFieldFocus)
    SCRIPTINGAPI_STACK_CHECK(set_eatKeyPressOnTextFieldFocus)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_eatKeyPressOnTextFieldFocus)
     return GetInputManager().SetEatKeyPressOnTextFieldFocus(value); 
}

#if !UNITY_FLASH && !PLATFORM_WEBGL
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropMultiTouchEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropMultiTouchEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_multiTouchEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_multiTouchEnabled)
    return IsMultiTouchEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_Set_Custom_PropMultiTouchEnabled(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Set_Custom_PropMultiTouchEnabled)
    SCRIPTINGAPI_STACK_CHECK(set_multiTouchEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_multiTouchEnabled)
     return SetMultiTouchEnabled (value); 
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
unsigned SCRIPT_CALL_CONVENTION Input_Get_Custom_PropDeviceOrientation()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropDeviceOrientation)
    SCRIPTINGAPI_STACK_CHECK(get_deviceOrientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceOrientation)
    return GetOrientation();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Input_Get_Custom_PropImeCompositionMode()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropImeCompositionMode)
    SCRIPTINGAPI_STACK_CHECK(get_imeCompositionMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_imeCompositionMode)
    return GetInputManager().GetIMECompositionMode();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_Set_Custom_PropImeCompositionMode(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Set_Custom_PropImeCompositionMode)
    SCRIPTINGAPI_STACK_CHECK(set_imeCompositionMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_imeCompositionMode)
     GetInputManager().SetIMECompositionMode (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Input_Get_Custom_PropCompositionString()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropCompositionString)
    SCRIPTINGAPI_STACK_CHECK(get_compositionString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_compositionString)
    return CreateScriptingString (GetInputManager ().GetCompositionString ());
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Input_Get_Custom_PropImeIsSelected()
{
    SCRIPTINGAPI_ETW_ENTRY(Input_Get_Custom_PropImeIsSelected)
    SCRIPTINGAPI_STACK_CHECK(get_imeIsSelected)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_imeIsSelected)
    return (GetInputManager().GetIMEIsSelected());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_INTERNAL_get_compositionCursorPos(Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_INTERNAL_get_compositionCursorPos)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_compositionCursorPos)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_compositionCursorPos)
    { *returnValue =(GetInputManager().GetTextFieldCursorPos ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Input_CUSTOM_INTERNAL_set_compositionCursorPos(const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Input_CUSTOM_INTERNAL_set_compositionCursorPos)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_compositionCursorPos)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_compositionCursorPos)
     GetInputManager().GetTextFieldCursorPos () = value; 
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Touch_Internal_GetPtr()
{
    scripting_add_internal_call( "UnityEngine.Touch::Internal_GetPtr" , (gpointer)& Touch_CUSTOM_Internal_GetPtr );
}

void Register_UnityEngine_Gyroscope_INTERNAL_CALL_rotationRate_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::INTERNAL_CALL_rotationRate_Internal" , (gpointer)& Gyroscope_CUSTOM_INTERNAL_CALL_rotationRate_Internal );
}

void Register_UnityEngine_Gyroscope_INTERNAL_CALL_rotationRateUnbiased_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::INTERNAL_CALL_rotationRateUnbiased_Internal" , (gpointer)& Gyroscope_CUSTOM_INTERNAL_CALL_rotationRateUnbiased_Internal );
}

void Register_UnityEngine_Gyroscope_INTERNAL_CALL_gravity_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::INTERNAL_CALL_gravity_Internal" , (gpointer)& Gyroscope_CUSTOM_INTERNAL_CALL_gravity_Internal );
}

void Register_UnityEngine_Gyroscope_INTERNAL_CALL_userAcceleration_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::INTERNAL_CALL_userAcceleration_Internal" , (gpointer)& Gyroscope_CUSTOM_INTERNAL_CALL_userAcceleration_Internal );
}

void Register_UnityEngine_Gyroscope_INTERNAL_CALL_attitude_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::INTERNAL_CALL_attitude_Internal" , (gpointer)& Gyroscope_CUSTOM_INTERNAL_CALL_attitude_Internal );
}

void Register_UnityEngine_Gyroscope_getEnabled_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::getEnabled_Internal" , (gpointer)& Gyroscope_CUSTOM_getEnabled_Internal );
}

void Register_UnityEngine_Gyroscope_setEnabled_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::setEnabled_Internal" , (gpointer)& Gyroscope_CUSTOM_setEnabled_Internal );
}

void Register_UnityEngine_Gyroscope_getUpdateInterval_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::getUpdateInterval_Internal" , (gpointer)& Gyroscope_CUSTOM_getUpdateInterval_Internal );
}

void Register_UnityEngine_Gyroscope_setUpdateInterval_Internal()
{
    scripting_add_internal_call( "UnityEngine.Gyroscope::setUpdateInterval_Internal" , (gpointer)& Gyroscope_CUSTOM_setUpdateInterval_Internal );
}

void Register_UnityEngine_LocationService_get_isEnabledByUser()
{
    scripting_add_internal_call( "UnityEngine.LocationService::get_isEnabledByUser" , (gpointer)& LocationService_Get_Custom_PropIsEnabledByUser );
}

void Register_UnityEngine_LocationService_get_status()
{
    scripting_add_internal_call( "UnityEngine.LocationService::get_status" , (gpointer)& LocationService_Get_Custom_PropStatus );
}

void Register_UnityEngine_LocationService_get_lastData()
{
    scripting_add_internal_call( "UnityEngine.LocationService::get_lastData" , (gpointer)& LocationService_Get_Custom_PropLastData );
}

void Register_UnityEngine_LocationService_Start()
{
    scripting_add_internal_call( "UnityEngine.LocationService::Start" , (gpointer)& LocationService_CUSTOM_Start );
}

void Register_UnityEngine_LocationService_Stop()
{
    scripting_add_internal_call( "UnityEngine.LocationService::Stop" , (gpointer)& LocationService_CUSTOM_Stop );
}

void Register_UnityEngine_Compass_get_magneticHeading()
{
    scripting_add_internal_call( "UnityEngine.Compass::get_magneticHeading" , (gpointer)& Compass_Get_Custom_PropMagneticHeading );
}

void Register_UnityEngine_Compass_get_trueHeading()
{
    scripting_add_internal_call( "UnityEngine.Compass::get_trueHeading" , (gpointer)& Compass_Get_Custom_PropTrueHeading );
}

void Register_UnityEngine_Compass_INTERNAL_get_rawVector()
{
    scripting_add_internal_call( "UnityEngine.Compass::INTERNAL_get_rawVector" , (gpointer)& Compass_CUSTOM_INTERNAL_get_rawVector );
}

void Register_UnityEngine_Compass_get_timestamp()
{
    scripting_add_internal_call( "UnityEngine.Compass::get_timestamp" , (gpointer)& Compass_Get_Custom_PropTimestamp );
}

void Register_UnityEngine_Compass_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Compass::get_enabled" , (gpointer)& Compass_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Compass_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Compass::set_enabled" , (gpointer)& Compass_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Input_mainGyroIndex_Internal()
{
    scripting_add_internal_call( "UnityEngine.Input::mainGyroIndex_Internal" , (gpointer)& Input_CUSTOM_mainGyroIndex_Internal );
}

void Register_UnityEngine_Input_GetKeyInt()
{
    scripting_add_internal_call( "UnityEngine.Input::GetKeyInt" , (gpointer)& Input_CUSTOM_GetKeyInt );
}

void Register_UnityEngine_Input_GetKeyString()
{
    scripting_add_internal_call( "UnityEngine.Input::GetKeyString" , (gpointer)& Input_CUSTOM_GetKeyString );
}

void Register_UnityEngine_Input_GetKeyUpInt()
{
    scripting_add_internal_call( "UnityEngine.Input::GetKeyUpInt" , (gpointer)& Input_CUSTOM_GetKeyUpInt );
}

void Register_UnityEngine_Input_GetKeyUpString()
{
    scripting_add_internal_call( "UnityEngine.Input::GetKeyUpString" , (gpointer)& Input_CUSTOM_GetKeyUpString );
}

void Register_UnityEngine_Input_GetKeyDownInt()
{
    scripting_add_internal_call( "UnityEngine.Input::GetKeyDownInt" , (gpointer)& Input_CUSTOM_GetKeyDownInt );
}

void Register_UnityEngine_Input_GetKeyDownString()
{
    scripting_add_internal_call( "UnityEngine.Input::GetKeyDownString" , (gpointer)& Input_CUSTOM_GetKeyDownString );
}

void Register_UnityEngine_Input_GetAxis()
{
    scripting_add_internal_call( "UnityEngine.Input::GetAxis" , (gpointer)& Input_CUSTOM_GetAxis );
}

void Register_UnityEngine_Input_GetAxisRaw()
{
    scripting_add_internal_call( "UnityEngine.Input::GetAxisRaw" , (gpointer)& Input_CUSTOM_GetAxisRaw );
}

void Register_UnityEngine_Input_GetButton()
{
    scripting_add_internal_call( "UnityEngine.Input::GetButton" , (gpointer)& Input_CUSTOM_GetButton );
}

void Register_UnityEngine_Input_get_compensateSensors()
{
    scripting_add_internal_call( "UnityEngine.Input::get_compensateSensors" , (gpointer)& Input_Get_Custom_PropCompensateSensors );
}

void Register_UnityEngine_Input_set_compensateSensors()
{
    scripting_add_internal_call( "UnityEngine.Input::set_compensateSensors" , (gpointer)& Input_Set_Custom_PropCompensateSensors );
}

void Register_UnityEngine_Input_get_isGyroAvailable()
{
    scripting_add_internal_call( "UnityEngine.Input::get_isGyroAvailable" , (gpointer)& Input_Get_Custom_PropIsGyroAvailable );
}

void Register_UnityEngine_Input_GetButtonDown()
{
    scripting_add_internal_call( "UnityEngine.Input::GetButtonDown" , (gpointer)& Input_CUSTOM_GetButtonDown );
}

void Register_UnityEngine_Input_GetButtonUp()
{
    scripting_add_internal_call( "UnityEngine.Input::GetButtonUp" , (gpointer)& Input_CUSTOM_GetButtonUp );
}

#if ENABLE_MONO || PLATFORM_METRO
void Register_UnityEngine_Input_GetJoystickNames()
{
    scripting_add_internal_call( "UnityEngine.Input::GetJoystickNames" , (gpointer)& Input_CUSTOM_GetJoystickNames );
}

#endif
void Register_UnityEngine_Input_GetMouseButton()
{
    scripting_add_internal_call( "UnityEngine.Input::GetMouseButton" , (gpointer)& Input_CUSTOM_GetMouseButton );
}

void Register_UnityEngine_Input_GetMouseButtonDown()
{
    scripting_add_internal_call( "UnityEngine.Input::GetMouseButtonDown" , (gpointer)& Input_CUSTOM_GetMouseButtonDown );
}

void Register_UnityEngine_Input_GetMouseButtonUp()
{
    scripting_add_internal_call( "UnityEngine.Input::GetMouseButtonUp" , (gpointer)& Input_CUSTOM_GetMouseButtonUp );
}

void Register_UnityEngine_Input_ResetInputAxes()
{
    scripting_add_internal_call( "UnityEngine.Input::ResetInputAxes" , (gpointer)& Input_CUSTOM_ResetInputAxes );
}

void Register_UnityEngine_Input_INTERNAL_get_mousePosition()
{
    scripting_add_internal_call( "UnityEngine.Input::INTERNAL_get_mousePosition" , (gpointer)& Input_CUSTOM_INTERNAL_get_mousePosition );
}

void Register_UnityEngine_Input_get_mousePresent()
{
    scripting_add_internal_call( "UnityEngine.Input::get_mousePresent" , (gpointer)& Input_Get_Custom_PropMousePresent );
}

void Register_UnityEngine_Input_get_simulateMouseWithTouches()
{
    scripting_add_internal_call( "UnityEngine.Input::get_simulateMouseWithTouches" , (gpointer)& Input_Get_Custom_PropSimulateMouseWithTouches );
}

void Register_UnityEngine_Input_set_simulateMouseWithTouches()
{
    scripting_add_internal_call( "UnityEngine.Input::set_simulateMouseWithTouches" , (gpointer)& Input_Set_Custom_PropSimulateMouseWithTouches );
}

void Register_UnityEngine_Input_get_anyKey()
{
    scripting_add_internal_call( "UnityEngine.Input::get_anyKey" , (gpointer)& Input_Get_Custom_PropAnyKey );
}

void Register_UnityEngine_Input_get_anyKeyDown()
{
    scripting_add_internal_call( "UnityEngine.Input::get_anyKeyDown" , (gpointer)& Input_Get_Custom_PropAnyKeyDown );
}

void Register_UnityEngine_Input_get_inputString()
{
    scripting_add_internal_call( "UnityEngine.Input::get_inputString" , (gpointer)& Input_Get_Custom_PropInputString );
}

void Register_UnityEngine_Input_INTERNAL_get_acceleration()
{
    scripting_add_internal_call( "UnityEngine.Input::INTERNAL_get_acceleration" , (gpointer)& Input_CUSTOM_INTERNAL_get_acceleration );
}

void Register_UnityEngine_Input_INTERNAL_CALL_GetAccelerationEvent()
{
    scripting_add_internal_call( "UnityEngine.Input::INTERNAL_CALL_GetAccelerationEvent" , (gpointer)& Input_CUSTOM_INTERNAL_CALL_GetAccelerationEvent );
}

void Register_UnityEngine_Input_get_accelerationEventCount()
{
    scripting_add_internal_call( "UnityEngine.Input::get_accelerationEventCount" , (gpointer)& Input_Get_Custom_PropAccelerationEventCount );
}

void Register_UnityEngine_Input_INTERNAL_CALL_GetTouch()
{
    scripting_add_internal_call( "UnityEngine.Input::INTERNAL_CALL_GetTouch" , (gpointer)& Input_CUSTOM_INTERNAL_CALL_GetTouch );
}

void Register_UnityEngine_Input_get_touchCount()
{
    scripting_add_internal_call( "UnityEngine.Input::get_touchCount" , (gpointer)& Input_Get_Custom_PropTouchCount );
}

void Register_UnityEngine_Input_get_eatKeyPressOnTextFieldFocus()
{
    scripting_add_internal_call( "UnityEngine.Input::get_eatKeyPressOnTextFieldFocus" , (gpointer)& Input_Get_Custom_PropEatKeyPressOnTextFieldFocus );
}

void Register_UnityEngine_Input_set_eatKeyPressOnTextFieldFocus()
{
    scripting_add_internal_call( "UnityEngine.Input::set_eatKeyPressOnTextFieldFocus" , (gpointer)& Input_Set_Custom_PropEatKeyPressOnTextFieldFocus );
}

#if !UNITY_FLASH && !PLATFORM_WEBGL
void Register_UnityEngine_Input_get_multiTouchEnabled()
{
    scripting_add_internal_call( "UnityEngine.Input::get_multiTouchEnabled" , (gpointer)& Input_Get_Custom_PropMultiTouchEnabled );
}

void Register_UnityEngine_Input_set_multiTouchEnabled()
{
    scripting_add_internal_call( "UnityEngine.Input::set_multiTouchEnabled" , (gpointer)& Input_Set_Custom_PropMultiTouchEnabled );
}

#endif
void Register_UnityEngine_Input_get_deviceOrientation()
{
    scripting_add_internal_call( "UnityEngine.Input::get_deviceOrientation" , (gpointer)& Input_Get_Custom_PropDeviceOrientation );
}

void Register_UnityEngine_Input_get_imeCompositionMode()
{
    scripting_add_internal_call( "UnityEngine.Input::get_imeCompositionMode" , (gpointer)& Input_Get_Custom_PropImeCompositionMode );
}

void Register_UnityEngine_Input_set_imeCompositionMode()
{
    scripting_add_internal_call( "UnityEngine.Input::set_imeCompositionMode" , (gpointer)& Input_Set_Custom_PropImeCompositionMode );
}

void Register_UnityEngine_Input_get_compositionString()
{
    scripting_add_internal_call( "UnityEngine.Input::get_compositionString" , (gpointer)& Input_Get_Custom_PropCompositionString );
}

void Register_UnityEngine_Input_get_imeIsSelected()
{
    scripting_add_internal_call( "UnityEngine.Input::get_imeIsSelected" , (gpointer)& Input_Get_Custom_PropImeIsSelected );
}

void Register_UnityEngine_Input_INTERNAL_get_compositionCursorPos()
{
    scripting_add_internal_call( "UnityEngine.Input::INTERNAL_get_compositionCursorPos" , (gpointer)& Input_CUSTOM_INTERNAL_get_compositionCursorPos );
}

void Register_UnityEngine_Input_INTERNAL_set_compositionCursorPos()
{
    scripting_add_internal_call( "UnityEngine.Input::INTERNAL_set_compositionCursorPos" , (gpointer)& Input_CUSTOM_INTERNAL_set_compositionCursorPos );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineInput_IcallNames [] =
{
    "UnityEngine.Touch::Internal_GetPtr"    ,    // -> Touch_CUSTOM_Internal_GetPtr
    "UnityEngine.Gyroscope::INTERNAL_CALL_rotationRate_Internal",    // -> Gyroscope_CUSTOM_INTERNAL_CALL_rotationRate_Internal
    "UnityEngine.Gyroscope::INTERNAL_CALL_rotationRateUnbiased_Internal",    // -> Gyroscope_CUSTOM_INTERNAL_CALL_rotationRateUnbiased_Internal
    "UnityEngine.Gyroscope::INTERNAL_CALL_gravity_Internal",    // -> Gyroscope_CUSTOM_INTERNAL_CALL_gravity_Internal
    "UnityEngine.Gyroscope::INTERNAL_CALL_userAcceleration_Internal",    // -> Gyroscope_CUSTOM_INTERNAL_CALL_userAcceleration_Internal
    "UnityEngine.Gyroscope::INTERNAL_CALL_attitude_Internal",    // -> Gyroscope_CUSTOM_INTERNAL_CALL_attitude_Internal
    "UnityEngine.Gyroscope::getEnabled_Internal",    // -> Gyroscope_CUSTOM_getEnabled_Internal
    "UnityEngine.Gyroscope::setEnabled_Internal",    // -> Gyroscope_CUSTOM_setEnabled_Internal
    "UnityEngine.Gyroscope::getUpdateInterval_Internal",    // -> Gyroscope_CUSTOM_getUpdateInterval_Internal
    "UnityEngine.Gyroscope::setUpdateInterval_Internal",    // -> Gyroscope_CUSTOM_setUpdateInterval_Internal
    "UnityEngine.LocationService::get_isEnabledByUser",    // -> LocationService_Get_Custom_PropIsEnabledByUser
    "UnityEngine.LocationService::get_status",    // -> LocationService_Get_Custom_PropStatus
    "UnityEngine.LocationService::get_lastData",    // -> LocationService_Get_Custom_PropLastData
    "UnityEngine.LocationService::Start"    ,    // -> LocationService_CUSTOM_Start
    "UnityEngine.LocationService::Stop"     ,    // -> LocationService_CUSTOM_Stop
    "UnityEngine.Compass::get_magneticHeading",    // -> Compass_Get_Custom_PropMagneticHeading
    "UnityEngine.Compass::get_trueHeading"  ,    // -> Compass_Get_Custom_PropTrueHeading
    "UnityEngine.Compass::INTERNAL_get_rawVector",    // -> Compass_CUSTOM_INTERNAL_get_rawVector
    "UnityEngine.Compass::get_timestamp"    ,    // -> Compass_Get_Custom_PropTimestamp
    "UnityEngine.Compass::get_enabled"      ,    // -> Compass_Get_Custom_PropEnabled
    "UnityEngine.Compass::set_enabled"      ,    // -> Compass_Set_Custom_PropEnabled
    "UnityEngine.Input::mainGyroIndex_Internal",    // -> Input_CUSTOM_mainGyroIndex_Internal
    "UnityEngine.Input::GetKeyInt"          ,    // -> Input_CUSTOM_GetKeyInt
    "UnityEngine.Input::GetKeyString"       ,    // -> Input_CUSTOM_GetKeyString
    "UnityEngine.Input::GetKeyUpInt"        ,    // -> Input_CUSTOM_GetKeyUpInt
    "UnityEngine.Input::GetKeyUpString"     ,    // -> Input_CUSTOM_GetKeyUpString
    "UnityEngine.Input::GetKeyDownInt"      ,    // -> Input_CUSTOM_GetKeyDownInt
    "UnityEngine.Input::GetKeyDownString"   ,    // -> Input_CUSTOM_GetKeyDownString
    "UnityEngine.Input::GetAxis"            ,    // -> Input_CUSTOM_GetAxis
    "UnityEngine.Input::GetAxisRaw"         ,    // -> Input_CUSTOM_GetAxisRaw
    "UnityEngine.Input::GetButton"          ,    // -> Input_CUSTOM_GetButton
    "UnityEngine.Input::get_compensateSensors",    // -> Input_Get_Custom_PropCompensateSensors
    "UnityEngine.Input::set_compensateSensors",    // -> Input_Set_Custom_PropCompensateSensors
    "UnityEngine.Input::get_isGyroAvailable",    // -> Input_Get_Custom_PropIsGyroAvailable
    "UnityEngine.Input::GetButtonDown"      ,    // -> Input_CUSTOM_GetButtonDown
    "UnityEngine.Input::GetButtonUp"        ,    // -> Input_CUSTOM_GetButtonUp
#if ENABLE_MONO || PLATFORM_METRO
    "UnityEngine.Input::GetJoystickNames"   ,    // -> Input_CUSTOM_GetJoystickNames
#endif
    "UnityEngine.Input::GetMouseButton"     ,    // -> Input_CUSTOM_GetMouseButton
    "UnityEngine.Input::GetMouseButtonDown" ,    // -> Input_CUSTOM_GetMouseButtonDown
    "UnityEngine.Input::GetMouseButtonUp"   ,    // -> Input_CUSTOM_GetMouseButtonUp
    "UnityEngine.Input::ResetInputAxes"     ,    // -> Input_CUSTOM_ResetInputAxes
    "UnityEngine.Input::INTERNAL_get_mousePosition",    // -> Input_CUSTOM_INTERNAL_get_mousePosition
    "UnityEngine.Input::get_mousePresent"   ,    // -> Input_Get_Custom_PropMousePresent
    "UnityEngine.Input::get_simulateMouseWithTouches",    // -> Input_Get_Custom_PropSimulateMouseWithTouches
    "UnityEngine.Input::set_simulateMouseWithTouches",    // -> Input_Set_Custom_PropSimulateMouseWithTouches
    "UnityEngine.Input::get_anyKey"         ,    // -> Input_Get_Custom_PropAnyKey
    "UnityEngine.Input::get_anyKeyDown"     ,    // -> Input_Get_Custom_PropAnyKeyDown
    "UnityEngine.Input::get_inputString"    ,    // -> Input_Get_Custom_PropInputString
    "UnityEngine.Input::INTERNAL_get_acceleration",    // -> Input_CUSTOM_INTERNAL_get_acceleration
    "UnityEngine.Input::INTERNAL_CALL_GetAccelerationEvent",    // -> Input_CUSTOM_INTERNAL_CALL_GetAccelerationEvent
    "UnityEngine.Input::get_accelerationEventCount",    // -> Input_Get_Custom_PropAccelerationEventCount
    "UnityEngine.Input::INTERNAL_CALL_GetTouch",    // -> Input_CUSTOM_INTERNAL_CALL_GetTouch
    "UnityEngine.Input::get_touchCount"     ,    // -> Input_Get_Custom_PropTouchCount
    "UnityEngine.Input::get_eatKeyPressOnTextFieldFocus",    // -> Input_Get_Custom_PropEatKeyPressOnTextFieldFocus
    "UnityEngine.Input::set_eatKeyPressOnTextFieldFocus",    // -> Input_Set_Custom_PropEatKeyPressOnTextFieldFocus
#if !UNITY_FLASH && !PLATFORM_WEBGL
    "UnityEngine.Input::get_multiTouchEnabled",    // -> Input_Get_Custom_PropMultiTouchEnabled
    "UnityEngine.Input::set_multiTouchEnabled",    // -> Input_Set_Custom_PropMultiTouchEnabled
#endif
    "UnityEngine.Input::get_deviceOrientation",    // -> Input_Get_Custom_PropDeviceOrientation
    "UnityEngine.Input::get_imeCompositionMode",    // -> Input_Get_Custom_PropImeCompositionMode
    "UnityEngine.Input::set_imeCompositionMode",    // -> Input_Set_Custom_PropImeCompositionMode
    "UnityEngine.Input::get_compositionString",    // -> Input_Get_Custom_PropCompositionString
    "UnityEngine.Input::get_imeIsSelected"  ,    // -> Input_Get_Custom_PropImeIsSelected
    "UnityEngine.Input::INTERNAL_get_compositionCursorPos",    // -> Input_CUSTOM_INTERNAL_get_compositionCursorPos
    "UnityEngine.Input::INTERNAL_set_compositionCursorPos",    // -> Input_CUSTOM_INTERNAL_set_compositionCursorPos
    NULL
};

static const void* s_UnityEngineInput_IcallFuncs [] =
{
    (const void*)&Touch_CUSTOM_Internal_GetPtr            ,  //  <- UnityEngine.Touch::Internal_GetPtr
    (const void*)&Gyroscope_CUSTOM_INTERNAL_CALL_rotationRate_Internal,  //  <- UnityEngine.Gyroscope::INTERNAL_CALL_rotationRate_Internal
    (const void*)&Gyroscope_CUSTOM_INTERNAL_CALL_rotationRateUnbiased_Internal,  //  <- UnityEngine.Gyroscope::INTERNAL_CALL_rotationRateUnbiased_Internal
    (const void*)&Gyroscope_CUSTOM_INTERNAL_CALL_gravity_Internal,  //  <- UnityEngine.Gyroscope::INTERNAL_CALL_gravity_Internal
    (const void*)&Gyroscope_CUSTOM_INTERNAL_CALL_userAcceleration_Internal,  //  <- UnityEngine.Gyroscope::INTERNAL_CALL_userAcceleration_Internal
    (const void*)&Gyroscope_CUSTOM_INTERNAL_CALL_attitude_Internal,  //  <- UnityEngine.Gyroscope::INTERNAL_CALL_attitude_Internal
    (const void*)&Gyroscope_CUSTOM_getEnabled_Internal    ,  //  <- UnityEngine.Gyroscope::getEnabled_Internal
    (const void*)&Gyroscope_CUSTOM_setEnabled_Internal    ,  //  <- UnityEngine.Gyroscope::setEnabled_Internal
    (const void*)&Gyroscope_CUSTOM_getUpdateInterval_Internal,  //  <- UnityEngine.Gyroscope::getUpdateInterval_Internal
    (const void*)&Gyroscope_CUSTOM_setUpdateInterval_Internal,  //  <- UnityEngine.Gyroscope::setUpdateInterval_Internal
    (const void*)&LocationService_Get_Custom_PropIsEnabledByUser,  //  <- UnityEngine.LocationService::get_isEnabledByUser
    (const void*)&LocationService_Get_Custom_PropStatus   ,  //  <- UnityEngine.LocationService::get_status
    (const void*)&LocationService_Get_Custom_PropLastData ,  //  <- UnityEngine.LocationService::get_lastData
    (const void*)&LocationService_CUSTOM_Start            ,  //  <- UnityEngine.LocationService::Start
    (const void*)&LocationService_CUSTOM_Stop             ,  //  <- UnityEngine.LocationService::Stop
    (const void*)&Compass_Get_Custom_PropMagneticHeading  ,  //  <- UnityEngine.Compass::get_magneticHeading
    (const void*)&Compass_Get_Custom_PropTrueHeading      ,  //  <- UnityEngine.Compass::get_trueHeading
    (const void*)&Compass_CUSTOM_INTERNAL_get_rawVector   ,  //  <- UnityEngine.Compass::INTERNAL_get_rawVector
    (const void*)&Compass_Get_Custom_PropTimestamp        ,  //  <- UnityEngine.Compass::get_timestamp
    (const void*)&Compass_Get_Custom_PropEnabled          ,  //  <- UnityEngine.Compass::get_enabled
    (const void*)&Compass_Set_Custom_PropEnabled          ,  //  <- UnityEngine.Compass::set_enabled
    (const void*)&Input_CUSTOM_mainGyroIndex_Internal     ,  //  <- UnityEngine.Input::mainGyroIndex_Internal
    (const void*)&Input_CUSTOM_GetKeyInt                  ,  //  <- UnityEngine.Input::GetKeyInt
    (const void*)&Input_CUSTOM_GetKeyString               ,  //  <- UnityEngine.Input::GetKeyString
    (const void*)&Input_CUSTOM_GetKeyUpInt                ,  //  <- UnityEngine.Input::GetKeyUpInt
    (const void*)&Input_CUSTOM_GetKeyUpString             ,  //  <- UnityEngine.Input::GetKeyUpString
    (const void*)&Input_CUSTOM_GetKeyDownInt              ,  //  <- UnityEngine.Input::GetKeyDownInt
    (const void*)&Input_CUSTOM_GetKeyDownString           ,  //  <- UnityEngine.Input::GetKeyDownString
    (const void*)&Input_CUSTOM_GetAxis                    ,  //  <- UnityEngine.Input::GetAxis
    (const void*)&Input_CUSTOM_GetAxisRaw                 ,  //  <- UnityEngine.Input::GetAxisRaw
    (const void*)&Input_CUSTOM_GetButton                  ,  //  <- UnityEngine.Input::GetButton
    (const void*)&Input_Get_Custom_PropCompensateSensors  ,  //  <- UnityEngine.Input::get_compensateSensors
    (const void*)&Input_Set_Custom_PropCompensateSensors  ,  //  <- UnityEngine.Input::set_compensateSensors
    (const void*)&Input_Get_Custom_PropIsGyroAvailable    ,  //  <- UnityEngine.Input::get_isGyroAvailable
    (const void*)&Input_CUSTOM_GetButtonDown              ,  //  <- UnityEngine.Input::GetButtonDown
    (const void*)&Input_CUSTOM_GetButtonUp                ,  //  <- UnityEngine.Input::GetButtonUp
#if ENABLE_MONO || PLATFORM_METRO
    (const void*)&Input_CUSTOM_GetJoystickNames           ,  //  <- UnityEngine.Input::GetJoystickNames
#endif
    (const void*)&Input_CUSTOM_GetMouseButton             ,  //  <- UnityEngine.Input::GetMouseButton
    (const void*)&Input_CUSTOM_GetMouseButtonDown         ,  //  <- UnityEngine.Input::GetMouseButtonDown
    (const void*)&Input_CUSTOM_GetMouseButtonUp           ,  //  <- UnityEngine.Input::GetMouseButtonUp
    (const void*)&Input_CUSTOM_ResetInputAxes             ,  //  <- UnityEngine.Input::ResetInputAxes
    (const void*)&Input_CUSTOM_INTERNAL_get_mousePosition ,  //  <- UnityEngine.Input::INTERNAL_get_mousePosition
    (const void*)&Input_Get_Custom_PropMousePresent       ,  //  <- UnityEngine.Input::get_mousePresent
    (const void*)&Input_Get_Custom_PropSimulateMouseWithTouches,  //  <- UnityEngine.Input::get_simulateMouseWithTouches
    (const void*)&Input_Set_Custom_PropSimulateMouseWithTouches,  //  <- UnityEngine.Input::set_simulateMouseWithTouches
    (const void*)&Input_Get_Custom_PropAnyKey             ,  //  <- UnityEngine.Input::get_anyKey
    (const void*)&Input_Get_Custom_PropAnyKeyDown         ,  //  <- UnityEngine.Input::get_anyKeyDown
    (const void*)&Input_Get_Custom_PropInputString        ,  //  <- UnityEngine.Input::get_inputString
    (const void*)&Input_CUSTOM_INTERNAL_get_acceleration  ,  //  <- UnityEngine.Input::INTERNAL_get_acceleration
    (const void*)&Input_CUSTOM_INTERNAL_CALL_GetAccelerationEvent,  //  <- UnityEngine.Input::INTERNAL_CALL_GetAccelerationEvent
    (const void*)&Input_Get_Custom_PropAccelerationEventCount,  //  <- UnityEngine.Input::get_accelerationEventCount
    (const void*)&Input_CUSTOM_INTERNAL_CALL_GetTouch     ,  //  <- UnityEngine.Input::INTERNAL_CALL_GetTouch
    (const void*)&Input_Get_Custom_PropTouchCount         ,  //  <- UnityEngine.Input::get_touchCount
    (const void*)&Input_Get_Custom_PropEatKeyPressOnTextFieldFocus,  //  <- UnityEngine.Input::get_eatKeyPressOnTextFieldFocus
    (const void*)&Input_Set_Custom_PropEatKeyPressOnTextFieldFocus,  //  <- UnityEngine.Input::set_eatKeyPressOnTextFieldFocus
#if !UNITY_FLASH && !PLATFORM_WEBGL
    (const void*)&Input_Get_Custom_PropMultiTouchEnabled  ,  //  <- UnityEngine.Input::get_multiTouchEnabled
    (const void*)&Input_Set_Custom_PropMultiTouchEnabled  ,  //  <- UnityEngine.Input::set_multiTouchEnabled
#endif
    (const void*)&Input_Get_Custom_PropDeviceOrientation  ,  //  <- UnityEngine.Input::get_deviceOrientation
    (const void*)&Input_Get_Custom_PropImeCompositionMode ,  //  <- UnityEngine.Input::get_imeCompositionMode
    (const void*)&Input_Set_Custom_PropImeCompositionMode ,  //  <- UnityEngine.Input::set_imeCompositionMode
    (const void*)&Input_Get_Custom_PropCompositionString  ,  //  <- UnityEngine.Input::get_compositionString
    (const void*)&Input_Get_Custom_PropImeIsSelected      ,  //  <- UnityEngine.Input::get_imeIsSelected
    (const void*)&Input_CUSTOM_INTERNAL_get_compositionCursorPos,  //  <- UnityEngine.Input::INTERNAL_get_compositionCursorPos
    (const void*)&Input_CUSTOM_INTERNAL_set_compositionCursorPos,  //  <- UnityEngine.Input::INTERNAL_set_compositionCursorPos
    NULL
};

void ExportUnityEngineInputBindings();
void ExportUnityEngineInputBindings()
{
    for (int i = 0; s_UnityEngineInput_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineInput_IcallNames [i], s_UnityEngineInput_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineInputBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Touch_CUSTOM_Internal_GetPtr); //  <- UnityEngine.Touch::Internal_GetPtr
    SET_METRO_BINDING(Gyroscope_CUSTOM_INTERNAL_CALL_rotationRate_Internal); //  <- UnityEngine.Gyroscope::INTERNAL_CALL_rotationRate_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_INTERNAL_CALL_rotationRateUnbiased_Internal); //  <- UnityEngine.Gyroscope::INTERNAL_CALL_rotationRateUnbiased_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_INTERNAL_CALL_gravity_Internal); //  <- UnityEngine.Gyroscope::INTERNAL_CALL_gravity_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_INTERNAL_CALL_userAcceleration_Internal); //  <- UnityEngine.Gyroscope::INTERNAL_CALL_userAcceleration_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_INTERNAL_CALL_attitude_Internal); //  <- UnityEngine.Gyroscope::INTERNAL_CALL_attitude_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_getEnabled_Internal); //  <- UnityEngine.Gyroscope::getEnabled_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_setEnabled_Internal); //  <- UnityEngine.Gyroscope::setEnabled_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_getUpdateInterval_Internal); //  <- UnityEngine.Gyroscope::getUpdateInterval_Internal
    SET_METRO_BINDING(Gyroscope_CUSTOM_setUpdateInterval_Internal); //  <- UnityEngine.Gyroscope::setUpdateInterval_Internal
    SET_METRO_BINDING(LocationService_Get_Custom_PropIsEnabledByUser); //  <- UnityEngine.LocationService::get_isEnabledByUser
    SET_METRO_BINDING(LocationService_Get_Custom_PropStatus); //  <- UnityEngine.LocationService::get_status
    SET_METRO_BINDING(LocationService_Get_Custom_PropLastData); //  <- UnityEngine.LocationService::get_lastData
    SET_METRO_BINDING(LocationService_CUSTOM_Start); //  <- UnityEngine.LocationService::Start
    SET_METRO_BINDING(LocationService_CUSTOM_Stop); //  <- UnityEngine.LocationService::Stop
    SET_METRO_BINDING(Compass_Get_Custom_PropMagneticHeading); //  <- UnityEngine.Compass::get_magneticHeading
    SET_METRO_BINDING(Compass_Get_Custom_PropTrueHeading); //  <- UnityEngine.Compass::get_trueHeading
    SET_METRO_BINDING(Compass_CUSTOM_INTERNAL_get_rawVector); //  <- UnityEngine.Compass::INTERNAL_get_rawVector
    SET_METRO_BINDING(Compass_Get_Custom_PropTimestamp); //  <- UnityEngine.Compass::get_timestamp
    SET_METRO_BINDING(Compass_Get_Custom_PropEnabled); //  <- UnityEngine.Compass::get_enabled
    SET_METRO_BINDING(Compass_Set_Custom_PropEnabled); //  <- UnityEngine.Compass::set_enabled
    SET_METRO_BINDING(Input_CUSTOM_mainGyroIndex_Internal); //  <- UnityEngine.Input::mainGyroIndex_Internal
    SET_METRO_BINDING(Input_CUSTOM_GetKeyInt); //  <- UnityEngine.Input::GetKeyInt
    SET_METRO_BINDING(Input_CUSTOM_GetKeyString); //  <- UnityEngine.Input::GetKeyString
    SET_METRO_BINDING(Input_CUSTOM_GetKeyUpInt); //  <- UnityEngine.Input::GetKeyUpInt
    SET_METRO_BINDING(Input_CUSTOM_GetKeyUpString); //  <- UnityEngine.Input::GetKeyUpString
    SET_METRO_BINDING(Input_CUSTOM_GetKeyDownInt); //  <- UnityEngine.Input::GetKeyDownInt
    SET_METRO_BINDING(Input_CUSTOM_GetKeyDownString); //  <- UnityEngine.Input::GetKeyDownString
    SET_METRO_BINDING(Input_CUSTOM_GetAxis); //  <- UnityEngine.Input::GetAxis
    SET_METRO_BINDING(Input_CUSTOM_GetAxisRaw); //  <- UnityEngine.Input::GetAxisRaw
    SET_METRO_BINDING(Input_CUSTOM_GetButton); //  <- UnityEngine.Input::GetButton
    SET_METRO_BINDING(Input_Get_Custom_PropCompensateSensors); //  <- UnityEngine.Input::get_compensateSensors
    SET_METRO_BINDING(Input_Set_Custom_PropCompensateSensors); //  <- UnityEngine.Input::set_compensateSensors
    SET_METRO_BINDING(Input_Get_Custom_PropIsGyroAvailable); //  <- UnityEngine.Input::get_isGyroAvailable
    SET_METRO_BINDING(Input_CUSTOM_GetButtonDown); //  <- UnityEngine.Input::GetButtonDown
    SET_METRO_BINDING(Input_CUSTOM_GetButtonUp); //  <- UnityEngine.Input::GetButtonUp
#if ENABLE_MONO || PLATFORM_METRO
    SET_METRO_BINDING(Input_CUSTOM_GetJoystickNames); //  <- UnityEngine.Input::GetJoystickNames
#endif
    SET_METRO_BINDING(Input_CUSTOM_GetMouseButton); //  <- UnityEngine.Input::GetMouseButton
    SET_METRO_BINDING(Input_CUSTOM_GetMouseButtonDown); //  <- UnityEngine.Input::GetMouseButtonDown
    SET_METRO_BINDING(Input_CUSTOM_GetMouseButtonUp); //  <- UnityEngine.Input::GetMouseButtonUp
    SET_METRO_BINDING(Input_CUSTOM_ResetInputAxes); //  <- UnityEngine.Input::ResetInputAxes
    SET_METRO_BINDING(Input_CUSTOM_INTERNAL_get_mousePosition); //  <- UnityEngine.Input::INTERNAL_get_mousePosition
    SET_METRO_BINDING(Input_Get_Custom_PropMousePresent); //  <- UnityEngine.Input::get_mousePresent
    SET_METRO_BINDING(Input_Get_Custom_PropSimulateMouseWithTouches); //  <- UnityEngine.Input::get_simulateMouseWithTouches
    SET_METRO_BINDING(Input_Set_Custom_PropSimulateMouseWithTouches); //  <- UnityEngine.Input::set_simulateMouseWithTouches
    SET_METRO_BINDING(Input_Get_Custom_PropAnyKey); //  <- UnityEngine.Input::get_anyKey
    SET_METRO_BINDING(Input_Get_Custom_PropAnyKeyDown); //  <- UnityEngine.Input::get_anyKeyDown
    SET_METRO_BINDING(Input_Get_Custom_PropInputString); //  <- UnityEngine.Input::get_inputString
    SET_METRO_BINDING(Input_CUSTOM_INTERNAL_get_acceleration); //  <- UnityEngine.Input::INTERNAL_get_acceleration
    SET_METRO_BINDING(Input_CUSTOM_INTERNAL_CALL_GetAccelerationEvent); //  <- UnityEngine.Input::INTERNAL_CALL_GetAccelerationEvent
    SET_METRO_BINDING(Input_Get_Custom_PropAccelerationEventCount); //  <- UnityEngine.Input::get_accelerationEventCount
    SET_METRO_BINDING(Input_CUSTOM_INTERNAL_CALL_GetTouch); //  <- UnityEngine.Input::INTERNAL_CALL_GetTouch
    SET_METRO_BINDING(Input_Get_Custom_PropTouchCount); //  <- UnityEngine.Input::get_touchCount
    SET_METRO_BINDING(Input_Get_Custom_PropEatKeyPressOnTextFieldFocus); //  <- UnityEngine.Input::get_eatKeyPressOnTextFieldFocus
    SET_METRO_BINDING(Input_Set_Custom_PropEatKeyPressOnTextFieldFocus); //  <- UnityEngine.Input::set_eatKeyPressOnTextFieldFocus
#if !UNITY_FLASH && !PLATFORM_WEBGL
    SET_METRO_BINDING(Input_Get_Custom_PropMultiTouchEnabled); //  <- UnityEngine.Input::get_multiTouchEnabled
    SET_METRO_BINDING(Input_Set_Custom_PropMultiTouchEnabled); //  <- UnityEngine.Input::set_multiTouchEnabled
#endif
    SET_METRO_BINDING(Input_Get_Custom_PropDeviceOrientation); //  <- UnityEngine.Input::get_deviceOrientation
    SET_METRO_BINDING(Input_Get_Custom_PropImeCompositionMode); //  <- UnityEngine.Input::get_imeCompositionMode
    SET_METRO_BINDING(Input_Set_Custom_PropImeCompositionMode); //  <- UnityEngine.Input::set_imeCompositionMode
    SET_METRO_BINDING(Input_Get_Custom_PropCompositionString); //  <- UnityEngine.Input::get_compositionString
    SET_METRO_BINDING(Input_Get_Custom_PropImeIsSelected); //  <- UnityEngine.Input::get_imeIsSelected
    SET_METRO_BINDING(Input_CUSTOM_INTERNAL_get_compositionCursorPos); //  <- UnityEngine.Input::INTERNAL_get_compositionCursorPos
    SET_METRO_BINDING(Input_CUSTOM_INTERNAL_set_compositionCursorPos); //  <- UnityEngine.Input::INTERNAL_set_compositionCursorPos
}

#endif
