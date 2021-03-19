#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Animation/Animation.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/Animation/AnimationManager.h"
#include "Runtime/Animation/AnimationState.h"
#include "Runtime/Animation/Animator.h"
#include "Runtime/Animation/Motion.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Animation/AnimationCurveUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoScript.h"
#include "Runtime/Scripting/ScriptingManager.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/ScriptingObjectWithIntPtrField.h"
#include "Runtime/mecanim/human/human.h"
#include "Runtime/mecanim/generic/crc32.h"

using namespace Unity;


	struct AnimationEventMono
	{
		AnimationEvent* m_Ptr;
		int m_OwnsData;
	};
	

 void VerifyReadOnly (ScriptingObjectWithIntPtrField<AnimationEvent>& self)
	{
#if ENABLE_MONO
		if (ExtractMonoObjectData<AnimationEventMono> (self.object).m_OwnsData != 1)
			RaiseMonoException("AnimationEvents sent by an Animation Event callback may not modify the AnimationEvent data");
#endif
	}

 inline AnimationEvent* GetAnimationEvent (ScriptingObjectWithIntPtrField<AnimationEvent>& self)
	{
		AnimationEvent* event = self.GetPtr();
		if (!event)
			RaiseNullException("Animation Event is out of scope");

		return event;
	}
	
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_CUSTOM_Create(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_CUSTOM_Create)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Create)
    
    		self.SetPtr(new AnimationEvent());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_CUSTOM_Destroy(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_CUSTOM_Destroy)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Destroy)
    
    		delete self.GetPtr();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropData(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropData)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_data)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_data)
    return CreateScriptingString(GetAnimationEvent(self)->stringParameter);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropData(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropData)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_data)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_data)
     VerifyReadOnly(self); GetAnimationEvent(self)->stringParameter = value.ToUTF8(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropStringParameter(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropStringParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stringParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stringParameter)
    return CreateScriptingString(GetAnimationEvent(self)->stringParameter);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropStringParameter(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropStringParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_stringParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stringParameter)
     VerifyReadOnly(self); GetAnimationEvent(self)->stringParameter = value.ToUTF8(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropFloatParameter(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropFloatParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_floatParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_floatParameter)
    return GetAnimationEvent(self)->floatParameter;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropFloatParameter(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropFloatParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_floatParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_floatParameter)
     VerifyReadOnly(self); GetAnimationEvent(self)->floatParameter = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropIntParameter(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropIntParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_intParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_intParameter)
    return GetAnimationEvent(self)->intParameter;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropIntParameter(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropIntParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_intParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_intParameter)
     VerifyReadOnly(self); GetAnimationEvent(self)->intParameter = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropObjectReferenceParameter(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropObjectReferenceParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_objectReferenceParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_objectReferenceParameter)
    return Scripting::ScriptingWrapperFor(GetAnimationEvent(self)->objectReferenceParameter);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropObjectReferenceParameter(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropObjectReferenceParameter)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Object> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_objectReferenceParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_objectReferenceParameter)
     VerifyReadOnly(self); GetAnimationEvent(self)->objectReferenceParameter = (Object*)value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropFunctionName(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropFunctionName)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_functionName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_functionName)
    return CreateScriptingString(GetAnimationEvent(self)->functionName);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropFunctionName(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropFunctionName)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_functionName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_functionName)
     VerifyReadOnly(self);GetAnimationEvent(self)->functionName = value.ToUTF8(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropTime(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropTime)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time)
    return GetAnimationEvent(self)->time;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropTime(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropTime)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_time)
     VerifyReadOnly(self); GetAnimationEvent(self)->time = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropMessageOptions(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropMessageOptions)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_messageOptions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_messageOptions)
    return GetAnimationEvent(self)->messageOptions;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationEvent_Set_Custom_PropMessageOptions(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Set_Custom_PropMessageOptions)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_messageOptions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_messageOptions)
     VerifyReadOnly(self); GetAnimationEvent(self)->messageOptions = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AnimationEvent_Get_Custom_PropAnimationState(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationEvent_Get_Custom_PropAnimationState)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_animationState)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_animationState)
    return TrackedReferenceBaseToScriptingObject(GetAnimationEvent(self)->stateSender, animationState);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_Internal_CreateAnimationClip(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_Internal_CreateAnimationClip)
    ScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateAnimationClip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateAnimationClip)
    
    		Object* animClip = NEW_OBJECT(AnimationClip);
    		animClip->Reset();
    		ConnectScriptingWrapperToObject (self.GetScriptingObject(), animClip);
    		animClip->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationClip_Get_Custom_PropLength(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_Get_Custom_PropLength)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_length)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_length)
    return self->GetRange ().second;
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationClip_Get_Custom_PropStartTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_Get_Custom_PropStartTime)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startTime)
    return self->GetRange ().first;
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationClip_Get_Custom_PropStopTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_Get_Custom_PropStopTime)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stopTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stopTime)
    return self->GetRange ().second;
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationClip_Get_Custom_PropFrameRate(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_Get_Custom_PropFrameRate)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_frameRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_frameRate)
    return self->GetSampleRate ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_SetCurve(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument relativePath_, ICallType_SystemTypeObject_Argument type_, ICallType_String_Argument propertyName_, ICallType_Object_Argument curve_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_SetCurve)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    ICallType_String_Local relativePath(relativePath_);
    UNUSED(relativePath);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    ScriptingObjectWithIntPtrField<AnimationCurve> curve(curve_);
    UNUSED(curve);
    SCRIPTINGAPI_STACK_CHECK(SetCurve)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetCurve)
    
    #if ENABLE_MONO
    		MonoRaiseIfNull(type);
    		MonoClass* klass = GetScriptingTypeRegistry().GetType(type);
    		MonoScript* script = NULL;
    		int classID = GetClassIDFromScriptingClass(klass);
    		if (classID == ClassID(MonoBehaviour))
    		{
    			script = GetMonoScriptManager().FindRuntimeScript(klass);
    			if (script == NULL)
    			{
    				ErrorString("The script class couldn't be found");
    				return;
    			}
    		}	
    
    		self->SetCurve(relativePath, classID, script, propertyName, curve.GetPtr(), true);
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_INTERNAL_CALL_EnsureQuaternionContinuity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_INTERNAL_CALL_EnsureQuaternionContinuity)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_EnsureQuaternionContinuity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_EnsureQuaternionContinuity)
    return self->EnsureQuaternionContinuity();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_INTERNAL_CALL_ClearCurves(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_INTERNAL_CALL_ClearCurves)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ClearCurves)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ClearCurves)
    return self->ClearCurves();
}

