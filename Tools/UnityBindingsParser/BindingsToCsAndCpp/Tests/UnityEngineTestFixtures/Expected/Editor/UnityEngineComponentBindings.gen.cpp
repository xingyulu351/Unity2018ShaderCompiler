#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/BaseClasses/Tags.h"
#if ENABLE_AUDIO
#include "Runtime/Audio/AudioClip.h"
#include "Runtime/Audio/AudioSource.h"
#include "Runtime/Audio/AudioListener.h"
#endif
#include "Runtime/Animation/Animation.h"
#include "Runtime/Math/Color.h"
#include "Runtime/Utilities/PlayerPrefs.h"
#include "Runtime/Camera/Camera.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Filters/Misc/TextMesh.h"
#include "Runtime/Dynamics/ConstantForce.h"
#include "Runtime/Filters/Renderer.h"
#include "Runtime/Network/NetworkView.h"
#include "Runtime/Camera/RenderLayers/GUIText.h"
#include "Runtime/Camera/RenderLayers/GUITexture.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/GetComponent.h"
#if ENABLE_PHYSICS
#include "Runtime/Dynamics/Rigidbody.h"
#include "Runtime/Dynamics/Collider.h"
#include "Runtime/Dynamics/HingeJoint.h"
#endif
#include "Runtime/Dynamics/2D/RigidBody2D.h"
#include "Runtime/Dynamics/2D/Collider2D.h"
#include "Runtime/ParticleSystem/ParticleSystem.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/File/ApplicationSpecificPersistentDataPath.h"
#if ENABLE_MONO
#include "Runtime/Mono/Coroutine.h"
#endif

using namespace Unity;

 #define FAST_COMPONENT_QUERY_COMPONENT_HANDLE(x)  GameObject* go = self->GetGameObjectPtr(); if (go) {Unity::Component* com = go->QueryComponent (x); GetComponentScripting::ScriptingWrapperFor (com, *go, ClassID (x)); return com->GetGCHandle(); } else { RaiseMonoException ("The component is not attached to any game object!"); return 0; }

 #define FAST_COMPONENT_QUERY_COMPONENT(x)  GameObject* go = self->GetGameObjectPtr(); if (go) return GetComponentScripting::ScriptingWrapperFor (go->QueryComponent (x), *go, ClassID (x)); else { RaiseMonoException ("The component is not attached to any game object!"); return SCRIPTING_NULL; }
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_CUSTOM_InternalGetTransform(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_InternalGetTransform)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InternalGetTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalGetTransform)
     
    		FAST_COMPONENT_QUERY_COMPONENT(Transform);
    	
}

#if PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Component_CUSTOM_InternalGetTransformHandle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_InternalGetTransformHandle)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InternalGetTransformHandle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalGetTransformHandle)
     
    		FAST_COMPONENT_QUERY_COMPONENT_HANDLE(Transform);
    	
}

#endif
#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropRigidbody(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropRigidbody)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rigidbody)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rigidbody)
    FAST_COMPONENT_QUERY_COMPONENT(Rigidbody)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropRigidbody2D(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropRigidbody2D)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rigidbody2D)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rigidbody2D)
    FAST_COMPONENT_QUERY_COMPONENT(Rigidbody2D)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropCamera(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropCamera)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_camera)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_camera)
    FAST_COMPONENT_QUERY_COMPONENT(Camera)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropLight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropLight)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_light)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_light)
    FAST_COMPONENT_QUERY_COMPONENT(Light)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropAnimation(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropAnimation)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_animation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_animation)
    FAST_COMPONENT_QUERY_COMPONENT(Animation)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropConstantForce(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropConstantForce)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_constantForce)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_constantForce)
    FAST_COMPONENT_QUERY_COMPONENT(ConstantForce)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropRenderer(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropRenderer)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderer)
    FAST_COMPONENT_QUERY_COMPONENT(Renderer)
}

#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropAudio(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropAudio)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_audio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_audio)
    FAST_COMPONENT_QUERY_COMPONENT(AudioSource)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropGuiText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropGuiText)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_guiText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_guiText)
    FAST_COMPONENT_QUERY_COMPONENT(GUIText)
}

