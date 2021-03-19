#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingManager.h"

#include "Runtime/Dynamics/2D/Physics2DManager.h"
#include "Runtime/Dynamics/2D/Physics2DSettings.h"
#include "Runtime/Dynamics/2D/RigidBody2D.h"
#include "Runtime/Dynamics/2D/PolygonCollider2D.h"
#include "Runtime/Dynamics/2D/CircleCollider2D.h"
#include "Runtime/Dynamics/2D/BoxCollider2D.h"

#include "Runtime/Graphics/SpriteFrame.h"
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_INTERNAL_get_gravity(Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_INTERNAL_get_gravity)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_gravity)
    { *returnValue =(GetPhysics2DSettings().GetGravity ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_INTERNAL_set_gravity(const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_INTERNAL_set_gravity)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_gravity)
     return GetPhysics2DSettings().SetGravity (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_INTERNAL_CALL_Internal_Raycast(const Vector2fIcall& pointA, const Vector2fIcall& pointB, RaycastHit2D* hitInfo, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_INTERNAL_CALL_Internal_Raycast)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Raycast)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Raycast)
    
    		bool didHit = GetPhysics2DManager().Raycast (pointA, pointB, layermask, hitInfo);
    		if (!didHit)
    			return false;
    		hitInfo->collider = reinterpret_cast<Collider2D*>(ScriptingGetObjectReference (hitInfo->collider));
    		return true;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_INTERNAL_CALL_Internal_RaycastTest(const Vector2fIcall& pointA, const Vector2fIcall& pointB, int layermask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_INTERNAL_CALL_Internal_RaycastTest)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_RaycastTest)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_RaycastTest)
    
    		return GetPhysics2DManager().RaycastTest (pointA, pointB, layermask);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_INTERNAL_CALL_LinecastAll(const Vector2fIcall& start, const Vector2fIcall& end, int layerMask)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_INTERNAL_CALL_LinecastAll)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_LinecastAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_LinecastAll)
    
    		return ScriptingRaycastAll2D (start, end, layerMask);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_IgnoreLayerCollision(int layer1, int layer2, ScriptingBool ignore)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_IgnoreLayerCollision)
    SCRIPTINGAPI_STACK_CHECK(IgnoreLayerCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IgnoreLayerCollision)
    
    		GetPhysics2DSettings().IgnoreCollision(layer1, layer2, ignore);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Physics2D_CUSTOM_GetIgnoreLayerCollision(int layer1, int layer2)
{
    SCRIPTINGAPI_ETW_ENTRY(Physics2D_CUSTOM_GetIgnoreLayerCollision)
    SCRIPTINGAPI_STACK_CHECK(GetIgnoreLayerCollision)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIgnoreLayerCollision)
    
    		return GetPhysics2DSettings().GetIgnoreCollision(layer1, layer2);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForce(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& force)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForce)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddForce)
     self->AddForce (force); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_CUSTOM_AddTorque(ICallType_ReadOnlyUnityEngineObject_Argument self_, float torque)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_CUSTOM_AddTorque)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(AddTorque)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddTorque)
     self->AddTorque (torque); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForceAtPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& force, const Vector2fIcall& position)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForceAtPosition)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddForceAtPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddForceAtPosition)
     self->AddForceAtPosition (force, position); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_CUSTOM_Sleep(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_CUSTOM_Sleep)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Sleep)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Sleep)
     self->SetSleeping(true); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Rigidbody2D_CUSTOM_IsSleeping(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_CUSTOM_IsSleeping)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(IsSleeping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsSleeping)
     return self->IsSleeping(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_CUSTOM_WakeUp(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_CUSTOM_WakeUp)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(WakeUp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(WakeUp)
     self->SetSleeping(false); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody2D_Get_Custom_PropDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Get_Custom_PropDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_drag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_drag)
    return self->GetDrag ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_Set_Custom_PropDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Set_Custom_PropDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_drag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_drag)
    
    self->SetDrag (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody2D_Get_Custom_PropAngularDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Get_Custom_PropAngularDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_angularDrag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_angularDrag)
    return self->GetAngularDrag ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_Set_Custom_PropAngularDrag(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Set_Custom_PropAngularDrag)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_angularDrag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_angularDrag)
    
    self->SetAngularDrag (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody2D_Get_Custom_PropGravityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Get_Custom_PropGravityScale)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_gravityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_gravityScale)
    return self->GetGravityScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_Set_Custom_PropGravityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Set_Custom_PropGravityScale)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_gravityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_gravityScale)
    
    self->SetGravityScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Rigidbody2D_Get_Custom_PropMass(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Get_Custom_PropMass)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mass)
    return self->GetMass ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Rigidbody2D_Set_Custom_PropMass(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Rigidbody2D_Set_Custom_PropMass)
    ReadOnlyScriptingObjectOfType<Rigidbody2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_mass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mass)
    
    self->SetMass (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Collider2D_Get_Custom_PropEnabled(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider2D_Get_Custom_PropEnabled)
    ScriptingObjectOfType<Collider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Collider2D_Set_Custom_PropEnabled(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider2D_Set_Custom_PropEnabled)
    ScriptingObjectOfType<Collider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    self->SetEnabled (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Collider2D_Get_Custom_PropAttachedRigidbody(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider2D_Get_Custom_PropAttachedRigidbody)
    ScriptingObjectOfType<Collider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_attachedRigidbody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_attachedRigidbody)
    return Scripting::ScriptingWrapperFor(self->GetRigidbody());
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Collider2D_Get_Custom_PropShapeCount(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Collider2D_Get_Custom_PropShapeCount)
    ScriptingObjectOfType<Collider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shapeCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shapeCount)
    return self->GetShapeCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CircleCollider2D_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(CircleCollider2D_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<CircleCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CircleCollider2D_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(CircleCollider2D_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<CircleCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION CircleCollider2D_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(CircleCollider2D_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<CircleCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CircleCollider2D_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(CircleCollider2D_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<CircleCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider2D_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider2D_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<BoxCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider2D_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider2D_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<BoxCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider2D_CUSTOM_INTERNAL_get_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider2D_CUSTOM_INTERNAL_get_size)
    ReadOnlyScriptingObjectOfType<BoxCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_size)
    *returnValue = self->GetSize();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BoxCollider2D_CUSTOM_INTERNAL_set_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BoxCollider2D_CUSTOM_INTERNAL_set_size)
    ReadOnlyScriptingObjectOfType<BoxCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_size)
    
    self->SetSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION PolygonCollider2D_Get_Custom_PropPoints(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_Get_Custom_PropPoints)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_points)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_points)
    ScriptingClassPtr klass = GetScriptingManager().GetCommonClasses().vector2;
    return CreateScriptingArrayStride<Vector2f>(self->GetPoints(), self->GetPointCount(), klass, sizeof(*self->GetPoints()));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PolygonCollider2D_Set_Custom_PropPoints(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_Set_Custom_PropPoints)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_points)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_points)
    
    self->SetPoints(GetScriptingArrayStart<Vector2f>(value), GetScriptingArraySize(value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION PolygonCollider2D_Get_Custom_PropPathCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_Get_Custom_PropPathCount)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pathCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pathCount)
    return self->GetPathCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PolygonCollider2D_Set_Custom_PropPathCount(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_Set_Custom_PropPathCount)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_pathCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pathCount)
    
    self->SetPathCount (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION PolygonCollider2D_CUSTOM_CalculatePointCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_CUSTOM_CalculatePointCount)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(CalculatePointCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CalculatePointCount)
     return self->CalculatePointCount(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION PolygonCollider2D_CUSTOM_GetPath(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_CUSTOM_GetPath)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPath)
    
    		const PolygonCollider2D::TPath& path = self->GetPath(index);
    		ScriptingClassPtr klass = GetScriptingManager().GetCommonClasses().vector2;
    		return CreateScriptingArrayStride<Vector2f>(path.data(), path.size(), klass, sizeof(*path.data()));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PolygonCollider2D_CUSTOM_SetPath(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index, ICallType_Array_Argument points_)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_CUSTOM_SetPath)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    ICallType_Array_Local points(points_);
    UNUSED(points);
    SCRIPTINGAPI_STACK_CHECK(SetPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPath)
    
    		const Vector2f* arrayStart = GetScriptingArrayStart<Vector2f>(points);
    		const int arraySize = GetScriptingArraySize(points);
    		PolygonCollider2D::TPath path;
    		path.assign(arrayStart, arrayStart+arraySize);
    		self->SetPath(index, path);
    	
}

