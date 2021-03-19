#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Camera/Camera.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Camera/Skybox.h"
#include "Runtime/Graphics/LightmapSettings.h"
#include "Runtime/Filters/Renderer.h"
#include "Runtime/Camera/IntermediateRenderer.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Shaders/Material.h"
#include "Runtime/Filters/Misc/TextMesh.h"
#include "Runtime/Shaders/Shader.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Camera/LightManager.h"
#include "Runtime/Misc/QualitySettings.h"
#include "Runtime/Camera/RenderSettings.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include <list>
#include <vector>
SCRIPT_BINDINGS_EXPORT_DECL
LightType SCRIPT_CALL_CONVENTION Light_Get_Custom_PropType(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropType)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_type)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_type)
    return self->GetType ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropType(ICallType_ReadOnlyUnityEngineObject_Argument self_, LightType value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropType)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_type)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_type)
    
    self->SetType (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_CUSTOM_INTERNAL_get_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_CUSTOM_INTERNAL_get_color)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    *returnValue = self->GetColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_CUSTOM_INTERNAL_set_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_CUSTOM_INTERNAL_set_color)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
    
    self->SetColor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropIntensity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropIntensity)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_intensity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_intensity)
    return self->GetIntensity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropIntensity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropIntensity)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_intensity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_intensity)
    
    self->SetIntensity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Light_Get_Custom_PropShadows(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropShadows)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadows)
    return self->GetShadows ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropShadows(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropShadows)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_shadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadows)
    
    self->SetShadows (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropShadowStrength(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropShadowStrength)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shadowStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowStrength)
    return self->GetShadowStrength ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropShadowStrength(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropShadowStrength)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_shadowStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowStrength)
    
    self->SetShadowStrength (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropShadowBias(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropShadowBias)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shadowBias)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowBias)
    return self->GetShadowBias ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropShadowBias(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropShadowBias)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_shadowBias)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowBias)
    
    self->SetShadowBias (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropShadowSoftness(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropShadowSoftness)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shadowSoftness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowSoftness)
    return self->GetShadowSoftness ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropShadowSoftness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropShadowSoftness)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_shadowSoftness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowSoftness)
    
    self->SetShadowSoftness (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropShadowSoftnessFade(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropShadowSoftnessFade)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shadowSoftnessFade)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowSoftnessFade)
    return self->GetShadowSoftnessFade ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropShadowSoftnessFade(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropShadowSoftnessFade)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_shadowSoftnessFade)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowSoftnessFade)
    
    self->SetShadowSoftnessFade (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropRange(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropRange)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_range)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_range)
    return self->GetRange ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropRange(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropRange)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_range)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_range)
    
    self->SetRange (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Light_Get_Custom_PropSpotAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropSpotAngle)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_spotAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spotAngle)
    return self->GetSpotAngle ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropSpotAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropSpotAngle)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_spotAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_spotAngle)
    
    self->SetSpotAngle (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Light_Get_Custom_PropCookie(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropCookie)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cookie)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cookie)
    return Scripting::ScriptingWrapperFor(self->GetCookie());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropCookie(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropCookie)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Texture> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_cookie)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cookie)
    
    self->SetCookie (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Light_Get_Custom_PropFlare(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropFlare)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_flare)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_flare)
    return Scripting::ScriptingWrapperFor(self->GetFlare());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropFlare(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropFlare)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Flare> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_flare)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_flare)
    
    self->SetFlare (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Light_Get_Custom_PropRenderMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropRenderMode)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderMode)
    return self->GetRenderMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropRenderMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropRenderMode)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_renderMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_renderMode)
    
    self->SetRenderMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Light_Get_Custom_PropAlreadyLightmapped(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropAlreadyLightmapped)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alreadyLightmapped)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alreadyLightmapped)
    return self->GetActuallyLightmapped ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropAlreadyLightmapped(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropAlreadyLightmapped)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_alreadyLightmapped)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alreadyLightmapped)
    
    self->SetActuallyLightmapped (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Light_Get_Custom_PropCullingMask(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropCullingMask)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cullingMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cullingMask)
    return self->GetCullingMask ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropCullingMask(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropCullingMask)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cullingMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cullingMask)
    
    self->SetCullingMask (value);
    
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_CUSTOM_INTERNAL_get_areaSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_CUSTOM_INTERNAL_get_areaSize)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_areaSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_areaSize)
    *returnValue = self->GetAreaSize();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_CUSTOM_INTERNAL_set_areaSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_CUSTOM_INTERNAL_set_areaSize)
    ReadOnlyScriptingObjectOfType<Light> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_areaSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_areaSize)
    
    self->SetAreaSize (value);
    
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Light_Get_Custom_PropPixelLightCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Get_Custom_PropPixelLightCount)
    SCRIPTINGAPI_STACK_CHECK(get_pixelLightCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pixelLightCount)
    return GetQualitySettings().GetCurrent().pixelLightCount;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Light_Set_Custom_PropPixelLightCount(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_Set_Custom_PropPixelLightCount)
    SCRIPTINGAPI_STACK_CHECK(set_pixelLightCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pixelLightCount)
     GetQualitySettings().SetPixelLightCount(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Light_CUSTOM_GetLights(LightType type, int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(Light_CUSTOM_GetLights)
    SCRIPTINGAPI_STACK_CHECK(GetLights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetLights)
    
    		UNITY_TEMP_VECTOR(kMemUtility, Light*) lightsvector;
    			
    		layer = 1 << layer;
    		LightManager::Lights& lights = GetLightManager().GetAllLights();
    		for (LightManager::Lights::iterator i=lights.begin();i != lights.end();i++)
    		{
    			Light* light = &*i;
    			if (!light)
    				continue;
    			if (light->GetType() == type && (light->GetCullingMask() & layer) != 0)
    				lightsvector.push_back(light);
    		}
    		
    		return CreateScriptingArrayFromUnityObjects(lightsvector,ClassIDToScriptingType(ClassID(Light)));
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Light_get_type()
{
    scripting_add_internal_call( "UnityEngine.Light::get_type" , (gpointer)& Light_Get_Custom_PropType );
}

void Register_UnityEngine_Light_set_type()
{
    scripting_add_internal_call( "UnityEngine.Light::set_type" , (gpointer)& Light_Set_Custom_PropType );
}

void Register_UnityEngine_Light_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.Light::INTERNAL_get_color" , (gpointer)& Light_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_Light_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.Light::INTERNAL_set_color" , (gpointer)& Light_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_Light_get_intensity()
{
    scripting_add_internal_call( "UnityEngine.Light::get_intensity" , (gpointer)& Light_Get_Custom_PropIntensity );
}

void Register_UnityEngine_Light_set_intensity()
{
    scripting_add_internal_call( "UnityEngine.Light::set_intensity" , (gpointer)& Light_Set_Custom_PropIntensity );
}

void Register_UnityEngine_Light_get_shadows()
{
    scripting_add_internal_call( "UnityEngine.Light::get_shadows" , (gpointer)& Light_Get_Custom_PropShadows );
}

void Register_UnityEngine_Light_set_shadows()
{
    scripting_add_internal_call( "UnityEngine.Light::set_shadows" , (gpointer)& Light_Set_Custom_PropShadows );
}

void Register_UnityEngine_Light_get_shadowStrength()
{
    scripting_add_internal_call( "UnityEngine.Light::get_shadowStrength" , (gpointer)& Light_Get_Custom_PropShadowStrength );
}

void Register_UnityEngine_Light_set_shadowStrength()
{
    scripting_add_internal_call( "UnityEngine.Light::set_shadowStrength" , (gpointer)& Light_Set_Custom_PropShadowStrength );
}

void Register_UnityEngine_Light_get_shadowBias()
{
    scripting_add_internal_call( "UnityEngine.Light::get_shadowBias" , (gpointer)& Light_Get_Custom_PropShadowBias );
}

void Register_UnityEngine_Light_set_shadowBias()
{
    scripting_add_internal_call( "UnityEngine.Light::set_shadowBias" , (gpointer)& Light_Set_Custom_PropShadowBias );
}

void Register_UnityEngine_Light_get_shadowSoftness()
{
    scripting_add_internal_call( "UnityEngine.Light::get_shadowSoftness" , (gpointer)& Light_Get_Custom_PropShadowSoftness );
}

void Register_UnityEngine_Light_set_shadowSoftness()
{
    scripting_add_internal_call( "UnityEngine.Light::set_shadowSoftness" , (gpointer)& Light_Set_Custom_PropShadowSoftness );
}

void Register_UnityEngine_Light_get_shadowSoftnessFade()
{
    scripting_add_internal_call( "UnityEngine.Light::get_shadowSoftnessFade" , (gpointer)& Light_Get_Custom_PropShadowSoftnessFade );
}

void Register_UnityEngine_Light_set_shadowSoftnessFade()
{
    scripting_add_internal_call( "UnityEngine.Light::set_shadowSoftnessFade" , (gpointer)& Light_Set_Custom_PropShadowSoftnessFade );
}

void Register_UnityEngine_Light_get_range()
{
    scripting_add_internal_call( "UnityEngine.Light::get_range" , (gpointer)& Light_Get_Custom_PropRange );
}

void Register_UnityEngine_Light_set_range()
{
    scripting_add_internal_call( "UnityEngine.Light::set_range" , (gpointer)& Light_Set_Custom_PropRange );
}

void Register_UnityEngine_Light_get_spotAngle()
{
    scripting_add_internal_call( "UnityEngine.Light::get_spotAngle" , (gpointer)& Light_Get_Custom_PropSpotAngle );
}

void Register_UnityEngine_Light_set_spotAngle()
{
    scripting_add_internal_call( "UnityEngine.Light::set_spotAngle" , (gpointer)& Light_Set_Custom_PropSpotAngle );
}

void Register_UnityEngine_Light_get_cookie()
{
    scripting_add_internal_call( "UnityEngine.Light::get_cookie" , (gpointer)& Light_Get_Custom_PropCookie );
}

void Register_UnityEngine_Light_set_cookie()
{
    scripting_add_internal_call( "UnityEngine.Light::set_cookie" , (gpointer)& Light_Set_Custom_PropCookie );
}

void Register_UnityEngine_Light_get_flare()
{
    scripting_add_internal_call( "UnityEngine.Light::get_flare" , (gpointer)& Light_Get_Custom_PropFlare );
}

void Register_UnityEngine_Light_set_flare()
{
    scripting_add_internal_call( "UnityEngine.Light::set_flare" , (gpointer)& Light_Set_Custom_PropFlare );
}

void Register_UnityEngine_Light_get_renderMode()
{
    scripting_add_internal_call( "UnityEngine.Light::get_renderMode" , (gpointer)& Light_Get_Custom_PropRenderMode );
}

void Register_UnityEngine_Light_set_renderMode()
{
    scripting_add_internal_call( "UnityEngine.Light::set_renderMode" , (gpointer)& Light_Set_Custom_PropRenderMode );
}

void Register_UnityEngine_Light_get_alreadyLightmapped()
{
    scripting_add_internal_call( "UnityEngine.Light::get_alreadyLightmapped" , (gpointer)& Light_Get_Custom_PropAlreadyLightmapped );
}

void Register_UnityEngine_Light_set_alreadyLightmapped()
{
    scripting_add_internal_call( "UnityEngine.Light::set_alreadyLightmapped" , (gpointer)& Light_Set_Custom_PropAlreadyLightmapped );
}

void Register_UnityEngine_Light_get_cullingMask()
{
    scripting_add_internal_call( "UnityEngine.Light::get_cullingMask" , (gpointer)& Light_Get_Custom_PropCullingMask );
}

void Register_UnityEngine_Light_set_cullingMask()
{
    scripting_add_internal_call( "UnityEngine.Light::set_cullingMask" , (gpointer)& Light_Set_Custom_PropCullingMask );
}

#if UNITY_EDITOR
void Register_UnityEngine_Light_INTERNAL_get_areaSize()
{
    scripting_add_internal_call( "UnityEngine.Light::INTERNAL_get_areaSize" , (gpointer)& Light_CUSTOM_INTERNAL_get_areaSize );
}

void Register_UnityEngine_Light_INTERNAL_set_areaSize()
{
    scripting_add_internal_call( "UnityEngine.Light::INTERNAL_set_areaSize" , (gpointer)& Light_CUSTOM_INTERNAL_set_areaSize );
}

#endif
void Register_UnityEngine_Light_get_pixelLightCount()
{
    scripting_add_internal_call( "UnityEngine.Light::get_pixelLightCount" , (gpointer)& Light_Get_Custom_PropPixelLightCount );
}

void Register_UnityEngine_Light_set_pixelLightCount()
{
    scripting_add_internal_call( "UnityEngine.Light::set_pixelLightCount" , (gpointer)& Light_Set_Custom_PropPixelLightCount );
}

void Register_UnityEngine_Light_GetLights()
{
    scripting_add_internal_call( "UnityEngine.Light::GetLights" , (gpointer)& Light_CUSTOM_GetLights );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineLight_IcallNames [] =
{
    "UnityEngine.Light::get_type"           ,    // -> Light_Get_Custom_PropType
    "UnityEngine.Light::set_type"           ,    // -> Light_Set_Custom_PropType
    "UnityEngine.Light::INTERNAL_get_color" ,    // -> Light_CUSTOM_INTERNAL_get_color
    "UnityEngine.Light::INTERNAL_set_color" ,    // -> Light_CUSTOM_INTERNAL_set_color
    "UnityEngine.Light::get_intensity"      ,    // -> Light_Get_Custom_PropIntensity
    "UnityEngine.Light::set_intensity"      ,    // -> Light_Set_Custom_PropIntensity
    "UnityEngine.Light::get_shadows"        ,    // -> Light_Get_Custom_PropShadows
    "UnityEngine.Light::set_shadows"        ,    // -> Light_Set_Custom_PropShadows
    "UnityEngine.Light::get_shadowStrength" ,    // -> Light_Get_Custom_PropShadowStrength
    "UnityEngine.Light::set_shadowStrength" ,    // -> Light_Set_Custom_PropShadowStrength
    "UnityEngine.Light::get_shadowBias"     ,    // -> Light_Get_Custom_PropShadowBias
    "UnityEngine.Light::set_shadowBias"     ,    // -> Light_Set_Custom_PropShadowBias
    "UnityEngine.Light::get_shadowSoftness" ,    // -> Light_Get_Custom_PropShadowSoftness
    "UnityEngine.Light::set_shadowSoftness" ,    // -> Light_Set_Custom_PropShadowSoftness
    "UnityEngine.Light::get_shadowSoftnessFade",    // -> Light_Get_Custom_PropShadowSoftnessFade
    "UnityEngine.Light::set_shadowSoftnessFade",    // -> Light_Set_Custom_PropShadowSoftnessFade
    "UnityEngine.Light::get_range"          ,    // -> Light_Get_Custom_PropRange
    "UnityEngine.Light::set_range"          ,    // -> Light_Set_Custom_PropRange
    "UnityEngine.Light::get_spotAngle"      ,    // -> Light_Get_Custom_PropSpotAngle
    "UnityEngine.Light::set_spotAngle"      ,    // -> Light_Set_Custom_PropSpotAngle
    "UnityEngine.Light::get_cookie"         ,    // -> Light_Get_Custom_PropCookie
    "UnityEngine.Light::set_cookie"         ,    // -> Light_Set_Custom_PropCookie
    "UnityEngine.Light::get_flare"          ,    // -> Light_Get_Custom_PropFlare
    "UnityEngine.Light::set_flare"          ,    // -> Light_Set_Custom_PropFlare
    "UnityEngine.Light::get_renderMode"     ,    // -> Light_Get_Custom_PropRenderMode
    "UnityEngine.Light::set_renderMode"     ,    // -> Light_Set_Custom_PropRenderMode
    "UnityEngine.Light::get_alreadyLightmapped",    // -> Light_Get_Custom_PropAlreadyLightmapped
    "UnityEngine.Light::set_alreadyLightmapped",    // -> Light_Set_Custom_PropAlreadyLightmapped
    "UnityEngine.Light::get_cullingMask"    ,    // -> Light_Get_Custom_PropCullingMask
    "UnityEngine.Light::set_cullingMask"    ,    // -> Light_Set_Custom_PropCullingMask
#if UNITY_EDITOR
    "UnityEngine.Light::INTERNAL_get_areaSize",    // -> Light_CUSTOM_INTERNAL_get_areaSize
    "UnityEngine.Light::INTERNAL_set_areaSize",    // -> Light_CUSTOM_INTERNAL_set_areaSize
#endif
    "UnityEngine.Light::get_pixelLightCount",    // -> Light_Get_Custom_PropPixelLightCount
    "UnityEngine.Light::set_pixelLightCount",    // -> Light_Set_Custom_PropPixelLightCount
    "UnityEngine.Light::GetLights"          ,    // -> Light_CUSTOM_GetLights
    NULL
};

static const void* s_UnityEngineLight_IcallFuncs [] =
{
    (const void*)&Light_Get_Custom_PropType               ,  //  <- UnityEngine.Light::get_type
    (const void*)&Light_Set_Custom_PropType               ,  //  <- UnityEngine.Light::set_type
    (const void*)&Light_CUSTOM_INTERNAL_get_color         ,  //  <- UnityEngine.Light::INTERNAL_get_color
    (const void*)&Light_CUSTOM_INTERNAL_set_color         ,  //  <- UnityEngine.Light::INTERNAL_set_color
    (const void*)&Light_Get_Custom_PropIntensity          ,  //  <- UnityEngine.Light::get_intensity
    (const void*)&Light_Set_Custom_PropIntensity          ,  //  <- UnityEngine.Light::set_intensity
    (const void*)&Light_Get_Custom_PropShadows            ,  //  <- UnityEngine.Light::get_shadows
    (const void*)&Light_Set_Custom_PropShadows            ,  //  <- UnityEngine.Light::set_shadows
    (const void*)&Light_Get_Custom_PropShadowStrength     ,  //  <- UnityEngine.Light::get_shadowStrength
    (const void*)&Light_Set_Custom_PropShadowStrength     ,  //  <- UnityEngine.Light::set_shadowStrength
    (const void*)&Light_Get_Custom_PropShadowBias         ,  //  <- UnityEngine.Light::get_shadowBias
    (const void*)&Light_Set_Custom_PropShadowBias         ,  //  <- UnityEngine.Light::set_shadowBias
    (const void*)&Light_Get_Custom_PropShadowSoftness     ,  //  <- UnityEngine.Light::get_shadowSoftness
    (const void*)&Light_Set_Custom_PropShadowSoftness     ,  //  <- UnityEngine.Light::set_shadowSoftness
    (const void*)&Light_Get_Custom_PropShadowSoftnessFade ,  //  <- UnityEngine.Light::get_shadowSoftnessFade
    (const void*)&Light_Set_Custom_PropShadowSoftnessFade ,  //  <- UnityEngine.Light::set_shadowSoftnessFade
    (const void*)&Light_Get_Custom_PropRange              ,  //  <- UnityEngine.Light::get_range
    (const void*)&Light_Set_Custom_PropRange              ,  //  <- UnityEngine.Light::set_range
    (const void*)&Light_Get_Custom_PropSpotAngle          ,  //  <- UnityEngine.Light::get_spotAngle
    (const void*)&Light_Set_Custom_PropSpotAngle          ,  //  <- UnityEngine.Light::set_spotAngle
    (const void*)&Light_Get_Custom_PropCookie             ,  //  <- UnityEngine.Light::get_cookie
    (const void*)&Light_Set_Custom_PropCookie             ,  //  <- UnityEngine.Light::set_cookie
    (const void*)&Light_Get_Custom_PropFlare              ,  //  <- UnityEngine.Light::get_flare
    (const void*)&Light_Set_Custom_PropFlare              ,  //  <- UnityEngine.Light::set_flare
    (const void*)&Light_Get_Custom_PropRenderMode         ,  //  <- UnityEngine.Light::get_renderMode
    (const void*)&Light_Set_Custom_PropRenderMode         ,  //  <- UnityEngine.Light::set_renderMode
    (const void*)&Light_Get_Custom_PropAlreadyLightmapped ,  //  <- UnityEngine.Light::get_alreadyLightmapped
    (const void*)&Light_Set_Custom_PropAlreadyLightmapped ,  //  <- UnityEngine.Light::set_alreadyLightmapped
    (const void*)&Light_Get_Custom_PropCullingMask        ,  //  <- UnityEngine.Light::get_cullingMask
    (const void*)&Light_Set_Custom_PropCullingMask        ,  //  <- UnityEngine.Light::set_cullingMask
#if UNITY_EDITOR
    (const void*)&Light_CUSTOM_INTERNAL_get_areaSize      ,  //  <- UnityEngine.Light::INTERNAL_get_areaSize
    (const void*)&Light_CUSTOM_INTERNAL_set_areaSize      ,  //  <- UnityEngine.Light::INTERNAL_set_areaSize
#endif
    (const void*)&Light_Get_Custom_PropPixelLightCount    ,  //  <- UnityEngine.Light::get_pixelLightCount
    (const void*)&Light_Set_Custom_PropPixelLightCount    ,  //  <- UnityEngine.Light::set_pixelLightCount
    (const void*)&Light_CUSTOM_GetLights                  ,  //  <- UnityEngine.Light::GetLights
    NULL
};

void ExportUnityEngineLightBindings();
void ExportUnityEngineLightBindings()
{
    for (int i = 0; s_UnityEngineLight_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineLight_IcallNames [i], s_UnityEngineLight_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineLightBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Light_Get_Custom_PropType); //  <- UnityEngine.Light::get_type
    SET_METRO_BINDING(Light_Set_Custom_PropType); //  <- UnityEngine.Light::set_type
    SET_METRO_BINDING(Light_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.Light::INTERNAL_get_color
    SET_METRO_BINDING(Light_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.Light::INTERNAL_set_color
    SET_METRO_BINDING(Light_Get_Custom_PropIntensity); //  <- UnityEngine.Light::get_intensity
    SET_METRO_BINDING(Light_Set_Custom_PropIntensity); //  <- UnityEngine.Light::set_intensity
    SET_METRO_BINDING(Light_Get_Custom_PropShadows); //  <- UnityEngine.Light::get_shadows
    SET_METRO_BINDING(Light_Set_Custom_PropShadows); //  <- UnityEngine.Light::set_shadows
    SET_METRO_BINDING(Light_Get_Custom_PropShadowStrength); //  <- UnityEngine.Light::get_shadowStrength
    SET_METRO_BINDING(Light_Set_Custom_PropShadowStrength); //  <- UnityEngine.Light::set_shadowStrength
    SET_METRO_BINDING(Light_Get_Custom_PropShadowBias); //  <- UnityEngine.Light::get_shadowBias
    SET_METRO_BINDING(Light_Set_Custom_PropShadowBias); //  <- UnityEngine.Light::set_shadowBias
    SET_METRO_BINDING(Light_Get_Custom_PropShadowSoftness); //  <- UnityEngine.Light::get_shadowSoftness
    SET_METRO_BINDING(Light_Set_Custom_PropShadowSoftness); //  <- UnityEngine.Light::set_shadowSoftness
    SET_METRO_BINDING(Light_Get_Custom_PropShadowSoftnessFade); //  <- UnityEngine.Light::get_shadowSoftnessFade
    SET_METRO_BINDING(Light_Set_Custom_PropShadowSoftnessFade); //  <- UnityEngine.Light::set_shadowSoftnessFade
    SET_METRO_BINDING(Light_Get_Custom_PropRange); //  <- UnityEngine.Light::get_range
    SET_METRO_BINDING(Light_Set_Custom_PropRange); //  <- UnityEngine.Light::set_range
    SET_METRO_BINDING(Light_Get_Custom_PropSpotAngle); //  <- UnityEngine.Light::get_spotAngle
    SET_METRO_BINDING(Light_Set_Custom_PropSpotAngle); //  <- UnityEngine.Light::set_spotAngle
    SET_METRO_BINDING(Light_Get_Custom_PropCookie); //  <- UnityEngine.Light::get_cookie
    SET_METRO_BINDING(Light_Set_Custom_PropCookie); //  <- UnityEngine.Light::set_cookie
    SET_METRO_BINDING(Light_Get_Custom_PropFlare); //  <- UnityEngine.Light::get_flare
    SET_METRO_BINDING(Light_Set_Custom_PropFlare); //  <- UnityEngine.Light::set_flare
    SET_METRO_BINDING(Light_Get_Custom_PropRenderMode); //  <- UnityEngine.Light::get_renderMode
    SET_METRO_BINDING(Light_Set_Custom_PropRenderMode); //  <- UnityEngine.Light::set_renderMode
    SET_METRO_BINDING(Light_Get_Custom_PropAlreadyLightmapped); //  <- UnityEngine.Light::get_alreadyLightmapped
    SET_METRO_BINDING(Light_Set_Custom_PropAlreadyLightmapped); //  <- UnityEngine.Light::set_alreadyLightmapped
    SET_METRO_BINDING(Light_Get_Custom_PropCullingMask); //  <- UnityEngine.Light::get_cullingMask
    SET_METRO_BINDING(Light_Set_Custom_PropCullingMask); //  <- UnityEngine.Light::set_cullingMask
#if UNITY_EDITOR
    SET_METRO_BINDING(Light_CUSTOM_INTERNAL_get_areaSize); //  <- UnityEngine.Light::INTERNAL_get_areaSize
    SET_METRO_BINDING(Light_CUSTOM_INTERNAL_set_areaSize); //  <- UnityEngine.Light::INTERNAL_set_areaSize
#endif
    SET_METRO_BINDING(Light_Get_Custom_PropPixelLightCount); //  <- UnityEngine.Light::get_pixelLightCount
    SET_METRO_BINDING(Light_Set_Custom_PropPixelLightCount); //  <- UnityEngine.Light::set_pixelLightCount
    SET_METRO_BINDING(Light_CUSTOM_GetLights); //  <- UnityEngine.Light::GetLights
}

#endif
