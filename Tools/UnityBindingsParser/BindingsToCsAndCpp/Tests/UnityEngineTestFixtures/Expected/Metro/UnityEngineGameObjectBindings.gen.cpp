#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/BaseClasses/Tags.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Export/GameObjectExport.h"
#if ENABLE_AUDIO
#include "Runtime/Audio/AudioSource.h"
#endif

#include "Runtime/Animation/Animation.h"
#include "Runtime/Animation/AnimationUtility.h"
#include "Runtime/Camera/Camera.h"

#if ENABLE_PHYSICS
#include "Runtime/Dynamics/Rigidbody.h"
#include "Runtime/Dynamics/ConstantForce.h"
#include "Runtime/Dynamics/HingeJoint.h"
#include "Runtime/Dynamics/Collider.h"
#endif
#include "Runtime/Dynamics/2D/RigidBody2D.h"
#include "Runtime/Dynamics/2D/Collider2D.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Filters/Renderer.h"
#if ENABLE_NETWORK
#include "Runtime/Network/NetworkView.h"
#endif
#include "Runtime/Camera/RenderLayers/GUIText.h"
#include "Runtime/Camera/RenderLayers/GUITexture.h"
#include "Runtime/ParticleSystem/ParticleSystem.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/Misc/GOCreation.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/GetComponent.h"

#if WEBPLUG
	#include "PlatformDependent/CommonWebPlugin/UnityWebStream.h"
	#include "PlatformDependent/CommonWebPlugin/WebScripting.h"
#endif

#if UNITY_EDITOR
	#include "Editor/Src/EditorSettings.h"
	#include "Editor/Src/EditorUserBuildSettings.h"
	#include "Editor/Mono/MonoEditorUtility.h"
#endif

using namespace Unity;

 

class AudioSource;
void PauseEditor ();
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_CreatePrimitive(int type)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_CreatePrimitive)
    SCRIPTINGAPI_STACK_CHECK(CreatePrimitive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CreatePrimitive)
     
    		return Scripting::ScriptingWrapperFor(CreatePrimitive(type)); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_GetComponent(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_GetComponent)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(GetComponent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetComponent)
     return ScriptingGetComponentOfType (*self, type); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_GetComponentByName(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_GetComponentByName)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(GetComponentByName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetComponentByName)
     return GetScriptingWrapperOfComponentOfGameObject (*self, type); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropIsStatic(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropIsStatic)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isStatic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isStatic)
    return self->GetIsStatic ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_Set_Custom_PropIsStatic(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Set_Custom_PropIsStatic)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_isStatic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isStatic)
    
    self->SetIsStatic (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropIsStaticBatchable(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropIsStaticBatchable)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isStaticBatchable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isStaticBatchable)
    return self->IsStaticBatchable ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_GetComponentsInternal(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument type_, ScriptingBool isGenericTypeArray, ScriptingBool recursive, ScriptingBool includeInactive)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_GetComponentsInternal)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(GetComponentsInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetComponentsInternal)
     
    		return ScriptingGetComponentsOfType (*self, type, isGenericTypeArray, recursive, includeInactive);
    	
}


 #define FASTGO_QUERY_COMPONENT(x) GameObject& go = *self; return GetComponentScripting::ScriptingWrapperFor (go.QueryComponent (x), go, ClassID (x));

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropTransform(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropTransform)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_transform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_transform)
    FASTGO_QUERY_COMPONENT(Transform)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropRigidbody(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropRigidbody)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rigidbody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rigidbody)
    FASTGO_QUERY_COMPONENT(Rigidbody)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropRigidbody2D(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropRigidbody2D)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rigidbody2D)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rigidbody2D)
    FASTGO_QUERY_COMPONENT(Rigidbody2D)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropCamera(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropCamera)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_camera)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_camera)
    FASTGO_QUERY_COMPONENT(Camera)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropLight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropLight)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_light)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_light)
    FASTGO_QUERY_COMPONENT(Light)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropAnimation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropAnimation)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_animation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_animation)
    FASTGO_QUERY_COMPONENT(Animation)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropConstantForce(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropConstantForce)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_constantForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_constantForce)
    FASTGO_QUERY_COMPONENT(ConstantForce)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropRenderer(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropRenderer)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderer)
    FASTGO_QUERY_COMPONENT(Renderer)
}

#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropAudio(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropAudio)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_audio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_audio)
    FASTGO_QUERY_COMPONENT(AudioSource)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropGuiText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropGuiText)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_guiText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_guiText)
    FASTGO_QUERY_COMPONENT(GUIText)
}