#if ENABLE_SPRITES
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PolygonCollider2D_CUSTOM_GenerateFrom(ICallType_ReadOnlyUnityEngineObject_Argument self_, SpriteFrame spriteFrame, float hullTolerance, UInt8 alphaTolerance, ScriptingBool holeDetection)
{
    SCRIPTINGAPI_ETW_ENTRY(PolygonCollider2D_CUSTOM_GenerateFrom)
    ReadOnlyScriptingObjectOfType<PolygonCollider2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GenerateFrom)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GenerateFrom)
    
    		self->GenerateFrom(spriteFrame, hullTolerance, alphaTolerance, holeDetection, 0);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Physics2D_INTERNAL_get_gravity()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::INTERNAL_get_gravity" , (gpointer)& Physics2D_CUSTOM_INTERNAL_get_gravity );
}

void Register_UnityEngine_Physics2D_INTERNAL_set_gravity()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::INTERNAL_set_gravity" , (gpointer)& Physics2D_CUSTOM_INTERNAL_set_gravity );
}

void Register_UnityEngine_Physics2D_INTERNAL_CALL_Internal_Raycast()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::INTERNAL_CALL_Internal_Raycast" , (gpointer)& Physics2D_CUSTOM_INTERNAL_CALL_Internal_Raycast );
}

