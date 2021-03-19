#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoManager.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/NavMesh/NavMesh.h"
#include "Runtime/NavMesh/NavMeshAgent.h"
#include "Runtime/NavMesh/OffMeshLink.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_CALL_SetDestination(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& target)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_CALL_SetDestination)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetDestination)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetDestination)
    
    		return self->SetDestination (target);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_get_destination(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_get_destination)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_destination)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_destination)
    *returnValue = self->GetDestination();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_set_destination(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_set_destination)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_destination)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_destination)
    
    self->SetDestination (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropStoppingDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropStoppingDistance)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stoppingDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stoppingDistance)
    return self->GetStoppingDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropStoppingDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropStoppingDistance)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stoppingDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stoppingDistance)
    
    self->SetStoppingDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_get_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_get_velocity)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_velocity)
    *returnValue = self->GetVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_set_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_set_velocity)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_velocity)
    
    self->SetVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_get_nextPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_get_nextPosition)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_nextPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_nextPosition)
    *returnValue = self->GetNextPosition();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_set_nextPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_set_nextPosition)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_nextPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_nextPosition)
    
    self->SetInternalAgentPosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_get_steeringTarget(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_get_steeringTarget)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_steeringTarget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_steeringTarget)
    *returnValue = self->GetNextCorner();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_get_desiredVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_get_desiredVelocity)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_desiredVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_desiredVelocity)
    *returnValue = self->GetDesiredVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropRemainingDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropRemainingDistance)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_remainingDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_remainingDistance)
    return self->GetRemainingDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropBaseOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropBaseOffset)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_baseOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_baseOffset)
    return self->GetBaseOffset ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropBaseOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropBaseOffset)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_baseOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_baseOffset)
    
    self->SetBaseOffset (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropIsOnOffMeshLink(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropIsOnOffMeshLink)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isOnOffMeshLink)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isOnOffMeshLink)
    return self->IsOnOffMeshLink ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_ActivateCurrentOffMeshLink(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool activated)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_ActivateCurrentOffMeshLink)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ActivateCurrentOffMeshLink)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ActivateCurrentOffMeshLink)
    
    		return self->ActivateCurrentOffMeshLink (activated);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
OffMeshLinkData SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_GetCurrentOffMeshLinkDataInternal(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_GetCurrentOffMeshLinkDataInternal)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetCurrentOffMeshLinkDataInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetCurrentOffMeshLinkDataInternal)
    
    		OffMeshLinkData data;
    		self->GetCurrentOffMeshLinkData (&data);
    		return data;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
OffMeshLinkData SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_GetNextOffMeshLinkDataInternal(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_GetNextOffMeshLinkDataInternal)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetNextOffMeshLinkDataInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNextOffMeshLinkDataInternal)
    
    		OffMeshLinkData data;
    		self->GetNextOffMeshLinkData (&data);
    		return data;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_CompleteOffMeshLink(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_CompleteOffMeshLink)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(CompleteOffMeshLink)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CompleteOffMeshLink)
    
    		return self->CompleteOffMeshLink ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropAutoTraverseOffMeshLink(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropAutoTraverseOffMeshLink)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_autoTraverseOffMeshLink)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autoTraverseOffMeshLink)
    return self->GetAutoTraverseOffMeshLink ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropAutoTraverseOffMeshLink(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropAutoTraverseOffMeshLink)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_autoTraverseOffMeshLink)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autoTraverseOffMeshLink)
    
    self->SetAutoTraverseOffMeshLink (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropAutoBraking(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropAutoBraking)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_autoBraking)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autoBraking)
    return self->GetAutoBraking ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropAutoBraking(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropAutoBraking)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_autoBraking)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autoBraking)
    
    self->SetAutoBraking (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropAutoRepath(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropAutoRepath)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_autoRepath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autoRepath)
    return self->GetAutoRepath ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropAutoRepath(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropAutoRepath)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_autoRepath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autoRepath)
    
    self->SetAutoRepath (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropHasPath(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropHasPath)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hasPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hasPath)
    return self->HasPath ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropPathPending(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropPathPending)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pathPending)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pathPending)
    return self->PathPending ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropIsPathStale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropIsPathStale)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPathStale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPathStale)
    return self->IsPathStale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
