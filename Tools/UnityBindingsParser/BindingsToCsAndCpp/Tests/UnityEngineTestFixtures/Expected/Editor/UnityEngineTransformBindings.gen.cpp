#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"

using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_position(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_position)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_position)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_position)
    *returnValue = self->GetPosition();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_set_position(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_set_position)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_position)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_position)
    
    self->SetPosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_localPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_localPosition)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localPosition)
    *returnValue = self->GetLocalPosition();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_set_localPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_set_localPosition)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localPosition)
    
    self->SetLocalPosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_localEulerAngles(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_localEulerAngles)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localEulerAngles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localEulerAngles)
    *returnValue = self->GetLocalEulerAngles();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_set_localEulerAngles(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_set_localEulerAngles)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localEulerAngles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localEulerAngles)
    
    self->SetLocalEulerAngles (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_rotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_rotation)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rotation)
    *returnValue = self->GetRotation();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_set_rotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_set_rotation)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_rotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_rotation)
    
    self->SetRotationSafe (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_localRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_localRotation)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localRotation)
    *returnValue = self->GetLocalRotation();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_set_localRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_set_localRotation)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localRotation)
    
    self->SetLocalRotationSafe (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_localScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_localScale)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localScale)
    *returnValue = self->GetLocalScale();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_set_localScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_set_localScale)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localScale)
    
    self->SetLocalScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Transform_Get_Custom_PropParent(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_Get_Custom_PropParent)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_parent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_parent)
    return Scripting::ScriptingWrapperFor(self->GetParent());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_Set_Custom_PropParent(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_Set_Custom_PropParent)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_parent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_parent)
    
    self->SetParent (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_worldToLocalMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_worldToLocalMatrix)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_worldToLocalMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_worldToLocalMatrix)
    *returnValue = self->GetWorldToLocalMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_localToWorldMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_localToWorldMatrix)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localToWorldMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localToWorldMatrix)
    *returnValue = self->GetLocalToWorldMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_RotateAroundInternal(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& axis, float angle)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_RotateAroundInternal)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RotateAroundInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_RotateAroundInternal)
     self->RotateAroundSafe (axis, angle); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_LookAt(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& worldPosition, const Vector3f& worldUp)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_LookAt)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_LookAt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_LookAt)
    
    		Vector3f forward = worldPosition - self->GetPosition ();
    		Quaternionf q = Quaternionf::identity ();
    		if (LookRotationToQuaternion (forward, worldUp, &q))
    			self->SetRotationSafe (q);
    		else	
    		{
    			float mag = Magnitude (forward);
    			if (mag > Vector3f::epsilon)
    			{
    				Matrix3x3f m;
    				m.SetFromToRotation (Vector3f::zAxis, forward / mag);
    				MatrixToQuaternion (m, q);
    				self->SetRotationSafe (q);
    			}
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_TransformDirection(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& direction, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_TransformDirection)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TransformDirection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_TransformDirection)
    returnValue = self->TransformDirection(direction);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_InverseTransformDirection(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& direction, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_InverseTransformDirection)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_InverseTransformDirection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_InverseTransformDirection)
    returnValue = self->InverseTransformDirection(direction);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_TransformPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_TransformPoint)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TransformPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_TransformPoint)
    returnValue = self->TransformPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_InverseTransformPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_InverseTransformPoint)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_InverseTransformPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_InverseTransformPoint)
    returnValue = self->InverseTransformPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Transform_Get_Custom_PropRoot(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_Get_Custom_PropRoot)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_root)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_root)
    return Scripting::ScriptingWrapperFor(&self->GetRoot());
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Transform_Get_Custom_PropChildCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_Get_Custom_PropChildCount)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_childCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_childCount)
    return self->GetChildrenCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_DetachChildren(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_DetachChildren)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(DetachChildren)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DetachChildren)
    
    		Transform& transform = *self;
    		while (transform.begin () != transform.end ())
    			(**transform.begin ()).SetParent (NULL);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Transform_CUSTOM_Find(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_Find)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Find)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Find)
    
    		return Scripting::ScriptingWrapperFor (FindRelativeTransformWithPath (*self, name.ToUTF8().c_str ()));
    	
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_SendTransformChangedScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_SendTransformChangedScale)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SendTransformChangedScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SendTransformChangedScale)
    
    		self->SendTransformChanged ( Transform::kScaleChanged );
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_get_lossyScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_get_lossyScale)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_lossyScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_lossyScale)
    *returnValue = self->GetWorldScaleLossy();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Transform_CUSTOM_IsChildOf(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument parent_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_IsChildOf)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> parent(parent_);
    UNUSED(parent);
    SCRIPTINGAPI_STACK_CHECK(IsChildOf)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsChildOf)
    
    		return IsChildOrSameTransform(*self, *parent); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Transform_Get_Custom_PropHasChanged(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_Get_Custom_PropHasChanged)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hasChanged)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hasChanged)
    return self->GetChangedFlag();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_Set_Custom_PropHasChanged(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_Set_Custom_PropHasChanged)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_hasChanged)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hasChanged)
     self->SetChangedFlag(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_RotateAround(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& axis, float angle)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_RotateAround)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RotateAround)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_RotateAround)
     self->RotateAroundSafe (axis, angle); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Transform_CUSTOM_INTERNAL_CALL_RotateAroundLocal(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& axis, float angle)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_INTERNAL_CALL_RotateAroundLocal)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RotateAroundLocal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_RotateAroundLocal)
     self->RotateAroundLocalSafe (axis, angle); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Transform_CUSTOM_GetChild(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_GetChild)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetChild)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetChild)
    
    		Transform& transform = *self;
    		if (index >= 0 && index < transform.GetChildrenCount ())
    			return Scripting::ScriptingWrapperFor (&transform.GetChild (index));
    		else
    		{
    			RaiseMonoException ("Transform child out of bounds");
    			return SCRIPTING_NULL;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Transform_CUSTOM_GetChildCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_GetChildCount)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetChildCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetChildCount)
     return self->GetChildrenCount (); 
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Transform_CUSTOM_IsNonUniformScaleTransform(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Transform_CUSTOM_IsNonUniformScaleTransform)
    ReadOnlyScriptingObjectOfType<Transform> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(IsNonUniformScaleTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsNonUniformScaleTransform)
    
    		Transform& transform = *self;
    		Matrix4x4f temp;
    		TransformType scaleType = transform.CalculateTransformMatrix (temp);
    		return IsNonUniformScaleTransform (scaleType);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Transform_INTERNAL_get_position()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_position" , (gpointer)& Transform_CUSTOM_INTERNAL_get_position );
}

