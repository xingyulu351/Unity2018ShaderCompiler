#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/ParticleSystem/ParticleSystem.h"
#include "Runtime/ParticleSystem/ParticleSystemRenderer.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Filters/Mesh/LodMesh.h"
#if ENABLE_PHYSICS
#include "Runtime/Dynamics/Collider.h"
#endif

#if UNITY_EDITOR
#include "Editor/Src/ParticleSystem/ParticleSystemEditor.h"
#endif

using namespace Unity;
using namespace std;


#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_InstanceIDToCollider(int instanceID)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_InstanceIDToCollider)
    SCRIPTINGAPI_STACK_CHECK(InstanceIDToCollider)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InstanceIDToCollider)
     
        return instanceID != 0 ? Scripting::ScriptingWrapperFor (PPtr<Collider> (instanceID)) : SCRIPTING_NULL;
     
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropStartDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropStartDelay)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startDelay) self->SyncJobs();
    return self->GetStartDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropStartDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropStartDelay)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_startDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_startDelay) self->SyncJobs();
    
    self->SetStartDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropIsPlaying(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropIsPlaying)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPlaying) self->SyncJobs();
    return self->IsPlaying ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropIsStopped(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropIsStopped)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isStopped)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isStopped) self->SyncJobs();
    return self->IsStopped ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropIsPaused(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropIsPaused)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPaused)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPaused) self->SyncJobs();
    return self->IsPaused ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropLoop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropLoop)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_loop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_loop) self->SyncJobs();
    return self->GetLoop ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropLoop(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropLoop)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_loop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_loop) self->SyncJobs();
    
    self->SetLoop (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropPlayOnAwake(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropPlayOnAwake)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_playOnAwake)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_playOnAwake) self->SyncJobs();
    return self->GetPlayOnAwake ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropPlayOnAwake(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropPlayOnAwake)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_playOnAwake)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_playOnAwake) self->SyncJobs();
    
    self->SetPlayOnAwake (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropTime)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time) self->SyncJobs();
    return self->GetSecPosition ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropTime)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_time) self->SyncJobs();
    
    self->SetSecPosition (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropDuration(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropDuration)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_duration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_duration) self->SyncJobs();
    return self->GetLengthInSec ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropPlaybackSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropPlaybackSpeed)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_playbackSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_playbackSpeed) self->SyncJobs();
    return self->GetPlaybackSpeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropPlaybackSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropPlaybackSpeed)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_playbackSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_playbackSpeed) self->SyncJobs();
    
    self->SetPlaybackSpeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropParticleCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropParticleCount)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_particleCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_particleCount) self->SyncJobs();
    return self->GetParticleCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropSafeCollisionEventSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropSafeCollisionEventSize)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_safeCollisionEventSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_safeCollisionEventSize)
    return self->GetSafeCollisionEventSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropEnableEmission(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropEnableEmission)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enableEmission)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enableEmission) self->SyncJobs();
    return self->GetEnableEmission ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropEnableEmission(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropEnableEmission)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enableEmission)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enableEmission) self->SyncJobs();
    
    self->SetEnableEmission (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropEmissionRate(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropEmissionRate)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_emissionRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_emissionRate) self->SyncJobs();
    return self->GetEmissionRate ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropEmissionRate(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropEmissionRate)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_emissionRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_emissionRate) self->SyncJobs();
    
    self->SetEmissionRate (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropStartSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropStartSpeed)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startSpeed) self->SyncJobs();
    return self->GetStartSpeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropStartSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropStartSpeed)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_startSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_startSpeed) self->SyncJobs();
    
    self->SetStartSpeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropStartSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropStartSize)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startSize) self->SyncJobs();
    return self->GetStartSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropStartSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropStartSize)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_startSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_startSize) self->SyncJobs();
    
    self->SetStartSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_INTERNAL_get_startColor(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_INTERNAL_get_startColor)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_startColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_startColor) self->SyncJobs();
    *returnValue = self->GetStartColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_INTERNAL_set_startColor(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_INTERNAL_set_startColor)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_startColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_startColor) self->SyncJobs();
    
    self->SetStartColor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropStartRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropStartRotation)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startRotation) self->SyncJobs();
    return self->GetStartRotation ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropStartRotation(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropStartRotation)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_startRotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_startRotation) self->SyncJobs();
    
    self->SetStartRotation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropStartLifetime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropStartLifetime)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startLifetime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startLifetime) self->SyncJobs();
    return self->GetStartLifeTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropStartLifetime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropStartLifetime)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_startLifetime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_startLifetime) self->SyncJobs();
    
    self->SetStartLifeTime (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropGravityModifier(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropGravityModifier)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_gravityModifier)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_gravityModifier) self->SyncJobs();
    return self->GetGravityModifier ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropGravityModifier(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropGravityModifier)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_gravityModifier)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_gravityModifier) self->SyncJobs();
    
    self->SetGravityModifier (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION ParticleSystem_Get_Custom_PropRandomSeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Get_Custom_PropRandomSeed)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_randomSeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_randomSeed) self->SyncJobs();
    return self->GetRandomSeed ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_Set_Custom_PropRandomSeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, UInt32 value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_Set_Custom_PropRandomSeed)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_randomSeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_randomSeed) self->SyncJobs();
    
    self->SetRandomSeed (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_SetParticles(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument particles_, int size)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_SetParticles)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    ICallType_Array_Local particles(particles_);
    UNUSED(particles);
    SCRIPTINGAPI_STACK_CHECK(SetParticles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetParticles) self->SyncJobs();
    
    		unsigned int actualSize = GetScriptingArraySize(particles);
    		if (size < 0 || actualSize < size)
    			size = actualSize;
    	
    		self->SetParticlesExternal (GetScriptingArrayStart<ParticleSystemParticle>(particles), size);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_GetParticles(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument particles_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_GetParticles)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    ICallType_Array_Local particles(particles_);
    UNUSED(particles);
    SCRIPTINGAPI_STACK_CHECK(GetParticles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetParticles) self->SyncJobs();
    
    		int size = std::min<unsigned int>(self->GetParticleCount(), GetScriptingArraySize(particles));
    		self->GetParticlesExternal (GetScriptingArrayStart<ParticleSystemParticle>(particles), size);
    		return size;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_GetCollisionEvents(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument go_, ICallType_Array_Argument collisionEvents_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_GetCollisionEvents)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<GameObject> go(go_);
    UNUSED(go);
    ICallType_Array_Local collisionEvents(collisionEvents_);
    UNUSED(collisionEvents);
    SCRIPTINGAPI_STACK_CHECK(GetCollisionEvents)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetCollisionEvents)
    
    		return self->GetCollisionEventsExternal (go->GetInstanceID (), GetScriptingArrayStart<MonoParticleCollisionEvent>(collisionEvents), GetScriptingArraySize(collisionEvents));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_Simulate(ICallType_ReadOnlyUnityEngineObject_Argument self_, float t, ScriptingBool restart)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_Simulate)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_Simulate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Simulate) self->SyncJobs();
     self->Simulate (t, restart); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_Play(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_Play)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_Play)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Play) self->SyncJobs();
     self->Play (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_Stop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_Stop)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Stop) self->SyncJobs();
     self->Stop (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_Pause(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_Pause)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_Pause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Pause) self->SyncJobs();
     self->Pause (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_Clear(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_Clear)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_Clear)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Clear) self->SyncJobs();
     self->Clear (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_IsAlive(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_IsAlive)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_IsAlive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_IsAlive) self->SyncJobs();
     return self->IsAlive(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_INTERNAL_CALL_Emit(ICallType_ReadOnlyUnityEngineObject_Argument self_, int count)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_INTERNAL_CALL_Emit)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Emit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Emit) self->SyncJobs();
    return self->Emit(count);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_Internal_Emit(ICallType_ReadOnlyUnityEngineObject_Argument self_, ParticleSystemParticle& particle)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_Internal_Emit)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_Emit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Emit) self->SyncJobs();
    
    		self->EmitParticleExternal (&particle);
    	
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystem_CUSTOM_SetupDefaultType(ICallType_ReadOnlyUnityEngineObject_Argument self_, int type)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystem_CUSTOM_SetupDefaultType)
    ReadOnlyScriptingObjectOfType<ParticleSystem> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetupDefaultType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetupDefaultType)
    	
    		ParticleSystemEditor::SetupDefaultParticleSystemType(*self, (ParticleSystemSubType)type);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ParticleSystemRenderMode SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropRenderMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropRenderMode)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderMode)
    return self->GetRenderMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropRenderMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, ParticleSystemRenderMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropRenderMode)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_renderMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_renderMode)
    
    self->SetRenderMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropLengthScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropLengthScale)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lengthScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lengthScale)
    return self->GetLengthScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropLengthScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropLengthScale)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lengthScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lengthScale)
    
    self->SetLengthScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropVelocityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropVelocityScale)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_velocityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_velocityScale)
    return self->GetVelocityScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropVelocityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropVelocityScale)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_velocityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_velocityScale)
    
    self->SetVelocityScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropCameraVelocityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropCameraVelocityScale)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cameraVelocityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cameraVelocityScale)
    return self->GetCameraVelocityScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropCameraVelocityScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropCameraVelocityScale)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cameraVelocityScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cameraVelocityScale)
    
    self->SetCameraVelocityScale (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropMaxParticleSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropMaxParticleSize)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maxParticleSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxParticleSize)
    return self->GetMaxParticleSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropMaxParticleSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropMaxParticleSize)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maxParticleSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxParticleSize)
    
    self->SetMaxParticleSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropMesh)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mesh)
    return Scripting::ScriptingWrapperFor(self->GetMesh());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropMesh)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_mesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mesh)
    
    self->SetMesh (val);
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Get_Custom_PropEditorEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Get_Custom_PropEditorEnabled)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_editorEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_editorEnabled)
    return self->GetEditorEnabled();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ParticleSystemRenderer_Set_Custom_PropEditorEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(ParticleSystemRenderer_Set_Custom_PropEditorEnabled)
    ReadOnlyScriptingObjectOfType<ParticleSystemRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_editorEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_editorEnabled)
    self->SetEditorEnabled(value);
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_PHYSICS
void Register_UnityEngine_ParticleSystem_InstanceIDToCollider()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::InstanceIDToCollider" , (gpointer)& ParticleSystem_CUSTOM_InstanceIDToCollider );
}