NavMeshPathStatus SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropPathStatus(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropPathStatus)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pathStatus)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pathStatus)
    return self->GetPathStatus ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_get_pathEndPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_get_pathEndPosition)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_pathEndPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_pathEndPosition)
    *returnValue = self->GetEndPositionOfCurrentPath();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_CALL_Warp(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& newPosition)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_CALL_Warp)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Warp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Warp)
    
    		return self->Warp(newPosition);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_CALL_Move(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& offset)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_CALL_Move)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Move)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Move)
    
    		return self->Move (offset);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_Stop(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool stopUpdates)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_Stop)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Stop)
    
    		return self->Stop (stopUpdates);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_Resume(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_Resume)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Resume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Resume)
    
    		return self->Resume ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_ResetPath(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_ResetPath)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ResetPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ResetPath)
    
    		return self->ResetPath ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_SetPath(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_SetPath)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    ICallType_Object_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(SetPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPath)
    
    		MonoRaiseIfNull (path);
    		MonoNavMeshPath monopath;
    		MarshallManagedStructIntoNative (path, &monopath);
    		return self->SetPath (monopath.native);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_CopyPathTo(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_CopyPathTo)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    ICallType_Object_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(CopyPathTo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CopyPathTo)
    
    		MonoRaiseIfNull (path);
    		MonoNavMeshPath monopath;
    		MarshallManagedStructIntoNative (path, &monopath);
    		self->CopyPath (monopath.native);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_FindClosestEdge(ICallType_ReadOnlyUnityEngineObject_Argument self_, NavMeshHit* hit)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_FindClosestEdge)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(FindClosestEdge)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindClosestEdge)
    
    		return self->DistanceToEdge (hit);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_CALL_Raycast(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& targetPosition, NavMeshHit* hit)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_CALL_Raycast)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Raycast)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Raycast)
    
    		return self->Raycast (targetPosition, hit);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_INTERNAL_CALL_CalculatePathInternal(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& targetPosition, ICallType_Object_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_INTERNAL_CALL_CalculatePathInternal)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    ICallType_Object_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CalculatePathInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CalculatePathInternal)
    
    		MonoNavMeshPath monopath;
    		MarshallManagedStructIntoNative (path, &monopath);
    		int actualSize = self->CalculatePolygonPath (targetPosition, monopath.native);
    		return actualSize>0;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_SamplePathPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, int passableMask, float maxDistance, NavMeshHit* hit)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_SamplePathPosition)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SamplePathPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SamplePathPosition)
    
    		return self->SamplePathPosition (passableMask, maxDistance, hit);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_SetLayerCost(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layer, float cost)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_SetLayerCost)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetLayerCost)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLayerCost)
    
    		self->SetLayerCost (layer, cost);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_CUSTOM_GetLayerCost(ICallType_ReadOnlyUnityEngineObject_Argument self_, int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_CUSTOM_GetLayerCost)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetLayerCost)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetLayerCost)
    
    		return self->GetLayerCost (layer);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropWalkableMask(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropWalkableMask)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_walkableMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_walkableMask)
    return self->GetWalkableMask ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropWalkableMask(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropWalkableMask)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_walkableMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_walkableMask)
    
    self->SetWalkableMask (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropSpeed)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_speed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_speed)
    return self->GetSpeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropSpeed)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_speed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_speed)
    
    self->SetSpeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropAngularSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropAngularSpeed)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angularSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angularSpeed)
    return self->GetAngularSpeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropAngularSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropAngularSpeed)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_angularSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_angularSpeed)
    
    self->SetAngularSpeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropAcceleration(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropAcceleration)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_acceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_acceleration)
    return self->GetAcceleration ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropAcceleration(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropAcceleration)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_acceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_acceleration)
    
    self->SetAcceleration (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropUpdatePosition(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropUpdatePosition)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_updatePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_updatePosition)
    return self->GetUpdatePosition ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropUpdatePosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropUpdatePosition)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_updatePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_updatePosition)
    
    self->SetUpdatePosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropUpdateRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropUpdateRotation)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_updateRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_updateRotation)
    return self->GetUpdateRotation ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropUpdateRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropUpdateRotation)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_updateRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_updateRotation)
    
    self->SetUpdateRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_height)
    return self->GetHeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_height)
    
    self->SetHeight (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ObstacleAvoidanceType SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropObstacleAvoidanceType(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropObstacleAvoidanceType)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_obstacleAvoidanceType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_obstacleAvoidanceType)
    return self->GetObstacleAvoidanceType ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropObstacleAvoidanceType(ICallType_ReadOnlyUnityEngineObject_Argument self_, ObstacleAvoidanceType value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropObstacleAvoidanceType)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_obstacleAvoidanceType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_obstacleAvoidanceType)
    
    self->SetObstacleAvoidanceType (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NavMeshAgent_Get_Custom_PropAvoidancePriority(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Get_Custom_PropAvoidancePriority)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_avoidancePriority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_avoidancePriority)
    return self->GetAvoidancePriority ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshAgent_Set_Custom_PropAvoidancePriority(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshAgent_Set_Custom_PropAvoidancePriority)
    ReadOnlyScriptingObjectOfType<NavMeshAgent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_avoidancePriority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_avoidancePriority)
    
    self->SetAvoidancePriority (value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_NavMeshAgent_INTERNAL_CALL_SetDestination()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_CALL_SetDestination" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_CALL_SetDestination );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_get_destination()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_get_destination" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_get_destination );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_set_destination()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_set_destination" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_set_destination );
}

void Register_UnityEngine_NavMeshAgent_get_stoppingDistance()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_stoppingDistance" , (gpointer)& NavMeshAgent_Get_Custom_PropStoppingDistance );
}

void Register_UnityEngine_NavMeshAgent_set_stoppingDistance()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_stoppingDistance" , (gpointer)& NavMeshAgent_Set_Custom_PropStoppingDistance );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_get_velocity()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_get_velocity" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_get_velocity );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_set_velocity()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_set_velocity" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_set_velocity );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_get_nextPosition()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_get_nextPosition" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_get_nextPosition );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_set_nextPosition()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_set_nextPosition" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_set_nextPosition );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_get_steeringTarget()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_get_steeringTarget" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_get_steeringTarget );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_get_desiredVelocity()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_get_desiredVelocity" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_get_desiredVelocity );
}

void Register_UnityEngine_NavMeshAgent_get_remainingDistance()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_remainingDistance" , (gpointer)& NavMeshAgent_Get_Custom_PropRemainingDistance );
}

void Register_UnityEngine_NavMeshAgent_get_baseOffset()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_baseOffset" , (gpointer)& NavMeshAgent_Get_Custom_PropBaseOffset );
}

void Register_UnityEngine_NavMeshAgent_set_baseOffset()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_baseOffset" , (gpointer)& NavMeshAgent_Set_Custom_PropBaseOffset );
}

void Register_UnityEngine_NavMeshAgent_get_isOnOffMeshLink()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_isOnOffMeshLink" , (gpointer)& NavMeshAgent_Get_Custom_PropIsOnOffMeshLink );
}