#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropNetworkView(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropNetworkView)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_networkView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_networkView)
    #if ENABLE_NETWORK
    FASTGO_QUERY_COMPONENT(NetworkView)
    #else
    return SCRIPTING_NULL;
    #endif
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropGuiElement(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropGuiElement)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_guiElement)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_guiElement)
    FASTGO_QUERY_COMPONENT(GUIElement)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropGuiTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropGuiTexture)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_guiTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_guiTexture)
    FASTGO_QUERY_COMPONENT(GUITexture)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropCollider(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropCollider)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collider)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collider)
    FASTGO_QUERY_COMPONENT(Collider)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropCollider2D(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropCollider2D)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collider2D)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collider2D)
    FASTGO_QUERY_COMPONENT(Collider2D)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropHingeJoint(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropHingeJoint)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hingeJoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hingeJoint)
    FASTGO_QUERY_COMPONENT(HingeJoint)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropParticleSystem(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropParticleSystem)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_particleSystem)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_particleSystem)
    FASTGO_QUERY_COMPONENT(ParticleSystem)
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropLayer(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropLayer)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_layer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_layer)
    return self->GetLayer ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_Set_Custom_PropLayer(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Set_Custom_PropLayer)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_layer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_layer)
    
    self->SetLayer (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_active)
    return self->IsActive ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_Set_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Set_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_active)
     if (value) self->Activate (); else self->Deactivate (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_SetActive(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_SetActive)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetActive)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetActive)
    
    		if (value) self->Activate (); else self->Deactivate ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropActiveSelf(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropActiveSelf)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_activeSelf)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_activeSelf)
    return self->IsSelfActive ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropActiveInHierarchy(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropActiveInHierarchy)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_activeInHierarchy)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_activeInHierarchy)
    return self->IsActive ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_SetActiveRecursively(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool state)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_SetActiveRecursively)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetActiveRecursively)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetActiveRecursively)
    
    		self->SetActiveRecursively (state);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GameObject_Get_Custom_PropTag(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Get_Custom_PropTag)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_tag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_tag)
    const string& tag = TagToString (self->GetTag ());
    if (!tag.empty ())
    return CreateScriptingString (tag);
    else
    {
    RaiseMonoException ("GameObject has undefined tag!");
    return SCRIPTING_NULL;
    }
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_Set_Custom_PropTag(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_Set_Custom_PropTag)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_tag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_tag)
    
    self->SetTag (ExtractTagThrowing (value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameObject_CUSTOM_CompareTag(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument tag_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_CompareTag)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local tag(tag_);
    UNUSED(tag);
    SCRIPTINGAPI_STACK_CHECK(CompareTag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CompareTag)
    
    		return ExtractTagThrowing (tag) == self->GetTag ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_FindGameObjectWithTag(ICallType_String_Argument tag_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_FindGameObjectWithTag)
    ICallType_String_Local tag(tag_);
    UNUSED(tag);
    SCRIPTINGAPI_STACK_CHECK(FindGameObjectWithTag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindGameObjectWithTag)
    
    		int tagInt = ExtractTagThrowing (tag);
    		return Scripting::ScriptingWrapperFor(FindGameObjectWithTag (tagInt));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_FindGameObjectsWithTag(ICallType_String_Argument tag_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_FindGameObjectsWithTag)
    ICallType_String_Local tag(tag_);
    UNUSED(tag);
    SCRIPTINGAPI_STACK_CHECK(FindGameObjectsWithTag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindGameObjectsWithTag)
    
    		static vector<GameObject*> go; go.clear ();
    		int tagInt = ExtractTagThrowing (tag);
    		FindGameObjectsWithTag (tagInt, go);
    		ScriptingClassPtr goClass = ClassIDToScriptingType (ClassID (GameObject));
    		return CreateScriptingArrayFromUnityObjects(go,goClass);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_SendMessageUpwards(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument value_, int options)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_SendMessageUpwards)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(SendMessageUpwards)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SendMessageUpwards)
    
    		Scripting::SendScriptingMessageUpwards (*self, methodName, value, options);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_SendMessage(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument value_, int options)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_SendMessage)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(SendMessage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SendMessage)
    
    		SendMonoMessage (*self, methodName, value, options);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_BroadcastMessage(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument parameter_, int options)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_BroadcastMessage)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local parameter(parameter_);
    UNUSED(parameter);
    SCRIPTINGAPI_STACK_CHECK(BroadcastMessage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BroadcastMessage)
    
    		Scripting::BroadcastScriptingMessage(*self, methodName, parameter, options);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_AddComponent(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument className_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_AddComponent)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ICallType_String_Local className(className_);
    UNUSED(className);
    SCRIPTINGAPI_STACK_CHECK(AddComponent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddComponent)
     return MonoAddComponent (*self, className.ToUTF8().c_str()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_Internal_AddComponentWithType(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument componentType_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_Internal_AddComponentWithType)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr componentType(componentType_);
    UNUSED(componentType);
    SCRIPTINGAPI_STACK_CHECK(Internal_AddComponentWithType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_AddComponentWithType)
    
    		return MonoAddComponentWithType(*self, componentType);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_Internal_CreateGameObject(ICallType_Object_Argument mono_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_Internal_CreateGameObject)
    ScriptingObjectOfType<GameObject> mono(mono_);
    UNUSED(mono);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateGameObject)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateGameObject)
    
    		GameObject* go = MonoCreateGameObject (name.IsNull() ? NULL : name.ToUTF8().c_str() ); 
    		ConnectScriptingWrapperToObject (mono.GetScriptingObject(), go); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_SampleAnimation(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument animation_, float time)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_SampleAnimation)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AnimationClip> animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(SampleAnimation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SampleAnimation)
     SampleAnimation (*self, *animation, time, animation->GetWrapMode()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_PlayAnimation(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument animation_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_PlayAnimation)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AnimationClip> animation(animation_);
    UNUSED(animation);
    SCRIPTINGAPI_STACK_CHECK(PlayAnimation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PlayAnimation)
     ErrorString("gameObject.PlayAnimation(); is not supported anymore. Use animation.Stop();"); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameObject_CUSTOM_StopAnimation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_StopAnimation)
    ReadOnlyScriptingObjectOfType<GameObject> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(StopAnimation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopAnimation)
     ErrorString("gameObject.StopAnimation(); is not supported anymore. Use animation.Stop();"); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameObject_CUSTOM_Find(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameObject_CUSTOM_Find)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Find)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Find)
    
    		Transform* transform = FindActiveTransformWithPath (name.ToUTF8().c_str ());
    		GameObject* go = NULL;
    		if (transform)
    			go = transform->GetGameObjectPtr ();
    		return Scripting::ScriptingWrapperFor (go);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_GameObject_CreatePrimitive()
{
    scripting_add_internal_call( "UnityEngine.GameObject::CreatePrimitive" , (gpointer)& GameObject_CUSTOM_CreatePrimitive );
}