void Register_UnityEngine_Transform_INTERNAL_set_position()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_set_position" , (gpointer)& Transform_CUSTOM_INTERNAL_set_position );
}

void Register_UnityEngine_Transform_INTERNAL_get_localPosition()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_localPosition" , (gpointer)& Transform_CUSTOM_INTERNAL_get_localPosition );
}

void Register_UnityEngine_Transform_INTERNAL_set_localPosition()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_set_localPosition" , (gpointer)& Transform_CUSTOM_INTERNAL_set_localPosition );
}

void Register_UnityEngine_Transform_INTERNAL_get_localEulerAngles()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_localEulerAngles" , (gpointer)& Transform_CUSTOM_INTERNAL_get_localEulerAngles );
}

void Register_UnityEngine_Transform_INTERNAL_set_localEulerAngles()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_set_localEulerAngles" , (gpointer)& Transform_CUSTOM_INTERNAL_set_localEulerAngles );
}

void Register_UnityEngine_Transform_INTERNAL_get_rotation()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_rotation" , (gpointer)& Transform_CUSTOM_INTERNAL_get_rotation );
}

void Register_UnityEngine_Transform_INTERNAL_set_rotation()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_set_rotation" , (gpointer)& Transform_CUSTOM_INTERNAL_set_rotation );
}