void Register_UnityEngine_NavMeshAgent_ActivateCurrentOffMeshLink()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::ActivateCurrentOffMeshLink" , (gpointer)& NavMeshAgent_CUSTOM_ActivateCurrentOffMeshLink );
}

void Register_UnityEngine_NavMeshAgent_GetCurrentOffMeshLinkDataInternal()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::GetCurrentOffMeshLinkDataInternal" , (gpointer)& NavMeshAgent_CUSTOM_GetCurrentOffMeshLinkDataInternal );
}

void Register_UnityEngine_NavMeshAgent_GetNextOffMeshLinkDataInternal()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::GetNextOffMeshLinkDataInternal" , (gpointer)& NavMeshAgent_CUSTOM_GetNextOffMeshLinkDataInternal );
}

void Register_UnityEngine_NavMeshAgent_CompleteOffMeshLink()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::CompleteOffMeshLink" , (gpointer)& NavMeshAgent_CUSTOM_CompleteOffMeshLink );
}

void Register_UnityEngine_NavMeshAgent_get_autoTraverseOffMeshLink()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_autoTraverseOffMeshLink" , (gpointer)& NavMeshAgent_Get_Custom_PropAutoTraverseOffMeshLink );
}

void Register_UnityEngine_NavMeshAgent_set_autoTraverseOffMeshLink()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_autoTraverseOffMeshLink" , (gpointer)& NavMeshAgent_Set_Custom_PropAutoTraverseOffMeshLink );
}

void Register_UnityEngine_NavMeshAgent_get_autoBraking()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_autoBraking" , (gpointer)& NavMeshAgent_Get_Custom_PropAutoBraking );
}

void Register_UnityEngine_NavMeshAgent_set_autoBraking()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_autoBraking" , (gpointer)& NavMeshAgent_Set_Custom_PropAutoBraking );
}

void Register_UnityEngine_NavMeshAgent_get_autoRepath()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_autoRepath" , (gpointer)& NavMeshAgent_Get_Custom_PropAutoRepath );
}

void Register_UnityEngine_NavMeshAgent_set_autoRepath()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_autoRepath" , (gpointer)& NavMeshAgent_Set_Custom_PropAutoRepath );
}

void Register_UnityEngine_NavMeshAgent_get_hasPath()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_hasPath" , (gpointer)& NavMeshAgent_Get_Custom_PropHasPath );
}

void Register_UnityEngine_NavMeshAgent_get_pathPending()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_pathPending" , (gpointer)& NavMeshAgent_Get_Custom_PropPathPending );
}

void Register_UnityEngine_NavMeshAgent_get_isPathStale()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_isPathStale" , (gpointer)& NavMeshAgent_Get_Custom_PropIsPathStale );
}

void Register_UnityEngine_NavMeshAgent_get_pathStatus()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_pathStatus" , (gpointer)& NavMeshAgent_Get_Custom_PropPathStatus );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_get_pathEndPosition()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_get_pathEndPosition" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_get_pathEndPosition );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_CALL_Warp()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_CALL_Warp" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_CALL_Warp );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_CALL_Move()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_CALL_Move" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_CALL_Move );
}

void Register_UnityEngine_NavMeshAgent_Stop()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::Stop" , (gpointer)& NavMeshAgent_CUSTOM_Stop );
}

void Register_UnityEngine_NavMeshAgent_Resume()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::Resume" , (gpointer)& NavMeshAgent_CUSTOM_Resume );
}

void Register_UnityEngine_NavMeshAgent_ResetPath()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::ResetPath" , (gpointer)& NavMeshAgent_CUSTOM_ResetPath );
}

void Register_UnityEngine_NavMeshAgent_SetPath()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::SetPath" , (gpointer)& NavMeshAgent_CUSTOM_SetPath );
}

void Register_UnityEngine_NavMeshAgent_CopyPathTo()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::CopyPathTo" , (gpointer)& NavMeshAgent_CUSTOM_CopyPathTo );
}

void Register_UnityEngine_NavMeshAgent_FindClosestEdge()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::FindClosestEdge" , (gpointer)& NavMeshAgent_CUSTOM_FindClosestEdge );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_CALL_Raycast()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_CALL_Raycast" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_CALL_Raycast );
}

void Register_UnityEngine_NavMeshAgent_INTERNAL_CALL_CalculatePathInternal()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::INTERNAL_CALL_CalculatePathInternal" , (gpointer)& NavMeshAgent_CUSTOM_INTERNAL_CALL_CalculatePathInternal );
}

void Register_UnityEngine_NavMeshAgent_SamplePathPosition()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::SamplePathPosition" , (gpointer)& NavMeshAgent_CUSTOM_SamplePathPosition );
}

void Register_UnityEngine_NavMeshAgent_SetLayerCost()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::SetLayerCost" , (gpointer)& NavMeshAgent_CUSTOM_SetLayerCost );
}

void Register_UnityEngine_NavMeshAgent_GetLayerCost()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::GetLayerCost" , (gpointer)& NavMeshAgent_CUSTOM_GetLayerCost );
}

void Register_UnityEngine_NavMeshAgent_get_walkableMask()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_walkableMask" , (gpointer)& NavMeshAgent_Get_Custom_PropWalkableMask );
}

void Register_UnityEngine_NavMeshAgent_set_walkableMask()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_walkableMask" , (gpointer)& NavMeshAgent_Set_Custom_PropWalkableMask );
}

void Register_UnityEngine_NavMeshAgent_get_speed()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_speed" , (gpointer)& NavMeshAgent_Get_Custom_PropSpeed );
}

