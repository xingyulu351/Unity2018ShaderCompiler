#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Mono/MonoScript.h"
#include "Runtime/Animation/Animator.h"
#include "Runtime/Animation/RuntimeAnimatorController.h"
#include "Runtime/Animation/Avatar.h"
#include "Runtime/mecanim/human/human.h"
#include "Runtime/mecanim/generic/crc32.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoUtility.h"

using namespace Unity;



struct MonoAnimationInfo
{
	ScriptingObjectPtr clip;
	float weight;
};

void AnimationInfoToMono ( const AnimationInfo& src, MonoAnimationInfo &dest)
{
	dest.clip = Scripting::ScriptingWrapperFor (src.clip);
	dest.weight = src.weight;
}

 enum HumanBodyBones { Hips=0, LeftUpperLeg, RightUpperLeg,	LeftLowerLeg,	RightLowerLeg,	LeftFoot,	RightFoot,	Spine,	Chest,	Neck,	Head,	LeftShoulder,	RightShoulder,	LeftUpperArm,	RightUpperArm,	LeftLowerArm,	RightLowerArm,	LeftHand,	RightHand,	LeftToes,	RightToes,	LeftEye,	RightEye,	Jaw,	LastBone};

					SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropIsHuman(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropIsHuman)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isHuman)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isHuman)
    return self->IsHuman();
}


SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropHumanScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropHumanScale)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_humanScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_humanScale)
    return self->GetHumanScale();
}


	
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_deltaPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_deltaPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_deltaPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_deltaPosition)
    { *returnValue =(self->GetDeltaPosition()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_deltaRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_deltaRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_deltaRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_deltaRotation)
    { *returnValue =(self->GetDeltaRotation()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_rootPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_rootPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rootPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rootPosition)
    { *returnValue =(self->GetAvatarPosition()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_set_rootPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_set_rootPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_rootPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_rootPosition)
     self->SetAvatarPosition(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_rootRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_rootRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rootRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rootRotation)
    { *returnValue =(self->GetAvatarRotation()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_set_rootRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_set_rootRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_rootRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_rootRotation)
     self->SetAvatarRotation(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropApplyRootMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropApplyRootMotion)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_applyRootMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_applyRootMotion)
    return self->GetApplyRootMotion();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropApplyRootMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropApplyRootMotion)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_applyRootMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_applyRootMotion)
     self->SetApplyRootMotion(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropAnimatePhysics(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropAnimatePhysics)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_animatePhysics)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_animatePhysics)
    return self->GetAnimatePhysics();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropAnimatePhysics(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropAnimatePhysics)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_animatePhysics)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_animatePhysics)
     self->SetAnimatePhysics(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropGravityWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropGravityWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_gravityWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_gravityWeight)
    return self->GetGravityWeight();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_bodyPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_bodyPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_bodyPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_bodyPosition)
    { *returnValue =(self->GetBodyPosition()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_set_bodyPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_set_bodyPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_bodyPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_bodyPosition)
     self->SetBodyPosition(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_bodyRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_bodyRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_bodyRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_bodyRotation)
    { *returnValue =(self->GetBodyRotation()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_set_bodyRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_set_bodyRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_bodyRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_bodyRotation)
     self->SetBodyRotation(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_GetIKPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_GetIKPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetIKPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetIKPosition)
      { returnValue =(self->GetGoalPosition(goal) ); return; }
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetIKPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal, const Vector3f& goalPosition)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetIKPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetIKPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetIKPosition)
     self->SetGoalPosition(goal,goalPosition); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_GetIKRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_GetIKRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetIKRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetIKRotation)
     { returnValue =( self->GetGoalRotation(goal)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetIKRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal, const Quaternionf& goalRotation)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetIKRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetIKRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetIKRotation)
     self->SetGoalRotation(goal, goalRotation); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetIKPositionWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetIKPositionWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetIKPositionWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIKPositionWeight)
     return self->GetGoalWeightPosition(goal); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetIKPositionWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetIKPositionWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetIKPositionWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetIKPositionWeight)
     self->SetGoalWeightPosition(goal,value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetIKRotationWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetIKRotationWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetIKRotationWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIKRotationWeight)
     return self->GetGoalWeightRotation(goal); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetIKRotationWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int goal, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetIKRotationWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetIKRotationWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetIKRotationWeight)
     self->SetGoalWeightRotation(goal,value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetLookAtPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& lookAtPosition)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetLookAtPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetLookAtPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetLookAtPosition)
     self->SetLookAtPosition(lookAtPosition); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetLookAtWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, float weight, float bodyWeight, float headWeight, float eyesWeight, float clampWeight)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetLookAtWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetLookAtWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLookAtWeight)
     
    		self->SetLookAtBodyWeight(weight*bodyWeight); 
    		self->SetLookAtHeadWeight(weight*headWeight); 
    		self->SetLookAtEyesWeight(weight*eyesWeight); 
    		self->SetLookAtClampWeight(clampWeight); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropStabilizeFeet(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropStabilizeFeet)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stabilizeFeet)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stabilizeFeet)
    return self->GetStabilizeFeet();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropStabilizeFeet(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropStabilizeFeet)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stabilizeFeet)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stabilizeFeet)
     self->SetStabilizeFeet(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropLayerCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropLayerCount)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_layerCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_layerCount)
    return self->GetHumanLayerCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetLayerName(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetLayerName)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetLayerName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetLayerName)
     return CreateScriptingString(self->GetHumanLayerName(layerIndex)) ; 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetLayerWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetLayerWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetLayerWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetLayerWeight)
     return self->GetHumanLayerWeight(layerIndex) ; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetLayerWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex, float weight)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetLayerWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetLayerWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLayerWeight)
     self->SetHumanLayerWeight(layerIndex, weight); 
}

SCRIPT_BINDINGS_EXPORT_DECL
AnimatorStateInfo SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetCurrentAnimatorStateInfo(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetCurrentAnimatorStateInfo)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetCurrentAnimatorStateInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetCurrentAnimatorStateInfo)
    
    		AnimatorStateInfo info;
    		self->GetAnimatorStateInfo (layerIndex, true, info);
    		return info;		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
AnimatorStateInfo SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetNextAnimatorStateInfo(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetNextAnimatorStateInfo)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetNextAnimatorStateInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNextAnimatorStateInfo)
    		
    		AnimatorStateInfo info;
    		self->GetAnimatorStateInfo(layerIndex, false, info); 
    		return info;		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
AnimatorTransitionInfo SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetAnimatorTransitionInfo(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetAnimatorTransitionInfo)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetAnimatorTransitionInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAnimatorTransitionInfo)
    		
    		AnimatorTransitionInfo  info;					
    		self->GetAnimatorTransitionInfo(layerIndex, info);
    		return info;		
    	
}

#if !UNITY_FLASH && !PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetCurrentAnimationClipState(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetCurrentAnimationClipState)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetCurrentAnimationClipState)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetCurrentAnimationClipState)
    
    		dynamic_array<AnimationInfo> clips (kMemAnimationTemp);
    		self->GetAnimationClipState(layerIndex, true, clips);
    		return DynamicArrayToMonoStructArray(clips, GetMonoManager().GetCommonClasses().animationInfo, AnimationInfoToMono);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetNextAnimationClipState(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetNextAnimationClipState)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetNextAnimationClipState)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNextAnimationClipState)
    
    		dynamic_array<AnimationInfo> clips (kMemAnimationTemp);
    		self->GetAnimationClipState(layerIndex, false, clips);
    		return DynamicArrayToMonoStructArray(clips, GetMonoManager().GetCommonClasses().animationInfo, AnimationInfoToMono);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_CUSTOM_IsInTransition(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_IsInTransition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(IsInTransition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsInTransition)
     return self->IsInTransition(layerIndex); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropFeetPivotActive(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropFeetPivotActive)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_feetPivotActive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_feetPivotActive)
    return self->GetFeetPivotActive();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropFeetPivotActive(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropFeetPivotActive)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_feetPivotActive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_feetPivotActive)
     self->SetFeetPivotActive(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropPivotWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropPivotWeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pivotWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pivotWeight)
    return self->GetPivotWeight();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_pivotPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_pivotPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_pivotPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_pivotPosition)
    { *returnValue =(self->GetPivotPosition()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_MatchTarget(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& matchPosition, const Quaternionf& matchRotation, int targetBodyPart, const MatchTargetWeightMask& weightMask, float startNormalizedTime, float targetNormalizedTime)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_MatchTarget)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_MatchTarget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_MatchTarget)
     		
    		self->MatchTarget(matchPosition, matchRotation, (int)targetBodyPart, weightMask, startNormalizedTime, targetNormalizedTime);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_InterruptMatchTarget(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool completeMatch)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_InterruptMatchTarget)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InterruptMatchTarget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InterruptMatchTarget)
     self->InterruptMatchTarget(completeMatch);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropIsMatchingTarget(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropIsMatchingTarget)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isMatchingTarget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isMatchingTarget)
    return self->IsMatchingTarget();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropSpeed)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_speed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_speed)
    return self->GetSpeed() ;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropSpeed)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_speed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_speed)
     self->SetSpeed(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_ForceStateNormalizedTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float normalizedTime)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_ForceStateNormalizedTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ForceStateNormalizedTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ForceStateNormalizedTime)
     self->ForceStateNormalizedTime(normalizedTime);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetTarget(ICallType_ReadOnlyUnityEngineObject_Argument self_, int targetIndex, float targetNormalizedTime)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetTarget)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetTarget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetTarget)
    self->SetTarget((int)targetIndex, targetNormalizedTime);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_targetPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_targetPosition)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetPosition)
    { *returnValue =(self->GetTargetPosition()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_get_targetRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_get_targetRotation)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetRotation)
    { *returnValue =(self->GetTargetRotation()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_CUSTOM_IsControlled(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument transform_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_IsControlled)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> transform(transform_);
    UNUSED(transform);
    SCRIPTINGAPI_STACK_CHECK(IsControlled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsControlled)
    return self->IsControlled(*transform) ;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetBoneTransform(ICallType_ReadOnlyUnityEngineObject_Argument self_, HumanBodyBones humanBoneId)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetBoneTransform)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetBoneTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBoneTransform)
    return Scripting::ScriptingWrapperFor(&self->GetBoneTransform((int)humanBoneId));
}