void Register_UnityEngine_Transform_INTERNAL_get_localRotation()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_localRotation" , (gpointer)& Transform_CUSTOM_INTERNAL_get_localRotation );
}

void Register_UnityEngine_Transform_INTERNAL_set_localRotation()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_set_localRotation" , (gpointer)& Transform_CUSTOM_INTERNAL_set_localRotation );
}

void Register_UnityEngine_Transform_INTERNAL_get_localScale()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_localScale" , (gpointer)& Transform_CUSTOM_INTERNAL_get_localScale );
}

void Register_UnityEngine_Transform_INTERNAL_set_localScale()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_set_localScale" , (gpointer)& Transform_CUSTOM_INTERNAL_set_localScale );
}

void Register_UnityEngine_Transform_get_parent()
{
    scripting_add_internal_call( "UnityEngine.Transform::get_parent" , (gpointer)& Transform_Get_Custom_PropParent );
}

void Register_UnityEngine_Transform_set_parent()
{
    scripting_add_internal_call( "UnityEngine.Transform::set_parent" , (gpointer)& Transform_Set_Custom_PropParent );
}

void Register_UnityEngine_Transform_INTERNAL_get_worldToLocalMatrix()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_worldToLocalMatrix" , (gpointer)& Transform_CUSTOM_INTERNAL_get_worldToLocalMatrix );
}

void Register_UnityEngine_Transform_INTERNAL_get_localToWorldMatrix()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_localToWorldMatrix" , (gpointer)& Transform_CUSTOM_INTERNAL_get_localToWorldMatrix );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_RotateAroundInternal()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_RotateAroundInternal" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_RotateAroundInternal );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_LookAt()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_LookAt" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_LookAt );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_TransformDirection()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_TransformDirection" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_TransformDirection );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_InverseTransformDirection()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_InverseTransformDirection" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_InverseTransformDirection );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_TransformPoint()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_TransformPoint" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_TransformPoint );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_InverseTransformPoint()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_InverseTransformPoint" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_InverseTransformPoint );
}

void Register_UnityEngine_Transform_get_root()
{
    scripting_add_internal_call( "UnityEngine.Transform::get_root" , (gpointer)& Transform_Get_Custom_PropRoot );
}

void Register_UnityEngine_Transform_get_childCount()
{
    scripting_add_internal_call( "UnityEngine.Transform::get_childCount" , (gpointer)& Transform_Get_Custom_PropChildCount );
}

void Register_UnityEngine_Transform_DetachChildren()
{
    scripting_add_internal_call( "UnityEngine.Transform::DetachChildren" , (gpointer)& Transform_CUSTOM_DetachChildren );
}

void Register_UnityEngine_Transform_Find()
{
    scripting_add_internal_call( "UnityEngine.Transform::Find" , (gpointer)& Transform_CUSTOM_Find );
}

#if UNITY_EDITOR
void Register_UnityEngine_Transform_SendTransformChangedScale()
{
    scripting_add_internal_call( "UnityEngine.Transform::SendTransformChangedScale" , (gpointer)& Transform_CUSTOM_SendTransformChangedScale );
}

#endif
void Register_UnityEngine_Transform_INTERNAL_get_lossyScale()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_get_lossyScale" , (gpointer)& Transform_CUSTOM_INTERNAL_get_lossyScale );
}

void Register_UnityEngine_Transform_IsChildOf()
{
    scripting_add_internal_call( "UnityEngine.Transform::IsChildOf" , (gpointer)& Transform_CUSTOM_IsChildOf );
}

void Register_UnityEngine_Transform_get_hasChanged()
{
    scripting_add_internal_call( "UnityEngine.Transform::get_hasChanged" , (gpointer)& Transform_Get_Custom_PropHasChanged );
}

void Register_UnityEngine_Transform_set_hasChanged()
{
    scripting_add_internal_call( "UnityEngine.Transform::set_hasChanged" , (gpointer)& Transform_Set_Custom_PropHasChanged );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_RotateAround()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_RotateAround" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_RotateAround );
}