void Register_UnityEngine_GameObject_GetComponent()
{
    scripting_add_internal_call( "UnityEngine.GameObject::GetComponent" , (gpointer)& GameObject_CUSTOM_GetComponent );
}

void Register_UnityEngine_GameObject_GetComponentByName()
{
    scripting_add_internal_call( "UnityEngine.GameObject::GetComponentByName" , (gpointer)& GameObject_CUSTOM_GetComponentByName );
}

void Register_UnityEngine_GameObject_get_isStatic()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_isStatic" , (gpointer)& GameObject_Get_Custom_PropIsStatic );
}

void Register_UnityEngine_GameObject_set_isStatic()
{
    scripting_add_internal_call( "UnityEngine.GameObject::set_isStatic" , (gpointer)& GameObject_Set_Custom_PropIsStatic );
}

void Register_UnityEngine_GameObject_get_isStaticBatchable()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_isStaticBatchable" , (gpointer)& GameObject_Get_Custom_PropIsStaticBatchable );
}

void Register_UnityEngine_GameObject_GetComponentsInternal()
{
    scripting_add_internal_call( "UnityEngine.GameObject::GetComponentsInternal" , (gpointer)& GameObject_CUSTOM_GetComponentsInternal );
}

void Register_UnityEngine_GameObject_get_transform()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_transform" , (gpointer)& GameObject_Get_Custom_PropTransform );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_GameObject_get_rigidbody()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_rigidbody" , (gpointer)& GameObject_Get_Custom_PropRigidbody );
}

#endif
void Register_UnityEngine_GameObject_get_rigidbody2D()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_rigidbody2D" , (gpointer)& GameObject_Get_Custom_PropRigidbody2D );
}

void Register_UnityEngine_GameObject_get_camera()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_camera" , (gpointer)& GameObject_Get_Custom_PropCamera );
}

void Register_UnityEngine_GameObject_get_light()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_light" , (gpointer)& GameObject_Get_Custom_PropLight );
}

void Register_UnityEngine_GameObject_get_animation()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_animation" , (gpointer)& GameObject_Get_Custom_PropAnimation );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_GameObject_get_constantForce()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_constantForce" , (gpointer)& GameObject_Get_Custom_PropConstantForce );
}

#endif
void Register_UnityEngine_GameObject_get_renderer()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_renderer" , (gpointer)& GameObject_Get_Custom_PropRenderer );
}

#if ENABLE_AUDIO
void Register_UnityEngine_GameObject_get_audio()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_audio" , (gpointer)& GameObject_Get_Custom_PropAudio );
}

#endif
void Register_UnityEngine_GameObject_get_guiText()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_guiText" , (gpointer)& GameObject_Get_Custom_PropGuiText );
}

#if ENABLE_NETWORK
void Register_UnityEngine_GameObject_get_networkView()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_networkView" , (gpointer)& GameObject_Get_Custom_PropNetworkView );
}

#endif
void Register_UnityEngine_GameObject_get_guiElement()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_guiElement" , (gpointer)& GameObject_Get_Custom_PropGuiElement );
}

void Register_UnityEngine_GameObject_get_guiTexture()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_guiTexture" , (gpointer)& GameObject_Get_Custom_PropGuiTexture );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_GameObject_get_collider()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_collider" , (gpointer)& GameObject_Get_Custom_PropCollider );
}