void Register_UnityEngine_NavMeshAgent_set_speed()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_speed" , (gpointer)& NavMeshAgent_Set_Custom_PropSpeed );
}

void Register_UnityEngine_NavMeshAgent_get_angularSpeed()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_angularSpeed" , (gpointer)& NavMeshAgent_Get_Custom_PropAngularSpeed );
}

void Register_UnityEngine_NavMeshAgent_set_angularSpeed()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_angularSpeed" , (gpointer)& NavMeshAgent_Set_Custom_PropAngularSpeed );
}

void Register_UnityEngine_NavMeshAgent_get_acceleration()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_acceleration" , (gpointer)& NavMeshAgent_Get_Custom_PropAcceleration );
}

void Register_UnityEngine_NavMeshAgent_set_acceleration()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_acceleration" , (gpointer)& NavMeshAgent_Set_Custom_PropAcceleration );
}

void Register_UnityEngine_NavMeshAgent_get_updatePosition()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_updatePosition" , (gpointer)& NavMeshAgent_Get_Custom_PropUpdatePosition );
}

void Register_UnityEngine_NavMeshAgent_set_updatePosition()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_updatePosition" , (gpointer)& NavMeshAgent_Set_Custom_PropUpdatePosition );
}

void Register_UnityEngine_NavMeshAgent_get_updateRotation()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_updateRotation" , (gpointer)& NavMeshAgent_Get_Custom_PropUpdateRotation );
}

void Register_UnityEngine_NavMeshAgent_set_updateRotation()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_updateRotation" , (gpointer)& NavMeshAgent_Set_Custom_PropUpdateRotation );
}

void Register_UnityEngine_NavMeshAgent_get_radius()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_radius" , (gpointer)& NavMeshAgent_Get_Custom_PropRadius );
}

void Register_UnityEngine_NavMeshAgent_set_radius()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_radius" , (gpointer)& NavMeshAgent_Set_Custom_PropRadius );
}

void Register_UnityEngine_NavMeshAgent_get_height()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_height" , (gpointer)& NavMeshAgent_Get_Custom_PropHeight );
}

void Register_UnityEngine_NavMeshAgent_set_height()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_height" , (gpointer)& NavMeshAgent_Set_Custom_PropHeight );
}

void Register_UnityEngine_NavMeshAgent_get_obstacleAvoidanceType()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_obstacleAvoidanceType" , (gpointer)& NavMeshAgent_Get_Custom_PropObstacleAvoidanceType );
}

void Register_UnityEngine_NavMeshAgent_set_obstacleAvoidanceType()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_obstacleAvoidanceType" , (gpointer)& NavMeshAgent_Set_Custom_PropObstacleAvoidanceType );
}

void Register_UnityEngine_NavMeshAgent_get_avoidancePriority()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::get_avoidancePriority" , (gpointer)& NavMeshAgent_Get_Custom_PropAvoidancePriority );
}