void Register_UnityEngine_Transform_INTERNAL_CALL_RotateAroundLocal()
{
    scripting_add_internal_call( "UnityEngine.Transform::INTERNAL_CALL_RotateAroundLocal" , (gpointer)& Transform_CUSTOM_INTERNAL_CALL_RotateAroundLocal );
}

void Register_UnityEngine_Transform_GetChild()
{
    scripting_add_internal_call( "UnityEngine.Transform::GetChild" , (gpointer)& Transform_CUSTOM_GetChild );
}

void Register_UnityEngine_Transform_GetChildCount()
{
    scripting_add_internal_call( "UnityEngine.Transform::GetChildCount" , (gpointer)& Transform_CUSTOM_GetChildCount );
}

#if UNITY_EDITOR
void Register_UnityEngine_Transform_IsNonUniformScaleTransform()
{
    scripting_add_internal_call( "UnityEngine.Transform::IsNonUniformScaleTransform" , (gpointer)& Transform_CUSTOM_IsNonUniformScaleTransform );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineTransform_IcallNames [] =
{
    "UnityEngine.Transform::INTERNAL_get_position",    // -> Transform_CUSTOM_INTERNAL_get_position
    "UnityEngine.Transform::INTERNAL_set_position",    // -> Transform_CUSTOM_INTERNAL_set_position
    "UnityEngine.Transform::INTERNAL_get_localPosition",    // -> Transform_CUSTOM_INTERNAL_get_localPosition
    "UnityEngine.Transform::INTERNAL_set_localPosition",    // -> Transform_CUSTOM_INTERNAL_set_localPosition
    "UnityEngine.Transform::INTERNAL_get_localEulerAngles",    // -> Transform_CUSTOM_INTERNAL_get_localEulerAngles
    "UnityEngine.Transform::INTERNAL_set_localEulerAngles",    // -> Transform_CUSTOM_INTERNAL_set_localEulerAngles
    "UnityEngine.Transform::INTERNAL_get_rotation",    // -> Transform_CUSTOM_INTERNAL_get_rotation
    "UnityEngine.Transform::INTERNAL_set_rotation",    // -> Transform_CUSTOM_INTERNAL_set_rotation
    "UnityEngine.Transform::INTERNAL_get_localRotation",    // -> Transform_CUSTOM_INTERNAL_get_localRotation
    "UnityEngine.Transform::INTERNAL_set_localRotation",    // -> Transform_CUSTOM_INTERNAL_set_localRotation
    "UnityEngine.Transform::INTERNAL_get_localScale",    // -> Transform_CUSTOM_INTERNAL_get_localScale
    "UnityEngine.Transform::INTERNAL_set_localScale",    // -> Transform_CUSTOM_INTERNAL_set_localScale
    "UnityEngine.Transform::get_parent"     ,    // -> Transform_Get_Custom_PropParent
    "UnityEngine.Transform::set_parent"     ,    // -> Transform_Set_Custom_PropParent
    "UnityEngine.Transform::INTERNAL_get_worldToLocalMatrix",    // -> Transform_CUSTOM_INTERNAL_get_worldToLocalMatrix
    "UnityEngine.Transform::INTERNAL_get_localToWorldMatrix",    // -> Transform_CUSTOM_INTERNAL_get_localToWorldMatrix
    "UnityEngine.Transform::INTERNAL_CALL_RotateAroundInternal",    // -> Transform_CUSTOM_INTERNAL_CALL_RotateAroundInternal
    "UnityEngine.Transform::INTERNAL_CALL_LookAt",    // -> Transform_CUSTOM_INTERNAL_CALL_LookAt
    "UnityEngine.Transform::INTERNAL_CALL_TransformDirection",    // -> Transform_CUSTOM_INTERNAL_CALL_TransformDirection
    "UnityEngine.Transform::INTERNAL_CALL_InverseTransformDirection",    // -> Transform_CUSTOM_INTERNAL_CALL_InverseTransformDirection
    "UnityEngine.Transform::INTERNAL_CALL_TransformPoint",    // -> Transform_CUSTOM_INTERNAL_CALL_TransformPoint
    "UnityEngine.Transform::INTERNAL_CALL_InverseTransformPoint",    // -> Transform_CUSTOM_INTERNAL_CALL_InverseTransformPoint
    "UnityEngine.Transform::get_root"       ,    // -> Transform_Get_Custom_PropRoot
    "UnityEngine.Transform::get_childCount" ,    // -> Transform_Get_Custom_PropChildCount
    "UnityEngine.Transform::DetachChildren" ,    // -> Transform_CUSTOM_DetachChildren
    "UnityEngine.Transform::Find"           ,    // -> Transform_CUSTOM_Find
#if UNITY_EDITOR
    "UnityEngine.Transform::SendTransformChangedScale",    // -> Transform_CUSTOM_SendTransformChangedScale
#endif
    "UnityEngine.Transform::INTERNAL_get_lossyScale",    // -> Transform_CUSTOM_INTERNAL_get_lossyScale
    "UnityEngine.Transform::IsChildOf"      ,    // -> Transform_CUSTOM_IsChildOf
    "UnityEngine.Transform::get_hasChanged" ,    // -> Transform_Get_Custom_PropHasChanged
    "UnityEngine.Transform::set_hasChanged" ,    // -> Transform_Set_Custom_PropHasChanged
    "UnityEngine.Transform::INTERNAL_CALL_RotateAround",    // -> Transform_CUSTOM_INTERNAL_CALL_RotateAround
    "UnityEngine.Transform::INTERNAL_CALL_RotateAroundLocal",    // -> Transform_CUSTOM_INTERNAL_CALL_RotateAroundLocal
    "UnityEngine.Transform::GetChild"       ,    // -> Transform_CUSTOM_GetChild
    "UnityEngine.Transform::GetChildCount"  ,    // -> Transform_CUSTOM_GetChildCount
#if UNITY_EDITOR
    "UnityEngine.Transform::IsNonUniformScaleTransform",    // -> Transform_CUSTOM_IsNonUniformScaleTransform
#endif
    NULL
};

static const void* s_UnityEngineTransform_IcallFuncs [] =
{
    (const void*)&Transform_CUSTOM_INTERNAL_get_position  ,  //  <- UnityEngine.Transform::INTERNAL_get_position
    (const void*)&Transform_CUSTOM_INTERNAL_set_position  ,  //  <- UnityEngine.Transform::INTERNAL_set_position
    (const void*)&Transform_CUSTOM_INTERNAL_get_localPosition,  //  <- UnityEngine.Transform::INTERNAL_get_localPosition
    (const void*)&Transform_CUSTOM_INTERNAL_set_localPosition,  //  <- UnityEngine.Transform::INTERNAL_set_localPosition
    (const void*)&Transform_CUSTOM_INTERNAL_get_localEulerAngles,  //  <- UnityEngine.Transform::INTERNAL_get_localEulerAngles
    (const void*)&Transform_CUSTOM_INTERNAL_set_localEulerAngles,  //  <- UnityEngine.Transform::INTERNAL_set_localEulerAngles
    (const void*)&Transform_CUSTOM_INTERNAL_get_rotation  ,  //  <- UnityEngine.Transform::INTERNAL_get_rotation
    (const void*)&Transform_CUSTOM_INTERNAL_set_rotation  ,  //  <- UnityEngine.Transform::INTERNAL_set_rotation
    (const void*)&Transform_CUSTOM_INTERNAL_get_localRotation,  //  <- UnityEngine.Transform::INTERNAL_get_localRotation
    (const void*)&Transform_CUSTOM_INTERNAL_set_localRotation,  //  <- UnityEngine.Transform::INTERNAL_set_localRotation
    (const void*)&Transform_CUSTOM_INTERNAL_get_localScale,  //  <- UnityEngine.Transform::INTERNAL_get_localScale
    (const void*)&Transform_CUSTOM_INTERNAL_set_localScale,  //  <- UnityEngine.Transform::INTERNAL_set_localScale
    (const void*)&Transform_Get_Custom_PropParent         ,  //  <- UnityEngine.Transform::get_parent
    (const void*)&Transform_Set_Custom_PropParent         ,  //  <- UnityEngine.Transform::set_parent
    (const void*)&Transform_CUSTOM_INTERNAL_get_worldToLocalMatrix,  //  <- UnityEngine.Transform::INTERNAL_get_worldToLocalMatrix
    (const void*)&Transform_CUSTOM_INTERNAL_get_localToWorldMatrix,  //  <- UnityEngine.Transform::INTERNAL_get_localToWorldMatrix
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_RotateAroundInternal,  //  <- UnityEngine.Transform::INTERNAL_CALL_RotateAroundInternal
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_LookAt   ,  //  <- UnityEngine.Transform::INTERNAL_CALL_LookAt
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_TransformDirection,  //  <- UnityEngine.Transform::INTERNAL_CALL_TransformDirection
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_InverseTransformDirection,  //  <- UnityEngine.Transform::INTERNAL_CALL_InverseTransformDirection
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_TransformPoint,  //  <- UnityEngine.Transform::INTERNAL_CALL_TransformPoint
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_InverseTransformPoint,  //  <- UnityEngine.Transform::INTERNAL_CALL_InverseTransformPoint
    (const void*)&Transform_Get_Custom_PropRoot           ,  //  <- UnityEngine.Transform::get_root
    (const void*)&Transform_Get_Custom_PropChildCount     ,  //  <- UnityEngine.Transform::get_childCount
    (const void*)&Transform_CUSTOM_DetachChildren         ,  //  <- UnityEngine.Transform::DetachChildren
    (const void*)&Transform_CUSTOM_Find                   ,  //  <- UnityEngine.Transform::Find
#if UNITY_EDITOR
    (const void*)&Transform_CUSTOM_SendTransformChangedScale,  //  <- UnityEngine.Transform::SendTransformChangedScale
#endif
    (const void*)&Transform_CUSTOM_INTERNAL_get_lossyScale,  //  <- UnityEngine.Transform::INTERNAL_get_lossyScale
    (const void*)&Transform_CUSTOM_IsChildOf              ,  //  <- UnityEngine.Transform::IsChildOf
    (const void*)&Transform_Get_Custom_PropHasChanged     ,  //  <- UnityEngine.Transform::get_hasChanged
    (const void*)&Transform_Set_Custom_PropHasChanged     ,  //  <- UnityEngine.Transform::set_hasChanged
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_RotateAround,  //  <- UnityEngine.Transform::INTERNAL_CALL_RotateAround
    (const void*)&Transform_CUSTOM_INTERNAL_CALL_RotateAroundLocal,  //  <- UnityEngine.Transform::INTERNAL_CALL_RotateAroundLocal
    (const void*)&Transform_CUSTOM_GetChild               ,  //  <- UnityEngine.Transform::GetChild
    (const void*)&Transform_CUSTOM_GetChildCount          ,  //  <- UnityEngine.Transform::GetChildCount
#if UNITY_EDITOR
    (const void*)&Transform_CUSTOM_IsNonUniformScaleTransform,  //  <- UnityEngine.Transform::IsNonUniformScaleTransform
#endif
    NULL
};

void ExportUnityEngineTransformBindings();
void ExportUnityEngineTransformBindings()
{
    for (int i = 0; s_UnityEngineTransform_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineTransform_IcallNames [i], s_UnityEngineTransform_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineTransformBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_position); //  <- UnityEngine.Transform::INTERNAL_get_position
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_set_position); //  <- UnityEngine.Transform::INTERNAL_set_position
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_localPosition); //  <- UnityEngine.Transform::INTERNAL_get_localPosition
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_set_localPosition); //  <- UnityEngine.Transform::INTERNAL_set_localPosition
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_localEulerAngles); //  <- UnityEngine.Transform::INTERNAL_get_localEulerAngles
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_set_localEulerAngles); //  <- UnityEngine.Transform::INTERNAL_set_localEulerAngles
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_rotation); //  <- UnityEngine.Transform::INTERNAL_get_rotation
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_set_rotation); //  <- UnityEngine.Transform::INTERNAL_set_rotation
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_localRotation); //  <- UnityEngine.Transform::INTERNAL_get_localRotation
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_set_localRotation); //  <- UnityEngine.Transform::INTERNAL_set_localRotation
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_localScale); //  <- UnityEngine.Transform::INTERNAL_get_localScale
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_set_localScale); //  <- UnityEngine.Transform::INTERNAL_set_localScale
    SET_METRO_BINDING(Transform_Get_Custom_PropParent); //  <- UnityEngine.Transform::get_parent
    SET_METRO_BINDING(Transform_Set_Custom_PropParent); //  <- UnityEngine.Transform::set_parent
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_worldToLocalMatrix); //  <- UnityEngine.Transform::INTERNAL_get_worldToLocalMatrix
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_localToWorldMatrix); //  <- UnityEngine.Transform::INTERNAL_get_localToWorldMatrix
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_RotateAroundInternal); //  <- UnityEngine.Transform::INTERNAL_CALL_RotateAroundInternal
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_LookAt); //  <- UnityEngine.Transform::INTERNAL_CALL_LookAt
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_TransformDirection); //  <- UnityEngine.Transform::INTERNAL_CALL_TransformDirection
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_InverseTransformDirection); //  <- UnityEngine.Transform::INTERNAL_CALL_InverseTransformDirection
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_TransformPoint); //  <- UnityEngine.Transform::INTERNAL_CALL_TransformPoint
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_InverseTransformPoint); //  <- UnityEngine.Transform::INTERNAL_CALL_InverseTransformPoint
    SET_METRO_BINDING(Transform_Get_Custom_PropRoot); //  <- UnityEngine.Transform::get_root
    SET_METRO_BINDING(Transform_Get_Custom_PropChildCount); //  <- UnityEngine.Transform::get_childCount
    SET_METRO_BINDING(Transform_CUSTOM_DetachChildren); //  <- UnityEngine.Transform::DetachChildren
    SET_METRO_BINDING(Transform_CUSTOM_Find); //  <- UnityEngine.Transform::Find