#endif
void Register_UnityEngine_GameObject_get_collider2D()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_collider2D" , (gpointer)& GameObject_Get_Custom_PropCollider2D );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_GameObject_get_hingeJoint()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_hingeJoint" , (gpointer)& GameObject_Get_Custom_PropHingeJoint );
}

#endif
void Register_UnityEngine_GameObject_get_particleSystem()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_particleSystem" , (gpointer)& GameObject_Get_Custom_PropParticleSystem );
}

void Register_UnityEngine_GameObject_get_layer()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_layer" , (gpointer)& GameObject_Get_Custom_PropLayer );
}

void Register_UnityEngine_GameObject_set_layer()
{
    scripting_add_internal_call( "UnityEngine.GameObject::set_layer" , (gpointer)& GameObject_Set_Custom_PropLayer );
}

void Register_UnityEngine_GameObject_get_active()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_active" , (gpointer)& GameObject_Get_Custom_PropActive );
}

void Register_UnityEngine_GameObject_set_active()
{
    scripting_add_internal_call( "UnityEngine.GameObject::set_active" , (gpointer)& GameObject_Set_Custom_PropActive );
}

void Register_UnityEngine_GameObject_SetActive()
{
    scripting_add_internal_call( "UnityEngine.GameObject::SetActive" , (gpointer)& GameObject_CUSTOM_SetActive );
}

void Register_UnityEngine_GameObject_get_activeSelf()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_activeSelf" , (gpointer)& GameObject_Get_Custom_PropActiveSelf );
}

void Register_UnityEngine_GameObject_get_activeInHierarchy()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_activeInHierarchy" , (gpointer)& GameObject_Get_Custom_PropActiveInHierarchy );
}

void Register_UnityEngine_GameObject_SetActiveRecursively()
{
    scripting_add_internal_call( "UnityEngine.GameObject::SetActiveRecursively" , (gpointer)& GameObject_CUSTOM_SetActiveRecursively );
}

void Register_UnityEngine_GameObject_get_tag()
{
    scripting_add_internal_call( "UnityEngine.GameObject::get_tag" , (gpointer)& GameObject_Get_Custom_PropTag );
}

void Register_UnityEngine_GameObject_set_tag()
{
    scripting_add_internal_call( "UnityEngine.GameObject::set_tag" , (gpointer)& GameObject_Set_Custom_PropTag );
}

void Register_UnityEngine_GameObject_CompareTag()
{
    scripting_add_internal_call( "UnityEngine.GameObject::CompareTag" , (gpointer)& GameObject_CUSTOM_CompareTag );
}

void Register_UnityEngine_GameObject_FindGameObjectWithTag()
{
    scripting_add_internal_call( "UnityEngine.GameObject::FindGameObjectWithTag" , (gpointer)& GameObject_CUSTOM_FindGameObjectWithTag );
}

void Register_UnityEngine_GameObject_FindGameObjectsWithTag()
{
    scripting_add_internal_call( "UnityEngine.GameObject::FindGameObjectsWithTag" , (gpointer)& GameObject_CUSTOM_FindGameObjectsWithTag );
}

void Register_UnityEngine_GameObject_SendMessageUpwards()
{
    scripting_add_internal_call( "UnityEngine.GameObject::SendMessageUpwards" , (gpointer)& GameObject_CUSTOM_SendMessageUpwards );
}

void Register_UnityEngine_GameObject_SendMessage()
{
    scripting_add_internal_call( "UnityEngine.GameObject::SendMessage" , (gpointer)& GameObject_CUSTOM_SendMessage );
}

void Register_UnityEngine_GameObject_BroadcastMessage()
{
    scripting_add_internal_call( "UnityEngine.GameObject::BroadcastMessage" , (gpointer)& GameObject_CUSTOM_BroadcastMessage );
}

void Register_UnityEngine_GameObject_AddComponent()
{
    scripting_add_internal_call( "UnityEngine.GameObject::AddComponent" , (gpointer)& GameObject_CUSTOM_AddComponent );
}

void Register_UnityEngine_GameObject_Internal_AddComponentWithType()
{
    scripting_add_internal_call( "UnityEngine.GameObject::Internal_AddComponentWithType" , (gpointer)& GameObject_CUSTOM_Internal_AddComponentWithType );
}

void Register_UnityEngine_GameObject_Internal_CreateGameObject()
{
    scripting_add_internal_call( "UnityEngine.GameObject::Internal_CreateGameObject" , (gpointer)& GameObject_CUSTOM_Internal_CreateGameObject );
}

void Register_UnityEngine_GameObject_SampleAnimation()
{
    scripting_add_internal_call( "UnityEngine.GameObject::SampleAnimation" , (gpointer)& GameObject_CUSTOM_SampleAnimation );
}

void Register_UnityEngine_GameObject_PlayAnimation()
{
    scripting_add_internal_call( "UnityEngine.GameObject::PlayAnimation" , (gpointer)& GameObject_CUSTOM_PlayAnimation );
}