void Register_UnityEngine_NavMeshAgent_set_avoidancePriority()
{
    scripting_add_internal_call( "UnityEngine.NavMeshAgent::set_avoidancePriority" , (gpointer)& NavMeshAgent_Set_Custom_PropAvoidancePriority );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_NavMeshAgent_IcallNames [] =
{
    "UnityEngine.NavMeshAgent::INTERNAL_CALL_SetDestination",    // -> NavMeshAgent_CUSTOM_INTERNAL_CALL_SetDestination
    "UnityEngine.NavMeshAgent::INTERNAL_get_destination",    // -> NavMeshAgent_CUSTOM_INTERNAL_get_destination
    "UnityEngine.NavMeshAgent::INTERNAL_set_destination",    // -> NavMeshAgent_CUSTOM_INTERNAL_set_destination
    "UnityEngine.NavMeshAgent::get_stoppingDistance",    // -> NavMeshAgent_Get_Custom_PropStoppingDistance
    "UnityEngine.NavMeshAgent::set_stoppingDistance",    // -> NavMeshAgent_Set_Custom_PropStoppingDistance
    "UnityEngine.NavMeshAgent::INTERNAL_get_velocity",    // -> NavMeshAgent_CUSTOM_INTERNAL_get_velocity
    "UnityEngine.NavMeshAgent::INTERNAL_set_velocity",    // -> NavMeshAgent_CUSTOM_INTERNAL_set_velocity
    "UnityEngine.NavMeshAgent::INTERNAL_get_nextPosition",    // -> NavMeshAgent_CUSTOM_INTERNAL_get_nextPosition
    "UnityEngine.NavMeshAgent::INTERNAL_set_nextPosition",    // -> NavMeshAgent_CUSTOM_INTERNAL_set_nextPosition
    "UnityEngine.NavMeshAgent::INTERNAL_get_steeringTarget",    // -> NavMeshAgent_CUSTOM_INTERNAL_get_steeringTarget
    "UnityEngine.NavMeshAgent::INTERNAL_get_desiredVelocity",    // -> NavMeshAgent_CUSTOM_INTERNAL_get_desiredVelocity
    "UnityEngine.NavMeshAgent::get_remainingDistance",    // -> NavMeshAgent_Get_Custom_PropRemainingDistance
    "UnityEngine.NavMeshAgent::get_baseOffset",    // -> NavMeshAgent_Get_Custom_PropBaseOffset
    "UnityEngine.NavMeshAgent::set_baseOffset",    // -> NavMeshAgent_Set_Custom_PropBaseOffset
    "UnityEngine.NavMeshAgent::get_isOnOffMeshLink",    // -> NavMeshAgent_Get_Custom_PropIsOnOffMeshLink
    "UnityEngine.NavMeshAgent::ActivateCurrentOffMeshLink",    // -> NavMeshAgent_CUSTOM_ActivateCurrentOffMeshLink
    "UnityEngine.NavMeshAgent::GetCurrentOffMeshLinkDataInternal",    // -> NavMeshAgent_CUSTOM_GetCurrentOffMeshLinkDataInternal
    "UnityEngine.NavMeshAgent::GetNextOffMeshLinkDataInternal",    // -> NavMeshAgent_CUSTOM_GetNextOffMeshLinkDataInternal
    "UnityEngine.NavMeshAgent::CompleteOffMeshLink",    // -> NavMeshAgent_CUSTOM_CompleteOffMeshLink
    "UnityEngine.NavMeshAgent::get_autoTraverseOffMeshLink",    // -> NavMeshAgent_Get_Custom_PropAutoTraverseOffMeshLink
    "UnityEngine.NavMeshAgent::set_autoTraverseOffMeshLink",    // -> NavMeshAgent_Set_Custom_PropAutoTraverseOffMeshLink
    "UnityEngine.NavMeshAgent::get_autoBraking",    // -> NavMeshAgent_Get_Custom_PropAutoBraking
    "UnityEngine.NavMeshAgent::set_autoBraking",    // -> NavMeshAgent_Set_Custom_PropAutoBraking
    "UnityEngine.NavMeshAgent::get_autoRepath",    // -> NavMeshAgent_Get_Custom_PropAutoRepath
    "UnityEngine.NavMeshAgent::set_autoRepath",    // -> NavMeshAgent_Set_Custom_PropAutoRepath
    "UnityEngine.NavMeshAgent::get_hasPath" ,    // -> NavMeshAgent_Get_Custom_PropHasPath
    "UnityEngine.NavMeshAgent::get_pathPending",    // -> NavMeshAgent_Get_Custom_PropPathPending
    "UnityEngine.NavMeshAgent::get_isPathStale",    // -> NavMeshAgent_Get_Custom_PropIsPathStale
    "UnityEngine.NavMeshAgent::get_pathStatus",    // -> NavMeshAgent_Get_Custom_PropPathStatus
    "UnityEngine.NavMeshAgent::INTERNAL_get_pathEndPosition",    // -> NavMeshAgent_CUSTOM_INTERNAL_get_pathEndPosition
    "UnityEngine.NavMeshAgent::INTERNAL_CALL_Warp",    // -> NavMeshAgent_CUSTOM_INTERNAL_CALL_Warp
    "UnityEngine.NavMeshAgent::INTERNAL_CALL_Move",    // -> NavMeshAgent_CUSTOM_INTERNAL_CALL_Move
    "UnityEngine.NavMeshAgent::Stop"        ,    // -> NavMeshAgent_CUSTOM_Stop
    "UnityEngine.NavMeshAgent::Resume"      ,    // -> NavMeshAgent_CUSTOM_Resume
    "UnityEngine.NavMeshAgent::ResetPath"   ,    // -> NavMeshAgent_CUSTOM_ResetPath
    "UnityEngine.NavMeshAgent::SetPath"     ,    // -> NavMeshAgent_CUSTOM_SetPath
    "UnityEngine.NavMeshAgent::CopyPathTo"  ,    // -> NavMeshAgent_CUSTOM_CopyPathTo
    "UnityEngine.NavMeshAgent::FindClosestEdge",    // -> NavMeshAgent_CUSTOM_FindClosestEdge
    "UnityEngine.NavMeshAgent::INTERNAL_CALL_Raycast",    // -> NavMeshAgent_CUSTOM_INTERNAL_CALL_Raycast
    "UnityEngine.NavMeshAgent::INTERNAL_CALL_CalculatePathInternal",    // -> NavMeshAgent_CUSTOM_INTERNAL_CALL_CalculatePathInternal
    "UnityEngine.NavMeshAgent::SamplePathPosition",    // -> NavMeshAgent_CUSTOM_SamplePathPosition
    "UnityEngine.NavMeshAgent::SetLayerCost",    // -> NavMeshAgent_CUSTOM_SetLayerCost
    "UnityEngine.NavMeshAgent::GetLayerCost",    // -> NavMeshAgent_CUSTOM_GetLayerCost
    "UnityEngine.NavMeshAgent::get_walkableMask",    // -> NavMeshAgent_Get_Custom_PropWalkableMask
    "UnityEngine.NavMeshAgent::set_walkableMask",    // -> NavMeshAgent_Set_Custom_PropWalkableMask
    "UnityEngine.NavMeshAgent::get_speed"   ,    // -> NavMeshAgent_Get_Custom_PropSpeed
    "UnityEngine.NavMeshAgent::set_speed"   ,    // -> NavMeshAgent_Set_Custom_PropSpeed
    "UnityEngine.NavMeshAgent::get_angularSpeed",    // -> NavMeshAgent_Get_Custom_PropAngularSpeed
    "UnityEngine.NavMeshAgent::set_angularSpeed",    // -> NavMeshAgent_Set_Custom_PropAngularSpeed
    "UnityEngine.NavMeshAgent::get_acceleration",    // -> NavMeshAgent_Get_Custom_PropAcceleration
    "UnityEngine.NavMeshAgent::set_acceleration",    // -> NavMeshAgent_Set_Custom_PropAcceleration
    "UnityEngine.NavMeshAgent::get_updatePosition",    // -> NavMeshAgent_Get_Custom_PropUpdatePosition
    "UnityEngine.NavMeshAgent::set_updatePosition",    // -> NavMeshAgent_Set_Custom_PropUpdatePosition
    "UnityEngine.NavMeshAgent::get_updateRotation",    // -> NavMeshAgent_Get_Custom_PropUpdateRotation
    "UnityEngine.NavMeshAgent::set_updateRotation",    // -> NavMeshAgent_Set_Custom_PropUpdateRotation
    "UnityEngine.NavMeshAgent::get_radius"  ,    // -> NavMeshAgent_Get_Custom_PropRadius
    "UnityEngine.NavMeshAgent::set_radius"  ,    // -> NavMeshAgent_Set_Custom_PropRadius
    "UnityEngine.NavMeshAgent::get_height"  ,    // -> NavMeshAgent_Get_Custom_PropHeight
    "UnityEngine.NavMeshAgent::set_height"  ,    // -> NavMeshAgent_Set_Custom_PropHeight
    "UnityEngine.NavMeshAgent::get_obstacleAvoidanceType",    // -> NavMeshAgent_Get_Custom_PropObstacleAvoidanceType
    "UnityEngine.NavMeshAgent::set_obstacleAvoidanceType",    // -> NavMeshAgent_Set_Custom_PropObstacleAvoidanceType
    "UnityEngine.NavMeshAgent::get_avoidancePriority",    // -> NavMeshAgent_Get_Custom_PropAvoidancePriority
    "UnityEngine.NavMeshAgent::set_avoidancePriority",    // -> NavMeshAgent_Set_Custom_PropAvoidancePriority
    NULL
};

static const void* s_NavMeshAgent_IcallFuncs [] =
{
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_CALL_SetDestination,  //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_SetDestination
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_get_destination,  //  <- UnityEngine.NavMeshAgent::INTERNAL_get_destination
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_set_destination,  //  <- UnityEngine.NavMeshAgent::INTERNAL_set_destination
    (const void*)&NavMeshAgent_Get_Custom_PropStoppingDistance,  //  <- UnityEngine.NavMeshAgent::get_stoppingDistance
    (const void*)&NavMeshAgent_Set_Custom_PropStoppingDistance,  //  <- UnityEngine.NavMeshAgent::set_stoppingDistance
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_get_velocity,  //  <- UnityEngine.NavMeshAgent::INTERNAL_get_velocity
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_set_velocity,  //  <- UnityEngine.NavMeshAgent::INTERNAL_set_velocity
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_get_nextPosition,  //  <- UnityEngine.NavMeshAgent::INTERNAL_get_nextPosition
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_set_nextPosition,  //  <- UnityEngine.NavMeshAgent::INTERNAL_set_nextPosition
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_get_steeringTarget,  //  <- UnityEngine.NavMeshAgent::INTERNAL_get_steeringTarget
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_get_desiredVelocity,  //  <- UnityEngine.NavMeshAgent::INTERNAL_get_desiredVelocity
    (const void*)&NavMeshAgent_Get_Custom_PropRemainingDistance,  //  <- UnityEngine.NavMeshAgent::get_remainingDistance
    (const void*)&NavMeshAgent_Get_Custom_PropBaseOffset  ,  //  <- UnityEngine.NavMeshAgent::get_baseOffset
    (const void*)&NavMeshAgent_Set_Custom_PropBaseOffset  ,  //  <- UnityEngine.NavMeshAgent::set_baseOffset
    (const void*)&NavMeshAgent_Get_Custom_PropIsOnOffMeshLink,  //  <- UnityEngine.NavMeshAgent::get_isOnOffMeshLink
    (const void*)&NavMeshAgent_CUSTOM_ActivateCurrentOffMeshLink,  //  <- UnityEngine.NavMeshAgent::ActivateCurrentOffMeshLink
    (const void*)&NavMeshAgent_CUSTOM_GetCurrentOffMeshLinkDataInternal,  //  <- UnityEngine.NavMeshAgent::GetCurrentOffMeshLinkDataInternal
    (const void*)&NavMeshAgent_CUSTOM_GetNextOffMeshLinkDataInternal,  //  <- UnityEngine.NavMeshAgent::GetNextOffMeshLinkDataInternal
    (const void*)&NavMeshAgent_CUSTOM_CompleteOffMeshLink ,  //  <- UnityEngine.NavMeshAgent::CompleteOffMeshLink
    (const void*)&NavMeshAgent_Get_Custom_PropAutoTraverseOffMeshLink,  //  <- UnityEngine.NavMeshAgent::get_autoTraverseOffMeshLink
    (const void*)&NavMeshAgent_Set_Custom_PropAutoTraverseOffMeshLink,  //  <- UnityEngine.NavMeshAgent::set_autoTraverseOffMeshLink
    (const void*)&NavMeshAgent_Get_Custom_PropAutoBraking ,  //  <- UnityEngine.NavMeshAgent::get_autoBraking
    (const void*)&NavMeshAgent_Set_Custom_PropAutoBraking ,  //  <- UnityEngine.NavMeshAgent::set_autoBraking
    (const void*)&NavMeshAgent_Get_Custom_PropAutoRepath  ,  //  <- UnityEngine.NavMeshAgent::get_autoRepath
    (const void*)&NavMeshAgent_Set_Custom_PropAutoRepath  ,  //  <- UnityEngine.NavMeshAgent::set_autoRepath
    (const void*)&NavMeshAgent_Get_Custom_PropHasPath     ,  //  <- UnityEngine.NavMeshAgent::get_hasPath
    (const void*)&NavMeshAgent_Get_Custom_PropPathPending ,  //  <- UnityEngine.NavMeshAgent::get_pathPending
    (const void*)&NavMeshAgent_Get_Custom_PropIsPathStale ,  //  <- UnityEngine.NavMeshAgent::get_isPathStale
    (const void*)&NavMeshAgent_Get_Custom_PropPathStatus  ,  //  <- UnityEngine.NavMeshAgent::get_pathStatus
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_get_pathEndPosition,  //  <- UnityEngine.NavMeshAgent::INTERNAL_get_pathEndPosition
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_CALL_Warp  ,  //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_Warp
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_CALL_Move  ,  //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_Move
    (const void*)&NavMeshAgent_CUSTOM_Stop                ,  //  <- UnityEngine.NavMeshAgent::Stop
    (const void*)&NavMeshAgent_CUSTOM_Resume              ,  //  <- UnityEngine.NavMeshAgent::Resume
    (const void*)&NavMeshAgent_CUSTOM_ResetPath           ,  //  <- UnityEngine.NavMeshAgent::ResetPath
    (const void*)&NavMeshAgent_CUSTOM_SetPath             ,  //  <- UnityEngine.NavMeshAgent::SetPath
    (const void*)&NavMeshAgent_CUSTOM_CopyPathTo          ,  //  <- UnityEngine.NavMeshAgent::CopyPathTo
    (const void*)&NavMeshAgent_CUSTOM_FindClosestEdge     ,  //  <- UnityEngine.NavMeshAgent::FindClosestEdge
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_CALL_Raycast,  //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_Raycast
    (const void*)&NavMeshAgent_CUSTOM_INTERNAL_CALL_CalculatePathInternal,  //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_CalculatePathInternal
    (const void*)&NavMeshAgent_CUSTOM_SamplePathPosition  ,  //  <- UnityEngine.NavMeshAgent::SamplePathPosition
    (const void*)&NavMeshAgent_CUSTOM_SetLayerCost        ,  //  <- UnityEngine.NavMeshAgent::SetLayerCost
    (const void*)&NavMeshAgent_CUSTOM_GetLayerCost        ,  //  <- UnityEngine.NavMeshAgent::GetLayerCost
    (const void*)&NavMeshAgent_Get_Custom_PropWalkableMask,  //  <- UnityEngine.NavMeshAgent::get_walkableMask
    (const void*)&NavMeshAgent_Set_Custom_PropWalkableMask,  //  <- UnityEngine.NavMeshAgent::set_walkableMask
    (const void*)&NavMeshAgent_Get_Custom_PropSpeed       ,  //  <- UnityEngine.NavMeshAgent::get_speed
    (const void*)&NavMeshAgent_Set_Custom_PropSpeed       ,  //  <- UnityEngine.NavMeshAgent::set_speed
    (const void*)&NavMeshAgent_Get_Custom_PropAngularSpeed,  //  <- UnityEngine.NavMeshAgent::get_angularSpeed
    (const void*)&NavMeshAgent_Set_Custom_PropAngularSpeed,  //  <- UnityEngine.NavMeshAgent::set_angularSpeed
    (const void*)&NavMeshAgent_Get_Custom_PropAcceleration,  //  <- UnityEngine.NavMeshAgent::get_acceleration
    (const void*)&NavMeshAgent_Set_Custom_PropAcceleration,  //  <- UnityEngine.NavMeshAgent::set_acceleration
    (const void*)&NavMeshAgent_Get_Custom_PropUpdatePosition,  //  <- UnityEngine.NavMeshAgent::get_updatePosition
    (const void*)&NavMeshAgent_Set_Custom_PropUpdatePosition,  //  <- UnityEngine.NavMeshAgent::set_updatePosition
    (const void*)&NavMeshAgent_Get_Custom_PropUpdateRotation,  //  <- UnityEngine.NavMeshAgent::get_updateRotation
    (const void*)&NavMeshAgent_Set_Custom_PropUpdateRotation,  //  <- UnityEngine.NavMeshAgent::set_updateRotation
    (const void*)&NavMeshAgent_Get_Custom_PropRadius      ,  //  <- UnityEngine.NavMeshAgent::get_radius
    (const void*)&NavMeshAgent_Set_Custom_PropRadius      ,  //  <- UnityEngine.NavMeshAgent::set_radius
    (const void*)&NavMeshAgent_Get_Custom_PropHeight      ,  //  <- UnityEngine.NavMeshAgent::get_height
    (const void*)&NavMeshAgent_Set_Custom_PropHeight      ,  //  <- UnityEngine.NavMeshAgent::set_height
    (const void*)&NavMeshAgent_Get_Custom_PropObstacleAvoidanceType,  //  <- UnityEngine.NavMeshAgent::get_obstacleAvoidanceType
    (const void*)&NavMeshAgent_Set_Custom_PropObstacleAvoidanceType,  //  <- UnityEngine.NavMeshAgent::set_obstacleAvoidanceType
    (const void*)&NavMeshAgent_Get_Custom_PropAvoidancePriority,  //  <- UnityEngine.NavMeshAgent::get_avoidancePriority
    (const void*)&NavMeshAgent_Set_Custom_PropAvoidancePriority,  //  <- UnityEngine.NavMeshAgent::set_avoidancePriority
    NULL
};

void ExportNavMeshAgentBindings();
void ExportNavMeshAgentBindings()
{
    for (int i = 0; s_NavMeshAgent_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_NavMeshAgent_IcallNames [i], s_NavMeshAgent_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportNavMeshAgentBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_CALL_SetDestination); //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_SetDestination
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_get_destination); //  <- UnityEngine.NavMeshAgent::INTERNAL_get_destination
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_set_destination); //  <- UnityEngine.NavMeshAgent::INTERNAL_set_destination
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropStoppingDistance); //  <- UnityEngine.NavMeshAgent::get_stoppingDistance
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropStoppingDistance); //  <- UnityEngine.NavMeshAgent::set_stoppingDistance
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_get_velocity); //  <- UnityEngine.NavMeshAgent::INTERNAL_get_velocity
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_set_velocity); //  <- UnityEngine.NavMeshAgent::INTERNAL_set_velocity
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_get_nextPosition); //  <- UnityEngine.NavMeshAgent::INTERNAL_get_nextPosition
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_set_nextPosition); //  <- UnityEngine.NavMeshAgent::INTERNAL_set_nextPosition
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_get_steeringTarget); //  <- UnityEngine.NavMeshAgent::INTERNAL_get_steeringTarget
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_get_desiredVelocity); //  <- UnityEngine.NavMeshAgent::INTERNAL_get_desiredVelocity
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropRemainingDistance); //  <- UnityEngine.NavMeshAgent::get_remainingDistance
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropBaseOffset); //  <- UnityEngine.NavMeshAgent::get_baseOffset
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropBaseOffset); //  <- UnityEngine.NavMeshAgent::set_baseOffset
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropIsOnOffMeshLink); //  <- UnityEngine.NavMeshAgent::get_isOnOffMeshLink
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_ActivateCurrentOffMeshLink); //  <- UnityEngine.NavMeshAgent::ActivateCurrentOffMeshLink
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_GetCurrentOffMeshLinkDataInternal); //  <- UnityEngine.NavMeshAgent::GetCurrentOffMeshLinkDataInternal
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_GetNextOffMeshLinkDataInternal); //  <- UnityEngine.NavMeshAgent::GetNextOffMeshLinkDataInternal
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_CompleteOffMeshLink); //  <- UnityEngine.NavMeshAgent::CompleteOffMeshLink
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropAutoTraverseOffMeshLink); //  <- UnityEngine.NavMeshAgent::get_autoTraverseOffMeshLink
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropAutoTraverseOffMeshLink); //  <- UnityEngine.NavMeshAgent::set_autoTraverseOffMeshLink
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropAutoBraking); //  <- UnityEngine.NavMeshAgent::get_autoBraking
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropAutoBraking); //  <- UnityEngine.NavMeshAgent::set_autoBraking
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropAutoRepath); //  <- UnityEngine.NavMeshAgent::get_autoRepath
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropAutoRepath); //  <- UnityEngine.NavMeshAgent::set_autoRepath
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropHasPath); //  <- UnityEngine.NavMeshAgent::get_hasPath
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropPathPending); //  <- UnityEngine.NavMeshAgent::get_pathPending
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropIsPathStale); //  <- UnityEngine.NavMeshAgent::get_isPathStale
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropPathStatus); //  <- UnityEngine.NavMeshAgent::get_pathStatus
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_get_pathEndPosition); //  <- UnityEngine.NavMeshAgent::INTERNAL_get_pathEndPosition
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_CALL_Warp); //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_Warp
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_CALL_Move); //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_Move
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_Stop); //  <- UnityEngine.NavMeshAgent::Stop
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_Resume); //  <- UnityEngine.NavMeshAgent::Resume
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_ResetPath); //  <- UnityEngine.NavMeshAgent::ResetPath
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_SetPath); //  <- UnityEngine.NavMeshAgent::SetPath
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_CopyPathTo); //  <- UnityEngine.NavMeshAgent::CopyPathTo
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_FindClosestEdge); //  <- UnityEngine.NavMeshAgent::FindClosestEdge
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_CALL_Raycast); //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_Raycast
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_INTERNAL_CALL_CalculatePathInternal); //  <- UnityEngine.NavMeshAgent::INTERNAL_CALL_CalculatePathInternal
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_SamplePathPosition); //  <- UnityEngine.NavMeshAgent::SamplePathPosition
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_SetLayerCost); //  <- UnityEngine.NavMeshAgent::SetLayerCost
    SET_METRO_BINDING(NavMeshAgent_CUSTOM_GetLayerCost); //  <- UnityEngine.NavMeshAgent::GetLayerCost
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropWalkableMask); //  <- UnityEngine.NavMeshAgent::get_walkableMask
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropWalkableMask); //  <- UnityEngine.NavMeshAgent::set_walkableMask
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropSpeed); //  <- UnityEngine.NavMeshAgent::get_speed
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropSpeed); //  <- UnityEngine.NavMeshAgent::set_speed
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropAngularSpeed); //  <- UnityEngine.NavMeshAgent::get_angularSpeed
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropAngularSpeed); //  <- UnityEngine.NavMeshAgent::set_angularSpeed
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropAcceleration); //  <- UnityEngine.NavMeshAgent::get_acceleration
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropAcceleration); //  <- UnityEngine.NavMeshAgent::set_acceleration
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropUpdatePosition); //  <- UnityEngine.NavMeshAgent::get_updatePosition
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropUpdatePosition); //  <- UnityEngine.NavMeshAgent::set_updatePosition
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropUpdateRotation); //  <- UnityEngine.NavMeshAgent::get_updateRotation
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropUpdateRotation); //  <- UnityEngine.NavMeshAgent::set_updateRotation
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropRadius); //  <- UnityEngine.NavMeshAgent::get_radius
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropRadius); //  <- UnityEngine.NavMeshAgent::set_radius
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropHeight); //  <- UnityEngine.NavMeshAgent::get_height
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropHeight); //  <- UnityEngine.NavMeshAgent::set_height
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropObstacleAvoidanceType); //  <- UnityEngine.NavMeshAgent::get_obstacleAvoidanceType
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropObstacleAvoidanceType); //  <- UnityEngine.NavMeshAgent::set_obstacleAvoidanceType
    SET_METRO_BINDING(NavMeshAgent_Get_Custom_PropAvoidancePriority); //  <- UnityEngine.NavMeshAgent::get_avoidancePriority
    SET_METRO_BINDING(NavMeshAgent_Set_Custom_PropAvoidancePriority); //  <- UnityEngine.NavMeshAgent::set_avoidancePriority
}

#endif