SCRIPT_BINDINGS_EXPORT_DECL
WrapMode SCRIPT_CALL_CONVENTION AnimationClip_Get_Custom_PropWrapMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_Get_Custom_PropWrapMode)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wrapMode)
    return self->GetWrapMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_Set_Custom_PropWrapMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, WrapMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_Set_Custom_PropWrapMode)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wrapMode)
    
    self->SetWrapMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_AddEvent(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument evt_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_AddEvent)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    ICallType_Object_Local evt(evt_);
    UNUSED(evt);
    SCRIPTINGAPI_STACK_CHECK(AddEvent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddEvent)
    
    		MonoRaiseIfNull(evt.GetPtr());
    		self->AddRuntimeEvent(*GetAnimationEvent(evt));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_INTERNAL_get_localBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_INTERNAL_get_localBounds)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localBounds)
    *returnValue = self->GetBounds();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationClip_CUSTOM_INTERNAL_set_localBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, const AABB& value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationClip_CUSTOM_INTERNAL_set_localBounds)
    ReadOnlyScriptingObjectOfType<AnimationClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localBounds)
    
    self->SetBounds (value);
    
}


 static void CleanupAnimationCurve(void* animationCurve){ delete ((AnimationCurve*)animationCurve); };
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_Cleanup)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
     CleanupAnimationCurve(self.GetPtr()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_Evaluate(ICallType_Object_Argument self_, float time)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_Evaluate)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Evaluate)
    
    		return self->Evaluate(time);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_AddKey(ICallType_Object_Argument self_, float time, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_AddKey)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(AddKey)
     return AddKeySmoothTangents(*self, time, value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_INTERNAL_CALL_AddKey_Internal(ICallType_Object_Argument self_, const AnimationCurve::Keyframe& key)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_INTERNAL_CALL_AddKey_Internal)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddKey_Internal)
     return self->AddKey (key); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_INTERNAL_CALL_MoveKey(ICallType_Object_Argument self_, int index, const AnimationCurve::Keyframe& key)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_INTERNAL_CALL_MoveKey)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_MoveKey)
    
    		if (index >= 0 && index < self->GetKeyCount())
    			return MoveCurveKey(*self, index, key);
    		else {
    			RaiseOutOfRangeException("");
    			return 0;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_RemoveKey(ICallType_Object_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_RemoveKey)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RemoveKey)
    
    		if (index >= 0 && index < self->GetKeyCount())
    			self->RemoveKeys(self->begin() + index, self->begin() + index + 1);
    		else
    			RaiseOutOfRangeException("");
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationCurve_Get_Custom_PropLength(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_Get_Custom_PropLength)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_length)
    return self->GetKeyCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_SetKeys(ICallType_Object_Argument self_, ICallType_Array_Argument keys_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_SetKeys)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    ICallType_Array_Local keys(keys_);
    UNUSED(keys);
    SCRIPTINGAPI_STACK_CHECK(SetKeys)
    
    		AnimationCurve::Keyframe* first = GetScriptingArrayStart<AnimationCurve::Keyframe > (keys);
    		self->Assign(first, first + GetScriptingArraySize(keys));
    		self->Sort();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_INTERNAL_CALL_GetKey_Internal(ICallType_Object_Argument self_, int index, AnimationCurve::Keyframe& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_INTERNAL_CALL_GetKey_Internal)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetKey_Internal)
    
    		if (index >= 0 && index < self->GetKeyCount())
    		{
    			{ returnValue =(self->GetKey(index)); return; }
    		}
    		else
    		{
    			RaiseOutOfRangeException("");
    			{ returnValue =(AnimationCurve::Keyframe()); return; }
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_GetKeys(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_GetKeys)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetKeys)
    
    		if (self->GetKeyCount() <= 0)
    			return CreateEmptyStructArray(GetCommonScriptingClasses ().keyframe);
    		return CreateScriptingArray(&self->GetKey(0), self->GetKeyCount(), GetCommonScriptingClasses ().keyframe);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_SmoothTangents(ICallType_Object_Argument self_, int index, float weight)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_SmoothTangents)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SmoothTangents)
    
    		if (index >= 0 && index < self->GetKeyCount())
    			RecalculateSplineSlope(*self, index, weight);
    		else
    		{
    #if ENABLE_MONO			
    			RaiseOutOfRangeException("");
    #endif
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
WrapMode SCRIPT_CALL_CONVENTION AnimationCurve_Get_Custom_PropPreWrapMode(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_Get_Custom_PropPreWrapMode)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_preWrapMode)
    return self->GetPreInfinity();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_Set_Custom_PropPreWrapMode(ICallType_Object_Argument self_, WrapMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_Set_Custom_PropPreWrapMode)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_preWrapMode)
     self->SetPreInfinity(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
WrapMode SCRIPT_CALL_CONVENTION AnimationCurve_Get_Custom_PropPostWrapMode(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_Get_Custom_PropPostWrapMode)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_postWrapMode)
    return self->GetPostInfinity();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_Set_Custom_PropPostWrapMode(ICallType_Object_Argument self_, WrapMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_Set_Custom_PropPostWrapMode)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_postWrapMode)
     self->SetPostInfinity(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationCurve_CUSTOM_Init(ICallType_Object_Argument self_, ICallType_Array_Argument keys_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationCurve_CUSTOM_Init)
    ScriptingObjectWithIntPtrField<AnimationCurve> self(self_);
    UNUSED(self);
    ICallType_Array_Local keys(keys_);
    UNUSED(keys);
    SCRIPTINGAPI_STACK_CHECK(Init)
    
    		self.SetPtr(new AnimationCurve(), CleanupAnimationCurve);
    		#if PLATFORM_WINRT
    		if (keys != SCRIPTING_NULL) AnimationCurve_CUSTOM_SetKeys(self.object, keys); 
    		#else
    		if (keys != SCRIPTING_NULL) AnimationCurve_CUSTOM_SetKeys(self, keys); 
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropClip(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropClip)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clip)
    return Scripting::ScriptingWrapperFor(self->GetClip());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_Set_Custom_PropClip(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Set_Custom_PropClip)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AnimationClip> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_clip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_clip)
    
    self->SetClip (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropPlayAutomatically(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropPlayAutomatically)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_playAutomatically)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_playAutomatically)
    return self->GetPlayAutomatically ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_Set_Custom_PropPlayAutomatically(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Set_Custom_PropPlayAutomatically)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_playAutomatically)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_playAutomatically)
    
    self->SetPlayAutomatically (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
WrapMode SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropWrapMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropWrapMode)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wrapMode)
    return self->GetWrapMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_Set_Custom_PropWrapMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, WrapMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Set_Custom_PropWrapMode)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wrapMode)
    
    self->SetWrapMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_INTERNAL_CALL_Stop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_INTERNAL_CALL_Stop)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Stop)
    return self->Stop();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_Internal_StopByName(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_Internal_StopByName)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_StopByName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_StopByName)
     return self->Stop (name); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_Internal_RewindByName(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_Internal_RewindByName)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_RewindByName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_RewindByName)
     self->Rewind(name); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_INTERNAL_CALL_Rewind(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_INTERNAL_CALL_Rewind)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Rewind)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Rewind)
    return self->Rewind();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_INTERNAL_CALL_Sample(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_INTERNAL_CALL_Sample)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Sample)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Sample)
    return self->Sample();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropIsPlaying(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropIsPlaying)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPlaying)
    return self->IsPlaying ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_CUSTOM_IsPlaying(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_IsPlaying)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(IsPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsPlaying)
     return self->IsPlaying (name); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_CUSTOM_Play(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument animation_, Animation::PlayMode mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_Play)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(Play)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Play)
     return self->Play(animation, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_CrossFade(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument animation_, float fadeLength, Animation::PlayMode mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_CrossFade)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(CrossFade)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CrossFade)
     self->CrossFade(animation, fadeLength, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_Blend(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument animation_, float targetWeight, float fadeLength)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_Blend)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(Blend)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Blend)
     self->Blend(animation, targetWeight, fadeLength); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animation_CUSTOM_CrossFadeQueued(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument animation_, float fadeLength, int queue, Animation::PlayMode mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_CrossFadeQueued)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(CrossFadeQueued)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CrossFadeQueued)
     
    		AnimationState* as = self->QueueCrossFade(animation, fadeLength, queue, mode);
    		return TrackedReferenceBaseToScriptingObject(as,animationState); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animation_CUSTOM_PlayQueued(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument animation_, int queue, Animation::PlayMode mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_PlayQueued)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(PlayQueued)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PlayQueued)
     
    		AnimationState* as = self->QueuePlay(animation, queue, mode);
    		return TrackedReferenceBaseToScriptingObject(as, animationState); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_AddClip(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument clip_, ICallType_String_Argument newName_, int firstFrame, int lastFrame, ScriptingBool addLoopFrame)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_AddClip)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AnimationClip> clip(clip_);
    UNUSED(clip);
    ICallType_String_Local newName(newName_);
    UNUSED(newName);
    SCRIPTINGAPI_STACK_CHECK(AddClip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddClip)
     self->AddClip(*clip, newName, firstFrame, lastFrame, addLoopFrame); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_RemoveClip(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument clip_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_RemoveClip)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AnimationClip> clip(clip_);
    UNUSED(clip);
    SCRIPTINGAPI_STACK_CHECK(RemoveClip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RemoveClip)
     self->RemoveClip (*clip); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Animation_CUSTOM_GetClipCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_GetClipCount)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetClipCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetClipCount)
     return self->GetClipCount(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_RemoveClip2(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument clipName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_RemoveClip2)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local clipName(clipName_);
    UNUSED(clipName);
    SCRIPTINGAPI_STACK_CHECK(RemoveClip2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RemoveClip2)
     self->RemoveClip (clipName); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_CUSTOM_PlayDefaultAnimation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Animation::PlayMode mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_PlayDefaultAnimation)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(PlayDefaultAnimation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PlayDefaultAnimation)
     return self->Play(mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_INTERNAL_CALL_SyncLayer(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_INTERNAL_CALL_SyncLayer)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SyncLayer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SyncLayer)
    return self->SyncLayer(layer);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animation_CUSTOM_GetState(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_GetState)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetState)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetState)
    
    		AnimationState* state = self->GetState(name);
    		return TrackedReferenceBaseToScriptingObject(state, animationState);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animation_CUSTOM_GetStateAtIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_GetStateAtIndex)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetStateAtIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetStateAtIndex)
    
    
    		Animation& selfRef = *self;
    		if (index >= 0 || index < selfRef.GetAnimationStateCount())	
    		{
    			return TrackedReferenceBaseToScriptingObject(&selfRef.GetAnimationStateAtIndex (index), animationState);
    		}
    #if ENABLE_MONO
    		RaiseMonoException("Animation State out of bounds!");
    #endif
    		return SCRIPTING_NULL;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Animation_CUSTOM_GetStateCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_GetStateCount)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetStateCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetStateCount)
     return self->GetAnimationStateCount(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropAnimatePhysics(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropAnimatePhysics)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_animatePhysics)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_animatePhysics)
    return self->GetAnimatePhysics ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_Set_Custom_PropAnimatePhysics(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Set_Custom_PropAnimatePhysics)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_animatePhysics)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_animatePhysics)
    
    self->SetAnimatePhysics (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropAnimateOnlyIfVisible(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropAnimateOnlyIfVisible)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_animateOnlyIfVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_animateOnlyIfVisible)
    Animation::CullingType type = self->GetCullingType();
    AssertFormatMsg(type == Animation::kCulling_AlwaysAnimate || type == Animation::kCulling_BasedOnRenderers,
    "Culling type %d cannot be converted to animateOnlyIfVisible. animateOnlyIfVisible is obsolete, please use cullingType instead.", type);
    return type == Animation::kCulling_BasedOnRenderers;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_Set_Custom_PropAnimateOnlyIfVisible(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Set_Custom_PropAnimateOnlyIfVisible)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_animateOnlyIfVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_animateOnlyIfVisible)
    
    self->SetCullingType(value ? Animation::kCulling_BasedOnRenderers : Animation::kCulling_AlwaysAnimate);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
Animation::CullingType SCRIPT_CALL_CONVENTION Animation_Get_Custom_PropCullingType(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Get_Custom_PropCullingType)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cullingType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cullingType)
    return self->GetCullingType ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_Set_Custom_PropCullingType(ICallType_ReadOnlyUnityEngineObject_Argument self_, Animation::CullingType value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_Set_Custom_PropCullingType)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cullingType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cullingType)
    
    self->SetCullingType (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_INTERNAL_get_localBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_INTERNAL_get_localBounds)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localBounds)
    *returnValue = self->GetLocalAABB();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animation_CUSTOM_INTERNAL_set_localBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, const AABB& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animation_CUSTOM_INTERNAL_set_localBounds)
    ReadOnlyScriptingObjectOfType<Animation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localBounds)
    
    self->SetLocalAABB (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropEnabled(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropEnabled)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropEnabled(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropEnabled)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    self->SetEnabled (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropWeight(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropWeight)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_weight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_weight)
    return self->GetWeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropWeight(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropWeight)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_weight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_weight)
    
    self->SetWeight (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
WrapMode SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropWrapMode(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropWrapMode)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wrapMode)
    return self->GetWrapMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropWrapMode(ICallType_Object_Argument self_, WrapMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropWrapMode)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wrapMode)
    
    self->SetWrapMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropTime(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropTime)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time)
    return self->GetTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropTime(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropTime)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_time)
    
    self->SetTime (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropNormalizedTime(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropNormalizedTime)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_normalizedTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_normalizedTime)
    return self->GetNormalizedTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropNormalizedTime(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropNormalizedTime)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_normalizedTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_normalizedTime)
    
    self->SetNormalizedTime (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropSpeed(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropSpeed)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_speed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_speed)
    return self->GetSpeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropSpeed(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropSpeed)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_speed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_speed)
    
    self->SetSpeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropNormalizedSpeed(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropNormalizedSpeed)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_normalizedSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_normalizedSpeed)
    return self->GetNormalizedSpeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropNormalizedSpeed(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropNormalizedSpeed)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_normalizedSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_normalizedSpeed)
    
    self->SetNormalizedSpeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropLength(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropLength)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_length)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_length)
    return self->GetLength ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropLayer(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropLayer)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_layer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_layer)
    return self->GetLayer ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropLayer(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropLayer)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_layer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_layer)
    
    self->SetLayer (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropClip(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropClip)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clip)
    return Scripting::ScriptingWrapperFor(self->GetClip());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_CUSTOM_AddMixingTransform(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument mix_, ScriptingBool recursive)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_CUSTOM_AddMixingTransform)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> mix(mix_);
    UNUSED(mix);
    SCRIPTINGAPI_STACK_CHECK(AddMixingTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddMixingTransform)
     self->AddMixingTransform(*mix, recursive); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_CUSTOM_RemoveMixingTransform(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument mix_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_CUSTOM_RemoveMixingTransform)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> mix(mix_);
    UNUSED(mix);
    SCRIPTINGAPI_STACK_CHECK(RemoveMixingTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RemoveMixingTransform)
     self->RemoveMixingTransform(*mix); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropName(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropName)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_name)
    return CreateScriptingString(self->GetName());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropName(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropName)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_name)
     self->SetName(value.ToUTF8()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AnimationState_Get_Custom_PropBlendMode(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Get_Custom_PropBlendMode)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_blendMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_blendMode)
    return self->GetBlendMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AnimationState_Set_Custom_PropBlendMode(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AnimationState_Set_Custom_PropBlendMode)
    ScriptingObjectWithIntPtrField<AnimationState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_blendMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_blendMode)
    
    self->SetBlendMode (value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_AnimationEvent_Create()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::Create" , (gpointer)& AnimationEvent_CUSTOM_Create );
}