SCRIPT_BINDINGS_EXPORT_DECL
Animator::CullingMode SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropCullingMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropCullingMode)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cullingMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cullingMode)
    return self->GetCullingMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropCullingMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, Animator::CullingMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropCullingMode)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cullingMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cullingMode)
    
    self->SetCullingMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_StartPlayback(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_StartPlayback)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(StartPlayback)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StartPlayback)
     self->StartPlayback();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_StopPlayback(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_StopPlayback)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(StopPlayback)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopPlayback)
     self->StopPlayback();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropPlaybackTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropPlaybackTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_playbackTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_playbackTime)
    return self->GetPlaybackTime();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropPlaybackTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropPlaybackTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_playbackTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_playbackTime)
     self->SetPlaybackTime(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_StartRecording(ICallType_ReadOnlyUnityEngineObject_Argument self_, int frameCount)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_StartRecording)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(StartRecording)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StartRecording)
     self->StartRecording(frameCount);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_StopRecording(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_StopRecording)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(StopRecording)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopRecording)
     self->StopRecording();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropRecorderStartTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropRecorderStartTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_recorderStartTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_recorderStartTime)
    return self->GetRecorderStartTime();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropRecorderStartTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropRecorderStartTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_recorderStartTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_recorderStartTime)
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropRecorderStopTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropRecorderStopTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_recorderStopTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_recorderStopTime)
    return self->GetRecorderStopTime();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropRecorderStopTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropRecorderStopTime)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_recorderStopTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_recorderStopTime)
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropRuntimeAnimatorController(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropRuntimeAnimatorController)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_runtimeAnimatorController)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_runtimeAnimatorController)
    return Scripting::ScriptingWrapperFor(self->GetRuntimeAnimatorController());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropRuntimeAnimatorController(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropRuntimeAnimatorController)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<RuntimeAnimatorController> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_runtimeAnimatorController)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_runtimeAnimatorController)
     self->SetRuntimeAnimatorController(value);
}

static int ScriptingStringToCRC32 (ICallString& stringValue)
	{
		if(stringValue.IsNull()) return 0;
		#if ENABLE_MONO
		const gunichar2* chars = mono_string_chars(stringValue.str);
		const size_t length = stringValue.Length();
		
		if (IsUtf16InAsciiRange (chars, length))
		{
			return mecanim::processCRC32UTF16Ascii (chars, length);
		}
		else
		#endif
		{
			return mecanim::processCRC32 (stringValue.ToUTF8().c_str());
		}
	}
	
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Animator_CUSTOM_StringToHash(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_StringToHash)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(StringToHash)
     return ScriptingStringToCRC32(name); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropAvatar(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropAvatar)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_avatar)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_avatar)
    return Scripting::ScriptingWrapperFor(self->GetAvatar());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropAvatar(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropAvatar)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Avatar> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_avatar)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_avatar)
     self->SetAvatar(value);
}



	#define GET_NAME_IMPL(Func,x) \
		x value; \
		GetSetValueResult result = self->Func(ScriptingStringToCRC32(name), value); \
		if (result != kGetSetSuccess)  \
			self->ValidateParameterString (result, name); \
		return value;

	#define SET_NAME_IMPL(Func) \
		GetSetValueResult result = self->Func(ScriptingStringToCRC32(name), value); \
		if (result != kGetSetSuccess) \
			self->ValidateParameterString (result, name);


	#define GET_ID_IMPL(Func,x) \
		x value; \
		GetSetValueResult result = self->Func(id, value); \
		if (result != kGetSetSuccess) \
			self->ValidateParameterID (result, id); \
		return value;

	#define SET_ID_IMPL(Func) \
		GetSetValueResult result = self->Func(id, value); \
		if (result != kGetSetSuccess)  \
			self->ValidateParameterID (result, id);

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetFloatString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetFloatString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetFloatString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFloatString)
     SET_NAME_IMPL(SetFloat) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetFloatID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetFloatID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetFloatID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFloatID)
     SET_ID_IMPL  (SetFloat) 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetFloatString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetFloatString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetFloatString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetFloatString)
     GET_NAME_IMPL(GetFloat, float) 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetFloatID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetFloatID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetFloatID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetFloatID)
     GET_ID_IMPL  (GetFloat, float) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetBoolString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetBoolString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetBoolString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetBoolString)
     SET_NAME_IMPL(SetBool)   
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetBoolID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetBoolID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetBoolID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetBoolID)
     SET_ID_IMPL  (SetBool)   
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetBoolString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetBoolString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetBoolString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBoolString)
     GET_NAME_IMPL(GetBool, bool)   
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetBoolID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetBoolID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetBoolID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBoolID)
     GET_ID_IMPL  (GetBool, bool)   
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetIntegerString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetIntegerString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetIntegerString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetIntegerString)
     SET_NAME_IMPL(SetInteger) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetIntegerID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetIntegerID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetIntegerID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetIntegerID)
     SET_ID_IMPL  (SetInteger)
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetIntegerString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetIntegerString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(GetIntegerString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIntegerString)
     GET_NAME_IMPL(GetInteger, int) 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetIntegerID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetIntegerID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetIntegerID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIntegerID)
     GET_ID_IMPL  (GetInteger, int) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetVectorString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetVectorString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetVectorString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetVectorString)
     SET_NAME_IMPL(SetVector) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetVectorID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetVectorID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetVectorID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetVectorID)
     SET_ID_IMPL  (SetVector) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_GetVectorString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_GetVectorString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetVectorString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetVectorString)
     GET_NAME_IMPL(GetVector, Vector3f) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_GetVectorID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_GetVectorID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetVectorID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetVectorID)
     GET_ID_IMPL(GetVector, Vector3f) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetQuaternionString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetQuaternionString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetQuaternionString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetQuaternionString)
     SET_NAME_IMPL(SetQuaternion) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_SetQuaternionID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_SetQuaternionID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetQuaternionID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetQuaternionID)
     SET_ID_IMPL  (SetQuaternion) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_GetQuaternionString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_GetQuaternionString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetQuaternionString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetQuaternionString)
     GET_NAME_IMPL(GetQuaternion, Quaternionf) 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_INTERNAL_CALL_GetQuaternionID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_INTERNAL_CALL_GetQuaternionID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetQuaternionID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetQuaternionID)
     GET_ID_IMPL  (GetQuaternion, Quaternionf) 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_CUSTOM_IsParameterControlledByCurveString(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_IsParameterControlledByCurveString)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(IsParameterControlledByCurveString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsParameterControlledByCurveString)
    
    		GetSetValueResult result = self->ParameterControlledByCurve(ScriptingStringToCRC32(name));
    		if (result == kParameterIsControlledByCurve)
    			return true;
    		else if (result == kGetSetSuccess)
    			return false;
    		else
    		{
    			self->ValidateParameterString (result, name);
    			return false;
    		}	
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_CUSTOM_IsParameterControlledByCurveID(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_IsParameterControlledByCurveID)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(IsParameterControlledByCurveID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsParameterControlledByCurveID)
    
    		GetSetValueResult result = self->ParameterControlledByCurve(id);
    		if (result == kParameterIsControlledByCurve)
    			return true;
    		else if (result == kGetSetSuccess)
    			return false;
    		else
    		{
    			self->ValidateParameterID (result, id);
    			return false;
    		}	
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetFloatStringDamp(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, float value, float dampTime, float deltaTime)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetFloatStringDamp)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetFloatStringDamp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFloatStringDamp)
    
    		GetSetValueResult result = self->SetFloatDamp(ScriptingStringToCRC32(name), value, dampTime, deltaTime);
    		if (result != kGetSetSuccess)
    			self->ValidateParameterString (result, name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_SetFloatIDDamp(ICallType_ReadOnlyUnityEngineObject_Argument self_, int id, float value, float dampTime, float deltaTime)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_SetFloatIDDamp)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetFloatIDDamp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFloatIDDamp)
    
    		GetSetValueResult result = self->SetFloatDamp(id, value, dampTime, deltaTime);
    		if (result != kGetSetSuccess)
    			self->ValidateParameterID (result, id);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropLayersAffectMassCenter(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropLayersAffectMassCenter)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_layersAffectMassCenter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_layersAffectMassCenter)
    return self->GetLayersAffectMassCenter();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropLayersAffectMassCenter(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropLayersAffectMassCenter)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_layersAffectMassCenter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_layersAffectMassCenter)
     self->SetLayersAffectMassCenter(value);
}



SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropLeftFeetBottomHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropLeftFeetBottomHeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_leftFeetBottomHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_leftFeetBottomHeight)
    return self->GetLeftFeetBottomHeight();
}


SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropRightFeetBottomHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropRightFeetBottomHeight)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rightFeetBottomHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rightFeetBottomHeight)
    return self->GetRightFeetBottomHeight();
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropSupportsOnAnimatorMove(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropSupportsOnAnimatorMove)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_supportsOnAnimatorMove)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsOnAnimatorMove)
    return self->SupportsOnAnimatorMove ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_WriteDefaultPose(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_WriteDefaultPose)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(WriteDefaultPose)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(WriteDefaultPose)
     self->WriteDefaultPose(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_Update(ICallType_ReadOnlyUnityEngineObject_Argument self_, float deltaTime)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_Update)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Update)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Update)
     self->Update(deltaTime);
}

