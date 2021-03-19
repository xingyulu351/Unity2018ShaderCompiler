#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"

#include "Runtime/Graphics/Transform.h"
#include "Runtime/Dynamics/Rigidbody.h"
#include "Runtime/Geometry/Ray.h"
#include "Runtime/Dynamics/PhysicsManager.h"
#include "Runtime/Geometry/Sphere.h"
#include "Runtime/Dynamics/Joints.h"
#include "Runtime/Dynamics/ConstantForce.h"
#include "Runtime/Graphics/Heightmap.h"

#include "Runtime/Dynamics/CapsuleCollider.h"
#include "Runtime/Dynamics/BoxCollider.h"
#include "Runtime/Dynamics/SphereCollider.h"
#include "Runtime/Dynamics/RaycastCollider.h"
#include "Runtime/Dynamics/WheelCollider.h"
#include "Runtime/Dynamics/MeshCollider.h"
#include "Runtime/Dynamics/PhysicMaterial.h"
#include "Runtime/Dynamics/CharacterController.h"
#include "Runtime/Dynamics/CharacterJoint.h"
#include "Runtime/Dynamics/ConfigurableJoint.h"
#include "Runtime/Dynamics/SpringJoint.h"
#include "Runtime/Geometry/AABB.h"
#include "Runtime/Filters/Mesh/LodMesh.h"
#include "Runtime/Dynamics/Cloth.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Dynamics/SkinnedCloth.h"
#include "Runtime/Filters/Misc/ClothRenderer.h"
#include "Runtime/Dynamics/RaycastHit.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoManager.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

using namespace Unity;

#define out



#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_get_gravity(Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_get_gravity)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_gravity)
    { *returnValue =(GetPhysicsManager ().GetGravity ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_set_gravity(const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_set_gravity)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_gravity)
     return GetPhysicsManager ().SetGravity (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropMinPenetrationForPenalty()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropMinPenetrationForPenalty)
    SCRIPTINGAPI_STACK_CHECK(get_minPenetrationForPenalty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_minPenetrationForPenalty)
    return GetPhysicsManager ().GetMinPenetrationForPenalty ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropMinPenetrationForPenalty(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropMinPenetrationForPenalty)
    SCRIPTINGAPI_STACK_CHECK(set_minPenetrationForPenalty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_minPenetrationForPenalty)
     return GetPhysicsManager ().SetMinPenetrationForPenalty (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropBounceThreshold()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropBounceThreshold)
    SCRIPTINGAPI_STACK_CHECK(get_bounceThreshold)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bounceThreshold)
    return GetPhysicsManager ().GetBounceThreshold ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropBounceThreshold(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropBounceThreshold)
    SCRIPTINGAPI_STACK_CHECK(set_bounceThreshold)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bounceThreshold)
     return GetPhysicsManager ().SetBounceThreshold (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropSleepVelocity()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropSleepVelocity)
    SCRIPTINGAPI_STACK_CHECK(get_sleepVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sleepVelocity)
    return GetPhysicsManager ().GetSleepVelocity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropSleepVelocity(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropSleepVelocity)
    SCRIPTINGAPI_STACK_CHECK(set_sleepVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sleepVelocity)
     return GetPhysicsManager ().SetSleepVelocity (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropSleepAngularVelocity()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropSleepAngularVelocity)
    SCRIPTINGAPI_STACK_CHECK(get_sleepAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sleepAngularVelocity)
    return GetPhysicsManager ().GetSleepAngularVelocity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropSleepAngularVelocity(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropSleepAngularVelocity)
    SCRIPTINGAPI_STACK_CHECK(set_sleepAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sleepAngularVelocity)
     return GetPhysicsManager ().SetSleepAngularVelocity (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropMaxAngularVelocity()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropMaxAngularVelocity)
    SCRIPTINGAPI_STACK_CHECK(get_maxAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxAngularVelocity)
    return GetPhysicsManager ().GetMaxAngularVelocity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropMaxAngularVelocity(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropMaxAngularVelocity)
    SCRIPTINGAPI_STACK_CHECK(set_maxAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxAngularVelocity)
     return GetPhysicsManager ().SetMaxAngularVelocity (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropSolverIterationCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropSolverIterationCount)
    SCRIPTINGAPI_STACK_CHECK(get_solverIterationCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_solverIterationCount)
    return GetPhysicsManager ().GetSolverIterationCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropSolverIterationCount(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropSolverIterationCount)
    SCRIPTINGAPI_STACK_CHECK(set_solverIterationCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_solverIterationCount)
     return GetPhysicsManager ().SetSolverIterationCount (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_Internal_Raycast(const Vector3f& origin, const Vector3f& direction, RaycastHit* hitInfo, float distance, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_Internal_Raycast)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Raycast)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Raycast)
    
    		hitInfo->collider = NULL;
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    			Ray ray (origin, normalizedDirection);
    			
    			bool didHit = GetPhysicsManager ().Raycast (ray, distance, *hitInfo, layermask);
    			
    			if (didHit)
    			{
    				hitInfo->collider = reinterpret_cast<Collider*>(ScriptingGetObjectReference (hitInfo->collider));
    				return true;
    			}
    			else
    			{
    				return false;
    			}
    		}
    		else
    			return false;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_Internal_CapsuleCast(const Vector3f& point1, const Vector3f& point2, float radius, const Vector3f& direction, RaycastHit* hitInfo, float distance, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_Internal_CapsuleCast)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_CapsuleCast)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_CapsuleCast)
    
    		hitInfo->collider = NULL;
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    			
    			bool didHit = GetPhysicsManager ().CapsuleCast (point1, point2, radius, normalizedDirection, distance, *hitInfo, layermask);
    			
    			if (didHit)
    			{
    				hitInfo->collider = reinterpret_cast<Collider*>(ScriptingGetObjectReference (hitInfo->collider));
    				return true;
    			}
    			else
    			{
    				return false;
    			}
    		}
    		else
    			return false;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_Internal_RaycastTest(const Vector3f& origin, const Vector3f& direction, float distance, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_Internal_RaycastTest)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_RaycastTest)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_RaycastTest)
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    			Ray ray (origin, normalizedDirection);
    			return GetPhysicsManager ().RaycastTest (ray, distance, layermask);
    		}
    		else
    			return false;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_RaycastAll(const Vector3f& origin, const Vector3f& direction, float distance, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_RaycastAll)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RaycastAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_RaycastAll)
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    			Ray ray (origin, normalizedDirection);
    
    			const PhysicsManager::RaycastHits& hits = GetPhysicsManager ().RaycastAll (ray, distance, layermask);
    
    			return ConvertNativeRaycastHitsToManaged(hits);
    		}
    		else
    		{
    			return CreateEmptyStructArray(GetMonoManager().GetCommonClasses().raycastHit);
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_OverlapSphere(const Vector3f& position, float radius, int layerMask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_OverlapSphere)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_OverlapSphere)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_OverlapSphere)
    
    		const vector<Collider*>& colliders = GetPhysicsManager ().OverlapSphere (position, radius, layerMask);
    		
    		return CreateScriptingArrayFromUnityObjects(colliders, ScriptingClassFor(Collider));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_CapsuleCastAll(const Vector3f& point1, const Vector3f& point2, float radius, const Vector3f& direction, float distance, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_CapsuleCastAll)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CapsuleCastAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CapsuleCastAll)
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    
    			const PhysicsManager::RaycastHits& hits = GetPhysicsManager ().CapsuleCastAll (point1, point2, radius, normalizedDirection, distance, layermask);
    		
    			return ConvertNativeRaycastHitsToManaged(hits);
    		}
    		else
    		{
    			return CreateEmptyStructArray(GetMonoManager().GetCommonClasses().raycastHit);
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_CheckSphere(const Vector3f& position, float radius, int layerMask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_CheckSphere)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CheckSphere)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CheckSphere)
    
    		return GetPhysicsManager ().SphereTest (position, radius, layerMask);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics_CUSTOM_INTERNAL_CALL_CheckCapsule(const Vector3f& start, const Vector3f& end, float radius, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_INTERNAL_CALL_CheckCapsule)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CheckCapsule)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CheckCapsule)
    
    		return GetPhysicsManager().CapsuleTest(start, end, radius, layermask);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Physics_Get_Custom_PropPenetrationPenaltyForce()
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Get_Custom_PropPenetrationPenaltyForce)
    SCRIPTINGAPI_STACK_CHECK(get_penetrationPenaltyForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_penetrationPenaltyForce)
    return 0;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_Set_Custom_PropPenetrationPenaltyForce(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_Set_Custom_PropPenetrationPenaltyForce)
    SCRIPTINGAPI_STACK_CHECK(set_penetrationPenaltyForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_penetrationPenaltyForce)
      
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_CUSTOM_IgnoreCollision(ICallType_Object_Argument collider1_, ICallType_Object_Argument collider2_, ScriptingBool ignore)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_IgnoreCollision)
    ScriptingObjectOfType<Collider> collider1(collider1_);
    UNUSED(collider1);
    ScriptingObjectOfType<Collider> collider2(collider2_);
    UNUSED(collider2);
    SCRIPTINGAPI_STACK_CHECK(IgnoreCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IgnoreCollision)
    
    		GetPhysicsManager().IgnoreCollision(*collider1, *collider2, ignore);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics_CUSTOM_IgnoreLayerCollision(int layer1, int layer2, ScriptingBool ignore)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_IgnoreLayerCollision)
    SCRIPTINGAPI_STACK_CHECK(IgnoreLayerCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IgnoreLayerCollision)
    
    		GetPhysicsManager().IgnoreCollision(layer1, layer2, ignore);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics_CUSTOM_GetIgnoreLayerCollision(int layer1, int layer2)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics_CUSTOM_GetIgnoreLayerCollision)
    SCRIPTINGAPI_STACK_CHECK(GetIgnoreLayerCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIgnoreLayerCollision)
    
    		return GetPhysicsManager().GetIgnoreCollision(layer1, layer2);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_velocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_velocity)
    *returnValue = self->GetVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_velocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_velocity)
    
    self->SetVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_angularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_angularVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_angularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_angularVelocity)
    *returnValue = self->GetAngularVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_angularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_angularVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_angularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_angularVelocity)
    
    self->SetAngularVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_drag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_drag)
    return self->GetDrag ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_drag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_drag)
    
    self->SetDrag (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropAngularDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropAngularDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angularDrag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angularDrag)
    return self->GetAngularDrag ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropAngularDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropAngularDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_angularDrag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_angularDrag)
    
    self->SetAngularDrag (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropMass(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropMass)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mass)
    return self->GetMass ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropMass(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropMass)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_mass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mass)
    
    self->SetMass (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_SetDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float density)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_SetDensity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetDensity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetDensity)
    return self->SetDensity(density);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropUseGravity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropUseGravity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useGravity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useGravity)
    return self->GetUseGravity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropUseGravity(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropUseGravity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useGravity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useGravity)
    
    self->SetUseGravity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropIsKinematic(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropIsKinematic)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isKinematic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isKinematic)
    return self->GetIsKinematic ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropIsKinematic(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropIsKinematic)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_isKinematic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isKinematic)
    
    self->SetIsKinematic (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropFreezeRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropFreezeRotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_freezeRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_freezeRotation)
    return self->GetFreezeRotation ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropFreezeRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropFreezeRotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_freezeRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_freezeRotation)
    
    self->SetFreezeRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropConstraints(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropConstraints)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_constraints)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_constraints)
    return self->GetConstraints ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropConstraints(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropConstraints)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_constraints)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_constraints)
    
    self->SetConstraints (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropCollisionDetectionMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropCollisionDetectionMode)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collisionDetectionMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collisionDetectionMode)
    return self->GetCollisionDetectionMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropCollisionDetectionMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropCollisionDetectionMode)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_collisionDetectionMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_collisionDetectionMode)
    
    self->SetCollisionDetectionMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_AddForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& force, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_AddForce)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddForce)
     self->AddForce (force, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& force, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeForce)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddRelativeForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddRelativeForce)
     self->AddRelativeForce (force, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_AddTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& torque, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_AddTorque)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddTorque)
     self->AddTorque (torque, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& torque, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeTorque)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddRelativeTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddRelativeTorque)
     self->AddRelativeTorque (torque, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_AddForceAtPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& force, const Vector3f& position, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_AddForceAtPosition)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddForceAtPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddForceAtPosition)
     self->AddForceAtPosition (force, position, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_AddExplosionForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, float explosionForce, const Vector3f& explosionPosition, float explosionRadius, float upwardsModifier, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_AddExplosionForce)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddExplosionForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddExplosionForce)
     self->AddExplosionForce (explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ClosestPointOnBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ClosestPointOnBounds)
    
    		float dist; Vector3f outpos;
    		self->ClosestPointOnBounds(position, outpos, dist);
    		{ returnValue =(outpos); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_GetRelativePointVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& relativePoint, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_GetRelativePointVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetRelativePointVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetRelativePointVelocity)
    returnValue = self->GetRelativePointVelocity(relativePoint);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_GetPointVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& worldPoint, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_GetPointVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetPointVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetPointVelocity)
    returnValue = self->GetPointVelocity(worldPoint);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_centerOfMass(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_centerOfMass)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_centerOfMass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_centerOfMass)
    *returnValue = self->GetCenterOfMass();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_centerOfMass(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_centerOfMass)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_centerOfMass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_centerOfMass)
    
    self->SetCenterOfMass (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_worldCenterOfMass(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_worldCenterOfMass)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_worldCenterOfMass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_worldCenterOfMass)
    *returnValue = self->GetWorldCenterOfMass();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_inertiaTensorRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_inertiaTensorRotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_inertiaTensorRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_inertiaTensorRotation)
    *returnValue = self->GetInertiaTensorRotation();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_inertiaTensorRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_inertiaTensorRotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_inertiaTensorRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_inertiaTensorRotation)
    
    self->SetInertiaTensorRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_inertiaTensor(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_inertiaTensor)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_inertiaTensor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_inertiaTensor)
    *returnValue = self->GetInertiaTensor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_inertiaTensor(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_inertiaTensor)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_inertiaTensor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_inertiaTensor)
    
    self->SetInertiaTensor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropDetectCollisions(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropDetectCollisions)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detectCollisions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detectCollisions)
    return self->GetDetectCollisions ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropDetectCollisions(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropDetectCollisions)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_detectCollisions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_detectCollisions)
    
    self->SetDetectCollisions (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropUseConeFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropUseConeFriction)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useConeFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useConeFriction)
    return self->GetUseConeFriction ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropUseConeFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropUseConeFriction)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useConeFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useConeFriction)
    
    self->SetUseConeFriction (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_position(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_position)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_position)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_position)
    *returnValue = self->GetPosition();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_position(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_position)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_position)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_position)
    
    self->SetPosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_get_rotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_get_rotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rotation)
    *returnValue = self->GetRotation();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_set_rotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_set_rotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_rotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_rotation)
    
    self->SetRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_MovePosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_MovePosition)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_MovePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_MovePosition)
    return self->MovePosition(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_MoveRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& rot)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_MoveRotation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_MoveRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_MoveRotation)
    return self->MoveRotation(rot);
}

SCRIPT_BINDINGS_EXPORT_DECL
RigidbodyInterpolation SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropInterpolation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropInterpolation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_interpolation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_interpolation)
    return self->GetInterpolation ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropInterpolation(ICallType_ReadOnlyUnityEngineObject_Argument self_, RigidbodyInterpolation value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropInterpolation)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_interpolation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_interpolation)
    
    self->SetInterpolation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_Sleep(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_Sleep)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Sleep)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Sleep)
    return self->Sleep();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_IsSleeping(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_IsSleeping)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_IsSleeping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_IsSleeping)
    return self->IsSleeping();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_WakeUp(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_WakeUp)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_WakeUp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_WakeUp)
    return self->WakeUp();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropSolverIterationCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropSolverIterationCount)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_solverIterationCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_solverIterationCount)
    return self->GetSolverIterationCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropSolverIterationCount(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropSolverIterationCount)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_solverIterationCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_solverIterationCount)
    
    self->SetSolverIterationCount (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropSleepVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropSleepVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sleepVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sleepVelocity)
    return self->GetSleepVelocity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropSleepVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropSleepVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_sleepVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sleepVelocity)
    
    self->SetSleepVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropSleepAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropSleepAngularVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sleepAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sleepAngularVelocity)
    return self->GetSleepAngularVelocity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropSleepAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropSleepAngularVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_sleepAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sleepAngularVelocity)
    
    self->SetSleepAngularVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody_Get_Custom_PropMaxAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Get_Custom_PropMaxAngularVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maxAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxAngularVelocity)
    return self->GetMaxAngularVelocity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody_Set_Custom_PropMaxAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_Set_Custom_PropMaxAngularVelocity)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maxAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxAngularVelocity)
    
    self->SetMaxAngularVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_SweepTest(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& direction, RaycastHit* hitInfo, float distance)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_SweepTest)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SweepTest)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SweepTest)
    
    		hitInfo->collider = NULL;
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    			
    			bool didHit = self->SweepTest (normalizedDirection, distance, *hitInfo);
    			
    			if (didHit)
    			{
    				hitInfo->collider = reinterpret_cast<Collider*>(ScriptingGetObjectReference (hitInfo->collider));
    				return true;
    			}
    			else
    			{
    				return false;
    			}
    		}
    		else
    			return false;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Rigidbody_CUSTOM_INTERNAL_CALL_SweepTestAll(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& direction, float distance)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody_CUSTOM_INTERNAL_CALL_SweepTestAll)
    ReadOnlyScriptingObjectOfType<Rigidbody> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SweepTestAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SweepTestAll)
    
    		float dirLength = Magnitude (direction);
    		if (dirLength > Vector3f::epsilon)
    		{
    			Vector3f normalizedDirection = direction / dirLength;
    
    			const PhysicsManager::RaycastHits& hits = self->SweepTestAll (normalizedDirection, distance);
    	
    			return ConvertNativeRaycastHitsToManaged(hits);
    		}
    		else
    		{
    			return CreateScriptingArray<RaycastHit>(NULL, 0, GetMonoManager().GetCommonClasses().raycastHit);
    		}
    	
}

#endif


struct MonoJointMotor
{
	float targetVelocity;
	float force;
	short freeSpin;

	MonoJointMotor (const JointMotor& motor)
	{
		targetVelocity = motor.targetVelocity;
		force = motor.force;
		freeSpin = motor.freeSpin;
	}

	operator JointMotor () const
	{
		JointMotor motor;
		motor.targetVelocity = targetVelocity;
		motor.force = force;
		motor.freeSpin = freeSpin;
		return motor;
	}
};


#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Joint_Get_Custom_PropConnectedBody(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_Get_Custom_PropConnectedBody)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_connectedBody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_connectedBody)
    return Scripting::ScriptingWrapperFor(self->GetConnectedBody());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_Set_Custom_PropConnectedBody(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_Set_Custom_PropConnectedBody)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Rigidbody> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_connectedBody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_connectedBody)
    
    self->SetConnectedBody (val);
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_CUSTOM_INTERNAL_get_axis(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_CUSTOM_INTERNAL_get_axis)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_axis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_axis)
    *returnValue = self->GetAxis();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_CUSTOM_INTERNAL_set_axis(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_CUSTOM_INTERNAL_set_axis)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_axis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_axis)
    
    self->SetAxis (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_CUSTOM_INTERNAL_get_anchor(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_CUSTOM_INTERNAL_get_anchor)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_anchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_anchor)
    *returnValue = self->GetAnchor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_CUSTOM_INTERNAL_set_anchor(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_CUSTOM_INTERNAL_set_anchor)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_anchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_anchor)
    
    self->SetAnchor (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Joint_Get_Custom_PropBreakForce(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_Get_Custom_PropBreakForce)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_breakForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_breakForce)
    return self->GetBreakForce ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_Set_Custom_PropBreakForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_Set_Custom_PropBreakForce)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_breakForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_breakForce)
    
    self->SetBreakForce (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Joint_Get_Custom_PropBreakTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_Get_Custom_PropBreakTorque)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_breakTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_breakTorque)
    return self->GetBreakTorque ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Joint_Set_Custom_PropBreakTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Joint_Set_Custom_PropBreakTorque)
    ReadOnlyScriptingObjectOfType<Joint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_breakTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_breakTorque)
    
    self->SetBreakTorque (value);
    
}

#endif

	
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_CUSTOM_INTERNAL_get_motor(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointMotor* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_CUSTOM_INTERNAL_get_motor)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_motor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_motor)
    *returnValue = self->GetMotor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_CUSTOM_INTERNAL_set_motor(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointMotor& value)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_CUSTOM_INTERNAL_set_motor)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_motor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_motor)
    
    self->SetMotor (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_CUSTOM_INTERNAL_get_limits(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointLimits* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_CUSTOM_INTERNAL_get_limits)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_limits)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_limits)
    *returnValue = self->GetLimits();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_CUSTOM_INTERNAL_set_limits(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointLimits& value)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_CUSTOM_INTERNAL_set_limits)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_limits)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_limits)
    
    self->SetLimits (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_CUSTOM_INTERNAL_get_spring(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointSpring* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_CUSTOM_INTERNAL_get_spring)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_spring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_spring)
    *returnValue = self->GetSpring();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_CUSTOM_INTERNAL_set_spring(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointSpring& value)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_CUSTOM_INTERNAL_set_spring)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_spring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_spring)
    
    self->SetSpring (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION HingeJoint_Get_Custom_PropUseMotor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Get_Custom_PropUseMotor)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useMotor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useMotor)
    return self->GetUseMotor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_Set_Custom_PropUseMotor(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Set_Custom_PropUseMotor)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useMotor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useMotor)
    
    self->SetUseMotor (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION HingeJoint_Get_Custom_PropUseLimits(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Get_Custom_PropUseLimits)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useLimits)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useLimits)
    return self->GetUseLimits ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_Set_Custom_PropUseLimits(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Set_Custom_PropUseLimits)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useLimits)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useLimits)
    
    self->SetUseLimits (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION HingeJoint_Get_Custom_PropUseSpring(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Get_Custom_PropUseSpring)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useSpring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useSpring)
    return self->GetUseSpring ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION HingeJoint_Set_Custom_PropUseSpring(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Set_Custom_PropUseSpring)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useSpring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useSpring)
    
    self->SetUseSpring (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION HingeJoint_Get_Custom_PropVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Get_Custom_PropVelocity)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_velocity)
    return self->GetVelocity ();
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION HingeJoint_Get_Custom_PropAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(HingeJoint_Get_Custom_PropAngle)
    ReadOnlyScriptingObjectOfType<HingeJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angle)
    return self->GetAngle ();
}

#endif