void Register_UnityEngine_Physics2D_INTERNAL_CALL_Internal_RaycastTest()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::INTERNAL_CALL_Internal_RaycastTest" , (gpointer)& Physics2D_CUSTOM_INTERNAL_CALL_Internal_RaycastTest );
}

void Register_UnityEngine_Physics2D_INTERNAL_CALL_LinecastAll()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::INTERNAL_CALL_LinecastAll" , (gpointer)& Physics2D_CUSTOM_INTERNAL_CALL_LinecastAll );
}

void Register_UnityEngine_Physics2D_IgnoreLayerCollision()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::IgnoreLayerCollision" , (gpointer)& Physics2D_CUSTOM_IgnoreLayerCollision );
}

void Register_UnityEngine_Physics2D_GetIgnoreLayerCollision()
{
    scripting_add_internal_call( "UnityEngine.Physics2D::GetIgnoreLayerCollision" , (gpointer)& Physics2D_CUSTOM_GetIgnoreLayerCollision );
}

void Register_UnityEngine_Rigidbody2D_INTERNAL_CALL_AddForce()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForce" , (gpointer)& Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForce );
}

void Register_UnityEngine_Rigidbody2D_AddTorque()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::AddTorque" , (gpointer)& Rigidbody2D_CUSTOM_AddTorque );
}

void Register_UnityEngine_Rigidbody2D_INTERNAL_CALL_AddForceAtPosition()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForceAtPosition" , (gpointer)& Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForceAtPosition );
}

void Register_UnityEngine_Rigidbody2D_Sleep()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::Sleep" , (gpointer)& Rigidbody2D_CUSTOM_Sleep );
}

void Register_UnityEngine_Rigidbody2D_IsSleeping()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::IsSleeping" , (gpointer)& Rigidbody2D_CUSTOM_IsSleeping );
}

void Register_UnityEngine_Rigidbody2D_WakeUp()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::WakeUp" , (gpointer)& Rigidbody2D_CUSTOM_WakeUp );
}

void Register_UnityEngine_Rigidbody2D_get_drag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::get_drag" , (gpointer)& Rigidbody2D_Get_Custom_PropDrag );
}

void Register_UnityEngine_Rigidbody2D_set_drag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::set_drag" , (gpointer)& Rigidbody2D_Set_Custom_PropDrag );
}

void Register_UnityEngine_Rigidbody2D_get_angularDrag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::get_angularDrag" , (gpointer)& Rigidbody2D_Get_Custom_PropAngularDrag );
}

void Register_UnityEngine_Rigidbody2D_set_angularDrag()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::set_angularDrag" , (gpointer)& Rigidbody2D_Set_Custom_PropAngularDrag );
}

void Register_UnityEngine_Rigidbody2D_get_gravityScale()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::get_gravityScale" , (gpointer)& Rigidbody2D_Get_Custom_PropGravityScale );
}

void Register_UnityEngine_Rigidbody2D_set_gravityScale()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::set_gravityScale" , (gpointer)& Rigidbody2D_Set_Custom_PropGravityScale );
}

void Register_UnityEngine_Rigidbody2D_get_mass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::get_mass" , (gpointer)& Rigidbody2D_Get_Custom_PropMass );
}

void Register_UnityEngine_Rigidbody2D_set_mass()
{
    scripting_add_internal_call( "UnityEngine.Rigidbody2D::set_mass" , (gpointer)& Rigidbody2D_Set_Custom_PropMass );
}