#endif
#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_CUSTOM_EvaluateSM(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_EvaluateSM)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(EvaluateSM)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(EvaluateSM)
    self->EvaluateSM();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetCurrentStateName(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetCurrentStateName)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetCurrentStateName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetCurrentStateName)
     return CreateScriptingString(self->GetAnimatorStateName(layerIndex,true));
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Animator_CUSTOM_GetNextStateName(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layerIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_CUSTOM_GetNextStateName)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetNextStateName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNextStateName)
     return CreateScriptingString(self->GetAnimatorStateName(layerIndex,false));
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropIsInManagerList(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropIsInManagerList)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isInManagerList)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isInManagerList)
    return self->IsInManagerList();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Animator_Get_Custom_PropLogWarnings(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Get_Custom_PropLogWarnings)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_logWarnings)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_logWarnings)
    return self->GetLogWarnings ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Animator_Set_Custom_PropLogWarnings(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Animator_Set_Custom_PropLogWarnings)
    ReadOnlyScriptingObjectOfType<Animator> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_logWarnings)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_logWarnings)
    
    self->SetLogWarnings (value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Animator_get_isHuman()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_isHuman" , (gpointer)& Animator_Get_Custom_PropIsHuman );
}

void Register_UnityEngine_Animator_get_humanScale()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_humanScale" , (gpointer)& Animator_Get_Custom_PropHumanScale );
}

void Register_UnityEngine_Animator_INTERNAL_get_deltaPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_deltaPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_get_deltaPosition );
}

void Register_UnityEngine_Animator_INTERNAL_get_deltaRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_deltaRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_get_deltaRotation );
}

void Register_UnityEngine_Animator_INTERNAL_get_rootPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_rootPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_get_rootPosition );
}

void Register_UnityEngine_Animator_INTERNAL_set_rootPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_set_rootPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_set_rootPosition );
}

void Register_UnityEngine_Animator_INTERNAL_get_rootRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_rootRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_get_rootRotation );
}

void Register_UnityEngine_Animator_INTERNAL_set_rootRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_set_rootRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_set_rootRotation );
}

void Register_UnityEngine_Animator_get_applyRootMotion()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_applyRootMotion" , (gpointer)& Animator_Get_Custom_PropApplyRootMotion );
}

void Register_UnityEngine_Animator_set_applyRootMotion()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_applyRootMotion" , (gpointer)& Animator_Set_Custom_PropApplyRootMotion );
}

void Register_UnityEngine_Animator_get_animatePhysics()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_animatePhysics" , (gpointer)& Animator_Get_Custom_PropAnimatePhysics );
}

void Register_UnityEngine_Animator_set_animatePhysics()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_animatePhysics" , (gpointer)& Animator_Set_Custom_PropAnimatePhysics );
}

void Register_UnityEngine_Animator_get_gravityWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_gravityWeight" , (gpointer)& Animator_Get_Custom_PropGravityWeight );
}

void Register_UnityEngine_Animator_INTERNAL_get_bodyPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_bodyPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_get_bodyPosition );
}

void Register_UnityEngine_Animator_INTERNAL_set_bodyPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_set_bodyPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_set_bodyPosition );
}

void Register_UnityEngine_Animator_INTERNAL_get_bodyRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_bodyRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_get_bodyRotation );
}

void Register_UnityEngine_Animator_INTERNAL_set_bodyRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_set_bodyRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_set_bodyRotation );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_GetIKPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_GetIKPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_GetIKPosition );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetIKPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetIKPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetIKPosition );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_GetIKRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_GetIKRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_GetIKRotation );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetIKRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetIKRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetIKRotation );
}

void Register_UnityEngine_Animator_GetIKPositionWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetIKPositionWeight" , (gpointer)& Animator_CUSTOM_GetIKPositionWeight );
}

void Register_UnityEngine_Animator_SetIKPositionWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetIKPositionWeight" , (gpointer)& Animator_CUSTOM_SetIKPositionWeight );
}

void Register_UnityEngine_Animator_GetIKRotationWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetIKRotationWeight" , (gpointer)& Animator_CUSTOM_GetIKRotationWeight );
}

void Register_UnityEngine_Animator_SetIKRotationWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetIKRotationWeight" , (gpointer)& Animator_CUSTOM_SetIKRotationWeight );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetLookAtPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetLookAtPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetLookAtPosition );
}

void Register_UnityEngine_Animator_SetLookAtWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetLookAtWeight" , (gpointer)& Animator_CUSTOM_SetLookAtWeight );
}

void Register_UnityEngine_Animator_get_stabilizeFeet()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_stabilizeFeet" , (gpointer)& Animator_Get_Custom_PropStabilizeFeet );
}

void Register_UnityEngine_Animator_set_stabilizeFeet()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_stabilizeFeet" , (gpointer)& Animator_Set_Custom_PropStabilizeFeet );
}

void Register_UnityEngine_Animator_get_layerCount()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_layerCount" , (gpointer)& Animator_Get_Custom_PropLayerCount );
}

void Register_UnityEngine_Animator_GetLayerName()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetLayerName" , (gpointer)& Animator_CUSTOM_GetLayerName );
}

void Register_UnityEngine_Animator_GetLayerWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetLayerWeight" , (gpointer)& Animator_CUSTOM_GetLayerWeight );
}

void Register_UnityEngine_Animator_SetLayerWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetLayerWeight" , (gpointer)& Animator_CUSTOM_SetLayerWeight );
}

void Register_UnityEngine_Animator_GetCurrentAnimatorStateInfo()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetCurrentAnimatorStateInfo" , (gpointer)& Animator_CUSTOM_GetCurrentAnimatorStateInfo );
}

void Register_UnityEngine_Animator_GetNextAnimatorStateInfo()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetNextAnimatorStateInfo" , (gpointer)& Animator_CUSTOM_GetNextAnimatorStateInfo );
}

void Register_UnityEngine_Animator_GetAnimatorTransitionInfo()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetAnimatorTransitionInfo" , (gpointer)& Animator_CUSTOM_GetAnimatorTransitionInfo );
}

#if !UNITY_FLASH && !PLATFORM_WINRT
void Register_UnityEngine_Animator_GetCurrentAnimationClipState()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetCurrentAnimationClipState" , (gpointer)& Animator_CUSTOM_GetCurrentAnimationClipState );
}

void Register_UnityEngine_Animator_GetNextAnimationClipState()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetNextAnimationClipState" , (gpointer)& Animator_CUSTOM_GetNextAnimationClipState );
}

#endif
void Register_UnityEngine_Animator_IsInTransition()
{
    scripting_add_internal_call( "UnityEngine.Animator::IsInTransition" , (gpointer)& Animator_CUSTOM_IsInTransition );
}

void Register_UnityEngine_Animator_get_feetPivotActive()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_feetPivotActive" , (gpointer)& Animator_Get_Custom_PropFeetPivotActive );
}

void Register_UnityEngine_Animator_set_feetPivotActive()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_feetPivotActive" , (gpointer)& Animator_Set_Custom_PropFeetPivotActive );
}

void Register_UnityEngine_Animator_get_pivotWeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_pivotWeight" , (gpointer)& Animator_Get_Custom_PropPivotWeight );
}

void Register_UnityEngine_Animator_INTERNAL_get_pivotPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_pivotPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_get_pivotPosition );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_MatchTarget()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_MatchTarget" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_MatchTarget );
}

void Register_UnityEngine_Animator_InterruptMatchTarget()
{
    scripting_add_internal_call( "UnityEngine.Animator::InterruptMatchTarget" , (gpointer)& Animator_CUSTOM_InterruptMatchTarget );
}

void Register_UnityEngine_Animator_get_isMatchingTarget()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_isMatchingTarget" , (gpointer)& Animator_Get_Custom_PropIsMatchingTarget );
}

void Register_UnityEngine_Animator_get_speed()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_speed" , (gpointer)& Animator_Get_Custom_PropSpeed );
}

void Register_UnityEngine_Animator_set_speed()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_speed" , (gpointer)& Animator_Set_Custom_PropSpeed );
}

void Register_UnityEngine_Animator_ForceStateNormalizedTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::ForceStateNormalizedTime" , (gpointer)& Animator_CUSTOM_ForceStateNormalizedTime );
}

void Register_UnityEngine_Animator_SetTarget()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetTarget" , (gpointer)& Animator_CUSTOM_SetTarget );
}

void Register_UnityEngine_Animator_INTERNAL_get_targetPosition()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_targetPosition" , (gpointer)& Animator_CUSTOM_INTERNAL_get_targetPosition );
}

void Register_UnityEngine_Animator_INTERNAL_get_targetRotation()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_get_targetRotation" , (gpointer)& Animator_CUSTOM_INTERNAL_get_targetRotation );
}

void Register_UnityEngine_Animator_IsControlled()
{
    scripting_add_internal_call( "UnityEngine.Animator::IsControlled" , (gpointer)& Animator_CUSTOM_IsControlled );
}

void Register_UnityEngine_Animator_GetBoneTransform()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetBoneTransform" , (gpointer)& Animator_CUSTOM_GetBoneTransform );
}

void Register_UnityEngine_Animator_get_cullingMode()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_cullingMode" , (gpointer)& Animator_Get_Custom_PropCullingMode );
}

void Register_UnityEngine_Animator_set_cullingMode()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_cullingMode" , (gpointer)& Animator_Set_Custom_PropCullingMode );
}

void Register_UnityEngine_Animator_StartPlayback()
{
    scripting_add_internal_call( "UnityEngine.Animator::StartPlayback" , (gpointer)& Animator_CUSTOM_StartPlayback );
}

void Register_UnityEngine_Animator_StopPlayback()
{
    scripting_add_internal_call( "UnityEngine.Animator::StopPlayback" , (gpointer)& Animator_CUSTOM_StopPlayback );
}

void Register_UnityEngine_Animator_get_playbackTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_playbackTime" , (gpointer)& Animator_Get_Custom_PropPlaybackTime );
}

void Register_UnityEngine_Animator_set_playbackTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_playbackTime" , (gpointer)& Animator_Set_Custom_PropPlaybackTime );
}

void Register_UnityEngine_Animator_StartRecording()
{
    scripting_add_internal_call( "UnityEngine.Animator::StartRecording" , (gpointer)& Animator_CUSTOM_StartRecording );
}

void Register_UnityEngine_Animator_StopRecording()
{
    scripting_add_internal_call( "UnityEngine.Animator::StopRecording" , (gpointer)& Animator_CUSTOM_StopRecording );
}

void Register_UnityEngine_Animator_get_recorderStartTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_recorderStartTime" , (gpointer)& Animator_Get_Custom_PropRecorderStartTime );
}