#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SpringJoint_Get_Custom_PropSpring(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Get_Custom_PropSpring)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_spring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spring)
    return self->GetSpring ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpringJoint_Set_Custom_PropSpring(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Set_Custom_PropSpring)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_spring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_spring)
    
    self->SetSpring (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SpringJoint_Get_Custom_PropDamper(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Get_Custom_PropDamper)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_damper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_damper)
    return self->GetDamper ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpringJoint_Set_Custom_PropDamper(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Set_Custom_PropDamper)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_damper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_damper)
    
    self->SetDamper (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SpringJoint_Get_Custom_PropMinDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Get_Custom_PropMinDistance)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_minDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_minDistance)
    return self->GetMinDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpringJoint_Set_Custom_PropMinDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Set_Custom_PropMinDistance)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_minDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_minDistance)
    
    self->SetMinDistance (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SpringJoint_Get_Custom_PropMaxDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Get_Custom_PropMaxDistance)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maxDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxDistance)
    return self->GetMaxDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpringJoint_Set_Custom_PropMaxDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SpringJoint_Set_Custom_PropMaxDistance)
    ReadOnlyScriptingObjectOfType<SpringJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maxDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxDistance)
    
    self->SetMaxDistance (value);
    
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_swingAxis(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_swingAxis)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_swingAxis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_swingAxis)
    *returnValue = self->GetSwingAxis();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_swingAxis(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_swingAxis)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_swingAxis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_swingAxis)
    
    self->SetSwingAxis (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_lowTwistLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_lowTwistLimit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_lowTwistLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_lowTwistLimit)
    *returnValue = self->GetLowTwistLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_lowTwistLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_lowTwistLimit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_lowTwistLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_lowTwistLimit)
    
    self->SetLowTwistLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_highTwistLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_highTwistLimit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_highTwistLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_highTwistLimit)
    *returnValue = self->GetHighTwistLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_highTwistLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_highTwistLimit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_highTwistLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_highTwistLimit)
    
    self->SetHighTwistLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_swing1Limit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_swing1Limit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_swing1Limit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_swing1Limit)
    *returnValue = self->GetSwing1Limit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_swing1Limit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_swing1Limit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_swing1Limit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_swing1Limit)
    
    self->SetSwing1Limit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_swing2Limit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_swing2Limit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_swing2Limit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_swing2Limit)
    *returnValue = self->GetSwing2Limit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_swing2Limit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_swing2Limit)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_swing2Limit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_swing2Limit)
    
    self->SetSwing2Limit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_targetRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_targetRotation)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetRotation)
    *returnValue = self->GetTargetRotation();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_targetRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_targetRotation)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_targetRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_targetRotation)
    
    self->SetTargetRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_targetAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_targetAngularVelocity)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetAngularVelocity)
    *returnValue = self->GetTargetAngularVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_targetAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_targetAngularVelocity)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_targetAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_targetAngularVelocity)
    
    self->SetTargetAngularVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_get_rotationDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_get_rotationDrive)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rotationDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rotationDrive)
    *returnValue = self->GetRotationDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterJoint_CUSTOM_INTERNAL_set_rotationDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterJoint_CUSTOM_INTERNAL_set_rotationDrive)
    ReadOnlyScriptingObjectOfType<CharacterJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_rotationDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_rotationDrive)
    
    self->SetRotationDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_secondaryAxis(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_secondaryAxis)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_secondaryAxis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_secondaryAxis)
    *returnValue = self->GetSecondaryAxis();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_secondaryAxis(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_secondaryAxis)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_secondaryAxis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_secondaryAxis)
    
    self->SetSecondaryAxis (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropXMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropXMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_xMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_xMotion)
    return self->GetXMotion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropXMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropXMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_xMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_xMotion)
    
    self->SetXMotion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropYMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropYMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_yMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_yMotion)
    return self->GetYMotion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropYMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropYMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_yMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_yMotion)
    
    self->SetYMotion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropZMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropZMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_zMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_zMotion)
    return self->GetZMotion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropZMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropZMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_zMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_zMotion)
    
    self->SetZMotion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropAngularXMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropAngularXMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angularXMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angularXMotion)
    return self->GetAngularXMotion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropAngularXMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropAngularXMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_angularXMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_angularXMotion)
    
    self->SetAngularXMotion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropAngularYMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropAngularYMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angularYMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angularYMotion)
    return self->GetAngularYMotion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropAngularYMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropAngularYMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_angularYMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_angularYMotion)
    
    self->SetAngularYMotion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropAngularZMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropAngularZMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angularZMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angularZMotion)
    return self->GetAngularZMotion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropAngularZMotion(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropAngularZMotion)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_angularZMotion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_angularZMotion)
    
    self->SetAngularZMotion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_linearLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_linearLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_linearLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_linearLimit)
    *returnValue = self->GetLinearLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_linearLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_linearLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_linearLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_linearLimit)
    
    self->SetLinearLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_lowAngularXLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_lowAngularXLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_lowAngularXLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_lowAngularXLimit)
    *returnValue = self->GetLowAngularXLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_lowAngularXLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_lowAngularXLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_lowAngularXLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_lowAngularXLimit)
    
    self->SetLowAngularXLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_highAngularXLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_highAngularXLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_highAngularXLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_highAngularXLimit)
    *returnValue = self->GetHighAngularXLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_highAngularXLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_highAngularXLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_highAngularXLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_highAngularXLimit)
    
    self->SetHighAngularXLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_angularYLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_angularYLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_angularYLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_angularYLimit)
    *returnValue = self->GetAngularYLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_angularYLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_angularYLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_angularYLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_angularYLimit)
    
    self->SetAngularYLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_angularZLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, SoftJointLimit* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_angularZLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_angularZLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_angularZLimit)
    *returnValue = self->GetAngularZLimit();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_angularZLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, const SoftJointLimit& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_angularZLimit)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_angularZLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_angularZLimit)
    
    self->SetAngularZLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_targetPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_targetPosition)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetPosition)
    *returnValue = self->GetTargetPosition();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_targetPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_targetPosition)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_targetPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_targetPosition)
    
    self->SetTargetPosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_targetVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_targetVelocity)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetVelocity)
    *returnValue = self->GetTargetVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_targetVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_targetVelocity)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_targetVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_targetVelocity)
    
    self->SetTargetVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_xDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_xDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_xDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_xDrive)
    *returnValue = self->GetXDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_xDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_xDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_xDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_xDrive)
    
    self->SetXDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_yDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_yDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_yDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_yDrive)
    *returnValue = self->GetYDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_yDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_yDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_yDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_yDrive)
    
    self->SetYDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_zDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_zDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_zDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_zDrive)
    *returnValue = self->GetZDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_zDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_zDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_zDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_zDrive)
    
    self->SetZDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_targetRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_targetRotation)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetRotation)
    *returnValue = self->GetTargetRotation();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_targetRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Quaternionf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_targetRotation)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_targetRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_targetRotation)
    
    self->SetTargetRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_targetAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_targetAngularVelocity)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_targetAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_targetAngularVelocity)
    *returnValue = self->GetTargetAngularVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_targetAngularVelocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_targetAngularVelocity)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_targetAngularVelocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_targetAngularVelocity)
    
    self->SetTargetAngularVelocity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropRotationDriveMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropRotationDriveMode)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rotationDriveMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rotationDriveMode)
    return self->GetRotationDriveMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropRotationDriveMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropRotationDriveMode)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_rotationDriveMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_rotationDriveMode)
    
    self->SetRotationDriveMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_angularXDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_angularXDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_angularXDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_angularXDrive)
    *returnValue = self->GetAngularXDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_angularXDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_angularXDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_angularXDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_angularXDrive)
    
    self->SetAngularXDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_angularYZDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_angularYZDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_angularYZDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_angularYZDrive)
    *returnValue = self->GetAngularYZDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_angularYZDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_angularYZDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_angularYZDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_angularYZDrive)
    
    self->SetAngularYZDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_get_slerpDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointDrive* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_get_slerpDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_slerpDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_slerpDrive)
    *returnValue = self->GetSlerpDrive();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_CUSTOM_INTERNAL_set_slerpDrive(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointDrive& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_CUSTOM_INTERNAL_set_slerpDrive)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_slerpDrive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_slerpDrive)
    
    self->SetSlerpDrive (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropProjectionMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropProjectionMode)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_projectionMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_projectionMode)
    return self->GetProjectionMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropProjectionMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropProjectionMode)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_projectionMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_projectionMode)
    
    self->SetProjectionMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropProjectionDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropProjectionDistance)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_projectionDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_projectionDistance)
    return self->GetProjectionDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropProjectionDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropProjectionDistance)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_projectionDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_projectionDistance)
    
    self->SetProjectionDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropProjectionAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropProjectionAngle)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_projectionAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_projectionAngle)
    return self->GetProjectionAngle ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropProjectionAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropProjectionAngle)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_projectionAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_projectionAngle)
    
    self->SetProjectionAngle (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropConfiguredInWorldSpace(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropConfiguredInWorldSpace)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_configuredInWorldSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_configuredInWorldSpace)
    return self->GetConfiguredInWorldSpace ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropConfiguredInWorldSpace(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropConfiguredInWorldSpace)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_configuredInWorldSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_configuredInWorldSpace)
    
    self->SetConfiguredInWorldSpace (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ConfigurableJoint_Get_Custom_PropSwapBodies(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Get_Custom_PropSwapBodies)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_swapBodies)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_swapBodies)
    return self->GetSwapBodies ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConfigurableJoint_Set_Custom_PropSwapBodies(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConfigurableJoint_Set_Custom_PropSwapBodies)
    ReadOnlyScriptingObjectOfType<ConfigurableJoint> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_swapBodies)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_swapBodies)
    
    self->SetSwapBodies (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_get_force(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_get_force)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_force)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_force)
    { *returnValue =(self->m_Force); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_set_force(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_set_force)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_force)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_force)
     self->m_Force = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_get_relativeForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_get_relativeForce)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_relativeForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_relativeForce)
    { *returnValue =(self->m_RelativeForce); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_set_relativeForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_set_relativeForce)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_relativeForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_relativeForce)
     self->m_RelativeForce = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_get_torque(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_get_torque)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_torque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_torque)
    { *returnValue =(self->m_Torque); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_set_torque(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_set_torque)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_torque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_torque)
     self->m_Torque = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_get_relativeTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_get_relativeTorque)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_relativeTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_relativeTorque)
    { *returnValue =(self->m_RelativeTorque); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ConstantForce_CUSTOM_INTERNAL_set_relativeTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ConstantForce_CUSTOM_INTERNAL_set_relativeTorque)
    ReadOnlyScriptingObjectOfType<ConstantForce> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_relativeTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_relativeTorque)
     self->m_RelativeTorque = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Collider_Get_Custom_PropEnabled(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Get_Custom_PropEnabled)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider_Set_Custom_PropEnabled(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Set_Custom_PropEnabled)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    self->SetEnabled (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Collider_Get_Custom_PropAttachedRigidbody(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Get_Custom_PropAttachedRigidbody)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_attachedRigidbody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_attachedRigidbody)
    return Scripting::ScriptingWrapperFor(self->GetRigidbody());
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Collider_Get_Custom_PropIsTrigger(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Get_Custom_PropIsTrigger)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isTrigger)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isTrigger)
    return self->GetIsTrigger ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider_Set_Custom_PropIsTrigger(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Set_Custom_PropIsTrigger)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_isTrigger)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isTrigger)
    
    self->SetIsTrigger (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Collider_Get_Custom_PropMaterial(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Get_Custom_PropMaterial)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    PhysicMaterial* material = self->GetMaterial ();
    PhysicMaterial* instance = &PhysicMaterial::GetInstantiatedMaterial (material, *self);
    if (instance != material)
    self->SetMaterial (instance);
    return Scripting::ScriptingWrapperFor (instance);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider_Set_Custom_PropMaterial(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Set_Custom_PropMaterial)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<PhysicMaterial> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_material)
    
    self->SetMaterial (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds(ICallType_Object_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ClosestPointOnBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ClosestPointOnBounds)
    
    		float dist; Vector3f outpos;
    		self->ClosestPointOnBounds(position, outpos, dist);
    		{ returnValue =(outpos); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Collider_Get_Custom_PropSharedMaterial(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Get_Custom_PropSharedMaterial)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sharedMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sharedMaterial)
    return Scripting::ScriptingWrapperFor (self->GetMaterial ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider_Set_Custom_PropSharedMaterial(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_Set_Custom_PropSharedMaterial)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<PhysicMaterial> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_sharedMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sharedMaterial)
     self->SetMaterial (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider_CUSTOM_INTERNAL_get_bounds(ICallType_Object_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_CUSTOM_INTERNAL_get_bounds)
    ScriptingObjectOfType<Collider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_bounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_bounds)
    *returnValue = self->GetBounds();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Collider_CUSTOM_INTERNAL_CALL_Internal_Raycast(ICallType_Object_Argument col_, const Ray& ray, RaycastHit* hitInfo, float distance)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider_CUSTOM_INTERNAL_CALL_Internal_Raycast)
    ScriptingObjectOfType<Collider> col(col_);
    UNUSED(col);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Raycast)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Raycast)
    
    		hitInfo->collider = NULL;
    
    		bool didHit = col->Raycast (ray, distance, *hitInfo);
    		if (didHit)
    		{
    		#if PLATFORM_WINRT
    			hitInfo->collider = reinterpret_cast<Collider*> (ScriptingGetObjectReference((Collider*)col));
    		#else
    			hitInfo->collider = reinterpret_cast<Collider*> (col.GetScriptingObject());
    		#endif
    			return true;
    		}
    		else
    		{
    			return false;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<BoxCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<BoxCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider_CUSTOM_INTERNAL_get_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider_CUSTOM_INTERNAL_get_size)
    ReadOnlyScriptingObjectOfType<BoxCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_size)
    *returnValue = self->GetSize();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider_CUSTOM_INTERNAL_set_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider_CUSTOM_INTERNAL_set_size)
    ReadOnlyScriptingObjectOfType<BoxCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_size)
    
    self->SetSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SphereCollider_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(SphereCollider_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<SphereCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SphereCollider_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(SphereCollider_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<SphereCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SphereCollider_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SphereCollider_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<SphereCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SphereCollider_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SphereCollider_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<SphereCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION MeshCollider_Get_Custom_PropSharedMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshCollider_Get_Custom_PropSharedMesh)
    ReadOnlyScriptingObjectOfType<MeshCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sharedMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sharedMesh)
    return Scripting::ScriptingWrapperFor(self->GetSharedMesh());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MeshCollider_Set_Custom_PropSharedMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshCollider_Set_Custom_PropSharedMesh)
    ReadOnlyScriptingObjectOfType<MeshCollider> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_sharedMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sharedMesh)
    
    self->SetSharedMesh (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MeshCollider_Get_Custom_PropConvex(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshCollider_Get_Custom_PropConvex)
    ReadOnlyScriptingObjectOfType<MeshCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_convex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_convex)
    return self->GetConvex ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MeshCollider_Set_Custom_PropConvex(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshCollider_Set_Custom_PropConvex)
    ReadOnlyScriptingObjectOfType<MeshCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_convex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_convex)
    
    self->SetConvex (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MeshCollider_Get_Custom_PropSmoothSphereCollisions(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshCollider_Get_Custom_PropSmoothSphereCollisions)
    ReadOnlyScriptingObjectOfType<MeshCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_smoothSphereCollisions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_smoothSphereCollisions)
    return self->GetSmoothSphereCollisions ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MeshCollider_Set_Custom_PropSmoothSphereCollisions(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshCollider_Set_Custom_PropSmoothSphereCollisions)
    ReadOnlyScriptingObjectOfType<MeshCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_smoothSphereCollisions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_smoothSphereCollisions)
    
    self->SetSmoothSphereCollisions (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CapsuleCollider_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CapsuleCollider_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CapsuleCollider_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CapsuleCollider_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CapsuleCollider_Get_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_Get_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_height)
    return self->GetHeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CapsuleCollider_Set_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_Set_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_height)
    
    self->SetHeight (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION CapsuleCollider_Get_Custom_PropDirection(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_Get_Custom_PropDirection)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_direction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_direction)
    return self->GetDirection ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CapsuleCollider_Set_Custom_PropDirection(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(CapsuleCollider_Set_Custom_PropDirection)
    ReadOnlyScriptingObjectOfType<CapsuleCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_direction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_direction)
    
    self->SetDirection (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RaycastCollider_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(RaycastCollider_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<RaycastCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RaycastCollider_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(RaycastCollider_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<RaycastCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION RaycastCollider_Get_Custom_PropLength(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RaycastCollider_Get_Custom_PropLength)
    ReadOnlyScriptingObjectOfType<RaycastCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_length)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_length)
    return self->GetLength ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RaycastCollider_Set_Custom_PropLength(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(RaycastCollider_Set_Custom_PropLength)
    ReadOnlyScriptingObjectOfType<RaycastCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_length)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_length)
    
    self->SetLength (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropSuspensionDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropSuspensionDistance)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_suspensionDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_suspensionDistance)
    return self->GetSuspensionDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_Set_Custom_PropSuspensionDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Set_Custom_PropSuspensionDistance)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_suspensionDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_suspensionDistance)
    
    self->SetSuspensionDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_get_suspensionSpring(ICallType_ReadOnlyUnityEngineObject_Argument self_, JointSpring* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_get_suspensionSpring)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_suspensionSpring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_suspensionSpring)
    *returnValue = self->GetSuspensionSpring();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_set_suspensionSpring(ICallType_ReadOnlyUnityEngineObject_Argument self_, const JointSpring& value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_set_suspensionSpring)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_suspensionSpring)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_suspensionSpring)
    
    self->SetSuspensionSpring (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropMass(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropMass)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mass)
    return self->GetMass ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_Set_Custom_PropMass(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Set_Custom_PropMass)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_mass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mass)
    
    self->SetMass (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_get_forwardFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, WheelFrictionCurve* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_get_forwardFriction)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_forwardFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_forwardFriction)
    *returnValue = self->GetForwardFriction();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_set_forwardFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, const WheelFrictionCurve& value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_set_forwardFriction)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_forwardFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_forwardFriction)
    
    self->SetForwardFriction (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_get_sidewaysFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, WheelFrictionCurve* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_get_sidewaysFriction)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_sidewaysFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_sidewaysFriction)
    *returnValue = self->GetSidewaysFriction();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_INTERNAL_set_sidewaysFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, const WheelFrictionCurve& value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_INTERNAL_set_sidewaysFriction)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_sidewaysFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_sidewaysFriction)
    
    self->SetSidewaysFriction (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropMotorTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropMotorTorque)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_motorTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_motorTorque)
    return self->GetMotorTorque ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_Set_Custom_PropMotorTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Set_Custom_PropMotorTorque)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_motorTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_motorTorque)
    
    self->SetMotorTorque (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropBrakeTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropBrakeTorque)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_brakeTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_brakeTorque)
    return self->GetBrakeTorque ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_Set_Custom_PropBrakeTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Set_Custom_PropBrakeTorque)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_brakeTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_brakeTorque)
    
    self->SetBrakeTorque (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropSteerAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropSteerAngle)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_steerAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_steerAngle)
    return self->GetSteerAngle ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WheelCollider_Set_Custom_PropSteerAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Set_Custom_PropSteerAngle)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_steerAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_steerAngle)
    
    self->SetSteerAngle (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropIsGrounded(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropIsGrounded)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isGrounded)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isGrounded)
    return self->IsGrounded ();
}

#endif


	struct MonoWheelHit
	{
		Vector3f	point;
		Vector3f	normal;
		Vector3f	forwardDir;
		Vector3f	sidewaysDir;
		float		force;
		float		forwardSlip;
		float		sidewaysSlip;
		ScriptingObjectPtr	collider;
	};

	
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION WheelCollider_CUSTOM_GetGroundHit(ICallType_ReadOnlyUnityEngineObject_Argument self_, MonoWheelHit* hit)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_CUSTOM_GetGroundHit)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetGroundHit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetGroundHit)
    
    		WheelHit col;
    		bool didHit = self->GetGroundHit( col );
    		if( didHit )
    		{
    			hit->point = col.point;
    			hit->normal = col.normal;
    			hit->forwardDir = col.forwardDir;
    			hit->sidewaysDir = col.sidewaysDir;
    			hit->force = col.force;
    			hit->forwardSlip = col.forwardSlip;
    			hit->sidewaysSlip = col.sidewaysSlip;
    			hit->collider = Scripting::ScriptingWrapperFor( col.collider );
    			return true;
    		}
    		return false;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WheelCollider_Get_Custom_PropRpm(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WheelCollider_Get_Custom_PropRpm)
    ReadOnlyScriptingObjectOfType<WheelCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rpm)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rpm)
    return self->GetRpm ();
}

#endif

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RaycastHit_CUSTOM_INTERNAL_CALL_CalculateRaycastTexCoord(Vector2fIcall* output, ICallType_Object_Argument col_, const Vector2fIcall& uv, const Vector3f& point, int face, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(RaycastHit_CUSTOM_INTERNAL_CALL_CalculateRaycastTexCoord)
    ScriptingObjectOfType<Collider> col(col_);
    UNUSED(col);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CalculateRaycastTexCoord)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CalculateRaycastTexCoord)
    
    		*output = CalculateRaycastTexcoord(col, uv, point, face, index);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_CUSTOM_Internal_CreateDynamicsMaterial(ICallType_Object_Argument mat_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_CUSTOM_Internal_CreateDynamicsMaterial)
    ScriptingObjectOfType<PhysicMaterial> mat(mat_);
    UNUSED(mat);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateDynamicsMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateDynamicsMaterial)
    
    		PhysicMaterial* material = NEW_OBJECT (PhysicMaterial);
    		SmartResetObject(*material);
    		material->SetNameCpp (name);
    		ConnectScriptingWrapperToObject (mat.GetScriptingObject(), material);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropDynamicFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropDynamicFriction)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_dynamicFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dynamicFriction)
    return self->GetDynamicFriction ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropDynamicFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropDynamicFriction)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_dynamicFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dynamicFriction)
    
    self->SetDynamicFriction (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropStaticFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropStaticFriction)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_staticFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_staticFriction)
    return self->GetStaticFriction ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropStaticFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropStaticFriction)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_staticFriction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_staticFriction)
    
    self->SetStaticFriction (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropBounciness(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropBounciness)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bounciness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bounciness)
    return self->GetBounciness ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropBounciness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropBounciness)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bounciness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bounciness)
    
    self->SetBounciness (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_CUSTOM_INTERNAL_get_frictionDirection2(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_CUSTOM_INTERNAL_get_frictionDirection2)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_frictionDirection2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_frictionDirection2)
    *returnValue = self->GetFrictionDirection2();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_CUSTOM_INTERNAL_set_frictionDirection2(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_CUSTOM_INTERNAL_set_frictionDirection2)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_frictionDirection2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_frictionDirection2)
    
    self->SetFrictionDirection2 (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropDynamicFriction2(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropDynamicFriction2)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_dynamicFriction2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dynamicFriction2)
    return self->GetDynamicFriction2 ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropDynamicFriction2(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropDynamicFriction2)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_dynamicFriction2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dynamicFriction2)
    
    self->SetDynamicFriction2 (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropStaticFriction2(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropStaticFriction2)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_staticFriction2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_staticFriction2)
    return self->GetStaticFriction2 ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropStaticFriction2(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropStaticFriction2)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_staticFriction2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_staticFriction2)
    
    self->SetStaticFriction2 (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropFrictionCombine(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropFrictionCombine)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_frictionCombine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_frictionCombine)
    return self->GetFrictionCombine ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropFrictionCombine(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropFrictionCombine)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_frictionCombine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_frictionCombine)
    
    self->SetFrictionCombine (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION PhysicMaterial_Get_Custom_PropBounceCombine(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Get_Custom_PropBounceCombine)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bounceCombine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bounceCombine)
    return self->GetBounceCombine ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PhysicMaterial_Set_Custom_PropBounceCombine(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(PhysicMaterial_Set_Custom_PropBounceCombine)
    ReadOnlyScriptingObjectOfType<PhysicMaterial> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bounceCombine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bounceCombine)
    
    self->SetBounceCombine (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION CharacterController_CUSTOM_INTERNAL_CALL_SimpleMove(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& speed)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_CUSTOM_INTERNAL_CALL_SimpleMove)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SimpleMove)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SimpleMove)
    return self->SimpleMove(speed);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION CharacterController_CUSTOM_INTERNAL_CALL_Move(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& motion)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_CUSTOM_INTERNAL_CALL_Move)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Move)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Move)
    return self->Move(motion);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropIsGrounded(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropIsGrounded)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isGrounded)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isGrounded)
    return self->IsGrounded ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_CUSTOM_INTERNAL_get_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_CUSTOM_INTERNAL_get_velocity)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_velocity)
    *returnValue = self->GetVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropCollisionFlags(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropCollisionFlags)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collisionFlags)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collisionFlags)
    return self->GetCollisionFlags ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_height)
    return self->GetHeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_Set_Custom_PropHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Set_Custom_PropHeight)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_height)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_height)
    
    self->SetHeight (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropSlopeLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropSlopeLimit)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_slopeLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_slopeLimit)
    return self->GetSlopeLimit ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_Set_Custom_PropSlopeLimit(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Set_Custom_PropSlopeLimit)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_slopeLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_slopeLimit)
    
    self->SetSlopeLimit (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropStepOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropStepOffset)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stepOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stepOffset)
    return self->GetStepOffset ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_Set_Custom_PropStepOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Set_Custom_PropStepOffset)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stepOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stepOffset)
    
    self->SetStepOffset (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION CharacterController_Get_Custom_PropDetectCollisions(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Get_Custom_PropDetectCollisions)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detectCollisions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detectCollisions)
    return self->GetDetectCollisions ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CharacterController_Set_Custom_PropDetectCollisions(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(CharacterController_Set_Custom_PropDetectCollisions)
    ReadOnlyScriptingObjectOfType<CharacterController> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_detectCollisions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_detectCollisions)
    
    self->SetDetectCollisions (value);
    
}

#endif
#if ENABLE_CLOTH
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropBendingStiffness(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropBendingStiffness)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bendingStiffness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bendingStiffness)
    return self->GetBendingStiffness ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropBendingStiffness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropBendingStiffness)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bendingStiffness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bendingStiffness)
    
    self->SetBendingStiffness (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropStretchingStiffness(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropStretchingStiffness)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stretchingStiffness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stretchingStiffness)
    return self->GetStretchingStiffness ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropStretchingStiffness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropStretchingStiffness)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stretchingStiffness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stretchingStiffness)
    
    self->SetStretchingStiffness (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropDamping(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropDamping)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_damping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_damping)
    return self->GetDamping ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropDamping(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropDamping)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_damping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_damping)
    
    self->SetDamping (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropThickness(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropThickness)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_thickness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_thickness)
    return self->GetThickness ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropThickness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropThickness)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_thickness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_thickness)
    
    self->SetThickness (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_CUSTOM_INTERNAL_get_externalAcceleration(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_CUSTOM_INTERNAL_get_externalAcceleration)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_externalAcceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_externalAcceleration)
    *returnValue = self->GetExternalAcceleration();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_CUSTOM_INTERNAL_set_externalAcceleration(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_CUSTOM_INTERNAL_set_externalAcceleration)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_externalAcceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_externalAcceleration)
    
    self->SetExternalAcceleration (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_CUSTOM_INTERNAL_get_randomAcceleration(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_CUSTOM_INTERNAL_get_randomAcceleration)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_randomAcceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_randomAcceleration)
    *returnValue = self->GetRandomAcceleration();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_CUSTOM_INTERNAL_set_randomAcceleration(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_CUSTOM_INTERNAL_set_randomAcceleration)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_randomAcceleration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_randomAcceleration)
    
    self->SetRandomAcceleration (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropUseGravity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropUseGravity)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useGravity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useGravity)
    return self->GetUseGravity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropUseGravity(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropUseGravity)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useGravity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useGravity)
    
    self->SetUseGravity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropSelfCollision(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropSelfCollision)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_selfCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_selfCollision)
    return self->GetSelfCollision ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropSelfCollision(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropSelfCollision)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_selfCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_selfCollision)
    
    self->SetSelfCollision (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cloth_Set_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Set_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    self->SetEnabled (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropVertices(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropVertices)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_vertices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_vertices)
    return CreateScriptingArray(&self->GetVertices()[0], self->GetVertices().size(), GetMonoManager().GetCommonClasses().vector3);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Cloth_Get_Custom_PropNormals(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cloth_Get_Custom_PropNormals)
    ReadOnlyScriptingObjectOfType<Cloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_normals)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_normals)
    return CreateScriptingArray(&self->GetNormals()[0], self->GetNormals().size(), GetMonoManager().GetCommonClasses().vector3);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropMesh)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mesh)
    return Scripting::ScriptingWrapperFor(self->GetMesh());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropMesh)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_mesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mesh)
    
    self->SetMesh (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropFriction)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_friction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_friction)
    return self->GetFriction ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropFriction(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropFriction)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_friction)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_friction)
    
    self->SetFriction (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropDensity)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_density)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_density)
    return self->GetDensity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropDensity)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_density)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_density)
    
    self->SetDensity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropPressure(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropPressure)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pressure)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pressure)
    return self->GetPressure ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropPressure(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropPressure)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_pressure)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pressure)
    
    self->SetPressure (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropCollisionResponse(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropCollisionResponse)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collisionResponse)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collisionResponse)
    return self->GetCollisionResponse ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropCollisionResponse(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropCollisionResponse)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_collisionResponse)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_collisionResponse)
    
    self->SetCollisionResponse (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropTearFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropTearFactor)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_tearFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_tearFactor)
    return self->GetTearFactor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropTearFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropTearFactor)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_tearFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_tearFactor)
    
    self->SetTearFactor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropAttachmentTearFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropAttachmentTearFactor)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_attachmentTearFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_attachmentTearFactor)
    return self->GetAttachmentTearFactor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropAttachmentTearFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropAttachmentTearFactor)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_attachmentTearFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_attachmentTearFactor)
    
    self->SetAttachmentTearFactor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropAttachmentResponse(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropAttachmentResponse)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_attachmentResponse)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_attachmentResponse)
    return self->GetAttachmentResponse ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_Set_Custom_PropAttachmentResponse(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Set_Custom_PropAttachmentResponse)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_attachmentResponse)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_attachmentResponse)
    
    self->SetAttachmentResponse (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION InteractiveCloth_Get_Custom_PropIsTeared(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_Get_Custom_PropIsTeared)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isTeared)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isTeared)
    return self->GetIsTeared ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_CUSTOM_INTERNAL_CALL_AddForceAtPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& force, const Vector3f& position, float radius, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_CUSTOM_INTERNAL_CALL_AddForceAtPosition)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddForceAtPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddForceAtPosition)
     self->AddForceAtPosition (force, position, radius, mode); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_CUSTOM_AttachToCollider(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument collider_, ScriptingBool tearable, ScriptingBool twoWayInteraction)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_CUSTOM_AttachToCollider)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    ScriptingObjectOfType<Collider> collider(collider_);
    UNUSED(collider);
    SCRIPTINGAPI_STACK_CHECK(AttachToCollider)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AttachToCollider)
     self->AttachToCollider (collider, tearable, twoWayInteraction); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION InteractiveCloth_CUSTOM_DetachFromCollider(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument collider_)
{
    SCRIPTINGAPI_ETW_ENTRY(InteractiveCloth_CUSTOM_DetachFromCollider)
    ReadOnlyScriptingObjectOfType<InteractiveCloth> self(self_);
    UNUSED(self);
    ScriptingObjectOfType<Collider> collider(collider_);
    UNUSED(collider);
    SCRIPTINGAPI_STACK_CHECK(DetachFromCollider)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DetachFromCollider)
     self->DetachFromCollider(collider); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION SkinnedCloth_Get_Custom_PropCoefficients(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_Get_Custom_PropCoefficients)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_coefficients)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_coefficients)
    #if !PLATFORM_WINRT
    return CreateScriptingArray(&self->GetCoefficients()[0], self->GetCoefficients().size(), GetMonoManager().GetBuiltinMonoClass("ClothSkinningCoefficient"));
    #else
    return SCRIPTING_NULL;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedCloth_Set_Custom_PropCoefficients(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_Set_Custom_PropCoefficients)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_coefficients)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_coefficients)
    
    #if !PLATFORM_WINRT
    int count = mono_array_length_safe(value);
    if (count == self->GetCoefficients().size())
    self->SetCoefficients(&GetMonoArrayElement<SkinnedCloth::ClothConstrainCoefficients> (value, 0));
    else
    ErrorString ("Number of coefficients must match number of vertices!");
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SkinnedCloth_Get_Custom_PropWorldVelocityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_Get_Custom_PropWorldVelocityScale)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_worldVelocityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_worldVelocityScale)
    return self->GetWorldVelocityScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedCloth_Set_Custom_PropWorldVelocityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_Set_Custom_PropWorldVelocityScale)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_worldVelocityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_worldVelocityScale)
    
    self->SetWorldVelocityScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SkinnedCloth_Get_Custom_PropWorldAccelerationScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_Get_Custom_PropWorldAccelerationScale)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_worldAccelerationScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_worldAccelerationScale)
    return self->GetWorldAccelerationScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedCloth_Set_Custom_PropWorldAccelerationScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_Set_Custom_PropWorldAccelerationScale)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_worldAccelerationScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_worldAccelerationScale)
    
    self->SetWorldAccelerationScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedCloth_CUSTOM_SetEnabledFading(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool enabled, float interpolationTime)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedCloth_CUSTOM_SetEnabledFading)
    ReadOnlyScriptingObjectOfType<SkinnedCloth> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetEnabledFading)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetEnabledFading)
     self->SetEnabledFading (enabled, interpolationTime); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ClothRenderer_Get_Custom_PropPauseWhenNotVisible(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ClothRenderer_Get_Custom_PropPauseWhenNotVisible)
    ReadOnlyScriptingObjectOfType<ClothRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pauseWhenNotVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pauseWhenNotVisible)
    return self->GetPauseWhenNotVisible ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ClothRenderer_Set_Custom_PropPauseWhenNotVisible(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ClothRenderer_Set_Custom_PropPauseWhenNotVisible)
    ReadOnlyScriptingObjectOfType<ClothRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_pauseWhenNotVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pauseWhenNotVisible)
    
    self->SetPauseWhenNotVisible (value);
    
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_PHYSICS
void Register_UnityEngine_Physics_INTERNAL_get_gravity()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_get_gravity" , (gpointer)& Physics_CUSTOM_INTERNAL_get_gravity );
}

void Register_UnityEngine_Physics_INTERNAL_set_gravity()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_set_gravity" , (gpointer)& Physics_CUSTOM_INTERNAL_set_gravity );
}

void Register_UnityEngine_Physics_get_minPenetrationForPenalty()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_minPenetrationForPenalty" , (gpointer)& Physics_Get_Custom_PropMinPenetrationForPenalty );
}

void Register_UnityEngine_Physics_set_minPenetrationForPenalty()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_minPenetrationForPenalty" , (gpointer)& Physics_Set_Custom_PropMinPenetrationForPenalty );
}

void Register_UnityEngine_Physics_get_bounceThreshold()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_bounceThreshold" , (gpointer)& Physics_Get_Custom_PropBounceThreshold );
}

void Register_UnityEngine_Physics_set_bounceThreshold()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_bounceThreshold" , (gpointer)& Physics_Set_Custom_PropBounceThreshold );
}

void Register_UnityEngine_Physics_get_sleepVelocity()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_sleepVelocity" , (gpointer)& Physics_Get_Custom_PropSleepVelocity );
}

void Register_UnityEngine_Physics_set_sleepVelocity()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_sleepVelocity" , (gpointer)& Physics_Set_Custom_PropSleepVelocity );
}

void Register_UnityEngine_Physics_get_sleepAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_sleepAngularVelocity" , (gpointer)& Physics_Get_Custom_PropSleepAngularVelocity );
}

void Register_UnityEngine_Physics_set_sleepAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_sleepAngularVelocity" , (gpointer)& Physics_Set_Custom_PropSleepAngularVelocity );
}