#if UNITY_EDITOR
    SET_METRO_BINDING(Transform_CUSTOM_SendTransformChangedScale); //  <- UnityEngine.Transform::SendTransformChangedScale
#endif
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_get_lossyScale); //  <- UnityEngine.Transform::INTERNAL_get_lossyScale
    SET_METRO_BINDING(Transform_CUSTOM_IsChildOf); //  <- UnityEngine.Transform::IsChildOf
    SET_METRO_BINDING(Transform_Get_Custom_PropHasChanged); //  <- UnityEngine.Transform::get_hasChanged
    SET_METRO_BINDING(Transform_Set_Custom_PropHasChanged); //  <- UnityEngine.Transform::set_hasChanged
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_RotateAround); //  <- UnityEngine.Transform::INTERNAL_CALL_RotateAround
    SET_METRO_BINDING(Transform_CUSTOM_INTERNAL_CALL_RotateAroundLocal); //  <- UnityEngine.Transform::INTERNAL_CALL_RotateAroundLocal
    SET_METRO_BINDING(Transform_CUSTOM_GetChild); //  <- UnityEngine.Transform::GetChild
    SET_METRO_BINDING(Transform_CUSTOM_GetChildCount); //  <- UnityEngine.Transform::GetChildCount
#if UNITY_EDITOR
    SET_METRO_BINDING(Transform_CUSTOM_IsNonUniformScaleTransform); //  <- UnityEngine.Transform::IsNonUniformScaleTransform
#endif
}

#endif