#endif
void Register_UnityEngine_ParticleSystem_get_startDelay()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_startDelay" , (gpointer)& ParticleSystem_Get_Custom_PropStartDelay );
}

void Register_UnityEngine_ParticleSystem_set_startDelay()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_startDelay" , (gpointer)& ParticleSystem_Set_Custom_PropStartDelay );
}

void Register_UnityEngine_ParticleSystem_get_isPlaying()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_isPlaying" , (gpointer)& ParticleSystem_Get_Custom_PropIsPlaying );
}

void Register_UnityEngine_ParticleSystem_get_isStopped()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_isStopped" , (gpointer)& ParticleSystem_Get_Custom_PropIsStopped );
}

void Register_UnityEngine_ParticleSystem_get_isPaused()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_isPaused" , (gpointer)& ParticleSystem_Get_Custom_PropIsPaused );
}

void Register_UnityEngine_ParticleSystem_get_loop()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_loop" , (gpointer)& ParticleSystem_Get_Custom_PropLoop );
}

void Register_UnityEngine_ParticleSystem_set_loop()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_loop" , (gpointer)& ParticleSystem_Set_Custom_PropLoop );
}

void Register_UnityEngine_ParticleSystem_get_playOnAwake()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_playOnAwake" , (gpointer)& ParticleSystem_Get_Custom_PropPlayOnAwake );
}