void Register_UnityEngine_Animator_set_recorderStartTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_recorderStartTime" , (gpointer)& Animator_Set_Custom_PropRecorderStartTime );
}

void Register_UnityEngine_Animator_get_recorderStopTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_recorderStopTime" , (gpointer)& Animator_Get_Custom_PropRecorderStopTime );
}

void Register_UnityEngine_Animator_set_recorderStopTime()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_recorderStopTime" , (gpointer)& Animator_Set_Custom_PropRecorderStopTime );
}

void Register_UnityEngine_Animator_get_runtimeAnimatorController()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_runtimeAnimatorController" , (gpointer)& Animator_Get_Custom_PropRuntimeAnimatorController );
}

void Register_UnityEngine_Animator_set_runtimeAnimatorController()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_runtimeAnimatorController" , (gpointer)& Animator_Set_Custom_PropRuntimeAnimatorController );
}

void Register_UnityEngine_Animator_StringToHash()
{
    scripting_add_internal_call( "UnityEngine.Animator::StringToHash" , (gpointer)& Animator_CUSTOM_StringToHash );
}

void Register_UnityEngine_Animator_get_avatar()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_avatar" , (gpointer)& Animator_Get_Custom_PropAvatar );
}

void Register_UnityEngine_Animator_set_avatar()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_avatar" , (gpointer)& Animator_Set_Custom_PropAvatar );
}

void Register_UnityEngine_Animator_SetFloatString()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetFloatString" , (gpointer)& Animator_CUSTOM_SetFloatString );
}

void Register_UnityEngine_Animator_SetFloatID()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetFloatID" , (gpointer)& Animator_CUSTOM_SetFloatID );
}

void Register_UnityEngine_Animator_GetFloatString()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetFloatString" , (gpointer)& Animator_CUSTOM_GetFloatString );
}

void Register_UnityEngine_Animator_GetFloatID()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetFloatID" , (gpointer)& Animator_CUSTOM_GetFloatID );
}

void Register_UnityEngine_Animator_SetBoolString()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetBoolString" , (gpointer)& Animator_CUSTOM_SetBoolString );
}

void Register_UnityEngine_Animator_SetBoolID()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetBoolID" , (gpointer)& Animator_CUSTOM_SetBoolID );
}

void Register_UnityEngine_Animator_GetBoolString()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetBoolString" , (gpointer)& Animator_CUSTOM_GetBoolString );
}

void Register_UnityEngine_Animator_GetBoolID()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetBoolID" , (gpointer)& Animator_CUSTOM_GetBoolID );
}

void Register_UnityEngine_Animator_SetIntegerString()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetIntegerString" , (gpointer)& Animator_CUSTOM_SetIntegerString );
}

void Register_UnityEngine_Animator_SetIntegerID()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetIntegerID" , (gpointer)& Animator_CUSTOM_SetIntegerID );
}

void Register_UnityEngine_Animator_GetIntegerString()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetIntegerString" , (gpointer)& Animator_CUSTOM_GetIntegerString );
}

void Register_UnityEngine_Animator_GetIntegerID()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetIntegerID" , (gpointer)& Animator_CUSTOM_GetIntegerID );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetVectorString()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetVectorString" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetVectorString );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetVectorID()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetVectorID" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetVectorID );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_GetVectorString()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_GetVectorString" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_GetVectorString );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_GetVectorID()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_GetVectorID" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_GetVectorID );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetQuaternionString()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetQuaternionString" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetQuaternionString );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_SetQuaternionID()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_SetQuaternionID" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_SetQuaternionID );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_GetQuaternionString()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_GetQuaternionString" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_GetQuaternionString );
}

void Register_UnityEngine_Animator_INTERNAL_CALL_GetQuaternionID()
{
    scripting_add_internal_call( "UnityEngine.Animator::INTERNAL_CALL_GetQuaternionID" , (gpointer)& Animator_CUSTOM_INTERNAL_CALL_GetQuaternionID );
}

void Register_UnityEngine_Animator_IsParameterControlledByCurveString()
{
    scripting_add_internal_call( "UnityEngine.Animator::IsParameterControlledByCurveString" , (gpointer)& Animator_CUSTOM_IsParameterControlledByCurveString );
}

void Register_UnityEngine_Animator_IsParameterControlledByCurveID()
{
    scripting_add_internal_call( "UnityEngine.Animator::IsParameterControlledByCurveID" , (gpointer)& Animator_CUSTOM_IsParameterControlledByCurveID );
}

void Register_UnityEngine_Animator_SetFloatStringDamp()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetFloatStringDamp" , (gpointer)& Animator_CUSTOM_SetFloatStringDamp );
}

void Register_UnityEngine_Animator_SetFloatIDDamp()
{
    scripting_add_internal_call( "UnityEngine.Animator::SetFloatIDDamp" , (gpointer)& Animator_CUSTOM_SetFloatIDDamp );
}

void Register_UnityEngine_Animator_get_layersAffectMassCenter()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_layersAffectMassCenter" , (gpointer)& Animator_Get_Custom_PropLayersAffectMassCenter );
}

void Register_UnityEngine_Animator_set_layersAffectMassCenter()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_layersAffectMassCenter" , (gpointer)& Animator_Set_Custom_PropLayersAffectMassCenter );
}

void Register_UnityEngine_Animator_get_leftFeetBottomHeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_leftFeetBottomHeight" , (gpointer)& Animator_Get_Custom_PropLeftFeetBottomHeight );
}

void Register_UnityEngine_Animator_get_rightFeetBottomHeight()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_rightFeetBottomHeight" , (gpointer)& Animator_Get_Custom_PropRightFeetBottomHeight );
}

#if UNITY_EDITOR
void Register_UnityEngine_Animator_get_supportsOnAnimatorMove()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_supportsOnAnimatorMove" , (gpointer)& Animator_Get_Custom_PropSupportsOnAnimatorMove );
}

void Register_UnityEngine_Animator_WriteDefaultPose()
{
    scripting_add_internal_call( "UnityEngine.Animator::WriteDefaultPose" , (gpointer)& Animator_CUSTOM_WriteDefaultPose );
}

void Register_UnityEngine_Animator_Update()
{
    scripting_add_internal_call( "UnityEngine.Animator::Update" , (gpointer)& Animator_CUSTOM_Update );
}

void Register_UnityEngine_Animator_EvaluateSM()
{
    scripting_add_internal_call( "UnityEngine.Animator::EvaluateSM" , (gpointer)& Animator_CUSTOM_EvaluateSM );
}

void Register_UnityEngine_Animator_GetCurrentStateName()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetCurrentStateName" , (gpointer)& Animator_CUSTOM_GetCurrentStateName );
}

void Register_UnityEngine_Animator_GetNextStateName()
{
    scripting_add_internal_call( "UnityEngine.Animator::GetNextStateName" , (gpointer)& Animator_CUSTOM_GetNextStateName );
}

#endif
void Register_UnityEngine_Animator_get_isInManagerList()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_isInManagerList" , (gpointer)& Animator_Get_Custom_PropIsInManagerList );
}

void Register_UnityEngine_Animator_get_logWarnings()
{
    scripting_add_internal_call( "UnityEngine.Animator::get_logWarnings" , (gpointer)& Animator_Get_Custom_PropLogWarnings );
}