void Register_UnityEngine_Physics_get_maxAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_maxAngularVelocity" , (gpointer)& Physics_Get_Custom_PropMaxAngularVelocity );
}

void Register_UnityEngine_Physics_set_maxAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_maxAngularVelocity" , (gpointer)& Physics_Set_Custom_PropMaxAngularVelocity );
}

void Register_UnityEngine_Physics_get_solverIterationCount()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_solverIterationCount" , (gpointer)& Physics_Get_Custom_PropSolverIterationCount );
}

void Register_UnityEngine_Physics_set_solverIterationCount()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_solverIterationCount" , (gpointer)& Physics_Set_Custom_PropSolverIterationCount );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_Internal_Raycast()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_Internal_Raycast" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_Internal_Raycast );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_Internal_CapsuleCast()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_Internal_CapsuleCast" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_Internal_CapsuleCast );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_Internal_RaycastTest()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_Internal_RaycastTest" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_Internal_RaycastTest );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_RaycastAll()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_RaycastAll" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_RaycastAll );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_OverlapSphere()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_OverlapSphere" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_OverlapSphere );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_CapsuleCastAll()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_CapsuleCastAll" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_CapsuleCastAll );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_CheckSphere()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_CheckSphere" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_CheckSphere );
}

void Register_UnityEngine_Physics_INTERNAL_CALL_CheckCapsule()
{
    scripting_add_internal_call( "UnityEngine.Physics::INTERNAL_CALL_CheckCapsule" , (gpointer)& Physics_CUSTOM_INTERNAL_CALL_CheckCapsule );
}

void Register_UnityEngine_Physics_get_penetrationPenaltyForce()
{
    scripting_add_internal_call( "UnityEngine.Physics::get_penetrationPenaltyForce" , (gpointer)& Physics_Get_Custom_PropPenetrationPenaltyForce );
}

void Register_UnityEngine_Physics_set_penetrationPenaltyForce()
{
    scripting_add_internal_call( "UnityEngine.Physics::set_penetrationPenaltyForce" , (gpointer)& Physics_Set_Custom_PropPenetrationPenaltyForce );
}

void Register_UnityEngine_Physics_IgnoreCollision()
{
    scripting_add_internal_call( "UnityEngine.Physics::IgnoreCollision" , (gpointer)& Physics_CUSTOM_IgnoreCollision );
}

void Register_UnityEngine_Physics_IgnoreLayerCollision()
{
    scripting_add_internal_call( "UnityEngine.Physics::IgnoreLayerCollision" , (gpointer)& Physics_CUSTOM_IgnoreLayerCollision );
}

void Register_UnityEngine_Physics_GetIgnoreLayerCollision()
{
    scripting_add_internal_call( "UnityEngine.Physics::GetIgnoreLayerCollision" , (gpointer)& Physics_CUSTOM_GetIgnoreLayerCollision );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_velocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_velocity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_velocity );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_velocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_velocity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_velocity );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_angularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_angularVelocity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_angularVelocity );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_angularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_angularVelocity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_angularVelocity );
}

void Register_UnityEngine_Rigidbody_get_drag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_drag" , (gpointer)& Rigidbody_Get_Custom_PropDrag );
}

void Register_UnityEngine_Rigidbody_set_drag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_drag" , (gpointer)& Rigidbody_Set_Custom_PropDrag );
}

void Register_UnityEngine_Rigidbody_get_angularDrag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_angularDrag" , (gpointer)& Rigidbody_Get_Custom_PropAngularDrag );
}

void Register_UnityEngine_Rigidbody_set_angularDrag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_angularDrag" , (gpointer)& Rigidbody_Set_Custom_PropAngularDrag );
}

void Register_UnityEngine_Rigidbody_get_mass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_mass" , (gpointer)& Rigidbody_Get_Custom_PropMass );
}

void Register_UnityEngine_Rigidbody_set_mass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_mass" , (gpointer)& Rigidbody_Set_Custom_PropMass );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_SetDensity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_SetDensity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_SetDensity );
}

void Register_UnityEngine_Rigidbody_get_useGravity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_useGravity" , (gpointer)& Rigidbody_Get_Custom_PropUseGravity );
}

void Register_UnityEngine_Rigidbody_set_useGravity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_useGravity" , (gpointer)& Rigidbody_Set_Custom_PropUseGravity );
}

void Register_UnityEngine_Rigidbody_get_isKinematic()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_isKinematic" , (gpointer)& Rigidbody_Get_Custom_PropIsKinematic );
}

void Register_UnityEngine_Rigidbody_set_isKinematic()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_isKinematic" , (gpointer)& Rigidbody_Set_Custom_PropIsKinematic );
}

void Register_UnityEngine_Rigidbody_get_freezeRotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_freezeRotation" , (gpointer)& Rigidbody_Get_Custom_PropFreezeRotation );
}

void Register_UnityEngine_Rigidbody_set_freezeRotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_freezeRotation" , (gpointer)& Rigidbody_Set_Custom_PropFreezeRotation );
}

void Register_UnityEngine_Rigidbody_get_constraints()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_constraints" , (gpointer)& Rigidbody_Get_Custom_PropConstraints );
}

void Register_UnityEngine_Rigidbody_set_constraints()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_constraints" , (gpointer)& Rigidbody_Set_Custom_PropConstraints );
}

void Register_UnityEngine_Rigidbody_get_collisionDetectionMode()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_collisionDetectionMode" , (gpointer)& Rigidbody_Get_Custom_PropCollisionDetectionMode );
}

void Register_UnityEngine_Rigidbody_set_collisionDetectionMode()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_collisionDetectionMode" , (gpointer)& Rigidbody_Set_Custom_PropCollisionDetectionMode );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_AddForce()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_AddForce" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_AddForce );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_AddRelativeForce()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeForce" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeForce );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_AddTorque()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_AddTorque" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_AddTorque );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_AddRelativeTorque()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeTorque" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeTorque );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_AddForceAtPosition()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_AddForceAtPosition" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_AddForceAtPosition );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_AddExplosionForce()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_AddExplosionForce" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_AddExplosionForce );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_ClosestPointOnBounds()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_ClosestPointOnBounds" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_GetRelativePointVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_GetRelativePointVelocity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_GetRelativePointVelocity );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_GetPointVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_GetPointVelocity" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_GetPointVelocity );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_centerOfMass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_centerOfMass" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_centerOfMass );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_centerOfMass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_centerOfMass" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_centerOfMass );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_worldCenterOfMass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_worldCenterOfMass" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_worldCenterOfMass );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_inertiaTensorRotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_inertiaTensorRotation" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_inertiaTensorRotation );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_inertiaTensorRotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_inertiaTensorRotation" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_inertiaTensorRotation );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_inertiaTensor()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_inertiaTensor" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_inertiaTensor );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_inertiaTensor()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_inertiaTensor" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_inertiaTensor );
}

void Register_UnityEngine_Rigidbody_get_detectCollisions()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_detectCollisions" , (gpointer)& Rigidbody_Get_Custom_PropDetectCollisions );
}

void Register_UnityEngine_Rigidbody_set_detectCollisions()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_detectCollisions" , (gpointer)& Rigidbody_Set_Custom_PropDetectCollisions );
}

void Register_UnityEngine_Rigidbody_get_useConeFriction()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_useConeFriction" , (gpointer)& Rigidbody_Get_Custom_PropUseConeFriction );
}

void Register_UnityEngine_Rigidbody_set_useConeFriction()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_useConeFriction" , (gpointer)& Rigidbody_Set_Custom_PropUseConeFriction );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_position()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_position" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_position );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_position()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_position" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_position );
}

void Register_UnityEngine_Rigidbody_INTERNAL_get_rotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_get_rotation" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_get_rotation );
}

void Register_UnityEngine_Rigidbody_INTERNAL_set_rotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_set_rotation" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_set_rotation );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_MovePosition()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_MovePosition" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_MovePosition );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_MoveRotation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_MoveRotation" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_MoveRotation );
}

void Register_UnityEngine_Rigidbody_get_interpolation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_interpolation" , (gpointer)& Rigidbody_Get_Custom_PropInterpolation );
}

void Register_UnityEngine_Rigidbody_set_interpolation()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_interpolation" , (gpointer)& Rigidbody_Set_Custom_PropInterpolation );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_Sleep()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_Sleep" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_Sleep );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_IsSleeping()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_IsSleeping" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_IsSleeping );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_WakeUp()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_WakeUp" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_WakeUp );
}

void Register_UnityEngine_Rigidbody_get_solverIterationCount()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_solverIterationCount" , (gpointer)& Rigidbody_Get_Custom_PropSolverIterationCount );
}

void Register_UnityEngine_Rigidbody_set_solverIterationCount()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_solverIterationCount" , (gpointer)& Rigidbody_Set_Custom_PropSolverIterationCount );
}

void Register_UnityEngine_Rigidbody_get_sleepVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_sleepVelocity" , (gpointer)& Rigidbody_Get_Custom_PropSleepVelocity );
}

void Register_UnityEngine_Rigidbody_set_sleepVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_sleepVelocity" , (gpointer)& Rigidbody_Set_Custom_PropSleepVelocity );
}

void Register_UnityEngine_Rigidbody_get_sleepAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_sleepAngularVelocity" , (gpointer)& Rigidbody_Get_Custom_PropSleepAngularVelocity );
}

void Register_UnityEngine_Rigidbody_set_sleepAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_sleepAngularVelocity" , (gpointer)& Rigidbody_Set_Custom_PropSleepAngularVelocity );
}

void Register_UnityEngine_Rigidbody_get_maxAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::get_maxAngularVelocity" , (gpointer)& Rigidbody_Get_Custom_PropMaxAngularVelocity );
}

void Register_UnityEngine_Rigidbody_set_maxAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::set_maxAngularVelocity" , (gpointer)& Rigidbody_Set_Custom_PropMaxAngularVelocity );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_SweepTest()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_SweepTest" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_SweepTest );
}

void Register_UnityEngine_Rigidbody_INTERNAL_CALL_SweepTestAll()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody::INTERNAL_CALL_SweepTestAll" , (gpointer)& Rigidbody_CUSTOM_INTERNAL_CALL_SweepTestAll );
}

void Register_UnityEngine_Joint_get_connectedBody()
{
    scripting_add_internal_call( "UnityEngine.Joint::get_connectedBody" , (gpointer)& Joint_Get_Custom_PropConnectedBody );
}

void Register_UnityEngine_Joint_set_connectedBody()
{
    scripting_add_internal_call( "UnityEngine.Joint::set_connectedBody" , (gpointer)& Joint_Set_Custom_PropConnectedBody );
}

void Register_UnityEngine_Joint_INTERNAL_get_axis()
{
    scripting_add_internal_call( "UnityEngine.Joint::INTERNAL_get_axis" , (gpointer)& Joint_CUSTOM_INTERNAL_get_axis );
}

void Register_UnityEngine_Joint_INTERNAL_set_axis()
{
    scripting_add_internal_call( "UnityEngine.Joint::INTERNAL_set_axis" , (gpointer)& Joint_CUSTOM_INTERNAL_set_axis );
}

void Register_UnityEngine_Joint_INTERNAL_get_anchor()
{
    scripting_add_internal_call( "UnityEngine.Joint::INTERNAL_get_anchor" , (gpointer)& Joint_CUSTOM_INTERNAL_get_anchor );
}

void Register_UnityEngine_Joint_INTERNAL_set_anchor()
{
    scripting_add_internal_call( "UnityEngine.Joint::INTERNAL_set_anchor" , (gpointer)& Joint_CUSTOM_INTERNAL_set_anchor );
}

void Register_UnityEngine_Joint_get_breakForce()
{
    scripting_add_internal_call( "UnityEngine.Joint::get_breakForce" , (gpointer)& Joint_Get_Custom_PropBreakForce );
}

void Register_UnityEngine_Joint_set_breakForce()
{
    scripting_add_internal_call( "UnityEngine.Joint::set_breakForce" , (gpointer)& Joint_Set_Custom_PropBreakForce );
}

void Register_UnityEngine_Joint_get_breakTorque()
{
    scripting_add_internal_call( "UnityEngine.Joint::get_breakTorque" , (gpointer)& Joint_Get_Custom_PropBreakTorque );
}

void Register_UnityEngine_Joint_set_breakTorque()
{
    scripting_add_internal_call( "UnityEngine.Joint::set_breakTorque" , (gpointer)& Joint_Set_Custom_PropBreakTorque );
}

void Register_UnityEngine_HingeJoint_INTERNAL_get_motor()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::INTERNAL_get_motor" , (gpointer)& HingeJoint_CUSTOM_INTERNAL_get_motor );
}

void Register_UnityEngine_HingeJoint_INTERNAL_set_motor()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::INTERNAL_set_motor" , (gpointer)& HingeJoint_CUSTOM_INTERNAL_set_motor );
}

void Register_UnityEngine_HingeJoint_INTERNAL_get_limits()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::INTERNAL_get_limits" , (gpointer)& HingeJoint_CUSTOM_INTERNAL_get_limits );
}

void Register_UnityEngine_HingeJoint_INTERNAL_set_limits()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::INTERNAL_set_limits" , (gpointer)& HingeJoint_CUSTOM_INTERNAL_set_limits );
}

void Register_UnityEngine_HingeJoint_INTERNAL_get_spring()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::INTERNAL_get_spring" , (gpointer)& HingeJoint_CUSTOM_INTERNAL_get_spring );
}

void Register_UnityEngine_HingeJoint_INTERNAL_set_spring()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::INTERNAL_set_spring" , (gpointer)& HingeJoint_CUSTOM_INTERNAL_set_spring );
}

void Register_UnityEngine_HingeJoint_get_useMotor()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::get_useMotor" , (gpointer)& HingeJoint_Get_Custom_PropUseMotor );
}

void Register_UnityEngine_HingeJoint_set_useMotor()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::set_useMotor" , (gpointer)& HingeJoint_Set_Custom_PropUseMotor );
}

void Register_UnityEngine_HingeJoint_get_useLimits()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::get_useLimits" , (gpointer)& HingeJoint_Get_Custom_PropUseLimits );
}

void Register_UnityEngine_HingeJoint_set_useLimits()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::set_useLimits" , (gpointer)& HingeJoint_Set_Custom_PropUseLimits );
}

void Register_UnityEngine_HingeJoint_get_useSpring()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::get_useSpring" , (gpointer)& HingeJoint_Get_Custom_PropUseSpring );
}

void Register_UnityEngine_HingeJoint_set_useSpring()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::set_useSpring" , (gpointer)& HingeJoint_Set_Custom_PropUseSpring );
}

void Register_UnityEngine_HingeJoint_get_velocity()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::get_velocity" , (gpointer)& HingeJoint_Get_Custom_PropVelocity );
}

void Register_UnityEngine_HingeJoint_get_angle()
{
    scripting_add_internal_call( "UnityEngine.HingeJoint::get_angle" , (gpointer)& HingeJoint_Get_Custom_PropAngle );
}

void Register_UnityEngine_SpringJoint_get_spring()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::get_spring" , (gpointer)& SpringJoint_Get_Custom_PropSpring );
}

void Register_UnityEngine_SpringJoint_set_spring()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::set_spring" , (gpointer)& SpringJoint_Set_Custom_PropSpring );
}

void Register_UnityEngine_SpringJoint_get_damper()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::get_damper" , (gpointer)& SpringJoint_Get_Custom_PropDamper );
}

void Register_UnityEngine_SpringJoint_set_damper()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::set_damper" , (gpointer)& SpringJoint_Set_Custom_PropDamper );
}

void Register_UnityEngine_SpringJoint_get_minDistance()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::get_minDistance" , (gpointer)& SpringJoint_Get_Custom_PropMinDistance );
}

void Register_UnityEngine_SpringJoint_set_minDistance()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::set_minDistance" , (gpointer)& SpringJoint_Set_Custom_PropMinDistance );
}

void Register_UnityEngine_SpringJoint_get_maxDistance()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::get_maxDistance" , (gpointer)& SpringJoint_Get_Custom_PropMaxDistance );
}

void Register_UnityEngine_SpringJoint_set_maxDistance()
{
    scripting_add_internal_call( "UnityEngine.SpringJoint::set_maxDistance" , (gpointer)& SpringJoint_Set_Custom_PropMaxDistance );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_swingAxis()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_swingAxis" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_swingAxis );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_swingAxis()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_swingAxis" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_swingAxis );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_lowTwistLimit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_lowTwistLimit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_lowTwistLimit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_lowTwistLimit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_lowTwistLimit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_lowTwistLimit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_highTwistLimit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_highTwistLimit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_highTwistLimit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_highTwistLimit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_highTwistLimit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_highTwistLimit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_swing1Limit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_swing1Limit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_swing1Limit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_swing1Limit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_swing1Limit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_swing1Limit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_swing2Limit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_swing2Limit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_swing2Limit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_swing2Limit()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_swing2Limit" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_swing2Limit );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_targetRotation()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_targetRotation" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_targetRotation );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_targetRotation()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_targetRotation" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_targetRotation );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_targetAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_targetAngularVelocity" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_targetAngularVelocity );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_targetAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_targetAngularVelocity" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_targetAngularVelocity );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_get_rotationDrive()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_get_rotationDrive" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_get_rotationDrive );
}

void Register_UnityEngine_CharacterJoint_INTERNAL_set_rotationDrive()
{
    scripting_add_internal_call( "UnityEngine.CharacterJoint::INTERNAL_set_rotationDrive" , (gpointer)& CharacterJoint_CUSTOM_INTERNAL_set_rotationDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_secondaryAxis()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_secondaryAxis" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_secondaryAxis );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_secondaryAxis()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_secondaryAxis" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_secondaryAxis );
}

void Register_UnityEngine_ConfigurableJoint_get_xMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_xMotion" , (gpointer)& ConfigurableJoint_Get_Custom_PropXMotion );
}

void Register_UnityEngine_ConfigurableJoint_set_xMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_xMotion" , (gpointer)& ConfigurableJoint_Set_Custom_PropXMotion );
}

void Register_UnityEngine_ConfigurableJoint_get_yMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_yMotion" , (gpointer)& ConfigurableJoint_Get_Custom_PropYMotion );
}

void Register_UnityEngine_ConfigurableJoint_set_yMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_yMotion" , (gpointer)& ConfigurableJoint_Set_Custom_PropYMotion );
}

void Register_UnityEngine_ConfigurableJoint_get_zMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_zMotion" , (gpointer)& ConfigurableJoint_Get_Custom_PropZMotion );
}

void Register_UnityEngine_ConfigurableJoint_set_zMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_zMotion" , (gpointer)& ConfigurableJoint_Set_Custom_PropZMotion );
}

void Register_UnityEngine_ConfigurableJoint_get_angularXMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_angularXMotion" , (gpointer)& ConfigurableJoint_Get_Custom_PropAngularXMotion );
}

void Register_UnityEngine_ConfigurableJoint_set_angularXMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_angularXMotion" , (gpointer)& ConfigurableJoint_Set_Custom_PropAngularXMotion );
}

void Register_UnityEngine_ConfigurableJoint_get_angularYMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_angularYMotion" , (gpointer)& ConfigurableJoint_Get_Custom_PropAngularYMotion );
}

void Register_UnityEngine_ConfigurableJoint_set_angularYMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_angularYMotion" , (gpointer)& ConfigurableJoint_Set_Custom_PropAngularYMotion );
}

void Register_UnityEngine_ConfigurableJoint_get_angularZMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_angularZMotion" , (gpointer)& ConfigurableJoint_Get_Custom_PropAngularZMotion );
}

void Register_UnityEngine_ConfigurableJoint_set_angularZMotion()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_angularZMotion" , (gpointer)& ConfigurableJoint_Set_Custom_PropAngularZMotion );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_linearLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_linearLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_linearLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_linearLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_linearLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_linearLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_lowAngularXLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_lowAngularXLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_lowAngularXLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_lowAngularXLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_lowAngularXLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_lowAngularXLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_highAngularXLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_highAngularXLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_highAngularXLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_highAngularXLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_highAngularXLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_highAngularXLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_angularYLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_angularYLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_angularYLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_angularYLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_angularYLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_angularYLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_angularZLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_angularZLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_angularZLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_angularZLimit()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_angularZLimit" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_angularZLimit );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_targetPosition()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_targetPosition" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_targetPosition );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_targetPosition()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_targetPosition" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_targetPosition );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_targetVelocity()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_targetVelocity" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_targetVelocity );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_targetVelocity()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_targetVelocity" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_targetVelocity );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_xDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_xDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_xDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_xDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_xDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_xDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_yDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_yDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_yDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_yDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_yDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_yDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_zDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_zDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_zDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_zDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_zDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_zDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_targetRotation()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_targetRotation" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_targetRotation );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_targetRotation()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_targetRotation" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_targetRotation );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_targetAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_targetAngularVelocity" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_targetAngularVelocity );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_targetAngularVelocity()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_targetAngularVelocity" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_targetAngularVelocity );
}

void Register_UnityEngine_ConfigurableJoint_get_rotationDriveMode()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_rotationDriveMode" , (gpointer)& ConfigurableJoint_Get_Custom_PropRotationDriveMode );
}

void Register_UnityEngine_ConfigurableJoint_set_rotationDriveMode()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_rotationDriveMode" , (gpointer)& ConfigurableJoint_Set_Custom_PropRotationDriveMode );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_angularXDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_angularXDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_angularXDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_angularXDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_angularXDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_angularXDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_angularYZDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_angularYZDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_angularYZDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_angularYZDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_angularYZDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_angularYZDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_get_slerpDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_get_slerpDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_get_slerpDrive );
}

void Register_UnityEngine_ConfigurableJoint_INTERNAL_set_slerpDrive()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::INTERNAL_set_slerpDrive" , (gpointer)& ConfigurableJoint_CUSTOM_INTERNAL_set_slerpDrive );
}

void Register_UnityEngine_ConfigurableJoint_get_projectionMode()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_projectionMode" , (gpointer)& ConfigurableJoint_Get_Custom_PropProjectionMode );
}

void Register_UnityEngine_ConfigurableJoint_set_projectionMode()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_projectionMode" , (gpointer)& ConfigurableJoint_Set_Custom_PropProjectionMode );
}

void Register_UnityEngine_ConfigurableJoint_get_projectionDistance()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_projectionDistance" , (gpointer)& ConfigurableJoint_Get_Custom_PropProjectionDistance );
}

void Register_UnityEngine_ConfigurableJoint_set_projectionDistance()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_projectionDistance" , (gpointer)& ConfigurableJoint_Set_Custom_PropProjectionDistance );
}

void Register_UnityEngine_ConfigurableJoint_get_projectionAngle()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_projectionAngle" , (gpointer)& ConfigurableJoint_Get_Custom_PropProjectionAngle );
}

void Register_UnityEngine_ConfigurableJoint_set_projectionAngle()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_projectionAngle" , (gpointer)& ConfigurableJoint_Set_Custom_PropProjectionAngle );
}

void Register_UnityEngine_ConfigurableJoint_get_configuredInWorldSpace()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_configuredInWorldSpace" , (gpointer)& ConfigurableJoint_Get_Custom_PropConfiguredInWorldSpace );
}

void Register_UnityEngine_ConfigurableJoint_set_configuredInWorldSpace()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_configuredInWorldSpace" , (gpointer)& ConfigurableJoint_Set_Custom_PropConfiguredInWorldSpace );
}

void Register_UnityEngine_ConfigurableJoint_get_swapBodies()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::get_swapBodies" , (gpointer)& ConfigurableJoint_Get_Custom_PropSwapBodies );
}

void Register_UnityEngine_ConfigurableJoint_set_swapBodies()
{
    scripting_add_internal_call( "UnityEngine.ConfigurableJoint::set_swapBodies" , (gpointer)& ConfigurableJoint_Set_Custom_PropSwapBodies );
}

void Register_UnityEngine_ConstantForce_INTERNAL_get_force()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_get_force" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_get_force );
}

void Register_UnityEngine_ConstantForce_INTERNAL_set_force()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_set_force" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_set_force );
}

void Register_UnityEngine_ConstantForce_INTERNAL_get_relativeForce()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_get_relativeForce" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_get_relativeForce );
}

void Register_UnityEngine_ConstantForce_INTERNAL_set_relativeForce()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_set_relativeForce" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_set_relativeForce );
}

void Register_UnityEngine_ConstantForce_INTERNAL_get_torque()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_get_torque" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_get_torque );
}

void Register_UnityEngine_ConstantForce_INTERNAL_set_torque()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_set_torque" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_set_torque );
}

void Register_UnityEngine_ConstantForce_INTERNAL_get_relativeTorque()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_get_relativeTorque" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_get_relativeTorque );
}

void Register_UnityEngine_ConstantForce_INTERNAL_set_relativeTorque()
{
    scripting_add_internal_call( "UnityEngine.ConstantForce::INTERNAL_set_relativeTorque" , (gpointer)& ConstantForce_CUSTOM_INTERNAL_set_relativeTorque );
}

void Register_UnityEngine_Collider_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Collider::get_enabled" , (gpointer)& Collider_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Collider_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Collider::set_enabled" , (gpointer)& Collider_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Collider_get_attachedRigidbody()
{
    scripting_add_internal_call( "UnityEngine.Collider::get_attachedRigidbody" , (gpointer)& Collider_Get_Custom_PropAttachedRigidbody );
}

void Register_UnityEngine_Collider_get_isTrigger()
{
    scripting_add_internal_call( "UnityEngine.Collider::get_isTrigger" , (gpointer)& Collider_Get_Custom_PropIsTrigger );
}

void Register_UnityEngine_Collider_set_isTrigger()
{
    scripting_add_internal_call( "UnityEngine.Collider::set_isTrigger" , (gpointer)& Collider_Set_Custom_PropIsTrigger );
}

void Register_UnityEngine_Collider_get_material()
{
    scripting_add_internal_call( "UnityEngine.Collider::get_material" , (gpointer)& Collider_Get_Custom_PropMaterial );
}

void Register_UnityEngine_Collider_set_material()
{
    scripting_add_internal_call( "UnityEngine.Collider::set_material" , (gpointer)& Collider_Set_Custom_PropMaterial );
}

void Register_UnityEngine_Collider_INTERNAL_CALL_ClosestPointOnBounds()
{
    scripting_add_internal_call( "UnityEngine.Collider::INTERNAL_CALL_ClosestPointOnBounds" , (gpointer)& Collider_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds );
}

void Register_UnityEngine_Collider_get_sharedMaterial()
{
    scripting_add_internal_call( "UnityEngine.Collider::get_sharedMaterial" , (gpointer)& Collider_Get_Custom_PropSharedMaterial );
}

void Register_UnityEngine_Collider_set_sharedMaterial()
{
    scripting_add_internal_call( "UnityEngine.Collider::set_sharedMaterial" , (gpointer)& Collider_Set_Custom_PropSharedMaterial );
}

void Register_UnityEngine_Collider_INTERNAL_get_bounds()
{
    scripting_add_internal_call( "UnityEngine.Collider::INTERNAL_get_bounds" , (gpointer)& Collider_CUSTOM_INTERNAL_get_bounds );
}

void Register_UnityEngine_Collider_INTERNAL_CALL_Internal_Raycast()
{
    scripting_add_internal_call( "UnityEngine.Collider::INTERNAL_CALL_Internal_Raycast" , (gpointer)& Collider_CUSTOM_INTERNAL_CALL_Internal_Raycast );
}

void Register_UnityEngine_BoxCollider_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider::INTERNAL_get_center" , (gpointer)& BoxCollider_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_BoxCollider_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider::INTERNAL_set_center" , (gpointer)& BoxCollider_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_BoxCollider_INTERNAL_get_size()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider::INTERNAL_get_size" , (gpointer)& BoxCollider_CUSTOM_INTERNAL_get_size );
}

void Register_UnityEngine_BoxCollider_INTERNAL_set_size()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider::INTERNAL_set_size" , (gpointer)& BoxCollider_CUSTOM_INTERNAL_set_size );
}

void Register_UnityEngine_SphereCollider_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.SphereCollider::INTERNAL_get_center" , (gpointer)& SphereCollider_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_SphereCollider_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.SphereCollider::INTERNAL_set_center" , (gpointer)& SphereCollider_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_SphereCollider_get_radius()
{
    scripting_add_internal_call( "UnityEngine.SphereCollider::get_radius" , (gpointer)& SphereCollider_Get_Custom_PropRadius );
}

void Register_UnityEngine_SphereCollider_set_radius()
{
    scripting_add_internal_call( "UnityEngine.SphereCollider::set_radius" , (gpointer)& SphereCollider_Set_Custom_PropRadius );
}

void Register_UnityEngine_MeshCollider_get_sharedMesh()
{
    scripting_add_internal_call( "UnityEngine.MeshCollider::get_sharedMesh" , (gpointer)& MeshCollider_Get_Custom_PropSharedMesh );
}

void Register_UnityEngine_MeshCollider_set_sharedMesh()
{
    scripting_add_internal_call( "UnityEngine.MeshCollider::set_sharedMesh" , (gpointer)& MeshCollider_Set_Custom_PropSharedMesh );
}

void Register_UnityEngine_MeshCollider_get_convex()
{
    scripting_add_internal_call( "UnityEngine.MeshCollider::get_convex" , (gpointer)& MeshCollider_Get_Custom_PropConvex );
}

void Register_UnityEngine_MeshCollider_set_convex()
{
    scripting_add_internal_call( "UnityEngine.MeshCollider::set_convex" , (gpointer)& MeshCollider_Set_Custom_PropConvex );
}

void Register_UnityEngine_MeshCollider_get_smoothSphereCollisions()
{
    scripting_add_internal_call( "UnityEngine.MeshCollider::get_smoothSphereCollisions" , (gpointer)& MeshCollider_Get_Custom_PropSmoothSphereCollisions );
}