void Register_UnityEngine_ParticleSystem_set_playOnAwake()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_playOnAwake" , (gpointer)& ParticleSystem_Set_Custom_PropPlayOnAwake );
}

void Register_UnityEngine_ParticleSystem_get_time()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_time" , (gpointer)& ParticleSystem_Get_Custom_PropTime );
}

void Register_UnityEngine_ParticleSystem_set_time()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_time" , (gpointer)& ParticleSystem_Set_Custom_PropTime );
}

void Register_UnityEngine_ParticleSystem_get_duration()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_duration" , (gpointer)& ParticleSystem_Get_Custom_PropDuration );
}

void Register_UnityEngine_ParticleSystem_get_playbackSpeed()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_playbackSpeed" , (gpointer)& ParticleSystem_Get_Custom_PropPlaybackSpeed );
}

void Register_UnityEngine_ParticleSystem_set_playbackSpeed()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_playbackSpeed" , (gpointer)& ParticleSystem_Set_Custom_PropPlaybackSpeed );
}

void Register_UnityEngine_ParticleSystem_get_particleCount()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_particleCount" , (gpointer)& ParticleSystem_Get_Custom_PropParticleCount );
}

void Register_UnityEngine_ParticleSystem_get_safeCollisionEventSize()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_safeCollisionEventSize" , (gpointer)& ParticleSystem_Get_Custom_PropSafeCollisionEventSize );
}

void Register_UnityEngine_ParticleSystem_get_enableEmission()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_enableEmission" , (gpointer)& ParticleSystem_Get_Custom_PropEnableEmission );
}

void Register_UnityEngine_ParticleSystem_set_enableEmission()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_enableEmission" , (gpointer)& ParticleSystem_Set_Custom_PropEnableEmission );
}

void Register_UnityEngine_ParticleSystem_get_emissionRate()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_emissionRate" , (gpointer)& ParticleSystem_Get_Custom_PropEmissionRate );
}

void Register_UnityEngine_ParticleSystem_set_emissionRate()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_emissionRate" , (gpointer)& ParticleSystem_Set_Custom_PropEmissionRate );
}

void Register_UnityEngine_ParticleSystem_get_startSpeed()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_startSpeed" , (gpointer)& ParticleSystem_Get_Custom_PropStartSpeed );
}

void Register_UnityEngine_ParticleSystem_set_startSpeed()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_startSpeed" , (gpointer)& ParticleSystem_Set_Custom_PropStartSpeed );
}

void Register_UnityEngine_ParticleSystem_get_startSize()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_startSize" , (gpointer)& ParticleSystem_Get_Custom_PropStartSize );
}

void Register_UnityEngine_ParticleSystem_set_startSize()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_startSize" , (gpointer)& ParticleSystem_Set_Custom_PropStartSize );
}

void Register_UnityEngine_ParticleSystem_INTERNAL_get_startColor()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::INTERNAL_get_startColor" , (gpointer)& ParticleSystem_CUSTOM_INTERNAL_get_startColor );
}

void Register_UnityEngine_ParticleSystem_INTERNAL_set_startColor()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::INTERNAL_set_startColor" , (gpointer)& ParticleSystem_CUSTOM_INTERNAL_set_startColor );
}

void Register_UnityEngine_ParticleSystem_get_startRotation()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_startRotation" , (gpointer)& ParticleSystem_Get_Custom_PropStartRotation );
}

void Register_UnityEngine_ParticleSystem_set_startRotation()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_startRotation" , (gpointer)& ParticleSystem_Set_Custom_PropStartRotation );
}

void Register_UnityEngine_ParticleSystem_get_startLifetime()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_startLifetime" , (gpointer)& ParticleSystem_Get_Custom_PropStartLifetime );
}

void Register_UnityEngine_ParticleSystem_set_startLifetime()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_startLifetime" , (gpointer)& ParticleSystem_Set_Custom_PropStartLifetime );
}

void Register_UnityEngine_ParticleSystem_get_gravityModifier()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_gravityModifier" , (gpointer)& ParticleSystem_Get_Custom_PropGravityModifier );
}

void Register_UnityEngine_ParticleSystem_set_gravityModifier()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_gravityModifier" , (gpointer)& ParticleSystem_Set_Custom_PropGravityModifier );
}

void Register_UnityEngine_ParticleSystem_get_randomSeed()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::get_randomSeed" , (gpointer)& ParticleSystem_Get_Custom_PropRandomSeed );
}

void Register_UnityEngine_ParticleSystem_set_randomSeed()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::set_randomSeed" , (gpointer)& ParticleSystem_Set_Custom_PropRandomSeed );
}

void Register_UnityEngine_ParticleSystem_SetParticles()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::SetParticles" , (gpointer)& ParticleSystem_CUSTOM_SetParticles );
}

void Register_UnityEngine_ParticleSystem_GetParticles()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::GetParticles" , (gpointer)& ParticleSystem_CUSTOM_GetParticles );
}

void Register_UnityEngine_ParticleSystem_GetCollisionEvents()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::GetCollisionEvents" , (gpointer)& ParticleSystem_CUSTOM_GetCollisionEvents );
}

void Register_UnityEngine_ParticleSystem_Internal_Simulate()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_Simulate" , (gpointer)& ParticleSystem_CUSTOM_Internal_Simulate );
}