void Register_UnityEngine_Animator_set_logWarnings()
{
    scripting_add_internal_call( "UnityEngine.Animator::set_logWarnings" , (gpointer)& Animator_Set_Custom_PropLogWarnings );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Animator_IcallNames [] =
{
    "UnityEngine.Animator::get_isHuman"     ,    // -> Animator_Get_Custom_PropIsHuman
    "UnityEngine.Animator::get_humanScale"  ,    // -> Animator_Get_Custom_PropHumanScale
    "UnityEngine.Animator::INTERNAL_get_deltaPosition",    // -> Animator_CUSTOM_INTERNAL_get_deltaPosition
    "UnityEngine.Animator::INTERNAL_get_deltaRotation",    // -> Animator_CUSTOM_INTERNAL_get_deltaRotation
    "UnityEngine.Animator::INTERNAL_get_rootPosition",    // -> Animator_CUSTOM_INTERNAL_get_rootPosition
    "UnityEngine.Animator::INTERNAL_set_rootPosition",    // -> Animator_CUSTOM_INTERNAL_set_rootPosition
    "UnityEngine.Animator::INTERNAL_get_rootRotation",    // -> Animator_CUSTOM_INTERNAL_get_rootRotation
    "UnityEngine.Animator::INTERNAL_set_rootRotation",    // -> Animator_CUSTOM_INTERNAL_set_rootRotation
    "UnityEngine.Animator::get_applyRootMotion",    // -> Animator_Get_Custom_PropApplyRootMotion
    "UnityEngine.Animator::set_applyRootMotion",    // -> Animator_Set_Custom_PropApplyRootMotion
    "UnityEngine.Animator::get_animatePhysics",    // -> Animator_Get_Custom_PropAnimatePhysics
    "UnityEngine.Animator::set_animatePhysics",    // -> Animator_Set_Custom_PropAnimatePhysics
    "UnityEngine.Animator::get_gravityWeight",    // -> Animator_Get_Custom_PropGravityWeight
    "UnityEngine.Animator::INTERNAL_get_bodyPosition",    // -> Animator_CUSTOM_INTERNAL_get_bodyPosition
    "UnityEngine.Animator::INTERNAL_set_bodyPosition",    // -> Animator_CUSTOM_INTERNAL_set_bodyPosition
    "UnityEngine.Animator::INTERNAL_get_bodyRotation",    // -> Animator_CUSTOM_INTERNAL_get_bodyRotation
    "UnityEngine.Animator::INTERNAL_set_bodyRotation",    // -> Animator_CUSTOM_INTERNAL_set_bodyRotation
    "UnityEngine.Animator::INTERNAL_CALL_GetIKPosition",    // -> Animator_CUSTOM_INTERNAL_CALL_GetIKPosition
    "UnityEngine.Animator::INTERNAL_CALL_SetIKPosition",    // -> Animator_CUSTOM_INTERNAL_CALL_SetIKPosition
    "UnityEngine.Animator::INTERNAL_CALL_GetIKRotation",    // -> Animator_CUSTOM_INTERNAL_CALL_GetIKRotation
    "UnityEngine.Animator::INTERNAL_CALL_SetIKRotation",    // -> Animator_CUSTOM_INTERNAL_CALL_SetIKRotation
    "UnityEngine.Animator::GetIKPositionWeight",    // -> Animator_CUSTOM_GetIKPositionWeight
    "UnityEngine.Animator::SetIKPositionWeight",    // -> Animator_CUSTOM_SetIKPositionWeight
    "UnityEngine.Animator::GetIKRotationWeight",    // -> Animator_CUSTOM_GetIKRotationWeight
    "UnityEngine.Animator::SetIKRotationWeight",    // -> Animator_CUSTOM_SetIKRotationWeight
    "UnityEngine.Animator::INTERNAL_CALL_SetLookAtPosition",    // -> Animator_CUSTOM_INTERNAL_CALL_SetLookAtPosition
    "UnityEngine.Animator::SetLookAtWeight" ,    // -> Animator_CUSTOM_SetLookAtWeight
    "UnityEngine.Animator::get_stabilizeFeet",    // -> Animator_Get_Custom_PropStabilizeFeet
    "UnityEngine.Animator::set_stabilizeFeet",    // -> Animator_Set_Custom_PropStabilizeFeet
    "UnityEngine.Animator::get_layerCount"  ,    // -> Animator_Get_Custom_PropLayerCount
    "UnityEngine.Animator::GetLayerName"    ,    // -> Animator_CUSTOM_GetLayerName
    "UnityEngine.Animator::GetLayerWeight"  ,    // -> Animator_CUSTOM_GetLayerWeight
    "UnityEngine.Animator::SetLayerWeight"  ,    // -> Animator_CUSTOM_SetLayerWeight
    "UnityEngine.Animator::GetCurrentAnimatorStateInfo",    // -> Animator_CUSTOM_GetCurrentAnimatorStateInfo
    "UnityEngine.Animator::GetNextAnimatorStateInfo",    // -> Animator_CUSTOM_GetNextAnimatorStateInfo
    "UnityEngine.Animator::GetAnimatorTransitionInfo",    // -> Animator_CUSTOM_GetAnimatorTransitionInfo
#if !UNITY_FLASH && !PLATFORM_WINRT
    "UnityEngine.Animator::GetCurrentAnimationClipState",    // -> Animator_CUSTOM_GetCurrentAnimationClipState
    "UnityEngine.Animator::GetNextAnimationClipState",    // -> Animator_CUSTOM_GetNextAnimationClipState
#endif
    "UnityEngine.Animator::IsInTransition"  ,    // -> Animator_CUSTOM_IsInTransition
    "UnityEngine.Animator::get_feetPivotActive",    // -> Animator_Get_Custom_PropFeetPivotActive
    "UnityEngine.Animator::set_feetPivotActive",    // -> Animator_Set_Custom_PropFeetPivotActive
    "UnityEngine.Animator::get_pivotWeight" ,    // -> Animator_Get_Custom_PropPivotWeight
    "UnityEngine.Animator::INTERNAL_get_pivotPosition",    // -> Animator_CUSTOM_INTERNAL_get_pivotPosition
    "UnityEngine.Animator::INTERNAL_CALL_MatchTarget",    // -> Animator_CUSTOM_INTERNAL_CALL_MatchTarget
    "UnityEngine.Animator::InterruptMatchTarget",    // -> Animator_CUSTOM_InterruptMatchTarget
    "UnityEngine.Animator::get_isMatchingTarget",    // -> Animator_Get_Custom_PropIsMatchingTarget
    "UnityEngine.Animator::get_speed"       ,    // -> Animator_Get_Custom_PropSpeed
    "UnityEngine.Animator::set_speed"       ,    // -> Animator_Set_Custom_PropSpeed
    "UnityEngine.Animator::ForceStateNormalizedTime",    // -> Animator_CUSTOM_ForceStateNormalizedTime
    "UnityEngine.Animator::SetTarget"       ,    // -> Animator_CUSTOM_SetTarget
    "UnityEngine.Animator::INTERNAL_get_targetPosition",    // -> Animator_CUSTOM_INTERNAL_get_targetPosition
    "UnityEngine.Animator::INTERNAL_get_targetRotation",    // -> Animator_CUSTOM_INTERNAL_get_targetRotation
    "UnityEngine.Animator::IsControlled"    ,    // -> Animator_CUSTOM_IsControlled
    "UnityEngine.Animator::GetBoneTransform",    // -> Animator_CUSTOM_GetBoneTransform
    "UnityEngine.Animator::get_cullingMode" ,    // -> Animator_Get_Custom_PropCullingMode
    "UnityEngine.Animator::set_cullingMode" ,    // -> Animator_Set_Custom_PropCullingMode
    "UnityEngine.Animator::StartPlayback"   ,    // -> Animator_CUSTOM_StartPlayback
    "UnityEngine.Animator::StopPlayback"    ,    // -> Animator_CUSTOM_StopPlayback
    "UnityEngine.Animator::get_playbackTime",    // -> Animator_Get_Custom_PropPlaybackTime
    "UnityEngine.Animator::set_playbackTime",    // -> Animator_Set_Custom_PropPlaybackTime
    "UnityEngine.Animator::StartRecording"  ,    // -> Animator_CUSTOM_StartRecording
    "UnityEngine.Animator::StopRecording"   ,    // -> Animator_CUSTOM_StopRecording
    "UnityEngine.Animator::get_recorderStartTime",    // -> Animator_Get_Custom_PropRecorderStartTime
    "UnityEngine.Animator::set_recorderStartTime",    // -> Animator_Set_Custom_PropRecorderStartTime
    "UnityEngine.Animator::get_recorderStopTime",    // -> Animator_Get_Custom_PropRecorderStopTime
    "UnityEngine.Animator::set_recorderStopTime",    // -> Animator_Set_Custom_PropRecorderStopTime
    "UnityEngine.Animator::get_runtimeAnimatorController",    // -> Animator_Get_Custom_PropRuntimeAnimatorController
    "UnityEngine.Animator::set_runtimeAnimatorController",    // -> Animator_Set_Custom_PropRuntimeAnimatorController
    "UnityEngine.Animator::StringToHash"    ,    // -> Animator_CUSTOM_StringToHash
    "UnityEngine.Animator::get_avatar"      ,    // -> Animator_Get_Custom_PropAvatar
    "UnityEngine.Animator::set_avatar"      ,    // -> Animator_Set_Custom_PropAvatar
    "UnityEngine.Animator::SetFloatString"  ,    // -> Animator_CUSTOM_SetFloatString
    "UnityEngine.Animator::SetFloatID"      ,    // -> Animator_CUSTOM_SetFloatID
    "UnityEngine.Animator::GetFloatString"  ,    // -> Animator_CUSTOM_GetFloatString
    "UnityEngine.Animator::GetFloatID"      ,    // -> Animator_CUSTOM_GetFloatID
    "UnityEngine.Animator::SetBoolString"   ,    // -> Animator_CUSTOM_SetBoolString
    "UnityEngine.Animator::SetBoolID"       ,    // -> Animator_CUSTOM_SetBoolID
    "UnityEngine.Animator::GetBoolString"   ,    // -> Animator_CUSTOM_GetBoolString
    "UnityEngine.Animator::GetBoolID"       ,    // -> Animator_CUSTOM_GetBoolID
    "UnityEngine.Animator::SetIntegerString",    // -> Animator_CUSTOM_SetIntegerString
    "UnityEngine.Animator::SetIntegerID"    ,    // -> Animator_CUSTOM_SetIntegerID
    "UnityEngine.Animator::GetIntegerString",    // -> Animator_CUSTOM_GetIntegerString
    "UnityEngine.Animator::GetIntegerID"    ,    // -> Animator_CUSTOM_GetIntegerID
    "UnityEngine.Animator::INTERNAL_CALL_SetVectorString",    // -> Animator_CUSTOM_INTERNAL_CALL_SetVectorString
    "UnityEngine.Animator::INTERNAL_CALL_SetVectorID",    // -> Animator_CUSTOM_INTERNAL_CALL_SetVectorID
    "UnityEngine.Animator::INTERNAL_CALL_GetVectorString",    // -> Animator_CUSTOM_INTERNAL_CALL_GetVectorString
    "UnityEngine.Animator::INTERNAL_CALL_GetVectorID",    // -> Animator_CUSTOM_INTERNAL_CALL_GetVectorID
    "UnityEngine.Animator::INTERNAL_CALL_SetQuaternionString",    // -> Animator_CUSTOM_INTERNAL_CALL_SetQuaternionString
    "UnityEngine.Animator::INTERNAL_CALL_SetQuaternionID",    // -> Animator_CUSTOM_INTERNAL_CALL_SetQuaternionID
    "UnityEngine.Animator::INTERNAL_CALL_GetQuaternionString",    // -> Animator_CUSTOM_INTERNAL_CALL_GetQuaternionString
    "UnityEngine.Animator::INTERNAL_CALL_GetQuaternionID",    // -> Animator_CUSTOM_INTERNAL_CALL_GetQuaternionID
    "UnityEngine.Animator::IsParameterControlledByCurveString",    // -> Animator_CUSTOM_IsParameterControlledByCurveString
    "UnityEngine.Animator::IsParameterControlledByCurveID",    // -> Animator_CUSTOM_IsParameterControlledByCurveID
    "UnityEngine.Animator::SetFloatStringDamp",    // -> Animator_CUSTOM_SetFloatStringDamp
    "UnityEngine.Animator::SetFloatIDDamp"  ,    // -> Animator_CUSTOM_SetFloatIDDamp
    "UnityEngine.Animator::get_layersAffectMassCenter",    // -> Animator_Get_Custom_PropLayersAffectMassCenter
    "UnityEngine.Animator::set_layersAffectMassCenter",    // -> Animator_Set_Custom_PropLayersAffectMassCenter
    "UnityEngine.Animator::get_leftFeetBottomHeight",    // -> Animator_Get_Custom_PropLeftFeetBottomHeight
    "UnityEngine.Animator::get_rightFeetBottomHeight",    // -> Animator_Get_Custom_PropRightFeetBottomHeight
#if UNITY_EDITOR
    "UnityEngine.Animator::get_supportsOnAnimatorMove",    // -> Animator_Get_Custom_PropSupportsOnAnimatorMove
    "UnityEngine.Animator::WriteDefaultPose",    // -> Animator_CUSTOM_WriteDefaultPose
    "UnityEngine.Animator::Update"          ,    // -> Animator_CUSTOM_Update
    "UnityEngine.Animator::EvaluateSM"      ,    // -> Animator_CUSTOM_EvaluateSM
    "UnityEngine.Animator::GetCurrentStateName",    // -> Animator_CUSTOM_GetCurrentStateName
    "UnityEngine.Animator::GetNextStateName",    // -> Animator_CUSTOM_GetNextStateName
#endif
    "UnityEngine.Animator::get_isInManagerList",    // -> Animator_Get_Custom_PropIsInManagerList
    "UnityEngine.Animator::get_logWarnings" ,    // -> Animator_Get_Custom_PropLogWarnings
    "UnityEngine.Animator::set_logWarnings" ,    // -> Animator_Set_Custom_PropLogWarnings
    NULL
};

static const void* s_Animator_IcallFuncs [] =
{
    (const void*)&Animator_Get_Custom_PropIsHuman         ,  //  <- UnityEngine.Animator::get_isHuman
    (const void*)&Animator_Get_Custom_PropHumanScale      ,  //  <- UnityEngine.Animator::get_humanScale
    (const void*)&Animator_CUSTOM_INTERNAL_get_deltaPosition,  //  <- UnityEngine.Animator::INTERNAL_get_deltaPosition
    (const void*)&Animator_CUSTOM_INTERNAL_get_deltaRotation,  //  <- UnityEngine.Animator::INTERNAL_get_deltaRotation
    (const void*)&Animator_CUSTOM_INTERNAL_get_rootPosition,  //  <- UnityEngine.Animator::INTERNAL_get_rootPosition
    (const void*)&Animator_CUSTOM_INTERNAL_set_rootPosition,  //  <- UnityEngine.Animator::INTERNAL_set_rootPosition
    (const void*)&Animator_CUSTOM_INTERNAL_get_rootRotation,  //  <- UnityEngine.Animator::INTERNAL_get_rootRotation
    (const void*)&Animator_CUSTOM_INTERNAL_set_rootRotation,  //  <- UnityEngine.Animator::INTERNAL_set_rootRotation
    (const void*)&Animator_Get_Custom_PropApplyRootMotion ,  //  <- UnityEngine.Animator::get_applyRootMotion
    (const void*)&Animator_Set_Custom_PropApplyRootMotion ,  //  <- UnityEngine.Animator::set_applyRootMotion
    (const void*)&Animator_Get_Custom_PropAnimatePhysics  ,  //  <- UnityEngine.Animator::get_animatePhysics
    (const void*)&Animator_Set_Custom_PropAnimatePhysics  ,  //  <- UnityEngine.Animator::set_animatePhysics
    (const void*)&Animator_Get_Custom_PropGravityWeight   ,  //  <- UnityEngine.Animator::get_gravityWeight
    (const void*)&Animator_CUSTOM_INTERNAL_get_bodyPosition,  //  <- UnityEngine.Animator::INTERNAL_get_bodyPosition
    (const void*)&Animator_CUSTOM_INTERNAL_set_bodyPosition,  //  <- UnityEngine.Animator::INTERNAL_set_bodyPosition
    (const void*)&Animator_CUSTOM_INTERNAL_get_bodyRotation,  //  <- UnityEngine.Animator::INTERNAL_get_bodyRotation
    (const void*)&Animator_CUSTOM_INTERNAL_set_bodyRotation,  //  <- UnityEngine.Animator::INTERNAL_set_bodyRotation
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_GetIKPosition,  //  <- UnityEngine.Animator::INTERNAL_CALL_GetIKPosition
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetIKPosition,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetIKPosition
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_GetIKRotation,  //  <- UnityEngine.Animator::INTERNAL_CALL_GetIKRotation
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetIKRotation,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetIKRotation
    (const void*)&Animator_CUSTOM_GetIKPositionWeight     ,  //  <- UnityEngine.Animator::GetIKPositionWeight
    (const void*)&Animator_CUSTOM_SetIKPositionWeight     ,  //  <- UnityEngine.Animator::SetIKPositionWeight
    (const void*)&Animator_CUSTOM_GetIKRotationWeight     ,  //  <- UnityEngine.Animator::GetIKRotationWeight
    (const void*)&Animator_CUSTOM_SetIKRotationWeight     ,  //  <- UnityEngine.Animator::SetIKRotationWeight
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetLookAtPosition,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetLookAtPosition
    (const void*)&Animator_CUSTOM_SetLookAtWeight         ,  //  <- UnityEngine.Animator::SetLookAtWeight
    (const void*)&Animator_Get_Custom_PropStabilizeFeet   ,  //  <- UnityEngine.Animator::get_stabilizeFeet
    (const void*)&Animator_Set_Custom_PropStabilizeFeet   ,  //  <- UnityEngine.Animator::set_stabilizeFeet
    (const void*)&Animator_Get_Custom_PropLayerCount      ,  //  <- UnityEngine.Animator::get_layerCount
    (const void*)&Animator_CUSTOM_GetLayerName            ,  //  <- UnityEngine.Animator::GetLayerName
    (const void*)&Animator_CUSTOM_GetLayerWeight          ,  //  <- UnityEngine.Animator::GetLayerWeight
    (const void*)&Animator_CUSTOM_SetLayerWeight          ,  //  <- UnityEngine.Animator::SetLayerWeight
    (const void*)&Animator_CUSTOM_GetCurrentAnimatorStateInfo,  //  <- UnityEngine.Animator::GetCurrentAnimatorStateInfo
    (const void*)&Animator_CUSTOM_GetNextAnimatorStateInfo,  //  <- UnityEngine.Animator::GetNextAnimatorStateInfo
    (const void*)&Animator_CUSTOM_GetAnimatorTransitionInfo,  //  <- UnityEngine.Animator::GetAnimatorTransitionInfo
#if !UNITY_FLASH && !PLATFORM_WINRT
    (const void*)&Animator_CUSTOM_GetCurrentAnimationClipState,  //  <- UnityEngine.Animator::GetCurrentAnimationClipState
    (const void*)&Animator_CUSTOM_GetNextAnimationClipState,  //  <- UnityEngine.Animator::GetNextAnimationClipState
#endif
    (const void*)&Animator_CUSTOM_IsInTransition          ,  //  <- UnityEngine.Animator::IsInTransition
    (const void*)&Animator_Get_Custom_PropFeetPivotActive ,  //  <- UnityEngine.Animator::get_feetPivotActive
    (const void*)&Animator_Set_Custom_PropFeetPivotActive ,  //  <- UnityEngine.Animator::set_feetPivotActive
    (const void*)&Animator_Get_Custom_PropPivotWeight     ,  //  <- UnityEngine.Animator::get_pivotWeight
    (const void*)&Animator_CUSTOM_INTERNAL_get_pivotPosition,  //  <- UnityEngine.Animator::INTERNAL_get_pivotPosition
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_MatchTarget,  //  <- UnityEngine.Animator::INTERNAL_CALL_MatchTarget
    (const void*)&Animator_CUSTOM_InterruptMatchTarget    ,  //  <- UnityEngine.Animator::InterruptMatchTarget
    (const void*)&Animator_Get_Custom_PropIsMatchingTarget,  //  <- UnityEngine.Animator::get_isMatchingTarget
    (const void*)&Animator_Get_Custom_PropSpeed           ,  //  <- UnityEngine.Animator::get_speed
    (const void*)&Animator_Set_Custom_PropSpeed           ,  //  <- UnityEngine.Animator::set_speed
    (const void*)&Animator_CUSTOM_ForceStateNormalizedTime,  //  <- UnityEngine.Animator::ForceStateNormalizedTime
    (const void*)&Animator_CUSTOM_SetTarget               ,  //  <- UnityEngine.Animator::SetTarget
    (const void*)&Animator_CUSTOM_INTERNAL_get_targetPosition,  //  <- UnityEngine.Animator::INTERNAL_get_targetPosition
    (const void*)&Animator_CUSTOM_INTERNAL_get_targetRotation,  //  <- UnityEngine.Animator::INTERNAL_get_targetRotation
    (const void*)&Animator_CUSTOM_IsControlled            ,  //  <- UnityEngine.Animator::IsControlled
    (const void*)&Animator_CUSTOM_GetBoneTransform        ,  //  <- UnityEngine.Animator::GetBoneTransform
    (const void*)&Animator_Get_Custom_PropCullingMode     ,  //  <- UnityEngine.Animator::get_cullingMode
    (const void*)&Animator_Set_Custom_PropCullingMode     ,  //  <- UnityEngine.Animator::set_cullingMode
    (const void*)&Animator_CUSTOM_StartPlayback           ,  //  <- UnityEngine.Animator::StartPlayback
    (const void*)&Animator_CUSTOM_StopPlayback            ,  //  <- UnityEngine.Animator::StopPlayback
    (const void*)&Animator_Get_Custom_PropPlaybackTime    ,  //  <- UnityEngine.Animator::get_playbackTime
    (const void*)&Animator_Set_Custom_PropPlaybackTime    ,  //  <- UnityEngine.Animator::set_playbackTime
    (const void*)&Animator_CUSTOM_StartRecording          ,  //  <- UnityEngine.Animator::StartRecording
    (const void*)&Animator_CUSTOM_StopRecording           ,  //  <- UnityEngine.Animator::StopRecording
    (const void*)&Animator_Get_Custom_PropRecorderStartTime,  //  <- UnityEngine.Animator::get_recorderStartTime
    (const void*)&Animator_Set_Custom_PropRecorderStartTime,  //  <- UnityEngine.Animator::set_recorderStartTime
    (const void*)&Animator_Get_Custom_PropRecorderStopTime,  //  <- UnityEngine.Animator::get_recorderStopTime
    (const void*)&Animator_Set_Custom_PropRecorderStopTime,  //  <- UnityEngine.Animator::set_recorderStopTime
    (const void*)&Animator_Get_Custom_PropRuntimeAnimatorController,  //  <- UnityEngine.Animator::get_runtimeAnimatorController
    (const void*)&Animator_Set_Custom_PropRuntimeAnimatorController,  //  <- UnityEngine.Animator::set_runtimeAnimatorController
    (const void*)&Animator_CUSTOM_StringToHash            ,  //  <- UnityEngine.Animator::StringToHash
    (const void*)&Animator_Get_Custom_PropAvatar          ,  //  <- UnityEngine.Animator::get_avatar
    (const void*)&Animator_Set_Custom_PropAvatar          ,  //  <- UnityEngine.Animator::set_avatar
    (const void*)&Animator_CUSTOM_SetFloatString          ,  //  <- UnityEngine.Animator::SetFloatString
    (const void*)&Animator_CUSTOM_SetFloatID              ,  //  <- UnityEngine.Animator::SetFloatID
    (const void*)&Animator_CUSTOM_GetFloatString          ,  //  <- UnityEngine.Animator::GetFloatString
    (const void*)&Animator_CUSTOM_GetFloatID              ,  //  <- UnityEngine.Animator::GetFloatID
    (const void*)&Animator_CUSTOM_SetBoolString           ,  //  <- UnityEngine.Animator::SetBoolString
    (const void*)&Animator_CUSTOM_SetBoolID               ,  //  <- UnityEngine.Animator::SetBoolID
    (const void*)&Animator_CUSTOM_GetBoolString           ,  //  <- UnityEngine.Animator::GetBoolString
    (const void*)&Animator_CUSTOM_GetBoolID               ,  //  <- UnityEngine.Animator::GetBoolID
    (const void*)&Animator_CUSTOM_SetIntegerString        ,  //  <- UnityEngine.Animator::SetIntegerString
    (const void*)&Animator_CUSTOM_SetIntegerID            ,  //  <- UnityEngine.Animator::SetIntegerID
    (const void*)&Animator_CUSTOM_GetIntegerString        ,  //  <- UnityEngine.Animator::GetIntegerString
    (const void*)&Animator_CUSTOM_GetIntegerID            ,  //  <- UnityEngine.Animator::GetIntegerID
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetVectorString,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetVectorString
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetVectorID,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetVectorID
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_GetVectorString,  //  <- UnityEngine.Animator::INTERNAL_CALL_GetVectorString
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_GetVectorID,  //  <- UnityEngine.Animator::INTERNAL_CALL_GetVectorID
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetQuaternionString,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetQuaternionString
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_SetQuaternionID,  //  <- UnityEngine.Animator::INTERNAL_CALL_SetQuaternionID
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_GetQuaternionString,  //  <- UnityEngine.Animator::INTERNAL_CALL_GetQuaternionString
    (const void*)&Animator_CUSTOM_INTERNAL_CALL_GetQuaternionID,  //  <- UnityEngine.Animator::INTERNAL_CALL_GetQuaternionID
    (const void*)&Animator_CUSTOM_IsParameterControlledByCurveString,  //  <- UnityEngine.Animator::IsParameterControlledByCurveString
    (const void*)&Animator_CUSTOM_IsParameterControlledByCurveID,  //  <- UnityEngine.Animator::IsParameterControlledByCurveID
    (const void*)&Animator_CUSTOM_SetFloatStringDamp      ,  //  <- UnityEngine.Animator::SetFloatStringDamp
    (const void*)&Animator_CUSTOM_SetFloatIDDamp          ,  //  <- UnityEngine.Animator::SetFloatIDDamp
    (const void*)&Animator_Get_Custom_PropLayersAffectMassCenter,  //  <- UnityEngine.Animator::get_layersAffectMassCenter
    (const void*)&Animator_Set_Custom_PropLayersAffectMassCenter,  //  <- UnityEngine.Animator::set_layersAffectMassCenter
    (const void*)&Animator_Get_Custom_PropLeftFeetBottomHeight,  //  <- UnityEngine.Animator::get_leftFeetBottomHeight
    (const void*)&Animator_Get_Custom_PropRightFeetBottomHeight,  //  <- UnityEngine.Animator::get_rightFeetBottomHeight
#if UNITY_EDITOR
    (const void*)&Animator_Get_Custom_PropSupportsOnAnimatorMove,  //  <- UnityEngine.Animator::get_supportsOnAnimatorMove
    (const void*)&Animator_CUSTOM_WriteDefaultPose        ,  //  <- UnityEngine.Animator::WriteDefaultPose
    (const void*)&Animator_CUSTOM_Update                  ,  //  <- UnityEngine.Animator::Update
    (const void*)&Animator_CUSTOM_EvaluateSM              ,  //  <- UnityEngine.Animator::EvaluateSM
    (const void*)&Animator_CUSTOM_GetCurrentStateName     ,  //  <- UnityEngine.Animator::GetCurrentStateName
    (const void*)&Animator_CUSTOM_GetNextStateName        ,  //  <- UnityEngine.Animator::GetNextStateName
#endif
    (const void*)&Animator_Get_Custom_PropIsInManagerList ,  //  <- UnityEngine.Animator::get_isInManagerList
    (const void*)&Animator_Get_Custom_PropLogWarnings     ,  //  <- UnityEngine.Animator::get_logWarnings
    (const void*)&Animator_Set_Custom_PropLogWarnings     ,  //  <- UnityEngine.Animator::set_logWarnings
    NULL
};

void ExportAnimatorBindings();
void ExportAnimatorBindings()
{
    for (int i = 0; s_Animator_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Animator_IcallNames [i], s_Animator_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportAnimatorBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Animator_Get_Custom_PropIsHuman); //  <- UnityEngine.Animator::get_isHuman
    SET_METRO_BINDING(Animator_Get_Custom_PropHumanScale); //  <- UnityEngine.Animator::get_humanScale
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_deltaPosition); //  <- UnityEngine.Animator::INTERNAL_get_deltaPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_deltaRotation); //  <- UnityEngine.Animator::INTERNAL_get_deltaRotation
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_rootPosition); //  <- UnityEngine.Animator::INTERNAL_get_rootPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_set_rootPosition); //  <- UnityEngine.Animator::INTERNAL_set_rootPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_rootRotation); //  <- UnityEngine.Animator::INTERNAL_get_rootRotation
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_set_rootRotation); //  <- UnityEngine.Animator::INTERNAL_set_rootRotation
    SET_METRO_BINDING(Animator_Get_Custom_PropApplyRootMotion); //  <- UnityEngine.Animator::get_applyRootMotion
    SET_METRO_BINDING(Animator_Set_Custom_PropApplyRootMotion); //  <- UnityEngine.Animator::set_applyRootMotion
    SET_METRO_BINDING(Animator_Get_Custom_PropAnimatePhysics); //  <- UnityEngine.Animator::get_animatePhysics
    SET_METRO_BINDING(Animator_Set_Custom_PropAnimatePhysics); //  <- UnityEngine.Animator::set_animatePhysics
    SET_METRO_BINDING(Animator_Get_Custom_PropGravityWeight); //  <- UnityEngine.Animator::get_gravityWeight
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_bodyPosition); //  <- UnityEngine.Animator::INTERNAL_get_bodyPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_set_bodyPosition); //  <- UnityEngine.Animator::INTERNAL_set_bodyPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_bodyRotation); //  <- UnityEngine.Animator::INTERNAL_get_bodyRotation
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_set_bodyRotation); //  <- UnityEngine.Animator::INTERNAL_set_bodyRotation
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_GetIKPosition); //  <- UnityEngine.Animator::INTERNAL_CALL_GetIKPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetIKPosition); //  <- UnityEngine.Animator::INTERNAL_CALL_SetIKPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_GetIKRotation); //  <- UnityEngine.Animator::INTERNAL_CALL_GetIKRotation
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetIKRotation); //  <- UnityEngine.Animator::INTERNAL_CALL_SetIKRotation
    SET_METRO_BINDING(Animator_CUSTOM_GetIKPositionWeight); //  <- UnityEngine.Animator::GetIKPositionWeight
    SET_METRO_BINDING(Animator_CUSTOM_SetIKPositionWeight); //  <- UnityEngine.Animator::SetIKPositionWeight
    SET_METRO_BINDING(Animator_CUSTOM_GetIKRotationWeight); //  <- UnityEngine.Animator::GetIKRotationWeight
    SET_METRO_BINDING(Animator_CUSTOM_SetIKRotationWeight); //  <- UnityEngine.Animator::SetIKRotationWeight
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetLookAtPosition); //  <- UnityEngine.Animator::INTERNAL_CALL_SetLookAtPosition
    SET_METRO_BINDING(Animator_CUSTOM_SetLookAtWeight); //  <- UnityEngine.Animator::SetLookAtWeight
    SET_METRO_BINDING(Animator_Get_Custom_PropStabilizeFeet); //  <- UnityEngine.Animator::get_stabilizeFeet
    SET_METRO_BINDING(Animator_Set_Custom_PropStabilizeFeet); //  <- UnityEngine.Animator::set_stabilizeFeet
    SET_METRO_BINDING(Animator_Get_Custom_PropLayerCount); //  <- UnityEngine.Animator::get_layerCount
    SET_METRO_BINDING(Animator_CUSTOM_GetLayerName); //  <- UnityEngine.Animator::GetLayerName
    SET_METRO_BINDING(Animator_CUSTOM_GetLayerWeight); //  <- UnityEngine.Animator::GetLayerWeight
    SET_METRO_BINDING(Animator_CUSTOM_SetLayerWeight); //  <- UnityEngine.Animator::SetLayerWeight
    SET_METRO_BINDING(Animator_CUSTOM_GetCurrentAnimatorStateInfo); //  <- UnityEngine.Animator::GetCurrentAnimatorStateInfo
    SET_METRO_BINDING(Animator_CUSTOM_GetNextAnimatorStateInfo); //  <- UnityEngine.Animator::GetNextAnimatorStateInfo
    SET_METRO_BINDING(Animator_CUSTOM_GetAnimatorTransitionInfo); //  <- UnityEngine.Animator::GetAnimatorTransitionInfo