void Register_UnityEngine_AnimationEvent_Destroy()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::Destroy" , (gpointer)& AnimationEvent_CUSTOM_Destroy );
}

void Register_UnityEngine_AnimationEvent_get_data()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_data" , (gpointer)& AnimationEvent_Get_Custom_PropData );
}

void Register_UnityEngine_AnimationEvent_set_data()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_data" , (gpointer)& AnimationEvent_Set_Custom_PropData );
}

void Register_UnityEngine_AnimationEvent_get_stringParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_stringParameter" , (gpointer)& AnimationEvent_Get_Custom_PropStringParameter );
}

void Register_UnityEngine_AnimationEvent_set_stringParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_stringParameter" , (gpointer)& AnimationEvent_Set_Custom_PropStringParameter );
}

void Register_UnityEngine_AnimationEvent_get_floatParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_floatParameter" , (gpointer)& AnimationEvent_Get_Custom_PropFloatParameter );
}

void Register_UnityEngine_AnimationEvent_set_floatParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_floatParameter" , (gpointer)& AnimationEvent_Set_Custom_PropFloatParameter );
}

void Register_UnityEngine_AnimationEvent_get_intParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_intParameter" , (gpointer)& AnimationEvent_Get_Custom_PropIntParameter );
}

void Register_UnityEngine_AnimationEvent_set_intParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_intParameter" , (gpointer)& AnimationEvent_Set_Custom_PropIntParameter );
}

void Register_UnityEngine_AnimationEvent_get_objectReferenceParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_objectReferenceParameter" , (gpointer)& AnimationEvent_Get_Custom_PropObjectReferenceParameter );
}

void Register_UnityEngine_AnimationEvent_set_objectReferenceParameter()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_objectReferenceParameter" , (gpointer)& AnimationEvent_Set_Custom_PropObjectReferenceParameter );
}

void Register_UnityEngine_AnimationEvent_get_functionName()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_functionName" , (gpointer)& AnimationEvent_Get_Custom_PropFunctionName );
}

void Register_UnityEngine_AnimationEvent_set_functionName()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_functionName" , (gpointer)& AnimationEvent_Set_Custom_PropFunctionName );
}

void Register_UnityEngine_AnimationEvent_get_time()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_time" , (gpointer)& AnimationEvent_Get_Custom_PropTime );
}

void Register_UnityEngine_AnimationEvent_set_time()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_time" , (gpointer)& AnimationEvent_Set_Custom_PropTime );
}

void Register_UnityEngine_AnimationEvent_get_messageOptions()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_messageOptions" , (gpointer)& AnimationEvent_Get_Custom_PropMessageOptions );
}

void Register_UnityEngine_AnimationEvent_set_messageOptions()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::set_messageOptions" , (gpointer)& AnimationEvent_Set_Custom_PropMessageOptions );
}

void Register_UnityEngine_AnimationEvent_get_animationState()
{
    scripting_add_internal_call( "UnityEngine.AnimationEvent::get_animationState" , (gpointer)& AnimationEvent_Get_Custom_PropAnimationState );
}

void Register_UnityEngine_AnimationClip_Internal_CreateAnimationClip()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::Internal_CreateAnimationClip" , (gpointer)& AnimationClip_CUSTOM_Internal_CreateAnimationClip );
}

void Register_UnityEngine_AnimationClip_get_length()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::get_length" , (gpointer)& AnimationClip_Get_Custom_PropLength );
}

void Register_UnityEngine_AnimationClip_get_startTime()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::get_startTime" , (gpointer)& AnimationClip_Get_Custom_PropStartTime );
}

void Register_UnityEngine_AnimationClip_get_stopTime()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::get_stopTime" , (gpointer)& AnimationClip_Get_Custom_PropStopTime );
}

void Register_UnityEngine_AnimationClip_get_frameRate()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::get_frameRate" , (gpointer)& AnimationClip_Get_Custom_PropFrameRate );
}

void Register_UnityEngine_AnimationClip_SetCurve()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::SetCurve" , (gpointer)& AnimationClip_CUSTOM_SetCurve );
}

void Register_UnityEngine_AnimationClip_INTERNAL_CALL_EnsureQuaternionContinuity()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::INTERNAL_CALL_EnsureQuaternionContinuity" , (gpointer)& AnimationClip_CUSTOM_INTERNAL_CALL_EnsureQuaternionContinuity );
}

void Register_UnityEngine_AnimationClip_INTERNAL_CALL_ClearCurves()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::INTERNAL_CALL_ClearCurves" , (gpointer)& AnimationClip_CUSTOM_INTERNAL_CALL_ClearCurves );
}

void Register_UnityEngine_AnimationClip_get_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::get_wrapMode" , (gpointer)& AnimationClip_Get_Custom_PropWrapMode );
}

void Register_UnityEngine_AnimationClip_set_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::set_wrapMode" , (gpointer)& AnimationClip_Set_Custom_PropWrapMode );
}

void Register_UnityEngine_AnimationClip_AddEvent()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::AddEvent" , (gpointer)& AnimationClip_CUSTOM_AddEvent );
}

void Register_UnityEngine_AnimationClip_INTERNAL_get_localBounds()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::INTERNAL_get_localBounds" , (gpointer)& AnimationClip_CUSTOM_INTERNAL_get_localBounds );
}

void Register_UnityEngine_AnimationClip_INTERNAL_set_localBounds()
{
    scripting_add_internal_call( "UnityEngine.AnimationClip::INTERNAL_set_localBounds" , (gpointer)& AnimationClip_CUSTOM_INTERNAL_set_localBounds );
}

void Register_UnityEngine_AnimationCurve_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::Cleanup" , (gpointer)& AnimationCurve_CUSTOM_Cleanup );
}

void Register_UnityEngine_AnimationCurve_Evaluate()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::Evaluate" , (gpointer)& AnimationCurve_CUSTOM_Evaluate );
}

void Register_UnityEngine_AnimationCurve_AddKey()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::AddKey" , (gpointer)& AnimationCurve_CUSTOM_AddKey );
}

void Register_UnityEngine_AnimationCurve_INTERNAL_CALL_AddKey_Internal()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::INTERNAL_CALL_AddKey_Internal" , (gpointer)& AnimationCurve_CUSTOM_INTERNAL_CALL_AddKey_Internal );
}

void Register_UnityEngine_AnimationCurve_INTERNAL_CALL_MoveKey()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::INTERNAL_CALL_MoveKey" , (gpointer)& AnimationCurve_CUSTOM_INTERNAL_CALL_MoveKey );
}

void Register_UnityEngine_AnimationCurve_RemoveKey()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::RemoveKey" , (gpointer)& AnimationCurve_CUSTOM_RemoveKey );
}

void Register_UnityEngine_AnimationCurve_get_length()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::get_length" , (gpointer)& AnimationCurve_Get_Custom_PropLength );
}

void Register_UnityEngine_AnimationCurve_SetKeys()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::SetKeys" , (gpointer)& AnimationCurve_CUSTOM_SetKeys );
}

void Register_UnityEngine_AnimationCurve_INTERNAL_CALL_GetKey_Internal()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::INTERNAL_CALL_GetKey_Internal" , (gpointer)& AnimationCurve_CUSTOM_INTERNAL_CALL_GetKey_Internal );
}

void Register_UnityEngine_AnimationCurve_GetKeys()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::GetKeys" , (gpointer)& AnimationCurve_CUSTOM_GetKeys );
}

void Register_UnityEngine_AnimationCurve_SmoothTangents()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::SmoothTangents" , (gpointer)& AnimationCurve_CUSTOM_SmoothTangents );
}

void Register_UnityEngine_AnimationCurve_get_preWrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::get_preWrapMode" , (gpointer)& AnimationCurve_Get_Custom_PropPreWrapMode );
}

void Register_UnityEngine_AnimationCurve_set_preWrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::set_preWrapMode" , (gpointer)& AnimationCurve_Set_Custom_PropPreWrapMode );
}

void Register_UnityEngine_AnimationCurve_get_postWrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::get_postWrapMode" , (gpointer)& AnimationCurve_Get_Custom_PropPostWrapMode );
}

void Register_UnityEngine_AnimationCurve_set_postWrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::set_postWrapMode" , (gpointer)& AnimationCurve_Set_Custom_PropPostWrapMode );
}

void Register_UnityEngine_AnimationCurve_Init()
{
    scripting_add_internal_call( "UnityEngine.AnimationCurve::Init" , (gpointer)& AnimationCurve_CUSTOM_Init );
}

void Register_UnityEngine_Animation_get_clip()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_clip" , (gpointer)& Animation_Get_Custom_PropClip );
}

void Register_UnityEngine_Animation_set_clip()
{
    scripting_add_internal_call( "UnityEngine.Animation::set_clip" , (gpointer)& Animation_Set_Custom_PropClip );
}

void Register_UnityEngine_Animation_get_playAutomatically()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_playAutomatically" , (gpointer)& Animation_Get_Custom_PropPlayAutomatically );
}

void Register_UnityEngine_Animation_set_playAutomatically()
{
    scripting_add_internal_call( "UnityEngine.Animation::set_playAutomatically" , (gpointer)& Animation_Set_Custom_PropPlayAutomatically );
}

void Register_UnityEngine_Animation_get_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_wrapMode" , (gpointer)& Animation_Get_Custom_PropWrapMode );
}

void Register_UnityEngine_Animation_set_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.Animation::set_wrapMode" , (gpointer)& Animation_Set_Custom_PropWrapMode );
}