#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropNetworkView(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropNetworkView)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_networkView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_networkView)
    #if ENABLE_NETWORK
    FAST_COMPONENT_QUERY_COMPONENT(NetworkView)
    #else
    return SCRIPTING_NULL;
    #endif
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropGuiElement(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropGuiElement)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_guiElement)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_guiElement)
    FAST_COMPONENT_QUERY_COMPONENT(GUIElement)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropGuiTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropGuiTexture)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_guiTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_guiTexture)
    FAST_COMPONENT_QUERY_COMPONENT(GUITexture)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropCollider(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropCollider)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collider)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collider)
    FAST_COMPONENT_QUERY_COMPONENT(Collider)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropCollider2D(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropCollider2D)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_collider2D)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_collider2D)
    FAST_COMPONENT_QUERY_COMPONENT(Collider2D)
}

#if ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropHingeJoint(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropHingeJoint)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hingeJoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hingeJoint)
    FAST_COMPONENT_QUERY_COMPONENT(HingeJoint)
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropParticleSystem(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropParticleSystem)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_particleSystem)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_particleSystem)
    FAST_COMPONENT_QUERY_COMPONENT(ParticleSystem)
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_CUSTOM_InternalGetGameObject(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_InternalGetGameObject)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InternalGetGameObject)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalGetGameObject)
     
    		return Scripting::ScriptingWrapperFor(self->GetGameObjectPtr()); 
    	
}