#if !UNITY_FLASH && !PLATFORM_WINRT
    SET_METRO_BINDING(Animator_CUSTOM_GetCurrentAnimationClipState); //  <- UnityEngine.Animator::GetCurrentAnimationClipState
    SET_METRO_BINDING(Animator_CUSTOM_GetNextAnimationClipState); //  <- UnityEngine.Animator::GetNextAnimationClipState
#endif
    SET_METRO_BINDING(Animator_CUSTOM_IsInTransition); //  <- UnityEngine.Animator::IsInTransition
    SET_METRO_BINDING(Animator_Get_Custom_PropFeetPivotActive); //  <- UnityEngine.Animator::get_feetPivotActive
    SET_METRO_BINDING(Animator_Set_Custom_PropFeetPivotActive); //  <- UnityEngine.Animator::set_feetPivotActive
    SET_METRO_BINDING(Animator_Get_Custom_PropPivotWeight); //  <- UnityEngine.Animator::get_pivotWeight
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_pivotPosition); //  <- UnityEngine.Animator::INTERNAL_get_pivotPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_MatchTarget); //  <- UnityEngine.Animator::INTERNAL_CALL_MatchTarget
    SET_METRO_BINDING(Animator_CUSTOM_InterruptMatchTarget); //  <- UnityEngine.Animator::InterruptMatchTarget
    SET_METRO_BINDING(Animator_Get_Custom_PropIsMatchingTarget); //  <- UnityEngine.Animator::get_isMatchingTarget
    SET_METRO_BINDING(Animator_Get_Custom_PropSpeed); //  <- UnityEngine.Animator::get_speed
    SET_METRO_BINDING(Animator_Set_Custom_PropSpeed); //  <- UnityEngine.Animator::set_speed
    SET_METRO_BINDING(Animator_CUSTOM_ForceStateNormalizedTime); //  <- UnityEngine.Animator::ForceStateNormalizedTime
    SET_METRO_BINDING(Animator_CUSTOM_SetTarget); //  <- UnityEngine.Animator::SetTarget
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_targetPosition); //  <- UnityEngine.Animator::INTERNAL_get_targetPosition
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_get_targetRotation); //  <- UnityEngine.Animator::INTERNAL_get_targetRotation
    SET_METRO_BINDING(Animator_CUSTOM_IsControlled); //  <- UnityEngine.Animator::IsControlled
    SET_METRO_BINDING(Animator_CUSTOM_GetBoneTransform); //  <- UnityEngine.Animator::GetBoneTransform
    SET_METRO_BINDING(Animator_Get_Custom_PropCullingMode); //  <- UnityEngine.Animator::get_cullingMode
    SET_METRO_BINDING(Animator_Set_Custom_PropCullingMode); //  <- UnityEngine.Animator::set_cullingMode
    SET_METRO_BINDING(Animator_CUSTOM_StartPlayback); //  <- UnityEngine.Animator::StartPlayback
    SET_METRO_BINDING(Animator_CUSTOM_StopPlayback); //  <- UnityEngine.Animator::StopPlayback
    SET_METRO_BINDING(Animator_Get_Custom_PropPlaybackTime); //  <- UnityEngine.Animator::get_playbackTime
    SET_METRO_BINDING(Animator_Set_Custom_PropPlaybackTime); //  <- UnityEngine.Animator::set_playbackTime
    SET_METRO_BINDING(Animator_CUSTOM_StartRecording); //  <- UnityEngine.Animator::StartRecording
    SET_METRO_BINDING(Animator_CUSTOM_StopRecording); //  <- UnityEngine.Animator::StopRecording
    SET_METRO_BINDING(Animator_Get_Custom_PropRecorderStartTime); //  <- UnityEngine.Animator::get_recorderStartTime
    SET_METRO_BINDING(Animator_Set_Custom_PropRecorderStartTime); //  <- UnityEngine.Animator::set_recorderStartTime
    SET_METRO_BINDING(Animator_Get_Custom_PropRecorderStopTime); //  <- UnityEngine.Animator::get_recorderStopTime
    SET_METRO_BINDING(Animator_Set_Custom_PropRecorderStopTime); //  <- UnityEngine.Animator::set_recorderStopTime
    SET_METRO_BINDING(Animator_Get_Custom_PropRuntimeAnimatorController); //  <- UnityEngine.Animator::get_runtimeAnimatorController
    SET_METRO_BINDING(Animator_Set_Custom_PropRuntimeAnimatorController); //  <- UnityEngine.Animator::set_runtimeAnimatorController
    SET_METRO_BINDING(Animator_CUSTOM_StringToHash); //  <- UnityEngine.Animator::StringToHash
    SET_METRO_BINDING(Animator_Get_Custom_PropAvatar); //  <- UnityEngine.Animator::get_avatar
    SET_METRO_BINDING(Animator_Set_Custom_PropAvatar); //  <- UnityEngine.Animator::set_avatar
    SET_METRO_BINDING(Animator_CUSTOM_SetFloatString); //  <- UnityEngine.Animator::SetFloatString
    SET_METRO_BINDING(Animator_CUSTOM_SetFloatID); //  <- UnityEngine.Animator::SetFloatID
    SET_METRO_BINDING(Animator_CUSTOM_GetFloatString); //  <- UnityEngine.Animator::GetFloatString
    SET_METRO_BINDING(Animator_CUSTOM_GetFloatID); //  <- UnityEngine.Animator::GetFloatID
    SET_METRO_BINDING(Animator_CUSTOM_SetBoolString); //  <- UnityEngine.Animator::SetBoolString
    SET_METRO_BINDING(Animator_CUSTOM_SetBoolID); //  <- UnityEngine.Animator::SetBoolID
    SET_METRO_BINDING(Animator_CUSTOM_GetBoolString); //  <- UnityEngine.Animator::GetBoolString
    SET_METRO_BINDING(Animator_CUSTOM_GetBoolID); //  <- UnityEngine.Animator::GetBoolID
    SET_METRO_BINDING(Animator_CUSTOM_SetIntegerString); //  <- UnityEngine.Animator::SetIntegerString
    SET_METRO_BINDING(Animator_CUSTOM_SetIntegerID); //  <- UnityEngine.Animator::SetIntegerID
    SET_METRO_BINDING(Animator_CUSTOM_GetIntegerString); //  <- UnityEngine.Animator::GetIntegerString
    SET_METRO_BINDING(Animator_CUSTOM_GetIntegerID); //  <- UnityEngine.Animator::GetIntegerID
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetVectorString); //  <- UnityEngine.Animator::INTERNAL_CALL_SetVectorString
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetVectorID); //  <- UnityEngine.Animator::INTERNAL_CALL_SetVectorID
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_GetVectorString); //  <- UnityEngine.Animator::INTERNAL_CALL_GetVectorString
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_GetVectorID); //  <- UnityEngine.Animator::INTERNAL_CALL_GetVectorID
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetQuaternionString); //  <- UnityEngine.Animator::INTERNAL_CALL_SetQuaternionString
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_SetQuaternionID); //  <- UnityEngine.Animator::INTERNAL_CALL_SetQuaternionID
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_GetQuaternionString); //  <- UnityEngine.Animator::INTERNAL_CALL_GetQuaternionString
    SET_METRO_BINDING(Animator_CUSTOM_INTERNAL_CALL_GetQuaternionID); //  <- UnityEngine.Animator::INTERNAL_CALL_GetQuaternionID
    SET_METRO_BINDING(Animator_CUSTOM_IsParameterControlledByCurveString); //  <- UnityEngine.Animator::IsParameterControlledByCurveString
    SET_METRO_BINDING(Animator_CUSTOM_IsParameterControlledByCurveID); //  <- UnityEngine.Animator::IsParameterControlledByCurveID
    SET_METRO_BINDING(Animator_CUSTOM_SetFloatStringDamp); //  <- UnityEngine.Animator::SetFloatStringDamp
    SET_METRO_BINDING(Animator_CUSTOM_SetFloatIDDamp); //  <- UnityEngine.Animator::SetFloatIDDamp
    SET_METRO_BINDING(Animator_Get_Custom_PropLayersAffectMassCenter); //  <- UnityEngine.Animator::get_layersAffectMassCenter
    SET_METRO_BINDING(Animator_Set_Custom_PropLayersAffectMassCenter); //  <- UnityEngine.Animator::set_layersAffectMassCenter
    SET_METRO_BINDING(Animator_Get_Custom_PropLeftFeetBottomHeight); //  <- UnityEngine.Animator::get_leftFeetBottomHeight
    SET_METRO_BINDING(Animator_Get_Custom_PropRightFeetBottomHeight); //  <- UnityEngine.Animator::get_rightFeetBottomHeight