void Register_UnityEngine_ParticleSystem_Internal_Play()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_Play" , (gpointer)& ParticleSystem_CUSTOM_Internal_Play );
}

void Register_UnityEngine_ParticleSystem_Internal_Stop()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_Stop" , (gpointer)& ParticleSystem_CUSTOM_Internal_Stop );
}

void Register_UnityEngine_ParticleSystem_Internal_Pause()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_Pause" , (gpointer)& ParticleSystem_CUSTOM_Internal_Pause );
}

void Register_UnityEngine_ParticleSystem_Internal_Clear()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_Clear" , (gpointer)& ParticleSystem_CUSTOM_Internal_Clear );
}

void Register_UnityEngine_ParticleSystem_Internal_IsAlive()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_IsAlive" , (gpointer)& ParticleSystem_CUSTOM_Internal_IsAlive );
}

void Register_UnityEngine_ParticleSystem_INTERNAL_CALL_Emit()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::INTERNAL_CALL_Emit" , (gpointer)& ParticleSystem_CUSTOM_INTERNAL_CALL_Emit );
}

void Register_UnityEngine_ParticleSystem_Internal_Emit()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::Internal_Emit" , (gpointer)& ParticleSystem_CUSTOM_Internal_Emit );
}

#if UNITY_EDITOR
void Register_UnityEngine_ParticleSystem_SetupDefaultType()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystem::SetupDefaultType" , (gpointer)& ParticleSystem_CUSTOM_SetupDefaultType );
}

#endif
void Register_UnityEngine_ParticleSystemRenderer_get_renderMode()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_renderMode" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropRenderMode );
}

void Register_UnityEngine_ParticleSystemRenderer_set_renderMode()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_renderMode" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropRenderMode );
}

void Register_UnityEngine_ParticleSystemRenderer_get_lengthScale()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_lengthScale" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropLengthScale );
}

void Register_UnityEngine_ParticleSystemRenderer_set_lengthScale()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_lengthScale" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropLengthScale );
}

void Register_UnityEngine_ParticleSystemRenderer_get_velocityScale()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_velocityScale" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropVelocityScale );
}

void Register_UnityEngine_ParticleSystemRenderer_set_velocityScale()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_velocityScale" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropVelocityScale );
}

void Register_UnityEngine_ParticleSystemRenderer_get_cameraVelocityScale()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_cameraVelocityScale" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropCameraVelocityScale );
}

void Register_UnityEngine_ParticleSystemRenderer_set_cameraVelocityScale()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_cameraVelocityScale" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropCameraVelocityScale );
}

void Register_UnityEngine_ParticleSystemRenderer_get_maxParticleSize()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_maxParticleSize" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropMaxParticleSize );
}

void Register_UnityEngine_ParticleSystemRenderer_set_maxParticleSize()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_maxParticleSize" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropMaxParticleSize );
}

void Register_UnityEngine_ParticleSystemRenderer_get_mesh()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_mesh" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropMesh );
}

void Register_UnityEngine_ParticleSystemRenderer_set_mesh()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_mesh" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropMesh );
}

#if UNITY_EDITOR
void Register_UnityEngine_ParticleSystemRenderer_get_editorEnabled()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::get_editorEnabled" , (gpointer)& ParticleSystemRenderer_Get_Custom_PropEditorEnabled );
}