void Register_UnityEngine_MeshCollider_set_smoothSphereCollisions()
{
    scripting_add_internal_call( "UnityEngine.MeshCollider::set_smoothSphereCollisions" , (gpointer)& MeshCollider_Set_Custom_PropSmoothSphereCollisions );
}

void Register_UnityEngine_CapsuleCollider_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::INTERNAL_get_center" , (gpointer)& CapsuleCollider_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_CapsuleCollider_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::INTERNAL_set_center" , (gpointer)& CapsuleCollider_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_CapsuleCollider_get_radius()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::get_radius" , (gpointer)& CapsuleCollider_Get_Custom_PropRadius );
}

void Register_UnityEngine_CapsuleCollider_set_radius()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::set_radius" , (gpointer)& CapsuleCollider_Set_Custom_PropRadius );
}

void Register_UnityEngine_CapsuleCollider_get_height()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::get_height" , (gpointer)& CapsuleCollider_Get_Custom_PropHeight );
}

void Register_UnityEngine_CapsuleCollider_set_height()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::set_height" , (gpointer)& CapsuleCollider_Set_Custom_PropHeight );
}

void Register_UnityEngine_CapsuleCollider_get_direction()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::get_direction" , (gpointer)& CapsuleCollider_Get_Custom_PropDirection );
}

void Register_UnityEngine_CapsuleCollider_set_direction()
{
    scripting_add_internal_call( "UnityEngine.CapsuleCollider::set_direction" , (gpointer)& CapsuleCollider_Set_Custom_PropDirection );
}

void Register_UnityEngine_RaycastCollider_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.RaycastCollider::INTERNAL_get_center" , (gpointer)& RaycastCollider_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_RaycastCollider_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.RaycastCollider::INTERNAL_set_center" , (gpointer)& RaycastCollider_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_RaycastCollider_get_length()
{
    scripting_add_internal_call( "UnityEngine.RaycastCollider::get_length" , (gpointer)& RaycastCollider_Get_Custom_PropLength );
}

void Register_UnityEngine_RaycastCollider_set_length()
{
    scripting_add_internal_call( "UnityEngine.RaycastCollider::set_length" , (gpointer)& RaycastCollider_Set_Custom_PropLength );
}

void Register_UnityEngine_WheelCollider_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_get_center" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_WheelCollider_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_set_center" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_WheelCollider_get_radius()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_radius" , (gpointer)& WheelCollider_Get_Custom_PropRadius );
}

void Register_UnityEngine_WheelCollider_set_radius()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::set_radius" , (gpointer)& WheelCollider_Set_Custom_PropRadius );
}

void Register_UnityEngine_WheelCollider_get_suspensionDistance()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_suspensionDistance" , (gpointer)& WheelCollider_Get_Custom_PropSuspensionDistance );
}

void Register_UnityEngine_WheelCollider_set_suspensionDistance()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::set_suspensionDistance" , (gpointer)& WheelCollider_Set_Custom_PropSuspensionDistance );
}

void Register_UnityEngine_WheelCollider_INTERNAL_get_suspensionSpring()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_get_suspensionSpring" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_get_suspensionSpring );
}

void Register_UnityEngine_WheelCollider_INTERNAL_set_suspensionSpring()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_set_suspensionSpring" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_set_suspensionSpring );
}

void Register_UnityEngine_WheelCollider_get_mass()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_mass" , (gpointer)& WheelCollider_Get_Custom_PropMass );
}

void Register_UnityEngine_WheelCollider_set_mass()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::set_mass" , (gpointer)& WheelCollider_Set_Custom_PropMass );
}

void Register_UnityEngine_WheelCollider_INTERNAL_get_forwardFriction()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_get_forwardFriction" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_get_forwardFriction );
}

void Register_UnityEngine_WheelCollider_INTERNAL_set_forwardFriction()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_set_forwardFriction" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_set_forwardFriction );
}

void Register_UnityEngine_WheelCollider_INTERNAL_get_sidewaysFriction()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_get_sidewaysFriction" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_get_sidewaysFriction );
}

void Register_UnityEngine_WheelCollider_INTERNAL_set_sidewaysFriction()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::INTERNAL_set_sidewaysFriction" , (gpointer)& WheelCollider_CUSTOM_INTERNAL_set_sidewaysFriction );
}

void Register_UnityEngine_WheelCollider_get_motorTorque()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_motorTorque" , (gpointer)& WheelCollider_Get_Custom_PropMotorTorque );
}

void Register_UnityEngine_WheelCollider_set_motorTorque()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::set_motorTorque" , (gpointer)& WheelCollider_Set_Custom_PropMotorTorque );
}

void Register_UnityEngine_WheelCollider_get_brakeTorque()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_brakeTorque" , (gpointer)& WheelCollider_Get_Custom_PropBrakeTorque );
}

void Register_UnityEngine_WheelCollider_set_brakeTorque()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::set_brakeTorque" , (gpointer)& WheelCollider_Set_Custom_PropBrakeTorque );
}

void Register_UnityEngine_WheelCollider_get_steerAngle()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_steerAngle" , (gpointer)& WheelCollider_Get_Custom_PropSteerAngle );
}

void Register_UnityEngine_WheelCollider_set_steerAngle()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::set_steerAngle" , (gpointer)& WheelCollider_Set_Custom_PropSteerAngle );
}

void Register_UnityEngine_WheelCollider_get_isGrounded()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_isGrounded" , (gpointer)& WheelCollider_Get_Custom_PropIsGrounded );
}

void Register_UnityEngine_WheelCollider_GetGroundHit()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::GetGroundHit" , (gpointer)& WheelCollider_CUSTOM_GetGroundHit );
}

void Register_UnityEngine_WheelCollider_get_rpm()
{
    scripting_add_internal_call( "UnityEngine.WheelCollider::get_rpm" , (gpointer)& WheelCollider_Get_Custom_PropRpm );
}

void Register_UnityEngine_RaycastHit_INTERNAL_CALL_CalculateRaycastTexCoord()
{
    scripting_add_internal_call( "UnityEngine.RaycastHit::INTERNAL_CALL_CalculateRaycastTexCoord" , (gpointer)& RaycastHit_CUSTOM_INTERNAL_CALL_CalculateRaycastTexCoord );
}

void Register_UnityEngine_PhysicMaterial_Internal_CreateDynamicsMaterial()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::Internal_CreateDynamicsMaterial" , (gpointer)& PhysicMaterial_CUSTOM_Internal_CreateDynamicsMaterial );
}

void Register_UnityEngine_PhysicMaterial_get_dynamicFriction()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_dynamicFriction" , (gpointer)& PhysicMaterial_Get_Custom_PropDynamicFriction );
}

void Register_UnityEngine_PhysicMaterial_set_dynamicFriction()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_dynamicFriction" , (gpointer)& PhysicMaterial_Set_Custom_PropDynamicFriction );
}

void Register_UnityEngine_PhysicMaterial_get_staticFriction()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_staticFriction" , (gpointer)& PhysicMaterial_Get_Custom_PropStaticFriction );
}

void Register_UnityEngine_PhysicMaterial_set_staticFriction()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_staticFriction" , (gpointer)& PhysicMaterial_Set_Custom_PropStaticFriction );
}

void Register_UnityEngine_PhysicMaterial_get_bounciness()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_bounciness" , (gpointer)& PhysicMaterial_Get_Custom_PropBounciness );
}

void Register_UnityEngine_PhysicMaterial_set_bounciness()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_bounciness" , (gpointer)& PhysicMaterial_Set_Custom_PropBounciness );
}

void Register_UnityEngine_PhysicMaterial_INTERNAL_get_frictionDirection2()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::INTERNAL_get_frictionDirection2" , (gpointer)& PhysicMaterial_CUSTOM_INTERNAL_get_frictionDirection2 );
}

void Register_UnityEngine_PhysicMaterial_INTERNAL_set_frictionDirection2()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::INTERNAL_set_frictionDirection2" , (gpointer)& PhysicMaterial_CUSTOM_INTERNAL_set_frictionDirection2 );
}

void Register_UnityEngine_PhysicMaterial_get_dynamicFriction2()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_dynamicFriction2" , (gpointer)& PhysicMaterial_Get_Custom_PropDynamicFriction2 );
}

void Register_UnityEngine_PhysicMaterial_set_dynamicFriction2()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_dynamicFriction2" , (gpointer)& PhysicMaterial_Set_Custom_PropDynamicFriction2 );
}

void Register_UnityEngine_PhysicMaterial_get_staticFriction2()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_staticFriction2" , (gpointer)& PhysicMaterial_Get_Custom_PropStaticFriction2 );
}

void Register_UnityEngine_PhysicMaterial_set_staticFriction2()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_staticFriction2" , (gpointer)& PhysicMaterial_Set_Custom_PropStaticFriction2 );
}

void Register_UnityEngine_PhysicMaterial_get_frictionCombine()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_frictionCombine" , (gpointer)& PhysicMaterial_Get_Custom_PropFrictionCombine );
}

void Register_UnityEngine_PhysicMaterial_set_frictionCombine()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_frictionCombine" , (gpointer)& PhysicMaterial_Set_Custom_PropFrictionCombine );
}

void Register_UnityEngine_PhysicMaterial_get_bounceCombine()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::get_bounceCombine" , (gpointer)& PhysicMaterial_Get_Custom_PropBounceCombine );
}

void Register_UnityEngine_PhysicMaterial_set_bounceCombine()
{
    scripting_add_internal_call( "UnityEngine.PhysicMaterial::set_bounceCombine" , (gpointer)& PhysicMaterial_Set_Custom_PropBounceCombine );
}

void Register_UnityEngine_CharacterController_INTERNAL_CALL_SimpleMove()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::INTERNAL_CALL_SimpleMove" , (gpointer)& CharacterController_CUSTOM_INTERNAL_CALL_SimpleMove );
}

void Register_UnityEngine_CharacterController_INTERNAL_CALL_Move()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::INTERNAL_CALL_Move" , (gpointer)& CharacterController_CUSTOM_INTERNAL_CALL_Move );
}

void Register_UnityEngine_CharacterController_get_isGrounded()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_isGrounded" , (gpointer)& CharacterController_Get_Custom_PropIsGrounded );
}

void Register_UnityEngine_CharacterController_INTERNAL_get_velocity()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::INTERNAL_get_velocity" , (gpointer)& CharacterController_CUSTOM_INTERNAL_get_velocity );
}

void Register_UnityEngine_CharacterController_get_collisionFlags()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_collisionFlags" , (gpointer)& CharacterController_Get_Custom_PropCollisionFlags );
}

void Register_UnityEngine_CharacterController_get_radius()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_radius" , (gpointer)& CharacterController_Get_Custom_PropRadius );
}

void Register_UnityEngine_CharacterController_set_radius()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::set_radius" , (gpointer)& CharacterController_Set_Custom_PropRadius );
}

void Register_UnityEngine_CharacterController_get_height()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_height" , (gpointer)& CharacterController_Get_Custom_PropHeight );
}

void Register_UnityEngine_CharacterController_set_height()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::set_height" , (gpointer)& CharacterController_Set_Custom_PropHeight );
}

void Register_UnityEngine_CharacterController_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::INTERNAL_get_center" , (gpointer)& CharacterController_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_CharacterController_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::INTERNAL_set_center" , (gpointer)& CharacterController_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_CharacterController_get_slopeLimit()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_slopeLimit" , (gpointer)& CharacterController_Get_Custom_PropSlopeLimit );
}

void Register_UnityEngine_CharacterController_set_slopeLimit()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::set_slopeLimit" , (gpointer)& CharacterController_Set_Custom_PropSlopeLimit );
}

void Register_UnityEngine_CharacterController_get_stepOffset()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_stepOffset" , (gpointer)& CharacterController_Get_Custom_PropStepOffset );
}

void Register_UnityEngine_CharacterController_set_stepOffset()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::set_stepOffset" , (gpointer)& CharacterController_Set_Custom_PropStepOffset );
}

void Register_UnityEngine_CharacterController_get_detectCollisions()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::get_detectCollisions" , (gpointer)& CharacterController_Get_Custom_PropDetectCollisions );
}

void Register_UnityEngine_CharacterController_set_detectCollisions()
{
    scripting_add_internal_call( "UnityEngine.CharacterController::set_detectCollisions" , (gpointer)& CharacterController_Set_Custom_PropDetectCollisions );
}

#endif
#if ENABLE_CLOTH
void Register_UnityEngine_Cloth_get_bendingStiffness()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_bendingStiffness" , (gpointer)& Cloth_Get_Custom_PropBendingStiffness );
}

void Register_UnityEngine_Cloth_set_bendingStiffness()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_bendingStiffness" , (gpointer)& Cloth_Set_Custom_PropBendingStiffness );
}

void Register_UnityEngine_Cloth_get_stretchingStiffness()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_stretchingStiffness" , (gpointer)& Cloth_Get_Custom_PropStretchingStiffness );
}

void Register_UnityEngine_Cloth_set_stretchingStiffness()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_stretchingStiffness" , (gpointer)& Cloth_Set_Custom_PropStretchingStiffness );
}

void Register_UnityEngine_Cloth_get_damping()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_damping" , (gpointer)& Cloth_Get_Custom_PropDamping );
}

void Register_UnityEngine_Cloth_set_damping()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_damping" , (gpointer)& Cloth_Set_Custom_PropDamping );
}

void Register_UnityEngine_Cloth_get_thickness()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_thickness" , (gpointer)& Cloth_Get_Custom_PropThickness );
}

void Register_UnityEngine_Cloth_set_thickness()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_thickness" , (gpointer)& Cloth_Set_Custom_PropThickness );
}

void Register_UnityEngine_Cloth_INTERNAL_get_externalAcceleration()
{
    scripting_add_internal_call( "UnityEngine.Cloth::INTERNAL_get_externalAcceleration" , (gpointer)& Cloth_CUSTOM_INTERNAL_get_externalAcceleration );
}

void Register_UnityEngine_Cloth_INTERNAL_set_externalAcceleration()
{
    scripting_add_internal_call( "UnityEngine.Cloth::INTERNAL_set_externalAcceleration" , (gpointer)& Cloth_CUSTOM_INTERNAL_set_externalAcceleration );
}

void Register_UnityEngine_Cloth_INTERNAL_get_randomAcceleration()
{
    scripting_add_internal_call( "UnityEngine.Cloth::INTERNAL_get_randomAcceleration" , (gpointer)& Cloth_CUSTOM_INTERNAL_get_randomAcceleration );
}

void Register_UnityEngine_Cloth_INTERNAL_set_randomAcceleration()
{
    scripting_add_internal_call( "UnityEngine.Cloth::INTERNAL_set_randomAcceleration" , (gpointer)& Cloth_CUSTOM_INTERNAL_set_randomAcceleration );
}

void Register_UnityEngine_Cloth_get_useGravity()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_useGravity" , (gpointer)& Cloth_Get_Custom_PropUseGravity );
}

void Register_UnityEngine_Cloth_set_useGravity()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_useGravity" , (gpointer)& Cloth_Set_Custom_PropUseGravity );
}

void Register_UnityEngine_Cloth_get_selfCollision()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_selfCollision" , (gpointer)& Cloth_Get_Custom_PropSelfCollision );
}

void Register_UnityEngine_Cloth_set_selfCollision()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_selfCollision" , (gpointer)& Cloth_Set_Custom_PropSelfCollision );
}

void Register_UnityEngine_Cloth_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_enabled" , (gpointer)& Cloth_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Cloth_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Cloth::set_enabled" , (gpointer)& Cloth_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Cloth_get_vertices()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_vertices" , (gpointer)& Cloth_Get_Custom_PropVertices );
}

void Register_UnityEngine_Cloth_get_normals()
{
    scripting_add_internal_call( "UnityEngine.Cloth::get_normals" , (gpointer)& Cloth_Get_Custom_PropNormals );
}

void Register_UnityEngine_InteractiveCloth_get_mesh()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_mesh" , (gpointer)& InteractiveCloth_Get_Custom_PropMesh );
}

void Register_UnityEngine_InteractiveCloth_set_mesh()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_mesh" , (gpointer)& InteractiveCloth_Set_Custom_PropMesh );
}

void Register_UnityEngine_InteractiveCloth_get_friction()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_friction" , (gpointer)& InteractiveCloth_Get_Custom_PropFriction );
}

void Register_UnityEngine_InteractiveCloth_set_friction()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_friction" , (gpointer)& InteractiveCloth_Set_Custom_PropFriction );
}

void Register_UnityEngine_InteractiveCloth_get_density()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_density" , (gpointer)& InteractiveCloth_Get_Custom_PropDensity );
}

void Register_UnityEngine_InteractiveCloth_set_density()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_density" , (gpointer)& InteractiveCloth_Set_Custom_PropDensity );
}

void Register_UnityEngine_InteractiveCloth_get_pressure()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_pressure" , (gpointer)& InteractiveCloth_Get_Custom_PropPressure );
}

void Register_UnityEngine_InteractiveCloth_set_pressure()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_pressure" , (gpointer)& InteractiveCloth_Set_Custom_PropPressure );
}

void Register_UnityEngine_InteractiveCloth_get_collisionResponse()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_collisionResponse" , (gpointer)& InteractiveCloth_Get_Custom_PropCollisionResponse );
}

void Register_UnityEngine_InteractiveCloth_set_collisionResponse()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_collisionResponse" , (gpointer)& InteractiveCloth_Set_Custom_PropCollisionResponse );
}

void Register_UnityEngine_InteractiveCloth_get_tearFactor()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_tearFactor" , (gpointer)& InteractiveCloth_Get_Custom_PropTearFactor );
}

void Register_UnityEngine_InteractiveCloth_set_tearFactor()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_tearFactor" , (gpointer)& InteractiveCloth_Set_Custom_PropTearFactor );
}

void Register_UnityEngine_InteractiveCloth_get_attachmentTearFactor()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_attachmentTearFactor" , (gpointer)& InteractiveCloth_Get_Custom_PropAttachmentTearFactor );
}

void Register_UnityEngine_InteractiveCloth_set_attachmentTearFactor()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_attachmentTearFactor" , (gpointer)& InteractiveCloth_Set_Custom_PropAttachmentTearFactor );
}

void Register_UnityEngine_InteractiveCloth_get_attachmentResponse()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_attachmentResponse" , (gpointer)& InteractiveCloth_Get_Custom_PropAttachmentResponse );
}

void Register_UnityEngine_InteractiveCloth_set_attachmentResponse()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::set_attachmentResponse" , (gpointer)& InteractiveCloth_Set_Custom_PropAttachmentResponse );
}

void Register_UnityEngine_InteractiveCloth_get_isTeared()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::get_isTeared" , (gpointer)& InteractiveCloth_Get_Custom_PropIsTeared );
}

void Register_UnityEngine_InteractiveCloth_INTERNAL_CALL_AddForceAtPosition()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::INTERNAL_CALL_AddForceAtPosition" , (gpointer)& InteractiveCloth_CUSTOM_INTERNAL_CALL_AddForceAtPosition );
}

void Register_UnityEngine_InteractiveCloth_AttachToCollider()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::AttachToCollider" , (gpointer)& InteractiveCloth_CUSTOM_AttachToCollider );
}

void Register_UnityEngine_InteractiveCloth_DetachFromCollider()
{
    scripting_add_internal_call( "UnityEngine.InteractiveCloth::DetachFromCollider" , (gpointer)& InteractiveCloth_CUSTOM_DetachFromCollider );
}

void Register_UnityEngine_SkinnedCloth_get_coefficients()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::get_coefficients" , (gpointer)& SkinnedCloth_Get_Custom_PropCoefficients );
}

void Register_UnityEngine_SkinnedCloth_set_coefficients()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::set_coefficients" , (gpointer)& SkinnedCloth_Set_Custom_PropCoefficients );
}

void Register_UnityEngine_SkinnedCloth_get_worldVelocityScale()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::get_worldVelocityScale" , (gpointer)& SkinnedCloth_Get_Custom_PropWorldVelocityScale );
}

void Register_UnityEngine_SkinnedCloth_set_worldVelocityScale()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::set_worldVelocityScale" , (gpointer)& SkinnedCloth_Set_Custom_PropWorldVelocityScale );
}

void Register_UnityEngine_SkinnedCloth_get_worldAccelerationScale()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::get_worldAccelerationScale" , (gpointer)& SkinnedCloth_Get_Custom_PropWorldAccelerationScale );
}

void Register_UnityEngine_SkinnedCloth_set_worldAccelerationScale()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::set_worldAccelerationScale" , (gpointer)& SkinnedCloth_Set_Custom_PropWorldAccelerationScale );
}

void Register_UnityEngine_SkinnedCloth_SetEnabledFading()
{
    scripting_add_internal_call( "UnityEngine.SkinnedCloth::SetEnabledFading" , (gpointer)& SkinnedCloth_CUSTOM_SetEnabledFading );
}

void Register_UnityEngine_ClothRenderer_get_pauseWhenNotVisible()
{
    scripting_add_internal_call( "UnityEngine.ClothRenderer::get_pauseWhenNotVisible" , (gpointer)& ClothRenderer_Get_Custom_PropPauseWhenNotVisible );
}