void Register_UnityEngine_Animation_INTERNAL_CALL_Stop()
{
    scripting_add_internal_call( "UnityEngine.Animation::INTERNAL_CALL_Stop" , (gpointer)& Animation_CUSTOM_INTERNAL_CALL_Stop );
}

void Register_UnityEngine_Animation_Internal_StopByName()
{
    scripting_add_internal_call( "UnityEngine.Animation::Internal_StopByName" , (gpointer)& Animation_CUSTOM_Internal_StopByName );
}

void Register_UnityEngine_Animation_Internal_RewindByName()
{
    scripting_add_internal_call( "UnityEngine.Animation::Internal_RewindByName" , (gpointer)& Animation_CUSTOM_Internal_RewindByName );
}

void Register_UnityEngine_Animation_INTERNAL_CALL_Rewind()
{
    scripting_add_internal_call( "UnityEngine.Animation::INTERNAL_CALL_Rewind" , (gpointer)& Animation_CUSTOM_INTERNAL_CALL_Rewind );
}

void Register_UnityEngine_Animation_INTERNAL_CALL_Sample()
{
    scripting_add_internal_call( "UnityEngine.Animation::INTERNAL_CALL_Sample" , (gpointer)& Animation_CUSTOM_INTERNAL_CALL_Sample );
}

void Register_UnityEngine_Animation_get_isPlaying()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_isPlaying" , (gpointer)& Animation_Get_Custom_PropIsPlaying );
}

void Register_UnityEngine_Animation_IsPlaying()
{
    scripting_add_internal_call( "UnityEngine.Animation::IsPlaying" , (gpointer)& Animation_CUSTOM_IsPlaying );
}

void Register_UnityEngine_Animation_Play()
{
    scripting_add_internal_call( "UnityEngine.Animation::Play" , (gpointer)& Animation_CUSTOM_Play );
}

void Register_UnityEngine_Animation_CrossFade()
{
    scripting_add_internal_call( "UnityEngine.Animation::CrossFade" , (gpointer)& Animation_CUSTOM_CrossFade );
}

void Register_UnityEngine_Animation_Blend()
{
    scripting_add_internal_call( "UnityEngine.Animation::Blend" , (gpointer)& Animation_CUSTOM_Blend );
}

void Register_UnityEngine_Animation_CrossFadeQueued()
{
    scripting_add_internal_call( "UnityEngine.Animation::CrossFadeQueued" , (gpointer)& Animation_CUSTOM_CrossFadeQueued );
}

void Register_UnityEngine_Animation_PlayQueued()
{
    scripting_add_internal_call( "UnityEngine.Animation::PlayQueued" , (gpointer)& Animation_CUSTOM_PlayQueued );
}

void Register_UnityEngine_Animation_AddClip()
{
    scripting_add_internal_call( "UnityEngine.Animation::AddClip" , (gpointer)& Animation_CUSTOM_AddClip );
}

void Register_UnityEngine_Animation_RemoveClip()
{
    scripting_add_internal_call( "UnityEngine.Animation::RemoveClip" , (gpointer)& Animation_CUSTOM_RemoveClip );
}

void Register_UnityEngine_Animation_GetClipCount()
{
    scripting_add_internal_call( "UnityEngine.Animation::GetClipCount" , (gpointer)& Animation_CUSTOM_GetClipCount );
}

void Register_UnityEngine_Animation_RemoveClip2()
{
    scripting_add_internal_call( "UnityEngine.Animation::RemoveClip2" , (gpointer)& Animation_CUSTOM_RemoveClip2 );
}

void Register_UnityEngine_Animation_PlayDefaultAnimation()
{
    scripting_add_internal_call( "UnityEngine.Animation::PlayDefaultAnimation" , (gpointer)& Animation_CUSTOM_PlayDefaultAnimation );
}

void Register_UnityEngine_Animation_INTERNAL_CALL_SyncLayer()
{
    scripting_add_internal_call( "UnityEngine.Animation::INTERNAL_CALL_SyncLayer" , (gpointer)& Animation_CUSTOM_INTERNAL_CALL_SyncLayer );
}

void Register_UnityEngine_Animation_GetState()
{
    scripting_add_internal_call( "UnityEngine.Animation::GetState" , (gpointer)& Animation_CUSTOM_GetState );
}

void Register_UnityEngine_Animation_GetStateAtIndex()
{
    scripting_add_internal_call( "UnityEngine.Animation::GetStateAtIndex" , (gpointer)& Animation_CUSTOM_GetStateAtIndex );
}

void Register_UnityEngine_Animation_GetStateCount()
{
    scripting_add_internal_call( "UnityEngine.Animation::GetStateCount" , (gpointer)& Animation_CUSTOM_GetStateCount );
}

void Register_UnityEngine_Animation_get_animatePhysics()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_animatePhysics" , (gpointer)& Animation_Get_Custom_PropAnimatePhysics );
}

void Register_UnityEngine_Animation_set_animatePhysics()
{
    scripting_add_internal_call( "UnityEngine.Animation::set_animatePhysics" , (gpointer)& Animation_Set_Custom_PropAnimatePhysics );
}

void Register_UnityEngine_Animation_get_animateOnlyIfVisible()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_animateOnlyIfVisible" , (gpointer)& Animation_Get_Custom_PropAnimateOnlyIfVisible );
}

void Register_UnityEngine_Animation_set_animateOnlyIfVisible()
{
    scripting_add_internal_call( "UnityEngine.Animation::set_animateOnlyIfVisible" , (gpointer)& Animation_Set_Custom_PropAnimateOnlyIfVisible );
}

void Register_UnityEngine_Animation_get_cullingType()
{
    scripting_add_internal_call( "UnityEngine.Animation::get_cullingType" , (gpointer)& Animation_Get_Custom_PropCullingType );
}

void Register_UnityEngine_Animation_set_cullingType()
{
    scripting_add_internal_call( "UnityEngine.Animation::set_cullingType" , (gpointer)& Animation_Set_Custom_PropCullingType );
}

void Register_UnityEngine_Animation_INTERNAL_get_localBounds()
{
    scripting_add_internal_call( "UnityEngine.Animation::INTERNAL_get_localBounds" , (gpointer)& Animation_CUSTOM_INTERNAL_get_localBounds );
}

void Register_UnityEngine_Animation_INTERNAL_set_localBounds()
{
    scripting_add_internal_call( "UnityEngine.Animation::INTERNAL_set_localBounds" , (gpointer)& Animation_CUSTOM_INTERNAL_set_localBounds );
}

void Register_UnityEngine_AnimationState_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_enabled" , (gpointer)& AnimationState_Get_Custom_PropEnabled );
}

void Register_UnityEngine_AnimationState_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_enabled" , (gpointer)& AnimationState_Set_Custom_PropEnabled );
}

void Register_UnityEngine_AnimationState_get_weight()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_weight" , (gpointer)& AnimationState_Get_Custom_PropWeight );
}

void Register_UnityEngine_AnimationState_set_weight()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_weight" , (gpointer)& AnimationState_Set_Custom_PropWeight );
}

void Register_UnityEngine_AnimationState_get_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_wrapMode" , (gpointer)& AnimationState_Get_Custom_PropWrapMode );
}

void Register_UnityEngine_AnimationState_set_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_wrapMode" , (gpointer)& AnimationState_Set_Custom_PropWrapMode );
}

void Register_UnityEngine_AnimationState_get_time()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_time" , (gpointer)& AnimationState_Get_Custom_PropTime );
}

void Register_UnityEngine_AnimationState_set_time()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_time" , (gpointer)& AnimationState_Set_Custom_PropTime );
}

void Register_UnityEngine_AnimationState_get_normalizedTime()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_normalizedTime" , (gpointer)& AnimationState_Get_Custom_PropNormalizedTime );
}

void Register_UnityEngine_AnimationState_set_normalizedTime()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_normalizedTime" , (gpointer)& AnimationState_Set_Custom_PropNormalizedTime );
}

void Register_UnityEngine_AnimationState_get_speed()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_speed" , (gpointer)& AnimationState_Get_Custom_PropSpeed );
}

void Register_UnityEngine_AnimationState_set_speed()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_speed" , (gpointer)& AnimationState_Set_Custom_PropSpeed );
}

void Register_UnityEngine_AnimationState_get_normalizedSpeed()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_normalizedSpeed" , (gpointer)& AnimationState_Get_Custom_PropNormalizedSpeed );
}

void Register_UnityEngine_AnimationState_set_normalizedSpeed()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_normalizedSpeed" , (gpointer)& AnimationState_Set_Custom_PropNormalizedSpeed );
}

void Register_UnityEngine_AnimationState_get_length()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_length" , (gpointer)& AnimationState_Get_Custom_PropLength );
}

void Register_UnityEngine_AnimationState_get_layer()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_layer" , (gpointer)& AnimationState_Get_Custom_PropLayer );
}

void Register_UnityEngine_AnimationState_set_layer()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_layer" , (gpointer)& AnimationState_Set_Custom_PropLayer );
}

void Register_UnityEngine_AnimationState_get_clip()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_clip" , (gpointer)& AnimationState_Get_Custom_PropClip );
}

void Register_UnityEngine_AnimationState_AddMixingTransform()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::AddMixingTransform" , (gpointer)& AnimationState_CUSTOM_AddMixingTransform );
}

void Register_UnityEngine_AnimationState_RemoveMixingTransform()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::RemoveMixingTransform" , (gpointer)& AnimationState_CUSTOM_RemoveMixingTransform );
}

void Register_UnityEngine_AnimationState_get_name()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_name" , (gpointer)& AnimationState_Get_Custom_PropName );
}

void Register_UnityEngine_AnimationState_set_name()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_name" , (gpointer)& AnimationState_Set_Custom_PropName );
}

void Register_UnityEngine_AnimationState_get_blendMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::get_blendMode" , (gpointer)& AnimationState_Get_Custom_PropBlendMode );
}