void Register_UnityEngine_ParticleSystemRenderer_set_editorEnabled()
{
    scripting_add_internal_call( "UnityEngine.ParticleSystemRenderer::set_editorEnabled" , (gpointer)& ParticleSystemRenderer_Set_Custom_PropEditorEnabled );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_ParticleSystem_IcallNames [] =
{
#if ENABLE_PHYSICS
    "UnityEngine.ParticleSystem::InstanceIDToCollider",    // -> ParticleSystem_CUSTOM_InstanceIDToCollider
#endif
    "UnityEngine.ParticleSystem::get_startDelay",    // -> ParticleSystem_Get_Custom_PropStartDelay
    "UnityEngine.ParticleSystem::set_startDelay",    // -> ParticleSystem_Set_Custom_PropStartDelay
    "UnityEngine.ParticleSystem::get_isPlaying",    // -> ParticleSystem_Get_Custom_PropIsPlaying
    "UnityEngine.ParticleSystem::get_isStopped",    // -> ParticleSystem_Get_Custom_PropIsStopped
    "UnityEngine.ParticleSystem::get_isPaused",    // -> ParticleSystem_Get_Custom_PropIsPaused
    "UnityEngine.ParticleSystem::get_loop"  ,    // -> ParticleSystem_Get_Custom_PropLoop
    "UnityEngine.ParticleSystem::set_loop"  ,    // -> ParticleSystem_Set_Custom_PropLoop
    "UnityEngine.ParticleSystem::get_playOnAwake",    // -> ParticleSystem_Get_Custom_PropPlayOnAwake
    "UnityEngine.ParticleSystem::set_playOnAwake",    // -> ParticleSystem_Set_Custom_PropPlayOnAwake
    "UnityEngine.ParticleSystem::get_time"  ,    // -> ParticleSystem_Get_Custom_PropTime
    "UnityEngine.ParticleSystem::set_time"  ,    // -> ParticleSystem_Set_Custom_PropTime
    "UnityEngine.ParticleSystem::get_duration",    // -> ParticleSystem_Get_Custom_PropDuration
    "UnityEngine.ParticleSystem::get_playbackSpeed",    // -> ParticleSystem_Get_Custom_PropPlaybackSpeed
    "UnityEngine.ParticleSystem::set_playbackSpeed",    // -> ParticleSystem_Set_Custom_PropPlaybackSpeed
    "UnityEngine.ParticleSystem::get_particleCount",    // -> ParticleSystem_Get_Custom_PropParticleCount
    "UnityEngine.ParticleSystem::get_safeCollisionEventSize",    // -> ParticleSystem_Get_Custom_PropSafeCollisionEventSize
    "UnityEngine.ParticleSystem::get_enableEmission",    // -> ParticleSystem_Get_Custom_PropEnableEmission
    "UnityEngine.ParticleSystem::set_enableEmission",    // -> ParticleSystem_Set_Custom_PropEnableEmission
    "UnityEngine.ParticleSystem::get_emissionRate",    // -> ParticleSystem_Get_Custom_PropEmissionRate
    "UnityEngine.ParticleSystem::set_emissionRate",    // -> ParticleSystem_Set_Custom_PropEmissionRate
    "UnityEngine.ParticleSystem::get_startSpeed",    // -> ParticleSystem_Get_Custom_PropStartSpeed
    "UnityEngine.ParticleSystem::set_startSpeed",    // -> ParticleSystem_Set_Custom_PropStartSpeed
    "UnityEngine.ParticleSystem::get_startSize",    // -> ParticleSystem_Get_Custom_PropStartSize
    "UnityEngine.ParticleSystem::set_startSize",    // -> ParticleSystem_Set_Custom_PropStartSize
    "UnityEngine.ParticleSystem::INTERNAL_get_startColor",    // -> ParticleSystem_CUSTOM_INTERNAL_get_startColor
    "UnityEngine.ParticleSystem::INTERNAL_set_startColor",    // -> ParticleSystem_CUSTOM_INTERNAL_set_startColor
    "UnityEngine.ParticleSystem::get_startRotation",    // -> ParticleSystem_Get_Custom_PropStartRotation
    "UnityEngine.ParticleSystem::set_startRotation",    // -> ParticleSystem_Set_Custom_PropStartRotation
    "UnityEngine.ParticleSystem::get_startLifetime",    // -> ParticleSystem_Get_Custom_PropStartLifetime
    "UnityEngine.ParticleSystem::set_startLifetime",    // -> ParticleSystem_Set_Custom_PropStartLifetime
    "UnityEngine.ParticleSystem::get_gravityModifier",    // -> ParticleSystem_Get_Custom_PropGravityModifier
    "UnityEngine.ParticleSystem::set_gravityModifier",    // -> ParticleSystem_Set_Custom_PropGravityModifier
    "UnityEngine.ParticleSystem::get_randomSeed",    // -> ParticleSystem_Get_Custom_PropRandomSeed
    "UnityEngine.ParticleSystem::set_randomSeed",    // -> ParticleSystem_Set_Custom_PropRandomSeed
    "UnityEngine.ParticleSystem::SetParticles",    // -> ParticleSystem_CUSTOM_SetParticles
    "UnityEngine.ParticleSystem::GetParticles",    // -> ParticleSystem_CUSTOM_GetParticles
    "UnityEngine.ParticleSystem::GetCollisionEvents",    // -> ParticleSystem_CUSTOM_GetCollisionEvents
    "UnityEngine.ParticleSystem::Internal_Simulate",    // -> ParticleSystem_CUSTOM_Internal_Simulate
    "UnityEngine.ParticleSystem::Internal_Play",    // -> ParticleSystem_CUSTOM_Internal_Play
    "UnityEngine.ParticleSystem::Internal_Stop",    // -> ParticleSystem_CUSTOM_Internal_Stop
    "UnityEngine.ParticleSystem::Internal_Pause",    // -> ParticleSystem_CUSTOM_Internal_Pause
    "UnityEngine.ParticleSystem::Internal_Clear",    // -> ParticleSystem_CUSTOM_Internal_Clear
    "UnityEngine.ParticleSystem::Internal_IsAlive",    // -> ParticleSystem_CUSTOM_Internal_IsAlive
    "UnityEngine.ParticleSystem::INTERNAL_CALL_Emit",    // -> ParticleSystem_CUSTOM_INTERNAL_CALL_Emit
    "UnityEngine.ParticleSystem::Internal_Emit",    // -> ParticleSystem_CUSTOM_Internal_Emit
#if UNITY_EDITOR
    "UnityEngine.ParticleSystem::SetupDefaultType",    // -> ParticleSystem_CUSTOM_SetupDefaultType
#endif
    "UnityEngine.ParticleSystemRenderer::get_renderMode",    // -> ParticleSystemRenderer_Get_Custom_PropRenderMode
    "UnityEngine.ParticleSystemRenderer::set_renderMode",    // -> ParticleSystemRenderer_Set_Custom_PropRenderMode
    "UnityEngine.ParticleSystemRenderer::get_lengthScale",    // -> ParticleSystemRenderer_Get_Custom_PropLengthScale
    "UnityEngine.ParticleSystemRenderer::set_lengthScale",    // -> ParticleSystemRenderer_Set_Custom_PropLengthScale
    "UnityEngine.ParticleSystemRenderer::get_velocityScale",    // -> ParticleSystemRenderer_Get_Custom_PropVelocityScale
    "UnityEngine.ParticleSystemRenderer::set_velocityScale",    // -> ParticleSystemRenderer_Set_Custom_PropVelocityScale
    "UnityEngine.ParticleSystemRenderer::get_cameraVelocityScale",    // -> ParticleSystemRenderer_Get_Custom_PropCameraVelocityScale
    "UnityEngine.ParticleSystemRenderer::set_cameraVelocityScale",    // -> ParticleSystemRenderer_Set_Custom_PropCameraVelocityScale
    "UnityEngine.ParticleSystemRenderer::get_maxParticleSize",    // -> ParticleSystemRenderer_Get_Custom_PropMaxParticleSize
    "UnityEngine.ParticleSystemRenderer::set_maxParticleSize",    // -> ParticleSystemRenderer_Set_Custom_PropMaxParticleSize
    "UnityEngine.ParticleSystemRenderer::get_mesh",    // -> ParticleSystemRenderer_Get_Custom_PropMesh
    "UnityEngine.ParticleSystemRenderer::set_mesh",    // -> ParticleSystemRenderer_Set_Custom_PropMesh
#if UNITY_EDITOR
    "UnityEngine.ParticleSystemRenderer::get_editorEnabled",    // -> ParticleSystemRenderer_Get_Custom_PropEditorEnabled
    "UnityEngine.ParticleSystemRenderer::set_editorEnabled",    // -> ParticleSystemRenderer_Set_Custom_PropEditorEnabled
#endif
    NULL
};

static const void* s_ParticleSystem_IcallFuncs [] =
{
#if ENABLE_PHYSICS
    (const void*)&ParticleSystem_CUSTOM_InstanceIDToCollider,  //  <- UnityEngine.ParticleSystem::InstanceIDToCollider
#endif
    (const void*)&ParticleSystem_Get_Custom_PropStartDelay,  //  <- UnityEngine.ParticleSystem::get_startDelay
    (const void*)&ParticleSystem_Set_Custom_PropStartDelay,  //  <- UnityEngine.ParticleSystem::set_startDelay
    (const void*)&ParticleSystem_Get_Custom_PropIsPlaying ,  //  <- UnityEngine.ParticleSystem::get_isPlaying
    (const void*)&ParticleSystem_Get_Custom_PropIsStopped ,  //  <- UnityEngine.ParticleSystem::get_isStopped
    (const void*)&ParticleSystem_Get_Custom_PropIsPaused  ,  //  <- UnityEngine.ParticleSystem::get_isPaused
    (const void*)&ParticleSystem_Get_Custom_PropLoop      ,  //  <- UnityEngine.ParticleSystem::get_loop
    (const void*)&ParticleSystem_Set_Custom_PropLoop      ,  //  <- UnityEngine.ParticleSystem::set_loop
    (const void*)&ParticleSystem_Get_Custom_PropPlayOnAwake,  //  <- UnityEngine.ParticleSystem::get_playOnAwake
    (const void*)&ParticleSystem_Set_Custom_PropPlayOnAwake,  //  <- UnityEngine.ParticleSystem::set_playOnAwake
    (const void*)&ParticleSystem_Get_Custom_PropTime      ,  //  <- UnityEngine.ParticleSystem::get_time
    (const void*)&ParticleSystem_Set_Custom_PropTime      ,  //  <- UnityEngine.ParticleSystem::set_time
    (const void*)&ParticleSystem_Get_Custom_PropDuration  ,  //  <- UnityEngine.ParticleSystem::get_duration
    (const void*)&ParticleSystem_Get_Custom_PropPlaybackSpeed,  //  <- UnityEngine.ParticleSystem::get_playbackSpeed
    (const void*)&ParticleSystem_Set_Custom_PropPlaybackSpeed,  //  <- UnityEngine.ParticleSystem::set_playbackSpeed
    (const void*)&ParticleSystem_Get_Custom_PropParticleCount,  //  <- UnityEngine.ParticleSystem::get_particleCount
    (const void*)&ParticleSystem_Get_Custom_PropSafeCollisionEventSize,  //  <- UnityEngine.ParticleSystem::get_safeCollisionEventSize
    (const void*)&ParticleSystem_Get_Custom_PropEnableEmission,  //  <- UnityEngine.ParticleSystem::get_enableEmission
    (const void*)&ParticleSystem_Set_Custom_PropEnableEmission,  //  <- UnityEngine.ParticleSystem::set_enableEmission
    (const void*)&ParticleSystem_Get_Custom_PropEmissionRate,  //  <- UnityEngine.ParticleSystem::get_emissionRate
    (const void*)&ParticleSystem_Set_Custom_PropEmissionRate,  //  <- UnityEngine.ParticleSystem::set_emissionRate
    (const void*)&ParticleSystem_Get_Custom_PropStartSpeed,  //  <- UnityEngine.ParticleSystem::get_startSpeed
    (const void*)&ParticleSystem_Set_Custom_PropStartSpeed,  //  <- UnityEngine.ParticleSystem::set_startSpeed
    (const void*)&ParticleSystem_Get_Custom_PropStartSize ,  //  <- UnityEngine.ParticleSystem::get_startSize
    (const void*)&ParticleSystem_Set_Custom_PropStartSize ,  //  <- UnityEngine.ParticleSystem::set_startSize
    (const void*)&ParticleSystem_CUSTOM_INTERNAL_get_startColor,  //  <- UnityEngine.ParticleSystem::INTERNAL_get_startColor
    (const void*)&ParticleSystem_CUSTOM_INTERNAL_set_startColor,  //  <- UnityEngine.ParticleSystem::INTERNAL_set_startColor
    (const void*)&ParticleSystem_Get_Custom_PropStartRotation,  //  <- UnityEngine.ParticleSystem::get_startRotation
    (const void*)&ParticleSystem_Set_Custom_PropStartRotation,  //  <- UnityEngine.ParticleSystem::set_startRotation
    (const void*)&ParticleSystem_Get_Custom_PropStartLifetime,  //  <- UnityEngine.ParticleSystem::get_startLifetime
    (const void*)&ParticleSystem_Set_Custom_PropStartLifetime,  //  <- UnityEngine.ParticleSystem::set_startLifetime
    (const void*)&ParticleSystem_Get_Custom_PropGravityModifier,  //  <- UnityEngine.ParticleSystem::get_gravityModifier
    (const void*)&ParticleSystem_Set_Custom_PropGravityModifier,  //  <- UnityEngine.ParticleSystem::set_gravityModifier
    (const void*)&ParticleSystem_Get_Custom_PropRandomSeed,  //  <- UnityEngine.ParticleSystem::get_randomSeed
    (const void*)&ParticleSystem_Set_Custom_PropRandomSeed,  //  <- UnityEngine.ParticleSystem::set_randomSeed
    (const void*)&ParticleSystem_CUSTOM_SetParticles      ,  //  <- UnityEngine.ParticleSystem::SetParticles
    (const void*)&ParticleSystem_CUSTOM_GetParticles      ,  //  <- UnityEngine.ParticleSystem::GetParticles
    (const void*)&ParticleSystem_CUSTOM_GetCollisionEvents,  //  <- UnityEngine.ParticleSystem::GetCollisionEvents
    (const void*)&ParticleSystem_CUSTOM_Internal_Simulate ,  //  <- UnityEngine.ParticleSystem::Internal_Simulate
    (const void*)&ParticleSystem_CUSTOM_Internal_Play     ,  //  <- UnityEngine.ParticleSystem::Internal_Play
    (const void*)&ParticleSystem_CUSTOM_Internal_Stop     ,  //  <- UnityEngine.ParticleSystem::Internal_Stop
    (const void*)&ParticleSystem_CUSTOM_Internal_Pause    ,  //  <- UnityEngine.ParticleSystem::Internal_Pause
    (const void*)&ParticleSystem_CUSTOM_Internal_Clear    ,  //  <- UnityEngine.ParticleSystem::Internal_Clear
    (const void*)&ParticleSystem_CUSTOM_Internal_IsAlive  ,  //  <- UnityEngine.ParticleSystem::Internal_IsAlive
    (const void*)&ParticleSystem_CUSTOM_INTERNAL_CALL_Emit,  //  <- UnityEngine.ParticleSystem::INTERNAL_CALL_Emit
    (const void*)&ParticleSystem_CUSTOM_Internal_Emit     ,  //  <- UnityEngine.ParticleSystem::Internal_Emit
#if UNITY_EDITOR
    (const void*)&ParticleSystem_CUSTOM_SetupDefaultType  ,  //  <- UnityEngine.ParticleSystem::SetupDefaultType
#endif
    (const void*)&ParticleSystemRenderer_Get_Custom_PropRenderMode,  //  <- UnityEngine.ParticleSystemRenderer::get_renderMode
    (const void*)&ParticleSystemRenderer_Set_Custom_PropRenderMode,  //  <- UnityEngine.ParticleSystemRenderer::set_renderMode
    (const void*)&ParticleSystemRenderer_Get_Custom_PropLengthScale,  //  <- UnityEngine.ParticleSystemRenderer::get_lengthScale
    (const void*)&ParticleSystemRenderer_Set_Custom_PropLengthScale,  //  <- UnityEngine.ParticleSystemRenderer::set_lengthScale
    (const void*)&ParticleSystemRenderer_Get_Custom_PropVelocityScale,  //  <- UnityEngine.ParticleSystemRenderer::get_velocityScale
    (const void*)&ParticleSystemRenderer_Set_Custom_PropVelocityScale,  //  <- UnityEngine.ParticleSystemRenderer::set_velocityScale
    (const void*)&ParticleSystemRenderer_Get_Custom_PropCameraVelocityScale,  //  <- UnityEngine.ParticleSystemRenderer::get_cameraVelocityScale
    (const void*)&ParticleSystemRenderer_Set_Custom_PropCameraVelocityScale,  //  <- UnityEngine.ParticleSystemRenderer::set_cameraVelocityScale
    (const void*)&ParticleSystemRenderer_Get_Custom_PropMaxParticleSize,  //  <- UnityEngine.ParticleSystemRenderer::get_maxParticleSize
    (const void*)&ParticleSystemRenderer_Set_Custom_PropMaxParticleSize,  //  <- UnityEngine.ParticleSystemRenderer::set_maxParticleSize
    (const void*)&ParticleSystemRenderer_Get_Custom_PropMesh,  //  <- UnityEngine.ParticleSystemRenderer::get_mesh
    (const void*)&ParticleSystemRenderer_Set_Custom_PropMesh,  //  <- UnityEngine.ParticleSystemRenderer::set_mesh
#if UNITY_EDITOR
    (const void*)&ParticleSystemRenderer_Get_Custom_PropEditorEnabled,  //  <- UnityEngine.ParticleSystemRenderer::get_editorEnabled
    (const void*)&ParticleSystemRenderer_Set_Custom_PropEditorEnabled,  //  <- UnityEngine.ParticleSystemRenderer::set_editorEnabled
#endif
    NULL
};

void ExportParticleSystemBindings();
void ExportParticleSystemBindings()
{
    for (int i = 0; s_ParticleSystem_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_ParticleSystem_IcallNames [i], s_ParticleSystem_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportParticleSystemBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_PHYSICS
    SET_METRO_BINDING(ParticleSystem_CUSTOM_InstanceIDToCollider); //  <- UnityEngine.ParticleSystem::InstanceIDToCollider
#endif
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropStartDelay); //  <- UnityEngine.ParticleSystem::get_startDelay
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropStartDelay); //  <- UnityEngine.ParticleSystem::set_startDelay
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropIsPlaying); //  <- UnityEngine.ParticleSystem::get_isPlaying
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropIsStopped); //  <- UnityEngine.ParticleSystem::get_isStopped
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropIsPaused); //  <- UnityEngine.ParticleSystem::get_isPaused
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropLoop); //  <- UnityEngine.ParticleSystem::get_loop
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropLoop); //  <- UnityEngine.ParticleSystem::set_loop
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropPlayOnAwake); //  <- UnityEngine.ParticleSystem::get_playOnAwake
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropPlayOnAwake); //  <- UnityEngine.ParticleSystem::set_playOnAwake
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropTime); //  <- UnityEngine.ParticleSystem::get_time
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropTime); //  <- UnityEngine.ParticleSystem::set_time
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropDuration); //  <- UnityEngine.ParticleSystem::get_duration
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropPlaybackSpeed); //  <- UnityEngine.ParticleSystem::get_playbackSpeed
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropPlaybackSpeed); //  <- UnityEngine.ParticleSystem::set_playbackSpeed
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropParticleCount); //  <- UnityEngine.ParticleSystem::get_particleCount
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropSafeCollisionEventSize); //  <- UnityEngine.ParticleSystem::get_safeCollisionEventSize
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropEnableEmission); //  <- UnityEngine.ParticleSystem::get_enableEmission
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropEnableEmission); //  <- UnityEngine.ParticleSystem::set_enableEmission
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropEmissionRate); //  <- UnityEngine.ParticleSystem::get_emissionRate
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropEmissionRate); //  <- UnityEngine.ParticleSystem::set_emissionRate
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropStartSpeed); //  <- UnityEngine.ParticleSystem::get_startSpeed
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropStartSpeed); //  <- UnityEngine.ParticleSystem::set_startSpeed
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropStartSize); //  <- UnityEngine.ParticleSystem::get_startSize
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropStartSize); //  <- UnityEngine.ParticleSystem::set_startSize
    SET_METRO_BINDING(ParticleSystem_CUSTOM_INTERNAL_get_startColor); //  <- UnityEngine.ParticleSystem::INTERNAL_get_startColor
    SET_METRO_BINDING(ParticleSystem_CUSTOM_INTERNAL_set_startColor); //  <- UnityEngine.ParticleSystem::INTERNAL_set_startColor
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropStartRotation); //  <- UnityEngine.ParticleSystem::get_startRotation
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropStartRotation); //  <- UnityEngine.ParticleSystem::set_startRotation
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropStartLifetime); //  <- UnityEngine.ParticleSystem::get_startLifetime
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropStartLifetime); //  <- UnityEngine.ParticleSystem::set_startLifetime
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropGravityModifier); //  <- UnityEngine.ParticleSystem::get_gravityModifier
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropGravityModifier); //  <- UnityEngine.ParticleSystem::set_gravityModifier
    SET_METRO_BINDING(ParticleSystem_Get_Custom_PropRandomSeed); //  <- UnityEngine.ParticleSystem::get_randomSeed
    SET_METRO_BINDING(ParticleSystem_Set_Custom_PropRandomSeed); //  <- UnityEngine.ParticleSystem::set_randomSeed
    SET_METRO_BINDING(ParticleSystem_CUSTOM_SetParticles); //  <- UnityEngine.ParticleSystem::SetParticles
    SET_METRO_BINDING(ParticleSystem_CUSTOM_GetParticles); //  <- UnityEngine.ParticleSystem::GetParticles
    SET_METRO_BINDING(ParticleSystem_CUSTOM_GetCollisionEvents); //  <- UnityEngine.ParticleSystem::GetCollisionEvents
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_Simulate); //  <- UnityEngine.ParticleSystem::Internal_Simulate
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_Play); //  <- UnityEngine.ParticleSystem::Internal_Play
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_Stop); //  <- UnityEngine.ParticleSystem::Internal_Stop
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_Pause); //  <- UnityEngine.ParticleSystem::Internal_Pause
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_Clear); //  <- UnityEngine.ParticleSystem::Internal_Clear
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_IsAlive); //  <- UnityEngine.ParticleSystem::Internal_IsAlive
    SET_METRO_BINDING(ParticleSystem_CUSTOM_INTERNAL_CALL_Emit); //  <- UnityEngine.ParticleSystem::INTERNAL_CALL_Emit
    SET_METRO_BINDING(ParticleSystem_CUSTOM_Internal_Emit); //  <- UnityEngine.ParticleSystem::Internal_Emit