void Register_UnityEngine_ClothRenderer_set_pauseWhenNotVisible()
{
    scripting_add_internal_call( "UnityEngine.ClothRenderer::set_pauseWhenNotVisible" , (gpointer)& ClothRenderer_Set_Custom_PropPauseWhenNotVisible );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_NewDynamics_IcallNames [] =
{
#if ENABLE_PHYSICS
    "UnityEngine.Physics::INTERNAL_get_gravity",    // -> Physics_CUSTOM_INTERNAL_get_gravity
    "UnityEngine.Physics::INTERNAL_set_gravity",    // -> Physics_CUSTOM_INTERNAL_set_gravity
    "UnityEngine.Physics::get_minPenetrationForPenalty",    // -> Physics_Get_Custom_PropMinPenetrationForPenalty
    "UnityEngine.Physics::set_minPenetrationForPenalty",    // -> Physics_Set_Custom_PropMinPenetrationForPenalty
    "UnityEngine.Physics::get_bounceThreshold",    // -> Physics_Get_Custom_PropBounceThreshold
    "UnityEngine.Physics::set_bounceThreshold",    // -> Physics_Set_Custom_PropBounceThreshold
    "UnityEngine.Physics::get_sleepVelocity",    // -> Physics_Get_Custom_PropSleepVelocity
    "UnityEngine.Physics::set_sleepVelocity",    // -> Physics_Set_Custom_PropSleepVelocity
    "UnityEngine.Physics::get_sleepAngularVelocity",    // -> Physics_Get_Custom_PropSleepAngularVelocity
    "UnityEngine.Physics::set_sleepAngularVelocity",    // -> Physics_Set_Custom_PropSleepAngularVelocity
    "UnityEngine.Physics::get_maxAngularVelocity",    // -> Physics_Get_Custom_PropMaxAngularVelocity
    "UnityEngine.Physics::set_maxAngularVelocity",    // -> Physics_Set_Custom_PropMaxAngularVelocity
    "UnityEngine.Physics::get_solverIterationCount",    // -> Physics_Get_Custom_PropSolverIterationCount
    "UnityEngine.Physics::set_solverIterationCount",    // -> Physics_Set_Custom_PropSolverIterationCount
    "UnityEngine.Physics::INTERNAL_CALL_Internal_Raycast",    // -> Physics_CUSTOM_INTERNAL_CALL_Internal_Raycast
    "UnityEngine.Physics::INTERNAL_CALL_Internal_CapsuleCast",    // -> Physics_CUSTOM_INTERNAL_CALL_Internal_CapsuleCast
    "UnityEngine.Physics::INTERNAL_CALL_Internal_RaycastTest",    // -> Physics_CUSTOM_INTERNAL_CALL_Internal_RaycastTest
    "UnityEngine.Physics::INTERNAL_CALL_RaycastAll",    // -> Physics_CUSTOM_INTERNAL_CALL_RaycastAll
    "UnityEngine.Physics::INTERNAL_CALL_OverlapSphere",    // -> Physics_CUSTOM_INTERNAL_CALL_OverlapSphere
    "UnityEngine.Physics::INTERNAL_CALL_CapsuleCastAll",    // -> Physics_CUSTOM_INTERNAL_CALL_CapsuleCastAll
    "UnityEngine.Physics::INTERNAL_CALL_CheckSphere",    // -> Physics_CUSTOM_INTERNAL_CALL_CheckSphere
    "UnityEngine.Physics::INTERNAL_CALL_CheckCapsule",    // -> Physics_CUSTOM_INTERNAL_CALL_CheckCapsule
    "UnityEngine.Physics::get_penetrationPenaltyForce",    // -> Physics_Get_Custom_PropPenetrationPenaltyForce
    "UnityEngine.Physics::set_penetrationPenaltyForce",    // -> Physics_Set_Custom_PropPenetrationPenaltyForce
    "UnityEngine.Physics::IgnoreCollision"  ,    // -> Physics_CUSTOM_IgnoreCollision
    "UnityEngine.Physics::IgnoreLayerCollision",    // -> Physics_CUSTOM_IgnoreLayerCollision
    "UnityEngine.Physics::GetIgnoreLayerCollision",    // -> Physics_CUSTOM_GetIgnoreLayerCollision
    "UnityEngine.Rigidbody::INTERNAL_get_velocity",    // -> Rigidbody_CUSTOM_INTERNAL_get_velocity
    "UnityEngine.Rigidbody::INTERNAL_set_velocity",    // -> Rigidbody_CUSTOM_INTERNAL_set_velocity
    "UnityEngine.Rigidbody::INTERNAL_get_angularVelocity",    // -> Rigidbody_CUSTOM_INTERNAL_get_angularVelocity
    "UnityEngine.Rigidbody::INTERNAL_set_angularVelocity",    // -> Rigidbody_CUSTOM_INTERNAL_set_angularVelocity
    "UnityEngine.Rigidbody::get_drag"       ,    // -> Rigidbody_Get_Custom_PropDrag
    "UnityEngine.Rigidbody::set_drag"       ,    // -> Rigidbody_Set_Custom_PropDrag
    "UnityEngine.Rigidbody::get_angularDrag",    // -> Rigidbody_Get_Custom_PropAngularDrag
    "UnityEngine.Rigidbody::set_angularDrag",    // -> Rigidbody_Set_Custom_PropAngularDrag
    "UnityEngine.Rigidbody::get_mass"       ,    // -> Rigidbody_Get_Custom_PropMass
    "UnityEngine.Rigidbody::set_mass"       ,    // -> Rigidbody_Set_Custom_PropMass
    "UnityEngine.Rigidbody::INTERNAL_CALL_SetDensity",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_SetDensity
    "UnityEngine.Rigidbody::get_useGravity" ,    // -> Rigidbody_Get_Custom_PropUseGravity
    "UnityEngine.Rigidbody::set_useGravity" ,    // -> Rigidbody_Set_Custom_PropUseGravity
    "UnityEngine.Rigidbody::get_isKinematic",    // -> Rigidbody_Get_Custom_PropIsKinematic
    "UnityEngine.Rigidbody::set_isKinematic",    // -> Rigidbody_Set_Custom_PropIsKinematic
    "UnityEngine.Rigidbody::get_freezeRotation",    // -> Rigidbody_Get_Custom_PropFreezeRotation
    "UnityEngine.Rigidbody::set_freezeRotation",    // -> Rigidbody_Set_Custom_PropFreezeRotation
    "UnityEngine.Rigidbody::get_constraints",    // -> Rigidbody_Get_Custom_PropConstraints
    "UnityEngine.Rigidbody::set_constraints",    // -> Rigidbody_Set_Custom_PropConstraints
    "UnityEngine.Rigidbody::get_collisionDetectionMode",    // -> Rigidbody_Get_Custom_PropCollisionDetectionMode
    "UnityEngine.Rigidbody::set_collisionDetectionMode",    // -> Rigidbody_Set_Custom_PropCollisionDetectionMode
    "UnityEngine.Rigidbody::INTERNAL_CALL_AddForce",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_AddForce
    "UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeForce",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeForce
    "UnityEngine.Rigidbody::INTERNAL_CALL_AddTorque",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_AddTorque
    "UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeTorque",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeTorque
    "UnityEngine.Rigidbody::INTERNAL_CALL_AddForceAtPosition",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_AddForceAtPosition
    "UnityEngine.Rigidbody::INTERNAL_CALL_AddExplosionForce",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_AddExplosionForce
    "UnityEngine.Rigidbody::INTERNAL_CALL_ClosestPointOnBounds",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds
    "UnityEngine.Rigidbody::INTERNAL_CALL_GetRelativePointVelocity",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_GetRelativePointVelocity
    "UnityEngine.Rigidbody::INTERNAL_CALL_GetPointVelocity",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_GetPointVelocity
    "UnityEngine.Rigidbody::INTERNAL_get_centerOfMass",    // -> Rigidbody_CUSTOM_INTERNAL_get_centerOfMass
    "UnityEngine.Rigidbody::INTERNAL_set_centerOfMass",    // -> Rigidbody_CUSTOM_INTERNAL_set_centerOfMass
    "UnityEngine.Rigidbody::INTERNAL_get_worldCenterOfMass",    // -> Rigidbody_CUSTOM_INTERNAL_get_worldCenterOfMass
    "UnityEngine.Rigidbody::INTERNAL_get_inertiaTensorRotation",    // -> Rigidbody_CUSTOM_INTERNAL_get_inertiaTensorRotation
    "UnityEngine.Rigidbody::INTERNAL_set_inertiaTensorRotation",    // -> Rigidbody_CUSTOM_INTERNAL_set_inertiaTensorRotation
    "UnityEngine.Rigidbody::INTERNAL_get_inertiaTensor",    // -> Rigidbody_CUSTOM_INTERNAL_get_inertiaTensor
    "UnityEngine.Rigidbody::INTERNAL_set_inertiaTensor",    // -> Rigidbody_CUSTOM_INTERNAL_set_inertiaTensor
    "UnityEngine.Rigidbody::get_detectCollisions",    // -> Rigidbody_Get_Custom_PropDetectCollisions
    "UnityEngine.Rigidbody::set_detectCollisions",    // -> Rigidbody_Set_Custom_PropDetectCollisions
    "UnityEngine.Rigidbody::get_useConeFriction",    // -> Rigidbody_Get_Custom_PropUseConeFriction
    "UnityEngine.Rigidbody::set_useConeFriction",    // -> Rigidbody_Set_Custom_PropUseConeFriction
    "UnityEngine.Rigidbody::INTERNAL_get_position",    // -> Rigidbody_CUSTOM_INTERNAL_get_position
    "UnityEngine.Rigidbody::INTERNAL_set_position",    // -> Rigidbody_CUSTOM_INTERNAL_set_position
    "UnityEngine.Rigidbody::INTERNAL_get_rotation",    // -> Rigidbody_CUSTOM_INTERNAL_get_rotation
    "UnityEngine.Rigidbody::INTERNAL_set_rotation",    // -> Rigidbody_CUSTOM_INTERNAL_set_rotation
    "UnityEngine.Rigidbody::INTERNAL_CALL_MovePosition",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_MovePosition
    "UnityEngine.Rigidbody::INTERNAL_CALL_MoveRotation",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_MoveRotation
    "UnityEngine.Rigidbody::get_interpolation",    // -> Rigidbody_Get_Custom_PropInterpolation
    "UnityEngine.Rigidbody::set_interpolation",    // -> Rigidbody_Set_Custom_PropInterpolation
    "UnityEngine.Rigidbody::INTERNAL_CALL_Sleep",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_Sleep
    "UnityEngine.Rigidbody::INTERNAL_CALL_IsSleeping",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_IsSleeping
    "UnityEngine.Rigidbody::INTERNAL_CALL_WakeUp",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_WakeUp
    "UnityEngine.Rigidbody::get_solverIterationCount",    // -> Rigidbody_Get_Custom_PropSolverIterationCount
    "UnityEngine.Rigidbody::set_solverIterationCount",    // -> Rigidbody_Set_Custom_PropSolverIterationCount
    "UnityEngine.Rigidbody::get_sleepVelocity",    // -> Rigidbody_Get_Custom_PropSleepVelocity
    "UnityEngine.Rigidbody::set_sleepVelocity",    // -> Rigidbody_Set_Custom_PropSleepVelocity
    "UnityEngine.Rigidbody::get_sleepAngularVelocity",    // -> Rigidbody_Get_Custom_PropSleepAngularVelocity
    "UnityEngine.Rigidbody::set_sleepAngularVelocity",    // -> Rigidbody_Set_Custom_PropSleepAngularVelocity
    "UnityEngine.Rigidbody::get_maxAngularVelocity",    // -> Rigidbody_Get_Custom_PropMaxAngularVelocity
    "UnityEngine.Rigidbody::set_maxAngularVelocity",    // -> Rigidbody_Set_Custom_PropMaxAngularVelocity
    "UnityEngine.Rigidbody::INTERNAL_CALL_SweepTest",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_SweepTest
    "UnityEngine.Rigidbody::INTERNAL_CALL_SweepTestAll",    // -> Rigidbody_CUSTOM_INTERNAL_CALL_SweepTestAll
    "UnityEngine.Joint::get_connectedBody"  ,    // -> Joint_Get_Custom_PropConnectedBody
    "UnityEngine.Joint::set_connectedBody"  ,    // -> Joint_Set_Custom_PropConnectedBody
    "UnityEngine.Joint::INTERNAL_get_axis"  ,    // -> Joint_CUSTOM_INTERNAL_get_axis
    "UnityEngine.Joint::INTERNAL_set_axis"  ,    // -> Joint_CUSTOM_INTERNAL_set_axis
    "UnityEngine.Joint::INTERNAL_get_anchor",    // -> Joint_CUSTOM_INTERNAL_get_anchor
    "UnityEngine.Joint::INTERNAL_set_anchor",    // -> Joint_CUSTOM_INTERNAL_set_anchor
    "UnityEngine.Joint::get_breakForce"     ,    // -> Joint_Get_Custom_PropBreakForce
    "UnityEngine.Joint::set_breakForce"     ,    // -> Joint_Set_Custom_PropBreakForce
    "UnityEngine.Joint::get_breakTorque"    ,    // -> Joint_Get_Custom_PropBreakTorque
    "UnityEngine.Joint::set_breakTorque"    ,    // -> Joint_Set_Custom_PropBreakTorque
    "UnityEngine.HingeJoint::INTERNAL_get_motor",    // -> HingeJoint_CUSTOM_INTERNAL_get_motor
    "UnityEngine.HingeJoint::INTERNAL_set_motor",    // -> HingeJoint_CUSTOM_INTERNAL_set_motor
    "UnityEngine.HingeJoint::INTERNAL_get_limits",    // -> HingeJoint_CUSTOM_INTERNAL_get_limits
    "UnityEngine.HingeJoint::INTERNAL_set_limits",    // -> HingeJoint_CUSTOM_INTERNAL_set_limits
    "UnityEngine.HingeJoint::INTERNAL_get_spring",    // -> HingeJoint_CUSTOM_INTERNAL_get_spring
    "UnityEngine.HingeJoint::INTERNAL_set_spring",    // -> HingeJoint_CUSTOM_INTERNAL_set_spring
    "UnityEngine.HingeJoint::get_useMotor"  ,    // -> HingeJoint_Get_Custom_PropUseMotor
    "UnityEngine.HingeJoint::set_useMotor"  ,    // -> HingeJoint_Set_Custom_PropUseMotor
    "UnityEngine.HingeJoint::get_useLimits" ,    // -> HingeJoint_Get_Custom_PropUseLimits
    "UnityEngine.HingeJoint::set_useLimits" ,    // -> HingeJoint_Set_Custom_PropUseLimits
    "UnityEngine.HingeJoint::get_useSpring" ,    // -> HingeJoint_Get_Custom_PropUseSpring
    "UnityEngine.HingeJoint::set_useSpring" ,    // -> HingeJoint_Set_Custom_PropUseSpring
    "UnityEngine.HingeJoint::get_velocity"  ,    // -> HingeJoint_Get_Custom_PropVelocity
    "UnityEngine.HingeJoint::get_angle"     ,    // -> HingeJoint_Get_Custom_PropAngle
    "UnityEngine.SpringJoint::get_spring"   ,    // -> SpringJoint_Get_Custom_PropSpring
    "UnityEngine.SpringJoint::set_spring"   ,    // -> SpringJoint_Set_Custom_PropSpring
    "UnityEngine.SpringJoint::get_damper"   ,    // -> SpringJoint_Get_Custom_PropDamper
    "UnityEngine.SpringJoint::set_damper"   ,    // -> SpringJoint_Set_Custom_PropDamper
    "UnityEngine.SpringJoint::get_minDistance",    // -> SpringJoint_Get_Custom_PropMinDistance
    "UnityEngine.SpringJoint::set_minDistance",    // -> SpringJoint_Set_Custom_PropMinDistance
    "UnityEngine.SpringJoint::get_maxDistance",    // -> SpringJoint_Get_Custom_PropMaxDistance
    "UnityEngine.SpringJoint::set_maxDistance",    // -> SpringJoint_Set_Custom_PropMaxDistance
    "UnityEngine.CharacterJoint::INTERNAL_get_swingAxis",    // -> CharacterJoint_CUSTOM_INTERNAL_get_swingAxis
    "UnityEngine.CharacterJoint::INTERNAL_set_swingAxis",    // -> CharacterJoint_CUSTOM_INTERNAL_set_swingAxis
    "UnityEngine.CharacterJoint::INTERNAL_get_lowTwistLimit",    // -> CharacterJoint_CUSTOM_INTERNAL_get_lowTwistLimit
    "UnityEngine.CharacterJoint::INTERNAL_set_lowTwistLimit",    // -> CharacterJoint_CUSTOM_INTERNAL_set_lowTwistLimit
    "UnityEngine.CharacterJoint::INTERNAL_get_highTwistLimit",    // -> CharacterJoint_CUSTOM_INTERNAL_get_highTwistLimit
    "UnityEngine.CharacterJoint::INTERNAL_set_highTwistLimit",    // -> CharacterJoint_CUSTOM_INTERNAL_set_highTwistLimit
    "UnityEngine.CharacterJoint::INTERNAL_get_swing1Limit",    // -> CharacterJoint_CUSTOM_INTERNAL_get_swing1Limit
    "UnityEngine.CharacterJoint::INTERNAL_set_swing1Limit",    // -> CharacterJoint_CUSTOM_INTERNAL_set_swing1Limit
    "UnityEngine.CharacterJoint::INTERNAL_get_swing2Limit",    // -> CharacterJoint_CUSTOM_INTERNAL_get_swing2Limit
    "UnityEngine.CharacterJoint::INTERNAL_set_swing2Limit",    // -> CharacterJoint_CUSTOM_INTERNAL_set_swing2Limit
    "UnityEngine.CharacterJoint::INTERNAL_get_targetRotation",    // -> CharacterJoint_CUSTOM_INTERNAL_get_targetRotation
    "UnityEngine.CharacterJoint::INTERNAL_set_targetRotation",    // -> CharacterJoint_CUSTOM_INTERNAL_set_targetRotation
    "UnityEngine.CharacterJoint::INTERNAL_get_targetAngularVelocity",    // -> CharacterJoint_CUSTOM_INTERNAL_get_targetAngularVelocity
    "UnityEngine.CharacterJoint::INTERNAL_set_targetAngularVelocity",    // -> CharacterJoint_CUSTOM_INTERNAL_set_targetAngularVelocity
    "UnityEngine.CharacterJoint::INTERNAL_get_rotationDrive",    // -> CharacterJoint_CUSTOM_INTERNAL_get_rotationDrive
    "UnityEngine.CharacterJoint::INTERNAL_set_rotationDrive",    // -> CharacterJoint_CUSTOM_INTERNAL_set_rotationDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_get_secondaryAxis",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_secondaryAxis
    "UnityEngine.ConfigurableJoint::INTERNAL_set_secondaryAxis",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_secondaryAxis
    "UnityEngine.ConfigurableJoint::get_xMotion",    // -> ConfigurableJoint_Get_Custom_PropXMotion
    "UnityEngine.ConfigurableJoint::set_xMotion",    // -> ConfigurableJoint_Set_Custom_PropXMotion
    "UnityEngine.ConfigurableJoint::get_yMotion",    // -> ConfigurableJoint_Get_Custom_PropYMotion
    "UnityEngine.ConfigurableJoint::set_yMotion",    // -> ConfigurableJoint_Set_Custom_PropYMotion
    "UnityEngine.ConfigurableJoint::get_zMotion",    // -> ConfigurableJoint_Get_Custom_PropZMotion
    "UnityEngine.ConfigurableJoint::set_zMotion",    // -> ConfigurableJoint_Set_Custom_PropZMotion
    "UnityEngine.ConfigurableJoint::get_angularXMotion",    // -> ConfigurableJoint_Get_Custom_PropAngularXMotion
    "UnityEngine.ConfigurableJoint::set_angularXMotion",    // -> ConfigurableJoint_Set_Custom_PropAngularXMotion
    "UnityEngine.ConfigurableJoint::get_angularYMotion",    // -> ConfigurableJoint_Get_Custom_PropAngularYMotion
    "UnityEngine.ConfigurableJoint::set_angularYMotion",    // -> ConfigurableJoint_Set_Custom_PropAngularYMotion
    "UnityEngine.ConfigurableJoint::get_angularZMotion",    // -> ConfigurableJoint_Get_Custom_PropAngularZMotion
    "UnityEngine.ConfigurableJoint::set_angularZMotion",    // -> ConfigurableJoint_Set_Custom_PropAngularZMotion
    "UnityEngine.ConfigurableJoint::INTERNAL_get_linearLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_linearLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_set_linearLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_linearLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_get_lowAngularXLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_lowAngularXLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_set_lowAngularXLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_lowAngularXLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_get_highAngularXLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_highAngularXLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_set_highAngularXLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_highAngularXLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_get_angularYLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_angularYLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_set_angularYLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_angularYLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_get_angularZLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_angularZLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_set_angularZLimit",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_angularZLimit
    "UnityEngine.ConfigurableJoint::INTERNAL_get_targetPosition",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_targetPosition
    "UnityEngine.ConfigurableJoint::INTERNAL_set_targetPosition",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_targetPosition
    "UnityEngine.ConfigurableJoint::INTERNAL_get_targetVelocity",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_targetVelocity
    "UnityEngine.ConfigurableJoint::INTERNAL_set_targetVelocity",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_targetVelocity
    "UnityEngine.ConfigurableJoint::INTERNAL_get_xDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_xDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_set_xDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_xDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_get_yDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_yDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_set_yDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_yDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_get_zDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_zDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_set_zDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_zDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_get_targetRotation",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_targetRotation
    "UnityEngine.ConfigurableJoint::INTERNAL_set_targetRotation",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_targetRotation
    "UnityEngine.ConfigurableJoint::INTERNAL_get_targetAngularVelocity",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_targetAngularVelocity
    "UnityEngine.ConfigurableJoint::INTERNAL_set_targetAngularVelocity",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_targetAngularVelocity
    "UnityEngine.ConfigurableJoint::get_rotationDriveMode",    // -> ConfigurableJoint_Get_Custom_PropRotationDriveMode
    "UnityEngine.ConfigurableJoint::set_rotationDriveMode",    // -> ConfigurableJoint_Set_Custom_PropRotationDriveMode
    "UnityEngine.ConfigurableJoint::INTERNAL_get_angularXDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_angularXDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_set_angularXDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_angularXDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_get_angularYZDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_angularYZDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_set_angularYZDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_angularYZDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_get_slerpDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_get_slerpDrive
    "UnityEngine.ConfigurableJoint::INTERNAL_set_slerpDrive",    // -> ConfigurableJoint_CUSTOM_INTERNAL_set_slerpDrive
    "UnityEngine.ConfigurableJoint::get_projectionMode",    // -> ConfigurableJoint_Get_Custom_PropProjectionMode
    "UnityEngine.ConfigurableJoint::set_projectionMode",    // -> ConfigurableJoint_Set_Custom_PropProjectionMode
    "UnityEngine.ConfigurableJoint::get_projectionDistance",    // -> ConfigurableJoint_Get_Custom_PropProjectionDistance
    "UnityEngine.ConfigurableJoint::set_projectionDistance",    // -> ConfigurableJoint_Set_Custom_PropProjectionDistance
    "UnityEngine.ConfigurableJoint::get_projectionAngle",    // -> ConfigurableJoint_Get_Custom_PropProjectionAngle
    "UnityEngine.ConfigurableJoint::set_projectionAngle",    // -> ConfigurableJoint_Set_Custom_PropProjectionAngle
    "UnityEngine.ConfigurableJoint::get_configuredInWorldSpace",    // -> ConfigurableJoint_Get_Custom_PropConfiguredInWorldSpace
    "UnityEngine.ConfigurableJoint::set_configuredInWorldSpace",    // -> ConfigurableJoint_Set_Custom_PropConfiguredInWorldSpace
    "UnityEngine.ConfigurableJoint::get_swapBodies",    // -> ConfigurableJoint_Get_Custom_PropSwapBodies
    "UnityEngine.ConfigurableJoint::set_swapBodies",    // -> ConfigurableJoint_Set_Custom_PropSwapBodies
    "UnityEngine.ConstantForce::INTERNAL_get_force",    // -> ConstantForce_CUSTOM_INTERNAL_get_force
    "UnityEngine.ConstantForce::INTERNAL_set_force",    // -> ConstantForce_CUSTOM_INTERNAL_set_force
    "UnityEngine.ConstantForce::INTERNAL_get_relativeForce",    // -> ConstantForce_CUSTOM_INTERNAL_get_relativeForce
    "UnityEngine.ConstantForce::INTERNAL_set_relativeForce",    // -> ConstantForce_CUSTOM_INTERNAL_set_relativeForce
    "UnityEngine.ConstantForce::INTERNAL_get_torque",    // -> ConstantForce_CUSTOM_INTERNAL_get_torque
    "UnityEngine.ConstantForce::INTERNAL_set_torque",    // -> ConstantForce_CUSTOM_INTERNAL_set_torque
    "UnityEngine.ConstantForce::INTERNAL_get_relativeTorque",    // -> ConstantForce_CUSTOM_INTERNAL_get_relativeTorque
    "UnityEngine.ConstantForce::INTERNAL_set_relativeTorque",    // -> ConstantForce_CUSTOM_INTERNAL_set_relativeTorque
    "UnityEngine.Collider::get_enabled"     ,    // -> Collider_Get_Custom_PropEnabled
    "UnityEngine.Collider::set_enabled"     ,    // -> Collider_Set_Custom_PropEnabled
    "UnityEngine.Collider::get_attachedRigidbody",    // -> Collider_Get_Custom_PropAttachedRigidbody
    "UnityEngine.Collider::get_isTrigger"   ,    // -> Collider_Get_Custom_PropIsTrigger
    "UnityEngine.Collider::set_isTrigger"   ,    // -> Collider_Set_Custom_PropIsTrigger
    "UnityEngine.Collider::get_material"    ,    // -> Collider_Get_Custom_PropMaterial
    "UnityEngine.Collider::set_material"    ,    // -> Collider_Set_Custom_PropMaterial
    "UnityEngine.Collider::INTERNAL_CALL_ClosestPointOnBounds",    // -> Collider_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds
    "UnityEngine.Collider::get_sharedMaterial",    // -> Collider_Get_Custom_PropSharedMaterial
    "UnityEngine.Collider::set_sharedMaterial",    // -> Collider_Set_Custom_PropSharedMaterial
    "UnityEngine.Collider::INTERNAL_get_bounds",    // -> Collider_CUSTOM_INTERNAL_get_bounds
    "UnityEngine.Collider::INTERNAL_CALL_Internal_Raycast",    // -> Collider_CUSTOM_INTERNAL_CALL_Internal_Raycast
    "UnityEngine.BoxCollider::INTERNAL_get_center",    // -> BoxCollider_CUSTOM_INTERNAL_get_center
    "UnityEngine.BoxCollider::INTERNAL_set_center",    // -> BoxCollider_CUSTOM_INTERNAL_set_center
    "UnityEngine.BoxCollider::INTERNAL_get_size",    // -> BoxCollider_CUSTOM_INTERNAL_get_size
    "UnityEngine.BoxCollider::INTERNAL_set_size",    // -> BoxCollider_CUSTOM_INTERNAL_set_size
    "UnityEngine.SphereCollider::INTERNAL_get_center",    // -> SphereCollider_CUSTOM_INTERNAL_get_center
    "UnityEngine.SphereCollider::INTERNAL_set_center",    // -> SphereCollider_CUSTOM_INTERNAL_set_center
    "UnityEngine.SphereCollider::get_radius",    // -> SphereCollider_Get_Custom_PropRadius
    "UnityEngine.SphereCollider::set_radius",    // -> SphereCollider_Set_Custom_PropRadius
    "UnityEngine.MeshCollider::get_sharedMesh",    // -> MeshCollider_Get_Custom_PropSharedMesh
    "UnityEngine.MeshCollider::set_sharedMesh",    // -> MeshCollider_Set_Custom_PropSharedMesh
    "UnityEngine.MeshCollider::get_convex"  ,    // -> MeshCollider_Get_Custom_PropConvex
    "UnityEngine.MeshCollider::set_convex"  ,    // -> MeshCollider_Set_Custom_PropConvex
    "UnityEngine.MeshCollider::get_smoothSphereCollisions",    // -> MeshCollider_Get_Custom_PropSmoothSphereCollisions
    "UnityEngine.MeshCollider::set_smoothSphereCollisions",    // -> MeshCollider_Set_Custom_PropSmoothSphereCollisions
    "UnityEngine.CapsuleCollider::INTERNAL_get_center",    // -> CapsuleCollider_CUSTOM_INTERNAL_get_center
    "UnityEngine.CapsuleCollider::INTERNAL_set_center",    // -> CapsuleCollider_CUSTOM_INTERNAL_set_center
    "UnityEngine.CapsuleCollider::get_radius",    // -> CapsuleCollider_Get_Custom_PropRadius
    "UnityEngine.CapsuleCollider::set_radius",    // -> CapsuleCollider_Set_Custom_PropRadius
    "UnityEngine.CapsuleCollider::get_height",    // -> CapsuleCollider_Get_Custom_PropHeight
    "UnityEngine.CapsuleCollider::set_height",    // -> CapsuleCollider_Set_Custom_PropHeight
    "UnityEngine.CapsuleCollider::get_direction",    // -> CapsuleCollider_Get_Custom_PropDirection
    "UnityEngine.CapsuleCollider::set_direction",    // -> CapsuleCollider_Set_Custom_PropDirection
    "UnityEngine.RaycastCollider::INTERNAL_get_center",    // -> RaycastCollider_CUSTOM_INTERNAL_get_center
    "UnityEngine.RaycastCollider::INTERNAL_set_center",    // -> RaycastCollider_CUSTOM_INTERNAL_set_center
    "UnityEngine.RaycastCollider::get_length",    // -> RaycastCollider_Get_Custom_PropLength
    "UnityEngine.RaycastCollider::set_length",    // -> RaycastCollider_Set_Custom_PropLength
    "UnityEngine.WheelCollider::INTERNAL_get_center",    // -> WheelCollider_CUSTOM_INTERNAL_get_center
    "UnityEngine.WheelCollider::INTERNAL_set_center",    // -> WheelCollider_CUSTOM_INTERNAL_set_center
    "UnityEngine.WheelCollider::get_radius" ,    // -> WheelCollider_Get_Custom_PropRadius
    "UnityEngine.WheelCollider::set_radius" ,    // -> WheelCollider_Set_Custom_PropRadius
    "UnityEngine.WheelCollider::get_suspensionDistance",    // -> WheelCollider_Get_Custom_PropSuspensionDistance
    "UnityEngine.WheelCollider::set_suspensionDistance",    // -> WheelCollider_Set_Custom_PropSuspensionDistance
    "UnityEngine.WheelCollider::INTERNAL_get_suspensionSpring",    // -> WheelCollider_CUSTOM_INTERNAL_get_suspensionSpring
    "UnityEngine.WheelCollider::INTERNAL_set_suspensionSpring",    // -> WheelCollider_CUSTOM_INTERNAL_set_suspensionSpring
    "UnityEngine.WheelCollider::get_mass"   ,    // -> WheelCollider_Get_Custom_PropMass
    "UnityEngine.WheelCollider::set_mass"   ,    // -> WheelCollider_Set_Custom_PropMass
    "UnityEngine.WheelCollider::INTERNAL_get_forwardFriction",    // -> WheelCollider_CUSTOM_INTERNAL_get_forwardFriction
    "UnityEngine.WheelCollider::INTERNAL_set_forwardFriction",    // -> WheelCollider_CUSTOM_INTERNAL_set_forwardFriction
    "UnityEngine.WheelCollider::INTERNAL_get_sidewaysFriction",    // -> WheelCollider_CUSTOM_INTERNAL_get_sidewaysFriction
    "UnityEngine.WheelCollider::INTERNAL_set_sidewaysFriction",    // -> WheelCollider_CUSTOM_INTERNAL_set_sidewaysFriction
    "UnityEngine.WheelCollider::get_motorTorque",    // -> WheelCollider_Get_Custom_PropMotorTorque
    "UnityEngine.WheelCollider::set_motorTorque",    // -> WheelCollider_Set_Custom_PropMotorTorque
    "UnityEngine.WheelCollider::get_brakeTorque",    // -> WheelCollider_Get_Custom_PropBrakeTorque
    "UnityEngine.WheelCollider::set_brakeTorque",    // -> WheelCollider_Set_Custom_PropBrakeTorque
    "UnityEngine.WheelCollider::get_steerAngle",    // -> WheelCollider_Get_Custom_PropSteerAngle
    "UnityEngine.WheelCollider::set_steerAngle",    // -> WheelCollider_Set_Custom_PropSteerAngle
    "UnityEngine.WheelCollider::get_isGrounded",    // -> WheelCollider_Get_Custom_PropIsGrounded
    "UnityEngine.WheelCollider::GetGroundHit",    // -> WheelCollider_CUSTOM_GetGroundHit
    "UnityEngine.WheelCollider::get_rpm"    ,    // -> WheelCollider_Get_Custom_PropRpm
    "UnityEngine.RaycastHit::INTERNAL_CALL_CalculateRaycastTexCoord",    // -> RaycastHit_CUSTOM_INTERNAL_CALL_CalculateRaycastTexCoord
    "UnityEngine.PhysicMaterial::Internal_CreateDynamicsMaterial",    // -> PhysicMaterial_CUSTOM_Internal_CreateDynamicsMaterial
    "UnityEngine.PhysicMaterial::get_dynamicFriction",    // -> PhysicMaterial_Get_Custom_PropDynamicFriction
    "UnityEngine.PhysicMaterial::set_dynamicFriction",    // -> PhysicMaterial_Set_Custom_PropDynamicFriction
    "UnityEngine.PhysicMaterial::get_staticFriction",    // -> PhysicMaterial_Get_Custom_PropStaticFriction
    "UnityEngine.PhysicMaterial::set_staticFriction",    // -> PhysicMaterial_Set_Custom_PropStaticFriction
    "UnityEngine.PhysicMaterial::get_bounciness",    // -> PhysicMaterial_Get_Custom_PropBounciness
    "UnityEngine.PhysicMaterial::set_bounciness",    // -> PhysicMaterial_Set_Custom_PropBounciness
    "UnityEngine.PhysicMaterial::INTERNAL_get_frictionDirection2",    // -> PhysicMaterial_CUSTOM_INTERNAL_get_frictionDirection2
    "UnityEngine.PhysicMaterial::INTERNAL_set_frictionDirection2",    // -> PhysicMaterial_CUSTOM_INTERNAL_set_frictionDirection2
    "UnityEngine.PhysicMaterial::get_dynamicFriction2",    // -> PhysicMaterial_Get_Custom_PropDynamicFriction2
    "UnityEngine.PhysicMaterial::set_dynamicFriction2",    // -> PhysicMaterial_Set_Custom_PropDynamicFriction2
    "UnityEngine.PhysicMaterial::get_staticFriction2",    // -> PhysicMaterial_Get_Custom_PropStaticFriction2
    "UnityEngine.PhysicMaterial::set_staticFriction2",    // -> PhysicMaterial_Set_Custom_PropStaticFriction2
    "UnityEngine.PhysicMaterial::get_frictionCombine",    // -> PhysicMaterial_Get_Custom_PropFrictionCombine
    "UnityEngine.PhysicMaterial::set_frictionCombine",    // -> PhysicMaterial_Set_Custom_PropFrictionCombine
    "UnityEngine.PhysicMaterial::get_bounceCombine",    // -> PhysicMaterial_Get_Custom_PropBounceCombine
    "UnityEngine.PhysicMaterial::set_bounceCombine",    // -> PhysicMaterial_Set_Custom_PropBounceCombine
    "UnityEngine.CharacterController::INTERNAL_CALL_SimpleMove",    // -> CharacterController_CUSTOM_INTERNAL_CALL_SimpleMove
    "UnityEngine.CharacterController::INTERNAL_CALL_Move",    // -> CharacterController_CUSTOM_INTERNAL_CALL_Move
    "UnityEngine.CharacterController::get_isGrounded",    // -> CharacterController_Get_Custom_PropIsGrounded
    "UnityEngine.CharacterController::INTERNAL_get_velocity",    // -> CharacterController_CUSTOM_INTERNAL_get_velocity
    "UnityEngine.CharacterController::get_collisionFlags",    // -> CharacterController_Get_Custom_PropCollisionFlags
    "UnityEngine.CharacterController::get_radius",    // -> CharacterController_Get_Custom_PropRadius
    "UnityEngine.CharacterController::set_radius",    // -> CharacterController_Set_Custom_PropRadius
    "UnityEngine.CharacterController::get_height",    // -> CharacterController_Get_Custom_PropHeight
    "UnityEngine.CharacterController::set_height",    // -> CharacterController_Set_Custom_PropHeight
    "UnityEngine.CharacterController::INTERNAL_get_center",    // -> CharacterController_CUSTOM_INTERNAL_get_center
    "UnityEngine.CharacterController::INTERNAL_set_center",    // -> CharacterController_CUSTOM_INTERNAL_set_center
    "UnityEngine.CharacterController::get_slopeLimit",    // -> CharacterController_Get_Custom_PropSlopeLimit
    "UnityEngine.CharacterController::set_slopeLimit",    // -> CharacterController_Set_Custom_PropSlopeLimit
    "UnityEngine.CharacterController::get_stepOffset",    // -> CharacterController_Get_Custom_PropStepOffset
    "UnityEngine.CharacterController::set_stepOffset",    // -> CharacterController_Set_Custom_PropStepOffset
    "UnityEngine.CharacterController::get_detectCollisions",    // -> CharacterController_Get_Custom_PropDetectCollisions
    "UnityEngine.CharacterController::set_detectCollisions",    // -> CharacterController_Set_Custom_PropDetectCollisions
#endif
#if ENABLE_CLOTH
    "UnityEngine.Cloth::get_bendingStiffness",    // -> Cloth_Get_Custom_PropBendingStiffness
    "UnityEngine.Cloth::set_bendingStiffness",    // -> Cloth_Set_Custom_PropBendingStiffness
    "UnityEngine.Cloth::get_stretchingStiffness",    // -> Cloth_Get_Custom_PropStretchingStiffness
    "UnityEngine.Cloth::set_stretchingStiffness",    // -> Cloth_Set_Custom_PropStretchingStiffness
    "UnityEngine.Cloth::get_damping"        ,    // -> Cloth_Get_Custom_PropDamping
    "UnityEngine.Cloth::set_damping"        ,    // -> Cloth_Set_Custom_PropDamping
    "UnityEngine.Cloth::get_thickness"      ,    // -> Cloth_Get_Custom_PropThickness
    "UnityEngine.Cloth::set_thickness"      ,    // -> Cloth_Set_Custom_PropThickness
    "UnityEngine.Cloth::INTERNAL_get_externalAcceleration",    // -> Cloth_CUSTOM_INTERNAL_get_externalAcceleration
    "UnityEngine.Cloth::INTERNAL_set_externalAcceleration",    // -> Cloth_CUSTOM_INTERNAL_set_externalAcceleration
    "UnityEngine.Cloth::INTERNAL_get_randomAcceleration",    // -> Cloth_CUSTOM_INTERNAL_get_randomAcceleration
    "UnityEngine.Cloth::INTERNAL_set_randomAcceleration",    // -> Cloth_CUSTOM_INTERNAL_set_randomAcceleration
    "UnityEngine.Cloth::get_useGravity"     ,    // -> Cloth_Get_Custom_PropUseGravity
    "UnityEngine.Cloth::set_useGravity"     ,    // -> Cloth_Set_Custom_PropUseGravity
    "UnityEngine.Cloth::get_selfCollision"  ,    // -> Cloth_Get_Custom_PropSelfCollision
    "UnityEngine.Cloth::set_selfCollision"  ,    // -> Cloth_Set_Custom_PropSelfCollision
    "UnityEngine.Cloth::get_enabled"        ,    // -> Cloth_Get_Custom_PropEnabled
    "UnityEngine.Cloth::set_enabled"        ,    // -> Cloth_Set_Custom_PropEnabled
    "UnityEngine.Cloth::get_vertices"       ,    // -> Cloth_Get_Custom_PropVertices
    "UnityEngine.Cloth::get_normals"        ,    // -> Cloth_Get_Custom_PropNormals
    "UnityEngine.InteractiveCloth::get_mesh",    // -> InteractiveCloth_Get_Custom_PropMesh
    "UnityEngine.InteractiveCloth::set_mesh",    // -> InteractiveCloth_Set_Custom_PropMesh
    "UnityEngine.InteractiveCloth::get_friction",    // -> InteractiveCloth_Get_Custom_PropFriction
    "UnityEngine.InteractiveCloth::set_friction",    // -> InteractiveCloth_Set_Custom_PropFriction
    "UnityEngine.InteractiveCloth::get_density",    // -> InteractiveCloth_Get_Custom_PropDensity
    "UnityEngine.InteractiveCloth::set_density",    // -> InteractiveCloth_Set_Custom_PropDensity
    "UnityEngine.InteractiveCloth::get_pressure",    // -> InteractiveCloth_Get_Custom_PropPressure
    "UnityEngine.InteractiveCloth::set_pressure",    // -> InteractiveCloth_Set_Custom_PropPressure
    "UnityEngine.InteractiveCloth::get_collisionResponse",    // -> InteractiveCloth_Get_Custom_PropCollisionResponse
    "UnityEngine.InteractiveCloth::set_collisionResponse",    // -> InteractiveCloth_Set_Custom_PropCollisionResponse
    "UnityEngine.InteractiveCloth::get_tearFactor",    // -> InteractiveCloth_Get_Custom_PropTearFactor
    "UnityEngine.InteractiveCloth::set_tearFactor",    // -> InteractiveCloth_Set_Custom_PropTearFactor
    "UnityEngine.InteractiveCloth::get_attachmentTearFactor",    // -> InteractiveCloth_Get_Custom_PropAttachmentTearFactor
    "UnityEngine.InteractiveCloth::set_attachmentTearFactor",    // -> InteractiveCloth_Set_Custom_PropAttachmentTearFactor
    "UnityEngine.InteractiveCloth::get_attachmentResponse",    // -> InteractiveCloth_Get_Custom_PropAttachmentResponse
    "UnityEngine.InteractiveCloth::set_attachmentResponse",    // -> InteractiveCloth_Set_Custom_PropAttachmentResponse
    "UnityEngine.InteractiveCloth::get_isTeared",    // -> InteractiveCloth_Get_Custom_PropIsTeared
    "UnityEngine.InteractiveCloth::INTERNAL_CALL_AddForceAtPosition",    // -> InteractiveCloth_CUSTOM_INTERNAL_CALL_AddForceAtPosition
    "UnityEngine.InteractiveCloth::AttachToCollider",    // -> InteractiveCloth_CUSTOM_AttachToCollider
    "UnityEngine.InteractiveCloth::DetachFromCollider",    // -> InteractiveCloth_CUSTOM_DetachFromCollider
    "UnityEngine.SkinnedCloth::get_coefficients",    // -> SkinnedCloth_Get_Custom_PropCoefficients
    "UnityEngine.SkinnedCloth::set_coefficients",    // -> SkinnedCloth_Set_Custom_PropCoefficients
    "UnityEngine.SkinnedCloth::get_worldVelocityScale",    // -> SkinnedCloth_Get_Custom_PropWorldVelocityScale
    "UnityEngine.SkinnedCloth::set_worldVelocityScale",    // -> SkinnedCloth_Set_Custom_PropWorldVelocityScale
    "UnityEngine.SkinnedCloth::get_worldAccelerationScale",    // -> SkinnedCloth_Get_Custom_PropWorldAccelerationScale
    "UnityEngine.SkinnedCloth::set_worldAccelerationScale",    // -> SkinnedCloth_Set_Custom_PropWorldAccelerationScale
    "UnityEngine.SkinnedCloth::SetEnabledFading",    // -> SkinnedCloth_CUSTOM_SetEnabledFading
    "UnityEngine.ClothRenderer::get_pauseWhenNotVisible",    // -> ClothRenderer_Get_Custom_PropPauseWhenNotVisible
    "UnityEngine.ClothRenderer::set_pauseWhenNotVisible",    // -> ClothRenderer_Set_Custom_PropPauseWhenNotVisible
#endif
    NULL
};

static const void* s_NewDynamics_IcallFuncs [] =
{
#if ENABLE_PHYSICS
    (const void*)&Physics_CUSTOM_INTERNAL_get_gravity     ,  //  <- UnityEngine.Physics::INTERNAL_get_gravity
    (const void*)&Physics_CUSTOM_INTERNAL_set_gravity     ,  //  <- UnityEngine.Physics::INTERNAL_set_gravity
    (const void*)&Physics_Get_Custom_PropMinPenetrationForPenalty,  //  <- UnityEngine.Physics::get_minPenetrationForPenalty
    (const void*)&Physics_Set_Custom_PropMinPenetrationForPenalty,  //  <- UnityEngine.Physics::set_minPenetrationForPenalty
    (const void*)&Physics_Get_Custom_PropBounceThreshold  ,  //  <- UnityEngine.Physics::get_bounceThreshold
    (const void*)&Physics_Set_Custom_PropBounceThreshold  ,  //  <- UnityEngine.Physics::set_bounceThreshold
    (const void*)&Physics_Get_Custom_PropSleepVelocity    ,  //  <- UnityEngine.Physics::get_sleepVelocity
    (const void*)&Physics_Set_Custom_PropSleepVelocity    ,  //  <- UnityEngine.Physics::set_sleepVelocity
    (const void*)&Physics_Get_Custom_PropSleepAngularVelocity,  //  <- UnityEngine.Physics::get_sleepAngularVelocity
    (const void*)&Physics_Set_Custom_PropSleepAngularVelocity,  //  <- UnityEngine.Physics::set_sleepAngularVelocity
    (const void*)&Physics_Get_Custom_PropMaxAngularVelocity,  //  <- UnityEngine.Physics::get_maxAngularVelocity
    (const void*)&Physics_Set_Custom_PropMaxAngularVelocity,  //  <- UnityEngine.Physics::set_maxAngularVelocity
    (const void*)&Physics_Get_Custom_PropSolverIterationCount,  //  <- UnityEngine.Physics::get_solverIterationCount
    (const void*)&Physics_Set_Custom_PropSolverIterationCount,  //  <- UnityEngine.Physics::set_solverIterationCount
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_Internal_Raycast,  //  <- UnityEngine.Physics::INTERNAL_CALL_Internal_Raycast
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_Internal_CapsuleCast,  //  <- UnityEngine.Physics::INTERNAL_CALL_Internal_CapsuleCast
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_Internal_RaycastTest,  //  <- UnityEngine.Physics::INTERNAL_CALL_Internal_RaycastTest
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_RaycastAll ,  //  <- UnityEngine.Physics::INTERNAL_CALL_RaycastAll
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_OverlapSphere,  //  <- UnityEngine.Physics::INTERNAL_CALL_OverlapSphere
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_CapsuleCastAll,  //  <- UnityEngine.Physics::INTERNAL_CALL_CapsuleCastAll
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_CheckSphere,  //  <- UnityEngine.Physics::INTERNAL_CALL_CheckSphere
    (const void*)&Physics_CUSTOM_INTERNAL_CALL_CheckCapsule,  //  <- UnityEngine.Physics::INTERNAL_CALL_CheckCapsule
    (const void*)&Physics_Get_Custom_PropPenetrationPenaltyForce,  //  <- UnityEngine.Physics::get_penetrationPenaltyForce
    (const void*)&Physics_Set_Custom_PropPenetrationPenaltyForce,  //  <- UnityEngine.Physics::set_penetrationPenaltyForce
    (const void*)&Physics_CUSTOM_IgnoreCollision          ,  //  <- UnityEngine.Physics::IgnoreCollision
    (const void*)&Physics_CUSTOM_IgnoreLayerCollision     ,  //  <- UnityEngine.Physics::IgnoreLayerCollision
    (const void*)&Physics_CUSTOM_GetIgnoreLayerCollision  ,  //  <- UnityEngine.Physics::GetIgnoreLayerCollision
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_velocity  ,  //  <- UnityEngine.Rigidbody::INTERNAL_get_velocity
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_velocity  ,  //  <- UnityEngine.Rigidbody::INTERNAL_set_velocity
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_angularVelocity,  //  <- UnityEngine.Rigidbody::INTERNAL_get_angularVelocity
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_angularVelocity,  //  <- UnityEngine.Rigidbody::INTERNAL_set_angularVelocity
    (const void*)&Rigidbody_Get_Custom_PropDrag           ,  //  <- UnityEngine.Rigidbody::get_drag
    (const void*)&Rigidbody_Set_Custom_PropDrag           ,  //  <- UnityEngine.Rigidbody::set_drag
    (const void*)&Rigidbody_Get_Custom_PropAngularDrag    ,  //  <- UnityEngine.Rigidbody::get_angularDrag
    (const void*)&Rigidbody_Set_Custom_PropAngularDrag    ,  //  <- UnityEngine.Rigidbody::set_angularDrag
    (const void*)&Rigidbody_Get_Custom_PropMass           ,  //  <- UnityEngine.Rigidbody::get_mass
    (const void*)&Rigidbody_Set_Custom_PropMass           ,  //  <- UnityEngine.Rigidbody::set_mass
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_SetDensity,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_SetDensity
    (const void*)&Rigidbody_Get_Custom_PropUseGravity     ,  //  <- UnityEngine.Rigidbody::get_useGravity
    (const void*)&Rigidbody_Set_Custom_PropUseGravity     ,  //  <- UnityEngine.Rigidbody::set_useGravity
    (const void*)&Rigidbody_Get_Custom_PropIsKinematic    ,  //  <- UnityEngine.Rigidbody::get_isKinematic
    (const void*)&Rigidbody_Set_Custom_PropIsKinematic    ,  //  <- UnityEngine.Rigidbody::set_isKinematic
    (const void*)&Rigidbody_Get_Custom_PropFreezeRotation ,  //  <- UnityEngine.Rigidbody::get_freezeRotation
    (const void*)&Rigidbody_Set_Custom_PropFreezeRotation ,  //  <- UnityEngine.Rigidbody::set_freezeRotation
    (const void*)&Rigidbody_Get_Custom_PropConstraints    ,  //  <- UnityEngine.Rigidbody::get_constraints
    (const void*)&Rigidbody_Set_Custom_PropConstraints    ,  //  <- UnityEngine.Rigidbody::set_constraints
    (const void*)&Rigidbody_Get_Custom_PropCollisionDetectionMode,  //  <- UnityEngine.Rigidbody::get_collisionDetectionMode
    (const void*)&Rigidbody_Set_Custom_PropCollisionDetectionMode,  //  <- UnityEngine.Rigidbody::set_collisionDetectionMode
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_AddForce ,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddForce
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeForce,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeForce
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_AddTorque,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddTorque
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeTorque,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeTorque
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_AddForceAtPosition,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddForceAtPosition
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_AddExplosionForce,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddExplosionForce
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_ClosestPointOnBounds
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_GetRelativePointVelocity,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_GetRelativePointVelocity
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_GetPointVelocity,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_GetPointVelocity
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_centerOfMass,  //  <- UnityEngine.Rigidbody::INTERNAL_get_centerOfMass
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_centerOfMass,  //  <- UnityEngine.Rigidbody::INTERNAL_set_centerOfMass
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_worldCenterOfMass,  //  <- UnityEngine.Rigidbody::INTERNAL_get_worldCenterOfMass
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_inertiaTensorRotation,  //  <- UnityEngine.Rigidbody::INTERNAL_get_inertiaTensorRotation
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_inertiaTensorRotation,  //  <- UnityEngine.Rigidbody::INTERNAL_set_inertiaTensorRotation
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_inertiaTensor,  //  <- UnityEngine.Rigidbody::INTERNAL_get_inertiaTensor
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_inertiaTensor,  //  <- UnityEngine.Rigidbody::INTERNAL_set_inertiaTensor
    (const void*)&Rigidbody_Get_Custom_PropDetectCollisions,  //  <- UnityEngine.Rigidbody::get_detectCollisions
    (const void*)&Rigidbody_Set_Custom_PropDetectCollisions,  //  <- UnityEngine.Rigidbody::set_detectCollisions
    (const void*)&Rigidbody_Get_Custom_PropUseConeFriction,  //  <- UnityEngine.Rigidbody::get_useConeFriction
    (const void*)&Rigidbody_Set_Custom_PropUseConeFriction,  //  <- UnityEngine.Rigidbody::set_useConeFriction
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_position  ,  //  <- UnityEngine.Rigidbody::INTERNAL_get_position
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_position  ,  //  <- UnityEngine.Rigidbody::INTERNAL_set_position
    (const void*)&Rigidbody_CUSTOM_INTERNAL_get_rotation  ,  //  <- UnityEngine.Rigidbody::INTERNAL_get_rotation
    (const void*)&Rigidbody_CUSTOM_INTERNAL_set_rotation  ,  //  <- UnityEngine.Rigidbody::INTERNAL_set_rotation
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_MovePosition,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_MovePosition
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_MoveRotation,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_MoveRotation
    (const void*)&Rigidbody_Get_Custom_PropInterpolation  ,  //  <- UnityEngine.Rigidbody::get_interpolation
    (const void*)&Rigidbody_Set_Custom_PropInterpolation  ,  //  <- UnityEngine.Rigidbody::set_interpolation
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_Sleep    ,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_Sleep
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_IsSleeping,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_IsSleeping
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_WakeUp   ,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_WakeUp
    (const void*)&Rigidbody_Get_Custom_PropSolverIterationCount,  //  <- UnityEngine.Rigidbody::get_solverIterationCount
    (const void*)&Rigidbody_Set_Custom_PropSolverIterationCount,  //  <- UnityEngine.Rigidbody::set_solverIterationCount
    (const void*)&Rigidbody_Get_Custom_PropSleepVelocity  ,  //  <- UnityEngine.Rigidbody::get_sleepVelocity
    (const void*)&Rigidbody_Set_Custom_PropSleepVelocity  ,  //  <- UnityEngine.Rigidbody::set_sleepVelocity
    (const void*)&Rigidbody_Get_Custom_PropSleepAngularVelocity,  //  <- UnityEngine.Rigidbody::get_sleepAngularVelocity
    (const void*)&Rigidbody_Set_Custom_PropSleepAngularVelocity,  //  <- UnityEngine.Rigidbody::set_sleepAngularVelocity
    (const void*)&Rigidbody_Get_Custom_PropMaxAngularVelocity,  //  <- UnityEngine.Rigidbody::get_maxAngularVelocity
    (const void*)&Rigidbody_Set_Custom_PropMaxAngularVelocity,  //  <- UnityEngine.Rigidbody::set_maxAngularVelocity
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_SweepTest,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_SweepTest
    (const void*)&Rigidbody_CUSTOM_INTERNAL_CALL_SweepTestAll,  //  <- UnityEngine.Rigidbody::INTERNAL_CALL_SweepTestAll
    (const void*)&Joint_Get_Custom_PropConnectedBody      ,  //  <- UnityEngine.Joint::get_connectedBody
    (const void*)&Joint_Set_Custom_PropConnectedBody      ,  //  <- UnityEngine.Joint::set_connectedBody
    (const void*)&Joint_CUSTOM_INTERNAL_get_axis          ,  //  <- UnityEngine.Joint::INTERNAL_get_axis
    (const void*)&Joint_CUSTOM_INTERNAL_set_axis          ,  //  <- UnityEngine.Joint::INTERNAL_set_axis
    (const void*)&Joint_CUSTOM_INTERNAL_get_anchor        ,  //  <- UnityEngine.Joint::INTERNAL_get_anchor
    (const void*)&Joint_CUSTOM_INTERNAL_set_anchor        ,  //  <- UnityEngine.Joint::INTERNAL_set_anchor
    (const void*)&Joint_Get_Custom_PropBreakForce         ,  //  <- UnityEngine.Joint::get_breakForce
    (const void*)&Joint_Set_Custom_PropBreakForce         ,  //  <- UnityEngine.Joint::set_breakForce
    (const void*)&Joint_Get_Custom_PropBreakTorque        ,  //  <- UnityEngine.Joint::get_breakTorque
    (const void*)&Joint_Set_Custom_PropBreakTorque        ,  //  <- UnityEngine.Joint::set_breakTorque
    (const void*)&HingeJoint_CUSTOM_INTERNAL_get_motor    ,  //  <- UnityEngine.HingeJoint::INTERNAL_get_motor
    (const void*)&HingeJoint_CUSTOM_INTERNAL_set_motor    ,  //  <- UnityEngine.HingeJoint::INTERNAL_set_motor
    (const void*)&HingeJoint_CUSTOM_INTERNAL_get_limits   ,  //  <- UnityEngine.HingeJoint::INTERNAL_get_limits
    (const void*)&HingeJoint_CUSTOM_INTERNAL_set_limits   ,  //  <- UnityEngine.HingeJoint::INTERNAL_set_limits
    (const void*)&HingeJoint_CUSTOM_INTERNAL_get_spring   ,  //  <- UnityEngine.HingeJoint::INTERNAL_get_spring
    (const void*)&HingeJoint_CUSTOM_INTERNAL_set_spring   ,  //  <- UnityEngine.HingeJoint::INTERNAL_set_spring
    (const void*)&HingeJoint_Get_Custom_PropUseMotor      ,  //  <- UnityEngine.HingeJoint::get_useMotor
    (const void*)&HingeJoint_Set_Custom_PropUseMotor      ,  //  <- UnityEngine.HingeJoint::set_useMotor
    (const void*)&HingeJoint_Get_Custom_PropUseLimits     ,  //  <- UnityEngine.HingeJoint::get_useLimits
    (const void*)&HingeJoint_Set_Custom_PropUseLimits     ,  //  <- UnityEngine.HingeJoint::set_useLimits
    (const void*)&HingeJoint_Get_Custom_PropUseSpring     ,  //  <- UnityEngine.HingeJoint::get_useSpring
    (const void*)&HingeJoint_Set_Custom_PropUseSpring     ,  //  <- UnityEngine.HingeJoint::set_useSpring
    (const void*)&HingeJoint_Get_Custom_PropVelocity      ,  //  <- UnityEngine.HingeJoint::get_velocity
    (const void*)&HingeJoint_Get_Custom_PropAngle         ,  //  <- UnityEngine.HingeJoint::get_angle
    (const void*)&SpringJoint_Get_Custom_PropSpring       ,  //  <- UnityEngine.SpringJoint::get_spring
    (const void*)&SpringJoint_Set_Custom_PropSpring       ,  //  <- UnityEngine.SpringJoint::set_spring
    (const void*)&SpringJoint_Get_Custom_PropDamper       ,  //  <- UnityEngine.SpringJoint::get_damper
    (const void*)&SpringJoint_Set_Custom_PropDamper       ,  //  <- UnityEngine.SpringJoint::set_damper
    (const void*)&SpringJoint_Get_Custom_PropMinDistance  ,  //  <- UnityEngine.SpringJoint::get_minDistance
    (const void*)&SpringJoint_Set_Custom_PropMinDistance  ,  //  <- UnityEngine.SpringJoint::set_minDistance
    (const void*)&SpringJoint_Get_Custom_PropMaxDistance  ,  //  <- UnityEngine.SpringJoint::get_maxDistance
    (const void*)&SpringJoint_Set_Custom_PropMaxDistance  ,  //  <- UnityEngine.SpringJoint::set_maxDistance
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_swingAxis,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_swingAxis
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_swingAxis,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_swingAxis
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_lowTwistLimit,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_lowTwistLimit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_lowTwistLimit,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_lowTwistLimit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_highTwistLimit,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_highTwistLimit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_highTwistLimit,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_highTwistLimit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_swing1Limit,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_swing1Limit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_swing1Limit,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_swing1Limit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_swing2Limit,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_swing2Limit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_swing2Limit,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_swing2Limit
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_targetRotation,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_targetRotation
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_targetRotation,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_targetRotation
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_targetAngularVelocity,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_targetAngularVelocity
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_targetAngularVelocity,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_targetAngularVelocity
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_get_rotationDrive,  //  <- UnityEngine.CharacterJoint::INTERNAL_get_rotationDrive
    (const void*)&CharacterJoint_CUSTOM_INTERNAL_set_rotationDrive,  //  <- UnityEngine.CharacterJoint::INTERNAL_set_rotationDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_secondaryAxis,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_secondaryAxis
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_secondaryAxis,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_secondaryAxis
    (const void*)&ConfigurableJoint_Get_Custom_PropXMotion,  //  <- UnityEngine.ConfigurableJoint::get_xMotion
    (const void*)&ConfigurableJoint_Set_Custom_PropXMotion,  //  <- UnityEngine.ConfigurableJoint::set_xMotion
    (const void*)&ConfigurableJoint_Get_Custom_PropYMotion,  //  <- UnityEngine.ConfigurableJoint::get_yMotion
    (const void*)&ConfigurableJoint_Set_Custom_PropYMotion,  //  <- UnityEngine.ConfigurableJoint::set_yMotion
    (const void*)&ConfigurableJoint_Get_Custom_PropZMotion,  //  <- UnityEngine.ConfigurableJoint::get_zMotion
    (const void*)&ConfigurableJoint_Set_Custom_PropZMotion,  //  <- UnityEngine.ConfigurableJoint::set_zMotion
    (const void*)&ConfigurableJoint_Get_Custom_PropAngularXMotion,  //  <- UnityEngine.ConfigurableJoint::get_angularXMotion
    (const void*)&ConfigurableJoint_Set_Custom_PropAngularXMotion,  //  <- UnityEngine.ConfigurableJoint::set_angularXMotion
    (const void*)&ConfigurableJoint_Get_Custom_PropAngularYMotion,  //  <- UnityEngine.ConfigurableJoint::get_angularYMotion
    (const void*)&ConfigurableJoint_Set_Custom_PropAngularYMotion,  //  <- UnityEngine.ConfigurableJoint::set_angularYMotion
    (const void*)&ConfigurableJoint_Get_Custom_PropAngularZMotion,  //  <- UnityEngine.ConfigurableJoint::get_angularZMotion
    (const void*)&ConfigurableJoint_Set_Custom_PropAngularZMotion,  //  <- UnityEngine.ConfigurableJoint::set_angularZMotion
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_linearLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_linearLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_linearLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_linearLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_lowAngularXLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_lowAngularXLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_lowAngularXLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_lowAngularXLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_highAngularXLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_highAngularXLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_highAngularXLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_highAngularXLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_angularYLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularYLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_angularYLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularYLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_angularZLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularZLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_angularZLimit,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularZLimit
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_targetPosition,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetPosition
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_targetPosition,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetPosition
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_targetVelocity,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetVelocity
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_targetVelocity,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetVelocity
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_xDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_xDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_xDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_xDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_yDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_yDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_yDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_yDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_zDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_zDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_zDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_zDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_targetRotation,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetRotation
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_targetRotation,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetRotation
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_targetAngularVelocity,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetAngularVelocity
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_targetAngularVelocity,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetAngularVelocity
    (const void*)&ConfigurableJoint_Get_Custom_PropRotationDriveMode,  //  <- UnityEngine.ConfigurableJoint::get_rotationDriveMode
    (const void*)&ConfigurableJoint_Set_Custom_PropRotationDriveMode,  //  <- UnityEngine.ConfigurableJoint::set_rotationDriveMode
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_angularXDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularXDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_angularXDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularXDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_angularYZDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularYZDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_angularYZDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularYZDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_get_slerpDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_slerpDrive
    (const void*)&ConfigurableJoint_CUSTOM_INTERNAL_set_slerpDrive,  //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_slerpDrive
    (const void*)&ConfigurableJoint_Get_Custom_PropProjectionMode,  //  <- UnityEngine.ConfigurableJoint::get_projectionMode
    (const void*)&ConfigurableJoint_Set_Custom_PropProjectionMode,  //  <- UnityEngine.ConfigurableJoint::set_projectionMode
    (const void*)&ConfigurableJoint_Get_Custom_PropProjectionDistance,  //  <- UnityEngine.ConfigurableJoint::get_projectionDistance
    (const void*)&ConfigurableJoint_Set_Custom_PropProjectionDistance,  //  <- UnityEngine.ConfigurableJoint::set_projectionDistance
    (const void*)&ConfigurableJoint_Get_Custom_PropProjectionAngle,  //  <- UnityEngine.ConfigurableJoint::get_projectionAngle
    (const void*)&ConfigurableJoint_Set_Custom_PropProjectionAngle,  //  <- UnityEngine.ConfigurableJoint::set_projectionAngle
    (const void*)&ConfigurableJoint_Get_Custom_PropConfiguredInWorldSpace,  //  <- UnityEngine.ConfigurableJoint::get_configuredInWorldSpace
    (const void*)&ConfigurableJoint_Set_Custom_PropConfiguredInWorldSpace,  //  <- UnityEngine.ConfigurableJoint::set_configuredInWorldSpace
    (const void*)&ConfigurableJoint_Get_Custom_PropSwapBodies,  //  <- UnityEngine.ConfigurableJoint::get_swapBodies
    (const void*)&ConfigurableJoint_Set_Custom_PropSwapBodies,  //  <- UnityEngine.ConfigurableJoint::set_swapBodies
    (const void*)&ConstantForce_CUSTOM_INTERNAL_get_force ,  //  <- UnityEngine.ConstantForce::INTERNAL_get_force
    (const void*)&ConstantForce_CUSTOM_INTERNAL_set_force ,  //  <- UnityEngine.ConstantForce::INTERNAL_set_force
    (const void*)&ConstantForce_CUSTOM_INTERNAL_get_relativeForce,  //  <- UnityEngine.ConstantForce::INTERNAL_get_relativeForce
    (const void*)&ConstantForce_CUSTOM_INTERNAL_set_relativeForce,  //  <- UnityEngine.ConstantForce::INTERNAL_set_relativeForce
    (const void*)&ConstantForce_CUSTOM_INTERNAL_get_torque,  //  <- UnityEngine.ConstantForce::INTERNAL_get_torque
    (const void*)&ConstantForce_CUSTOM_INTERNAL_set_torque,  //  <- UnityEngine.ConstantForce::INTERNAL_set_torque
    (const void*)&ConstantForce_CUSTOM_INTERNAL_get_relativeTorque,  //  <- UnityEngine.ConstantForce::INTERNAL_get_relativeTorque
    (const void*)&ConstantForce_CUSTOM_INTERNAL_set_relativeTorque,  //  <- UnityEngine.ConstantForce::INTERNAL_set_relativeTorque
    (const void*)&Collider_Get_Custom_PropEnabled         ,  //  <- UnityEngine.Collider::get_enabled
    (const void*)&Collider_Set_Custom_PropEnabled         ,  //  <- UnityEngine.Collider::set_enabled
    (const void*)&Collider_Get_Custom_PropAttachedRigidbody,  //  <- UnityEngine.Collider::get_attachedRigidbody
    (const void*)&Collider_Get_Custom_PropIsTrigger       ,  //  <- UnityEngine.Collider::get_isTrigger
    (const void*)&Collider_Set_Custom_PropIsTrigger       ,  //  <- UnityEngine.Collider::set_isTrigger
    (const void*)&Collider_Get_Custom_PropMaterial        ,  //  <- UnityEngine.Collider::get_material
    (const void*)&Collider_Set_Custom_PropMaterial        ,  //  <- UnityEngine.Collider::set_material
    (const void*)&Collider_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds,  //  <- UnityEngine.Collider::INTERNAL_CALL_ClosestPointOnBounds
    (const void*)&Collider_Get_Custom_PropSharedMaterial  ,  //  <- UnityEngine.Collider::get_sharedMaterial
    (const void*)&Collider_Set_Custom_PropSharedMaterial  ,  //  <- UnityEngine.Collider::set_sharedMaterial
    (const void*)&Collider_CUSTOM_INTERNAL_get_bounds     ,  //  <- UnityEngine.Collider::INTERNAL_get_bounds
    (const void*)&Collider_CUSTOM_INTERNAL_CALL_Internal_Raycast,  //  <- UnityEngine.Collider::INTERNAL_CALL_Internal_Raycast
    (const void*)&BoxCollider_CUSTOM_INTERNAL_get_center  ,  //  <- UnityEngine.BoxCollider::INTERNAL_get_center
    (const void*)&BoxCollider_CUSTOM_INTERNAL_set_center  ,  //  <- UnityEngine.BoxCollider::INTERNAL_set_center
    (const void*)&BoxCollider_CUSTOM_INTERNAL_get_size    ,  //  <- UnityEngine.BoxCollider::INTERNAL_get_size
    (const void*)&BoxCollider_CUSTOM_INTERNAL_set_size    ,  //  <- UnityEngine.BoxCollider::INTERNAL_set_size
    (const void*)&SphereCollider_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.SphereCollider::INTERNAL_get_center
    (const void*)&SphereCollider_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.SphereCollider::INTERNAL_set_center
    (const void*)&SphereCollider_Get_Custom_PropRadius    ,  //  <- UnityEngine.SphereCollider::get_radius
    (const void*)&SphereCollider_Set_Custom_PropRadius    ,  //  <- UnityEngine.SphereCollider::set_radius
    (const void*)&MeshCollider_Get_Custom_PropSharedMesh  ,  //  <- UnityEngine.MeshCollider::get_sharedMesh
    (const void*)&MeshCollider_Set_Custom_PropSharedMesh  ,  //  <- UnityEngine.MeshCollider::set_sharedMesh
    (const void*)&MeshCollider_Get_Custom_PropConvex      ,  //  <- UnityEngine.MeshCollider::get_convex
    (const void*)&MeshCollider_Set_Custom_PropConvex      ,  //  <- UnityEngine.MeshCollider::set_convex
    (const void*)&MeshCollider_Get_Custom_PropSmoothSphereCollisions,  //  <- UnityEngine.MeshCollider::get_smoothSphereCollisions
    (const void*)&MeshCollider_Set_Custom_PropSmoothSphereCollisions,  //  <- UnityEngine.MeshCollider::set_smoothSphereCollisions
    (const void*)&CapsuleCollider_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.CapsuleCollider::INTERNAL_get_center
    (const void*)&CapsuleCollider_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.CapsuleCollider::INTERNAL_set_center
    (const void*)&CapsuleCollider_Get_Custom_PropRadius   ,  //  <- UnityEngine.CapsuleCollider::get_radius
    (const void*)&CapsuleCollider_Set_Custom_PropRadius   ,  //  <- UnityEngine.CapsuleCollider::set_radius
    (const void*)&CapsuleCollider_Get_Custom_PropHeight   ,  //  <- UnityEngine.CapsuleCollider::get_height
    (const void*)&CapsuleCollider_Set_Custom_PropHeight   ,  //  <- UnityEngine.CapsuleCollider::set_height
    (const void*)&CapsuleCollider_Get_Custom_PropDirection,  //  <- UnityEngine.CapsuleCollider::get_direction
    (const void*)&CapsuleCollider_Set_Custom_PropDirection,  //  <- UnityEngine.CapsuleCollider::set_direction
    (const void*)&RaycastCollider_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.RaycastCollider::INTERNAL_get_center
    (const void*)&RaycastCollider_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.RaycastCollider::INTERNAL_set_center
    (const void*)&RaycastCollider_Get_Custom_PropLength   ,  //  <- UnityEngine.RaycastCollider::get_length
    (const void*)&RaycastCollider_Set_Custom_PropLength   ,  //  <- UnityEngine.RaycastCollider::set_length
    (const void*)&WheelCollider_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.WheelCollider::INTERNAL_get_center
    (const void*)&WheelCollider_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.WheelCollider::INTERNAL_set_center
    (const void*)&WheelCollider_Get_Custom_PropRadius     ,  //  <- UnityEngine.WheelCollider::get_radius
    (const void*)&WheelCollider_Set_Custom_PropRadius     ,  //  <- UnityEngine.WheelCollider::set_radius
    (const void*)&WheelCollider_Get_Custom_PropSuspensionDistance,  //  <- UnityEngine.WheelCollider::get_suspensionDistance
    (const void*)&WheelCollider_Set_Custom_PropSuspensionDistance,  //  <- UnityEngine.WheelCollider::set_suspensionDistance
    (const void*)&WheelCollider_CUSTOM_INTERNAL_get_suspensionSpring,  //  <- UnityEngine.WheelCollider::INTERNAL_get_suspensionSpring
    (const void*)&WheelCollider_CUSTOM_INTERNAL_set_suspensionSpring,  //  <- UnityEngine.WheelCollider::INTERNAL_set_suspensionSpring
    (const void*)&WheelCollider_Get_Custom_PropMass       ,  //  <- UnityEngine.WheelCollider::get_mass
    (const void*)&WheelCollider_Set_Custom_PropMass       ,  //  <- UnityEngine.WheelCollider::set_mass
    (const void*)&WheelCollider_CUSTOM_INTERNAL_get_forwardFriction,  //  <- UnityEngine.WheelCollider::INTERNAL_get_forwardFriction
    (const void*)&WheelCollider_CUSTOM_INTERNAL_set_forwardFriction,  //  <- UnityEngine.WheelCollider::INTERNAL_set_forwardFriction
    (const void*)&WheelCollider_CUSTOM_INTERNAL_get_sidewaysFriction,  //  <- UnityEngine.WheelCollider::INTERNAL_get_sidewaysFriction
    (const void*)&WheelCollider_CUSTOM_INTERNAL_set_sidewaysFriction,  //  <- UnityEngine.WheelCollider::INTERNAL_set_sidewaysFriction
    (const void*)&WheelCollider_Get_Custom_PropMotorTorque,  //  <- UnityEngine.WheelCollider::get_motorTorque
    (const void*)&WheelCollider_Set_Custom_PropMotorTorque,  //  <- UnityEngine.WheelCollider::set_motorTorque
    (const void*)&WheelCollider_Get_Custom_PropBrakeTorque,  //  <- UnityEngine.WheelCollider::get_brakeTorque
    (const void*)&WheelCollider_Set_Custom_PropBrakeTorque,  //  <- UnityEngine.WheelCollider::set_brakeTorque
    (const void*)&WheelCollider_Get_Custom_PropSteerAngle ,  //  <- UnityEngine.WheelCollider::get_steerAngle
    (const void*)&WheelCollider_Set_Custom_PropSteerAngle ,  //  <- UnityEngine.WheelCollider::set_steerAngle
    (const void*)&WheelCollider_Get_Custom_PropIsGrounded ,  //  <- UnityEngine.WheelCollider::get_isGrounded
    (const void*)&WheelCollider_CUSTOM_GetGroundHit       ,  //  <- UnityEngine.WheelCollider::GetGroundHit
    (const void*)&WheelCollider_Get_Custom_PropRpm        ,  //  <- UnityEngine.WheelCollider::get_rpm
    (const void*)&RaycastHit_CUSTOM_INTERNAL_CALL_CalculateRaycastTexCoord,  //  <- UnityEngine.RaycastHit::INTERNAL_CALL_CalculateRaycastTexCoord
    (const void*)&PhysicMaterial_CUSTOM_Internal_CreateDynamicsMaterial,  //  <- UnityEngine.PhysicMaterial::Internal_CreateDynamicsMaterial
    (const void*)&PhysicMaterial_Get_Custom_PropDynamicFriction,  //  <- UnityEngine.PhysicMaterial::get_dynamicFriction
    (const void*)&PhysicMaterial_Set_Custom_PropDynamicFriction,  //  <- UnityEngine.PhysicMaterial::set_dynamicFriction
    (const void*)&PhysicMaterial_Get_Custom_PropStaticFriction,  //  <- UnityEngine.PhysicMaterial::get_staticFriction
    (const void*)&PhysicMaterial_Set_Custom_PropStaticFriction,  //  <- UnityEngine.PhysicMaterial::set_staticFriction
    (const void*)&PhysicMaterial_Get_Custom_PropBounciness,  //  <- UnityEngine.PhysicMaterial::get_bounciness
    (const void*)&PhysicMaterial_Set_Custom_PropBounciness,  //  <- UnityEngine.PhysicMaterial::set_bounciness
    (const void*)&PhysicMaterial_CUSTOM_INTERNAL_get_frictionDirection2,  //  <- UnityEngine.PhysicMaterial::INTERNAL_get_frictionDirection2
    (const void*)&PhysicMaterial_CUSTOM_INTERNAL_set_frictionDirection2,  //  <- UnityEngine.PhysicMaterial::INTERNAL_set_frictionDirection2
    (const void*)&PhysicMaterial_Get_Custom_PropDynamicFriction2,  //  <- UnityEngine.PhysicMaterial::get_dynamicFriction2
    (const void*)&PhysicMaterial_Set_Custom_PropDynamicFriction2,  //  <- UnityEngine.PhysicMaterial::set_dynamicFriction2
    (const void*)&PhysicMaterial_Get_Custom_PropStaticFriction2,  //  <- UnityEngine.PhysicMaterial::get_staticFriction2
    (const void*)&PhysicMaterial_Set_Custom_PropStaticFriction2,  //  <- UnityEngine.PhysicMaterial::set_staticFriction2
    (const void*)&PhysicMaterial_Get_Custom_PropFrictionCombine,  //  <- UnityEngine.PhysicMaterial::get_frictionCombine
    (const void*)&PhysicMaterial_Set_Custom_PropFrictionCombine,  //  <- UnityEngine.PhysicMaterial::set_frictionCombine
    (const void*)&PhysicMaterial_Get_Custom_PropBounceCombine,  //  <- UnityEngine.PhysicMaterial::get_bounceCombine
    (const void*)&PhysicMaterial_Set_Custom_PropBounceCombine,  //  <- UnityEngine.PhysicMaterial::set_bounceCombine
    (const void*)&CharacterController_CUSTOM_INTERNAL_CALL_SimpleMove,  //  <- UnityEngine.CharacterController::INTERNAL_CALL_SimpleMove
    (const void*)&CharacterController_CUSTOM_INTERNAL_CALL_Move,  //  <- UnityEngine.CharacterController::INTERNAL_CALL_Move
    (const void*)&CharacterController_Get_Custom_PropIsGrounded,  //  <- UnityEngine.CharacterController::get_isGrounded
    (const void*)&CharacterController_CUSTOM_INTERNAL_get_velocity,  //  <- UnityEngine.CharacterController::INTERNAL_get_velocity
    (const void*)&CharacterController_Get_Custom_PropCollisionFlags,  //  <- UnityEngine.CharacterController::get_collisionFlags
    (const void*)&CharacterController_Get_Custom_PropRadius,  //  <- UnityEngine.CharacterController::get_radius
    (const void*)&CharacterController_Set_Custom_PropRadius,  //  <- UnityEngine.CharacterController::set_radius
    (const void*)&CharacterController_Get_Custom_PropHeight,  //  <- UnityEngine.CharacterController::get_height
    (const void*)&CharacterController_Set_Custom_PropHeight,  //  <- UnityEngine.CharacterController::set_height
    (const void*)&CharacterController_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.CharacterController::INTERNAL_get_center
    (const void*)&CharacterController_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.CharacterController::INTERNAL_set_center
    (const void*)&CharacterController_Get_Custom_PropSlopeLimit,  //  <- UnityEngine.CharacterController::get_slopeLimit
    (const void*)&CharacterController_Set_Custom_PropSlopeLimit,  //  <- UnityEngine.CharacterController::set_slopeLimit
    (const void*)&CharacterController_Get_Custom_PropStepOffset,  //  <- UnityEngine.CharacterController::get_stepOffset
    (const void*)&CharacterController_Set_Custom_PropStepOffset,  //  <- UnityEngine.CharacterController::set_stepOffset
    (const void*)&CharacterController_Get_Custom_PropDetectCollisions,  //  <- UnityEngine.CharacterController::get_detectCollisions
    (const void*)&CharacterController_Set_Custom_PropDetectCollisions,  //  <- UnityEngine.CharacterController::set_detectCollisions
#endif
#if ENABLE_CLOTH
    (const void*)&Cloth_Get_Custom_PropBendingStiffness   ,  //  <- UnityEngine.Cloth::get_bendingStiffness
    (const void*)&Cloth_Set_Custom_PropBendingStiffness   ,  //  <- UnityEngine.Cloth::set_bendingStiffness
    (const void*)&Cloth_Get_Custom_PropStretchingStiffness,  //  <- UnityEngine.Cloth::get_stretchingStiffness
    (const void*)&Cloth_Set_Custom_PropStretchingStiffness,  //  <- UnityEngine.Cloth::set_stretchingStiffness
    (const void*)&Cloth_Get_Custom_PropDamping            ,  //  <- UnityEngine.Cloth::get_damping
    (const void*)&Cloth_Set_Custom_PropDamping            ,  //  <- UnityEngine.Cloth::set_damping
    (const void*)&Cloth_Get_Custom_PropThickness          ,  //  <- UnityEngine.Cloth::get_thickness
    (const void*)&Cloth_Set_Custom_PropThickness          ,  //  <- UnityEngine.Cloth::set_thickness
    (const void*)&Cloth_CUSTOM_INTERNAL_get_externalAcceleration,  //  <- UnityEngine.Cloth::INTERNAL_get_externalAcceleration
    (const void*)&Cloth_CUSTOM_INTERNAL_set_externalAcceleration,  //  <- UnityEngine.Cloth::INTERNAL_set_externalAcceleration
    (const void*)&Cloth_CUSTOM_INTERNAL_get_randomAcceleration,  //  <- UnityEngine.Cloth::INTERNAL_get_randomAcceleration
    (const void*)&Cloth_CUSTOM_INTERNAL_set_randomAcceleration,  //  <- UnityEngine.Cloth::INTERNAL_set_randomAcceleration
    (const void*)&Cloth_Get_Custom_PropUseGravity         ,  //  <- UnityEngine.Cloth::get_useGravity
    (const void*)&Cloth_Set_Custom_PropUseGravity         ,  //  <- UnityEngine.Cloth::set_useGravity
    (const void*)&Cloth_Get_Custom_PropSelfCollision      ,  //  <- UnityEngine.Cloth::get_selfCollision
    (const void*)&Cloth_Set_Custom_PropSelfCollision      ,  //  <- UnityEngine.Cloth::set_selfCollision
    (const void*)&Cloth_Get_Custom_PropEnabled            ,  //  <- UnityEngine.Cloth::get_enabled
    (const void*)&Cloth_Set_Custom_PropEnabled            ,  //  <- UnityEngine.Cloth::set_enabled
    (const void*)&Cloth_Get_Custom_PropVertices           ,  //  <- UnityEngine.Cloth::get_vertices
    (const void*)&Cloth_Get_Custom_PropNormals            ,  //  <- UnityEngine.Cloth::get_normals
    (const void*)&InteractiveCloth_Get_Custom_PropMesh    ,  //  <- UnityEngine.InteractiveCloth::get_mesh
    (const void*)&InteractiveCloth_Set_Custom_PropMesh    ,  //  <- UnityEngine.InteractiveCloth::set_mesh
    (const void*)&InteractiveCloth_Get_Custom_PropFriction,  //  <- UnityEngine.InteractiveCloth::get_friction
    (const void*)&InteractiveCloth_Set_Custom_PropFriction,  //  <- UnityEngine.InteractiveCloth::set_friction
    (const void*)&InteractiveCloth_Get_Custom_PropDensity ,  //  <- UnityEngine.InteractiveCloth::get_density
    (const void*)&InteractiveCloth_Set_Custom_PropDensity ,  //  <- UnityEngine.InteractiveCloth::set_density
    (const void*)&InteractiveCloth_Get_Custom_PropPressure,  //  <- UnityEngine.InteractiveCloth::get_pressure
    (const void*)&InteractiveCloth_Set_Custom_PropPressure,  //  <- UnityEngine.InteractiveCloth::set_pressure
    (const void*)&InteractiveCloth_Get_Custom_PropCollisionResponse,  //  <- UnityEngine.InteractiveCloth::get_collisionResponse
    (const void*)&InteractiveCloth_Set_Custom_PropCollisionResponse,  //  <- UnityEngine.InteractiveCloth::set_collisionResponse
    (const void*)&InteractiveCloth_Get_Custom_PropTearFactor,  //  <- UnityEngine.InteractiveCloth::get_tearFactor
    (const void*)&InteractiveCloth_Set_Custom_PropTearFactor,  //  <- UnityEngine.InteractiveCloth::set_tearFactor
    (const void*)&InteractiveCloth_Get_Custom_PropAttachmentTearFactor,  //  <- UnityEngine.InteractiveCloth::get_attachmentTearFactor
    (const void*)&InteractiveCloth_Set_Custom_PropAttachmentTearFactor,  //  <- UnityEngine.InteractiveCloth::set_attachmentTearFactor
    (const void*)&InteractiveCloth_Get_Custom_PropAttachmentResponse,  //  <- UnityEngine.InteractiveCloth::get_attachmentResponse
    (const void*)&InteractiveCloth_Set_Custom_PropAttachmentResponse,  //  <- UnityEngine.InteractiveCloth::set_attachmentResponse
    (const void*)&InteractiveCloth_Get_Custom_PropIsTeared,  //  <- UnityEngine.InteractiveCloth::get_isTeared
    (const void*)&InteractiveCloth_CUSTOM_INTERNAL_CALL_AddForceAtPosition,  //  <- UnityEngine.InteractiveCloth::INTERNAL_CALL_AddForceAtPosition
    (const void*)&InteractiveCloth_CUSTOM_AttachToCollider,  //  <- UnityEngine.InteractiveCloth::AttachToCollider
    (const void*)&InteractiveCloth_CUSTOM_DetachFromCollider,  //  <- UnityEngine.InteractiveCloth::DetachFromCollider
    (const void*)&SkinnedCloth_Get_Custom_PropCoefficients,  //  <- UnityEngine.SkinnedCloth::get_coefficients
    (const void*)&SkinnedCloth_Set_Custom_PropCoefficients,  //  <- UnityEngine.SkinnedCloth::set_coefficients
    (const void*)&SkinnedCloth_Get_Custom_PropWorldVelocityScale,  //  <- UnityEngine.SkinnedCloth::get_worldVelocityScale
    (const void*)&SkinnedCloth_Set_Custom_PropWorldVelocityScale,  //  <- UnityEngine.SkinnedCloth::set_worldVelocityScale
    (const void*)&SkinnedCloth_Get_Custom_PropWorldAccelerationScale,  //  <- UnityEngine.SkinnedCloth::get_worldAccelerationScale
    (const void*)&SkinnedCloth_Set_Custom_PropWorldAccelerationScale,  //  <- UnityEngine.SkinnedCloth::set_worldAccelerationScale
    (const void*)&SkinnedCloth_CUSTOM_SetEnabledFading    ,  //  <- UnityEngine.SkinnedCloth::SetEnabledFading
    (const void*)&ClothRenderer_Get_Custom_PropPauseWhenNotVisible,  //  <- UnityEngine.ClothRenderer::get_pauseWhenNotVisible
    (const void*)&ClothRenderer_Set_Custom_PropPauseWhenNotVisible,  //  <- UnityEngine.ClothRenderer::set_pauseWhenNotVisible
#endif
    NULL
};

void ExportNewDynamicsBindings();
void ExportNewDynamicsBindings()
{
    for (int i = 0; s_NewDynamics_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_NewDynamics_IcallNames [i], s_NewDynamics_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportNewDynamicsBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_PHYSICS
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_get_gravity); //  <- UnityEngine.Physics::INTERNAL_get_gravity
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_set_gravity); //  <- UnityEngine.Physics::INTERNAL_set_gravity
    SET_METRO_BINDING(Physics_Get_Custom_PropMinPenetrationForPenalty); //  <- UnityEngine.Physics::get_minPenetrationForPenalty
    SET_METRO_BINDING(Physics_Set_Custom_PropMinPenetrationForPenalty); //  <- UnityEngine.Physics::set_minPenetrationForPenalty
    SET_METRO_BINDING(Physics_Get_Custom_PropBounceThreshold); //  <- UnityEngine.Physics::get_bounceThreshold
    SET_METRO_BINDING(Physics_Set_Custom_PropBounceThreshold); //  <- UnityEngine.Physics::set_bounceThreshold
    SET_METRO_BINDING(Physics_Get_Custom_PropSleepVelocity); //  <- UnityEngine.Physics::get_sleepVelocity
    SET_METRO_BINDING(Physics_Set_Custom_PropSleepVelocity); //  <- UnityEngine.Physics::set_sleepVelocity
    SET_METRO_BINDING(Physics_Get_Custom_PropSleepAngularVelocity); //  <- UnityEngine.Physics::get_sleepAngularVelocity
    SET_METRO_BINDING(Physics_Set_Custom_PropSleepAngularVelocity); //  <- UnityEngine.Physics::set_sleepAngularVelocity
    SET_METRO_BINDING(Physics_Get_Custom_PropMaxAngularVelocity); //  <- UnityEngine.Physics::get_maxAngularVelocity
    SET_METRO_BINDING(Physics_Set_Custom_PropMaxAngularVelocity); //  <- UnityEngine.Physics::set_maxAngularVelocity
    SET_METRO_BINDING(Physics_Get_Custom_PropSolverIterationCount); //  <- UnityEngine.Physics::get_solverIterationCount
    SET_METRO_BINDING(Physics_Set_Custom_PropSolverIterationCount); //  <- UnityEngine.Physics::set_solverIterationCount
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_Internal_Raycast); //  <- UnityEngine.Physics::INTERNAL_CALL_Internal_Raycast
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_Internal_CapsuleCast); //  <- UnityEngine.Physics::INTERNAL_CALL_Internal_CapsuleCast
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_Internal_RaycastTest); //  <- UnityEngine.Physics::INTERNAL_CALL_Internal_RaycastTest
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_RaycastAll); //  <- UnityEngine.Physics::INTERNAL_CALL_RaycastAll
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_OverlapSphere); //  <- UnityEngine.Physics::INTERNAL_CALL_OverlapSphere
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_CapsuleCastAll); //  <- UnityEngine.Physics::INTERNAL_CALL_CapsuleCastAll
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_CheckSphere); //  <- UnityEngine.Physics::INTERNAL_CALL_CheckSphere
    SET_METRO_BINDING(Physics_CUSTOM_INTERNAL_CALL_CheckCapsule); //  <- UnityEngine.Physics::INTERNAL_CALL_CheckCapsule
    SET_METRO_BINDING(Physics_Get_Custom_PropPenetrationPenaltyForce); //  <- UnityEngine.Physics::get_penetrationPenaltyForce
    SET_METRO_BINDING(Physics_Set_Custom_PropPenetrationPenaltyForce); //  <- UnityEngine.Physics::set_penetrationPenaltyForce
    SET_METRO_BINDING(Physics_CUSTOM_IgnoreCollision); //  <- UnityEngine.Physics::IgnoreCollision
    SET_METRO_BINDING(Physics_CUSTOM_IgnoreLayerCollision); //  <- UnityEngine.Physics::IgnoreLayerCollision
    SET_METRO_BINDING(Physics_CUSTOM_GetIgnoreLayerCollision); //  <- UnityEngine.Physics::GetIgnoreLayerCollision
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_velocity); //  <- UnityEngine.Rigidbody::INTERNAL_get_velocity
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_velocity); //  <- UnityEngine.Rigidbody::INTERNAL_set_velocity
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_angularVelocity); //  <- UnityEngine.Rigidbody::INTERNAL_get_angularVelocity
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_angularVelocity); //  <- UnityEngine.Rigidbody::INTERNAL_set_angularVelocity
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropDrag); //  <- UnityEngine.Rigidbody::get_drag
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropDrag); //  <- UnityEngine.Rigidbody::set_drag
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropAngularDrag); //  <- UnityEngine.Rigidbody::get_angularDrag
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropAngularDrag); //  <- UnityEngine.Rigidbody::set_angularDrag
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropMass); //  <- UnityEngine.Rigidbody::get_mass
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropMass); //  <- UnityEngine.Rigidbody::set_mass
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_SetDensity); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_SetDensity
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropUseGravity); //  <- UnityEngine.Rigidbody::get_useGravity
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropUseGravity); //  <- UnityEngine.Rigidbody::set_useGravity
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropIsKinematic); //  <- UnityEngine.Rigidbody::get_isKinematic
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropIsKinematic); //  <- UnityEngine.Rigidbody::set_isKinematic
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropFreezeRotation); //  <- UnityEngine.Rigidbody::get_freezeRotation
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropFreezeRotation); //  <- UnityEngine.Rigidbody::set_freezeRotation
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropConstraints); //  <- UnityEngine.Rigidbody::get_constraints
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropConstraints); //  <- UnityEngine.Rigidbody::set_constraints
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropCollisionDetectionMode); //  <- UnityEngine.Rigidbody::get_collisionDetectionMode
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropCollisionDetectionMode); //  <- UnityEngine.Rigidbody::set_collisionDetectionMode
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_AddForce); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddForce
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeForce); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeForce
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_AddTorque); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddTorque
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_AddRelativeTorque); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddRelativeTorque
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_AddForceAtPosition); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddForceAtPosition
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_AddExplosionForce); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_AddExplosionForce
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_ClosestPointOnBounds
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_GetRelativePointVelocity); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_GetRelativePointVelocity
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_GetPointVelocity); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_GetPointVelocity
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_centerOfMass); //  <- UnityEngine.Rigidbody::INTERNAL_get_centerOfMass
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_centerOfMass); //  <- UnityEngine.Rigidbody::INTERNAL_set_centerOfMass
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_worldCenterOfMass); //  <- UnityEngine.Rigidbody::INTERNAL_get_worldCenterOfMass
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_inertiaTensorRotation); //  <- UnityEngine.Rigidbody::INTERNAL_get_inertiaTensorRotation
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_inertiaTensorRotation); //  <- UnityEngine.Rigidbody::INTERNAL_set_inertiaTensorRotation
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_inertiaTensor); //  <- UnityEngine.Rigidbody::INTERNAL_get_inertiaTensor
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_inertiaTensor); //  <- UnityEngine.Rigidbody::INTERNAL_set_inertiaTensor
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropDetectCollisions); //  <- UnityEngine.Rigidbody::get_detectCollisions
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropDetectCollisions); //  <- UnityEngine.Rigidbody::set_detectCollisions
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropUseConeFriction); //  <- UnityEngine.Rigidbody::get_useConeFriction
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropUseConeFriction); //  <- UnityEngine.Rigidbody::set_useConeFriction
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_position); //  <- UnityEngine.Rigidbody::INTERNAL_get_position
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_position); //  <- UnityEngine.Rigidbody::INTERNAL_set_position
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_get_rotation); //  <- UnityEngine.Rigidbody::INTERNAL_get_rotation
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_set_rotation); //  <- UnityEngine.Rigidbody::INTERNAL_set_rotation
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_MovePosition); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_MovePosition
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_MoveRotation); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_MoveRotation
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropInterpolation); //  <- UnityEngine.Rigidbody::get_interpolation
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropInterpolation); //  <- UnityEngine.Rigidbody::set_interpolation
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_Sleep); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_Sleep
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_IsSleeping); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_IsSleeping
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_WakeUp); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_WakeUp
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropSolverIterationCount); //  <- UnityEngine.Rigidbody::get_solverIterationCount
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropSolverIterationCount); //  <- UnityEngine.Rigidbody::set_solverIterationCount
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropSleepVelocity); //  <- UnityEngine.Rigidbody::get_sleepVelocity
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropSleepVelocity); //  <- UnityEngine.Rigidbody::set_sleepVelocity
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropSleepAngularVelocity); //  <- UnityEngine.Rigidbody::get_sleepAngularVelocity
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropSleepAngularVelocity); //  <- UnityEngine.Rigidbody::set_sleepAngularVelocity
    SET_METRO_BINDING(Rigidbody_Get_Custom_PropMaxAngularVelocity); //  <- UnityEngine.Rigidbody::get_maxAngularVelocity
    SET_METRO_BINDING(Rigidbody_Set_Custom_PropMaxAngularVelocity); //  <- UnityEngine.Rigidbody::set_maxAngularVelocity
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_SweepTest); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_SweepTest
    SET_METRO_BINDING(Rigidbody_CUSTOM_INTERNAL_CALL_SweepTestAll); //  <- UnityEngine.Rigidbody::INTERNAL_CALL_SweepTestAll
    SET_METRO_BINDING(Joint_Get_Custom_PropConnectedBody); //  <- UnityEngine.Joint::get_connectedBody
    SET_METRO_BINDING(Joint_Set_Custom_PropConnectedBody); //  <- UnityEngine.Joint::set_connectedBody
    SET_METRO_BINDING(Joint_CUSTOM_INTERNAL_get_axis); //  <- UnityEngine.Joint::INTERNAL_get_axis
    SET_METRO_BINDING(Joint_CUSTOM_INTERNAL_set_axis); //  <- UnityEngine.Joint::INTERNAL_set_axis
    SET_METRO_BINDING(Joint_CUSTOM_INTERNAL_get_anchor); //  <- UnityEngine.Joint::INTERNAL_get_anchor
    SET_METRO_BINDING(Joint_CUSTOM_INTERNAL_set_anchor); //  <- UnityEngine.Joint::INTERNAL_set_anchor
    SET_METRO_BINDING(Joint_Get_Custom_PropBreakForce); //  <- UnityEngine.Joint::get_breakForce
    SET_METRO_BINDING(Joint_Set_Custom_PropBreakForce); //  <- UnityEngine.Joint::set_breakForce
    SET_METRO_BINDING(Joint_Get_Custom_PropBreakTorque); //  <- UnityEngine.Joint::get_breakTorque
    SET_METRO_BINDING(Joint_Set_Custom_PropBreakTorque); //  <- UnityEngine.Joint::set_breakTorque
    SET_METRO_BINDING(HingeJoint_CUSTOM_INTERNAL_get_motor); //  <- UnityEngine.HingeJoint::INTERNAL_get_motor
    SET_METRO_BINDING(HingeJoint_CUSTOM_INTERNAL_set_motor); //  <- UnityEngine.HingeJoint::INTERNAL_set_motor
    SET_METRO_BINDING(HingeJoint_CUSTOM_INTERNAL_get_limits); //  <- UnityEngine.HingeJoint::INTERNAL_get_limits
    SET_METRO_BINDING(HingeJoint_CUSTOM_INTERNAL_set_limits); //  <- UnityEngine.HingeJoint::INTERNAL_set_limits
    SET_METRO_BINDING(HingeJoint_CUSTOM_INTERNAL_get_spring); //  <- UnityEngine.HingeJoint::INTERNAL_get_spring
    SET_METRO_BINDING(HingeJoint_CUSTOM_INTERNAL_set_spring); //  <- UnityEngine.HingeJoint::INTERNAL_set_spring
    SET_METRO_BINDING(HingeJoint_Get_Custom_PropUseMotor); //  <- UnityEngine.HingeJoint::get_useMotor
    SET_METRO_BINDING(HingeJoint_Set_Custom_PropUseMotor); //  <- UnityEngine.HingeJoint::set_useMotor
    SET_METRO_BINDING(HingeJoint_Get_Custom_PropUseLimits); //  <- UnityEngine.HingeJoint::get_useLimits
    SET_METRO_BINDING(HingeJoint_Set_Custom_PropUseLimits); //  <- UnityEngine.HingeJoint::set_useLimits
    SET_METRO_BINDING(HingeJoint_Get_Custom_PropUseSpring); //  <- UnityEngine.HingeJoint::get_useSpring
    SET_METRO_BINDING(HingeJoint_Set_Custom_PropUseSpring); //  <- UnityEngine.HingeJoint::set_useSpring
    SET_METRO_BINDING(HingeJoint_Get_Custom_PropVelocity); //  <- UnityEngine.HingeJoint::get_velocity
    SET_METRO_BINDING(HingeJoint_Get_Custom_PropAngle); //  <- UnityEngine.HingeJoint::get_angle
    SET_METRO_BINDING(SpringJoint_Get_Custom_PropSpring); //  <- UnityEngine.SpringJoint::get_spring
    SET_METRO_BINDING(SpringJoint_Set_Custom_PropSpring); //  <- UnityEngine.SpringJoint::set_spring
    SET_METRO_BINDING(SpringJoint_Get_Custom_PropDamper); //  <- UnityEngine.SpringJoint::get_damper
    SET_METRO_BINDING(SpringJoint_Set_Custom_PropDamper); //  <- UnityEngine.SpringJoint::set_damper
    SET_METRO_BINDING(SpringJoint_Get_Custom_PropMinDistance); //  <- UnityEngine.SpringJoint::get_minDistance
    SET_METRO_BINDING(SpringJoint_Set_Custom_PropMinDistance); //  <- UnityEngine.SpringJoint::set_minDistance
    SET_METRO_BINDING(SpringJoint_Get_Custom_PropMaxDistance); //  <- UnityEngine.SpringJoint::get_maxDistance
    SET_METRO_BINDING(SpringJoint_Set_Custom_PropMaxDistance); //  <- UnityEngine.SpringJoint::set_maxDistance
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_swingAxis); //  <- UnityEngine.CharacterJoint::INTERNAL_get_swingAxis
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_swingAxis); //  <- UnityEngine.CharacterJoint::INTERNAL_set_swingAxis
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_lowTwistLimit); //  <- UnityEngine.CharacterJoint::INTERNAL_get_lowTwistLimit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_lowTwistLimit); //  <- UnityEngine.CharacterJoint::INTERNAL_set_lowTwistLimit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_highTwistLimit); //  <- UnityEngine.CharacterJoint::INTERNAL_get_highTwistLimit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_highTwistLimit); //  <- UnityEngine.CharacterJoint::INTERNAL_set_highTwistLimit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_swing1Limit); //  <- UnityEngine.CharacterJoint::INTERNAL_get_swing1Limit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_swing1Limit); //  <- UnityEngine.CharacterJoint::INTERNAL_set_swing1Limit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_swing2Limit); //  <- UnityEngine.CharacterJoint::INTERNAL_get_swing2Limit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_swing2Limit); //  <- UnityEngine.CharacterJoint::INTERNAL_set_swing2Limit
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_targetRotation); //  <- UnityEngine.CharacterJoint::INTERNAL_get_targetRotation
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_targetRotation); //  <- UnityEngine.CharacterJoint::INTERNAL_set_targetRotation
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_targetAngularVelocity); //  <- UnityEngine.CharacterJoint::INTERNAL_get_targetAngularVelocity
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_targetAngularVelocity); //  <- UnityEngine.CharacterJoint::INTERNAL_set_targetAngularVelocity
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_get_rotationDrive); //  <- UnityEngine.CharacterJoint::INTERNAL_get_rotationDrive
    SET_METRO_BINDING(CharacterJoint_CUSTOM_INTERNAL_set_rotationDrive); //  <- UnityEngine.CharacterJoint::INTERNAL_set_rotationDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_secondaryAxis); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_secondaryAxis
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_secondaryAxis); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_secondaryAxis
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropXMotion); //  <- UnityEngine.ConfigurableJoint::get_xMotion
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropXMotion); //  <- UnityEngine.ConfigurableJoint::set_xMotion
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropYMotion); //  <- UnityEngine.ConfigurableJoint::get_yMotion
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropYMotion); //  <- UnityEngine.ConfigurableJoint::set_yMotion
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropZMotion); //  <- UnityEngine.ConfigurableJoint::get_zMotion
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropZMotion); //  <- UnityEngine.ConfigurableJoint::set_zMotion
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropAngularXMotion); //  <- UnityEngine.ConfigurableJoint::get_angularXMotion
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropAngularXMotion); //  <- UnityEngine.ConfigurableJoint::set_angularXMotion
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropAngularYMotion); //  <- UnityEngine.ConfigurableJoint::get_angularYMotion
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropAngularYMotion); //  <- UnityEngine.ConfigurableJoint::set_angularYMotion
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropAngularZMotion); //  <- UnityEngine.ConfigurableJoint::get_angularZMotion
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropAngularZMotion); //  <- UnityEngine.ConfigurableJoint::set_angularZMotion
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_linearLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_linearLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_linearLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_linearLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_lowAngularXLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_lowAngularXLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_lowAngularXLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_lowAngularXLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_highAngularXLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_highAngularXLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_highAngularXLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_highAngularXLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_angularYLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularYLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_angularYLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularYLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_angularZLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularZLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_angularZLimit); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularZLimit
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_targetPosition); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetPosition
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_targetPosition); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetPosition
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_targetVelocity); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetVelocity
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_targetVelocity); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetVelocity
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_xDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_xDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_xDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_xDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_yDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_yDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_yDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_yDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_zDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_zDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_zDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_zDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_targetRotation); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetRotation
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_targetRotation); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetRotation
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_targetAngularVelocity); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_targetAngularVelocity
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_targetAngularVelocity); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_targetAngularVelocity
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropRotationDriveMode); //  <- UnityEngine.ConfigurableJoint::get_rotationDriveMode
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropRotationDriveMode); //  <- UnityEngine.ConfigurableJoint::set_rotationDriveMode
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_angularXDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularXDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_angularXDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularXDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_angularYZDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_angularYZDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_angularYZDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_angularYZDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_get_slerpDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_get_slerpDrive
    SET_METRO_BINDING(ConfigurableJoint_CUSTOM_INTERNAL_set_slerpDrive); //  <- UnityEngine.ConfigurableJoint::INTERNAL_set_slerpDrive
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropProjectionMode); //  <- UnityEngine.ConfigurableJoint::get_projectionMode
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropProjectionMode); //  <- UnityEngine.ConfigurableJoint::set_projectionMode
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropProjectionDistance); //  <- UnityEngine.ConfigurableJoint::get_projectionDistance
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropProjectionDistance); //  <- UnityEngine.ConfigurableJoint::set_projectionDistance
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropProjectionAngle); //  <- UnityEngine.ConfigurableJoint::get_projectionAngle
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropProjectionAngle); //  <- UnityEngine.ConfigurableJoint::set_projectionAngle
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropConfiguredInWorldSpace); //  <- UnityEngine.ConfigurableJoint::get_configuredInWorldSpace
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropConfiguredInWorldSpace); //  <- UnityEngine.ConfigurableJoint::set_configuredInWorldSpace
    SET_METRO_BINDING(ConfigurableJoint_Get_Custom_PropSwapBodies); //  <- UnityEngine.ConfigurableJoint::get_swapBodies
    SET_METRO_BINDING(ConfigurableJoint_Set_Custom_PropSwapBodies); //  <- UnityEngine.ConfigurableJoint::set_swapBodies
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_get_force); //  <- UnityEngine.ConstantForce::INTERNAL_get_force
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_set_force); //  <- UnityEngine.ConstantForce::INTERNAL_set_force
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_get_relativeForce); //  <- UnityEngine.ConstantForce::INTERNAL_get_relativeForce
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_set_relativeForce); //  <- UnityEngine.ConstantForce::INTERNAL_set_relativeForce
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_get_torque); //  <- UnityEngine.ConstantForce::INTERNAL_get_torque
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_set_torque); //  <- UnityEngine.ConstantForce::INTERNAL_set_torque
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_get_relativeTorque); //  <- UnityEngine.ConstantForce::INTERNAL_get_relativeTorque
    SET_METRO_BINDING(ConstantForce_CUSTOM_INTERNAL_set_relativeTorque); //  <- UnityEngine.ConstantForce::INTERNAL_set_relativeTorque
    SET_METRO_BINDING(Collider_Get_Custom_PropEnabled); //  <- UnityEngine.Collider::get_enabled
    SET_METRO_BINDING(Collider_Set_Custom_PropEnabled); //  <- UnityEngine.Collider::set_enabled
    SET_METRO_BINDING(Collider_Get_Custom_PropAttachedRigidbody); //  <- UnityEngine.Collider::get_attachedRigidbody
    SET_METRO_BINDING(Collider_Get_Custom_PropIsTrigger); //  <- UnityEngine.Collider::get_isTrigger
    SET_METRO_BINDING(Collider_Set_Custom_PropIsTrigger); //  <- UnityEngine.Collider::set_isTrigger
    SET_METRO_BINDING(Collider_Get_Custom_PropMaterial); //  <- UnityEngine.Collider::get_material
    SET_METRO_BINDING(Collider_Set_Custom_PropMaterial); //  <- UnityEngine.Collider::set_material
    SET_METRO_BINDING(Collider_CUSTOM_INTERNAL_CALL_ClosestPointOnBounds); //  <- UnityEngine.Collider::INTERNAL_CALL_ClosestPointOnBounds
    SET_METRO_BINDING(Collider_Get_Custom_PropSharedMaterial); //  <- UnityEngine.Collider::get_sharedMaterial
    SET_METRO_BINDING(Collider_Set_Custom_PropSharedMaterial); //  <- UnityEngine.Collider::set_sharedMaterial
    SET_METRO_BINDING(Collider_CUSTOM_INTERNAL_get_bounds); //  <- UnityEngine.Collider::INTERNAL_get_bounds
    SET_METRO_BINDING(Collider_CUSTOM_INTERNAL_CALL_Internal_Raycast); //  <- UnityEngine.Collider::INTERNAL_CALL_Internal_Raycast
    SET_METRO_BINDING(BoxCollider_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.BoxCollider::INTERNAL_get_center
    SET_METRO_BINDING(BoxCollider_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.BoxCollider::INTERNAL_set_center
    SET_METRO_BINDING(BoxCollider_CUSTOM_INTERNAL_get_size); //  <- UnityEngine.BoxCollider::INTERNAL_get_size
    SET_METRO_BINDING(BoxCollider_CUSTOM_INTERNAL_set_size); //  <- UnityEngine.BoxCollider::INTERNAL_set_size
    SET_METRO_BINDING(SphereCollider_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.SphereCollider::INTERNAL_get_center
    SET_METRO_BINDING(SphereCollider_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.SphereCollider::INTERNAL_set_center
    SET_METRO_BINDING(SphereCollider_Get_Custom_PropRadius); //  <- UnityEngine.SphereCollider::get_radius
    SET_METRO_BINDING(SphereCollider_Set_Custom_PropRadius); //  <- UnityEngine.SphereCollider::set_radius
    SET_METRO_BINDING(MeshCollider_Get_Custom_PropSharedMesh); //  <- UnityEngine.MeshCollider::get_sharedMesh
    SET_METRO_BINDING(MeshCollider_Set_Custom_PropSharedMesh); //  <- UnityEngine.MeshCollider::set_sharedMesh
    SET_METRO_BINDING(MeshCollider_Get_Custom_PropConvex); //  <- UnityEngine.MeshCollider::get_convex
    SET_METRO_BINDING(MeshCollider_Set_Custom_PropConvex); //  <- UnityEngine.MeshCollider::set_convex
    SET_METRO_BINDING(MeshCollider_Get_Custom_PropSmoothSphereCollisions); //  <- UnityEngine.MeshCollider::get_smoothSphereCollisions
    SET_METRO_BINDING(MeshCollider_Set_Custom_PropSmoothSphereCollisions); //  <- UnityEngine.MeshCollider::set_smoothSphereCollisions
    SET_METRO_BINDING(CapsuleCollider_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.CapsuleCollider::INTERNAL_get_center
    SET_METRO_BINDING(CapsuleCollider_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.CapsuleCollider::INTERNAL_set_center
    SET_METRO_BINDING(CapsuleCollider_Get_Custom_PropRadius); //  <- UnityEngine.CapsuleCollider::get_radius
    SET_METRO_BINDING(CapsuleCollider_Set_Custom_PropRadius); //  <- UnityEngine.CapsuleCollider::set_radius
    SET_METRO_BINDING(CapsuleCollider_Get_Custom_PropHeight); //  <- UnityEngine.CapsuleCollider::get_height
    SET_METRO_BINDING(CapsuleCollider_Set_Custom_PropHeight); //  <- UnityEngine.CapsuleCollider::set_height
    SET_METRO_BINDING(CapsuleCollider_Get_Custom_PropDirection); //  <- UnityEngine.CapsuleCollider::get_direction
    SET_METRO_BINDING(CapsuleCollider_Set_Custom_PropDirection); //  <- UnityEngine.CapsuleCollider::set_direction
    SET_METRO_BINDING(RaycastCollider_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.RaycastCollider::INTERNAL_get_center
    SET_METRO_BINDING(RaycastCollider_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.RaycastCollider::INTERNAL_set_center
    SET_METRO_BINDING(RaycastCollider_Get_Custom_PropLength); //  <- UnityEngine.RaycastCollider::get_length
    SET_METRO_BINDING(RaycastCollider_Set_Custom_PropLength); //  <- UnityEngine.RaycastCollider::set_length
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.WheelCollider::INTERNAL_get_center
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.WheelCollider::INTERNAL_set_center
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropRadius); //  <- UnityEngine.WheelCollider::get_radius
    SET_METRO_BINDING(WheelCollider_Set_Custom_PropRadius); //  <- UnityEngine.WheelCollider::set_radius
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropSuspensionDistance); //  <- UnityEngine.WheelCollider::get_suspensionDistance
    SET_METRO_BINDING(WheelCollider_Set_Custom_PropSuspensionDistance); //  <- UnityEngine.WheelCollider::set_suspensionDistance
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_get_suspensionSpring); //  <- UnityEngine.WheelCollider::INTERNAL_get_suspensionSpring
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_set_suspensionSpring); //  <- UnityEngine.WheelCollider::INTERNAL_set_suspensionSpring
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropMass); //  <- UnityEngine.WheelCollider::get_mass
    SET_METRO_BINDING(WheelCollider_Set_Custom_PropMass); //  <- UnityEngine.WheelCollider::set_mass
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_get_forwardFriction); //  <- UnityEngine.WheelCollider::INTERNAL_get_forwardFriction
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_set_forwardFriction); //  <- UnityEngine.WheelCollider::INTERNAL_set_forwardFriction
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_get_sidewaysFriction); //  <- UnityEngine.WheelCollider::INTERNAL_get_sidewaysFriction
    SET_METRO_BINDING(WheelCollider_CUSTOM_INTERNAL_set_sidewaysFriction); //  <- UnityEngine.WheelCollider::INTERNAL_set_sidewaysFriction
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropMotorTorque); //  <- UnityEngine.WheelCollider::get_motorTorque
    SET_METRO_BINDING(WheelCollider_Set_Custom_PropMotorTorque); //  <- UnityEngine.WheelCollider::set_motorTorque
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropBrakeTorque); //  <- UnityEngine.WheelCollider::get_brakeTorque
    SET_METRO_BINDING(WheelCollider_Set_Custom_PropBrakeTorque); //  <- UnityEngine.WheelCollider::set_brakeTorque
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropSteerAngle); //  <- UnityEngine.WheelCollider::get_steerAngle
    SET_METRO_BINDING(WheelCollider_Set_Custom_PropSteerAngle); //  <- UnityEngine.WheelCollider::set_steerAngle
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropIsGrounded); //  <- UnityEngine.WheelCollider::get_isGrounded
    SET_METRO_BINDING(WheelCollider_CUSTOM_GetGroundHit); //  <- UnityEngine.WheelCollider::GetGroundHit
    SET_METRO_BINDING(WheelCollider_Get_Custom_PropRpm); //  <- UnityEngine.WheelCollider::get_rpm
    SET_METRO_BINDING(RaycastHit_CUSTOM_INTERNAL_CALL_CalculateRaycastTexCoord); //  <- UnityEngine.RaycastHit::INTERNAL_CALL_CalculateRaycastTexCoord
    SET_METRO_BINDING(PhysicMaterial_CUSTOM_Internal_CreateDynamicsMaterial); //  <- UnityEngine.PhysicMaterial::Internal_CreateDynamicsMaterial
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropDynamicFriction); //  <- UnityEngine.PhysicMaterial::get_dynamicFriction
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropDynamicFriction); //  <- UnityEngine.PhysicMaterial::set_dynamicFriction
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropStaticFriction); //  <- UnityEngine.PhysicMaterial::get_staticFriction
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropStaticFriction); //  <- UnityEngine.PhysicMaterial::set_staticFriction
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropBounciness); //  <- UnityEngine.PhysicMaterial::get_bounciness
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropBounciness); //  <- UnityEngine.PhysicMaterial::set_bounciness
    SET_METRO_BINDING(PhysicMaterial_CUSTOM_INTERNAL_get_frictionDirection2); //  <- UnityEngine.PhysicMaterial::INTERNAL_get_frictionDirection2
    SET_METRO_BINDING(PhysicMaterial_CUSTOM_INTERNAL_set_frictionDirection2); //  <- UnityEngine.PhysicMaterial::INTERNAL_set_frictionDirection2
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropDynamicFriction2); //  <- UnityEngine.PhysicMaterial::get_dynamicFriction2
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropDynamicFriction2); //  <- UnityEngine.PhysicMaterial::set_dynamicFriction2
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropStaticFriction2); //  <- UnityEngine.PhysicMaterial::get_staticFriction2
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropStaticFriction2); //  <- UnityEngine.PhysicMaterial::set_staticFriction2
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropFrictionCombine); //  <- UnityEngine.PhysicMaterial::get_frictionCombine
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropFrictionCombine); //  <- UnityEngine.PhysicMaterial::set_frictionCombine
    SET_METRO_BINDING(PhysicMaterial_Get_Custom_PropBounceCombine); //  <- UnityEngine.PhysicMaterial::get_bounceCombine
    SET_METRO_BINDING(PhysicMaterial_Set_Custom_PropBounceCombine); //  <- UnityEngine.PhysicMaterial::set_bounceCombine
    SET_METRO_BINDING(CharacterController_CUSTOM_INTERNAL_CALL_SimpleMove); //  <- UnityEngine.CharacterController::INTERNAL_CALL_SimpleMove
    SET_METRO_BINDING(CharacterController_CUSTOM_INTERNAL_CALL_Move); //  <- UnityEngine.CharacterController::INTERNAL_CALL_Move
    SET_METRO_BINDING(CharacterController_Get_Custom_PropIsGrounded); //  <- UnityEngine.CharacterController::get_isGrounded
    SET_METRO_BINDING(CharacterController_CUSTOM_INTERNAL_get_velocity); //  <- UnityEngine.CharacterController::INTERNAL_get_velocity
    SET_METRO_BINDING(CharacterController_Get_Custom_PropCollisionFlags); //  <- UnityEngine.CharacterController::get_collisionFlags
    SET_METRO_BINDING(CharacterController_Get_Custom_PropRadius); //  <- UnityEngine.CharacterController::get_radius
    SET_METRO_BINDING(CharacterController_Set_Custom_PropRadius); //  <- UnityEngine.CharacterController::set_radius
    SET_METRO_BINDING(CharacterController_Get_Custom_PropHeight); //  <- UnityEngine.CharacterController::get_height
    SET_METRO_BINDING(CharacterController_Set_Custom_PropHeight); //  <- UnityEngine.CharacterController::set_height
    SET_METRO_BINDING(CharacterController_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.CharacterController::INTERNAL_get_center
    SET_METRO_BINDING(CharacterController_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.CharacterController::INTERNAL_set_center
    SET_METRO_BINDING(CharacterController_Get_Custom_PropSlopeLimit); //  <- UnityEngine.CharacterController::get_slopeLimit
    SET_METRO_BINDING(CharacterController_Set_Custom_PropSlopeLimit); //  <- UnityEngine.CharacterController::set_slopeLimit
    SET_METRO_BINDING(CharacterController_Get_Custom_PropStepOffset); //  <- UnityEngine.CharacterController::get_stepOffset
    SET_METRO_BINDING(CharacterController_Set_Custom_PropStepOffset); //  <- UnityEngine.CharacterController::set_stepOffset
    SET_METRO_BINDING(CharacterController_Get_Custom_PropDetectCollisions); //  <- UnityEngine.CharacterController::get_detectCollisions
    SET_METRO_BINDING(CharacterController_Set_Custom_PropDetectCollisions); //  <- UnityEngine.CharacterController::set_detectCollisions
#endif
#if ENABLE_CLOTH
    SET_METRO_BINDING(Cloth_Get_Custom_PropBendingStiffness); //  <- UnityEngine.Cloth::get_bendingStiffness
    SET_METRO_BINDING(Cloth_Set_Custom_PropBendingStiffness); //  <- UnityEngine.Cloth::set_bendingStiffness
    SET_METRO_BINDING(Cloth_Get_Custom_PropStretchingStiffness); //  <- UnityEngine.Cloth::get_stretchingStiffness
    SET_METRO_BINDING(Cloth_Set_Custom_PropStretchingStiffness); //  <- UnityEngine.Cloth::set_stretchingStiffness
    SET_METRO_BINDING(Cloth_Get_Custom_PropDamping); //  <- UnityEngine.Cloth::get_damping
    SET_METRO_BINDING(Cloth_Set_Custom_PropDamping); //  <- UnityEngine.Cloth::set_damping
    SET_METRO_BINDING(Cloth_Get_Custom_PropThickness); //  <- UnityEngine.Cloth::get_thickness
    SET_METRO_BINDING(Cloth_Set_Custom_PropThickness); //  <- UnityEngine.Cloth::set_thickness
    SET_METRO_BINDING(Cloth_CUSTOM_INTERNAL_get_externalAcceleration); //  <- UnityEngine.Cloth::INTERNAL_get_externalAcceleration
    SET_METRO_BINDING(Cloth_CUSTOM_INTERNAL_set_externalAcceleration); //  <- UnityEngine.Cloth::INTERNAL_set_externalAcceleration
    SET_METRO_BINDING(Cloth_CUSTOM_INTERNAL_get_randomAcceleration); //  <- UnityEngine.Cloth::INTERNAL_get_randomAcceleration
    SET_METRO_BINDING(Cloth_CUSTOM_INTERNAL_set_randomAcceleration); //  <- UnityEngine.Cloth::INTERNAL_set_randomAcceleration
    SET_METRO_BINDING(Cloth_Get_Custom_PropUseGravity); //  <- UnityEngine.Cloth::get_useGravity
    SET_METRO_BINDING(Cloth_Set_Custom_PropUseGravity); //  <- UnityEngine.Cloth::set_useGravity
    SET_METRO_BINDING(Cloth_Get_Custom_PropSelfCollision); //  <- UnityEngine.Cloth::get_selfCollision
    SET_METRO_BINDING(Cloth_Set_Custom_PropSelfCollision); //  <- UnityEngine.Cloth::set_selfCollision
    SET_METRO_BINDING(Cloth_Get_Custom_PropEnabled); //  <- UnityEngine.Cloth::get_enabled
    SET_METRO_BINDING(Cloth_Set_Custom_PropEnabled); //  <- UnityEngine.Cloth::set_enabled
    SET_METRO_BINDING(Cloth_Get_Custom_PropVertices); //  <- UnityEngine.Cloth::get_vertices
    SET_METRO_BINDING(Cloth_Get_Custom_PropNormals); //  <- UnityEngine.Cloth::get_normals
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropMesh); //  <- UnityEngine.InteractiveCloth::get_mesh
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropMesh); //  <- UnityEngine.InteractiveCloth::set_mesh
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropFriction); //  <- UnityEngine.InteractiveCloth::get_friction
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropFriction); //  <- UnityEngine.InteractiveCloth::set_friction
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropDensity); //  <- UnityEngine.InteractiveCloth::get_density
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropDensity); //  <- UnityEngine.InteractiveCloth::set_density
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropPressure); //  <- UnityEngine.InteractiveCloth::get_pressure
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropPressure); //  <- UnityEngine.InteractiveCloth::set_pressure
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropCollisionResponse); //  <- UnityEngine.InteractiveCloth::get_collisionResponse
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropCollisionResponse); //  <- UnityEngine.InteractiveCloth::set_collisionResponse
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropTearFactor); //  <- UnityEngine.InteractiveCloth::get_tearFactor
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropTearFactor); //  <- UnityEngine.InteractiveCloth::set_tearFactor
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropAttachmentTearFactor); //  <- UnityEngine.InteractiveCloth::get_attachmentTearFactor
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropAttachmentTearFactor); //  <- UnityEngine.InteractiveCloth::set_attachmentTearFactor
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropAttachmentResponse); //  <- UnityEngine.InteractiveCloth::get_attachmentResponse
    SET_METRO_BINDING(InteractiveCloth_Set_Custom_PropAttachmentResponse); //  <- UnityEngine.InteractiveCloth::set_attachmentResponse
    SET_METRO_BINDING(InteractiveCloth_Get_Custom_PropIsTeared); //  <- UnityEngine.InteractiveCloth::get_isTeared
    SET_METRO_BINDING(InteractiveCloth_CUSTOM_INTERNAL_CALL_AddForceAtPosition); //  <- UnityEngine.InteractiveCloth::INTERNAL_CALL_AddForceAtPosition
    SET_METRO_BINDING(InteractiveCloth_CUSTOM_AttachToCollider); //  <- UnityEngine.InteractiveCloth::AttachToCollider
    SET_METRO_BINDING(InteractiveCloth_CUSTOM_DetachFromCollider); //  <- UnityEngine.InteractiveCloth::DetachFromCollider
    SET_METRO_BINDING(SkinnedCloth_Get_Custom_PropCoefficients); //  <- UnityEngine.SkinnedCloth::get_coefficients
    SET_METRO_BINDING(SkinnedCloth_Set_Custom_PropCoefficients); //  <- UnityEngine.SkinnedCloth::set_coefficients
    SET_METRO_BINDING(SkinnedCloth_Get_Custom_PropWorldVelocityScale); //  <- UnityEngine.SkinnedCloth::get_worldVelocityScale
    SET_METRO_BINDING(SkinnedCloth_Set_Custom_PropWorldVelocityScale); //  <- UnityEngine.SkinnedCloth::set_worldVelocityScale
    SET_METRO_BINDING(SkinnedCloth_Get_Custom_PropWorldAccelerationScale); //  <- UnityEngine.SkinnedCloth::get_worldAccelerationScale
    SET_METRO_BINDING(SkinnedCloth_Set_Custom_PropWorldAccelerationScale); //  <- UnityEngine.SkinnedCloth::set_worldAccelerationScale
    SET_METRO_BINDING(SkinnedCloth_CUSTOM_SetEnabledFading); //  <- UnityEngine.SkinnedCloth::SetEnabledFading
    SET_METRO_BINDING(ClothRenderer_Get_Custom_PropPauseWhenNotVisible); //  <- UnityEngine.ClothRenderer::get_pauseWhenNotVisible
    SET_METRO_BINDING(ClothRenderer_Set_Custom_PropPauseWhenNotVisible); //  <- UnityEngine.ClothRenderer::set_pauseWhenNotVisible
#endif
}

#endif