#if UNITY_EDITOR
    SET_METRO_BINDING(Animator_Get_Custom_PropSupportsOnAnimatorMove); //  <- UnityEngine.Animator::get_supportsOnAnimatorMove
    SET_METRO_BINDING(Animator_CUSTOM_WriteDefaultPose); //  <- UnityEngine.Animator::WriteDefaultPose
    SET_METRO_BINDING(Animator_CUSTOM_Update); //  <- UnityEngine.Animator::Update
    SET_METRO_BINDING(Animator_CUSTOM_EvaluateSM); //  <- UnityEngine.Animator::EvaluateSM
    SET_METRO_BINDING(Animator_CUSTOM_GetCurrentStateName); //  <- UnityEngine.Animator::GetCurrentStateName
    SET_METRO_BINDING(Animator_CUSTOM_GetNextStateName); //  <- UnityEngine.Animator::GetNextStateName
#endif
    SET_METRO_BINDING(Animator_Get_Custom_PropIsInManagerList); //  <- UnityEngine.Animator::get_isInManagerList
    SET_METRO_BINDING(Animator_Get_Custom_PropLogWarnings); //  <- UnityEngine.Animator::get_logWarnings
    SET_METRO_BINDING(Animator_Set_Custom_PropLogWarnings); //  <- UnityEngine.Animator::set_logWarnings
}

#endif