void Register_UnityEngine_AnimationState_set_blendMode()
{
    scripting_add_internal_call( "UnityEngine.AnimationState::set_blendMode" , (gpointer)& AnimationState_Set_Custom_PropBlendMode );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Animations_IcallNames [] =
{
    "UnityEngine.AnimationEvent::Create"    ,    // -> AnimationEvent_CUSTOM_Create
    "UnityEngine.AnimationEvent::Destroy"   ,    // -> AnimationEvent_CUSTOM_Destroy
    "UnityEngine.AnimationEvent::get_data"  ,    // -> AnimationEvent_Get_Custom_PropData
    "UnityEngine.AnimationEvent::set_data"  ,    // -> AnimationEvent_Set_Custom_PropData
    "UnityEngine.AnimationEvent::get_stringParameter",    // -> AnimationEvent_Get_Custom_PropStringParameter
    "UnityEngine.AnimationEvent::set_stringParameter",    // -> AnimationEvent_Set_Custom_PropStringParameter
    "UnityEngine.AnimationEvent::get_floatParameter",    // -> AnimationEvent_Get_Custom_PropFloatParameter
    "UnityEngine.AnimationEvent::set_floatParameter",    // -> AnimationEvent_Set_Custom_PropFloatParameter
    "UnityEngine.AnimationEvent::get_intParameter",    // -> AnimationEvent_Get_Custom_PropIntParameter
    "UnityEngine.AnimationEvent::set_intParameter",    // -> AnimationEvent_Set_Custom_PropIntParameter
    "UnityEngine.AnimationEvent::get_objectReferenceParameter",    // -> AnimationEvent_Get_Custom_PropObjectReferenceParameter
    "UnityEngine.AnimationEvent::set_objectReferenceParameter",    // -> AnimationEvent_Set_Custom_PropObjectReferenceParameter
    "UnityEngine.AnimationEvent::get_functionName",    // -> AnimationEvent_Get_Custom_PropFunctionName
    "UnityEngine.AnimationEvent::set_functionName",    // -> AnimationEvent_Set_Custom_PropFunctionName
    "UnityEngine.AnimationEvent::get_time"  ,    // -> AnimationEvent_Get_Custom_PropTime
    "UnityEngine.AnimationEvent::set_time"  ,    // -> AnimationEvent_Set_Custom_PropTime
    "UnityEngine.AnimationEvent::get_messageOptions",    // -> AnimationEvent_Get_Custom_PropMessageOptions
    "UnityEngine.AnimationEvent::set_messageOptions",    // -> AnimationEvent_Set_Custom_PropMessageOptions
    "UnityEngine.AnimationEvent::get_animationState",    // -> AnimationEvent_Get_Custom_PropAnimationState
    "UnityEngine.AnimationClip::Internal_CreateAnimationClip",    // -> AnimationClip_CUSTOM_Internal_CreateAnimationClip
    "UnityEngine.AnimationClip::get_length" ,    // -> AnimationClip_Get_Custom_PropLength
    "UnityEngine.AnimationClip::get_startTime",    // -> AnimationClip_Get_Custom_PropStartTime
    "UnityEngine.AnimationClip::get_stopTime",    // -> AnimationClip_Get_Custom_PropStopTime
    "UnityEngine.AnimationClip::get_frameRate",    // -> AnimationClip_Get_Custom_PropFrameRate
    "UnityEngine.AnimationClip::SetCurve"   ,    // -> AnimationClip_CUSTOM_SetCurve
    "UnityEngine.AnimationClip::INTERNAL_CALL_EnsureQuaternionContinuity",    // -> AnimationClip_CUSTOM_INTERNAL_CALL_EnsureQuaternionContinuity
    "UnityEngine.AnimationClip::INTERNAL_CALL_ClearCurves",    // -> AnimationClip_CUSTOM_INTERNAL_CALL_ClearCurves
    "UnityEngine.AnimationClip::get_wrapMode",    // -> AnimationClip_Get_Custom_PropWrapMode
    "UnityEngine.AnimationClip::set_wrapMode",    // -> AnimationClip_Set_Custom_PropWrapMode
    "UnityEngine.AnimationClip::AddEvent"   ,    // -> AnimationClip_CUSTOM_AddEvent
    "UnityEngine.AnimationClip::INTERNAL_get_localBounds",    // -> AnimationClip_CUSTOM_INTERNAL_get_localBounds
    "UnityEngine.AnimationClip::INTERNAL_set_localBounds",    // -> AnimationClip_CUSTOM_INTERNAL_set_localBounds
    "UnityEngine.AnimationCurve::Cleanup"   ,    // -> AnimationCurve_CUSTOM_Cleanup
    "UnityEngine.AnimationCurve::Evaluate"  ,    // -> AnimationCurve_CUSTOM_Evaluate
    "UnityEngine.AnimationCurve::AddKey"    ,    // -> AnimationCurve_CUSTOM_AddKey
    "UnityEngine.AnimationCurve::INTERNAL_CALL_AddKey_Internal",    // -> AnimationCurve_CUSTOM_INTERNAL_CALL_AddKey_Internal
    "UnityEngine.AnimationCurve::INTERNAL_CALL_MoveKey",    // -> AnimationCurve_CUSTOM_INTERNAL_CALL_MoveKey
    "UnityEngine.AnimationCurve::RemoveKey" ,    // -> AnimationCurve_CUSTOM_RemoveKey
    "UnityEngine.AnimationCurve::get_length",    // -> AnimationCurve_Get_Custom_PropLength
    "UnityEngine.AnimationCurve::SetKeys"   ,    // -> AnimationCurve_CUSTOM_SetKeys
    "UnityEngine.AnimationCurve::INTERNAL_CALL_GetKey_Internal",    // -> AnimationCurve_CUSTOM_INTERNAL_CALL_GetKey_Internal
    "UnityEngine.AnimationCurve::GetKeys"   ,    // -> AnimationCurve_CUSTOM_GetKeys
    "UnityEngine.AnimationCurve::SmoothTangents",    // -> AnimationCurve_CUSTOM_SmoothTangents
    "UnityEngine.AnimationCurve::get_preWrapMode",    // -> AnimationCurve_Get_Custom_PropPreWrapMode
    "UnityEngine.AnimationCurve::set_preWrapMode",    // -> AnimationCurve_Set_Custom_PropPreWrapMode
    "UnityEngine.AnimationCurve::get_postWrapMode",    // -> AnimationCurve_Get_Custom_PropPostWrapMode
    "UnityEngine.AnimationCurve::set_postWrapMode",    // -> AnimationCurve_Set_Custom_PropPostWrapMode
    "UnityEngine.AnimationCurve::Init"      ,    // -> AnimationCurve_CUSTOM_Init
    "UnityEngine.Animation::get_clip"       ,    // -> Animation_Get_Custom_PropClip
    "UnityEngine.Animation::set_clip"       ,    // -> Animation_Set_Custom_PropClip
    "UnityEngine.Animation::get_playAutomatically",    // -> Animation_Get_Custom_PropPlayAutomatically
    "UnityEngine.Animation::set_playAutomatically",    // -> Animation_Set_Custom_PropPlayAutomatically
    "UnityEngine.Animation::get_wrapMode"   ,    // -> Animation_Get_Custom_PropWrapMode
    "UnityEngine.Animation::set_wrapMode"   ,    // -> Animation_Set_Custom_PropWrapMode
    "UnityEngine.Animation::INTERNAL_CALL_Stop",    // -> Animation_CUSTOM_INTERNAL_CALL_Stop
    "UnityEngine.Animation::Internal_StopByName",    // -> Animation_CUSTOM_Internal_StopByName
    "UnityEngine.Animation::Internal_RewindByName",    // -> Animation_CUSTOM_Internal_RewindByName
    "UnityEngine.Animation::INTERNAL_CALL_Rewind",    // -> Animation_CUSTOM_INTERNAL_CALL_Rewind
    "UnityEngine.Animation::INTERNAL_CALL_Sample",    // -> Animation_CUSTOM_INTERNAL_CALL_Sample
    "UnityEngine.Animation::get_isPlaying"  ,    // -> Animation_Get_Custom_PropIsPlaying
    "UnityEngine.Animation::IsPlaying"      ,    // -> Animation_CUSTOM_IsPlaying
    "UnityEngine.Animation::Play"           ,    // -> Animation_CUSTOM_Play
    "UnityEngine.Animation::CrossFade"      ,    // -> Animation_CUSTOM_CrossFade
    "UnityEngine.Animation::Blend"          ,    // -> Animation_CUSTOM_Blend
    "UnityEngine.Animation::CrossFadeQueued",    // -> Animation_CUSTOM_CrossFadeQueued
    "UnityEngine.Animation::PlayQueued"     ,    // -> Animation_CUSTOM_PlayQueued
    "UnityEngine.Animation::AddClip"        ,    // -> Animation_CUSTOM_AddClip
    "UnityEngine.Animation::RemoveClip"     ,    // -> Animation_CUSTOM_RemoveClip
    "UnityEngine.Animation::GetClipCount"   ,    // -> Animation_CUSTOM_GetClipCount
    "UnityEngine.Animation::RemoveClip2"    ,    // -> Animation_CUSTOM_RemoveClip2
    "UnityEngine.Animation::PlayDefaultAnimation",    // -> Animation_CUSTOM_PlayDefaultAnimation
    "UnityEngine.Animation::INTERNAL_CALL_SyncLayer",    // -> Animation_CUSTOM_INTERNAL_CALL_SyncLayer
    "UnityEngine.Animation::GetState"       ,    // -> Animation_CUSTOM_GetState
    "UnityEngine.Animation::GetStateAtIndex",    // -> Animation_CUSTOM_GetStateAtIndex
    "UnityEngine.Animation::GetStateCount"  ,    // -> Animation_CUSTOM_GetStateCount
    "UnityEngine.Animation::get_animatePhysics",    // -> Animation_Get_Custom_PropAnimatePhysics
    "UnityEngine.Animation::set_animatePhysics",    // -> Animation_Set_Custom_PropAnimatePhysics
    "UnityEngine.Animation::get_animateOnlyIfVisible",    // -> Animation_Get_Custom_PropAnimateOnlyIfVisible
    "UnityEngine.Animation::set_animateOnlyIfVisible",    // -> Animation_Set_Custom_PropAnimateOnlyIfVisible
    "UnityEngine.Animation::get_cullingType",    // -> Animation_Get_Custom_PropCullingType
    "UnityEngine.Animation::set_cullingType",    // -> Animation_Set_Custom_PropCullingType
    "UnityEngine.Animation::INTERNAL_get_localBounds",    // -> Animation_CUSTOM_INTERNAL_get_localBounds
    "UnityEngine.Animation::INTERNAL_set_localBounds",    // -> Animation_CUSTOM_INTERNAL_set_localBounds
    "UnityEngine.AnimationState::get_enabled",    // -> AnimationState_Get_Custom_PropEnabled
    "UnityEngine.AnimationState::set_enabled",    // -> AnimationState_Set_Custom_PropEnabled
    "UnityEngine.AnimationState::get_weight",    // -> AnimationState_Get_Custom_PropWeight
    "UnityEngine.AnimationState::set_weight",    // -> AnimationState_Set_Custom_PropWeight
    "UnityEngine.AnimationState::get_wrapMode",    // -> AnimationState_Get_Custom_PropWrapMode
    "UnityEngine.AnimationState::set_wrapMode",    // -> AnimationState_Set_Custom_PropWrapMode
    "UnityEngine.AnimationState::get_time"  ,    // -> AnimationState_Get_Custom_PropTime
    "UnityEngine.AnimationState::set_time"  ,    // -> AnimationState_Set_Custom_PropTime
    "UnityEngine.AnimationState::get_normalizedTime",    // -> AnimationState_Get_Custom_PropNormalizedTime
    "UnityEngine.AnimationState::set_normalizedTime",    // -> AnimationState_Set_Custom_PropNormalizedTime
    "UnityEngine.AnimationState::get_speed" ,    // -> AnimationState_Get_Custom_PropSpeed
    "UnityEngine.AnimationState::set_speed" ,    // -> AnimationState_Set_Custom_PropSpeed
    "UnityEngine.AnimationState::get_normalizedSpeed",    // -> AnimationState_Get_Custom_PropNormalizedSpeed
    "UnityEngine.AnimationState::set_normalizedSpeed",    // -> AnimationState_Set_Custom_PropNormalizedSpeed
    "UnityEngine.AnimationState::get_length",    // -> AnimationState_Get_Custom_PropLength
    "UnityEngine.AnimationState::get_layer" ,    // -> AnimationState_Get_Custom_PropLayer
    "UnityEngine.AnimationState::set_layer" ,    // -> AnimationState_Set_Custom_PropLayer
    "UnityEngine.AnimationState::get_clip"  ,    // -> AnimationState_Get_Custom_PropClip
    "UnityEngine.AnimationState::AddMixingTransform",    // -> AnimationState_CUSTOM_AddMixingTransform
    "UnityEngine.AnimationState::RemoveMixingTransform",    // -> AnimationState_CUSTOM_RemoveMixingTransform
    "UnityEngine.AnimationState::get_name"  ,    // -> AnimationState_Get_Custom_PropName
    "UnityEngine.AnimationState::set_name"  ,    // -> AnimationState_Set_Custom_PropName
    "UnityEngine.AnimationState::get_blendMode",    // -> AnimationState_Get_Custom_PropBlendMode
    "UnityEngine.AnimationState::set_blendMode",    // -> AnimationState_Set_Custom_PropBlendMode
    NULL
};

static const void* s_Animations_IcallFuncs [] =
{
    (const void*)&AnimationEvent_CUSTOM_Create            ,  //  <- UnityEngine.AnimationEvent::Create
    (const void*)&AnimationEvent_CUSTOM_Destroy           ,  //  <- UnityEngine.AnimationEvent::Destroy
    (const void*)&AnimationEvent_Get_Custom_PropData      ,  //  <- UnityEngine.AnimationEvent::get_data
    (const void*)&AnimationEvent_Set_Custom_PropData      ,  //  <- UnityEngine.AnimationEvent::set_data
    (const void*)&AnimationEvent_Get_Custom_PropStringParameter,  //  <- UnityEngine.AnimationEvent::get_stringParameter
    (const void*)&AnimationEvent_Set_Custom_PropStringParameter,  //  <- UnityEngine.AnimationEvent::set_stringParameter
    (const void*)&AnimationEvent_Get_Custom_PropFloatParameter,  //  <- UnityEngine.AnimationEvent::get_floatParameter
    (const void*)&AnimationEvent_Set_Custom_PropFloatParameter,  //  <- UnityEngine.AnimationEvent::set_floatParameter
    (const void*)&AnimationEvent_Get_Custom_PropIntParameter,  //  <- UnityEngine.AnimationEvent::get_intParameter
    (const void*)&AnimationEvent_Set_Custom_PropIntParameter,  //  <- UnityEngine.AnimationEvent::set_intParameter
    (const void*)&AnimationEvent_Get_Custom_PropObjectReferenceParameter,  //  <- UnityEngine.AnimationEvent::get_objectReferenceParameter
    (const void*)&AnimationEvent_Set_Custom_PropObjectReferenceParameter,  //  <- UnityEngine.AnimationEvent::set_objectReferenceParameter
    (const void*)&AnimationEvent_Get_Custom_PropFunctionName,  //  <- UnityEngine.AnimationEvent::get_functionName
    (const void*)&AnimationEvent_Set_Custom_PropFunctionName,  //  <- UnityEngine.AnimationEvent::set_functionName
    (const void*)&AnimationEvent_Get_Custom_PropTime      ,  //  <- UnityEngine.AnimationEvent::get_time
    (const void*)&AnimationEvent_Set_Custom_PropTime      ,  //  <- UnityEngine.AnimationEvent::set_time
    (const void*)&AnimationEvent_Get_Custom_PropMessageOptions,  //  <- UnityEngine.AnimationEvent::get_messageOptions
    (const void*)&AnimationEvent_Set_Custom_PropMessageOptions,  //  <- UnityEngine.AnimationEvent::set_messageOptions
    (const void*)&AnimationEvent_Get_Custom_PropAnimationState,  //  <- UnityEngine.AnimationEvent::get_animationState
    (const void*)&AnimationClip_CUSTOM_Internal_CreateAnimationClip,  //  <- UnityEngine.AnimationClip::Internal_CreateAnimationClip
    (const void*)&AnimationClip_Get_Custom_PropLength     ,  //  <- UnityEngine.AnimationClip::get_length
    (const void*)&AnimationClip_Get_Custom_PropStartTime  ,  //  <- UnityEngine.AnimationClip::get_startTime
    (const void*)&AnimationClip_Get_Custom_PropStopTime   ,  //  <- UnityEngine.AnimationClip::get_stopTime
    (const void*)&AnimationClip_Get_Custom_PropFrameRate  ,  //  <- UnityEngine.AnimationClip::get_frameRate
    (const void*)&AnimationClip_CUSTOM_SetCurve           ,  //  <- UnityEngine.AnimationClip::SetCurve
    (const void*)&AnimationClip_CUSTOM_INTERNAL_CALL_EnsureQuaternionContinuity,  //  <- UnityEngine.AnimationClip::INTERNAL_CALL_EnsureQuaternionContinuity
    (const void*)&AnimationClip_CUSTOM_INTERNAL_CALL_ClearCurves,  //  <- UnityEngine.AnimationClip::INTERNAL_CALL_ClearCurves
    (const void*)&AnimationClip_Get_Custom_PropWrapMode   ,  //  <- UnityEngine.AnimationClip::get_wrapMode
    (const void*)&AnimationClip_Set_Custom_PropWrapMode   ,  //  <- UnityEngine.AnimationClip::set_wrapMode
    (const void*)&AnimationClip_CUSTOM_AddEvent           ,  //  <- UnityEngine.AnimationClip::AddEvent
    (const void*)&AnimationClip_CUSTOM_INTERNAL_get_localBounds,  //  <- UnityEngine.AnimationClip::INTERNAL_get_localBounds
    (const void*)&AnimationClip_CUSTOM_INTERNAL_set_localBounds,  //  <- UnityEngine.AnimationClip::INTERNAL_set_localBounds
    (const void*)&AnimationCurve_CUSTOM_Cleanup           ,  //  <- UnityEngine.AnimationCurve::Cleanup
    (const void*)&AnimationCurve_CUSTOM_Evaluate          ,  //  <- UnityEngine.AnimationCurve::Evaluate
    (const void*)&AnimationCurve_CUSTOM_AddKey            ,  //  <- UnityEngine.AnimationCurve::AddKey
    (const void*)&AnimationCurve_CUSTOM_INTERNAL_CALL_AddKey_Internal,  //  <- UnityEngine.AnimationCurve::INTERNAL_CALL_AddKey_Internal
    (const void*)&AnimationCurve_CUSTOM_INTERNAL_CALL_MoveKey,  //  <- UnityEngine.AnimationCurve::INTERNAL_CALL_MoveKey
    (const void*)&AnimationCurve_CUSTOM_RemoveKey         ,  //  <- UnityEngine.AnimationCurve::RemoveKey
    (const void*)&AnimationCurve_Get_Custom_PropLength    ,  //  <- UnityEngine.AnimationCurve::get_length
    (const void*)&AnimationCurve_CUSTOM_SetKeys           ,  //  <- UnityEngine.AnimationCurve::SetKeys
    (const void*)&AnimationCurve_CUSTOM_INTERNAL_CALL_GetKey_Internal,  //  <- UnityEngine.AnimationCurve::INTERNAL_CALL_GetKey_Internal
    (const void*)&AnimationCurve_CUSTOM_GetKeys           ,  //  <- UnityEngine.AnimationCurve::GetKeys
    (const void*)&AnimationCurve_CUSTOM_SmoothTangents    ,  //  <- UnityEngine.AnimationCurve::SmoothTangents
    (const void*)&AnimationCurve_Get_Custom_PropPreWrapMode,  //  <- UnityEngine.AnimationCurve::get_preWrapMode
    (const void*)&AnimationCurve_Set_Custom_PropPreWrapMode,  //  <- UnityEngine.AnimationCurve::set_preWrapMode
    (const void*)&AnimationCurve_Get_Custom_PropPostWrapMode,  //  <- UnityEngine.AnimationCurve::get_postWrapMode
    (const void*)&AnimationCurve_Set_Custom_PropPostWrapMode,  //  <- UnityEngine.AnimationCurve::set_postWrapMode
    (const void*)&AnimationCurve_CUSTOM_Init              ,  //  <- UnityEngine.AnimationCurve::Init
    (const void*)&Animation_Get_Custom_PropClip           ,  //  <- UnityEngine.Animation::get_clip
    (const void*)&Animation_Set_Custom_PropClip           ,  //  <- UnityEngine.Animation::set_clip
    (const void*)&Animation_Get_Custom_PropPlayAutomatically,  //  <- UnityEngine.Animation::get_playAutomatically
    (const void*)&Animation_Set_Custom_PropPlayAutomatically,  //  <- UnityEngine.Animation::set_playAutomatically
    (const void*)&Animation_Get_Custom_PropWrapMode       ,  //  <- UnityEngine.Animation::get_wrapMode
    (const void*)&Animation_Set_Custom_PropWrapMode       ,  //  <- UnityEngine.Animation::set_wrapMode
    (const void*)&Animation_CUSTOM_INTERNAL_CALL_Stop     ,  //  <- UnityEngine.Animation::INTERNAL_CALL_Stop
    (const void*)&Animation_CUSTOM_Internal_StopByName    ,  //  <- UnityEngine.Animation::Internal_StopByName
    (const void*)&Animation_CUSTOM_Internal_RewindByName  ,  //  <- UnityEngine.Animation::Internal_RewindByName
    (const void*)&Animation_CUSTOM_INTERNAL_CALL_Rewind   ,  //  <- UnityEngine.Animation::INTERNAL_CALL_Rewind
    (const void*)&Animation_CUSTOM_INTERNAL_CALL_Sample   ,  //  <- UnityEngine.Animation::INTERNAL_CALL_Sample
    (const void*)&Animation_Get_Custom_PropIsPlaying      ,  //  <- UnityEngine.Animation::get_isPlaying
    (const void*)&Animation_CUSTOM_IsPlaying              ,  //  <- UnityEngine.Animation::IsPlaying
    (const void*)&Animation_CUSTOM_Play                   ,  //  <- UnityEngine.Animation::Play
    (const void*)&Animation_CUSTOM_CrossFade              ,  //  <- UnityEngine.Animation::CrossFade
    (const void*)&Animation_CUSTOM_Blend                  ,  //  <- UnityEngine.Animation::Blend
    (const void*)&Animation_CUSTOM_CrossFadeQueued        ,  //  <- UnityEngine.Animation::CrossFadeQueued
    (const void*)&Animation_CUSTOM_PlayQueued             ,  //  <- UnityEngine.Animation::PlayQueued
    (const void*)&Animation_CUSTOM_AddClip                ,  //  <- UnityEngine.Animation::AddClip
    (const void*)&Animation_CUSTOM_RemoveClip             ,  //  <- UnityEngine.Animation::RemoveClip
    (const void*)&Animation_CUSTOM_GetClipCount           ,  //  <- UnityEngine.Animation::GetClipCount
    (const void*)&Animation_CUSTOM_RemoveClip2            ,  //  <- UnityEngine.Animation::RemoveClip2
    (const void*)&Animation_CUSTOM_PlayDefaultAnimation   ,  //  <- UnityEngine.Animation::PlayDefaultAnimation
    (const void*)&Animation_CUSTOM_INTERNAL_CALL_SyncLayer,  //  <- UnityEngine.Animation::INTERNAL_CALL_SyncLayer
    (const void*)&Animation_CUSTOM_GetState               ,  //  <- UnityEngine.Animation::GetState
    (const void*)&Animation_CUSTOM_GetStateAtIndex        ,  //  <- UnityEngine.Animation::GetStateAtIndex
    (const void*)&Animation_CUSTOM_GetStateCount          ,  //  <- UnityEngine.Animation::GetStateCount
    (const void*)&Animation_Get_Custom_PropAnimatePhysics ,  //  <- UnityEngine.Animation::get_animatePhysics
    (const void*)&Animation_Set_Custom_PropAnimatePhysics ,  //  <- UnityEngine.Animation::set_animatePhysics
    (const void*)&Animation_Get_Custom_PropAnimateOnlyIfVisible,  //  <- UnityEngine.Animation::get_animateOnlyIfVisible
    (const void*)&Animation_Set_Custom_PropAnimateOnlyIfVisible,  //  <- UnityEngine.Animation::set_animateOnlyIfVisible
    (const void*)&Animation_Get_Custom_PropCullingType    ,  //  <- UnityEngine.Animation::get_cullingType
    (const void*)&Animation_Set_Custom_PropCullingType    ,  //  <- UnityEngine.Animation::set_cullingType
    (const void*)&Animation_CUSTOM_INTERNAL_get_localBounds,  //  <- UnityEngine.Animation::INTERNAL_get_localBounds
    (const void*)&Animation_CUSTOM_INTERNAL_set_localBounds,  //  <- UnityEngine.Animation::INTERNAL_set_localBounds
    (const void*)&AnimationState_Get_Custom_PropEnabled   ,  //  <- UnityEngine.AnimationState::get_enabled
    (const void*)&AnimationState_Set_Custom_PropEnabled   ,  //  <- UnityEngine.AnimationState::set_enabled
    (const void*)&AnimationState_Get_Custom_PropWeight    ,  //  <- UnityEngine.AnimationState::get_weight
    (const void*)&AnimationState_Set_Custom_PropWeight    ,  //  <- UnityEngine.AnimationState::set_weight
    (const void*)&AnimationState_Get_Custom_PropWrapMode  ,  //  <- UnityEngine.AnimationState::get_wrapMode
    (const void*)&AnimationState_Set_Custom_PropWrapMode  ,  //  <- UnityEngine.AnimationState::set_wrapMode
    (const void*)&AnimationState_Get_Custom_PropTime      ,  //  <- UnityEngine.AnimationState::get_time
    (const void*)&AnimationState_Set_Custom_PropTime      ,  //  <- UnityEngine.AnimationState::set_time
    (const void*)&AnimationState_Get_Custom_PropNormalizedTime,  //  <- UnityEngine.AnimationState::get_normalizedTime
    (const void*)&AnimationState_Set_Custom_PropNormalizedTime,  //  <- UnityEngine.AnimationState::set_normalizedTime
    (const void*)&AnimationState_Get_Custom_PropSpeed     ,  //  <- UnityEngine.AnimationState::get_speed
    (const void*)&AnimationState_Set_Custom_PropSpeed     ,  //  <- UnityEngine.AnimationState::set_speed
    (const void*)&AnimationState_Get_Custom_PropNormalizedSpeed,  //  <- UnityEngine.AnimationState::get_normalizedSpeed
    (const void*)&AnimationState_Set_Custom_PropNormalizedSpeed,  //  <- UnityEngine.AnimationState::set_normalizedSpeed
    (const void*)&AnimationState_Get_Custom_PropLength    ,  //  <- UnityEngine.AnimationState::get_length
    (const void*)&AnimationState_Get_Custom_PropLayer     ,  //  <- UnityEngine.AnimationState::get_layer
    (const void*)&AnimationState_Set_Custom_PropLayer     ,  //  <- UnityEngine.AnimationState::set_layer
    (const void*)&AnimationState_Get_Custom_PropClip      ,  //  <- UnityEngine.AnimationState::get_clip
    (const void*)&AnimationState_CUSTOM_AddMixingTransform,  //  <- UnityEngine.AnimationState::AddMixingTransform
    (const void*)&AnimationState_CUSTOM_RemoveMixingTransform,  //  <- UnityEngine.AnimationState::RemoveMixingTransform
    (const void*)&AnimationState_Get_Custom_PropName      ,  //  <- UnityEngine.AnimationState::get_name
    (const void*)&AnimationState_Set_Custom_PropName      ,  //  <- UnityEngine.AnimationState::set_name
    (const void*)&AnimationState_Get_Custom_PropBlendMode ,  //  <- UnityEngine.AnimationState::get_blendMode
    (const void*)&AnimationState_Set_Custom_PropBlendMode ,  //  <- UnityEngine.AnimationState::set_blendMode
    NULL
};

void ExportAnimationsBindings();
void ExportAnimationsBindings()
{
    for (int i = 0; s_Animations_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Animations_IcallNames [i], s_Animations_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportAnimationsBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(AnimationEvent_CUSTOM_Create); //  <- UnityEngine.AnimationEvent::Create
    SET_METRO_BINDING(AnimationEvent_CUSTOM_Destroy); //  <- UnityEngine.AnimationEvent::Destroy
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropData); //  <- UnityEngine.AnimationEvent::get_data
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropData); //  <- UnityEngine.AnimationEvent::set_data
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropStringParameter); //  <- UnityEngine.AnimationEvent::get_stringParameter
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropStringParameter); //  <- UnityEngine.AnimationEvent::set_stringParameter
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropFloatParameter); //  <- UnityEngine.AnimationEvent::get_floatParameter
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropFloatParameter); //  <- UnityEngine.AnimationEvent::set_floatParameter
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropIntParameter); //  <- UnityEngine.AnimationEvent::get_intParameter
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropIntParameter); //  <- UnityEngine.AnimationEvent::set_intParameter
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropObjectReferenceParameter); //  <- UnityEngine.AnimationEvent::get_objectReferenceParameter
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropObjectReferenceParameter); //  <- UnityEngine.AnimationEvent::set_objectReferenceParameter
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropFunctionName); //  <- UnityEngine.AnimationEvent::get_functionName
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropFunctionName); //  <- UnityEngine.AnimationEvent::set_functionName
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropTime); //  <- UnityEngine.AnimationEvent::get_time
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropTime); //  <- UnityEngine.AnimationEvent::set_time
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropMessageOptions); //  <- UnityEngine.AnimationEvent::get_messageOptions
    SET_METRO_BINDING(AnimationEvent_Set_Custom_PropMessageOptions); //  <- UnityEngine.AnimationEvent::set_messageOptions
    SET_METRO_BINDING(AnimationEvent_Get_Custom_PropAnimationState); //  <- UnityEngine.AnimationEvent::get_animationState
    SET_METRO_BINDING(AnimationClip_CUSTOM_Internal_CreateAnimationClip); //  <- UnityEngine.AnimationClip::Internal_CreateAnimationClip
    SET_METRO_BINDING(AnimationClip_Get_Custom_PropLength); //  <- UnityEngine.AnimationClip::get_length
    SET_METRO_BINDING(AnimationClip_Get_Custom_PropStartTime); //  <- UnityEngine.AnimationClip::get_startTime
    SET_METRO_BINDING(AnimationClip_Get_Custom_PropStopTime); //  <- UnityEngine.AnimationClip::get_stopTime
    SET_METRO_BINDING(AnimationClip_Get_Custom_PropFrameRate); //  <- UnityEngine.AnimationClip::get_frameRate
    SET_METRO_BINDING(AnimationClip_CUSTOM_SetCurve); //  <- UnityEngine.AnimationClip::SetCurve
    SET_METRO_BINDING(AnimationClip_CUSTOM_INTERNAL_CALL_EnsureQuaternionContinuity); //  <- UnityEngine.AnimationClip::INTERNAL_CALL_EnsureQuaternionContinuity
    SET_METRO_BINDING(AnimationClip_CUSTOM_INTERNAL_CALL_ClearCurves); //  <- UnityEngine.AnimationClip::INTERNAL_CALL_ClearCurves
    SET_METRO_BINDING(AnimationClip_Get_Custom_PropWrapMode); //  <- UnityEngine.AnimationClip::get_wrapMode
    SET_METRO_BINDING(AnimationClip_Set_Custom_PropWrapMode); //  <- UnityEngine.AnimationClip::set_wrapMode
    SET_METRO_BINDING(AnimationClip_CUSTOM_AddEvent); //  <- UnityEngine.AnimationClip::AddEvent
    SET_METRO_BINDING(AnimationClip_CUSTOM_INTERNAL_get_localBounds); //  <- UnityEngine.AnimationClip::INTERNAL_get_localBounds
    SET_METRO_BINDING(AnimationClip_CUSTOM_INTERNAL_set_localBounds); //  <- UnityEngine.AnimationClip::INTERNAL_set_localBounds
    SET_METRO_BINDING(AnimationCurve_CUSTOM_Cleanup); //  <- UnityEngine.AnimationCurve::Cleanup
    SET_METRO_BINDING(AnimationCurve_CUSTOM_Evaluate); //  <- UnityEngine.AnimationCurve::Evaluate
    SET_METRO_BINDING(AnimationCurve_CUSTOM_AddKey); //  <- UnityEngine.AnimationCurve::AddKey
    SET_METRO_BINDING(AnimationCurve_CUSTOM_INTERNAL_CALL_AddKey_Internal); //  <- UnityEngine.AnimationCurve::INTERNAL_CALL_AddKey_Internal
    SET_METRO_BINDING(AnimationCurve_CUSTOM_INTERNAL_CALL_MoveKey); //  <- UnityEngine.AnimationCurve::INTERNAL_CALL_MoveKey
    SET_METRO_BINDING(AnimationCurve_CUSTOM_RemoveKey); //  <- UnityEngine.AnimationCurve::RemoveKey
    SET_METRO_BINDING(AnimationCurve_Get_Custom_PropLength); //  <- UnityEngine.AnimationCurve::get_length
    SET_METRO_BINDING(AnimationCurve_CUSTOM_SetKeys); //  <- UnityEngine.AnimationCurve::SetKeys
    SET_METRO_BINDING(AnimationCurve_CUSTOM_INTERNAL_CALL_GetKey_Internal); //  <- UnityEngine.AnimationCurve::INTERNAL_CALL_GetKey_Internal
    SET_METRO_BINDING(AnimationCurve_CUSTOM_GetKeys); //  <- UnityEngine.AnimationCurve::GetKeys
    SET_METRO_BINDING(AnimationCurve_CUSTOM_SmoothTangents); //  <- UnityEngine.AnimationCurve::SmoothTangents
    SET_METRO_BINDING(AnimationCurve_Get_Custom_PropPreWrapMode); //  <- UnityEngine.AnimationCurve::get_preWrapMode
    SET_METRO_BINDING(AnimationCurve_Set_Custom_PropPreWrapMode); //  <- UnityEngine.AnimationCurve::set_preWrapMode
    SET_METRO_BINDING(AnimationCurve_Get_Custom_PropPostWrapMode); //  <- UnityEngine.AnimationCurve::get_postWrapMode
    SET_METRO_BINDING(AnimationCurve_Set_Custom_PropPostWrapMode); //  <- UnityEngine.AnimationCurve::set_postWrapMode
    SET_METRO_BINDING(AnimationCurve_CUSTOM_Init); //  <- UnityEngine.AnimationCurve::Init
    SET_METRO_BINDING(Animation_Get_Custom_PropClip); //  <- UnityEngine.Animation::get_clip
    SET_METRO_BINDING(Animation_Set_Custom_PropClip); //  <- UnityEngine.Animation::set_clip
    SET_METRO_BINDING(Animation_Get_Custom_PropPlayAutomatically); //  <- UnityEngine.Animation::get_playAutomatically
    SET_METRO_BINDING(Animation_Set_Custom_PropPlayAutomatically); //  <- UnityEngine.Animation::set_playAutomatically
    SET_METRO_BINDING(Animation_Get_Custom_PropWrapMode); //  <- UnityEngine.Animation::get_wrapMode
    SET_METRO_BINDING(Animation_Set_Custom_PropWrapMode); //  <- UnityEngine.Animation::set_wrapMode
    SET_METRO_BINDING(Animation_CUSTOM_INTERNAL_CALL_Stop); //  <- UnityEngine.Animation::INTERNAL_CALL_Stop
    SET_METRO_BINDING(Animation_CUSTOM_Internal_StopByName); //  <- UnityEngine.Animation::Internal_StopByName
    SET_METRO_BINDING(Animation_CUSTOM_Internal_RewindByName); //  <- UnityEngine.Animation::Internal_RewindByName
    SET_METRO_BINDING(Animation_CUSTOM_INTERNAL_CALL_Rewind); //  <- UnityEngine.Animation::INTERNAL_CALL_Rewind
    SET_METRO_BINDING(Animation_CUSTOM_INTERNAL_CALL_Sample); //  <- UnityEngine.Animation::INTERNAL_CALL_Sample
    SET_METRO_BINDING(Animation_Get_Custom_PropIsPlaying); //  <- UnityEngine.Animation::get_isPlaying
    SET_METRO_BINDING(Animation_CUSTOM_IsPlaying); //  <- UnityEngine.Animation::IsPlaying
    SET_METRO_BINDING(Animation_CUSTOM_Play); //  <- UnityEngine.Animation::Play
    SET_METRO_BINDING(Animation_CUSTOM_CrossFade); //  <- UnityEngine.Animation::CrossFade
    SET_METRO_BINDING(Animation_CUSTOM_Blend); //  <- UnityEngine.Animation::Blend
    SET_METRO_BINDING(Animation_CUSTOM_CrossFadeQueued); //  <- UnityEngine.Animation::CrossFadeQueued
    SET_METRO_BINDING(Animation_CUSTOM_PlayQueued); //  <- UnityEngine.Animation::PlayQueued
    SET_METRO_BINDING(Animation_CUSTOM_AddClip); //  <- UnityEngine.Animation::AddClip
    SET_METRO_BINDING(Animation_CUSTOM_RemoveClip); //  <- UnityEngine.Animation::RemoveClip
    SET_METRO_BINDING(Animation_CUSTOM_GetClipCount); //  <- UnityEngine.Animation::GetClipCount
    SET_METRO_BINDING(Animation_CUSTOM_RemoveClip2); //  <- UnityEngine.Animation::RemoveClip2
    SET_METRO_BINDING(Animation_CUSTOM_PlayDefaultAnimation); //  <- UnityEngine.Animation::PlayDefaultAnimation
    SET_METRO_BINDING(Animation_CUSTOM_INTERNAL_CALL_SyncLayer); //  <- UnityEngine.Animation::INTERNAL_CALL_SyncLayer
    SET_METRO_BINDING(Animation_CUSTOM_GetState); //  <- UnityEngine.Animation::GetState
    SET_METRO_BINDING(Animation_CUSTOM_GetStateAtIndex); //  <- UnityEngine.Animation::GetStateAtIndex
    SET_METRO_BINDING(Animation_CUSTOM_GetStateCount); //  <- UnityEngine.Animation::GetStateCount
    SET_METRO_BINDING(Animation_Get_Custom_PropAnimatePhysics); //  <- UnityEngine.Animation::get_animatePhysics
    SET_METRO_BINDING(Animation_Set_Custom_PropAnimatePhysics); //  <- UnityEngine.Animation::set_animatePhysics
    SET_METRO_BINDING(Animation_Get_Custom_PropAnimateOnlyIfVisible); //  <- UnityEngine.Animation::get_animateOnlyIfVisible
    SET_METRO_BINDING(Animation_Set_Custom_PropAnimateOnlyIfVisible); //  <- UnityEngine.Animation::set_animateOnlyIfVisible
    SET_METRO_BINDING(Animation_Get_Custom_PropCullingType); //  <- UnityEngine.Animation::get_cullingType
    SET_METRO_BINDING(Animation_Set_Custom_PropCullingType); //  <- UnityEngine.Animation::set_cullingType
    SET_METRO_BINDING(Animation_CUSTOM_INTERNAL_get_localBounds); //  <- UnityEngine.Animation::INTERNAL_get_localBounds
    SET_METRO_BINDING(Animation_CUSTOM_INTERNAL_set_localBounds); //  <- UnityEngine.Animation::INTERNAL_set_localBounds
    SET_METRO_BINDING(AnimationState_Get_Custom_PropEnabled); //  <- UnityEngine.AnimationState::get_enabled
    SET_METRO_BINDING(AnimationState_Set_Custom_PropEnabled); //  <- UnityEngine.AnimationState::set_enabled
    SET_METRO_BINDING(AnimationState_Get_Custom_PropWeight); //  <- UnityEngine.AnimationState::get_weight
    SET_METRO_BINDING(AnimationState_Set_Custom_PropWeight); //  <- UnityEngine.AnimationState::set_weight
    SET_METRO_BINDING(AnimationState_Get_Custom_PropWrapMode); //  <- UnityEngine.AnimationState::get_wrapMode
    SET_METRO_BINDING(AnimationState_Set_Custom_PropWrapMode); //  <- UnityEngine.AnimationState::set_wrapMode
    SET_METRO_BINDING(AnimationState_Get_Custom_PropTime); //  <- UnityEngine.AnimationState::get_time
    SET_METRO_BINDING(AnimationState_Set_Custom_PropTime); //  <- UnityEngine.AnimationState::set_time
    SET_METRO_BINDING(AnimationState_Get_Custom_PropNormalizedTime); //  <- UnityEngine.AnimationState::get_normalizedTime
    SET_METRO_BINDING(AnimationState_Set_Custom_PropNormalizedTime); //  <- UnityEngine.AnimationState::set_normalizedTime
    SET_METRO_BINDING(AnimationState_Get_Custom_PropSpeed); //  <- UnityEngine.AnimationState::get_speed
    SET_METRO_BINDING(AnimationState_Set_Custom_PropSpeed); //  <- UnityEngine.AnimationState::set_speed
    SET_METRO_BINDING(AnimationState_Get_Custom_PropNormalizedSpeed); //  <- UnityEngine.AnimationState::get_normalizedSpeed
    SET_METRO_BINDING(AnimationState_Set_Custom_PropNormalizedSpeed); //  <- UnityEngine.AnimationState::set_normalizedSpeed
    SET_METRO_BINDING(AnimationState_Get_Custom_PropLength); //  <- UnityEngine.AnimationState::get_length
    SET_METRO_BINDING(AnimationState_Get_Custom_PropLayer); //  <- UnityEngine.AnimationState::get_layer
    SET_METRO_BINDING(AnimationState_Set_Custom_PropLayer); //  <- UnityEngine.AnimationState::set_layer
    SET_METRO_BINDING(AnimationState_Get_Custom_PropClip); //  <- UnityEngine.AnimationState::get_clip
    SET_METRO_BINDING(AnimationState_CUSTOM_AddMixingTransform); //  <- UnityEngine.AnimationState::AddMixingTransform
    SET_METRO_BINDING(AnimationState_CUSTOM_RemoveMixingTransform); //  <- UnityEngine.AnimationState::RemoveMixingTransform
    SET_METRO_BINDING(AnimationState_Get_Custom_PropName); //  <- UnityEngine.AnimationState::get_name
    SET_METRO_BINDING(AnimationState_Set_Custom_PropName); //  <- UnityEngine.AnimationState::set_name
    SET_METRO_BINDING(AnimationState_Get_Custom_PropBlendMode); //  <- UnityEngine.AnimationState::get_blendMode
    SET_METRO_BINDING(AnimationState_Set_Custom_PropBlendMode); //  <- UnityEngine.AnimationState::set_blendMode
}

#endif