void Register_UnityEngine_Collider2D_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Collider2D::get_enabled" , (gpointer)& Collider2D_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Collider2D_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Collider2D::set_enabled" , (gpointer)& Collider2D_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Collider2D_get_attachedRigidbody()
{
    scripting_add_internal_call( "UnityEngine.Collider2D::get_attachedRigidbody" , (gpointer)& Collider2D_Get_Custom_PropAttachedRigidbody );
}

void Register_UnityEngine_Collider2D_get_shapeCount()
{
    scripting_add_internal_call( "UnityEngine.Collider2D::get_shapeCount" , (gpointer)& Collider2D_Get_Custom_PropShapeCount );
}

void Register_UnityEngine_CircleCollider2D_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.CircleCollider2D::INTERNAL_get_center" , (gpointer)& CircleCollider2D_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_CircleCollider2D_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.CircleCollider2D::INTERNAL_set_center" , (gpointer)& CircleCollider2D_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_CircleCollider2D_get_radius()
{
    scripting_add_internal_call( "UnityEngine.CircleCollider2D::get_radius" , (gpointer)& CircleCollider2D_Get_Custom_PropRadius );
}

void Register_UnityEngine_CircleCollider2D_set_radius()
{
    scripting_add_internal_call( "UnityEngine.CircleCollider2D::set_radius" , (gpointer)& CircleCollider2D_Set_Custom_PropRadius );
}

void Register_UnityEngine_BoxCollider2D_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider2D::INTERNAL_get_center" , (gpointer)& BoxCollider2D_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_BoxCollider2D_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider2D::INTERNAL_set_center" , (gpointer)& BoxCollider2D_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_BoxCollider2D_INTERNAL_get_size()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider2D::INTERNAL_get_size" , (gpointer)& BoxCollider2D_CUSTOM_INTERNAL_get_size );
}

void Register_UnityEngine_BoxCollider2D_INTERNAL_set_size()
{
    scripting_add_internal_call( "UnityEngine.BoxCollider2D::INTERNAL_set_size" , (gpointer)& BoxCollider2D_CUSTOM_INTERNAL_set_size );
}

void Register_UnityEngine_PolygonCollider2D_get_points()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::get_points" , (gpointer)& PolygonCollider2D_Get_Custom_PropPoints );
}

void Register_UnityEngine_PolygonCollider2D_set_points()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::set_points" , (gpointer)& PolygonCollider2D_Set_Custom_PropPoints );
}

void Register_UnityEngine_PolygonCollider2D_get_pathCount()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::get_pathCount" , (gpointer)& PolygonCollider2D_Get_Custom_PropPathCount );
}

void Register_UnityEngine_PolygonCollider2D_set_pathCount()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::set_pathCount" , (gpointer)& PolygonCollider2D_Set_Custom_PropPathCount );
}

void Register_UnityEngine_PolygonCollider2D_CalculatePointCount()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::CalculatePointCount" , (gpointer)& PolygonCollider2D_CUSTOM_CalculatePointCount );
}

void Register_UnityEngine_PolygonCollider2D_GetPath()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::GetPath" , (gpointer)& PolygonCollider2D_CUSTOM_GetPath );
}

void Register_UnityEngine_PolygonCollider2D_SetPath()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::SetPath" , (gpointer)& PolygonCollider2D_CUSTOM_SetPath );
}