void Register_UnityEngine_GameObject_StopAnimation()
{
    scripting_add_internal_call( "UnityEngine.GameObject::StopAnimation" , (gpointer)& GameObject_CUSTOM_StopAnimation );
}

void Register_UnityEngine_GameObject_Find()
{
    scripting_add_internal_call( "UnityEngine.GameObject::Find" , (gpointer)& GameObject_CUSTOM_Find );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineGameObject_IcallNames [] =
{
    "UnityEngine.GameObject::CreatePrimitive",    // -> GameObject_CUSTOM_CreatePrimitive
    "UnityEngine.GameObject::GetComponent"  ,    // -> GameObject_CUSTOM_GetComponent
    "UnityEngine.GameObject::GetComponentByName",    // -> GameObject_CUSTOM_GetComponentByName
    "UnityEngine.GameObject::get_isStatic"  ,    // -> GameObject_Get_Custom_PropIsStatic
    "UnityEngine.GameObject::set_isStatic"  ,    // -> GameObject_Set_Custom_PropIsStatic
    "UnityEngine.GameObject::get_isStaticBatchable",    // -> GameObject_Get_Custom_PropIsStaticBatchable
    "UnityEngine.GameObject::GetComponentsInternal",    // -> GameObject_CUSTOM_GetComponentsInternal
    "UnityEngine.GameObject::get_transform" ,    // -> GameObject_Get_Custom_PropTransform
#if ENABLE_PHYSICS
    "UnityEngine.GameObject::get_rigidbody" ,    // -> GameObject_Get_Custom_PropRigidbody
#endif
    "UnityEngine.GameObject::get_rigidbody2D",    // -> GameObject_Get_Custom_PropRigidbody2D
    "UnityEngine.GameObject::get_camera"    ,    // -> GameObject_Get_Custom_PropCamera
    "UnityEngine.GameObject::get_light"     ,    // -> GameObject_Get_Custom_PropLight
    "UnityEngine.GameObject::get_animation" ,    // -> GameObject_Get_Custom_PropAnimation
#if ENABLE_PHYSICS
    "UnityEngine.GameObject::get_constantForce",    // -> GameObject_Get_Custom_PropConstantForce
#endif
    "UnityEngine.GameObject::get_renderer"  ,    // -> GameObject_Get_Custom_PropRenderer
#if ENABLE_AUDIO
    "UnityEngine.GameObject::get_audio"     ,    // -> GameObject_Get_Custom_PropAudio
#endif
    "UnityEngine.GameObject::get_guiText"   ,    // -> GameObject_Get_Custom_PropGuiText
#if ENABLE_NETWORK
    "UnityEngine.GameObject::get_networkView",    // -> GameObject_Get_Custom_PropNetworkView
#endif
    "UnityEngine.GameObject::get_guiElement",    // -> GameObject_Get_Custom_PropGuiElement
    "UnityEngine.GameObject::get_guiTexture",    // -> GameObject_Get_Custom_PropGuiTexture
#if ENABLE_PHYSICS
    "UnityEngine.GameObject::get_collider"  ,    // -> GameObject_Get_Custom_PropCollider
#endif
    "UnityEngine.GameObject::get_collider2D",    // -> GameObject_Get_Custom_PropCollider2D
#if ENABLE_PHYSICS
    "UnityEngine.GameObject::get_hingeJoint",    // -> GameObject_Get_Custom_PropHingeJoint
#endif
    "UnityEngine.GameObject::get_particleSystem",    // -> GameObject_Get_Custom_PropParticleSystem
    "UnityEngine.GameObject::get_layer"     ,    // -> GameObject_Get_Custom_PropLayer
    "UnityEngine.GameObject::set_layer"     ,    // -> GameObject_Set_Custom_PropLayer
    "UnityEngine.GameObject::get_active"    ,    // -> GameObject_Get_Custom_PropActive
    "UnityEngine.GameObject::set_active"    ,    // -> GameObject_Set_Custom_PropActive
    "UnityEngine.GameObject::SetActive"     ,    // -> GameObject_CUSTOM_SetActive
    "UnityEngine.GameObject::get_activeSelf",    // -> GameObject_Get_Custom_PropActiveSelf
    "UnityEngine.GameObject::get_activeInHierarchy",    // -> GameObject_Get_Custom_PropActiveInHierarchy
    "UnityEngine.GameObject::SetActiveRecursively",    // -> GameObject_CUSTOM_SetActiveRecursively
    "UnityEngine.GameObject::get_tag"       ,    // -> GameObject_Get_Custom_PropTag
    "UnityEngine.GameObject::set_tag"       ,    // -> GameObject_Set_Custom_PropTag
    "UnityEngine.GameObject::CompareTag"    ,    // -> GameObject_CUSTOM_CompareTag
    "UnityEngine.GameObject::FindGameObjectWithTag",    // -> GameObject_CUSTOM_FindGameObjectWithTag
    "UnityEngine.GameObject::FindGameObjectsWithTag",    // -> GameObject_CUSTOM_FindGameObjectsWithTag
    "UnityEngine.GameObject::SendMessageUpwards",    // -> GameObject_CUSTOM_SendMessageUpwards
    "UnityEngine.GameObject::SendMessage"   ,    // -> GameObject_CUSTOM_SendMessage
    "UnityEngine.GameObject::BroadcastMessage",    // -> GameObject_CUSTOM_BroadcastMessage
    "UnityEngine.GameObject::AddComponent"  ,    // -> GameObject_CUSTOM_AddComponent
    "UnityEngine.GameObject::Internal_AddComponentWithType",    // -> GameObject_CUSTOM_Internal_AddComponentWithType
    "UnityEngine.GameObject::Internal_CreateGameObject",    // -> GameObject_CUSTOM_Internal_CreateGameObject
    "UnityEngine.GameObject::SampleAnimation",    // -> GameObject_CUSTOM_SampleAnimation
    "UnityEngine.GameObject::PlayAnimation" ,    // -> GameObject_CUSTOM_PlayAnimation
    "UnityEngine.GameObject::StopAnimation" ,    // -> GameObject_CUSTOM_StopAnimation
    "UnityEngine.GameObject::Find"          ,    // -> GameObject_CUSTOM_Find
    NULL
};

static const void* s_UnityEngineGameObject_IcallFuncs [] =
{
    (const void*)&GameObject_CUSTOM_CreatePrimitive       ,  //  <- UnityEngine.GameObject::CreatePrimitive
    (const void*)&GameObject_CUSTOM_GetComponent          ,  //  <- UnityEngine.GameObject::GetComponent
    (const void*)&GameObject_CUSTOM_GetComponentByName    ,  //  <- UnityEngine.GameObject::GetComponentByName
    (const void*)&GameObject_Get_Custom_PropIsStatic      ,  //  <- UnityEngine.GameObject::get_isStatic
    (const void*)&GameObject_Set_Custom_PropIsStatic      ,  //  <- UnityEngine.GameObject::set_isStatic
    (const void*)&GameObject_Get_Custom_PropIsStaticBatchable,  //  <- UnityEngine.GameObject::get_isStaticBatchable
    (const void*)&GameObject_CUSTOM_GetComponentsInternal ,  //  <- UnityEngine.GameObject::GetComponentsInternal
    (const void*)&GameObject_Get_Custom_PropTransform     ,  //  <- UnityEngine.GameObject::get_transform
#if ENABLE_PHYSICS
    (const void*)&GameObject_Get_Custom_PropRigidbody     ,  //  <- UnityEngine.GameObject::get_rigidbody
#endif
    (const void*)&GameObject_Get_Custom_PropRigidbody2D   ,  //  <- UnityEngine.GameObject::get_rigidbody2D
    (const void*)&GameObject_Get_Custom_PropCamera        ,  //  <- UnityEngine.GameObject::get_camera
    (const void*)&GameObject_Get_Custom_PropLight         ,  //  <- UnityEngine.GameObject::get_light
    (const void*)&GameObject_Get_Custom_PropAnimation     ,  //  <- UnityEngine.GameObject::get_animation
#if ENABLE_PHYSICS
    (const void*)&GameObject_Get_Custom_PropConstantForce ,  //  <- UnityEngine.GameObject::get_constantForce
#endif
    (const void*)&GameObject_Get_Custom_PropRenderer      ,  //  <- UnityEngine.GameObject::get_renderer
#if ENABLE_AUDIO
    (const void*)&GameObject_Get_Custom_PropAudio         ,  //  <- UnityEngine.GameObject::get_audio
#endif
    (const void*)&GameObject_Get_Custom_PropGuiText       ,  //  <- UnityEngine.GameObject::get_guiText
#if ENABLE_NETWORK
    (const void*)&GameObject_Get_Custom_PropNetworkView   ,  //  <- UnityEngine.GameObject::get_networkView
#endif
    (const void*)&GameObject_Get_Custom_PropGuiElement    ,  //  <- UnityEngine.GameObject::get_guiElement
    (const void*)&GameObject_Get_Custom_PropGuiTexture    ,  //  <- UnityEngine.GameObject::get_guiTexture
#if ENABLE_PHYSICS
    (const void*)&GameObject_Get_Custom_PropCollider      ,  //  <- UnityEngine.GameObject::get_collider
#endif
    (const void*)&GameObject_Get_Custom_PropCollider2D    ,  //  <- UnityEngine.GameObject::get_collider2D
#if ENABLE_PHYSICS
    (const void*)&GameObject_Get_Custom_PropHingeJoint    ,  //  <- UnityEngine.GameObject::get_hingeJoint
#endif
    (const void*)&GameObject_Get_Custom_PropParticleSystem,  //  <- UnityEngine.GameObject::get_particleSystem
    (const void*)&GameObject_Get_Custom_PropLayer         ,  //  <- UnityEngine.GameObject::get_layer
    (const void*)&GameObject_Set_Custom_PropLayer         ,  //  <- UnityEngine.GameObject::set_layer
    (const void*)&GameObject_Get_Custom_PropActive        ,  //  <- UnityEngine.GameObject::get_active
    (const void*)&GameObject_Set_Custom_PropActive        ,  //  <- UnityEngine.GameObject::set_active
    (const void*)&GameObject_CUSTOM_SetActive             ,  //  <- UnityEngine.GameObject::SetActive
    (const void*)&GameObject_Get_Custom_PropActiveSelf    ,  //  <- UnityEngine.GameObject::get_activeSelf
    (const void*)&GameObject_Get_Custom_PropActiveInHierarchy,  //  <- UnityEngine.GameObject::get_activeInHierarchy
    (const void*)&GameObject_CUSTOM_SetActiveRecursively  ,  //  <- UnityEngine.GameObject::SetActiveRecursively
    (const void*)&GameObject_Get_Custom_PropTag           ,  //  <- UnityEngine.GameObject::get_tag
    (const void*)&GameObject_Set_Custom_PropTag           ,  //  <- UnityEngine.GameObject::set_tag
    (const void*)&GameObject_CUSTOM_CompareTag            ,  //  <- UnityEngine.GameObject::CompareTag
    (const void*)&GameObject_CUSTOM_FindGameObjectWithTag ,  //  <- UnityEngine.GameObject::FindGameObjectWithTag
    (const void*)&GameObject_CUSTOM_FindGameObjectsWithTag,  //  <- UnityEngine.GameObject::FindGameObjectsWithTag
    (const void*)&GameObject_CUSTOM_SendMessageUpwards    ,  //  <- UnityEngine.GameObject::SendMessageUpwards
    (const void*)&GameObject_CUSTOM_SendMessage           ,  //  <- UnityEngine.GameObject::SendMessage
    (const void*)&GameObject_CUSTOM_BroadcastMessage      ,  //  <- UnityEngine.GameObject::BroadcastMessage
    (const void*)&GameObject_CUSTOM_AddComponent          ,  //  <- UnityEngine.GameObject::AddComponent
    (const void*)&GameObject_CUSTOM_Internal_AddComponentWithType,  //  <- UnityEngine.GameObject::Internal_AddComponentWithType
    (const void*)&GameObject_CUSTOM_Internal_CreateGameObject,  //  <- UnityEngine.GameObject::Internal_CreateGameObject
    (const void*)&GameObject_CUSTOM_SampleAnimation       ,  //  <- UnityEngine.GameObject::SampleAnimation
    (const void*)&GameObject_CUSTOM_PlayAnimation         ,  //  <- UnityEngine.GameObject::PlayAnimation
    (const void*)&GameObject_CUSTOM_StopAnimation         ,  //  <- UnityEngine.GameObject::StopAnimation
    (const void*)&GameObject_CUSTOM_Find                  ,  //  <- UnityEngine.GameObject::Find
    NULL
};

void ExportUnityEngineGameObjectBindings();
void ExportUnityEngineGameObjectBindings()
{
    for (int i = 0; s_UnityEngineGameObject_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineGameObject_IcallNames [i], s_UnityEngineGameObject_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineGameObjectBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(GameObject_CUSTOM_CreatePrimitive); //  <- UnityEngine.GameObject::CreatePrimitive
    SET_METRO_BINDING(GameObject_CUSTOM_GetComponent); //  <- UnityEngine.GameObject::GetComponent
    SET_METRO_BINDING(GameObject_CUSTOM_GetComponentByName); //  <- UnityEngine.GameObject::GetComponentByName
    SET_METRO_BINDING(GameObject_Get_Custom_PropIsStatic); //  <- UnityEngine.GameObject::get_isStatic
    SET_METRO_BINDING(GameObject_Set_Custom_PropIsStatic); //  <- UnityEngine.GameObject::set_isStatic
    SET_METRO_BINDING(GameObject_Get_Custom_PropIsStaticBatchable); //  <- UnityEngine.GameObject::get_isStaticBatchable
    SET_METRO_BINDING(GameObject_CUSTOM_GetComponentsInternal); //  <- UnityEngine.GameObject::GetComponentsInternal
    SET_METRO_BINDING(GameObject_Get_Custom_PropTransform); //  <- UnityEngine.GameObject::get_transform
#if ENABLE_PHYSICS
    SET_METRO_BINDING(GameObject_Get_Custom_PropRigidbody); //  <- UnityEngine.GameObject::get_rigidbody
#endif
    SET_METRO_BINDING(GameObject_Get_Custom_PropRigidbody2D); //  <- UnityEngine.GameObject::get_rigidbody2D
    SET_METRO_BINDING(GameObject_Get_Custom_PropCamera); //  <- UnityEngine.GameObject::get_camera
    SET_METRO_BINDING(GameObject_Get_Custom_PropLight); //  <- UnityEngine.GameObject::get_light
    SET_METRO_BINDING(GameObject_Get_Custom_PropAnimation); //  <- UnityEngine.GameObject::get_animation
#if ENABLE_PHYSICS
    SET_METRO_BINDING(GameObject_Get_Custom_PropConstantForce); //  <- UnityEngine.GameObject::get_constantForce
#endif
    SET_METRO_BINDING(GameObject_Get_Custom_PropRenderer); //  <- UnityEngine.GameObject::get_renderer
#if ENABLE_AUDIO
    SET_METRO_BINDING(GameObject_Get_Custom_PropAudio); //  <- UnityEngine.GameObject::get_audio
#endif
    SET_METRO_BINDING(GameObject_Get_Custom_PropGuiText); //  <- UnityEngine.GameObject::get_guiText
#if ENABLE_NETWORK
    SET_METRO_BINDING(GameObject_Get_Custom_PropNetworkView); //  <- UnityEngine.GameObject::get_networkView
#endif
    SET_METRO_BINDING(GameObject_Get_Custom_PropGuiElement); //  <- UnityEngine.GameObject::get_guiElement
    SET_METRO_BINDING(GameObject_Get_Custom_PropGuiTexture); //  <- UnityEngine.GameObject::get_guiTexture
#if ENABLE_PHYSICS
    SET_METRO_BINDING(GameObject_Get_Custom_PropCollider); //  <- UnityEngine.GameObject::get_collider
#endif
    SET_METRO_BINDING(GameObject_Get_Custom_PropCollider2D); //  <- UnityEngine.GameObject::get_collider2D
#if ENABLE_PHYSICS
    SET_METRO_BINDING(GameObject_Get_Custom_PropHingeJoint); //  <- UnityEngine.GameObject::get_hingeJoint
#endif
    SET_METRO_BINDING(GameObject_Get_Custom_PropParticleSystem); //  <- UnityEngine.GameObject::get_particleSystem
    SET_METRO_BINDING(GameObject_Get_Custom_PropLayer); //  <- UnityEngine.GameObject::get_layer
    SET_METRO_BINDING(GameObject_Set_Custom_PropLayer); //  <- UnityEngine.GameObject::set_layer
    SET_METRO_BINDING(GameObject_Get_Custom_PropActive); //  <- UnityEngine.GameObject::get_active
    SET_METRO_BINDING(GameObject_Set_Custom_PropActive); //  <- UnityEngine.GameObject::set_active
    SET_METRO_BINDING(GameObject_CUSTOM_SetActive); //  <- UnityEngine.GameObject::SetActive
    SET_METRO_BINDING(GameObject_Get_Custom_PropActiveSelf); //  <- UnityEngine.GameObject::get_activeSelf
    SET_METRO_BINDING(GameObject_Get_Custom_PropActiveInHierarchy); //  <- UnityEngine.GameObject::get_activeInHierarchy
    SET_METRO_BINDING(GameObject_CUSTOM_SetActiveRecursively); //  <- UnityEngine.GameObject::SetActiveRecursively
    SET_METRO_BINDING(GameObject_Get_Custom_PropTag); //  <- UnityEngine.GameObject::get_tag
    SET_METRO_BINDING(GameObject_Set_Custom_PropTag); //  <- UnityEngine.GameObject::set_tag
    SET_METRO_BINDING(GameObject_CUSTOM_CompareTag); //  <- UnityEngine.GameObject::CompareTag
    SET_METRO_BINDING(GameObject_CUSTOM_FindGameObjectWithTag); //  <- UnityEngine.GameObject::FindGameObjectWithTag
    SET_METRO_BINDING(GameObject_CUSTOM_FindGameObjectsWithTag); //  <- UnityEngine.GameObject::FindGameObjectsWithTag
    SET_METRO_BINDING(GameObject_CUSTOM_SendMessageUpwards); //  <- UnityEngine.GameObject::SendMessageUpwards
    SET_METRO_BINDING(GameObject_CUSTOM_SendMessage); //  <- UnityEngine.GameObject::SendMessage
    SET_METRO_BINDING(GameObject_CUSTOM_BroadcastMessage); //  <- UnityEngine.GameObject::BroadcastMessage
    SET_METRO_BINDING(GameObject_CUSTOM_AddComponent); //  <- UnityEngine.GameObject::AddComponent
    SET_METRO_BINDING(GameObject_CUSTOM_Internal_AddComponentWithType); //  <- UnityEngine.GameObject::Internal_AddComponentWithType
    SET_METRO_BINDING(GameObject_CUSTOM_Internal_CreateGameObject); //  <- UnityEngine.GameObject::Internal_CreateGameObject
    SET_METRO_BINDING(GameObject_CUSTOM_SampleAnimation); //  <- UnityEngine.GameObject::SampleAnimation
    SET_METRO_BINDING(GameObject_CUSTOM_PlayAnimation); //  <- UnityEngine.GameObject::PlayAnimation
    SET_METRO_BINDING(GameObject_CUSTOM_StopAnimation); //  <- UnityEngine.GameObject::StopAnimation
    SET_METRO_BINDING(GameObject_CUSTOM_Find); //  <- UnityEngine.GameObject::Find
}

#endif