#if PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Component_CUSTOM_InternalGetGameObjectHandle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_InternalGetGameObjectHandle)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InternalGetGameObjectHandle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalGetGameObjectHandle)
     
    		GameObject* go = self->GetGameObjectPtr();
    		if (go->GetGCHandle() == 0) Scripting::ScriptingWrapperFor(go); 
    		return go->GetGCHandle();
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Component_CUSTOM_GetComponent(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_GetComponent)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(GetComponent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetComponent)
    
    		GameObject* go = self->GetGameObjectPtr();
    		if (go)
    		{
    			return ScriptingGetComponentOfType (*go, type);
    		}
    		else
    		{
    			RaiseMonoException ("The component is not attached to any game object!");
    			return SCRIPTING_NULL;
    		}
    	
}




	#if PLATFORM_WINRT
	GameObject& GetGameObjectThrow (void* com_)
	{
		ReadOnlyScriptingObjectOfType<Unity::Component> com(com_);
	#else
	GameObject& GetGameObjectThrow (ReadOnlyScriptingObjectOfType<Unity::Component> com)
	{
	#endif
		GameObject* go = com->GetGameObjectPtr();
		if (go)
			return *go;
		{
			RaiseMonoException ("The component is not attached to any game object!");
			return *go;
		}
	}
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Component_CUSTOM_GetComponents(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_GetComponents)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(GetComponents)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetComponents)
     return ScriptingGetComponentsOfType (GetGameObjectThrow(self), type, false, false, true); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Component_CUSTOM_GetComponentsWithCorrectReturnType(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_GetComponentsWithCorrectReturnType)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(GetComponentsWithCorrectReturnType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetComponentsWithCorrectReturnType)
     return ScriptingGetComponentsOfType (GetGameObjectThrow(self), type, true, false, true); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Component_Get_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_active)
    return self->IsActive ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Component_Set_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Set_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_active)
     GameObject& go = GetGameObjectThrow(self); if (value) go.Activate (); else go.Deactivate (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Component_Get_Custom_PropTag(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Get_Custom_PropTag)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_tag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_tag)
    const string& tag = TagToString (GetGameObjectThrow(self).GetTag ());
    if (!tag.empty ())
    return CreateScriptingString (tag);
    else
    {
    RaiseMonoException ("GameObject has undefined tag!");
    return SCRIPTING_NULL;
    }
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Component_Set_Custom_PropTag(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_Set_Custom_PropTag)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_tag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_tag)
    
    GetGameObjectThrow(self).SetTag (ExtractTagThrowing (value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Component_CUSTOM_CompareTag(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument tag_)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_CompareTag)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ICallType_String_Local tag(tag_);
    UNUSED(tag);
    SCRIPTINGAPI_STACK_CHECK(CompareTag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CompareTag)
     return ExtractTagThrowing (tag) == GetGameObjectThrow(self).GetTag (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Component_CUSTOM_SendMessageUpwards(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument value_, int options)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_SendMessageUpwards)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(SendMessageUpwards)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SendMessageUpwards)
    
    		Scripting::SendScriptingMessageUpwards(GetGameObjectThrow(self), methodName, value, options);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Component_CUSTOM_SendMessage(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument value_, int options)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_SendMessage)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(SendMessage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SendMessage)
    
    		Scripting::SendScriptingMessage(GetGameObjectThrow(self), methodName, value, options);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Component_CUSTOM_BroadcastMessage(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument parameter_, int options)
{
    SCRIPTINGAPI_ETW_ENTRY(Component_CUSTOM_BroadcastMessage)
    ReadOnlyScriptingObjectOfType<Unity::Component> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local parameter(parameter_);
    UNUSED(parameter);
    SCRIPTINGAPI_STACK_CHECK(BroadcastMessage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BroadcastMessage)
    
    		Scripting::BroadcastScriptingMessage (GetGameObjectThrow(self), methodName, parameter, options);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Component_InternalGetTransform()
{
    scripting_add_internal_call( "UnityEngine.Component::InternalGetTransform" , (gpointer)& Component_CUSTOM_InternalGetTransform );
}

#if PLATFORM_WINRT
void Register_UnityEngine_Component_InternalGetTransformHandle()
{
    scripting_add_internal_call( "UnityEngine.Component::InternalGetTransformHandle" , (gpointer)& Component_CUSTOM_InternalGetTransformHandle );
}

#endif
#if ENABLE_PHYSICS
void Register_UnityEngine_Component_get_rigidbody()
{
    scripting_add_internal_call( "UnityEngine.Component::get_rigidbody" , (gpointer)& Component_Get_Custom_PropRigidbody );
}

#endif
void Register_UnityEngine_Component_get_rigidbody2D()
{
    scripting_add_internal_call( "UnityEngine.Component::get_rigidbody2D" , (gpointer)& Component_Get_Custom_PropRigidbody2D );
}

void Register_UnityEngine_Component_get_camera()
{
    scripting_add_internal_call( "UnityEngine.Component::get_camera" , (gpointer)& Component_Get_Custom_PropCamera );
}

void Register_UnityEngine_Component_get_light()
{
    scripting_add_internal_call( "UnityEngine.Component::get_light" , (gpointer)& Component_Get_Custom_PropLight );
}

void Register_UnityEngine_Component_get_animation()
{
    scripting_add_internal_call( "UnityEngine.Component::get_animation" , (gpointer)& Component_Get_Custom_PropAnimation );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_Component_get_constantForce()
{
    scripting_add_internal_call( "UnityEngine.Component::get_constantForce" , (gpointer)& Component_Get_Custom_PropConstantForce );
}

#endif
void Register_UnityEngine_Component_get_renderer()
{
    scripting_add_internal_call( "UnityEngine.Component::get_renderer" , (gpointer)& Component_Get_Custom_PropRenderer );
}

#if ENABLE_AUDIO
void Register_UnityEngine_Component_get_audio()
{
    scripting_add_internal_call( "UnityEngine.Component::get_audio" , (gpointer)& Component_Get_Custom_PropAudio );
}

#endif
void Register_UnityEngine_Component_get_guiText()
{
    scripting_add_internal_call( "UnityEngine.Component::get_guiText" , (gpointer)& Component_Get_Custom_PropGuiText );
}

#if ENABLE_NETWORK
void Register_UnityEngine_Component_get_networkView()
{
    scripting_add_internal_call( "UnityEngine.Component::get_networkView" , (gpointer)& Component_Get_Custom_PropNetworkView );
}

#endif
void Register_UnityEngine_Component_get_guiElement()
{
    scripting_add_internal_call( "UnityEngine.Component::get_guiElement" , (gpointer)& Component_Get_Custom_PropGuiElement );
}

void Register_UnityEngine_Component_get_guiTexture()
{
    scripting_add_internal_call( "UnityEngine.Component::get_guiTexture" , (gpointer)& Component_Get_Custom_PropGuiTexture );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_Component_get_collider()
{
    scripting_add_internal_call( "UnityEngine.Component::get_collider" , (gpointer)& Component_Get_Custom_PropCollider );
}

#endif
void Register_UnityEngine_Component_get_collider2D()
{
    scripting_add_internal_call( "UnityEngine.Component::get_collider2D" , (gpointer)& Component_Get_Custom_PropCollider2D );
}

#if ENABLE_PHYSICS
void Register_UnityEngine_Component_get_hingeJoint()
{
    scripting_add_internal_call( "UnityEngine.Component::get_hingeJoint" , (gpointer)& Component_Get_Custom_PropHingeJoint );
}

#endif
void Register_UnityEngine_Component_get_particleSystem()
{
    scripting_add_internal_call( "UnityEngine.Component::get_particleSystem" , (gpointer)& Component_Get_Custom_PropParticleSystem );
}

void Register_UnityEngine_Component_InternalGetGameObject()
{
    scripting_add_internal_call( "UnityEngine.Component::InternalGetGameObject" , (gpointer)& Component_CUSTOM_InternalGetGameObject );
}

#if PLATFORM_WINRT
void Register_UnityEngine_Component_InternalGetGameObjectHandle()
{
    scripting_add_internal_call( "UnityEngine.Component::InternalGetGameObjectHandle" , (gpointer)& Component_CUSTOM_InternalGetGameObjectHandle );
}

#endif
void Register_UnityEngine_Component_GetComponent()
{
    scripting_add_internal_call( "UnityEngine.Component::GetComponent" , (gpointer)& Component_CUSTOM_GetComponent );
}

void Register_UnityEngine_Component_GetComponents()
{
    scripting_add_internal_call( "UnityEngine.Component::GetComponents" , (gpointer)& Component_CUSTOM_GetComponents );
}

void Register_UnityEngine_Component_GetComponentsWithCorrectReturnType()
{
    scripting_add_internal_call( "UnityEngine.Component::GetComponentsWithCorrectReturnType" , (gpointer)& Component_CUSTOM_GetComponentsWithCorrectReturnType );
}

void Register_UnityEngine_Component_get_active()
{
    scripting_add_internal_call( "UnityEngine.Component::get_active" , (gpointer)& Component_Get_Custom_PropActive );
}

void Register_UnityEngine_Component_set_active()
{
    scripting_add_internal_call( "UnityEngine.Component::set_active" , (gpointer)& Component_Set_Custom_PropActive );
}

void Register_UnityEngine_Component_get_tag()
{
    scripting_add_internal_call( "UnityEngine.Component::get_tag" , (gpointer)& Component_Get_Custom_PropTag );
}

void Register_UnityEngine_Component_set_tag()
{
    scripting_add_internal_call( "UnityEngine.Component::set_tag" , (gpointer)& Component_Set_Custom_PropTag );
}

void Register_UnityEngine_Component_CompareTag()
{
    scripting_add_internal_call( "UnityEngine.Component::CompareTag" , (gpointer)& Component_CUSTOM_CompareTag );
}

void Register_UnityEngine_Component_SendMessageUpwards()
{
    scripting_add_internal_call( "UnityEngine.Component::SendMessageUpwards" , (gpointer)& Component_CUSTOM_SendMessageUpwards );
}

void Register_UnityEngine_Component_SendMessage()
{
    scripting_add_internal_call( "UnityEngine.Component::SendMessage" , (gpointer)& Component_CUSTOM_SendMessage );
}

void Register_UnityEngine_Component_BroadcastMessage()
{
    scripting_add_internal_call( "UnityEngine.Component::BroadcastMessage" , (gpointer)& Component_CUSTOM_BroadcastMessage );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineComponent_IcallNames [] =
{
    "UnityEngine.Component::InternalGetTransform",    // -> Component_CUSTOM_InternalGetTransform
#if PLATFORM_WINRT
    "UnityEngine.Component::InternalGetTransformHandle",    // -> Component_CUSTOM_InternalGetTransformHandle
#endif
#if ENABLE_PHYSICS
    "UnityEngine.Component::get_rigidbody"  ,    // -> Component_Get_Custom_PropRigidbody
#endif
    "UnityEngine.Component::get_rigidbody2D",    // -> Component_Get_Custom_PropRigidbody2D
    "UnityEngine.Component::get_camera"     ,    // -> Component_Get_Custom_PropCamera
    "UnityEngine.Component::get_light"      ,    // -> Component_Get_Custom_PropLight
    "UnityEngine.Component::get_animation"  ,    // -> Component_Get_Custom_PropAnimation
#if ENABLE_PHYSICS
    "UnityEngine.Component::get_constantForce",    // -> Component_Get_Custom_PropConstantForce
#endif
    "UnityEngine.Component::get_renderer"   ,    // -> Component_Get_Custom_PropRenderer
#if ENABLE_AUDIO
    "UnityEngine.Component::get_audio"      ,    // -> Component_Get_Custom_PropAudio
#endif
    "UnityEngine.Component::get_guiText"    ,    // -> Component_Get_Custom_PropGuiText
#if ENABLE_NETWORK
    "UnityEngine.Component::get_networkView",    // -> Component_Get_Custom_PropNetworkView
#endif
    "UnityEngine.Component::get_guiElement" ,    // -> Component_Get_Custom_PropGuiElement
    "UnityEngine.Component::get_guiTexture" ,    // -> Component_Get_Custom_PropGuiTexture
#if ENABLE_PHYSICS
    "UnityEngine.Component::get_collider"   ,    // -> Component_Get_Custom_PropCollider
#endif
    "UnityEngine.Component::get_collider2D" ,    // -> Component_Get_Custom_PropCollider2D
#if ENABLE_PHYSICS
    "UnityEngine.Component::get_hingeJoint" ,    // -> Component_Get_Custom_PropHingeJoint
#endif
    "UnityEngine.Component::get_particleSystem",    // -> Component_Get_Custom_PropParticleSystem
    "UnityEngine.Component::InternalGetGameObject",    // -> Component_CUSTOM_InternalGetGameObject
#if PLATFORM_WINRT
    "UnityEngine.Component::InternalGetGameObjectHandle",    // -> Component_CUSTOM_InternalGetGameObjectHandle
#endif
    "UnityEngine.Component::GetComponent"   ,    // -> Component_CUSTOM_GetComponent
    "UnityEngine.Component::GetComponents"  ,    // -> Component_CUSTOM_GetComponents
    "UnityEngine.Component::GetComponentsWithCorrectReturnType",    // -> Component_CUSTOM_GetComponentsWithCorrectReturnType
    "UnityEngine.Component::get_active"     ,    // -> Component_Get_Custom_PropActive
    "UnityEngine.Component::set_active"     ,    // -> Component_Set_Custom_PropActive
    "UnityEngine.Component::get_tag"        ,    // -> Component_Get_Custom_PropTag
    "UnityEngine.Component::set_tag"        ,    // -> Component_Set_Custom_PropTag
    "UnityEngine.Component::CompareTag"     ,    // -> Component_CUSTOM_CompareTag
    "UnityEngine.Component::SendMessageUpwards",    // -> Component_CUSTOM_SendMessageUpwards
    "UnityEngine.Component::SendMessage"    ,    // -> Component_CUSTOM_SendMessage
    "UnityEngine.Component::BroadcastMessage",    // -> Component_CUSTOM_BroadcastMessage
    NULL
};

static const void* s_UnityEngineComponent_IcallFuncs [] =
{
    (const void*)&Component_CUSTOM_InternalGetTransform   ,  //  <- UnityEngine.Component::InternalGetTransform
#if PLATFORM_WINRT
    (const void*)&Component_CUSTOM_InternalGetTransformHandle,  //  <- UnityEngine.Component::InternalGetTransformHandle
#endif
#if ENABLE_PHYSICS
    (const void*)&Component_Get_Custom_PropRigidbody      ,  //  <- UnityEngine.Component::get_rigidbody
#endif
    (const void*)&Component_Get_Custom_PropRigidbody2D    ,  //  <- UnityEngine.Component::get_rigidbody2D
    (const void*)&Component_Get_Custom_PropCamera         ,  //  <- UnityEngine.Component::get_camera
    (const void*)&Component_Get_Custom_PropLight          ,  //  <- UnityEngine.Component::get_light
    (const void*)&Component_Get_Custom_PropAnimation      ,  //  <- UnityEngine.Component::get_animation
#if ENABLE_PHYSICS
    (const void*)&Component_Get_Custom_PropConstantForce  ,  //  <- UnityEngine.Component::get_constantForce
#endif
    (const void*)&Component_Get_Custom_PropRenderer       ,  //  <- UnityEngine.Component::get_renderer
#if ENABLE_AUDIO
    (const void*)&Component_Get_Custom_PropAudio          ,  //  <- UnityEngine.Component::get_audio
#endif
    (const void*)&Component_Get_Custom_PropGuiText        ,  //  <- UnityEngine.Component::get_guiText
#if ENABLE_NETWORK
    (const void*)&Component_Get_Custom_PropNetworkView    ,  //  <- UnityEngine.Component::get_networkView
#endif
    (const void*)&Component_Get_Custom_PropGuiElement     ,  //  <- UnityEngine.Component::get_guiElement
    (const void*)&Component_Get_Custom_PropGuiTexture     ,  //  <- UnityEngine.Component::get_guiTexture
#if ENABLE_PHYSICS
    (const void*)&Component_Get_Custom_PropCollider       ,  //  <- UnityEngine.Component::get_collider
#endif
    (const void*)&Component_Get_Custom_PropCollider2D     ,  //  <- UnityEngine.Component::get_collider2D
#if ENABLE_PHYSICS
    (const void*)&Component_Get_Custom_PropHingeJoint     ,  //  <- UnityEngine.Component::get_hingeJoint
#endif
    (const void*)&Component_Get_Custom_PropParticleSystem ,  //  <- UnityEngine.Component::get_particleSystem
    (const void*)&Component_CUSTOM_InternalGetGameObject  ,  //  <- UnityEngine.Component::InternalGetGameObject
#if PLATFORM_WINRT
    (const void*)&Component_CUSTOM_InternalGetGameObjectHandle,  //  <- UnityEngine.Component::InternalGetGameObjectHandle
#endif
    (const void*)&Component_CUSTOM_GetComponent           ,  //  <- UnityEngine.Component::GetComponent
    (const void*)&Component_CUSTOM_GetComponents          ,  //  <- UnityEngine.Component::GetComponents
    (const void*)&Component_CUSTOM_GetComponentsWithCorrectReturnType,  //  <- UnityEngine.Component::GetComponentsWithCorrectReturnType
    (const void*)&Component_Get_Custom_PropActive         ,  //  <- UnityEngine.Component::get_active
    (const void*)&Component_Set_Custom_PropActive         ,  //  <- UnityEngine.Component::set_active
    (const void*)&Component_Get_Custom_PropTag            ,  //  <- UnityEngine.Component::get_tag
    (const void*)&Component_Set_Custom_PropTag            ,  //  <- UnityEngine.Component::set_tag
    (const void*)&Component_CUSTOM_CompareTag             ,  //  <- UnityEngine.Component::CompareTag
    (const void*)&Component_CUSTOM_SendMessageUpwards     ,  //  <- UnityEngine.Component::SendMessageUpwards
    (const void*)&Component_CUSTOM_SendMessage            ,  //  <- UnityEngine.Component::SendMessage
    (const void*)&Component_CUSTOM_BroadcastMessage       ,  //  <- UnityEngine.Component::BroadcastMessage
    NULL
};

void ExportUnityEngineComponentBindings();
void ExportUnityEngineComponentBindings()
{
    for (int i = 0; s_UnityEngineComponent_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineComponent_IcallNames [i], s_UnityEngineComponent_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineComponentBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Component_CUSTOM_InternalGetTransform); //  <- UnityEngine.Component::InternalGetTransform
#if PLATFORM_WINRT
    SET_METRO_BINDING(Component_CUSTOM_InternalGetTransformHandle); //  <- UnityEngine.Component::InternalGetTransformHandle
#endif
#if ENABLE_PHYSICS
    SET_METRO_BINDING(Component_Get_Custom_PropRigidbody); //  <- UnityEngine.Component::get_rigidbody
#endif
    SET_METRO_BINDING(Component_Get_Custom_PropRigidbody2D); //  <- UnityEngine.Component::get_rigidbody2D
    SET_METRO_BINDING(Component_Get_Custom_PropCamera); //  <- UnityEngine.Component::get_camera
    SET_METRO_BINDING(Component_Get_Custom_PropLight); //  <- UnityEngine.Component::get_light
    SET_METRO_BINDING(Component_Get_Custom_PropAnimation); //  <- UnityEngine.Component::get_animation
#if ENABLE_PHYSICS
    SET_METRO_BINDING(Component_Get_Custom_PropConstantForce); //  <- UnityEngine.Component::get_constantForce
#endif
    SET_METRO_BINDING(Component_Get_Custom_PropRenderer); //  <- UnityEngine.Component::get_renderer
#if ENABLE_AUDIO
    SET_METRO_BINDING(Component_Get_Custom_PropAudio); //  <- UnityEngine.Component::get_audio
#endif
    SET_METRO_BINDING(Component_Get_Custom_PropGuiText); //  <- UnityEngine.Component::get_guiText
#if ENABLE_NETWORK
    SET_METRO_BINDING(Component_Get_Custom_PropNetworkView); //  <- UnityEngine.Component::get_networkView
#endif
    SET_METRO_BINDING(Component_Get_Custom_PropGuiElement); //  <- UnityEngine.Component::get_guiElement
    SET_METRO_BINDING(Component_Get_Custom_PropGuiTexture); //  <- UnityEngine.Component::get_guiTexture
#if ENABLE_PHYSICS
    SET_METRO_BINDING(Component_Get_Custom_PropCollider); //  <- UnityEngine.Component::get_collider
#endif
    SET_METRO_BINDING(Component_Get_Custom_PropCollider2D); //  <- UnityEngine.Component::get_collider2D
#if ENABLE_PHYSICS
    SET_METRO_BINDING(Component_Get_Custom_PropHingeJoint); //  <- UnityEngine.Component::get_hingeJoint
#endif
    SET_METRO_BINDING(Component_Get_Custom_PropParticleSystem); //  <- UnityEngine.Component::get_particleSystem
    SET_METRO_BINDING(Component_CUSTOM_InternalGetGameObject); //  <- UnityEngine.Component::InternalGetGameObject
#if PLATFORM_WINRT
    SET_METRO_BINDING(Component_CUSTOM_InternalGetGameObjectHandle); //  <- UnityEngine.Component::InternalGetGameObjectHandle
#endif
    SET_METRO_BINDING(Component_CUSTOM_GetComponent); //  <- UnityEngine.Component::GetComponent
    SET_METRO_BINDING(Component_CUSTOM_GetComponents); //  <- UnityEngine.Component::GetComponents
    SET_METRO_BINDING(Component_CUSTOM_GetComponentsWithCorrectReturnType); //  <- UnityEngine.Component::GetComponentsWithCorrectReturnType
    SET_METRO_BINDING(Component_Get_Custom_PropActive); //  <- UnityEngine.Component::get_active
    SET_METRO_BINDING(Component_Set_Custom_PropActive); //  <- UnityEngine.Component::set_active
    SET_METRO_BINDING(Component_Get_Custom_PropTag); //  <- UnityEngine.Component::get_tag
    SET_METRO_BINDING(Component_Set_Custom_PropTag); //  <- UnityEngine.Component::set_tag
    SET_METRO_BINDING(Component_CUSTOM_CompareTag); //  <- UnityEngine.Component::CompareTag
    SET_METRO_BINDING(Component_CUSTOM_SendMessageUpwards); //  <- UnityEngine.Component::SendMessageUpwards
    SET_METRO_BINDING(Component_CUSTOM_SendMessage); //  <- UnityEngine.Component::SendMessage
    SET_METRO_BINDING(Component_CUSTOM_BroadcastMessage); //  <- UnityEngine.Component::BroadcastMessage
}

#endif