#if ENABLE_SPRITES
void Register_UnityEngine_PolygonCollider2D_GenerateFrom()
{
    scripting_add_internal_call( "UnityEngine.PolygonCollider2D::GenerateFrom" , (gpointer)& PolygonCollider2D_CUSTOM_GenerateFrom );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Physics2D_IcallNames [] =
{
    "UnityEngine.Physics2D::INTERNAL_get_gravity",    // -> Physics2D_CUSTOM_INTERNAL_get_gravity
    "UnityEngine.Physics2D::INTERNAL_set_gravity",    // -> Physics2D_CUSTOM_INTERNAL_set_gravity
    "UnityEngine.Physics2D::INTERNAL_CALL_Internal_Raycast",    // -> Physics2D_CUSTOM_INTERNAL_CALL_Internal_Raycast
    "UnityEngine.Physics2D::INTERNAL_CALL_Internal_RaycastTest",    // -> Physics2D_CUSTOM_INTERNAL_CALL_Internal_RaycastTest
    "UnityEngine.Physics2D::INTERNAL_CALL_LinecastAll",    // -> Physics2D_CUSTOM_INTERNAL_CALL_LinecastAll
    "UnityEngine.Physics2D::IgnoreLayerCollision",    // -> Physics2D_CUSTOM_IgnoreLayerCollision
    "UnityEngine.Physics2D::GetIgnoreLayerCollision",    // -> Physics2D_CUSTOM_GetIgnoreLayerCollision
    "UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForce",    // -> Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForce
    "UnityEngine.Rigidbody2D::AddTorque"    ,    // -> Rigidbody2D_CUSTOM_AddTorque
    "UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForceAtPosition",    // -> Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForceAtPosition
    "UnityEngine.Rigidbody2D::Sleep"        ,    // -> Rigidbody2D_CUSTOM_Sleep
    "UnityEngine.Rigidbody2D::IsSleeping"   ,    // -> Rigidbody2D_CUSTOM_IsSleeping
    "UnityEngine.Rigidbody2D::WakeUp"       ,    // -> Rigidbody2D_CUSTOM_WakeUp
    "UnityEngine.Rigidbody2D::get_drag"     ,    // -> Rigidbody2D_Get_Custom_PropDrag
    "UnityEngine.Rigidbody2D::set_drag"     ,    // -> Rigidbody2D_Set_Custom_PropDrag
    "UnityEngine.Rigidbody2D::get_angularDrag",    // -> Rigidbody2D_Get_Custom_PropAngularDrag
    "UnityEngine.Rigidbody2D::set_angularDrag",    // -> Rigidbody2D_Set_Custom_PropAngularDrag
    "UnityEngine.Rigidbody2D::get_gravityScale",    // -> Rigidbody2D_Get_Custom_PropGravityScale
    "UnityEngine.Rigidbody2D::set_gravityScale",    // -> Rigidbody2D_Set_Custom_PropGravityScale
    "UnityEngine.Rigidbody2D::get_mass"     ,    // -> Rigidbody2D_Get_Custom_PropMass
    "UnityEngine.Rigidbody2D::set_mass"     ,    // -> Rigidbody2D_Set_Custom_PropMass
    "UnityEngine.Collider2D::get_enabled"   ,    // -> Collider2D_Get_Custom_PropEnabled
    "UnityEngine.Collider2D::set_enabled"   ,    // -> Collider2D_Set_Custom_PropEnabled
    "UnityEngine.Collider2D::get_attachedRigidbody",    // -> Collider2D_Get_Custom_PropAttachedRigidbody
    "UnityEngine.Collider2D::get_shapeCount",    // -> Collider2D_Get_Custom_PropShapeCount
    "UnityEngine.CircleCollider2D::INTERNAL_get_center",    // -> CircleCollider2D_CUSTOM_INTERNAL_get_center
    "UnityEngine.CircleCollider2D::INTERNAL_set_center",    // -> CircleCollider2D_CUSTOM_INTERNAL_set_center
    "UnityEngine.CircleCollider2D::get_radius",    // -> CircleCollider2D_Get_Custom_PropRadius
    "UnityEngine.CircleCollider2D::set_radius",    // -> CircleCollider2D_Set_Custom_PropRadius
    "UnityEngine.BoxCollider2D::INTERNAL_get_center",    // -> BoxCollider2D_CUSTOM_INTERNAL_get_center
    "UnityEngine.BoxCollider2D::INTERNAL_set_center",    // -> BoxCollider2D_CUSTOM_INTERNAL_set_center
    "UnityEngine.BoxCollider2D::INTERNAL_get_size",    // -> BoxCollider2D_CUSTOM_INTERNAL_get_size
    "UnityEngine.BoxCollider2D::INTERNAL_set_size",    // -> BoxCollider2D_CUSTOM_INTERNAL_set_size
    "UnityEngine.PolygonCollider2D::get_points",    // -> PolygonCollider2D_Get_Custom_PropPoints
    "UnityEngine.PolygonCollider2D::set_points",    // -> PolygonCollider2D_Set_Custom_PropPoints
    "UnityEngine.PolygonCollider2D::get_pathCount",    // -> PolygonCollider2D_Get_Custom_PropPathCount
    "UnityEngine.PolygonCollider2D::set_pathCount",    // -> PolygonCollider2D_Set_Custom_PropPathCount
    "UnityEngine.PolygonCollider2D::CalculatePointCount",    // -> PolygonCollider2D_CUSTOM_CalculatePointCount
    "UnityEngine.PolygonCollider2D::GetPath",    // -> PolygonCollider2D_CUSTOM_GetPath
    "UnityEngine.PolygonCollider2D::SetPath",    // -> PolygonCollider2D_CUSTOM_SetPath
#if ENABLE_SPRITES
    "UnityEngine.PolygonCollider2D::GenerateFrom",    // -> PolygonCollider2D_CUSTOM_GenerateFrom
#endif
    NULL
};

static const void* s_Physics2D_IcallFuncs [] =
{
    (const void*)&Physics2D_CUSTOM_INTERNAL_get_gravity   ,  //  <- UnityEngine.Physics2D::INTERNAL_get_gravity
    (const void*)&Physics2D_CUSTOM_INTERNAL_set_gravity   ,  //  <- UnityEngine.Physics2D::INTERNAL_set_gravity
    (const void*)&Physics2D_CUSTOM_INTERNAL_CALL_Internal_Raycast,  //  <- UnityEngine.Physics2D::INTERNAL_CALL_Internal_Raycast
    (const void*)&Physics2D_CUSTOM_INTERNAL_CALL_Internal_RaycastTest,  //  <- UnityEngine.Physics2D::INTERNAL_CALL_Internal_RaycastTest
    (const void*)&Physics2D_CUSTOM_INTERNAL_CALL_LinecastAll,  //  <- UnityEngine.Physics2D::INTERNAL_CALL_LinecastAll
    (const void*)&Physics2D_CUSTOM_IgnoreLayerCollision   ,  //  <- UnityEngine.Physics2D::IgnoreLayerCollision
    (const void*)&Physics2D_CUSTOM_GetIgnoreLayerCollision,  //  <- UnityEngine.Physics2D::GetIgnoreLayerCollision
    (const void*)&Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForce,  //  <- UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForce
    (const void*)&Rigidbody2D_CUSTOM_AddTorque            ,  //  <- UnityEngine.Rigidbody2D::AddTorque
    (const void*)&Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForceAtPosition,  //  <- UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForceAtPosition
    (const void*)&Rigidbody2D_CUSTOM_Sleep                ,  //  <- UnityEngine.Rigidbody2D::Sleep
    (const void*)&Rigidbody2D_CUSTOM_IsSleeping           ,  //  <- UnityEngine.Rigidbody2D::IsSleeping
    (const void*)&Rigidbody2D_CUSTOM_WakeUp               ,  //  <- UnityEngine.Rigidbody2D::WakeUp
    (const void*)&Rigidbody2D_Get_Custom_PropDrag         ,  //  <- UnityEngine.Rigidbody2D::get_drag
    (const void*)&Rigidbody2D_Set_Custom_PropDrag         ,  //  <- UnityEngine.Rigidbody2D::set_drag
    (const void*)&Rigidbody2D_Get_Custom_PropAngularDrag  ,  //  <- UnityEngine.Rigidbody2D::get_angularDrag
    (const void*)&Rigidbody2D_Set_Custom_PropAngularDrag  ,  //  <- UnityEngine.Rigidbody2D::set_angularDrag
    (const void*)&Rigidbody2D_Get_Custom_PropGravityScale ,  //  <- UnityEngine.Rigidbody2D::get_gravityScale
    (const void*)&Rigidbody2D_Set_Custom_PropGravityScale ,  //  <- UnityEngine.Rigidbody2D::set_gravityScale
    (const void*)&Rigidbody2D_Get_Custom_PropMass         ,  //  <- UnityEngine.Rigidbody2D::get_mass
    (const void*)&Rigidbody2D_Set_Custom_PropMass         ,  //  <- UnityEngine.Rigidbody2D::set_mass
    (const void*)&Collider2D_Get_Custom_PropEnabled       ,  //  <- UnityEngine.Collider2D::get_enabled
    (const void*)&Collider2D_Set_Custom_PropEnabled       ,  //  <- UnityEngine.Collider2D::set_enabled
    (const void*)&Collider2D_Get_Custom_PropAttachedRigidbody,  //  <- UnityEngine.Collider2D::get_attachedRigidbody
    (const void*)&Collider2D_Get_Custom_PropShapeCount    ,  //  <- UnityEngine.Collider2D::get_shapeCount
    (const void*)&CircleCollider2D_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.CircleCollider2D::INTERNAL_get_center
    (const void*)&CircleCollider2D_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.CircleCollider2D::INTERNAL_set_center
    (const void*)&CircleCollider2D_Get_Custom_PropRadius  ,  //  <- UnityEngine.CircleCollider2D::get_radius
    (const void*)&CircleCollider2D_Set_Custom_PropRadius  ,  //  <- UnityEngine.CircleCollider2D::set_radius
    (const void*)&BoxCollider2D_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.BoxCollider2D::INTERNAL_get_center
    (const void*)&BoxCollider2D_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.BoxCollider2D::INTERNAL_set_center
    (const void*)&BoxCollider2D_CUSTOM_INTERNAL_get_size  ,  //  <- UnityEngine.BoxCollider2D::INTERNAL_get_size
    (const void*)&BoxCollider2D_CUSTOM_INTERNAL_set_size  ,  //  <- UnityEngine.BoxCollider2D::INTERNAL_set_size
    (const void*)&PolygonCollider2D_Get_Custom_PropPoints ,  //  <- UnityEngine.PolygonCollider2D::get_points
    (const void*)&PolygonCollider2D_Set_Custom_PropPoints ,  //  <- UnityEngine.PolygonCollider2D::set_points
    (const void*)&PolygonCollider2D_Get_Custom_PropPathCount,  //  <- UnityEngine.PolygonCollider2D::get_pathCount
    (const void*)&PolygonCollider2D_Set_Custom_PropPathCount,  //  <- UnityEngine.PolygonCollider2D::set_pathCount
    (const void*)&PolygonCollider2D_CUSTOM_CalculatePointCount,  //  <- UnityEngine.PolygonCollider2D::CalculatePointCount
    (const void*)&PolygonCollider2D_CUSTOM_GetPath        ,  //  <- UnityEngine.PolygonCollider2D::GetPath
    (const void*)&PolygonCollider2D_CUSTOM_SetPath        ,  //  <- UnityEngine.PolygonCollider2D::SetPath
#if ENABLE_SPRITES
    (const void*)&PolygonCollider2D_CUSTOM_GenerateFrom   ,  //  <- UnityEngine.PolygonCollider2D::GenerateFrom
#endif
    NULL
};

void ExportPhysics2DBindings();
void ExportPhysics2DBindings()
{
    for (int i = 0; s_Physics2D_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Physics2D_IcallNames [i], s_Physics2D_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportPhysics2DBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Physics2D_CUSTOM_INTERNAL_get_gravity); //  <- UnityEngine.Physics2D::INTERNAL_get_gravity
    SET_METRO_BINDING(Physics2D_CUSTOM_INTERNAL_set_gravity); //  <- UnityEngine.Physics2D::INTERNAL_set_gravity
    SET_METRO_BINDING(Physics2D_CUSTOM_INTERNAL_CALL_Internal_Raycast); //  <- UnityEngine.Physics2D::INTERNAL_CALL_Internal_Raycast
    SET_METRO_BINDING(Physics2D_CUSTOM_INTERNAL_CALL_Internal_RaycastTest); //  <- UnityEngine.Physics2D::INTERNAL_CALL_Internal_RaycastTest
    SET_METRO_BINDING(Physics2D_CUSTOM_INTERNAL_CALL_LinecastAll); //  <- UnityEngine.Physics2D::INTERNAL_CALL_LinecastAll
    SET_METRO_BINDING(Physics2D_CUSTOM_IgnoreLayerCollision); //  <- UnityEngine.Physics2D::IgnoreLayerCollision
    SET_METRO_BINDING(Physics2D_CUSTOM_GetIgnoreLayerCollision); //  <- UnityEngine.Physics2D::GetIgnoreLayerCollision
    SET_METRO_BINDING(Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForce); //  <- UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForce
    SET_METRO_BINDING(Rigidbody2D_CUSTOM_AddTorque); //  <- UnityEngine.Rigidbody2D::AddTorque
    SET_METRO_BINDING(Rigidbody2D_CUSTOM_INTERNAL_CALL_AddForceAtPosition); //  <- UnityEngine.Rigidbody2D::INTERNAL_CALL_AddForceAtPosition
    SET_METRO_BINDING(Rigidbody2D_CUSTOM_Sleep); //  <- UnityEngine.Rigidbody2D::Sleep
    SET_METRO_BINDING(Rigidbody2D_CUSTOM_IsSleeping); //  <- UnityEngine.Rigidbody2D::IsSleeping
    SET_METRO_BINDING(Rigidbody2D_CUSTOM_WakeUp); //  <- UnityEngine.Rigidbody2D::WakeUp
    SET_METRO_BINDING(Rigidbody2D_Get_Custom_PropDrag); //  <- UnityEngine.Rigidbody2D::get_drag
    SET_METRO_BINDING(Rigidbody2D_Set_Custom_PropDrag); //  <- UnityEngine.Rigidbody2D::set_drag
    SET_METRO_BINDING(Rigidbody2D_Get_Custom_PropAngularDrag); //  <- UnityEngine.Rigidbody2D::get_angularDrag
    SET_METRO_BINDING(Rigidbody2D_Set_Custom_PropAngularDrag); //  <- UnityEngine.Rigidbody2D::set_angularDrag
    SET_METRO_BINDING(Rigidbody2D_Get_Custom_PropGravityScale); //  <- UnityEngine.Rigidbody2D::get_gravityScale
    SET_METRO_BINDING(Rigidbody2D_Set_Custom_PropGravityScale); //  <- UnityEngine.Rigidbody2D::set_gravityScale
    SET_METRO_BINDING(Rigidbody2D_Get_Custom_PropMass); //  <- UnityEngine.Rigidbody2D::get_mass
    SET_METRO_BINDING(Rigidbody2D_Set_Custom_PropMass); //  <- UnityEngine.Rigidbody2D::set_mass
    SET_METRO_BINDING(Collider2D_Get_Custom_PropEnabled); //  <- UnityEngine.Collider2D::get_enabled
    SET_METRO_BINDING(Collider2D_Set_Custom_PropEnabled); //  <- UnityEngine.Collider2D::set_enabled
    SET_METRO_BINDING(Collider2D_Get_Custom_PropAttachedRigidbody); //  <- UnityEngine.Collider2D::get_attachedRigidbody
    SET_METRO_BINDING(Collider2D_Get_Custom_PropShapeCount); //  <- UnityEngine.Collider2D::get_shapeCount
    SET_METRO_BINDING(CircleCollider2D_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.CircleCollider2D::INTERNAL_get_center
    SET_METRO_BINDING(CircleCollider2D_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.CircleCollider2D::INTERNAL_set_center
    SET_METRO_BINDING(CircleCollider2D_Get_Custom_PropRadius); //  <- UnityEngine.CircleCollider2D::get_radius
    SET_METRO_BINDING(CircleCollider2D_Set_Custom_PropRadius); //  <- UnityEngine.CircleCollider2D::set_radius
    SET_METRO_BINDING(BoxCollider2D_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.BoxCollider2D::INTERNAL_get_center
    SET_METRO_BINDING(BoxCollider2D_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.BoxCollider2D::INTERNAL_set_center
    SET_METRO_BINDING(BoxCollider2D_CUSTOM_INTERNAL_get_size); //  <- UnityEngine.BoxCollider2D::INTERNAL_get_size
    SET_METRO_BINDING(BoxCollider2D_CUSTOM_INTERNAL_set_size); //  <- UnityEngine.BoxCollider2D::INTERNAL_set_size
    SET_METRO_BINDING(PolygonCollider2D_Get_Custom_PropPoints); //  <- UnityEngine.PolygonCollider2D::get_points
    SET_METRO_BINDING(PolygonCollider2D_Set_Custom_PropPoints); //  <- UnityEngine.PolygonCollider2D::set_points
    SET_METRO_BINDING(PolygonCollider2D_Get_Custom_PropPathCount); //  <- UnityEngine.PolygonCollider2D::get_pathCount
    SET_METRO_BINDING(PolygonCollider2D_Set_Custom_PropPathCount); //  <- UnityEngine.PolygonCollider2D::set_pathCount
    SET_METRO_BINDING(PolygonCollider2D_CUSTOM_CalculatePointCount); //  <- UnityEngine.PolygonCollider2D::CalculatePointCount
    SET_METRO_BINDING(PolygonCollider2D_CUSTOM_GetPath); //  <- UnityEngine.PolygonCollider2D::GetPath
    SET_METRO_BINDING(PolygonCollider2D_CUSTOM_SetPath); //  <- UnityEngine.PolygonCollider2D::SetPath
#if ENABLE_SPRITES
    SET_METRO_BINDING(PolygonCollider2D_CUSTOM_GenerateFrom); //  <- UnityEngine.PolygonCollider2D::GenerateFrom
#endif
}

#endif