#if UNITY_EDITOR
    SET_METRO_BINDING(ParticleSystem_CUSTOM_SetupDefaultType); //  <- UnityEngine.ParticleSystem::SetupDefaultType
#endif
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropRenderMode); //  <- UnityEngine.ParticleSystemRenderer::get_renderMode
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropRenderMode); //  <- UnityEngine.ParticleSystemRenderer::set_renderMode
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropLengthScale); //  <- UnityEngine.ParticleSystemRenderer::get_lengthScale
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropLengthScale); //  <- UnityEngine.ParticleSystemRenderer::set_lengthScale
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropVelocityScale); //  <- UnityEngine.ParticleSystemRenderer::get_velocityScale
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropVelocityScale); //  <- UnityEngine.ParticleSystemRenderer::set_velocityScale
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropCameraVelocityScale); //  <- UnityEngine.ParticleSystemRenderer::get_cameraVelocityScale
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropCameraVelocityScale); //  <- UnityEngine.ParticleSystemRenderer::set_cameraVelocityScale
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropMaxParticleSize); //  <- UnityEngine.ParticleSystemRenderer::get_maxParticleSize
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropMaxParticleSize); //  <- UnityEngine.ParticleSystemRenderer::set_maxParticleSize
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropMesh); //  <- UnityEngine.ParticleSystemRenderer::get_mesh
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropMesh); //  <- UnityEngine.ParticleSystemRenderer::set_mesh
#if UNITY_EDITOR
    SET_METRO_BINDING(ParticleSystemRenderer_Get_Custom_PropEditorEnabled); //  <- UnityEngine.ParticleSystemRenderer::get_editorEnabled
    SET_METRO_BINDING(ParticleSystemRenderer_Set_Custom_PropEditorEnabled); //  <- UnityEngine.ParticleSystemRenderer::set_editorEnabled
#endif
}

#endif
