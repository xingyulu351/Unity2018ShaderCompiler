#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Camera/Camera.h"
#include "Runtime/Camera/ImageFilters.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Camera/Skybox.h"
#include "Runtime/Graphics/LightmapSettings.h"
#include "Runtime/Filters/Renderer.h"
#include "Runtime/Camera/IntermediateRenderer.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Shaders/Material.h"
#include "Runtime/Filters/Misc/TextMesh.h"
#include "Runtime/Shaders/Shader.h"
#include "External/shaderlab/Library/intshader.h"
#include "Runtime/Camera/Renderqueue.h"
#include "Runtime/Filters/Deformation/SkinnedMeshFilter.h"
#include "Runtime/Camera/Flare.h"
#include "Runtime/GfxDevice/GfxDevice.h"
#include "External/shaderlab/Library/texenv.h"
#include "External/shaderlab/Library/properties.h"
#include "Runtime/Geometry/Ray.h"
#include "Runtime/Graphics/Texture2D.h"
#include "Runtime/Graphics/CubemapTexture.h"
#include "Runtime/Graphics/Texture3D.h"
#include "Runtime/Graphics/VideoTexture.h"
#include "Runtime/Camera/Projector.h"
#include "Runtime/Camera/RenderLayers/GUITexture.h"
#include "Runtime/Camera/RenderLayers/GUIText.h"
#include "Runtime/Camera/RenderLayers/GUILayer.h"
#include "Runtime/Filters/Misc/LineRenderer.h"
#include "Runtime/Graphics/DrawUtil.h"
#include "Runtime/Math/Quaternion.h"
#include "Runtime/Filters/Mesh/LodMesh.h"
#include "Runtime/Filters/Mesh/LodMeshFilter.h"
#include "Runtime/Graphics/RenderTexture.h"
#include "Runtime/Graphics/MatrixStack.h"
#include "Runtime/Graphics/MovieTexture.h"
#include "Runtime/Filters/Misc/TrailRenderer.h"
#include "Runtime/Camera/LODGroupManager.h"
#include "Runtime/Graphics/RenderBufferManager.h"
#include "Runtime/Camera/RenderManager.h"
#include "Runtime/Shaders/GraphicsCaps.h"
#include "Runtime/Graphics/ScreenManager.h"
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Geometry/Plane.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Filters/AABBUtility.h"
#include <list>
#include <vector>
#include "Runtime/Misc/QualitySettings.h"
#include "Runtime/Filters/Misc/Font.h"
#include "Runtime/Camera/RenderSettings.h"
#include "Runtime/Geometry/Intersection.h"
#include "Runtime/Shaders/ShaderNameRegistry.h"
#include "Runtime/Shaders/ShaderKeywords.h"
#include "Runtime/Shaders/ComputeShader.h"
#include "Runtime/Geometry/TextureAtlas.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Camera/CameraUtil.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/Graphics/ImageConversion.h"
#include "Runtime/Filters/Mesh/MeshCombiner.h"
#include "Runtime/Filters/Mesh/MeshOptimizer.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Camera/Culler.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Threads/AtomicRefCounter.h"
#include "Runtime/Camera/PVSVolume.h"
#include "Runtime/Camera/OcclusionPortal.h"
#include "Runtime/Graphics/DrawSplashScreenAndWatermarks.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingManager.h"
#include "Runtime/ScriptingBackend/ScriptingTypeRegistry.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/MonoManagerFlash.h"
#include "Runtime/Utilities/BitUtility.h"
#include "Runtime/Profiler/Profiler.h"
#include "Runtime/IMGUI/GUIStyle.h"
#include "Runtime/ScriptingBackend/ScriptingApi.h"
#include "Runtime/Graphics/TriStripper.h"

#if ENABLE_TEXTUREID_MAP
	#include "Runtime/GfxDevice/TextureIdMap.h"
#endif


#if UNITY_EDITOR
#include "Editor/Src/Gizmos/GizmoManager.h"
#include "Editor/Src/Gizmos/GizmoUtil.h"
#endif


static MatrixStack g_WorldMatrixStack;
static MatrixStack g_ViewMatrixStack;
static MatrixStack g_ProjectionMatrixStack;



class MonoMaterialPropertyBlock : public MaterialPropertyBlock {
public:
	MonoMaterialPropertyBlock() : m_Counter() { }

	void Retain()
	{
		m_Counter.Retain();
	}
	void Release()
	{
		if( m_Counter.Release() ) {
			delete this;
		}
	}

	static void CleanupMonoMaterialPropertyBlock(void* mmpb)
	{
		((MonoMaterialPropertyBlock*)mmpb)->Release();
	}

private:
	AtomicRefCounter m_Counter;
};


static PPtr<Shader> s_CurrentShader;
static const ChannelAssigns* s_CurrentChannels;

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OcclusionArea_CUSTOM_INTERNAL_get_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(OcclusionArea_CUSTOM_INTERNAL_get_center)
    ReadOnlyScriptingObjectOfType<OcclusionArea> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_center)
    *returnValue = self->GetCenter();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OcclusionArea_CUSTOM_INTERNAL_set_center(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(OcclusionArea_CUSTOM_INTERNAL_set_center)
    ReadOnlyScriptingObjectOfType<OcclusionArea> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_center)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_center)
    
    self->SetCenter (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OcclusionArea_CUSTOM_INTERNAL_get_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(OcclusionArea_CUSTOM_INTERNAL_get_size)
    ReadOnlyScriptingObjectOfType<OcclusionArea> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_size)
    *returnValue = self->GetSize();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OcclusionArea_CUSTOM_INTERNAL_set_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(OcclusionArea_CUSTOM_INTERNAL_set_size)
    ReadOnlyScriptingObjectOfType<OcclusionArea> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_size)
    
    self->SetSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION OcclusionPortal_Get_Custom_PropOpen(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(OcclusionPortal_Get_Custom_PropOpen)
    ReadOnlyScriptingObjectOfType<OcclusionPortal> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_open)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_open)
    return self->GetIsOpen();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OcclusionPortal_Set_Custom_PropOpen(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(OcclusionPortal_Set_Custom_PropOpen)
    ReadOnlyScriptingObjectOfType<OcclusionPortal> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_open)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_open)
     self->SetIsOpen(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropFog()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropFog)
    SCRIPTINGAPI_STACK_CHECK(get_fog)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fog)
    return GetRenderSettings().GetUseFog();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropFog(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropFog)
    SCRIPTINGAPI_STACK_CHECK(set_fog)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fog)
     return GetRenderSettings().SetUseFog(value);  
}

SCRIPT_BINDINGS_EXPORT_DECL
FogMode SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropFogMode()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropFogMode)
    SCRIPTINGAPI_STACK_CHECK(get_fogMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fogMode)
    return GetRenderSettings().GetFogMode();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropFogMode(FogMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropFogMode)
    SCRIPTINGAPI_STACK_CHECK(set_fogMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fogMode)
     return GetRenderSettings().SetFogMode(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_CUSTOM_INTERNAL_get_fogColor(ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_CUSTOM_INTERNAL_get_fogColor)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_fogColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_fogColor)
    { *returnValue =(GetRenderSettings().GetFogColor()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_CUSTOM_INTERNAL_set_fogColor(const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_CUSTOM_INTERNAL_set_fogColor)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_fogColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_fogColor)
     GetRenderSettings().SetFogColor(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropFogDensity()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropFogDensity)
    SCRIPTINGAPI_STACK_CHECK(get_fogDensity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fogDensity)
    return GetRenderSettings().GetFogDensity();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropFogDensity(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropFogDensity)
    SCRIPTINGAPI_STACK_CHECK(set_fogDensity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fogDensity)
     GetRenderSettings().SetFogDensity(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropFogStartDistance()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropFogStartDistance)
    SCRIPTINGAPI_STACK_CHECK(get_fogStartDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fogStartDistance)
    return GetRenderSettings().GetLinearFogStart();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropFogStartDistance(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropFogStartDistance)
    SCRIPTINGAPI_STACK_CHECK(set_fogStartDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fogStartDistance)
     GetRenderSettings().SetLinearFogStart(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropFogEndDistance()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropFogEndDistance)
    SCRIPTINGAPI_STACK_CHECK(get_fogEndDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fogEndDistance)
    return GetRenderSettings().GetLinearFogEnd();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropFogEndDistance(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropFogEndDistance)
    SCRIPTINGAPI_STACK_CHECK(set_fogEndDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fogEndDistance)
     GetRenderSettings().SetLinearFogEnd(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_CUSTOM_INTERNAL_get_ambientLight(ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_CUSTOM_INTERNAL_get_ambientLight)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_ambientLight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_ambientLight)
    { *returnValue =(GetRenderSettings().GetAmbientLight()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_CUSTOM_INTERNAL_set_ambientLight(const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_CUSTOM_INTERNAL_set_ambientLight)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_ambientLight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_ambientLight)
     GetRenderSettings().SetAmbientLight(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropHaloStrength()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropHaloStrength)
    SCRIPTINGAPI_STACK_CHECK(get_haloStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_haloStrength)
    return GetRenderSettings().GetHaloStrength();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropHaloStrength(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropHaloStrength)
    SCRIPTINGAPI_STACK_CHECK(set_haloStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_haloStrength)
     GetRenderSettings().SetHaloStrength(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropFlareStrength()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropFlareStrength)
    SCRIPTINGAPI_STACK_CHECK(get_flareStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_flareStrength)
    return GetRenderSettings().GetFlareStrength();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropFlareStrength(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropFlareStrength)
    SCRIPTINGAPI_STACK_CHECK(set_flareStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_flareStrength)
     GetRenderSettings().SetFlareStrength(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION RenderSettings_Get_Custom_PropSkybox()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Get_Custom_PropSkybox)
    SCRIPTINGAPI_STACK_CHECK(get_skybox)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_skybox)
    return Scripting::ScriptingWrapperFor(GetRenderSettings().GetSkyboxMaterial());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderSettings_Set_Custom_PropSkybox(ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderSettings_Set_Custom_PropSkybox)
    ReadOnlyScriptingObjectOfType<Material> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_skybox)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_skybox)
     GetRenderSettings().SetSkyboxMaterial(value); 
}

#if ENABLE_MONO || PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropNames()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropNames)
    SCRIPTINGAPI_STACK_CHECK(get_names)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_names)
    return StringVectorToScripting(GetQualitySettings().GetQualitySettingsNames ());
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_CUSTOM_GetQualityLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_CUSTOM_GetQualityLevel)
    SCRIPTINGAPI_STACK_CHECK(GetQualityLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetQualityLevel)
    
    		return GetQualitySettings().GetCurrentIndex();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_CUSTOM_SetQualityLevel(int index, ScriptingBool applyExpensiveChanges)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_CUSTOM_SetQualityLevel)
    SCRIPTINGAPI_STACK_CHECK(SetQualityLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetQualityLevel)
    
    		GetQualitySettings().SetCurrentIndex (index, applyExpensiveChanges);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropCurrentLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropCurrentLevel)
    SCRIPTINGAPI_STACK_CHECK(get_currentLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_currentLevel)
    return GetQualitySettings().GetCurrentIndex();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropCurrentLevel(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropCurrentLevel)
    SCRIPTINGAPI_STACK_CHECK(set_currentLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_currentLevel)
     GetQualitySettings().SetCurrentIndex( value, true ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_CUSTOM_IncreaseLevel(ScriptingBool applyExpensiveChanges)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_CUSTOM_IncreaseLevel)
    SCRIPTINGAPI_STACK_CHECK(IncreaseLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IncreaseLevel)
    
    		QualitySettings& q = GetQualitySettings();
    		q.SetCurrentIndex( q.GetCurrentIndex() + 1, applyExpensiveChanges ); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_CUSTOM_DecreaseLevel(ScriptingBool applyExpensiveChanges)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_CUSTOM_DecreaseLevel)
    SCRIPTINGAPI_STACK_CHECK(DecreaseLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DecreaseLevel)
    
    		QualitySettings& q = GetQualitySettings();
    		q.SetCurrentIndex( q.GetCurrentIndex() - 1, applyExpensiveChanges ); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropPixelLightCount()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropPixelLightCount)
    SCRIPTINGAPI_STACK_CHECK(get_pixelLightCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pixelLightCount)
    return GetQualitySettings().GetCurrent().pixelLightCount;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropPixelLightCount(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropPixelLightCount)
    SCRIPTINGAPI_STACK_CHECK(set_pixelLightCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pixelLightCount)
     GetQualitySettings().SetPixelLightCount(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ShadowProjection SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropShadowProjection()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropShadowProjection)
    SCRIPTINGAPI_STACK_CHECK(get_shadowProjection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowProjection)
    return (ShadowProjection)GetQualitySettings().GetCurrent().shadowProjection;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropShadowProjection(ShadowProjection value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropShadowProjection)
    SCRIPTINGAPI_STACK_CHECK(set_shadowProjection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowProjection)
     GetQualitySettings().SetShadowProjection((ShadowProjection)value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropShadowCascades()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropShadowCascades)
    SCRIPTINGAPI_STACK_CHECK(get_shadowCascades)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowCascades)
    return GetQualitySettings().GetCurrent().shadowCascades;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropShadowCascades(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropShadowCascades)
    SCRIPTINGAPI_STACK_CHECK(set_shadowCascades)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowCascades)
     GetQualitySettings().SetShadowCascades(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropShadowDistance()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropShadowDistance)
    SCRIPTINGAPI_STACK_CHECK(get_shadowDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shadowDistance)
    return GetQualitySettings().GetCurrent().shadowDistance;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropShadowDistance(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropShadowDistance)
    SCRIPTINGAPI_STACK_CHECK(set_shadowDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shadowDistance)
     GetQualitySettings().SetShadowDistance(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropMasterTextureLimit()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropMasterTextureLimit)
    SCRIPTINGAPI_STACK_CHECK(get_masterTextureLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_masterTextureLimit)
    return GetQualitySettings().GetCurrent().textureQuality;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropMasterTextureLimit(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropMasterTextureLimit)
    SCRIPTINGAPI_STACK_CHECK(set_masterTextureLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_masterTextureLimit)
     GetQualitySettings().SetMasterTextureLimit(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
Texture::AnisotropicFiltering SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropAnisotropicFiltering()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropAnisotropicFiltering)
    SCRIPTINGAPI_STACK_CHECK(get_anisotropicFiltering)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anisotropicFiltering)
    return Texture::GetAnisoLimit ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropAnisotropicFiltering(Texture::AnisotropicFiltering value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropAnisotropicFiltering)
    SCRIPTINGAPI_STACK_CHECK(set_anisotropicFiltering)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_anisotropicFiltering)
     Texture::SetAnisoLimit (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropLodBias()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropLodBias)
    SCRIPTINGAPI_STACK_CHECK(get_lodBias)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lodBias)
    return GetQualitySettings().GetCurrent().lodBias;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropLodBias(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropLodBias)
    SCRIPTINGAPI_STACK_CHECK(set_lodBias)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lodBias)
     GetQualitySettings().SetLODBias (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropMaximumLODLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropMaximumLODLevel)
    SCRIPTINGAPI_STACK_CHECK(get_maximumLODLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maximumLODLevel)
    return GetQualitySettings().GetCurrent().maximumLODLevel;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropMaximumLODLevel(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropMaximumLODLevel)
    SCRIPTINGAPI_STACK_CHECK(set_maximumLODLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maximumLODLevel)
     GetQualitySettings().SetMaximumLODLevel (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropParticleRaycastBudget()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropParticleRaycastBudget)
    SCRIPTINGAPI_STACK_CHECK(get_particleRaycastBudget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_particleRaycastBudget)
    return GetQualitySettings().GetCurrent().particleRaycastBudget;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropParticleRaycastBudget(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropParticleRaycastBudget)
    SCRIPTINGAPI_STACK_CHECK(set_particleRaycastBudget)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_particleRaycastBudget)
     GetQualitySettings().SetParticleRaycastBudget (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropSoftVegetation()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropSoftVegetation)
    SCRIPTINGAPI_STACK_CHECK(get_softVegetation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_softVegetation)
    return GetQualitySettings().GetCurrent().softVegetation;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropSoftVegetation(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropSoftVegetation)
    SCRIPTINGAPI_STACK_CHECK(set_softVegetation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_softVegetation)
     GetQualitySettings().SetSoftVegetation (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropMaxQueuedFrames()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropMaxQueuedFrames)
    SCRIPTINGAPI_STACK_CHECK(get_maxQueuedFrames)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxQueuedFrames)
    return GetGfxDevice().GetMaxBufferedFrames();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropMaxQueuedFrames(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropMaxQueuedFrames)
    SCRIPTINGAPI_STACK_CHECK(set_maxQueuedFrames)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxQueuedFrames)
     GetGfxDevice().SetMaxBufferedFrames (clamp(value, -1, 10)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropVSyncCount()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropVSyncCount)
    SCRIPTINGAPI_STACK_CHECK(get_vSyncCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_vSyncCount)
    return GetQualitySettings().GetCurrent().vSyncCount;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropVSyncCount(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropVSyncCount)
    SCRIPTINGAPI_STACK_CHECK(set_vSyncCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_vSyncCount)
     GetQualitySettings().SetVSyncCount(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropAntiAliasing()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropAntiAliasing)
    SCRIPTINGAPI_STACK_CHECK(get_antiAliasing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_antiAliasing)
    return GetQualitySettings().GetCurrent().antiAliasing;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropAntiAliasing(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropAntiAliasing)
    SCRIPTINGAPI_STACK_CHECK(set_antiAliasing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_antiAliasing)
     GetQualitySettings().SetAntiAliasing(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ColorSpace SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropDesiredColorSpace()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropDesiredColorSpace)
    SCRIPTINGAPI_STACK_CHECK(get_desiredColorSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_desiredColorSpace)
    return GetPlayerSettings().GetDesiredColorSpace ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ColorSpace SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropActiveColorSpace()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropActiveColorSpace)
    SCRIPTINGAPI_STACK_CHECK(get_activeColorSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_activeColorSpace)
    return GetPlayerSettings().GetValidatedColorSpace ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION QualitySettings_Get_Custom_PropBlendWeights()
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Get_Custom_PropBlendWeights)
    SCRIPTINGAPI_STACK_CHECK(get_blendWeights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_blendWeights)
    return GetQualitySettings().GetCurrent().blendWeights;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION QualitySettings_Set_Custom_PropBlendWeights(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(QualitySettings_Set_Custom_PropBlendWeights)
    SCRIPTINGAPI_STACK_CHECK(set_blendWeights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_blendWeights)
     GetQualitySettings().SetBlendWeights(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Shader_CUSTOM_Find(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_Find)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Find)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Find)
    
    		return Scripting::ScriptingWrapperFor(GetScriptMapper().FindShader(name));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Shader_CUSTOM_FindBuiltin(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_FindBuiltin)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(FindBuiltin)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindBuiltin)
    
    		return Scripting::ScriptingWrapperFor(GetBuiltinResource<Shader> (name));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Shader_Get_Custom_PropIsSupported(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Get_Custom_PropIsSupported)
    ReadOnlyScriptingObjectOfType<Shader> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isSupported)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isSupported)
    return self->IsSupported ();
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Shader_Get_Custom_PropCustomEditor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Get_Custom_PropCustomEditor)
    ReadOnlyScriptingObjectOfType<Shader> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_customEditor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_customEditor)
    return CreateScriptingString (self->GetCustomEditorName());
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_EnableKeyword(ICallType_String_Argument keyword_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_EnableKeyword)
    ICallType_String_Local keyword(keyword_);
    UNUSED(keyword);
    SCRIPTINGAPI_STACK_CHECK(EnableKeyword)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(EnableKeyword)
    
    		g_ShaderKeywords.Enable( keywords::Create( keyword ) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_DisableKeyword(ICallType_String_Argument keyword_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_DisableKeyword)
    ICallType_String_Local keyword(keyword_);
    UNUSED(keyword);
    SCRIPTINGAPI_STACK_CHECK(DisableKeyword)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DisableKeyword)
    
    		g_ShaderKeywords.Disable( keywords::Create( keyword ) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Shader_Get_Custom_PropMaximumLOD(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Get_Custom_PropMaximumLOD)
    ReadOnlyScriptingObjectOfType<Shader> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maximumLOD)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maximumLOD)
    return self->GetMaximumShaderLOD ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_Set_Custom_PropMaximumLOD(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Set_Custom_PropMaximumLOD)
    ReadOnlyScriptingObjectOfType<Shader> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maximumLOD)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maximumLOD)
    
    self->SetMaximumShaderLOD (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Shader_Get_Custom_PropGlobalMaximumLOD()
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Get_Custom_PropGlobalMaximumLOD)
    SCRIPTINGAPI_STACK_CHECK(get_globalMaximumLOD)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_globalMaximumLOD)
    return Shader::GetGlobalMaximumShaderLOD();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_Set_Custom_PropGlobalMaximumLOD(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Set_Custom_PropGlobalMaximumLOD)
    SCRIPTINGAPI_STACK_CHECK(set_globalMaximumLOD)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_globalMaximumLOD)
     Shader::SetGLobalMaximumShaderLOD (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Shader_Get_Custom_PropRenderQueue(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_Get_Custom_PropRenderQueue)
    ReadOnlyScriptingObjectOfType<Shader> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderQueue)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderQueue)
    return self->GetShaderLabShader()->GetRenderQueue();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_INTERNAL_CALL_SetGlobalColor(ICallType_String_Argument propertyName_, const ColorRGBAf& color)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_INTERNAL_CALL_SetGlobalColor)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetGlobalColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetGlobalColor)
    
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->SetVector (ScriptingStringToProperty (propertyName), color.GetPtr ());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_SetGlobalFloat(ICallType_String_Argument propertyName_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_SetGlobalFloat)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(SetGlobalFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalFloat)
    
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->SetFloat (ScriptingStringToProperty (propertyName), value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_SetGlobalTexture(ICallType_String_Argument propertyName_, ICallType_ReadOnlyUnityEngineObject_Argument tex_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_SetGlobalTexture)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    ReadOnlyScriptingObjectOfType<Texture> tex(tex_);
    UNUSED(tex);
    SCRIPTINGAPI_STACK_CHECK(SetGlobalTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalTexture)
    
    		Texture& texture = *tex;
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->SetTexture (ScriptingStringToProperty (propertyName), &texture);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_INTERNAL_CALL_SetGlobalMatrix(ICallType_String_Argument propertyName_, const Matrix4x4f& mat)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_INTERNAL_CALL_SetGlobalMatrix)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetGlobalMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetGlobalMatrix)
    
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->SetValueProp (ScriptingStringToProperty (propertyName), 16, mat.GetPtr());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_SetGlobalTexGenMode(ICallType_String_Argument propertyName_, int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_SetGlobalTexGenMode)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(SetGlobalTexGenMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalTexGenMode)
    
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->GetTexEnv(ScriptingStringToProperty (propertyName))->SetTexGen ((TexGenMode)mode);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_SetGlobalTextureMatrixName(ICallType_String_Argument propertyName_, ICallType_String_Argument matrixName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_SetGlobalTextureMatrixName)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    ICallType_String_Local matrixName(matrixName_);
    UNUSED(matrixName);
    SCRIPTINGAPI_STACK_CHECK(SetGlobalTextureMatrixName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalTextureMatrixName)
    
    		if(propertyName.Length() == 0)
    		{
    			ErrorString ("SetGlobalTextureMatrixName: Invalid empty propertyName");
    			return;
    		}
    		if(matrixName.Length() == 0)
    		{
    			ErrorString ("SetGlobalTextureMatrixName: Invalid empty matrixName");
    			return;
    		}
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->GetTexEnv(ScriptingStringToProperty (propertyName))->SetMatrixName (ScriptingStringToProperty (matrixName));
    	
}

#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_SetGlobalBuffer(ICallType_String_Argument propertyName_, ICallType_Object_Argument buffer_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_SetGlobalBuffer)
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    ScriptingObjectWithIntPtrField<ComputeBuffer> buffer(buffer_);
    UNUSED(buffer);
    SCRIPTINGAPI_STACK_CHECK(SetGlobalBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalBuffer)
    
    		ShaderLab::PropertySheet *props = ShaderLab::g_GlobalProperties;
    		props->SetComputeBuffer (ScriptingStringToProperty (propertyName), buffer ? buffer->GetBufferHandle() : ComputeBufferID());
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Shader_CUSTOM_PropertyToID(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_PropertyToID)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(PropertyToID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PropertyToID)
    
    		return ScriptingStringToProperty (name).index;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Shader_CUSTOM_WarmupAllShaders()
{
    SCRIPTINGAPI_ETW_ENTRY(Shader_CUSTOM_WarmupAllShaders)
    SCRIPTINGAPI_STACK_CHECK(WarmupAllShaders)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(WarmupAllShaders)
    
    		WarmupAllShaders ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_Get_Custom_PropMasterTextureLimit()
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Get_Custom_PropMasterTextureLimit)
    SCRIPTINGAPI_STACK_CHECK(get_masterTextureLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_masterTextureLimit)
    return Texture::GetMasterTextureLimit ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_Set_Custom_PropMasterTextureLimit(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Set_Custom_PropMasterTextureLimit)
    SCRIPTINGAPI_STACK_CHECK(set_masterTextureLimit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_masterTextureLimit)
     Texture::SetMasterTextureLimit (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
Texture::AnisotropicFiltering SCRIPT_CALL_CONVENTION Texture_Get_Custom_PropAnisotropicFiltering()
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Get_Custom_PropAnisotropicFiltering)
    SCRIPTINGAPI_STACK_CHECK(get_anisotropicFiltering)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anisotropicFiltering)
    return Texture::GetAnisoLimit ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_Set_Custom_PropAnisotropicFiltering(Texture::AnisotropicFiltering value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Set_Custom_PropAnisotropicFiltering)
    SCRIPTINGAPI_STACK_CHECK(set_anisotropicFiltering)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_anisotropicFiltering)
     Texture::SetAnisoLimit (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_CUSTOM_SetGlobalAnisotropicFilteringLimits(int forcedMin, int globalMax)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_CUSTOM_SetGlobalAnisotropicFilteringLimits)
    SCRIPTINGAPI_STACK_CHECK(SetGlobalAnisotropicFilteringLimits)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalAnisotropicFilteringLimits)
    
    		Texture::SetGlobalAnisoLimits(forcedMin, globalMax);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_CUSTOM_Internal_GetWidth(ICallType_ReadOnlyUnityEngineObject_Argument mono_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_CUSTOM_Internal_GetWidth)
    ReadOnlyScriptingObjectOfType<Texture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetWidth)
     return mono->GetDataWidth(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_CUSTOM_Internal_GetHeight(ICallType_ReadOnlyUnityEngineObject_Argument mono_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_CUSTOM_Internal_GetHeight)
    ReadOnlyScriptingObjectOfType<Texture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetHeight)
     return mono->GetDataHeight(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_Get_Custom_PropFilterMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Get_Custom_PropFilterMode)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_filterMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_filterMode)
    return self->GetFilterMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_Set_Custom_PropFilterMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Set_Custom_PropFilterMode)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_filterMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_filterMode)
    
    self->SetFilterMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_Get_Custom_PropAnisoLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Get_Custom_PropAnisoLevel)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_anisoLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anisoLevel)
    return self->GetAnisoLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_Set_Custom_PropAnisoLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Set_Custom_PropAnisoLevel)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_anisoLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_anisoLevel)
    
    self->SetAnisoLevel (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_Get_Custom_PropWrapMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Get_Custom_PropWrapMode)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wrapMode)
    return self->GetWrapMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_Set_Custom_PropWrapMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Set_Custom_PropWrapMode)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wrapMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wrapMode)
    
    self->SetWrapMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Texture_Get_Custom_PropMipMapBias(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Get_Custom_PropMipMapBias)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mipMapBias)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mipMapBias)
    return self->GetMipMapBias ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_Set_Custom_PropMipMapBias(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_Set_Custom_PropMipMapBias)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_mipMapBias)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mipMapBias)
    
    self->SetMipMapBias (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_CUSTOM_Internal_GetTexelSize(ICallType_ReadOnlyUnityEngineObject_Argument tex_, Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_CUSTOM_Internal_GetTexelSize)
    ReadOnlyScriptingObjectOfType<Texture> tex(tex_);
    UNUSED(tex);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetTexelSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetTexelSize)
    
    		Texture& texture = *tex;
    		output->x = texture.GetTexelSizeX();
    		output->y = texture.GetTexelSizeY();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture_CUSTOM_INTERNAL_CALL_GetNativeTexturePtr(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_CUSTOM_INTERNAL_CALL_GetNativeTexturePtr)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetNativeTexturePtr)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetNativeTexturePtr)
    
    		{ returnValue =(self->GetNativeTexturePtr()); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture_CUSTOM_GetNativeTextureID(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture_CUSTOM_GetNativeTextureID)
    ReadOnlyScriptingObjectOfType<Texture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetNativeTextureID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNativeTextureID)
    
    	#if ENABLE_TEXTUREID_MAP
    		return TextureIdMap::QueryNativeTexture(self->GetTextureID());
    	#else
    		return self->GetTextureID().m_ID;
    	#endif
    	
}


 #define CHECK_READABLE { if (!self->GetIsReadable()) RaiseMonoException("Texture '%s' is not readable, the texture memory can not be accessed from scripts. You can make the texture readable in the Texture Import Settings.", self->GetName()); }

 static void CheckReadAllowedAndThrow(Texture2D *tex)
	{
#if ENABLE_MONO && ENABLE_SECURITY
		if ( !tex->GetReadAllowed() )
			RaiseSecurityException("No read access to the texture data: %s", tex->GetName());
#endif
	}

 static void CheckScreenReadAllowedAndThrow()
	{
		if ( !Texture2D::GetScreenReadAllowed() )
			RaiseSecurityException("Reading from the screen is not allowed when you have used a downloaded texture without proper crossdomain.xml authorization");
	}
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture2D_Get_Custom_PropMipmapCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_Get_Custom_PropMipmapCount)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mipmapCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mipmapCount)
    return self->CountDataMipmaps ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_Internal_Create(ICallType_Object_Argument mono_, int width, int height, TextureFormat format, ScriptingBool mipmap, ScriptingBool linear, ICallType_IntPtr_Argument nativeTex_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_Internal_Create)
    ScriptingObjectOfType<Texture2D> mono(mono_);
    UNUSED(mono);
    void* nativeTex(nativeTex_);
    UNUSED(nativeTex);
    SCRIPTINGAPI_STACK_CHECK(Internal_Create)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Create)
    
    		if(!GetBuildSettings().hasAdvancedVersion && nativeTex != 0)
    		{
    			RaiseMonoException("Creating texture from native texture is PRO only.");
    			return;
    		}
    
    		Texture2D* texture = NEW_OBJECT_MAIN_THREAD (Texture2D);
    		texture->Reset();
    
    		if (texture->InitTexture(width, height, format, mipmap ? Texture2D::kFlagCreateMipChain : Texture2D::kFlagNone, 1, (intptr_t)nativeTex))
    		{
    			texture->SetStoredColorSpace (linear ? kTexColorSpaceLinear : kTexColorSpaceSRGB);
    			ConnectScriptingWrapperToObject (mono.GetScriptingObject(), texture);
    			texture->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    		}
    		else
    		{
    			DestroySingleObject (texture);
    			RaiseMonoException("Failed to create texture because of invalid parameters.");
    		}
    	
}

#if ENABLE_TEXTUREID_MAP
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_UpdateExternalTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_IntPtr_Argument nativeTex_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_UpdateExternalTexture)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    void* nativeTex(nativeTex_);
    UNUSED(nativeTex);
    SCRIPTINGAPI_STACK_CHECK(UpdateExternalTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UpdateExternalTexture)
    
    		if(!GetBuildSettings().hasAdvancedVersion)
    		{
    			RaiseMonoException("Updating native texture is PRO only.");
    			return;
    		}
    		GetGfxDevice().UpdateExternalTextureFromNative(self->GetTextureID(), (intptr_t)nativeTex);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
TextureFormat SCRIPT_CALL_CONVENTION Texture2D_Get_Custom_PropFormat(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_Get_Custom_PropFormat)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_format)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_format)
    return self->GetTextureFormat ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_INTERNAL_CALL_SetPixel(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, const ColorRGBAf& color)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_INTERNAL_CALL_SetPixel)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetPixel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetPixel)
    
    		CHECK_READABLE
    		self->SetPixel (0, x, y, color);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_INTERNAL_CALL_GetPixel(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, ColorRGBAf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_INTERNAL_CALL_GetPixel)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetPixel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetPixel)
    
    		CHECK_READABLE
    		CheckReadAllowedAndThrow(self);
    		{ returnValue =(self->GetPixel (0, x, y)); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_INTERNAL_CALL_GetPixelBilinear(ICallType_ReadOnlyUnityEngineObject_Argument self_, float u, float v, ColorRGBAf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_INTERNAL_CALL_GetPixelBilinear)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetPixelBilinear)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetPixelBilinear)
    
    		CHECK_READABLE
    		CheckReadAllowedAndThrow(self);
    		{ returnValue =(self->GetPixelBilinear (0, u, v)); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_SetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, int blockWidth, int blockHeight, ICallType_Array_Argument colors_, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_SetPixels)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    ICallType_Array_Local colors(colors_);
    UNUSED(colors);
    SCRIPTINGAPI_STACK_CHECK(SetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPixels)
    
    		CHECK_READABLE
    		self->SetPixels( x, y, blockWidth, blockHeight, GetScriptingArraySize(colors), GetScriptingArrayStart<ColorRGBAf>(colors), miplevel );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_SetPixels32(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument colors_, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_SetPixels32)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    ICallType_Array_Local colors(colors_);
    UNUSED(colors);
    SCRIPTINGAPI_STACK_CHECK(SetPixels32)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPixels32)
    
    		CHECK_READABLE
    		self->SetPixels32( miplevel, GetScriptingArrayStart<ColorRGBA32>(colors), GetScriptingArraySize(colors) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_LoadImage(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument data_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_LoadImage)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    ICallType_Array_Local data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(LoadImage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadImage)
    
    		return LoadMemoryBufferIntoTexture(*self, GetScriptingArrayStart<UInt8>(data), GetScriptingArraySize(data), IsCompressedDXTTextureFormat(self->GetTextureFormat())?kLoadImageDXTCompressDithered:kLoadImageUncompressed);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_GetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, int blockWidth, int blockHeight, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_GetPixels)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPixels)
    
    		CHECK_READABLE
    		CheckReadAllowedAndThrow(self);
    
    		int res = blockWidth * blockHeight;
    		if (blockWidth != 0 && blockHeight != res / blockWidth) {
    			return SCRIPTING_NULL;
    		}
    
    
    		ScriptingArrayPtr colors = CreateScriptingArray<ColorRGBAf>(GetMonoManager().GetCommonClasses().color, blockWidth * blockHeight);
    		ColorRGBAf* firstElement = GetScriptingArrayStart<ColorRGBAf>(colors);
    		self->GetPixels( x, y, blockWidth, blockHeight, miplevel,  firstElement);
    		return colors;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_GetPixels32(ICallType_ReadOnlyUnityEngineObject_Argument self_, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_GetPixels32)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetPixels32)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPixels32)
    
    		CHECK_READABLE
    		CheckReadAllowedAndThrow(self);
    		int w = self->GetDataWidth() >> miplevel; if( w < 1 ) w = 1;
    		int h = self->GetDataHeight() >> miplevel; if( h < 1 ) h = 1;
    
    
    		ScriptingArrayPtr colors = CreateScriptingArray<ColorRGBA32>(GetMonoManager().GetCommonClasses().color32, w * h);
    		ColorRGBA32* firstElement = GetScriptingArrayStart<ColorRGBA32>(colors);
    		self->GetPixels32( miplevel, firstElement);
    		return colors;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_Apply(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool updateMipmaps, ScriptingBool makeNoLongerReadable)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_Apply)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Apply)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Apply)
    
    		CHECK_READABLE
    		self->Apply(updateMipmaps, makeNoLongerReadable);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_Resize(ICallType_ReadOnlyUnityEngineObject_Argument self_, int width, int height, TextureFormat format, ScriptingBool hasMipMap)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_Resize)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Resize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Resize)
    
    		return self->ResizeWithFormat (width, height, format, hasMipMap ? Texture2D::kFlagCreateMipChain : Texture2D::kFlagNone);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_Internal_ResizeWH(ICallType_ReadOnlyUnityEngineObject_Argument self_, int width, int height)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_Internal_ResizeWH)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_ResizeWH)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ResizeWH)
    
    		CHECK_READABLE
    		return self->Resize(width, height);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_INTERNAL_CALL_Compress(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool highQuality)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_INTERNAL_CALL_Compress)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Compress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Compress)
    return self->Compress(highQuality);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_PackTextures(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument textures_, int padding, int maximumAtlasSize, ScriptingBool makeNoLongerReadable)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_PackTextures)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    ICallType_Array_Local textures(textures_);
    UNUSED(textures);
    SCRIPTINGAPI_STACK_CHECK(PackTextures)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PackTextures)
    
    		int textureCount = GetScriptingArraySize(textures);
    		Texture2D** texturePtrs = new Texture2D*[textureCount];
    		for( int i = 0; i < textureCount; ++i )
    		{
    			Texture2D* tex = ScriptingObjectToObject<Texture2D>( GetScriptingArrayElementNoRef<ScriptingObjectPtr>(textures,i));
    			if (tex && !tex->GetIsReadable())
    			{
    				ErrorString("Texture atlas needs textures to have Readable flag set!");
    				tex = NULL;
    			}
    			texturePtrs[i] = tex;
    		}
    
    		ScriptingArrayPtr rects = CreateScriptingArray<Rectf>(GetMonoManager().GetCommonClasses().rect, textureCount);
    		Rectf* firstElement = GetScriptingArrayStart<Rectf>(rects);
    
    		if( !PackTextureAtlasSimple( &*self, maximumAtlasSize, textureCount, texturePtrs, firstElement, padding, true, makeNoLongerReadable ) )
    		{
    			delete[] texturePtrs; 
    			return SCRIPTING_NULL;
    		}
    
    		delete[] texturePtrs;
    		return rects;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_INTERNAL_CALL_ReadPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Rectf& source, int destX, int destY, ScriptingBool recalculateMipMaps)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_INTERNAL_CALL_ReadPixels)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ReadPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ReadPixels)
    
    		#if !UNITY_FLASH
    		CHECK_READABLE
    		CheckScreenReadAllowedAndThrow();
    		bool flipVertical = GetGfxDevice().GetInvertProjectionMatrix();
    		self->ReadPixels (0, (int)source.x, (int)source.y, (int)source.Width(), (int)source.Height(), destX, destY, flipVertical, recalculateMipMaps);
    		#elif UNITY_FLASH
    		printf_console("ReadPixels is unsupported on Flash");
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Texture2D_CUSTOM_EncodeToPNG(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_CUSTOM_EncodeToPNG)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(EncodeToPNG)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(EncodeToPNG)
    
    		#if ENABLE_PNG_JPG
    		CHECK_READABLE
    		CheckReadAllowedAndThrow(self);
    
    		Texture2D* tex = self;
    		if( !tex )
    		{
    			ErrorString( "EncodeToPNG failed: texture is null" );
    			return SCRIPTING_NULL;
    		}
    		dynamic_array<UInt8> buffer;
    		if( !tex->EncodeToPNG( buffer ) )
    		{
    			return SCRIPTING_NULL;
    		}
    		return CreateScriptingArray<UInt8>( &buffer[0], buffer.size(), GetMonoManager().GetCommonClasses().byte );
    		#else
    		return SCRIPTING_NULL;
    		#endif
    	
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Texture2D_Get_Custom_PropAlphaIsTransparency(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_Get_Custom_PropAlphaIsTransparency)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphaIsTransparency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alphaIsTransparency)
    return self->GetAlphaIsTransparency ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture2D_Set_Custom_PropAlphaIsTransparency(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture2D_Set_Custom_PropAlphaIsTransparency)
    ReadOnlyScriptingObjectOfType<Texture2D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_alphaIsTransparency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alphaIsTransparency)
    
    self->SetAlphaIsTransparency (value);
    
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_INTERNAL_CALL_SetPixel(ICallType_ReadOnlyUnityEngineObject_Argument self_, int face, int x, int y, const ColorRGBAf& color)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_INTERNAL_CALL_SetPixel)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetPixel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetPixel)
    
    		CHECK_READABLE
    		self->SetPixel (face, x, y, color);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_INTERNAL_CALL_GetPixel(ICallType_ReadOnlyUnityEngineObject_Argument self_, int face, int x, int y, ColorRGBAf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_INTERNAL_CALL_GetPixel)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetPixel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetPixel)
    
    		CHECK_READABLE
    		{ returnValue =(self->GetPixel (face, x, y)); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_GetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, int face, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_GetPixels)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPixels)
    
    		CHECK_READABLE
    		int size = std::max(self->GetDataWidth() >> miplevel, 1);
    
    		ScriptingArrayPtr colors = CreateScriptingArray<ColorRGBAf>(GetMonoManager().GetCommonClasses().color, size*size);
    		ColorRGBAf* firstElement = GetScriptingArrayStart<ColorRGBAf>(colors);
    		self->GetPixels( 0, 0, size, size, miplevel,  firstElement, (int)face );
    		return colors;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_SetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument colors_, int face, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_SetPixels)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    ICallType_Array_Local colors(colors_);
    UNUSED(colors);
    SCRIPTINGAPI_STACK_CHECK(SetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPixels)
    
    		CHECK_READABLE
    		int size = std::max(self->GetDataWidth() >> miplevel, 1);
    		self->SetPixels( 0, 0, size, size, GetScriptingArraySize(colors), GetScriptingArrayStart<ColorRGBAf>(colors), miplevel, (int)face );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_Apply(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool updateMipmaps, ScriptingBool makeNoLongerReadable)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_Apply)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Apply)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Apply)
    
    	#if !UNITY_EDITOR
    		CHECK_READABLE
    	#endif
    
    		if(makeNoLongerReadable)
    	{
    			self->SetIsReadable(false);
    			self->SetIsUnreloadable(true);
    		}
    
    		if (updateMipmaps)
    			self->UpdateImageData();
    		else
    			self->UpdateImageDataDontTouchMipmap();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
TextureFormat SCRIPT_CALL_CONVENTION Cubemap_Get_Custom_PropFormat(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_Get_Custom_PropFormat)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_format)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_format)
    return self->GetTextureFormat ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_Internal_Create(ICallType_Object_Argument mono_, int size, TextureFormat format, ScriptingBool mipmap)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_Internal_Create)
    ScriptingObjectOfType<Cubemap> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_Create)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Create)
    
    		Cubemap* texture = NEW_OBJECT_MAIN_THREAD (Cubemap);
    		texture->Reset();
    
    		if (texture->InitTexture(size, size, format, mipmap ? Texture2D::kFlagCreateMipChain : Texture2D::kFlagNone, 6))
    		{
    			ConnectScriptingWrapperToObject (mono.GetScriptingObject(), texture);
    			texture->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    		}
    		else
    		{
    			RaiseMonoException("Failed to create texture because of invalid parameters.");
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cubemap_CUSTOM_SmoothEdges(ICallType_ReadOnlyUnityEngineObject_Argument self_, int smoothRegionWidthInPixels)
{
    SCRIPTINGAPI_ETW_ENTRY(Cubemap_CUSTOM_SmoothEdges)
    ReadOnlyScriptingObjectOfType<Cubemap> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SmoothEdges)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SmoothEdges)
    
    		self->FixupEdges (smoothRegionWidthInPixels);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Texture3D_Get_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture3D_Get_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<Texture3D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_depth)
    return self->GetDepth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Texture3D_CUSTOM_GetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture3D_CUSTOM_GetPixels)
    ReadOnlyScriptingObjectOfType<Texture3D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPixels)
    
    		int w = std::max(self->GetDataWidth() >> miplevel, 1);
    		int h = std::max(self->GetDataHeight() >> miplevel, 1);
    		int d = std::max(self->GetDepth() >> miplevel, 1);
    
    		ScriptingArrayPtr colors = CreateScriptingArray<ColorRGBAf>(GetMonoManager().GetCommonClasses().color, w*h*d);
    		ColorRGBAf* firstElement = GetScriptingArrayStart<ColorRGBAf>(colors);
    		self->GetPixels (firstElement, miplevel);
    		return colors;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture3D_CUSTOM_SetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument colors_, int miplevel)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture3D_CUSTOM_SetPixels)
    ReadOnlyScriptingObjectOfType<Texture3D> self(self_);
    UNUSED(self);
    ICallType_Array_Local colors(colors_);
    UNUSED(colors);
    SCRIPTINGAPI_STACK_CHECK(SetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPixels)
    
    		int arrSize = GetScriptingArraySize(colors);
    		const ColorRGBAf* ptr = GetScriptingArrayStart<ColorRGBAf>(colors);
    		self->SetPixels (arrSize, ptr, miplevel);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture3D_CUSTOM_Apply(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool updateMipmaps)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture3D_CUSTOM_Apply)
    ReadOnlyScriptingObjectOfType<Texture3D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Apply)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Apply)
    
    		self->UpdateImageData (updateMipmaps);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
TextureFormat SCRIPT_CALL_CONVENTION Texture3D_Get_Custom_PropFormat(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture3D_Get_Custom_PropFormat)
    ReadOnlyScriptingObjectOfType<Texture3D> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_format)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_format)
    return self->GetTextureFormat ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Texture3D_CUSTOM_Internal_Create(ICallType_Object_Argument mono_, int width, int height, int depth, TextureFormat format, ScriptingBool mipmap)
{
    SCRIPTINGAPI_ETW_ENTRY(Texture3D_CUSTOM_Internal_Create)
    ScriptingObjectOfType<Texture3D> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_Create)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Create)
    
    		Texture3D* texture = NEW_OBJECT_MAIN_THREAD (Texture3D);
    		texture->Reset();
    
    		if (texture->InitTexture(width, height, depth, format, mipmap))
    		{
    			ConnectScriptingWrapperToObject (mono.GetScriptingObject(), texture);
    			texture->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    		}
    		else
    		{
    			RaiseMonoException("Failed to create texture because of invalid parameters.");
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION MeshFilter_Get_Custom_PropMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshFilter_Get_Custom_PropMesh)
    ReadOnlyScriptingObjectOfType<MeshFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mesh)
    return Scripting::ScriptingWrapperFor(self->GetInstantiatedMesh());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MeshFilter_Set_Custom_PropMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshFilter_Set_Custom_PropMesh)
    ReadOnlyScriptingObjectOfType<MeshFilter> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_mesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mesh)
    
    self->SetInstantiatedMesh (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION MeshFilter_Get_Custom_PropSharedMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshFilter_Get_Custom_PropSharedMesh)
    ReadOnlyScriptingObjectOfType<MeshFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sharedMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sharedMesh)
    return Scripting::ScriptingWrapperFor(self->GetSharedMesh());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MeshFilter_Set_Custom_PropSharedMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(MeshFilter_Set_Custom_PropSharedMesh)
    ReadOnlyScriptingObjectOfType<MeshFilter> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_sharedMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sharedMesh)
    
    self->SetSharedMesh (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION CombineInstance_CUSTOM_InternalGetMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, int instanceID)
{
    SCRIPTINGAPI_ETW_ENTRY(CombineInstance_CUSTOM_InternalGetMesh)
    ReadOnlyScriptingObjectOfType<CombineInstance> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InternalGetMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalGetMesh)
    
    		return instanceID != 0 ? Scripting::ScriptingWrapperFor<Mesh>(PPtr<Mesh>(instanceID)) : SCRIPTING_NULL;
    	
}



struct MonoCombineInstance
{
	int meshInstanceID;
	int subMeshIndex;
	Matrix4x4f transform;
};

 struct MonoMeshBlendShapeVertex
{
	Vector3f vertex;
	Vector3f normal;
	Vector3f tangent;
	int index;
};

 struct MonoMeshBlendShape
{
	ScriptingStringPtr name;
	ScriptingArrayPtr vertices;
};

void MeshBlendShapeVertexToMono (const MeshBlendShapeVertex &src, MonoMeshBlendShapeVertex &dest) {
	dest.vertex = src.vertex;
	dest.normal = src.normal;
	dest.tangent = src.tangent;
	dest.index = src.index;
}
void MeshBlendShapeVertexToCpp (const MonoMeshBlendShapeVertex &src, MeshBlendShapeVertex &dest) {
	dest.vertex = src.vertex;
	dest.normal = src.normal;
	dest.tangent = src.tangent;
	dest.index = src.index;
}

class MeshBlendShapeToMono
{
public:
	MeshBlendShapeToMono(const MeshBlendShape::VertexContainer& sharedVertices_) : sharedVertices(sharedVertices_) {}

	void operator() (const MeshBlendShape &src, MonoMeshBlendShape &dest)
	{
		dest.name = CreateScriptingString(src.name);
		const MeshBlendShape::VertexContainer vertices(sharedVertices.begin() + src.firstVertex, sharedVertices.begin() + src.firstVertex + src.vertexCount);

		ScriptingTypePtr classVertex = GetScriptingTypeRegistry().GetType("UnityEngine", "MeshBlendShapeVertex");
		dest.vertices = VectorToScriptingStructArray<MeshBlendShapeVertex, MonoMeshBlendShapeVertex>(vertices, classVertex, MeshBlendShapeVertexToMono);
	}

private:
	const MeshBlendShape::VertexContainer& sharedVertices;
};

class MeshBlendShapeToCpp
{
public:
	MeshBlendShapeToCpp(int meshVertexCount_, MeshBlendShape::VertexContainer& sharedVertices_) : meshVertexCount(meshVertexCount_), sharedVertices(sharedVertices_) {}

	void operator() (MonoMeshBlendShape &src, MeshBlendShape &dest)
	{
		dest.name = scripting_cpp_string_for(src.name);

		MeshBlendShape::VertexContainer vertices;
		ScriptingStructArrayToVector<MeshBlendShapeVertex, MonoMeshBlendShapeVertex>(src.vertices, vertices, MeshBlendShapeVertexToCpp);

		for (MeshBlendShape::VertexContainer::iterator it = vertices.begin(), end = vertices.end(); it != end; ++it)
		{
			MeshBlendShapeVertex& v = *it;
			if (v.index < 0 || v.index >= meshVertexCount)
			{
				ErrorStringMsg("Value (%d) of MeshBlendShapeVertex.index #%d is out of bounds (Mesh vertex count: %d) on BlendShape '%s'. It will be reset to 0.", v.index, it - vertices.begin(), meshVertexCount, dest.name.c_str());
				v.index = 0;
			}
		}

		dest.firstVertex = sharedVertices.size();
		dest.vertexCount = vertices.size();

		sharedVertices.insert(sharedVertices.end(), vertices.begin(), vertices.end());
		dest.UpdateFlags(sharedVertices);
	}

private:
	int meshVertexCount;
	MeshBlendShape::VertexContainer& sharedVertices;
};




SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_Internal_Create(ICallType_Object_Argument mono_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_Internal_Create)
    ScriptingObjectOfType<Mesh> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_Create)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Create)
    
    		Mesh* mesh = NEW_OBJECT_MAIN_THREAD (Mesh);
    		mesh->Reset();
    		ConnectScriptingWrapperToObject (mono.GetScriptingObject(), mesh);
    		mesh->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_Clear(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool keepVertexLayout)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_Clear)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Clear)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Clear)
    
    		self->Clear (keepVertexLayout);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropIsReadable(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropIsReadable)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isReadable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isReadable)
    return self->GetIsReadable ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropCanAccess(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropCanAccess)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_canAccess)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_canAccess)
    return self->CanAccessFromScript();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropVertices(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropVertices)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_vertices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_vertices)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().vector3;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelVertex))
    return CreateScriptingArrayStride<Vector3f>(self->GetChannelPointer(kShaderChannelVertex), self->GetVertexCount(), klass, self->GetStride(kShaderChannelVertex));
    }
    else
    ErrorStringMsg("Not allowed to access vertices on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropVertices(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropVertices)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_vertices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_vertices)
    
    if (self->CanAccessFromScript())
    self->SetVertices (GetScriptingArrayStart<Vector3f>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access vertices on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropNormals(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropNormals)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_normals)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_normals)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().vector3;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelNormal))
    return CreateScriptingArrayStride<Vector3f>(self->GetChannelPointer(kShaderChannelNormal), self->GetVertexCount(), klass, self->GetStride(kShaderChannelNormal));
    }
    else
    ErrorStringMsg("Not allowed to access normals on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropNormals(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropNormals)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_normals)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_normals)
    
    if (self->CanAccessFromScript())
    self->SetNormals (GetScriptingArrayStart<Vector3f>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access normals on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropTangents(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropTangents)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_tangents)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_tangents)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().vector4;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelTangent))
    return CreateScriptingArrayStride<Vector4f>(self->GetChannelPointer(kShaderChannelTangent), self->GetVertexCount(), klass, self->GetStride(kShaderChannelTangent));
    }
    else
    ErrorStringMsg("Not allowed to access tangents on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropTangents(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropTangents)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_tangents)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_tangents)
    
    if (self->CanAccessFromScript())
    self->SetTangents (GetScriptingArrayStart<Vector4f>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access tangents on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropUv(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropUv)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_uv)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_uv)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().vector2;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelTexCoord0))
    return CreateScriptingArrayStride<Vector2f>(self->GetChannelPointer(kShaderChannelTexCoord0), self->GetVertexCount(), klass, self->GetStride(kShaderChannelTexCoord0));
    }
    else
    ErrorStringMsg("Not allowed to access uv on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropUv(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropUv)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_uv)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_uv)
    
    if (self->CanAccessFromScript())
    self->SetUv (0, GetScriptingArrayStart<Vector2f>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access uv on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropUv2(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropUv2)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_uv2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_uv2)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().vector2;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelTexCoord1))
    return CreateScriptingArrayStride<Vector2f>(self->GetChannelPointer(kShaderChannelTexCoord1), self->GetVertexCount(), klass, self->GetStride(kShaderChannelTexCoord1));
    }
    else
    ErrorStringMsg("Not allowed to access uv2 on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropUv2(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropUv2)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_uv2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_uv2)
    
    if (self->CanAccessFromScript())
    self->SetUv (1, GetScriptingArrayStart<Vector2f>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access uv2 on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_INTERNAL_get_bounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_INTERNAL_get_bounds)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_bounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_bounds)
    *returnValue = self->GetBounds();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_INTERNAL_set_bounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, const AABB& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_INTERNAL_set_bounds)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_bounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_bounds)
    
    self->SetBounds (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropColors(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropColors)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_colors)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_colors)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().color;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelColor))
    {
    ScriptingArrayPtr array = CreateScriptingArray<ColorRGBAf>(klass, self->GetVertexCount());
    self->ExtractColorArrayConverting (GetScriptingArrayStart<ColorRGBAf>(array));
    return array;
    }
    }
    else
    ErrorStringMsg("Not allowed to access colors on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropColors(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropColors)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_colors)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_colors)
    
    if (self->CanAccessFromScript())
    self->SetColorsConverting (GetScriptingArrayStart<ColorRGBAf>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access colors on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropColors32(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropColors32)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_colors32)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_colors32)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().color32;
    if (self->CanAccessFromScript())
    {
    if (self->IsAvailable(kShaderChannelColor))
    {
    ScriptingArrayPtr array = CreateScriptingArray<ColorRGBA32>(klass, self->GetVertexCount());
    self->ExtractColorArray (GetScriptingArrayStart<ColorRGBA32>(array));
    return array;
    }
    }
    else
    ErrorStringMsg("Not allowed to access colors on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropColors32(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropColors32)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_colors32)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_colors32)
    
    if (self->CanAccessFromScript())
    self->SetColors (GetScriptingArrayStart<ColorRGBA32>(value), GetScriptingArraySize(value));
    else
    ErrorStringMsg("Not allowed to access colors on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_RecalculateBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_RecalculateBounds)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RecalculateBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RecalculateBounds)
    
    		if (self->CanAccessFromScript())
    			self->RecalculateBounds();
    		else
    			ErrorStringMsg("Not allowed to call RecalculateBounds() on mesh '%s'", self->GetName());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_RecalculateNormals(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_RecalculateNormals)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RecalculateNormals)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RecalculateNormals)
    
    		if (self->CanAccessFromScript())
    			self->RecalculateNormals();
    		else
    			ErrorStringMsg("Not allowed to call RecalculateNormals() on mesh '%s'", self->GetName());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_Optimize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_Optimize)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Optimize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Optimize)
    
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropTriangles(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropTriangles)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_triangles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_triangles)
    ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().int_32;
    if (self->CanAccessFromScript())
    {
    Mesh::TemporaryIndexContainer triangles;
    self->GetTriangles(triangles);
    return CreateScriptingArray(&triangles[0], triangles.size(), klass);
    }
    else
    ErrorStringMsg("Not allowed to access triangles on mesh '%s'", self->GetName());
    return CreateEmptyStructArray(klass);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropTriangles(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropTriangles)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_triangles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_triangles)
    
    if (self->CanAccessFromScript())
    {
    self->SetSubMeshCount(1);
    self->SetIndices (GetScriptingArrayStart<UInt32>(value), GetScriptingArraySize(value), 0, kPrimitiveTriangles);
    }
    else
    ErrorStringMsg("Not allowed to access triangles on mesh '%s'", self->GetName());
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_CUSTOM_GetTriangles(ICallType_ReadOnlyUnityEngineObject_Argument self_, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_GetTriangles)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetTriangles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTriangles)
    
    		ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().int_32;
    		if (self->CanAccessFromScript())
    		{
    			Mesh::TemporaryIndexContainer triangles;
    			self->GetTriangles(triangles, submesh);
    			return CreateScriptingArray(&triangles[0], triangles.size(), klass);
    		}
    		else
    			ErrorStringMsg("Not allowed to call GetTriangles() on mesh '%s'", self->GetName());
    
    		return CreateEmptyStructArray(klass);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_SetTriangles(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument triangles_, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_SetTriangles)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local triangles(triangles_);
    UNUSED(triangles);
    SCRIPTINGAPI_STACK_CHECK(SetTriangles)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetTriangles)
    
    		if (self->CanAccessFromScript())
    		{
    			self->SetIndices(GetScriptingArrayStart<UInt32>(triangles), GetScriptingArraySize(triangles), submesh, kPrimitiveTriangles);
    		}
    		else
    			ErrorStringMsg("Not allowed to call SetTriangles() on mesh '%s'", self->GetName());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_CUSTOM_GetIndices(ICallType_ReadOnlyUnityEngineObject_Argument self_, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_GetIndices)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetIndices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetIndices)
    
    		ScriptingClassPtr klass = GetMonoManager().GetCommonClasses().int_32;
    		if (self->CanAccessFromScript())
    		{
    			Mesh::TemporaryIndexContainer indices;
    			self->GetIndices (indices, submesh);
    			return CreateScriptingArray(&indices[0], indices.size(), klass);
    		}
    		else
    			ErrorStringMsg("Not allowed to call GetIndices() on mesh '%s'", self->GetName());
    
    		return CreateEmptyStructArray(klass);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_SetIndices(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument indices_, GfxPrimitiveType topology, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_SetIndices)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local indices(indices_);
    UNUSED(indices);
    SCRIPTINGAPI_STACK_CHECK(SetIndices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetIndices)
    
    		if (self->CanAccessFromScript())
    		{
    			self->SetIndices(GetScriptingArrayStart<UInt32>(indices), GetScriptingArraySize(indices), submesh, topology);
    		}
    		else
    			ErrorStringMsg("Not allowed to call SetIndices() on mesh '%s'", self->GetName());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
GfxPrimitiveType SCRIPT_CALL_CONVENTION Mesh_CUSTOM_GetTopology(ICallType_ReadOnlyUnityEngineObject_Argument self_, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_GetTopology)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetTopology)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTopology)
    
    		if ((unsigned)submesh >= self->GetSubMeshCount())
    		{
    			ErrorString("Failed getting topology. Submesh index is out of bounds.");
    			return kPrimitiveTriangles;
    		}
    		return self->GetSubMeshFast(submesh).topology;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropVertexCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropVertexCount)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_vertexCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_vertexCount)
    return self->GetVertexCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropSubMeshCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropSubMeshCount)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_subMeshCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_subMeshCount)
    return self->GetSubMeshCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropSubMeshCount(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropSubMeshCount)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_subMeshCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_subMeshCount)
    
    if(value < 0)
    {
    ErrorString ("subMeshCount can't be set to negative value");
    return;
    }
    self->SetSubMeshCount(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_SetTriangleStrip(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument triangles_, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_SetTriangleStrip)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local triangles(triangles_);
    UNUSED(triangles);
    SCRIPTINGAPI_STACK_CHECK(SetTriangleStrip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetTriangleStrip)
    
    		UInt32* triStrip = GetScriptingArrayStart<UInt32>(triangles);
    		UNITY_TEMP_VECTOR(kMemGeometry, UInt32) newTriangles;
    		Destripify(triStrip, GetScriptingArraySize(triangles), newTriangles);
    		self->SetIndices (&newTriangles[0], newTriangles.size(), submesh, kPrimitiveTriangles);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_CUSTOM_GetTriangleStrip(ICallType_ReadOnlyUnityEngineObject_Argument self_, int submesh)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_GetTriangleStrip)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetTriangleStrip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTriangleStrip)
    
    		Mesh::TemporaryIndexContainer triangles;
    		self->GetTriangles(triangles, submesh);
    
    		UNITY_TEMP_VECTOR(kMemGeometry, UInt32) strip;
    		Stripify(&triangles[0], triangles.size(), strip);
    
    		return CreateScriptingArray(&strip[0], strip.size(), GetMonoManager().GetCommonClasses().int_32);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_CombineMeshes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument combine_, ScriptingBool mergeSubMeshes, ScriptingBool useMatrices)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_CombineMeshes)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local combine(combine_);
    UNUSED(combine);
    SCRIPTINGAPI_STACK_CHECK(CombineMeshes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CombineMeshes)
    
    		CombineInstances combineVec;
    		combineVec.resize(GetScriptingArraySize(combine));
    		MonoCombineInstance* mono = GetScriptingArrayStart<MonoCombineInstance>(combine);
    		for (int i=0;i<combineVec.size();i++)
    		{
    			combineVec[i].transform = mono[i].transform;
    			combineVec[i].subMeshIndex = mono[i].subMeshIndex;
    			combineVec[i].mesh = PPtr<Mesh>(mono[i].meshInstanceID);
    		}
    		CombineMeshes(combineVec, *self, mergeSubMeshes, useMatrices);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropBoneWeights(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropBoneWeights)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_boneWeights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_boneWeights)
    int size = self->GetVertexCount();
    BoneInfluence* weights = self->GetBoneWeights();
    return CreateScriptingArray(weights, size, GetCommonScriptingClasses ().boneWeight);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropBoneWeights(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropBoneWeights)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_boneWeights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_boneWeights)
    
    self->SetBoneWeights(GetScriptingArrayStart<BoneInfluence> (value), GetScriptingArraySize(value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropBindposes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropBindposes)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bindposes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bindposes)
    return CreateScriptingArray(self->GetBindposes(), self->GetBindposeCount(), GetCommonScriptingClasses ().matrix4x4);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropBindposes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropBindposes)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_bindposes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bindposes)
    
    self->SetBindposes(GetScriptingArrayStart<Matrix4x4f> (value), GetScriptingArraySize(value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_CUSTOM_MarkDynamic(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_MarkDynamic)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(MarkDynamic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(MarkDynamic)
    
    		if (self->CanAccessFromScript())
    			self->MarkDynamic();
    	
}

#if USE_BLENDSHAPES
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropBlendShapes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropBlendShapes)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_blendShapes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_blendShapes)
    ScriptingTypePtr classBlendShape = GetScriptingTypeRegistry().GetType("UnityEngine", "MeshBlendShape");
    return VectorToScriptingStructArray<MeshBlendShape, MonoMeshBlendShape>(self->GetShapesVector(), classBlendShape, MeshBlendShapeToMono(self->GetShapeVertexVector()));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Mesh_Set_Custom_PropBlendShapes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Set_Custom_PropBlendShapes)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_blendShapes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_blendShapes)
    
    Mesh::MeshBlendShapeContainer shapes;
    self->GetShapeVertexVector().clear();
    ScriptingStructArrayToVector<MeshBlendShape, MonoMeshBlendShape>(value, shapes, MeshBlendShapeToCpp(self->GetVertexCount(), self->GetShapeVertexVector()));
    self->SwapShapesVector(shapes);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Mesh_Get_Custom_PropBlendShapeCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_Get_Custom_PropBlendShapeCount)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_blendShapeCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_blendShapeCount)
    return self->GetShapesVector().size();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Mesh_CUSTOM_GetBlendShapeName(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(Mesh_CUSTOM_GetBlendShapeName)
    ReadOnlyScriptingObjectOfType<Mesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetBlendShapeName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBlendShapeName)
    
    		const Mesh::MeshBlendShapeContainer& blendShapes = self->GetShapesVector();
    		if (index < 0 || index >= blendShapes.size())
    		{
    			ErrorStringMsg("Array index (%d) is out of bounds (size=%" PRINTF_SIZET_FORMAT ")", index, blendShapes.size());
    			return CreateScriptingString("");
    		}
    		else
    			return CreateScriptingString(blendShapes[index].name);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Get_Custom_PropBones(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Get_Custom_PropBones)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bones)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bones)
    return CreateScriptingArrayFromUnityObjects(self->GetBones(), ClassID(Transform));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Set_Custom_PropBones(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Set_Custom_PropBones)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_bones)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bones)
    
    dynamic_array<PPtr<Transform> > transforms;
    if (value != SCRIPTING_NULL)
    {
    int size = GetScriptingArraySize(value);
    transforms.resize_uninitialized(size);
    for (int i=0;i<size;i++)
    {
    int instanceID = GetInstanceIDFromScriptingWrapper(GetScriptingArrayElementNoRef<ScriptingObjectPtr>(value, i));
    transforms[i] = PPtr<Transform> (instanceID);
    }
    }
    self->SetBones(transforms);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Get_Custom_PropRootBone(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Get_Custom_PropRootBone)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rootBone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rootBone)
    return Scripting::ScriptingWrapperFor(self->GetRootBone());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Set_Custom_PropRootBone(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Set_Custom_PropRootBone)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_rootBone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_rootBone)
    
    self->SetRootBone (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Get_Custom_PropQuality(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Get_Custom_PropQuality)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_quality)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_quality)
    return self->GetQuality ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Set_Custom_PropQuality(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Set_Custom_PropQuality)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_quality)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_quality)
    
    self->SetQuality (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Get_Custom_PropSharedMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Get_Custom_PropSharedMesh)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sharedMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sharedMesh)
    return Scripting::ScriptingWrapperFor(self->GetMesh());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Set_Custom_PropSharedMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Set_Custom_PropSharedMesh)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_sharedMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sharedMesh)
    
    self->SetMesh (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Get_Custom_PropUpdateWhenOffscreen(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Get_Custom_PropUpdateWhenOffscreen)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_updateWhenOffscreen)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_updateWhenOffscreen)
    return self->GetUpdateWhenOffscreen ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Set_Custom_PropUpdateWhenOffscreen(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Set_Custom_PropUpdateWhenOffscreen)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_updateWhenOffscreen)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_updateWhenOffscreen)
    
    self->SetUpdateWhenOffscreen (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_CUSTOM_INTERNAL_get_localBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_CUSTOM_INTERNAL_get_localBounds)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localBounds)
    AABB result;
    self->GetSkinnedMeshLocalAABB(result);
    { *returnValue =(result); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_CUSTOM_INTERNAL_set_localBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, const AABB& value)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_CUSTOM_INTERNAL_set_localBounds)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localBounds)
    
    self->SetLocalAABB(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_CUSTOM_BakeMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument mesh_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_CUSTOM_BakeMesh)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Mesh> mesh(mesh_);
    UNUSED(mesh);
    SCRIPTINGAPI_STACK_CHECK(BakeMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BakeMesh)
    
    		self->BakeMesh(*mesh);
    	
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_Get_Custom_PropActualRootBone(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_Get_Custom_PropActualRootBone)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_actualRootBone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_actualRootBone)
    return Scripting::ScriptingWrapperFor(&self->GetActualRootBone());
}

#endif
#if USE_BLENDSHAPES
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_CUSTOM_GetBlendShapeWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_CUSTOM_GetBlendShapeWeight)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetBlendShapeWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBlendShapeWeight)
     return self->GetBlendShapeWeight(index); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SkinnedMeshRenderer_CUSTOM_SetBlendShapeWeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SkinnedMeshRenderer_CUSTOM_SetBlendShapeWeight)
    ReadOnlyScriptingObjectOfType<SkinnedMeshRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetBlendShapeWeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetBlendShapeWeight)
     self->SetBlendShapeWeight(index, value); 
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Material_Get_Custom_PropShader(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Get_Custom_PropShader)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shader)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shader)
    return Scripting::ScriptingWrapperFor(self->GetShader());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_Set_Custom_PropShader(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Set_Custom_PropShader)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Shader> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_shader)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shader)
    
    self->SetShader (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_INTERNAL_CALL_SetColor(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, const ColorRGBAf& color)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_INTERNAL_CALL_SetColor)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetColor)
    
    		self->SetColor (ScriptingStringToProperty (propertyName), color);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_INTERNAL_CALL_GetColor(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, ColorRGBAf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_INTERNAL_CALL_GetColor)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetColor)
    
    		{ returnValue =(self->GetColor (ScriptingStringToProperty (propertyName))); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_SetTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, ICallType_ReadOnlyUnityEngineObject_Argument texture_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_SetTexture)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    ReadOnlyScriptingObjectOfType<Texture> texture(texture_);
    UNUSED(texture);
    SCRIPTINGAPI_STACK_CHECK(SetTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetTexture)
    
    		self->SetTexture (ScriptingStringToProperty (propertyName), texture);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Material_CUSTOM_GetTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_GetTexture)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(GetTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTexture)
    
    		return Scripting::ScriptingWrapperFor (self->GetTexture (ScriptingStringToProperty (propertyName)));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_Internal_GetTextureOffset(ICallType_ReadOnlyUnityEngineObject_Argument mat_, ICallType_String_Argument name_, Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_Internal_GetTextureOffset)
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetTextureOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetTextureOffset)
    
    		*output = mat->GetTextureOffset( ScriptingStringToProperty(name) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_Internal_GetTextureScale(ICallType_ReadOnlyUnityEngineObject_Argument mat_, ICallType_String_Argument name_, Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_Internal_GetTextureScale)
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetTextureScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetTextureScale)
    
    		*output = mat->GetTextureScale( ScriptingStringToProperty(name) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_INTERNAL_CALL_SetTextureOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, const Vector2fIcall& offset)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_INTERNAL_CALL_SetTextureOffset)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetTextureOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetTextureOffset)
    
    		self->SetTextureOffset( ScriptingStringToProperty(propertyName), offset );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_INTERNAL_CALL_SetTextureScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, const Vector2fIcall& scale)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_INTERNAL_CALL_SetTextureScale)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetTextureScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetTextureScale)
    
    		self->SetTextureScale( ScriptingStringToProperty(propertyName), scale );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_INTERNAL_CALL_SetMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, const Matrix4x4f& matrix)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_INTERNAL_CALL_SetMatrix)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetMatrix)
    
    		self->SetMatrix (ScriptingStringToProperty(propertyName), matrix);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_INTERNAL_CALL_GetMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_INTERNAL_CALL_GetMatrix)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetMatrix)
    
    		{ returnValue =(self->GetMatrix (ScriptingStringToProperty(propertyName))); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_SetFloat(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_SetFloat)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(SetFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFloat)
    
    		self->SetFloat (ScriptingStringToProperty(propertyName), value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Material_CUSTOM_GetFloat(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_GetFloat)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(GetFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetFloat)
    
    		return self->GetFloat (ScriptingStringToProperty (propertyName));
    	
}

#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_SetBuffer(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_, ICallType_Object_Argument buffer_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_SetBuffer)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    ScriptingObjectWithIntPtrField<ComputeBuffer> buffer(buffer_);
    UNUSED(buffer);
    SCRIPTINGAPI_STACK_CHECK(SetBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetBuffer)
    
    		FastPropertyName fpName = ScriptingStringToProperty(propertyName);
    		self->SetComputeBuffer (fpName, buffer ? buffer->GetBufferHandle() : ComputeBufferID());
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Material_CUSTOM_HasProperty(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_HasProperty)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(HasProperty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasProperty)
    
    		return self->HasProperty (ScriptingStringToProperty (propertyName));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Material_CUSTOM_GetTag(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument tag_, ScriptingBool searchFallbacks, ICallType_String_Argument defaultValue_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_GetTag)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_String_Local tag(tag_);
    UNUSED(tag);
    ICallType_String_Local defaultValue(defaultValue_);
    UNUSED(defaultValue);
    SCRIPTINGAPI_STACK_CHECK(GetTag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTag)
    
    		return CreateScriptingString (self->GetTag (tag, !searchFallbacks, defaultValue));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_Lerp(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument start_, ICallType_ReadOnlyUnityEngineObject_Argument end_, float t)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_Lerp)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> start(start_);
    UNUSED(start);
    ReadOnlyScriptingObjectOfType<Material> end(end_);
    UNUSED(end);
    SCRIPTINGAPI_STACK_CHECK(Lerp)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Lerp)
    
    		const ShaderLab::PropertySheet &s1 = start->GetProperties();
    		const ShaderLab::PropertySheet &s2 = end->GetProperties();
    		self->GetWritableProperties().LerpProperties( s1, s2, clamp01(t) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Material_Get_Custom_PropPassCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Get_Custom_PropPassCount)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_passCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_passCount)
    return self->GetPassCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Material_CUSTOM_SetPass(ICallType_ReadOnlyUnityEngineObject_Argument self_, int pass)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_SetPass)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetPass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPass)
    
    		Material& mat = *self;
    		if (pass >= mat.GetPassCount())
    		{
    			ErrorStringMsg("Trying to access pass %d, but material '%s' subshader (0) has only %d valid passes.",
    				pass,
    				mat.GetName(),
    				mat.GetPassCount());
    			return false;
    		}
    
    		if (!CheckShouldRenderPass (pass, mat))
    			return false;
    		s_CurrentShader = mat.GetShaderPPtr();
    		s_CurrentChannels = mat.SetPass(pass);
    		return s_CurrentChannels != NULL;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Material_Get_Custom_PropRenderQueue(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Get_Custom_PropRenderQueue)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderQueue)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderQueue)
    return self->GetRenderQueue ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_Set_Custom_PropRenderQueue(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Set_Custom_PropRenderQueue)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_renderQueue)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_renderQueue)
    
    self->SetRenderQueue (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_Internal_CreateWithString(ICallType_Object_Argument mono_, ICallType_String_Argument contents_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_Internal_CreateWithString)
    ScriptingObjectOfType<Material> mono(mono_);
    UNUSED(mono);
    ICallType_String_Local contents(contents_);
    UNUSED(contents);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateWithString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateWithString)
    
    		Material *mat = Material::CreateMaterial (contents.ToUTF8().c_str(), 0);
    		ConnectScriptingWrapperToObject (mono.GetScriptingObject(), mat);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_Internal_CreateWithShader(ICallType_Object_Argument mono_, ICallType_ReadOnlyUnityEngineObject_Argument shader_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_Internal_CreateWithShader)
    ScriptingObjectOfType<Material> mono(mono_);
    UNUSED(mono);
    ReadOnlyScriptingObjectOfType<Shader> shader(shader_);
    UNUSED(shader);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateWithShader)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateWithShader)
    
    		Material *mat = Material::CreateMaterial (*shader, 0);
    		ConnectScriptingWrapperToObject (mono.GetScriptingObject(), mat);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_Internal_CreateWithMaterial(ICallType_Object_Argument mono_, ICallType_ReadOnlyUnityEngineObject_Argument source_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_Internal_CreateWithMaterial)
    ScriptingObjectOfType<Material> mono(mono_);
    UNUSED(mono);
    ReadOnlyScriptingObjectOfType<Material> source(source_);
    UNUSED(source);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateWithMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateWithMaterial)
    
    		Material *mat = Material::CreateMaterial (*source, 0);
    		ConnectScriptingWrapperToObject (mono.GetScriptingObject(), mat);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_CUSTOM_CopyPropertiesFromMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument mat_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_CUSTOM_CopyPropertiesFromMaterial)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    SCRIPTINGAPI_STACK_CHECK(CopyPropertiesFromMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CopyPropertiesFromMaterial)
    
    		if (mat != NULL)
    			self->CopyPropertiesFromMaterial(*mat);
    		else
    			ErrorString ("Trying to copy properties from null material.");
    	
}

#if ENABLE_MONO || PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Material_Get_Custom_PropShaderKeywords(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Get_Custom_PropShaderKeywords)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_shaderKeywords)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_shaderKeywords)
    const size_t count = self->GetShaderKeywords ().size ();
    ScriptingArrayPtr array = CreateScriptingArray<ScriptingStringPtr> (GetScriptingManager().GetCommonClasses().string, count);
    for (int i=0;i<count;i++)
    {
    SetScriptingArrayElement<ScriptingStringPtr>(array,i,CreateScriptingString(self->GetShaderKeywords ()[i]));
    }
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Material_Set_Custom_PropShaderKeywords(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Material_Set_Custom_PropShaderKeywords)
    ReadOnlyScriptingObjectOfType<Material> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_shaderKeywords)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_shaderKeywords)
    
    Material::ShaderKeywordsT names;
    for (int i=0;i<GetScriptingArraySize (value);i++)
    names.push_back (scripting_cpp_string_for (GetScriptingArrayElementNoRef<ScriptingStringPtr> (value, i)));
    self->SetShaderKeywords (names);
    
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION LensFlare_Get_Custom_PropFlare(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LensFlare_Get_Custom_PropFlare)
    ReadOnlyScriptingObjectOfType<LensFlare> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_flare)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_flare)
    return Scripting::ScriptingWrapperFor(self->GetFlare());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LensFlare_Set_Custom_PropFlare(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(LensFlare_Set_Custom_PropFlare)
    ReadOnlyScriptingObjectOfType<LensFlare> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Flare> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_flare)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_flare)
    
    self->SetFlare (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION LensFlare_Get_Custom_PropBrightness(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LensFlare_Get_Custom_PropBrightness)
    ReadOnlyScriptingObjectOfType<LensFlare> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_brightness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_brightness)
    return self->GetBrightness ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LensFlare_Set_Custom_PropBrightness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(LensFlare_Set_Custom_PropBrightness)
    ReadOnlyScriptingObjectOfType<LensFlare> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_brightness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_brightness)
    
    self->SetBrightness (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LensFlare_CUSTOM_INTERNAL_get_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(LensFlare_CUSTOM_INTERNAL_get_color)
    ReadOnlyScriptingObjectOfType<LensFlare> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    *returnValue = self->GetColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LensFlare_CUSTOM_INTERNAL_set_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(LensFlare_CUSTOM_INTERNAL_set_color)
    ReadOnlyScriptingObjectOfType<LensFlare> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
    
    self->SetColor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropStaticBatchRootTransform(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropStaticBatchRootTransform)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_staticBatchRootTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_staticBatchRootTransform)
    return Scripting::ScriptingWrapperFor (self->GetStaticBatchRoot ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropStaticBatchRootTransform(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropStaticBatchRootTransform)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_staticBatchRootTransform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_staticBatchRootTransform)
     self->SetStaticBatchRoot (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropStaticBatchIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropStaticBatchIndex)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_staticBatchIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_staticBatchIndex)
    return self->GetStaticBatchIndex();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_SetSubsetIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index, int subSetIndexForMaterial)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_SetSubsetIndex)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetSubsetIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetSubsetIndex)
    
    		self->SetMaterialCount (std::max(index+1, self->GetMaterialCount()));
    		self->SetSubsetIndex(index, subSetIndexForMaterial);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropIsPartOfStaticBatch(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropIsPartOfStaticBatch)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPartOfStaticBatch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPartOfStaticBatch)
    return self->GetStaticBatchIndex() != 0;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_INTERNAL_get_worldToLocalMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_INTERNAL_get_worldToLocalMatrix)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_worldToLocalMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_worldToLocalMatrix)
    *returnValue = self->GetWorldToLocalMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_INTERNAL_get_localToWorldMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_INTERNAL_get_localToWorldMatrix)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localToWorldMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localToWorldMatrix)
    *returnValue = self->GetLocalToWorldMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    self->SetEnabled (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropCastShadows(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropCastShadows)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_castShadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_castShadows)
    return self->GetCastShadows ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropCastShadows(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropCastShadows)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_castShadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_castShadows)
    
    self->SetCastShadows (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropReceiveShadows(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropReceiveShadows)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_receiveShadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_receiveShadows)
    return self->GetReceiveShadows ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropReceiveShadows(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropReceiveShadows)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_receiveShadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_receiveShadows)
    
    self->SetReceiveShadows (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    return Scripting::ScriptingWrapperFor (self->GetAndAssignInstantiatedMaterial (0, false));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_material)
    
    self->SetMaterialCount (std::max(1, self->GetMaterialCount()));
    self->SetMaterial (value, 0);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropSharedMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropSharedMaterial)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sharedMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sharedMaterial)
    if (self->GetMaterialCount ())
    return Scripting::ScriptingWrapperFor (self->GetMaterial (0));
    else
    return MonoObjectNULL (ClassID (Material));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropSharedMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropSharedMaterial)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_sharedMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sharedMaterial)
    
    self->SetMaterialCount (std::max(1, self->GetMaterialCount()));
    self->SetMaterial (value, 0);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropSharedMaterials(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropSharedMaterials)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sharedMaterials)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sharedMaterials)
    return CreateScriptingArrayFromUnityObjects(self->GetMaterialArray(), ClassID(Material));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropSharedMaterials(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropSharedMaterials)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_sharedMaterials)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sharedMaterials)
    
    #if ENABLE_MONO
    if (value == SCRIPTING_NULL)
    RaiseNullException("material array is null");
    #endif
    int size = GetScriptingArraySize(value);
    self->SetMaterialCount (size);
    for (int i=0;i<size;i++)
    self->SetMaterial (ScriptingObjectToObject<Material> (GetScriptingArrayElementNoRef<ScriptingObjectPtr>(value, i)), i);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropMaterials(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropMaterials)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_materials)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_materials)
    int length = self->GetMaterialCount();
    ScriptingArrayPtr array = CreateScriptingArray<ScriptingObjectPtr> (ScriptingClassFor(Material), length);
    for (int i=0;i<length;i++)
    {
    Material* instantiated = self->GetAndAssignInstantiatedMaterial(i, false);
    SetScriptingArrayElement<ScriptingObjectPtr>(array,i,Scripting::ScriptingWrapperFor(instantiated));
    }
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropMaterials(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropMaterials)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_materials)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_materials)
    
    if (value == SCRIPTING_NULL)
    RaiseNullException("material array is null");
    int size = GetScriptingArraySize(value);
    self->SetMaterialCount (size);
    for (int i=0;i<size;i++)
    {
    ScriptingObjectPtr o = GetScriptingArrayElementNoRef<ScriptingObjectPtr>(value,i);
    self->SetMaterial (ScriptingObjectToObject<Material> (o), i);
    }
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_INTERNAL_get_bounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_INTERNAL_get_bounds)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_bounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_bounds)
    { *returnValue =(CalculateWorldAABB (self->GetGameObject ())); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropLightmapIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropLightmapIndex)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightmapIndex)
    return self->GetLightmapIndexInt();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropLightmapIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropLightmapIndex)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightmapIndex)
     return self->SetLightmapIndexInt(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_INTERNAL_get_lightmapTilingOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_INTERNAL_get_lightmapTilingOffset)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_lightmapTilingOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_lightmapTilingOffset)
    Vector4f st = self->GetLightmapST();
    { *returnValue =(Vector4f(st.x, st.y, st.z, st.w)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_INTERNAL_set_lightmapTilingOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_INTERNAL_set_lightmapTilingOffset)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_lightmapTilingOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_lightmapTilingOffset)
    
    Vector4f st( value.x, value.y, value.z, value.w );
    self->SetLightmapST( st );
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropIsVisible(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropIsVisible)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isVisible)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isVisible)
    return self->IsVisibleInScene ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropUseLightProbes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropUseLightProbes)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useLightProbes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useLightProbes)
    return self->GetUseLightProbes ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropUseLightProbes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropUseLightProbes)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useLightProbes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useLightProbes)
    
    self->SetUseLightProbes (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Renderer_Get_Custom_PropLightProbeAnchor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Get_Custom_PropLightProbeAnchor)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lightProbeAnchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightProbeAnchor)
    return Scripting::ScriptingWrapperFor(self->GetLightProbeAnchor());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_Set_Custom_PropLightProbeAnchor(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_Set_Custom_PropLightProbeAnchor)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Transform> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_lightProbeAnchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightProbeAnchor)
    
    self->SetLightProbeAnchor (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_SetPropertyBlock(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument properties_)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_SetPropertyBlock)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> properties(properties_);
    UNUSED(properties);
    SCRIPTINGAPI_STACK_CHECK(SetPropertyBlock)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetPropertyBlock)
    
    		if (properties.GetPtr())
    			self->SetPropertyBlock (*properties);
    		else
    			self->ClearPropertyBlock ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Renderer_CUSTOM_Render(ICallType_ReadOnlyUnityEngineObject_Argument self_, int material)
{
    SCRIPTINGAPI_ETW_ENTRY(Renderer_CUSTOM_Render)
    ReadOnlyScriptingObjectOfType<Renderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Render)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Render)
    
    		Shader* shader = s_CurrentShader;
    		if (!shader) {
    			ErrorString ("Render requires material.SetPass before!");
    			return;
    		}
    
    		GfxDevice& device = GetGfxDevice();
    
    		#if UNITY_EDITOR
    		bool outsideOfFrame = !device.IsInsideFrame();
    		if( outsideOfFrame )
    			device.BeginFrame();
    		#endif
    
    		float matWorld[16], matView[16];
    
    		CopyMatrix(device.GetViewMatrix(), matView);
    		CopyMatrix(device.GetWorldMatrix(), matWorld);
    
    		Matrix4x4f transformMatrix;
    		Matrix4x4f scaleOnly;
    		float scale;
    		TransformType matrixType = self->GetTransform().CalculateTransformMatrixDisableNonUniformScale (transformMatrix, scaleOnly, scale);
    		SetupObjectMatrix (transformMatrix, matrixType);
    
    		self->Render (material, *s_CurrentChannels);
    		device.SetViewMatrix(matView);
    		device.SetWorldMatrix(matWorld);
    
    		#if UNITY_EDITOR
    		if( outsideOfFrame )
    			device.EndFrame();
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropNearClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropNearClipPlane)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_nearClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_nearClipPlane)
    return self->GetNearClipPlane ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropNearClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropNearClipPlane)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_nearClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_nearClipPlane)
    
    self->SetNearClipPlane (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropFarClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropFarClipPlane)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_farClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_farClipPlane)
    return self->GetFarClipPlane ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropFarClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropFarClipPlane)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_farClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_farClipPlane)
    
    self->SetFarClipPlane (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropFieldOfView(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropFieldOfView)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fieldOfView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fieldOfView)
    return self->GetFieldOfView ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropFieldOfView(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropFieldOfView)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fieldOfView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fieldOfView)
    
    self->SetFieldOfView (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropAspectRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropAspectRatio)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_aspectRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_aspectRatio)
    return self->GetAspectRatio ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropAspectRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropAspectRatio)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_aspectRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_aspectRatio)
    
    self->SetAspectRatio (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropOrthographic(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropOrthographic)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_orthographic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_orthographic)
    return self->GetOrthographic ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropOrthographic(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropOrthographic)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_orthographic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_orthographic)
    
    self->SetOrthographic (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropOrthographicSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropOrthographicSize)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_orthographicSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_orthographicSize)
    return self->GetOrthographicSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropOrthographicSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropOrthographicSize)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_orthographicSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_orthographicSize)
    
    self->SetOrthographicSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropIgnoreLayers(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropIgnoreLayers)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_ignoreLayers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_ignoreLayers)
    return self->GetIgnoreLayers ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropIgnoreLayers(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropIgnoreLayers)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_ignoreLayers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_ignoreLayers)
    
    self->SetIgnoreLayers (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Projector_Get_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Get_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    return Scripting::ScriptingWrapperFor(self->GetMaterial());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Projector_Set_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Projector_Set_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Projector> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_material)
    
    self->SetMaterial (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Skybox_Get_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Skybox_Get_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Skybox> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    return Scripting::ScriptingWrapperFor (self->GetMaterial ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Skybox_Set_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Skybox_Set_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Skybox> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_material)
     self->SetMaterial (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropText)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_text)
    return CreateScriptingString (self->GetText ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropText)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_text)
     self->SetText (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropFont(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropFont)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_font)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_font)
    return Scripting::ScriptingWrapperFor(self->GetFont());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropFont(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropFont)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Font> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_font)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_font)
    
    self->SetFont (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropFontSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropFontSize)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontSize)
    return self->GetFontSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropFontSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropFontSize)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fontSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontSize)
    
    self->SetFontSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropFontStyle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropFontStyle)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontStyle)
    return self->GetFontStyle ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropFontStyle(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropFontStyle)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fontStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontStyle)
    
    self->SetFontStyle (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropOffsetZ(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropOffsetZ)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_offsetZ)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_offsetZ)
    return self->GetOffsetZ ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropOffsetZ(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropOffsetZ)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_offsetZ)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_offsetZ)
    
    self->SetOffsetZ (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropAlignment(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropAlignment)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alignment)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alignment)
    return self->GetAlignment ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropAlignment(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropAlignment)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_alignment)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alignment)
    
    self->SetAlignment (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropAnchor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropAnchor)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_anchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anchor)
    return self->GetAnchor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropAnchor(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropAnchor)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_anchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_anchor)
    
    self->SetAnchor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropCharacterSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropCharacterSize)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_characterSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_characterSize)
    return self->GetCharacterSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropCharacterSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropCharacterSize)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_characterSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_characterSize)
    
    self->SetCharacterSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropLineSpacing(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropLineSpacing)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lineSpacing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lineSpacing)
    return self->GetLineSpacing ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropLineSpacing(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropLineSpacing)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lineSpacing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lineSpacing)
    
    self->SetLineSpacing (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropTabSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropTabSize)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_tabSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_tabSize)
    return self->GetTabSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropTabSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropTabSize)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_tabSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_tabSize)
    
    self->SetTabSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TextMesh_Get_Custom_PropRichText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Get_Custom_PropRichText)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_richText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_richText)
    return self->GetRichText ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_Set_Custom_PropRichText(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_Set_Custom_PropRichText)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_richText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_richText)
    
    self->SetRichText (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_CUSTOM_INTERNAL_get_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_CUSTOM_INTERNAL_get_color)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    *returnValue = self->GetColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TextMesh_CUSTOM_INTERNAL_set_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(TextMesh_CUSTOM_INTERNAL_set_color)
    ReadOnlyScriptingObjectOfType<TextMesh> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
    
    self->SetColor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TrailRenderer_Get_Custom_PropTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Get_Custom_PropTime)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time)
    return self->GetTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TrailRenderer_Set_Custom_PropTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Set_Custom_PropTime)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_time)
    
    self->SetTime (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TrailRenderer_Get_Custom_PropStartWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Get_Custom_PropStartWidth)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_startWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_startWidth)
    return self->GetStartWidth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TrailRenderer_Set_Custom_PropStartWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Set_Custom_PropStartWidth)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_startWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_startWidth)
    
    self->SetStartWidth (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TrailRenderer_Get_Custom_PropEndWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Get_Custom_PropEndWidth)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_endWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_endWidth)
    return self->GetEndWidth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TrailRenderer_Set_Custom_PropEndWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Set_Custom_PropEndWidth)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_endWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_endWidth)
    
    self->SetEndWidth (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TrailRenderer_Get_Custom_PropAutodestruct(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Get_Custom_PropAutodestruct)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_autodestruct)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autodestruct)
    return self->GetAutodestruct ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TrailRenderer_Set_Custom_PropAutodestruct(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(TrailRenderer_Set_Custom_PropAutodestruct)
    ReadOnlyScriptingObjectOfType<TrailRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_autodestruct)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autodestruct)
    
    self->SetAutodestruct (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LineRenderer_CUSTOM_INTERNAL_CALL_SetWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_, float start, float end)
{
    SCRIPTINGAPI_ETW_ENTRY(LineRenderer_CUSTOM_INTERNAL_CALL_SetWidth)
    ReadOnlyScriptingObjectOfType<LineRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetWidth)
    return self->SetWidth(start, end);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LineRenderer_CUSTOM_INTERNAL_CALL_SetColors(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& start, const ColorRGBAf& end)
{
    SCRIPTINGAPI_ETW_ENTRY(LineRenderer_CUSTOM_INTERNAL_CALL_SetColors)
    ReadOnlyScriptingObjectOfType<LineRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetColors)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetColors)
    return self->SetColors(start, end);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LineRenderer_CUSTOM_INTERNAL_CALL_SetVertexCount(ICallType_ReadOnlyUnityEngineObject_Argument self_, int count)
{
    SCRIPTINGAPI_ETW_ENTRY(LineRenderer_CUSTOM_INTERNAL_CALL_SetVertexCount)
    ReadOnlyScriptingObjectOfType<LineRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetVertexCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetVertexCount)
    return self->SetVertexCount(count);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LineRenderer_CUSTOM_INTERNAL_CALL_SetPosition(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index, const Vector3f& position)
{
    SCRIPTINGAPI_ETW_ENTRY(LineRenderer_CUSTOM_INTERNAL_CALL_SetPosition)
    ReadOnlyScriptingObjectOfType<LineRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetPosition)
    return self->SetPosition(index, position);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION LineRenderer_Get_Custom_PropUseWorldSpace(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LineRenderer_Get_Custom_PropUseWorldSpace)
    ReadOnlyScriptingObjectOfType<LineRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useWorldSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useWorldSpace)
    return self->GetUseWorldSpace ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LineRenderer_Set_Custom_PropUseWorldSpace(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(LineRenderer_Set_Custom_PropUseWorldSpace)
    ReadOnlyScriptingObjectOfType<LineRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useWorldSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useWorldSpace)
    
    self->SetUseWorldSpace (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_InitBlock(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_InitBlock)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InitBlock)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitBlock)
    
    		MonoMaterialPropertyBlock* block = new MonoMaterialPropertyBlock();
    		self.SetPtr(block, MonoMaterialPropertyBlock::CleanupMonoMaterialPropertyBlock);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_DestroyBlock(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_DestroyBlock)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(DestroyBlock)
    
    		MonoMaterialPropertyBlock::CleanupMonoMaterialPropertyBlock((MonoMaterialPropertyBlock*)self.GetPtr());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_AddFloat(ICallType_Object_Argument self_, int nameID, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_AddFloat)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(AddFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddFloat)
    
    		ShaderLab::FastPropertyName name;
    		name.index = nameID;
    		self->AddProperty( name, &value, 1, 1, 1 );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddVector(ICallType_Object_Argument self_, int nameID, const Vector4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddVector)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddVector)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddVector)
    
    		ShaderLab::FastPropertyName name;
    		name.index = nameID;
    #if ENABLE_MONO
    		self->AddProperty( name, value.GetPtr(), 1, 4, 1 );
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddColor(ICallType_Object_Argument self_, int nameID, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddColor)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddColor)
    
    		ShaderLab::FastPropertyName name;
    		name.index = nameID;
    #if ENABLE_MONO
    		self->AddProperty( name, value.GetPtr(), 1, 4, 1 );
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddMatrix(ICallType_Object_Argument self_, int nameID, const Matrix4x4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddMatrix)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AddMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_AddMatrix)
    
    		ShaderLab::FastPropertyName name;
    		name.index = nameID;
    #if ENABLE_MONO
    		self->AddProperty( name, value.GetPtr(), 4, 4, 1 );
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MaterialPropertyBlock_CUSTOM_Clear(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MaterialPropertyBlock_CUSTOM_Clear)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Clear)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Clear)
    
    #if ENABLE_MONO
    		self->Clear();
    #endif
    	
}



struct MonoInternalDrawTextureArguments {
	Rectf screenRect;
	#if PLATFORM_WINRT
	int				   textureInstanceId;
	#else
	ScriptingObjectOfType<Texture> texture;
	#endif
	Rectf sourceRect;
	int leftBorder;
	int rightBorder;
	int topBorder;
	int bottomBorder;
	ColorRGBA32 color;
	#if PLATFORM_WINRT
	int					matInstanceId;
	#else
	ScriptingObjectOfType<Material> mat;
	#endif
};


struct MonoInternal_DrawMeshTRArguments {
	int layer;
	int submeshIndex;
	Quaternionf rotation;
	Vector3f position;
	int castShadows;
	int receiveShadows;
};


struct MonoInternal_DrawMeshMatrixArguments {
	int layer;
	int submeshIndex;
	Matrix4x4f matrix;
	int castShadows;
	int receiveShadows;
};
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_DrawMeshTR(MonoInternal_DrawMeshTRArguments& arguments, ICallType_Object_Argument properties_, ICallType_ReadOnlyUnityEngineObject_Argument material_, ICallType_ReadOnlyUnityEngineObject_Argument mesh_, ICallType_ReadOnlyUnityEngineObject_Argument camera_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_DrawMeshTR)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> properties(properties_);
    UNUSED(properties);
    ReadOnlyScriptingObjectOfType<Material> material(material_);
    UNUSED(material);
    ReadOnlyScriptingObjectOfType<Mesh> mesh(mesh_);
    UNUSED(mesh);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(Internal_DrawMeshTR)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_DrawMeshTR)
    
    		Camera* cameraPointer = camera;
    		Matrix4x4f matrix;
    		matrix.SetTR( arguments.position, arguments.rotation );
    		IntermediateRenderer* r = new IntermediateRenderer( matrix, mesh, material, arguments.layer, arguments.castShadows != 0, arguments.receiveShadows != 0, arguments.submeshIndex );
    
    		MaterialPropertyBlock* propertiesPtr = properties.GetPtr();
    		if (propertiesPtr)
    			r->SetPropertyBlock (*propertiesPtr);
    		AddIntermediateRenderer( r, cameraPointer );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_DrawMeshMatrix(ScriptingDrawMeshMatrixArguments& arguments, ICallType_Object_Argument properties_, ICallType_ReadOnlyUnityEngineObject_Argument material_, ICallType_ReadOnlyUnityEngineObject_Argument mesh_, ICallType_ReadOnlyUnityEngineObject_Argument camera_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_DrawMeshMatrix)
    ScriptingObjectWithIntPtrField<ShaderPropertySheet> properties(properties_);
    UNUSED(properties);
    ReadOnlyScriptingObjectOfType<Material> material(material_);
    UNUSED(material);
    ReadOnlyScriptingObjectOfType<Mesh> mesh(mesh_);
    UNUSED(mesh);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(Internal_DrawMeshMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_DrawMeshMatrix)
    
    		Camera* cameraPointer = camera;
    		IntermediateRenderer* r = new IntermediateRenderer( arguments.matrix, mesh, material, arguments.layer, arguments.castShadows != 0, arguments.receiveShadows != 0, arguments.submeshIndex );
    
    		MaterialPropertyBlock* propertiesPtr = properties.GetPtr();
    		if (propertiesPtr)
    			r->SetPropertyBlock (*propertiesPtr);
    		AddIntermediateRenderer (r, cameraPointer);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow1(ICallType_ReadOnlyUnityEngineObject_Argument mesh_, const Vector3f& position, const Quaternionf& rotation, int materialIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow1)
    ReadOnlyScriptingObjectOfType<Mesh> mesh(mesh_);
    UNUSED(mesh);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_DrawMeshNow1)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_DrawMeshNow1)
    
    		Shader* shader = s_CurrentShader;
    		if (!shader) {
    			ErrorString ("DrawMesh requires material.SetPass before!");
    			return;
    		}
    		DrawUtil::DrawMesh (*s_CurrentChannels, *mesh, position, rotation);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow2(ICallType_ReadOnlyUnityEngineObject_Argument mesh_, const Matrix4x4f& matrix, int materialIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow2)
    ReadOnlyScriptingObjectOfType<Mesh> mesh(mesh_);
    UNUSED(mesh);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_DrawMeshNow2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_DrawMeshNow2)
    
    		Shader* shader = s_CurrentShader;
    		if (!shader) {
    			ErrorString ("DrawMesh requires material.SetPass before!");
    			return;
    		}
    		DrawUtil::DrawMesh (*s_CurrentChannels, *mesh, matrix, materialIndex);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_DrawProcedural(GfxPrimitiveType topology, int vertexCount, int instanceCount)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_DrawProcedural)
    SCRIPTINGAPI_STACK_CHECK(DrawProcedural)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DrawProcedural)
    
    		DrawUtil::DrawProcedural (topology, vertexCount, instanceCount);
    	
}

#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_DrawProceduralIndirect(GfxPrimitiveType topology, ICallType_Object_Argument bufferWithArgs_, int argsOffset)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_DrawProceduralIndirect)
    ScriptingObjectWithIntPtrField<ComputeBuffer> bufferWithArgs(bufferWithArgs_);
    UNUSED(bufferWithArgs);
    SCRIPTINGAPI_STACK_CHECK(DrawProceduralIndirect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DrawProceduralIndirect)
    
    		DrawUtil::DrawProceduralIndirect (topology, bufferWithArgs.GetPtr(), argsOffset);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_DrawTexture(InternalDrawTextureArguments& arguments)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_DrawTexture)
    SCRIPTINGAPI_STACK_CHECK(DrawTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DrawTexture)
    
    	#if PLATFORM_WINRT
    		DrawGUITexture (
    			arguments.screenRect,
    			PPtr<Texture>(arguments.textureInstanceId),
    			arguments.sourceRect,
    			arguments.leftBorder,
    			arguments.rightBorder,
    			arguments.topBorder,
    			arguments.bottomBorder,
    			ColorRGBA32(arguments.color),
    			PPtr<Material>(arguments.matInstanceId));
    	#else
    		DrawGUITexture (arguments.screenRect, arguments.texture, arguments.sourceRect, arguments.leftBorder, arguments.rightBorder, arguments.topBorder, arguments.bottomBorder, ColorRGBA32(arguments.color), arguments.mat);
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Blit(ICallType_ReadOnlyUnityEngineObject_Argument source_, ICallType_ReadOnlyUnityEngineObject_Argument dest_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Blit)
    ReadOnlyScriptingObjectOfType<Texture> source(source_);
    UNUSED(source);
    ReadOnlyScriptingObjectOfType<RenderTexture> dest(dest_);
    UNUSED(dest);
    SCRIPTINGAPI_STACK_CHECK(Blit)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Blit)
    
    		ImageFilters::Blit (source, dest);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_BlitMaterial(ICallType_ReadOnlyUnityEngineObject_Argument source_, ICallType_ReadOnlyUnityEngineObject_Argument dest_, ICallType_ReadOnlyUnityEngineObject_Argument mat_, int pass, ScriptingBool setRT)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_BlitMaterial)
    ReadOnlyScriptingObjectOfType<Texture> source(source_);
    UNUSED(source);
    ReadOnlyScriptingObjectOfType<RenderTexture> dest(dest_);
    UNUSED(dest);
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    SCRIPTINGAPI_STACK_CHECK(Internal_BlitMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_BlitMaterial)
    
    		Material* m = mat;
    		if (m == NULL)
    		{
    			ErrorString ("Graphics.Blit: material is null");
    			return;
    		}
    		ImageFilters::Blit (source, dest, m, pass, setRT);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_BlitMultiTap(ICallType_ReadOnlyUnityEngineObject_Argument source_, ICallType_ReadOnlyUnityEngineObject_Argument dest_, ICallType_ReadOnlyUnityEngineObject_Argument mat_, ICallType_Array_Argument offsets_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_BlitMultiTap)
    ReadOnlyScriptingObjectOfType<Texture> source(source_);
    UNUSED(source);
    ReadOnlyScriptingObjectOfType<RenderTexture> dest(dest_);
    UNUSED(dest);
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    ICallType_Array_Local offsets(offsets_);
    UNUSED(offsets);
    SCRIPTINGAPI_STACK_CHECK(Internal_BlitMultiTap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_BlitMultiTap)
    
    		Material* m = mat;
    		if (m == NULL)
    		{
    			ErrorString ("Graphics.BlitMultiTap: material is null");
    			return;
    		}
    		int size = GetScriptingArraySize (offsets);
    		const Vector2f* arr = GetScriptingArrayStart<Vector2f> (offsets);
    		ImageFilters::BlitMultiTap (source, dest, m, size, arr);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_SetRT(ICallType_ReadOnlyUnityEngineObject_Argument rt_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_SetRT)
    ReadOnlyScriptingObjectOfType<RenderTexture> rt(rt_);
    UNUSED(rt);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetRT)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetRT)
    
    		RenderTexture::SetActive (rt);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_SetRTBuffer(ScriptingRenderBuffer* colorBuffer, ScriptingRenderBuffer* depthBuffer)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_SetRTBuffer)
    SCRIPTINGAPI_STACK_CHECK(Internal_SetRTBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetRTBuffer)
    
    		RenderTexture* rtC = PPtr<RenderTexture>(colorBuffer->m_RenderTextureInstanceID);
    		RenderTexture* rtD = PPtr<RenderTexture>(depthBuffer->m_RenderTextureInstanceID);
    		if (rtC && !rtD)
    		{
    			ErrorString ("SetRenderTarget can only mix color & depth buffers from RenderTextures. You're trying to set depth buffer from the screen.");
    			return;
    		}
    		if (!rtC && rtD)
    		{
    			ErrorString ("SetRenderTarget can only mix color & depth buffers from RenderTextures. You're trying to set color buffer from the screen.");
    			return;
    		}
    		if (rtC)
    		{
    			int w = rtC->GetWidth();
    			int h = rtC->GetHeight();
    			RenderSurfaceHandle rsHandle(colorBuffer->m_BufferPtr);
    			RenderTexture::SetActive (1, &rsHandle, RenderSurfaceHandle(depthBuffer->m_BufferPtr), rtC, w, h);
    		}
    		else
    		{
    			RenderTexture::SetActive (NULL);
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_SetRTBuffers(ICallType_Array_Argument colorBuffers_, ScriptingRenderBuffer* depthBuffer)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_SetRTBuffers)
    ICallType_Array_Local colorBuffers(colorBuffers_);
    UNUSED(colorBuffers);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetRTBuffers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetRTBuffers)
    
    		int size = GetScriptingArraySize (colorBuffers);
    		if (size < 1 || size > kMaxSupportedRenderTargets)
    		{
    			ErrorString ("Invalid color buffer count for SetRenderTarget");
    			return;
    		}
    
    		const ScriptingRenderBuffer* arr = GetScriptingArrayStart<ScriptingRenderBuffer> (colorBuffers);
    		RenderTexture* rtC = PPtr<RenderTexture>(arr[0].m_RenderTextureInstanceID);
    		RenderTexture* rtD = PPtr<RenderTexture>(depthBuffer->m_RenderTextureInstanceID);
    		if (rtC && !rtD)
    		{
    			ErrorString ("SetRenderTarget can only mix color & depth buffers from RenderTextures. You're trying to set depth buffer from the screen.");
    			return;
    		}
    		if (!rtC && rtD)
    		{
    			ErrorString ("SetRenderTarget can only mix color & depth buffers from RenderTextures. You're trying to set color buffer from the screen.");
    			return;
    		}
    		if (rtC)
    		{
    			RenderSurfaceHandle colorHandles[kMaxSupportedRenderTargets];
    			for (int i = 0; i < size; ++i)
    				colorHandles[i].object = arr[i].m_BufferPtr;
    			int w = rtC->GetWidth();
    			int h = rtC->GetHeight();
    			RenderTexture::SetActive (size, colorHandles, RenderSurfaceHandle(depthBuffer->m_BufferPtr), rtC, w, h);
    		}
    		else
    		{
    			RenderTexture::SetActive (NULL);
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_GetActiveColorBuffer(ScriptingRenderBuffer* res)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_GetActiveColorBuffer)
    SCRIPTINGAPI_STACK_CHECK(GetActiveColorBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetActiveColorBuffer)
    
    		GfxDevice& device = GetGfxDevice();
    		RenderTexture* rt = RenderTexture::GetActive();
    		res->m_RenderTextureInstanceID = rt ? rt->GetInstanceID() : 0;
    		res->m_BufferPtr = rt ? device.GetActiveRenderColorSurface(0).object : NULL;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_GetActiveDepthBuffer(ScriptingRenderBuffer* res)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_GetActiveDepthBuffer)
    SCRIPTINGAPI_STACK_CHECK(GetActiveDepthBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetActiveDepthBuffer)
    
    		GfxDevice& device = GetGfxDevice();
    		RenderTexture* rt = RenderTexture::GetActive();
    		res->m_RenderTextureInstanceID = rt ? rt->GetInstanceID() : 0;
    		res->m_BufferPtr = rt ? device.GetActiveRenderDepthSurface().object : NULL;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_ClearRandomWriteTargets()
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_ClearRandomWriteTargets)
    SCRIPTINGAPI_STACK_CHECK(ClearRandomWriteTargets)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearRandomWriteTargets)
    
    		GetGfxDevice().ClearRandomWriteTargets();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_SetRandomWriteTargetRT(int index, ICallType_ReadOnlyUnityEngineObject_Argument uav_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_SetRandomWriteTargetRT)
    ReadOnlyScriptingObjectOfType<RenderTexture> uav(uav_);
    UNUSED(uav);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetRandomWriteTargetRT)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetRandomWriteTargetRT)
    
    		RenderTexture* rt = uav;
    		TextureID tid = rt ? rt->GetTextureID() : TextureID();
    		GetGfxDevice().SetRandomWriteTargetTexture(index, tid);
    	
}

#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_Internal_SetRandomWriteTargetBuffer(int index, ICallType_Object_Argument uav_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_Internal_SetRandomWriteTargetBuffer)
    ScriptingObjectWithIntPtrField<ComputeBuffer> uav(uav_);
    UNUSED(uav);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetRandomWriteTargetBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetRandomWriteTargetBuffer)
    
    		GetGfxDevice().SetRandomWriteTargetBuffer(index, uav->GetBufferHandle());
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Graphics_CUSTOM_SetupVertexLights(ICallType_Array_Argument lights_)
{
    SCRIPTINGAPI_ETW_ENTRY(Graphics_CUSTOM_SetupVertexLights)
    ICallType_Array_Local lights(lights_);
    UNUSED(lights);
    SCRIPTINGAPI_STACK_CHECK(SetupVertexLights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetupVertexLights)
    
    		const int size = GetScriptingArraySize(lights);
    		std::vector<Light*> lightsVec(size);
    		for (int i = 0; i < size; ++i)
    			lightsVec[i] = ScriptingObjectToObject<Light>(GetScriptingArrayElementNoRef<ScriptingObjectPtr> (lights, i));
    
    		SetupVertexLights(lightsVec);
    	
}

#if ENABLE_MONO || PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightProbes_CUSTOM_INTERNAL_CALL_GetInterpolatedLightProbe(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, ICallType_ReadOnlyUnityEngineObject_Argument renderer_, ICallType_Array_Argument coefficients_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbes_CUSTOM_INTERNAL_CALL_GetInterpolatedLightProbe)
    ReadOnlyScriptingObjectOfType<LightProbes> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Renderer> renderer(renderer_);
    UNUSED(renderer);
    ICallType_Array_Local coefficients(coefficients_);
    UNUSED(coefficients);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetInterpolatedLightProbe)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetInterpolatedLightProbe)
    
    		if (GetScriptingArraySize(coefficients) != kLightProbeBasisCount * 3)
    			RaiseArgumentException("Coefficients array must have 9*3 elements");
    
    		self->GetInterpolatedLightProbe(position, renderer, &GetScriptingArrayElement<float> (coefficients, 0));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION LightProbes_Get_Custom_PropPositions(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbes_Get_Custom_PropPositions)
    ReadOnlyScriptingObjectOfType<LightProbes> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_positions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_positions)
    return CreateScriptingArray(self->GetPositions(), self->GetPositionsSize(), GetCommonScriptingClasses ().vector3);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION LightProbes_Get_Custom_PropCoefficients(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbes_Get_Custom_PropCoefficients)
    ReadOnlyScriptingObjectOfType<LightProbes> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_coefficients)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_coefficients)
    return CreateScriptingArray(reinterpret_cast<float*>(self->GetCoefficients()), self->GetPositionsSize()*kLightProbeCoefficientCount, GetCommonScriptingClasses ().floatSingle);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightProbes_Set_Custom_PropCoefficients(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbes_Set_Custom_PropCoefficients)
    ReadOnlyScriptingObjectOfType<LightProbes> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_coefficients)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_coefficients)
    
    if (GetScriptingArraySize(value) != self->GetPositionsSize() * kLightProbeBasisCount * 3)
    RaiseArgumentException("Coefficients array must have probeCount*9*3 elements");
    self->SetCoefficients(&GetScriptingArrayElement<float> (value, 0), GetScriptingArraySize(value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION LightProbes_Get_Custom_PropCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbes_Get_Custom_PropCount)
    ReadOnlyScriptingObjectOfType<LightProbes> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_count)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_count)
    return self->GetPositionsSize();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION LightProbes_Get_Custom_PropCellCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightProbes_Get_Custom_PropCellCount)
    ReadOnlyScriptingObjectOfType<LightProbes> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cellCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cellCount)
    return self->GetTetrahedraSize();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION LightmapSettings_Get_Custom_PropLightmaps()
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Get_Custom_PropLightmaps)
    SCRIPTINGAPI_STACK_CHECK(get_lightmaps)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightmaps)
    return VectorToScriptingClassArray<LightmapData, LightmapDataMono> (GetLightmapSettings().GetLightmaps(), GetScriptingTypeRegistry().GetType("UnityEngine", "LightmapData"), LightmapDataToMono);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightmapSettings_Set_Custom_PropLightmaps(ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Set_Custom_PropLightmaps)
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_lightmaps)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightmaps)
     GetLightmapSettings().SetLightmaps (ScriptingClassArrayToVector<LightmapData, LightmapDataMono> (value, LightmapDataToCpp)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION LightmapSettings_Get_Custom_PropLightmapsMode()
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Get_Custom_PropLightmapsMode)
    SCRIPTINGAPI_STACK_CHECK(get_lightmapsMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightmapsMode)
    return  GetLightmapSettings().GetLightmapsMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightmapSettings_Set_Custom_PropLightmapsMode(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Set_Custom_PropLightmapsMode)
    SCRIPTINGAPI_STACK_CHECK(set_lightmapsMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightmapsMode)
     GetLightmapSettings().SetLightmapsMode (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ColorSpace SCRIPT_CALL_CONVENTION LightmapSettings_Get_Custom_PropBakedColorSpace()
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Get_Custom_PropBakedColorSpace)
    SCRIPTINGAPI_STACK_CHECK(get_bakedColorSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bakedColorSpace)
    return  GetLightmapSettings().GetBakedColorSpace ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightmapSettings_Set_Custom_PropBakedColorSpace(ColorSpace value)
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Set_Custom_PropBakedColorSpace)
    SCRIPTINGAPI_STACK_CHECK(set_bakedColorSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bakedColorSpace)
     
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION LightmapSettings_Get_Custom_PropLightProbes()
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Get_Custom_PropLightProbes)
    SCRIPTINGAPI_STACK_CHECK(get_lightProbes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightProbes)
    return Scripting::ScriptingWrapperFor(GetLightmapSettings().GetLightProbes());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LightmapSettings_Set_Custom_PropLightProbes(ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(LightmapSettings_Set_Custom_PropLightProbes)
    ReadOnlyScriptingObjectOfType<LightProbes> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_lightProbes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightProbes)
    
    GetLightmapSettings().SetLightProbes(value);
    
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GeometryUtility_CUSTOM_INTERNAL_CALL_Internal_ExtractPlanes(ICallType_Array_Argument planes_, const Matrix4x4f& worldToProjectionMatrix)
{
    SCRIPTINGAPI_ETW_ENTRY(GeometryUtility_CUSTOM_INTERNAL_CALL_Internal_ExtractPlanes)
    ICallType_Array_Local planes(planes_);
    UNUSED(planes);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_ExtractPlanes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_ExtractPlanes)
    
    		ExtractProjectionPlanes(worldToProjectionMatrix, GetScriptingArrayStart<Plane> (planes));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GeometryUtility_CUSTOM_INTERNAL_CALL_TestPlanesAABB(ICallType_Array_Argument planes_, const AABB& bounds)
{
    SCRIPTINGAPI_ETW_ENTRY(GeometryUtility_CUSTOM_INTERNAL_CALL_TestPlanesAABB)
    ICallType_Array_Local planes(planes_);
    UNUSED(planes);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TestPlanesAABB)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_TestPlanesAABB)
    
    		return TestPlanesAABB(GetScriptingArrayStart<Plane> (planes), GetScriptingArraySize(planes), bounds);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropResolutions()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropResolutions)
    SCRIPTINGAPI_STACK_CHECK(get_resolutions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_resolutions)
    ScriptingClassPtr klass = GetMonoManager ().GetCommonClasses ().resolution;
    ScreenManager::Resolutions resolutions = GetScreenManager ().GetResolutions ();
    ScriptingArrayPtr array = CreateScriptingArray<ScreenManager::Resolution> (klass, resolutions.size ());
    for (int i=0;i<resolutions.size ();i++)
    SetScriptingArrayElement (array, i, resolutions[i]);
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScreenManager::Resolution SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropCurrentResolution()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropCurrentResolution)
    SCRIPTINGAPI_STACK_CHECK(get_currentResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_currentResolution)
    return GetScreenManager().GetCurrentResolution ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_CUSTOM_SetResolution(int width, int height, ScriptingBool fullscreen, int preferredRefreshRate)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_CUSTOM_SetResolution)
    SCRIPTINGAPI_STACK_CHECK(SetResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetResolution)
    
    		#if WEBPLUG
    		if (fullscreen)
    		{
    			if (!GetScreenManager ().GetAllowFullscreenSwitch())
    			{
    				ErrorString("Fullscreen mode can only be enabled in the web player after clicking on the content.");
    				return;
    			}
    			if( !GetScreenManager().IsFullScreen() )
    				ShowFullscreenEscapeWarning();
    		}
    		#endif
    
    		GetScreenManager ().RequestResolution (width, height, fullscreen, preferredRefreshRate);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropShowCursor()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropShowCursor)
    SCRIPTINGAPI_STACK_CHECK(get_showCursor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_showCursor)
    return GetScreenManager ().GetShowCursor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropShowCursor(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropShowCursor)
    SCRIPTINGAPI_STACK_CHECK(set_showCursor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_showCursor)
     GetScreenManager ().SetShowCursor (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropLockCursor()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropLockCursor)
    SCRIPTINGAPI_STACK_CHECK(get_lockCursor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lockCursor)
    return GetScreenManager ().GetLockCursor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropLockCursor(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropLockCursor)
    SCRIPTINGAPI_STACK_CHECK(set_lockCursor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lockCursor)
     GetScreenManager ().SetLockCursor (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropWidth()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropWidth)
    SCRIPTINGAPI_STACK_CHECK(get_width)
    return GetScreenManager ().GetWidth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropHeight()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropHeight)
    SCRIPTINGAPI_STACK_CHECK(get_height)
    return GetScreenManager ().GetHeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropDpi()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropDpi)
    SCRIPTINGAPI_STACK_CHECK(get_dpi)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dpi)
    return GetScreenManager ().GetDPI ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropFullScreen()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropFullScreen)
    SCRIPTINGAPI_STACK_CHECK(get_fullScreen)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fullScreen)
    return GetScreenManager ().IsFullScreen ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropFullScreen(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropFullScreen)
    SCRIPTINGAPI_STACK_CHECK(set_fullScreen)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fullScreen)
    
    ScreenManager& screen = GetScreenManager();
    bool goFullscreen = (bool)value;
    if (goFullscreen  == screen.IsFullScreen ())
    {
    return;
    }
    #if WEBPLUG
    if (goFullscreen)
    {
    if (!GetScreenManager().GetAllowFullscreenSwitch())
    {
    ErrorString("Fullscreen mode can only be enabled in the web player after clicking on the content.");
    return;
    }
    if( !screen.IsFullScreen() )
    ShowFullscreenEscapeWarning();
    }
    #endif
    screen.RequestSetFullscreen (goFullscreen);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropAutorotateToPortrait()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropAutorotateToPortrait)
    SCRIPTINGAPI_STACK_CHECK(get_autorotateToPortrait)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autorotateToPortrait)
    return GetScreenManager().GetIsOrientationEnabled(kAutorotateToPortrait);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropAutorotateToPortrait(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropAutorotateToPortrait)
    SCRIPTINGAPI_STACK_CHECK(set_autorotateToPortrait)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autorotateToPortrait)
    
    GetScreenManager().SetIsOrientationEnabled(kAutorotateToPortrait, value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropAutorotateToPortraitUpsideDown()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropAutorotateToPortraitUpsideDown)
    SCRIPTINGAPI_STACK_CHECK(get_autorotateToPortraitUpsideDown)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autorotateToPortraitUpsideDown)
    return GetScreenManager().GetIsOrientationEnabled(kAutorotateToPortraitUpsideDown);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropAutorotateToPortraitUpsideDown(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropAutorotateToPortraitUpsideDown)
    SCRIPTINGAPI_STACK_CHECK(set_autorotateToPortraitUpsideDown)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autorotateToPortraitUpsideDown)
    
    GetScreenManager().SetIsOrientationEnabled(kAutorotateToPortraitUpsideDown, value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropAutorotateToLandscapeLeft()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropAutorotateToLandscapeLeft)
    SCRIPTINGAPI_STACK_CHECK(get_autorotateToLandscapeLeft)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autorotateToLandscapeLeft)
    return GetScreenManager().GetIsOrientationEnabled(kAutorotateToLandscapeLeft);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropAutorotateToLandscapeLeft(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropAutorotateToLandscapeLeft)
    SCRIPTINGAPI_STACK_CHECK(set_autorotateToLandscapeLeft)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autorotateToLandscapeLeft)
    
    GetScreenManager().SetIsOrientationEnabled(kAutorotateToLandscapeLeft, value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropAutorotateToLandscapeRight()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropAutorotateToLandscapeRight)
    SCRIPTINGAPI_STACK_CHECK(get_autorotateToLandscapeRight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_autorotateToLandscapeRight)
    return GetScreenManager().GetIsOrientationEnabled(kAutorotateToLandscapeRight);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropAutorotateToLandscapeRight(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropAutorotateToLandscapeRight)
    SCRIPTINGAPI_STACK_CHECK(set_autorotateToLandscapeRight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_autorotateToLandscapeRight)
    
    GetScreenManager().SetIsOrientationEnabled(kAutorotateToLandscapeRight, value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScreenOrientation SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropOrientation()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropOrientation)
    SCRIPTINGAPI_STACK_CHECK(get_orientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_orientation)
    return GetScreenManager ().GetScreenOrientation ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropOrientation(ScreenOrientation value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropOrientation)
    SCRIPTINGAPI_STACK_CHECK(set_orientation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_orientation)
    
    GetScreenManager ().RequestOrientation (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Screen_Get_Custom_PropSleepTimeout()
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Get_Custom_PropSleepTimeout)
    SCRIPTINGAPI_STACK_CHECK(get_sleepTimeout)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sleepTimeout)
    return GetScreenManager ().GetScreenTimeout ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Screen_Set_Custom_PropSleepTimeout(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Screen_Set_Custom_PropSleepTimeout)
    SCRIPTINGAPI_STACK_CHECK(set_sleepTimeout)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sleepTimeout)
    
    GetScreenManager ().SetScreenTimeout (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_CreateRenderTexture(ICallType_Object_Argument rt_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_CreateRenderTexture)
    ScriptingObjectOfType<RenderTexture> rt(rt_);
    UNUSED(rt);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateRenderTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateRenderTexture)
    
    		RenderTexture* texture = NEW_OBJECT_MAIN_THREAD (RenderTexture);
    		texture->SetCreatedFromScript(true);
    		texture->Reset();
    		ConnectScriptingWrapperToObject (rt.GetScriptingObject(), texture);
    		texture->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_GetTemporary(int width, int height, int depthBuffer, RenderTextureFormat format, RenderTextureReadWrite readWrite, int antiAliasing)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_GetTemporary)
    SCRIPTINGAPI_STACK_CHECK(GetTemporary)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTemporary)
    
    		DepthBufferFormat depthFormat = DepthBufferFormatFromBits(depthBuffer);
    		UInt32 flags = RenderBufferManager::kRBCreatedFromScript;
    		return Scripting::ScriptingWrapperFor (GetRenderBufferManager().GetTempBuffer (width, height, depthFormat, static_cast<RenderTextureFormat>(format), flags, readWrite, antiAliasing));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_ReleaseTemporary(ICallType_ReadOnlyUnityEngineObject_Argument temp_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_ReleaseTemporary)
    ReadOnlyScriptingObjectOfType<RenderTexture> temp(temp_);
    UNUSED(temp);
    SCRIPTINGAPI_STACK_CHECK(ReleaseTemporary)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReleaseTemporary)
    
    		GetRenderBufferManager().ReleaseTempBuffer (temp);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_GetWidth(ICallType_ReadOnlyUnityEngineObject_Argument mono_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_GetWidth)
    ReadOnlyScriptingObjectOfType<RenderTexture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetWidth)
     return mono->GetWidth(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_SetWidth(ICallType_ReadOnlyUnityEngineObject_Argument mono_, int width)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_SetWidth)
    ReadOnlyScriptingObjectOfType<RenderTexture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetWidth)
     mono->SetWidth(width); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_GetHeight(ICallType_ReadOnlyUnityEngineObject_Argument mono_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_GetHeight)
    ReadOnlyScriptingObjectOfType<RenderTexture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetHeight)
     return mono->GetHeight(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_SetHeight(ICallType_ReadOnlyUnityEngineObject_Argument mono_, int width)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_SetHeight)
    ReadOnlyScriptingObjectOfType<RenderTexture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetHeight)
     mono->SetHeight(width); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_SetSRGBReadWrite(ICallType_ReadOnlyUnityEngineObject_Argument mono_, ScriptingBool sRGB)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_SetSRGBReadWrite)
    ReadOnlyScriptingObjectOfType<RenderTexture> mono(mono_);
    UNUSED(mono);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetSRGBReadWrite)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetSRGBReadWrite)
     mono->SetSRGBReadWrite(sRGB); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_depth)
    DepthBufferFormat depthFormat = self->GetDepthFormat();
    static int kDepthFormatBits[kDepthFormatCount] = { 0, 16, 24 };
    return kDepthFormatBits[depthFormat];
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_depth)
    
    DepthBufferFormat depthFormat = DepthBufferFormatFromBits(value);
    self->SetDepthFormat( depthFormat );
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropIsPowerOfTwo(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropIsPowerOfTwo)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPowerOfTwo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPowerOfTwo)
    return self->GetIsPowerOfTwo();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropIsPowerOfTwo(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropIsPowerOfTwo)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_isPowerOfTwo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isPowerOfTwo)
      
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropSRGB(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropSRGB)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_sRGB)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sRGB)
    return self->GetSRGBReadWrite();
}

SCRIPT_BINDINGS_EXPORT_DECL
RenderTextureFormat SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropFormat(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropFormat)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_format)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_format)
    return self->GetColorFormat();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropFormat(ICallType_ReadOnlyUnityEngineObject_Argument self_, RenderTextureFormat value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropFormat)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_format)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_format)
     self->SetColorFormat( static_cast<RenderTextureFormat>(value) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropUseMipMap(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropUseMipMap)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useMipMap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useMipMap)
    return self->GetMipMap ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropUseMipMap(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropUseMipMap)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useMipMap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useMipMap)
    
    self->SetMipMap (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropIsCubemap(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropIsCubemap)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isCubemap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isCubemap)
    return self->GetDimension()==kTexDimCUBE;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropIsCubemap(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropIsCubemap)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_isCubemap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isCubemap)
    
    self->SetDimension (value ? kTexDimCUBE : kTexDim2D);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropIsVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropIsVolume)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isVolume)
    return self->GetDimension()==kTexDim3D;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropIsVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropIsVolume)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_isVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isVolume)
    
    self->SetDimension (value ? kTexDim3D : kTexDim2D);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropVolumeDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropVolumeDepth)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_volumeDepth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_volumeDepth)
    return self->GetVolumeDepth();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropVolumeDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropVolumeDepth)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_volumeDepth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_volumeDepth)
     self->SetVolumeDepth(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropAntiAliasing(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropAntiAliasing)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_antiAliasing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_antiAliasing)
    return self->GetAntiAliasing ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropAntiAliasing(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropAntiAliasing)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_antiAliasing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_antiAliasing)
    
    self->SetAntiAliasing (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropEnableRandomWrite(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropEnableRandomWrite)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enableRandomWrite)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enableRandomWrite)
    return self->GetEnableRandomWrite ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropEnableRandomWrite(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropEnableRandomWrite)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enableRandomWrite)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enableRandomWrite)
    
    self->SetEnableRandomWrite (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_INTERNAL_CALL_Create(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_INTERNAL_CALL_Create)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Create)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Create)
    return self->Create();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_INTERNAL_CALL_Release(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_INTERNAL_CALL_Release)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Release)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Release)
    return self->Release();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_INTERNAL_CALL_IsCreated(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_INTERNAL_CALL_IsCreated)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_IsCreated)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_IsCreated)
    return self->IsCreated();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_INTERNAL_CALL_DiscardContents(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_INTERNAL_CALL_DiscardContents)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DiscardContents)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DiscardContents)
    return self->DiscardContents();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_GetColorBuffer(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingRenderBuffer* res)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_GetColorBuffer)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetColorBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetColorBuffer)
    
    		RenderTexture* rt = self;
    		if (rt)
    		{
    			res->m_RenderTextureInstanceID = rt->GetInstanceID();
    			if (!rt->IsCreated())
    				rt->Create();
    			res->m_BufferPtr = rt->GetColorSurfaceHandle().object;
    		}
    		else
    		{
    			res->m_RenderTextureInstanceID = 0;
    			res->m_BufferPtr = NULL;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_GetDepthBuffer(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingRenderBuffer* res)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_GetDepthBuffer)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetDepthBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetDepthBuffer)
    
    		RenderTexture* rt = self;
    		if (rt)
    		{
    			res->m_RenderTextureInstanceID = rt->GetInstanceID();
    			if (!rt->IsCreated())
    				rt->Create();
    			res->m_BufferPtr = rt->GetDepthSurfaceHandle().object;
    		}
    		else
    		{
    			res->m_RenderTextureInstanceID = 0;
    			res->m_BufferPtr = NULL;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_SetGlobalShaderProperty(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument propertyName_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_SetGlobalShaderProperty)
    ReadOnlyScriptingObjectOfType<RenderTexture> self(self_);
    UNUSED(self);
    ICallType_String_Local propertyName(propertyName_);
    UNUSED(propertyName);
    SCRIPTINGAPI_STACK_CHECK(SetGlobalShaderProperty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetGlobalShaderProperty)
     self->SetGlobalProperty (ScriptingStringToProperty (propertyName)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropActive()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropActive)
    SCRIPTINGAPI_STACK_CHECK(get_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_active)
    return Scripting::ScriptingWrapperFor (RenderTexture::GetActive ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropActive(ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropActive)
    ReadOnlyScriptingObjectOfType<RenderTexture> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_active)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_active)
     RenderTexture::SetActive (value); RenderTexture::FindAndSetSRGBWrite (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_Get_Custom_PropEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Get_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return RenderTexture::IsEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_Set_Custom_PropEnabled(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_Set_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
     RenderTexture::SetEnabled (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_Internal_GetTexelOffset(ICallType_ReadOnlyUnityEngineObject_Argument tex_, Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_Internal_GetTexelOffset)
    ReadOnlyScriptingObjectOfType<RenderTexture> tex(tex_);
    UNUSED(tex);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetTexelOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetTexelOffset)
    
    		if (gGraphicsCaps.usesHalfTexelOffset)
    		{
    			RenderTexture& renderTex = *tex;
    			output->x = renderTex.GetTexelSizeX() * 0.5f;
    			output->y = renderTex.GetTexelSizeY() * 0.5f;
    		}
    		else
    		{
    			output->x = 0.0f;
    			output->y = 0.0f;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION RenderTexture_CUSTOM_SupportsStencil(ICallType_ReadOnlyUnityEngineObject_Argument rt_)
{
    SCRIPTINGAPI_ETW_ENTRY(RenderTexture_CUSTOM_SupportsStencil)
    ReadOnlyScriptingObjectOfType<RenderTexture> rt(rt_);
    UNUSED(rt);
    SCRIPTINGAPI_STACK_CHECK(SupportsStencil)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SupportsStencil)
    
    		return RenderTextureSupportsStencil(rt);
    	
}

#if ENABLE_MOVIES
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MovieTexture_CUSTOM_INTERNAL_CALL_Play(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_CUSTOM_INTERNAL_CALL_Play)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Play)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Play)
    return self->Play();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MovieTexture_CUSTOM_INTERNAL_CALL_Stop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_CUSTOM_INTERNAL_CALL_Stop)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Stop)
    return self->Stop();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MovieTexture_CUSTOM_INTERNAL_CALL_Pause(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_CUSTOM_INTERNAL_CALL_Pause)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Pause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Pause)
    return self->Pause();
}

#endif
#if (ENABLE_MOVIES) && (ENABLE_AUDIO)
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION MovieTexture_Get_Custom_PropAudioClip(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_Get_Custom_PropAudioClip)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_audioClip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_audioClip)
    return Scripting::ScriptingWrapperFor(self->GetMovieAudioClip());
}

#endif
#if ENABLE_MOVIES
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MovieTexture_Get_Custom_PropLoop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_Get_Custom_PropLoop)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_loop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_loop)
    return self->GetLoop ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MovieTexture_Set_Custom_PropLoop(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_Set_Custom_PropLoop)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_loop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_loop)
    
    self->SetLoop (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MovieTexture_Get_Custom_PropIsPlaying(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_Get_Custom_PropIsPlaying)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPlaying)
    return self->IsPlaying ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MovieTexture_Get_Custom_PropIsReadyToPlay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_Get_Custom_PropIsReadyToPlay)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isReadyToPlay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isReadyToPlay)
    return self->ReadyToPlay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION MovieTexture_Get_Custom_PropDuration(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MovieTexture_Get_Custom_PropDuration)
    ReadOnlyScriptingObjectOfType<MovieTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_duration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_duration)
    return self->GetMovieDuration ();
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_Vertex3(float x, float y, float z)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_Vertex3)
    SCRIPTINGAPI_STACK_CHECK(Vertex3)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Vertex3)
     GetGfxDevice().ImmediateVertex( x,y,z ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_Vertex(const Vector3f& v)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_Vertex)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Vertex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Vertex)
     GetGfxDevice().ImmediateVertex( v.x, v.y, v.z ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_Color(const ColorRGBAf& c)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_Color)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Color)
     GetGfxDevice().ImmediateColor(c.r, c.g, c.b, c.a); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_TexCoord(const Vector3f& v)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_TexCoord)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TexCoord)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_TexCoord)
     GetGfxDevice().ImmediateTexCoordAll(v.x, v.y, v.z); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_TexCoord2(float x, float y)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_TexCoord2)
    SCRIPTINGAPI_STACK_CHECK(TexCoord2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TexCoord2)
     GetGfxDevice().ImmediateTexCoordAll(x, y, 0.0f); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_TexCoord3(float x, float y, float z)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_TexCoord3)
    SCRIPTINGAPI_STACK_CHECK(TexCoord3)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TexCoord3)
     GetGfxDevice().ImmediateTexCoordAll(x, y, z); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_MultiTexCoord2(int unit, float x, float y)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_MultiTexCoord2)
    SCRIPTINGAPI_STACK_CHECK(MultiTexCoord2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(MultiTexCoord2)
    
    		GetGfxDevice().ImmediateTexCoord( unit, x, y, 0.0f );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_MultiTexCoord3(int unit, float x, float y, float z)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_MultiTexCoord3)
    SCRIPTINGAPI_STACK_CHECK(MultiTexCoord3)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(MultiTexCoord3)
    
    		GetGfxDevice().ImmediateTexCoord( unit, x, y, z );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_MultiTexCoord(int unit, const Vector3f& v)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_MultiTexCoord)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_MultiTexCoord)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_MultiTexCoord)
    
    		GetGfxDevice().ImmediateTexCoord( unit, v.x, v.y, v.z );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_Begin(int mode)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_Begin)
    SCRIPTINGAPI_STACK_CHECK(Begin)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Begin)
    
    		GfxPrimitiveType pt;
    		if( mode == 0x0004 )
    			pt = kPrimitiveTriangles;
    		else if( mode == 0x0005 )
    			pt = kPrimitiveTriangleStripDeprecated;
    		else if( mode == 0x0007 )
    			pt = kPrimitiveQuads;
    		else if( mode == 0x0001 )
    			pt = kPrimitiveLines;
    		else {
    			RaiseMonoException( "Invalid mode for GL.Begin" );
    			return;
    		}
    		GetGfxDevice().ImmediateBegin( pt );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_End()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_End)
    SCRIPTINGAPI_STACK_CHECK(End)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(End)
    
    		GetGfxDevice().ImmediateEnd();
    		GPU_TIMESTAMP();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_LoadOrtho()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_LoadOrtho)
    SCRIPTINGAPI_STACK_CHECK(LoadOrtho)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadOrtho)
    
    		LoadFullScreenOrthoMatrix();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_LoadPixelMatrix()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_LoadPixelMatrix)
    SCRIPTINGAPI_STACK_CHECK(LoadPixelMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadPixelMatrix)
    
    		if (GetCurrentCameraPtr() != NULL)
    		{
    			LoadPixelMatrix( GetCurrentCamera().GetScreenViewportRect(), GetGfxDevice(), true, true );
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_LoadPixelMatrixArgs(float left, float right, float bottom, float top)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_LoadPixelMatrixArgs)
    SCRIPTINGAPI_STACK_CHECK(LoadPixelMatrixArgs)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadPixelMatrixArgs)
    
    		Rectf rect( left, bottom, right-left, top-bottom );
    		LoadPixelMatrix( rect, GetGfxDevice(), true, true );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_Viewport(const Rectf& pixelRect)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_Viewport)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Viewport)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Viewport)
    
    		SetGLViewport(pixelRect);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_LoadProjectionMatrix(const Matrix4x4f& mat)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_LoadProjectionMatrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_LoadProjectionMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_LoadProjectionMatrix)
    
    		GetGfxDevice().SetProjectionMatrix (mat);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_LoadIdentity()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_LoadIdentity)
    SCRIPTINGAPI_STACK_CHECK(LoadIdentity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadIdentity)
    
    		GetGfxDevice().SetViewMatrix (Matrix4x4f::identity.GetPtr()); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_get_modelview(Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_get_modelview)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_modelview)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_modelview)
    Matrix4x4f temp;
    GetGfxDevice().GetMatrix( temp.GetPtr() );
    { *returnValue =(temp); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_set_modelview(const Matrix4x4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_set_modelview)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_modelview)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_modelview)
    
    GetGfxDevice().SetViewMatrix( value.GetPtr() );
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_MultMatrix(const Matrix4x4f& mat)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_MultMatrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_MultMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_MultMatrix)
    
    		GetGfxDevice().SetWorldMatrix( mat.GetPtr() );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_PushMatrix()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_PushMatrix)
    SCRIPTINGAPI_STACK_CHECK(PushMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PushMatrix)
    
    		GfxDevice& dev = GetGfxDevice();
    		g_ViewMatrixStack.Push(dev.GetViewMatrix());
    		g_WorldMatrixStack.Push(dev.GetWorldMatrix());
    		g_ProjectionMatrixStack.Push(dev.GetProjectionMatrix());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_PopMatrix()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_PopMatrix)
    SCRIPTINGAPI_STACK_CHECK(PopMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PopMatrix)
    
    		GfxDevice& dev = GetGfxDevice();
    		g_WorldMatrixStack.Pop();
    		g_ViewMatrixStack.Pop();
    		g_ProjectionMatrixStack.Pop();
    		dev.SetViewMatrix(g_ViewMatrixStack.GetMatrix().GetPtr());
    		dev.SetWorldMatrix(g_WorldMatrixStack.GetMatrix().GetPtr());
    		dev.SetProjectionMatrix(g_ProjectionMatrixStack.GetMatrix());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_GetGPUProjectionMatrix(const Matrix4x4f& proj, ScriptingBool renderIntoTexture, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_GetGPUProjectionMatrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetGPUProjectionMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetGPUProjectionMatrix)
    
    		bool openGLStyle = gGraphicsCaps.usesOpenGLTextureCoords;
    		Matrix4x4f m = proj;
    		CalculateDeviceProjectionMatrix (m, openGLStyle, !openGLStyle && renderIntoTexture);
    		{ returnValue =(m); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GL_Get_Custom_PropWireframe()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_Get_Custom_PropWireframe)
    SCRIPTINGAPI_STACK_CHECK(get_wireframe)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wireframe)
    return GetGfxDevice().GetWireframe();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_Set_Custom_PropWireframe(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_Set_Custom_PropWireframe)
    SCRIPTINGAPI_STACK_CHECK(set_wireframe)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wireframe)
     GetGfxDevice().SetWireframe(value);  
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_SetRevertBackfacing(ScriptingBool revertBackFaces)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_SetRevertBackfacing)
    SCRIPTINGAPI_STACK_CHECK(SetRevertBackfacing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetRevertBackfacing)
     GetGfxDevice().SetUserBackfaceMode (revertBackFaces); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_INTERNAL_CALL_Internal_Clear(ScriptingBool clearDepth, ScriptingBool clearColor, const ColorRGBAf& backgroundColor, float depth)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_INTERNAL_CALL_Internal_Clear)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Clear)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Clear)
    
    		UInt32 flags = 0;
    		if (clearColor) flags |= kGfxClearColor;
    		if (clearDepth) flags |= kGfxClearDepthStencil;
    		GetGfxDevice().Clear (flags, backgroundColor.GetPtr(), depth, 0);
    		GPU_TIMESTAMP();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_ClearWithSkybox(ScriptingBool clearDepth, ICallType_ReadOnlyUnityEngineObject_Argument camera_)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_ClearWithSkybox)
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(ClearWithSkybox)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearWithSkybox)
    
    		ClearWithSkybox(clearDepth, camera);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_InvalidateState()
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_InvalidateState)
    SCRIPTINGAPI_STACK_CHECK(InvalidateState)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InvalidateState)
    
    		GL_CUSTOM_PushMatrix();
    		GfxDevice& dev = GetGfxDevice();
    		dev.InvalidateState();
    		GL_CUSTOM_PopMatrix();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GL_CUSTOM_IssuePluginEvent(int eventID)
{
    SCRIPTINGAPI_ETW_ENTRY(GL_CUSTOM_IssuePluginEvent)
    SCRIPTINGAPI_STACK_CHECK(IssuePluginEvent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IssuePluginEvent)
    
    		GetGfxDevice().InsertCustomMarker (eventID);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIElement_CUSTOM_INTERNAL_CALL_HitTest(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& screenPosition, ICallType_ReadOnlyUnityEngineObject_Argument camera_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIElement_CUSTOM_INTERNAL_CALL_HitTest)
    ReadOnlyScriptingObjectOfType<GUIElement> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_HitTest)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_HitTest)
    
    		return self->HitTest (Vector2f (screenPosition.x, screenPosition.y), GetCameraOrWindowRect(camera));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIElement_CUSTOM_INTERNAL_CALL_GetScreenRect(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument camera_, Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIElement_CUSTOM_INTERNAL_CALL_GetScreenRect)
    ReadOnlyScriptingObjectOfType<GUIElement> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetScreenRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetScreenRect)
     { returnValue =(self->GetScreenRect (GetCameraOrWindowRect(camera))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUITexture_CUSTOM_INTERNAL_get_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_CUSTOM_INTERNAL_get_color)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    *returnValue = self->GetColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUITexture_CUSTOM_INTERNAL_set_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_CUSTOM_INTERNAL_set_color)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
    
    self->SetColor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUITexture_Get_Custom_PropTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_Get_Custom_PropTexture)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_texture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_texture)
    return Scripting::ScriptingWrapperFor(self->GetTexture());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUITexture_Set_Custom_PropTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_Set_Custom_PropTexture)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Texture> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_texture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_texture)
    
    self->SetTexture (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUITexture_CUSTOM_INTERNAL_get_pixelInset(ICallType_ReadOnlyUnityEngineObject_Argument self_, Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_CUSTOM_INTERNAL_get_pixelInset)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_pixelInset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_pixelInset)
    { *returnValue =(self->GetPixelInset()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUITexture_CUSTOM_INTERNAL_set_pixelInset(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Rectf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_CUSTOM_INTERNAL_set_pixelInset)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_pixelInset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_pixelInset)
     self->SetPixelInset(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUITexture_Get_Custom_PropBorder(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_Get_Custom_PropBorder)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_border)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_border)
    RectOffset* rectOffset = new RectOffset ();
    rectOffset->left	= self->m_LeftBorder;
    rectOffset->right	= self->m_RightBorder;
    rectOffset->top		= self->m_TopBorder;
    rectOffset->bottom	= self->m_BottomBorder;
    ScriptingObjectWithIntPtrField<RectOffset> monoObject(scripting_object_new (GetCommonScriptingClasses ().rectOffset));
    monoObject.SetPtr(rectOffset);
    return monoObject.object;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUITexture_Set_Custom_PropBorder(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUITexture_Set_Custom_PropBorder)
    ReadOnlyScriptingObjectOfType<GUITexture> self(self_);
    UNUSED(self);
    ScriptingObjectWithIntPtrField<RectOffset> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_border)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_border)
    
    self->m_LeftBorder		= value->left;
    self->m_RightBorder		= value->right;
    self->m_TopBorder		= value->top;
    self->m_BottomBorder	= value->bottom;
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropText)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_text)
    return CreateScriptingString (self->GetText ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropText)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_text)
     self->SetText (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    Material* material = self->GetMaterial ();
    if (material == NULL)
    material = GetBuiltinResource<Material> ("Font.mat");
    Material* instantiated = &Material::GetInstantiatedMaterial (material, *self, false);
    if (material != instantiated)
    self->SetMaterial (instantiated);
    return Scripting::ScriptingWrapperFor (instantiated);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_material)
    
    self->SetMaterial (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_CUSTOM_Internal_GetPixelOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_CUSTOM_Internal_GetPixelOffset)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetPixelOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetPixelOffset)
    
    		*output = self->GetPixelOffset();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_CUSTOM_INTERNAL_CALL_Internal_SetPixelOffset(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& p)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_CUSTOM_INTERNAL_CALL_Internal_SetPixelOffset)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_SetPixelOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_SetPixelOffset)
     self->SetPixelOffset(p); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropFont(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropFont)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_font)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_font)
    return Scripting::ScriptingWrapperFor(self->GetFont());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropFont(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropFont)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Font> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_font)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_font)
    
    self->SetFont (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropAlignment(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropAlignment)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alignment)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alignment)
    return self->GetAlignment ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropAlignment(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropAlignment)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_alignment)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alignment)
    
    self->SetAlignment (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropAnchor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropAnchor)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_anchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_anchor)
    return self->GetAnchor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropAnchor(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropAnchor)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_anchor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_anchor)
    
    self->SetAnchor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropLineSpacing(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropLineSpacing)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lineSpacing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lineSpacing)
    return self->GetLineSpacing ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropLineSpacing(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropLineSpacing)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lineSpacing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lineSpacing)
    
    self->SetLineSpacing (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropTabSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropTabSize)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_tabSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_tabSize)
    return self->GetTabSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropTabSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropTabSize)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_tabSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_tabSize)
    
    self->SetTabSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropFontSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropFontSize)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontSize)
    return self->GetFontSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropFontSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropFontSize)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fontSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontSize)
    
    self->SetFontSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropFontStyle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropFontStyle)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontStyle)
    return self->GetFontStyle ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropFontStyle(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropFontStyle)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fontStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontStyle)
    
    self->SetFontStyle (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIText_Get_Custom_PropRichText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Get_Custom_PropRichText)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_richText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_richText)
    return self->GetRichText ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_Set_Custom_PropRichText(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_Set_Custom_PropRichText)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_richText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_richText)
    
    self->SetRichText (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_CUSTOM_INTERNAL_get_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_CUSTOM_INTERNAL_get_color)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    *returnValue = self->GetColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIText_CUSTOM_INTERNAL_set_color(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIText_CUSTOM_INTERNAL_set_color)
    ReadOnlyScriptingObjectOfType<GUIText> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
    
    self->SetColor (value);
    
}



struct ScriptingCharacterInfo {
	int index;
	Rectf uv, vert;
	float width;
	int size, style;
	int flippedInt;
};
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Font_CUSTOM_Internal_CreateFont(ICallType_Object_Argument _font_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_CUSTOM_Internal_CreateFont)
    ScriptingObjectOfType<Font> _font(_font_);
    UNUSED(_font);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateFont)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateFont)
    
    		Font* font = NEW_OBJECT (Font);
    		SmartResetObject(*font);
    		font->SetNameCpp (name);
    		ConnectScriptingWrapperToObject (_font.GetScriptingObject(), font);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Font_Get_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_Get_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    return Scripting::ScriptingWrapperFor(self->GetMaterial());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Font_Set_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_Set_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Material> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_material)
    
    self->SetMaterial (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Font_CUSTOM_HasCharacter(ICallType_ReadOnlyUnityEngineObject_Argument self_, UInt16 c)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_CUSTOM_HasCharacter)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(HasCharacter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasCharacter)
     return self->HasCharacter((int)c);
}

#if ENABLE_MONO
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Font_Get_Custom_PropFontNames(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_Get_Custom_PropFontNames)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontNames)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontNames)
    MonoArray *arr = mono_array_new (mono_domain_get (), mono_get_string_class(), self->GetFontNames().size());
    int idx = 0;
    for (UNITY_VECTOR(kMemFont,UnityStr)::const_iterator i=self->GetFontNames().begin ();i != self->GetFontNames().end ();i++)
    {
    GetMonoArrayElement<MonoString*> (arr,idx) = CreateScriptingString (*i);
    idx++;
    }
    return arr;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Font_Set_Custom_PropFontNames(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_Set_Custom_PropFontNames)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_fontNames)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontNames)
    
    UNITY_VECTOR(kMemFont,UnityStr) names;
    for (int i=0;i<mono_array_length_safe(value);i++)
    names.push_back (scripting_cpp_string_for(GetMonoArrayElement<MonoString*>(value, i)));
    self->SetFontNames(names);
    
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Font_Get_Custom_PropCharacterInfo(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_Get_Custom_PropCharacterInfo)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_characterInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_characterInfo)
    const Font::CharacterInfos &infos = self->GetCharacterInfos();
    int size = infos.size();
    ScriptingArrayPtr array = CreateScriptingArray<ScriptingCharacterInfo> (GetMonoManager().GetCommonClasses().characterInfo, size);
    ScriptingCharacterInfo *sci = GetScriptingArrayStart<ScriptingCharacterInfo>(array);
    for (int i=0; i<size; i++)
    {
    sci[i].index = infos[i].index;
    sci[i].uv = infos[i].uv;
    sci[i].vert = infos[i].vert;
    sci[i].width = infos[i].width;
    sci[i].flippedInt = infos[i].flipped ? 1 : 0;
    sci[i].size = infos[i].size;
    sci[i].style = infos[i].style;
    }
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Font_Set_Custom_PropCharacterInfo(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_Set_Custom_PropCharacterInfo)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_characterInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_characterInfo)
    
    Font::CharacterInfos infos;
    int size = GetScriptingArraySize(value);
    infos.resize(size);
    ScriptingCharacterInfo *sci = GetScriptingArrayStart<ScriptingCharacterInfo>(value);
    for (int i=0; i<size; i++)
    {
    infos[i].index = sci[i].index;
    infos[i].uv = sci[i].uv;
    infos[i].vert = sci[i].vert;
    infos[i].width = sci[i].width;
    infos[i].flipped = sci[i].flippedInt != 0;
    infos[i].size = sci[i].size;
    infos[i].style = sci[i].style;
    }
    self->SetCharacterInfos (infos);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Font_CUSTOM_RequestCharactersInTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument characters_, int size, int style)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_CUSTOM_RequestCharactersInTexture)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    ICallType_String_Local characters(characters_);
    UNUSED(characters);
    SCRIPTINGAPI_STACK_CHECK(RequestCharactersInTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RequestCharactersInTexture)
    
    		UTF16String str(characters.ToUTF8().c_str());
    		self->CacheFontForText (str.text, str.length, size, style);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Font_CUSTOM_GetCharacterInfo(ICallType_ReadOnlyUnityEngineObject_Argument self_, UInt16 ch, ScriptingCharacterInfo* info, int size, int style)
{
    SCRIPTINGAPI_ETW_ENTRY(Font_CUSTOM_GetCharacterInfo)
    ReadOnlyScriptingObjectOfType<Font> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetCharacterInfo)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetCharacterInfo)
    
    		if (self->HasCharacterInTexture (ch, size, style))
    		{
    			info->index = ch;
    			info->size = size;
    			info->style = style;
    			bool flipped;
    			self->GetCharacterRenderInfo( ch, size, style, info->vert, info->uv, flipped);
    			info->flippedInt = flipped ? 1 : 0;
    			info->width = self->GetCharacterWidth (ch, size, style);
    			return true;
    		}
    		return false;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUILayer_CUSTOM_INTERNAL_CALL_HitTest(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& screenPosition)
{
    SCRIPTINGAPI_ETW_ENTRY(GUILayer_CUSTOM_INTERNAL_CALL_HitTest)
    ReadOnlyScriptingObjectOfType<GUILayer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_HitTest)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_HitTest)
     return Scripting::ScriptingWrapperFor (self->HitTest (Vector2f (screenPosition.x, screenPosition.y))); 
}



	struct InternalMonoMeshInstance
	{
		int					meshInstanceID;
		Matrix4x4f			transform;
		Vector4f			lightmapTilingOffset;
	};
#if !PLATFORM_WEBGL && !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION StaticBatchingUtility_CUSTOM_InternalCombineVertices(ICallType_Array_Argument meshes_, ICallType_String_Argument meshName_)
{
    SCRIPTINGAPI_ETW_ENTRY(StaticBatchingUtility_CUSTOM_InternalCombineVertices)
    ICallType_Array_Local meshes(meshes_);
    UNUSED(meshes);
    ICallType_String_Local meshName(meshName_);
    UNUSED(meshName);
    SCRIPTINGAPI_STACK_CHECK(InternalCombineVertices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalCombineVertices)
    
    		const int numMeshes = GetScriptingArraySize( meshes );
    
    		CombineInstances mi;
    
    		mi.resize ( numMeshes );
    		InternalMonoMeshInstance* instances = GetScriptingArrayStart<InternalMonoMeshInstance>( meshes );
    		for ( int i=0; i!=numMeshes; ++i )
    		{
    			mi[i].mesh = PPtr<Mesh>(instances[i].meshInstanceID);
    			mi[i].transform = instances[i].transform;
    			mi[i].lightmapTilingOffset = instances[i].lightmapTilingOffset;
    		}
    
    		Mesh* outMesh = CreateObjectFromCode<Mesh> (kDefaultAwakeFromLoad);
    
    		CombineMeshVerticesForStaticBatching ( mi, meshName, *outMesh );
    
    		return Scripting::ScriptingWrapperFor( outMesh );
    	
}

#endif


	struct InternalMonoSubMeshInstance
	{
		int	meshInstanceID;		
		int vertexOffset;			
		int goInstanceID;	
		int subMeshIndex;			
		Matrix4x4f transform;
	};
#if !PLATFORM_WEBGL && !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION StaticBatchingUtility_CUSTOM_InternalCombineIndices(ICallType_Array_Argument submeshes_, ICallType_Object_Argument combinedMesh_)
{
    SCRIPTINGAPI_ETW_ENTRY(StaticBatchingUtility_CUSTOM_InternalCombineIndices)
    ICallType_Array_Local submeshes(submeshes_);
    UNUSED(submeshes);
    ScriptingObjectOfType<Mesh> combinedMesh(combinedMesh_);
    UNUSED(combinedMesh);
    SCRIPTINGAPI_STACK_CHECK(InternalCombineIndices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalCombineIndices)
    
    		InternalMonoSubMeshInstance* instances = GetScriptingArrayStart<InternalMonoSubMeshInstance>( submeshes );
    		int numSubMeshes = GetScriptingArraySize( submeshes );
    		CombineInstances smi;
    		smi.resize( numSubMeshes );
    
    		for( int i=0; i!=numSubMeshes; ++i )
    		{
    			smi[i].mesh				= PPtr<Mesh>(instances[i].meshInstanceID);
    			smi[i].vertexOffset		= instances[i].vertexOffset;
    			smi[i].subMeshIndex		= instances[i].subMeshIndex;
    			smi[i].transform = instances[i].transform;
    		}
    
    		Mesh* mesh = combinedMesh;
    		CombineMeshIndicesForStaticBatching (smi, *mesh, false, true);
    		mesh->SetIsReadable (false);
    		mesh->SetKeepIndices (true);
    		mesh->AwakeFromLoad (kDefaultAwakeFromLoad);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_OcclusionArea_INTERNAL_get_center()
{
    scripting_add_internal_call( "UnityEngine.OcclusionArea::INTERNAL_get_center" , (gpointer)& OcclusionArea_CUSTOM_INTERNAL_get_center );
}

void Register_UnityEngine_OcclusionArea_INTERNAL_set_center()
{
    scripting_add_internal_call( "UnityEngine.OcclusionArea::INTERNAL_set_center" , (gpointer)& OcclusionArea_CUSTOM_INTERNAL_set_center );
}

void Register_UnityEngine_OcclusionArea_INTERNAL_get_size()
{
    scripting_add_internal_call( "UnityEngine.OcclusionArea::INTERNAL_get_size" , (gpointer)& OcclusionArea_CUSTOM_INTERNAL_get_size );
}

void Register_UnityEngine_OcclusionArea_INTERNAL_set_size()
{
    scripting_add_internal_call( "UnityEngine.OcclusionArea::INTERNAL_set_size" , (gpointer)& OcclusionArea_CUSTOM_INTERNAL_set_size );
}

void Register_UnityEngine_OcclusionPortal_get_open()
{
    scripting_add_internal_call( "UnityEngine.OcclusionPortal::get_open" , (gpointer)& OcclusionPortal_Get_Custom_PropOpen );
}

void Register_UnityEngine_OcclusionPortal_set_open()
{
    scripting_add_internal_call( "UnityEngine.OcclusionPortal::set_open" , (gpointer)& OcclusionPortal_Set_Custom_PropOpen );
}

void Register_UnityEngine_RenderSettings_get_fog()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_fog" , (gpointer)& RenderSettings_Get_Custom_PropFog );
}

void Register_UnityEngine_RenderSettings_set_fog()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_fog" , (gpointer)& RenderSettings_Set_Custom_PropFog );
}

void Register_UnityEngine_RenderSettings_get_fogMode()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_fogMode" , (gpointer)& RenderSettings_Get_Custom_PropFogMode );
}

void Register_UnityEngine_RenderSettings_set_fogMode()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_fogMode" , (gpointer)& RenderSettings_Set_Custom_PropFogMode );
}

void Register_UnityEngine_RenderSettings_INTERNAL_get_fogColor()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::INTERNAL_get_fogColor" , (gpointer)& RenderSettings_CUSTOM_INTERNAL_get_fogColor );
}

void Register_UnityEngine_RenderSettings_INTERNAL_set_fogColor()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::INTERNAL_set_fogColor" , (gpointer)& RenderSettings_CUSTOM_INTERNAL_set_fogColor );
}

void Register_UnityEngine_RenderSettings_get_fogDensity()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_fogDensity" , (gpointer)& RenderSettings_Get_Custom_PropFogDensity );
}

void Register_UnityEngine_RenderSettings_set_fogDensity()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_fogDensity" , (gpointer)& RenderSettings_Set_Custom_PropFogDensity );
}

void Register_UnityEngine_RenderSettings_get_fogStartDistance()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_fogStartDistance" , (gpointer)& RenderSettings_Get_Custom_PropFogStartDistance );
}

void Register_UnityEngine_RenderSettings_set_fogStartDistance()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_fogStartDistance" , (gpointer)& RenderSettings_Set_Custom_PropFogStartDistance );
}

void Register_UnityEngine_RenderSettings_get_fogEndDistance()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_fogEndDistance" , (gpointer)& RenderSettings_Get_Custom_PropFogEndDistance );
}

void Register_UnityEngine_RenderSettings_set_fogEndDistance()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_fogEndDistance" , (gpointer)& RenderSettings_Set_Custom_PropFogEndDistance );
}

void Register_UnityEngine_RenderSettings_INTERNAL_get_ambientLight()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::INTERNAL_get_ambientLight" , (gpointer)& RenderSettings_CUSTOM_INTERNAL_get_ambientLight );
}

void Register_UnityEngine_RenderSettings_INTERNAL_set_ambientLight()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::INTERNAL_set_ambientLight" , (gpointer)& RenderSettings_CUSTOM_INTERNAL_set_ambientLight );
}

void Register_UnityEngine_RenderSettings_get_haloStrength()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_haloStrength" , (gpointer)& RenderSettings_Get_Custom_PropHaloStrength );
}

void Register_UnityEngine_RenderSettings_set_haloStrength()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_haloStrength" , (gpointer)& RenderSettings_Set_Custom_PropHaloStrength );
}

void Register_UnityEngine_RenderSettings_get_flareStrength()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_flareStrength" , (gpointer)& RenderSettings_Get_Custom_PropFlareStrength );
}

void Register_UnityEngine_RenderSettings_set_flareStrength()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_flareStrength" , (gpointer)& RenderSettings_Set_Custom_PropFlareStrength );
}

void Register_UnityEngine_RenderSettings_get_skybox()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::get_skybox" , (gpointer)& RenderSettings_Get_Custom_PropSkybox );
}

void Register_UnityEngine_RenderSettings_set_skybox()
{
    scripting_add_internal_call( "UnityEngine.RenderSettings::set_skybox" , (gpointer)& RenderSettings_Set_Custom_PropSkybox );
}

#if ENABLE_MONO || PLATFORM_WINRT
void Register_UnityEngine_QualitySettings_get_names()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_names" , (gpointer)& QualitySettings_Get_Custom_PropNames );
}

#endif
void Register_UnityEngine_QualitySettings_GetQualityLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::GetQualityLevel" , (gpointer)& QualitySettings_CUSTOM_GetQualityLevel );
}

void Register_UnityEngine_QualitySettings_SetQualityLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::SetQualityLevel" , (gpointer)& QualitySettings_CUSTOM_SetQualityLevel );
}

void Register_UnityEngine_QualitySettings_get_currentLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_currentLevel" , (gpointer)& QualitySettings_Get_Custom_PropCurrentLevel );
}

void Register_UnityEngine_QualitySettings_set_currentLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_currentLevel" , (gpointer)& QualitySettings_Set_Custom_PropCurrentLevel );
}

void Register_UnityEngine_QualitySettings_IncreaseLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::IncreaseLevel" , (gpointer)& QualitySettings_CUSTOM_IncreaseLevel );
}

void Register_UnityEngine_QualitySettings_DecreaseLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::DecreaseLevel" , (gpointer)& QualitySettings_CUSTOM_DecreaseLevel );
}

void Register_UnityEngine_QualitySettings_get_pixelLightCount()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_pixelLightCount" , (gpointer)& QualitySettings_Get_Custom_PropPixelLightCount );
}

void Register_UnityEngine_QualitySettings_set_pixelLightCount()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_pixelLightCount" , (gpointer)& QualitySettings_Set_Custom_PropPixelLightCount );
}

void Register_UnityEngine_QualitySettings_get_shadowProjection()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_shadowProjection" , (gpointer)& QualitySettings_Get_Custom_PropShadowProjection );
}

void Register_UnityEngine_QualitySettings_set_shadowProjection()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_shadowProjection" , (gpointer)& QualitySettings_Set_Custom_PropShadowProjection );
}

void Register_UnityEngine_QualitySettings_get_shadowCascades()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_shadowCascades" , (gpointer)& QualitySettings_Get_Custom_PropShadowCascades );
}

void Register_UnityEngine_QualitySettings_set_shadowCascades()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_shadowCascades" , (gpointer)& QualitySettings_Set_Custom_PropShadowCascades );
}

void Register_UnityEngine_QualitySettings_get_shadowDistance()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_shadowDistance" , (gpointer)& QualitySettings_Get_Custom_PropShadowDistance );
}

void Register_UnityEngine_QualitySettings_set_shadowDistance()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_shadowDistance" , (gpointer)& QualitySettings_Set_Custom_PropShadowDistance );
}

void Register_UnityEngine_QualitySettings_get_masterTextureLimit()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_masterTextureLimit" , (gpointer)& QualitySettings_Get_Custom_PropMasterTextureLimit );
}

void Register_UnityEngine_QualitySettings_set_masterTextureLimit()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_masterTextureLimit" , (gpointer)& QualitySettings_Set_Custom_PropMasterTextureLimit );
}

void Register_UnityEngine_QualitySettings_get_anisotropicFiltering()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_anisotropicFiltering" , (gpointer)& QualitySettings_Get_Custom_PropAnisotropicFiltering );
}

void Register_UnityEngine_QualitySettings_set_anisotropicFiltering()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_anisotropicFiltering" , (gpointer)& QualitySettings_Set_Custom_PropAnisotropicFiltering );
}

void Register_UnityEngine_QualitySettings_get_lodBias()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_lodBias" , (gpointer)& QualitySettings_Get_Custom_PropLodBias );
}

void Register_UnityEngine_QualitySettings_set_lodBias()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_lodBias" , (gpointer)& QualitySettings_Set_Custom_PropLodBias );
}

void Register_UnityEngine_QualitySettings_get_maximumLODLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_maximumLODLevel" , (gpointer)& QualitySettings_Get_Custom_PropMaximumLODLevel );
}

void Register_UnityEngine_QualitySettings_set_maximumLODLevel()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_maximumLODLevel" , (gpointer)& QualitySettings_Set_Custom_PropMaximumLODLevel );
}

void Register_UnityEngine_QualitySettings_get_particleRaycastBudget()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_particleRaycastBudget" , (gpointer)& QualitySettings_Get_Custom_PropParticleRaycastBudget );
}

void Register_UnityEngine_QualitySettings_set_particleRaycastBudget()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_particleRaycastBudget" , (gpointer)& QualitySettings_Set_Custom_PropParticleRaycastBudget );
}

void Register_UnityEngine_QualitySettings_get_softVegetation()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_softVegetation" , (gpointer)& QualitySettings_Get_Custom_PropSoftVegetation );
}

void Register_UnityEngine_QualitySettings_set_softVegetation()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_softVegetation" , (gpointer)& QualitySettings_Set_Custom_PropSoftVegetation );
}

void Register_UnityEngine_QualitySettings_get_maxQueuedFrames()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_maxQueuedFrames" , (gpointer)& QualitySettings_Get_Custom_PropMaxQueuedFrames );
}

void Register_UnityEngine_QualitySettings_set_maxQueuedFrames()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_maxQueuedFrames" , (gpointer)& QualitySettings_Set_Custom_PropMaxQueuedFrames );
}

void Register_UnityEngine_QualitySettings_get_vSyncCount()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_vSyncCount" , (gpointer)& QualitySettings_Get_Custom_PropVSyncCount );
}

void Register_UnityEngine_QualitySettings_set_vSyncCount()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_vSyncCount" , (gpointer)& QualitySettings_Set_Custom_PropVSyncCount );
}

void Register_UnityEngine_QualitySettings_get_antiAliasing()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_antiAliasing" , (gpointer)& QualitySettings_Get_Custom_PropAntiAliasing );
}

void Register_UnityEngine_QualitySettings_set_antiAliasing()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_antiAliasing" , (gpointer)& QualitySettings_Set_Custom_PropAntiAliasing );
}

void Register_UnityEngine_QualitySettings_get_desiredColorSpace()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_desiredColorSpace" , (gpointer)& QualitySettings_Get_Custom_PropDesiredColorSpace );
}

void Register_UnityEngine_QualitySettings_get_activeColorSpace()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_activeColorSpace" , (gpointer)& QualitySettings_Get_Custom_PropActiveColorSpace );
}

void Register_UnityEngine_QualitySettings_get_blendWeights()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::get_blendWeights" , (gpointer)& QualitySettings_Get_Custom_PropBlendWeights );
}

void Register_UnityEngine_QualitySettings_set_blendWeights()
{
    scripting_add_internal_call( "UnityEngine.QualitySettings::set_blendWeights" , (gpointer)& QualitySettings_Set_Custom_PropBlendWeights );
}

void Register_UnityEngine_Shader_Find()
{
    scripting_add_internal_call( "UnityEngine.Shader::Find" , (gpointer)& Shader_CUSTOM_Find );
}

void Register_UnityEngine_Shader_FindBuiltin()
{
    scripting_add_internal_call( "UnityEngine.Shader::FindBuiltin" , (gpointer)& Shader_CUSTOM_FindBuiltin );
}

void Register_UnityEngine_Shader_get_isSupported()
{
    scripting_add_internal_call( "UnityEngine.Shader::get_isSupported" , (gpointer)& Shader_Get_Custom_PropIsSupported );
}

#if UNITY_EDITOR
void Register_UnityEngine_Shader_get_customEditor()
{
    scripting_add_internal_call( "UnityEngine.Shader::get_customEditor" , (gpointer)& Shader_Get_Custom_PropCustomEditor );
}

#endif
void Register_UnityEngine_Shader_EnableKeyword()
{
    scripting_add_internal_call( "UnityEngine.Shader::EnableKeyword" , (gpointer)& Shader_CUSTOM_EnableKeyword );
}

void Register_UnityEngine_Shader_DisableKeyword()
{
    scripting_add_internal_call( "UnityEngine.Shader::DisableKeyword" , (gpointer)& Shader_CUSTOM_DisableKeyword );
}

void Register_UnityEngine_Shader_get_maximumLOD()
{
    scripting_add_internal_call( "UnityEngine.Shader::get_maximumLOD" , (gpointer)& Shader_Get_Custom_PropMaximumLOD );
}

void Register_UnityEngine_Shader_set_maximumLOD()
{
    scripting_add_internal_call( "UnityEngine.Shader::set_maximumLOD" , (gpointer)& Shader_Set_Custom_PropMaximumLOD );
}

void Register_UnityEngine_Shader_get_globalMaximumLOD()
{
    scripting_add_internal_call( "UnityEngine.Shader::get_globalMaximumLOD" , (gpointer)& Shader_Get_Custom_PropGlobalMaximumLOD );
}

void Register_UnityEngine_Shader_set_globalMaximumLOD()
{
    scripting_add_internal_call( "UnityEngine.Shader::set_globalMaximumLOD" , (gpointer)& Shader_Set_Custom_PropGlobalMaximumLOD );
}

void Register_UnityEngine_Shader_get_renderQueue()
{
    scripting_add_internal_call( "UnityEngine.Shader::get_renderQueue" , (gpointer)& Shader_Get_Custom_PropRenderQueue );
}

void Register_UnityEngine_Shader_INTERNAL_CALL_SetGlobalColor()
{
    scripting_add_internal_call( "UnityEngine.Shader::INTERNAL_CALL_SetGlobalColor" , (gpointer)& Shader_CUSTOM_INTERNAL_CALL_SetGlobalColor );
}

void Register_UnityEngine_Shader_SetGlobalFloat()
{
    scripting_add_internal_call( "UnityEngine.Shader::SetGlobalFloat" , (gpointer)& Shader_CUSTOM_SetGlobalFloat );
}

void Register_UnityEngine_Shader_SetGlobalTexture()
{
    scripting_add_internal_call( "UnityEngine.Shader::SetGlobalTexture" , (gpointer)& Shader_CUSTOM_SetGlobalTexture );
}

void Register_UnityEngine_Shader_INTERNAL_CALL_SetGlobalMatrix()
{
    scripting_add_internal_call( "UnityEngine.Shader::INTERNAL_CALL_SetGlobalMatrix" , (gpointer)& Shader_CUSTOM_INTERNAL_CALL_SetGlobalMatrix );
}

void Register_UnityEngine_Shader_SetGlobalTexGenMode()
{
    scripting_add_internal_call( "UnityEngine.Shader::SetGlobalTexGenMode" , (gpointer)& Shader_CUSTOM_SetGlobalTexGenMode );
}

void Register_UnityEngine_Shader_SetGlobalTextureMatrixName()
{
    scripting_add_internal_call( "UnityEngine.Shader::SetGlobalTextureMatrixName" , (gpointer)& Shader_CUSTOM_SetGlobalTextureMatrixName );
}

#if !UNITY_FLASH
void Register_UnityEngine_Shader_SetGlobalBuffer()
{
    scripting_add_internal_call( "UnityEngine.Shader::SetGlobalBuffer" , (gpointer)& Shader_CUSTOM_SetGlobalBuffer );
}

#endif
void Register_UnityEngine_Shader_PropertyToID()
{
    scripting_add_internal_call( "UnityEngine.Shader::PropertyToID" , (gpointer)& Shader_CUSTOM_PropertyToID );
}

void Register_UnityEngine_Shader_WarmupAllShaders()
{
    scripting_add_internal_call( "UnityEngine.Shader::WarmupAllShaders" , (gpointer)& Shader_CUSTOM_WarmupAllShaders );
}

void Register_UnityEngine_Texture_get_masterTextureLimit()
{
    scripting_add_internal_call( "UnityEngine.Texture::get_masterTextureLimit" , (gpointer)& Texture_Get_Custom_PropMasterTextureLimit );
}

void Register_UnityEngine_Texture_set_masterTextureLimit()
{
    scripting_add_internal_call( "UnityEngine.Texture::set_masterTextureLimit" , (gpointer)& Texture_Set_Custom_PropMasterTextureLimit );
}

void Register_UnityEngine_Texture_get_anisotropicFiltering()
{
    scripting_add_internal_call( "UnityEngine.Texture::get_anisotropicFiltering" , (gpointer)& Texture_Get_Custom_PropAnisotropicFiltering );
}

void Register_UnityEngine_Texture_set_anisotropicFiltering()
{
    scripting_add_internal_call( "UnityEngine.Texture::set_anisotropicFiltering" , (gpointer)& Texture_Set_Custom_PropAnisotropicFiltering );
}

void Register_UnityEngine_Texture_SetGlobalAnisotropicFilteringLimits()
{
    scripting_add_internal_call( "UnityEngine.Texture::SetGlobalAnisotropicFilteringLimits" , (gpointer)& Texture_CUSTOM_SetGlobalAnisotropicFilteringLimits );
}

void Register_UnityEngine_Texture_Internal_GetWidth()
{
    scripting_add_internal_call( "UnityEngine.Texture::Internal_GetWidth" , (gpointer)& Texture_CUSTOM_Internal_GetWidth );
}

void Register_UnityEngine_Texture_Internal_GetHeight()
{
    scripting_add_internal_call( "UnityEngine.Texture::Internal_GetHeight" , (gpointer)& Texture_CUSTOM_Internal_GetHeight );
}

void Register_UnityEngine_Texture_get_filterMode()
{
    scripting_add_internal_call( "UnityEngine.Texture::get_filterMode" , (gpointer)& Texture_Get_Custom_PropFilterMode );
}

void Register_UnityEngine_Texture_set_filterMode()
{
    scripting_add_internal_call( "UnityEngine.Texture::set_filterMode" , (gpointer)& Texture_Set_Custom_PropFilterMode );
}

void Register_UnityEngine_Texture_get_anisoLevel()
{
    scripting_add_internal_call( "UnityEngine.Texture::get_anisoLevel" , (gpointer)& Texture_Get_Custom_PropAnisoLevel );
}

void Register_UnityEngine_Texture_set_anisoLevel()
{
    scripting_add_internal_call( "UnityEngine.Texture::set_anisoLevel" , (gpointer)& Texture_Set_Custom_PropAnisoLevel );
}

void Register_UnityEngine_Texture_get_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.Texture::get_wrapMode" , (gpointer)& Texture_Get_Custom_PropWrapMode );
}

void Register_UnityEngine_Texture_set_wrapMode()
{
    scripting_add_internal_call( "UnityEngine.Texture::set_wrapMode" , (gpointer)& Texture_Set_Custom_PropWrapMode );
}

void Register_UnityEngine_Texture_get_mipMapBias()
{
    scripting_add_internal_call( "UnityEngine.Texture::get_mipMapBias" , (gpointer)& Texture_Get_Custom_PropMipMapBias );
}

void Register_UnityEngine_Texture_set_mipMapBias()
{
    scripting_add_internal_call( "UnityEngine.Texture::set_mipMapBias" , (gpointer)& Texture_Set_Custom_PropMipMapBias );
}

void Register_UnityEngine_Texture_Internal_GetTexelSize()
{
    scripting_add_internal_call( "UnityEngine.Texture::Internal_GetTexelSize" , (gpointer)& Texture_CUSTOM_Internal_GetTexelSize );
}

void Register_UnityEngine_Texture_INTERNAL_CALL_GetNativeTexturePtr()
{
    scripting_add_internal_call( "UnityEngine.Texture::INTERNAL_CALL_GetNativeTexturePtr" , (gpointer)& Texture_CUSTOM_INTERNAL_CALL_GetNativeTexturePtr );
}

void Register_UnityEngine_Texture_GetNativeTextureID()
{
    scripting_add_internal_call( "UnityEngine.Texture::GetNativeTextureID" , (gpointer)& Texture_CUSTOM_GetNativeTextureID );
}

void Register_UnityEngine_Texture2D_get_mipmapCount()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::get_mipmapCount" , (gpointer)& Texture2D_Get_Custom_PropMipmapCount );
}

void Register_UnityEngine_Texture2D_Internal_Create()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::Internal_Create" , (gpointer)& Texture2D_CUSTOM_Internal_Create );
}

#if ENABLE_TEXTUREID_MAP
void Register_UnityEngine_Texture2D_UpdateExternalTexture()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::UpdateExternalTexture" , (gpointer)& Texture2D_CUSTOM_UpdateExternalTexture );
}

#endif
void Register_UnityEngine_Texture2D_get_format()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::get_format" , (gpointer)& Texture2D_Get_Custom_PropFormat );
}

void Register_UnityEngine_Texture2D_INTERNAL_CALL_SetPixel()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::INTERNAL_CALL_SetPixel" , (gpointer)& Texture2D_CUSTOM_INTERNAL_CALL_SetPixel );
}

void Register_UnityEngine_Texture2D_INTERNAL_CALL_GetPixel()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::INTERNAL_CALL_GetPixel" , (gpointer)& Texture2D_CUSTOM_INTERNAL_CALL_GetPixel );
}

void Register_UnityEngine_Texture2D_INTERNAL_CALL_GetPixelBilinear()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::INTERNAL_CALL_GetPixelBilinear" , (gpointer)& Texture2D_CUSTOM_INTERNAL_CALL_GetPixelBilinear );
}

void Register_UnityEngine_Texture2D_SetPixels()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::SetPixels" , (gpointer)& Texture2D_CUSTOM_SetPixels );
}

void Register_UnityEngine_Texture2D_SetPixels32()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::SetPixels32" , (gpointer)& Texture2D_CUSTOM_SetPixels32 );
}

void Register_UnityEngine_Texture2D_LoadImage()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::LoadImage" , (gpointer)& Texture2D_CUSTOM_LoadImage );
}

void Register_UnityEngine_Texture2D_GetPixels()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::GetPixels" , (gpointer)& Texture2D_CUSTOM_GetPixels );
}

void Register_UnityEngine_Texture2D_GetPixels32()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::GetPixels32" , (gpointer)& Texture2D_CUSTOM_GetPixels32 );
}

void Register_UnityEngine_Texture2D_Apply()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::Apply" , (gpointer)& Texture2D_CUSTOM_Apply );
}

void Register_UnityEngine_Texture2D_Resize()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::Resize" , (gpointer)& Texture2D_CUSTOM_Resize );
}

void Register_UnityEngine_Texture2D_Internal_ResizeWH()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::Internal_ResizeWH" , (gpointer)& Texture2D_CUSTOM_Internal_ResizeWH );
}

void Register_UnityEngine_Texture2D_INTERNAL_CALL_Compress()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::INTERNAL_CALL_Compress" , (gpointer)& Texture2D_CUSTOM_INTERNAL_CALL_Compress );
}

void Register_UnityEngine_Texture2D_PackTextures()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::PackTextures" , (gpointer)& Texture2D_CUSTOM_PackTextures );
}

void Register_UnityEngine_Texture2D_INTERNAL_CALL_ReadPixels()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::INTERNAL_CALL_ReadPixels" , (gpointer)& Texture2D_CUSTOM_INTERNAL_CALL_ReadPixels );
}

void Register_UnityEngine_Texture2D_EncodeToPNG()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::EncodeToPNG" , (gpointer)& Texture2D_CUSTOM_EncodeToPNG );
}

#if UNITY_EDITOR
void Register_UnityEngine_Texture2D_get_alphaIsTransparency()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::get_alphaIsTransparency" , (gpointer)& Texture2D_Get_Custom_PropAlphaIsTransparency );
}

void Register_UnityEngine_Texture2D_set_alphaIsTransparency()
{
    scripting_add_internal_call( "UnityEngine.Texture2D::set_alphaIsTransparency" , (gpointer)& Texture2D_Set_Custom_PropAlphaIsTransparency );
}

#endif
void Register_UnityEngine_Cubemap_INTERNAL_CALL_SetPixel()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::INTERNAL_CALL_SetPixel" , (gpointer)& Cubemap_CUSTOM_INTERNAL_CALL_SetPixel );
}

void Register_UnityEngine_Cubemap_INTERNAL_CALL_GetPixel()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::INTERNAL_CALL_GetPixel" , (gpointer)& Cubemap_CUSTOM_INTERNAL_CALL_GetPixel );
}

void Register_UnityEngine_Cubemap_GetPixels()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::GetPixels" , (gpointer)& Cubemap_CUSTOM_GetPixels );
}

void Register_UnityEngine_Cubemap_SetPixels()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::SetPixels" , (gpointer)& Cubemap_CUSTOM_SetPixels );
}

void Register_UnityEngine_Cubemap_Apply()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::Apply" , (gpointer)& Cubemap_CUSTOM_Apply );
}

void Register_UnityEngine_Cubemap_get_format()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::get_format" , (gpointer)& Cubemap_Get_Custom_PropFormat );
}

void Register_UnityEngine_Cubemap_Internal_Create()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::Internal_Create" , (gpointer)& Cubemap_CUSTOM_Internal_Create );
}

void Register_UnityEngine_Cubemap_SmoothEdges()
{
    scripting_add_internal_call( "UnityEngine.Cubemap::SmoothEdges" , (gpointer)& Cubemap_CUSTOM_SmoothEdges );
}

void Register_UnityEngine_Texture3D_get_depth()
{
    scripting_add_internal_call( "UnityEngine.Texture3D::get_depth" , (gpointer)& Texture3D_Get_Custom_PropDepth );
}

void Register_UnityEngine_Texture3D_GetPixels()
{
    scripting_add_internal_call( "UnityEngine.Texture3D::GetPixels" , (gpointer)& Texture3D_CUSTOM_GetPixels );
}

void Register_UnityEngine_Texture3D_SetPixels()
{
    scripting_add_internal_call( "UnityEngine.Texture3D::SetPixels" , (gpointer)& Texture3D_CUSTOM_SetPixels );
}

void Register_UnityEngine_Texture3D_Apply()
{
    scripting_add_internal_call( "UnityEngine.Texture3D::Apply" , (gpointer)& Texture3D_CUSTOM_Apply );
}

void Register_UnityEngine_Texture3D_get_format()
{
    scripting_add_internal_call( "UnityEngine.Texture3D::get_format" , (gpointer)& Texture3D_Get_Custom_PropFormat );
}

void Register_UnityEngine_Texture3D_Internal_Create()
{
    scripting_add_internal_call( "UnityEngine.Texture3D::Internal_Create" , (gpointer)& Texture3D_CUSTOM_Internal_Create );
}

void Register_UnityEngine_MeshFilter_get_mesh()
{
    scripting_add_internal_call( "UnityEngine.MeshFilter::get_mesh" , (gpointer)& MeshFilter_Get_Custom_PropMesh );
}

void Register_UnityEngine_MeshFilter_set_mesh()
{
    scripting_add_internal_call( "UnityEngine.MeshFilter::set_mesh" , (gpointer)& MeshFilter_Set_Custom_PropMesh );
}

void Register_UnityEngine_MeshFilter_get_sharedMesh()
{
    scripting_add_internal_call( "UnityEngine.MeshFilter::get_sharedMesh" , (gpointer)& MeshFilter_Get_Custom_PropSharedMesh );
}

void Register_UnityEngine_MeshFilter_set_sharedMesh()
{
    scripting_add_internal_call( "UnityEngine.MeshFilter::set_sharedMesh" , (gpointer)& MeshFilter_Set_Custom_PropSharedMesh );
}

void Register_UnityEngine_CombineInstance_InternalGetMesh()
{
    scripting_add_internal_call( "UnityEngine.CombineInstance::InternalGetMesh" , (gpointer)& CombineInstance_CUSTOM_InternalGetMesh );
}

void Register_UnityEngine_Mesh_Internal_Create()
{
    scripting_add_internal_call( "UnityEngine.Mesh::Internal_Create" , (gpointer)& Mesh_CUSTOM_Internal_Create );
}

void Register_UnityEngine_Mesh_Clear()
{
    scripting_add_internal_call( "UnityEngine.Mesh::Clear" , (gpointer)& Mesh_CUSTOM_Clear );
}

void Register_UnityEngine_Mesh_get_isReadable()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_isReadable" , (gpointer)& Mesh_Get_Custom_PropIsReadable );
}

void Register_UnityEngine_Mesh_get_canAccess()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_canAccess" , (gpointer)& Mesh_Get_Custom_PropCanAccess );
}

void Register_UnityEngine_Mesh_get_vertices()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_vertices" , (gpointer)& Mesh_Get_Custom_PropVertices );
}

void Register_UnityEngine_Mesh_set_vertices()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_vertices" , (gpointer)& Mesh_Set_Custom_PropVertices );
}

void Register_UnityEngine_Mesh_get_normals()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_normals" , (gpointer)& Mesh_Get_Custom_PropNormals );
}

void Register_UnityEngine_Mesh_set_normals()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_normals" , (gpointer)& Mesh_Set_Custom_PropNormals );
}

void Register_UnityEngine_Mesh_get_tangents()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_tangents" , (gpointer)& Mesh_Get_Custom_PropTangents );
}

void Register_UnityEngine_Mesh_set_tangents()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_tangents" , (gpointer)& Mesh_Set_Custom_PropTangents );
}

void Register_UnityEngine_Mesh_get_uv()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_uv" , (gpointer)& Mesh_Get_Custom_PropUv );
}

void Register_UnityEngine_Mesh_set_uv()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_uv" , (gpointer)& Mesh_Set_Custom_PropUv );
}

void Register_UnityEngine_Mesh_get_uv2()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_uv2" , (gpointer)& Mesh_Get_Custom_PropUv2 );
}

void Register_UnityEngine_Mesh_set_uv2()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_uv2" , (gpointer)& Mesh_Set_Custom_PropUv2 );
}

void Register_UnityEngine_Mesh_INTERNAL_get_bounds()
{
    scripting_add_internal_call( "UnityEngine.Mesh::INTERNAL_get_bounds" , (gpointer)& Mesh_CUSTOM_INTERNAL_get_bounds );
}

void Register_UnityEngine_Mesh_INTERNAL_set_bounds()
{
    scripting_add_internal_call( "UnityEngine.Mesh::INTERNAL_set_bounds" , (gpointer)& Mesh_CUSTOM_INTERNAL_set_bounds );
}

void Register_UnityEngine_Mesh_get_colors()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_colors" , (gpointer)& Mesh_Get_Custom_PropColors );
}

void Register_UnityEngine_Mesh_set_colors()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_colors" , (gpointer)& Mesh_Set_Custom_PropColors );
}

void Register_UnityEngine_Mesh_get_colors32()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_colors32" , (gpointer)& Mesh_Get_Custom_PropColors32 );
}

void Register_UnityEngine_Mesh_set_colors32()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_colors32" , (gpointer)& Mesh_Set_Custom_PropColors32 );
}

void Register_UnityEngine_Mesh_RecalculateBounds()
{
    scripting_add_internal_call( "UnityEngine.Mesh::RecalculateBounds" , (gpointer)& Mesh_CUSTOM_RecalculateBounds );
}

void Register_UnityEngine_Mesh_RecalculateNormals()
{
    scripting_add_internal_call( "UnityEngine.Mesh::RecalculateNormals" , (gpointer)& Mesh_CUSTOM_RecalculateNormals );
}

void Register_UnityEngine_Mesh_Optimize()
{
    scripting_add_internal_call( "UnityEngine.Mesh::Optimize" , (gpointer)& Mesh_CUSTOM_Optimize );
}

void Register_UnityEngine_Mesh_get_triangles()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_triangles" , (gpointer)& Mesh_Get_Custom_PropTriangles );
}

void Register_UnityEngine_Mesh_set_triangles()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_triangles" , (gpointer)& Mesh_Set_Custom_PropTriangles );
}

void Register_UnityEngine_Mesh_GetTriangles()
{
    scripting_add_internal_call( "UnityEngine.Mesh::GetTriangles" , (gpointer)& Mesh_CUSTOM_GetTriangles );
}

void Register_UnityEngine_Mesh_SetTriangles()
{
    scripting_add_internal_call( "UnityEngine.Mesh::SetTriangles" , (gpointer)& Mesh_CUSTOM_SetTriangles );
}

void Register_UnityEngine_Mesh_GetIndices()
{
    scripting_add_internal_call( "UnityEngine.Mesh::GetIndices" , (gpointer)& Mesh_CUSTOM_GetIndices );
}

void Register_UnityEngine_Mesh_SetIndices()
{
    scripting_add_internal_call( "UnityEngine.Mesh::SetIndices" , (gpointer)& Mesh_CUSTOM_SetIndices );
}

void Register_UnityEngine_Mesh_GetTopology()
{
    scripting_add_internal_call( "UnityEngine.Mesh::GetTopology" , (gpointer)& Mesh_CUSTOM_GetTopology );
}

void Register_UnityEngine_Mesh_get_vertexCount()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_vertexCount" , (gpointer)& Mesh_Get_Custom_PropVertexCount );
}

void Register_UnityEngine_Mesh_get_subMeshCount()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_subMeshCount" , (gpointer)& Mesh_Get_Custom_PropSubMeshCount );
}

void Register_UnityEngine_Mesh_set_subMeshCount()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_subMeshCount" , (gpointer)& Mesh_Set_Custom_PropSubMeshCount );
}

void Register_UnityEngine_Mesh_SetTriangleStrip()
{
    scripting_add_internal_call( "UnityEngine.Mesh::SetTriangleStrip" , (gpointer)& Mesh_CUSTOM_SetTriangleStrip );
}

void Register_UnityEngine_Mesh_GetTriangleStrip()
{
    scripting_add_internal_call( "UnityEngine.Mesh::GetTriangleStrip" , (gpointer)& Mesh_CUSTOM_GetTriangleStrip );
}

void Register_UnityEngine_Mesh_CombineMeshes()
{
    scripting_add_internal_call( "UnityEngine.Mesh::CombineMeshes" , (gpointer)& Mesh_CUSTOM_CombineMeshes );
}

void Register_UnityEngine_Mesh_get_boneWeights()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_boneWeights" , (gpointer)& Mesh_Get_Custom_PropBoneWeights );
}

void Register_UnityEngine_Mesh_set_boneWeights()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_boneWeights" , (gpointer)& Mesh_Set_Custom_PropBoneWeights );
}

void Register_UnityEngine_Mesh_get_bindposes()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_bindposes" , (gpointer)& Mesh_Get_Custom_PropBindposes );
}

void Register_UnityEngine_Mesh_set_bindposes()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_bindposes" , (gpointer)& Mesh_Set_Custom_PropBindposes );
}

void Register_UnityEngine_Mesh_MarkDynamic()
{
    scripting_add_internal_call( "UnityEngine.Mesh::MarkDynamic" , (gpointer)& Mesh_CUSTOM_MarkDynamic );
}

#if USE_BLENDSHAPES
void Register_UnityEngine_Mesh_get_blendShapes()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_blendShapes" , (gpointer)& Mesh_Get_Custom_PropBlendShapes );
}

void Register_UnityEngine_Mesh_set_blendShapes()
{
    scripting_add_internal_call( "UnityEngine.Mesh::set_blendShapes" , (gpointer)& Mesh_Set_Custom_PropBlendShapes );
}

void Register_UnityEngine_Mesh_get_blendShapeCount()
{
    scripting_add_internal_call( "UnityEngine.Mesh::get_blendShapeCount" , (gpointer)& Mesh_Get_Custom_PropBlendShapeCount );
}

void Register_UnityEngine_Mesh_GetBlendShapeName()
{
    scripting_add_internal_call( "UnityEngine.Mesh::GetBlendShapeName" , (gpointer)& Mesh_CUSTOM_GetBlendShapeName );
}

#endif
void Register_UnityEngine_SkinnedMeshRenderer_get_bones()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::get_bones" , (gpointer)& SkinnedMeshRenderer_Get_Custom_PropBones );
}

void Register_UnityEngine_SkinnedMeshRenderer_set_bones()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::set_bones" , (gpointer)& SkinnedMeshRenderer_Set_Custom_PropBones );
}

void Register_UnityEngine_SkinnedMeshRenderer_get_rootBone()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::get_rootBone" , (gpointer)& SkinnedMeshRenderer_Get_Custom_PropRootBone );
}

void Register_UnityEngine_SkinnedMeshRenderer_set_rootBone()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::set_rootBone" , (gpointer)& SkinnedMeshRenderer_Set_Custom_PropRootBone );
}

void Register_UnityEngine_SkinnedMeshRenderer_get_quality()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::get_quality" , (gpointer)& SkinnedMeshRenderer_Get_Custom_PropQuality );
}

void Register_UnityEngine_SkinnedMeshRenderer_set_quality()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::set_quality" , (gpointer)& SkinnedMeshRenderer_Set_Custom_PropQuality );
}

void Register_UnityEngine_SkinnedMeshRenderer_get_sharedMesh()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::get_sharedMesh" , (gpointer)& SkinnedMeshRenderer_Get_Custom_PropSharedMesh );
}

void Register_UnityEngine_SkinnedMeshRenderer_set_sharedMesh()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::set_sharedMesh" , (gpointer)& SkinnedMeshRenderer_Set_Custom_PropSharedMesh );
}

void Register_UnityEngine_SkinnedMeshRenderer_get_updateWhenOffscreen()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::get_updateWhenOffscreen" , (gpointer)& SkinnedMeshRenderer_Get_Custom_PropUpdateWhenOffscreen );
}

void Register_UnityEngine_SkinnedMeshRenderer_set_updateWhenOffscreen()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::set_updateWhenOffscreen" , (gpointer)& SkinnedMeshRenderer_Set_Custom_PropUpdateWhenOffscreen );
}

void Register_UnityEngine_SkinnedMeshRenderer_INTERNAL_get_localBounds()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::INTERNAL_get_localBounds" , (gpointer)& SkinnedMeshRenderer_CUSTOM_INTERNAL_get_localBounds );
}

void Register_UnityEngine_SkinnedMeshRenderer_INTERNAL_set_localBounds()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::INTERNAL_set_localBounds" , (gpointer)& SkinnedMeshRenderer_CUSTOM_INTERNAL_set_localBounds );
}

void Register_UnityEngine_SkinnedMeshRenderer_BakeMesh()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::BakeMesh" , (gpointer)& SkinnedMeshRenderer_CUSTOM_BakeMesh );
}

#if UNITY_EDITOR
void Register_UnityEngine_SkinnedMeshRenderer_get_actualRootBone()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::get_actualRootBone" , (gpointer)& SkinnedMeshRenderer_Get_Custom_PropActualRootBone );
}

#endif
#if USE_BLENDSHAPES
void Register_UnityEngine_SkinnedMeshRenderer_GetBlendShapeWeight()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::GetBlendShapeWeight" , (gpointer)& SkinnedMeshRenderer_CUSTOM_GetBlendShapeWeight );
}

void Register_UnityEngine_SkinnedMeshRenderer_SetBlendShapeWeight()
{
    scripting_add_internal_call( "UnityEngine.SkinnedMeshRenderer::SetBlendShapeWeight" , (gpointer)& SkinnedMeshRenderer_CUSTOM_SetBlendShapeWeight );
}

#endif
void Register_UnityEngine_Material_get_shader()
{
    scripting_add_internal_call( "UnityEngine.Material::get_shader" , (gpointer)& Material_Get_Custom_PropShader );
}

void Register_UnityEngine_Material_set_shader()
{
    scripting_add_internal_call( "UnityEngine.Material::set_shader" , (gpointer)& Material_Set_Custom_PropShader );
}

void Register_UnityEngine_Material_INTERNAL_CALL_SetColor()
{
    scripting_add_internal_call( "UnityEngine.Material::INTERNAL_CALL_SetColor" , (gpointer)& Material_CUSTOM_INTERNAL_CALL_SetColor );
}

void Register_UnityEngine_Material_INTERNAL_CALL_GetColor()
{
    scripting_add_internal_call( "UnityEngine.Material::INTERNAL_CALL_GetColor" , (gpointer)& Material_CUSTOM_INTERNAL_CALL_GetColor );
}

void Register_UnityEngine_Material_SetTexture()
{
    scripting_add_internal_call( "UnityEngine.Material::SetTexture" , (gpointer)& Material_CUSTOM_SetTexture );
}

void Register_UnityEngine_Material_GetTexture()
{
    scripting_add_internal_call( "UnityEngine.Material::GetTexture" , (gpointer)& Material_CUSTOM_GetTexture );
}

void Register_UnityEngine_Material_Internal_GetTextureOffset()
{
    scripting_add_internal_call( "UnityEngine.Material::Internal_GetTextureOffset" , (gpointer)& Material_CUSTOM_Internal_GetTextureOffset );
}

void Register_UnityEngine_Material_Internal_GetTextureScale()
{
    scripting_add_internal_call( "UnityEngine.Material::Internal_GetTextureScale" , (gpointer)& Material_CUSTOM_Internal_GetTextureScale );
}

void Register_UnityEngine_Material_INTERNAL_CALL_SetTextureOffset()
{
    scripting_add_internal_call( "UnityEngine.Material::INTERNAL_CALL_SetTextureOffset" , (gpointer)& Material_CUSTOM_INTERNAL_CALL_SetTextureOffset );
}

void Register_UnityEngine_Material_INTERNAL_CALL_SetTextureScale()
{
    scripting_add_internal_call( "UnityEngine.Material::INTERNAL_CALL_SetTextureScale" , (gpointer)& Material_CUSTOM_INTERNAL_CALL_SetTextureScale );
}

void Register_UnityEngine_Material_INTERNAL_CALL_SetMatrix()
{
    scripting_add_internal_call( "UnityEngine.Material::INTERNAL_CALL_SetMatrix" , (gpointer)& Material_CUSTOM_INTERNAL_CALL_SetMatrix );
}

void Register_UnityEngine_Material_INTERNAL_CALL_GetMatrix()
{
    scripting_add_internal_call( "UnityEngine.Material::INTERNAL_CALL_GetMatrix" , (gpointer)& Material_CUSTOM_INTERNAL_CALL_GetMatrix );
}

void Register_UnityEngine_Material_SetFloat()
{
    scripting_add_internal_call( "UnityEngine.Material::SetFloat" , (gpointer)& Material_CUSTOM_SetFloat );
}

void Register_UnityEngine_Material_GetFloat()
{
    scripting_add_internal_call( "UnityEngine.Material::GetFloat" , (gpointer)& Material_CUSTOM_GetFloat );
}

#if !UNITY_FLASH
void Register_UnityEngine_Material_SetBuffer()
{
    scripting_add_internal_call( "UnityEngine.Material::SetBuffer" , (gpointer)& Material_CUSTOM_SetBuffer );
}

#endif
void Register_UnityEngine_Material_HasProperty()
{
    scripting_add_internal_call( "UnityEngine.Material::HasProperty" , (gpointer)& Material_CUSTOM_HasProperty );
}

void Register_UnityEngine_Material_GetTag()
{
    scripting_add_internal_call( "UnityEngine.Material::GetTag" , (gpointer)& Material_CUSTOM_GetTag );
}

void Register_UnityEngine_Material_Lerp()
{
    scripting_add_internal_call( "UnityEngine.Material::Lerp" , (gpointer)& Material_CUSTOM_Lerp );
}

void Register_UnityEngine_Material_get_passCount()
{
    scripting_add_internal_call( "UnityEngine.Material::get_passCount" , (gpointer)& Material_Get_Custom_PropPassCount );
}

void Register_UnityEngine_Material_SetPass()
{
    scripting_add_internal_call( "UnityEngine.Material::SetPass" , (gpointer)& Material_CUSTOM_SetPass );
}

void Register_UnityEngine_Material_get_renderQueue()
{
    scripting_add_internal_call( "UnityEngine.Material::get_renderQueue" , (gpointer)& Material_Get_Custom_PropRenderQueue );
}

void Register_UnityEngine_Material_set_renderQueue()
{
    scripting_add_internal_call( "UnityEngine.Material::set_renderQueue" , (gpointer)& Material_Set_Custom_PropRenderQueue );
}

void Register_UnityEngine_Material_Internal_CreateWithString()
{
    scripting_add_internal_call( "UnityEngine.Material::Internal_CreateWithString" , (gpointer)& Material_CUSTOM_Internal_CreateWithString );
}

void Register_UnityEngine_Material_Internal_CreateWithShader()
{
    scripting_add_internal_call( "UnityEngine.Material::Internal_CreateWithShader" , (gpointer)& Material_CUSTOM_Internal_CreateWithShader );
}

void Register_UnityEngine_Material_Internal_CreateWithMaterial()
{
    scripting_add_internal_call( "UnityEngine.Material::Internal_CreateWithMaterial" , (gpointer)& Material_CUSTOM_Internal_CreateWithMaterial );
}

void Register_UnityEngine_Material_CopyPropertiesFromMaterial()
{
    scripting_add_internal_call( "UnityEngine.Material::CopyPropertiesFromMaterial" , (gpointer)& Material_CUSTOM_CopyPropertiesFromMaterial );
}

#if ENABLE_MONO || PLATFORM_WINRT
void Register_UnityEngine_Material_get_shaderKeywords()
{
    scripting_add_internal_call( "UnityEngine.Material::get_shaderKeywords" , (gpointer)& Material_Get_Custom_PropShaderKeywords );
}

void Register_UnityEngine_Material_set_shaderKeywords()
{
    scripting_add_internal_call( "UnityEngine.Material::set_shaderKeywords" , (gpointer)& Material_Set_Custom_PropShaderKeywords );
}

#endif
void Register_UnityEngine_LensFlare_get_flare()
{
    scripting_add_internal_call( "UnityEngine.LensFlare::get_flare" , (gpointer)& LensFlare_Get_Custom_PropFlare );
}

void Register_UnityEngine_LensFlare_set_flare()
{
    scripting_add_internal_call( "UnityEngine.LensFlare::set_flare" , (gpointer)& LensFlare_Set_Custom_PropFlare );
}

void Register_UnityEngine_LensFlare_get_brightness()
{
    scripting_add_internal_call( "UnityEngine.LensFlare::get_brightness" , (gpointer)& LensFlare_Get_Custom_PropBrightness );
}

void Register_UnityEngine_LensFlare_set_brightness()
{
    scripting_add_internal_call( "UnityEngine.LensFlare::set_brightness" , (gpointer)& LensFlare_Set_Custom_PropBrightness );
}

void Register_UnityEngine_LensFlare_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.LensFlare::INTERNAL_get_color" , (gpointer)& LensFlare_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_LensFlare_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.LensFlare::INTERNAL_set_color" , (gpointer)& LensFlare_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_Renderer_get_staticBatchRootTransform()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_staticBatchRootTransform" , (gpointer)& Renderer_Get_Custom_PropStaticBatchRootTransform );
}

void Register_UnityEngine_Renderer_set_staticBatchRootTransform()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_staticBatchRootTransform" , (gpointer)& Renderer_Set_Custom_PropStaticBatchRootTransform );
}

void Register_UnityEngine_Renderer_get_staticBatchIndex()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_staticBatchIndex" , (gpointer)& Renderer_Get_Custom_PropStaticBatchIndex );
}

void Register_UnityEngine_Renderer_SetSubsetIndex()
{
    scripting_add_internal_call( "UnityEngine.Renderer::SetSubsetIndex" , (gpointer)& Renderer_CUSTOM_SetSubsetIndex );
}

void Register_UnityEngine_Renderer_get_isPartOfStaticBatch()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_isPartOfStaticBatch" , (gpointer)& Renderer_Get_Custom_PropIsPartOfStaticBatch );
}

void Register_UnityEngine_Renderer_INTERNAL_get_worldToLocalMatrix()
{
    scripting_add_internal_call( "UnityEngine.Renderer::INTERNAL_get_worldToLocalMatrix" , (gpointer)& Renderer_CUSTOM_INTERNAL_get_worldToLocalMatrix );
}

void Register_UnityEngine_Renderer_INTERNAL_get_localToWorldMatrix()
{
    scripting_add_internal_call( "UnityEngine.Renderer::INTERNAL_get_localToWorldMatrix" , (gpointer)& Renderer_CUSTOM_INTERNAL_get_localToWorldMatrix );
}

void Register_UnityEngine_Renderer_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_enabled" , (gpointer)& Renderer_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Renderer_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_enabled" , (gpointer)& Renderer_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Renderer_get_castShadows()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_castShadows" , (gpointer)& Renderer_Get_Custom_PropCastShadows );
}

void Register_UnityEngine_Renderer_set_castShadows()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_castShadows" , (gpointer)& Renderer_Set_Custom_PropCastShadows );
}

void Register_UnityEngine_Renderer_get_receiveShadows()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_receiveShadows" , (gpointer)& Renderer_Get_Custom_PropReceiveShadows );
}

void Register_UnityEngine_Renderer_set_receiveShadows()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_receiveShadows" , (gpointer)& Renderer_Set_Custom_PropReceiveShadows );
}

void Register_UnityEngine_Renderer_get_material()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_material" , (gpointer)& Renderer_Get_Custom_PropMaterial );
}

void Register_UnityEngine_Renderer_set_material()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_material" , (gpointer)& Renderer_Set_Custom_PropMaterial );
}

void Register_UnityEngine_Renderer_get_sharedMaterial()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_sharedMaterial" , (gpointer)& Renderer_Get_Custom_PropSharedMaterial );
}

void Register_UnityEngine_Renderer_set_sharedMaterial()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_sharedMaterial" , (gpointer)& Renderer_Set_Custom_PropSharedMaterial );
}

void Register_UnityEngine_Renderer_get_sharedMaterials()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_sharedMaterials" , (gpointer)& Renderer_Get_Custom_PropSharedMaterials );
}

void Register_UnityEngine_Renderer_set_sharedMaterials()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_sharedMaterials" , (gpointer)& Renderer_Set_Custom_PropSharedMaterials );
}

void Register_UnityEngine_Renderer_get_materials()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_materials" , (gpointer)& Renderer_Get_Custom_PropMaterials );
}

void Register_UnityEngine_Renderer_set_materials()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_materials" , (gpointer)& Renderer_Set_Custom_PropMaterials );
}

void Register_UnityEngine_Renderer_INTERNAL_get_bounds()
{
    scripting_add_internal_call( "UnityEngine.Renderer::INTERNAL_get_bounds" , (gpointer)& Renderer_CUSTOM_INTERNAL_get_bounds );
}

void Register_UnityEngine_Renderer_get_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_lightmapIndex" , (gpointer)& Renderer_Get_Custom_PropLightmapIndex );
}

void Register_UnityEngine_Renderer_set_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_lightmapIndex" , (gpointer)& Renderer_Set_Custom_PropLightmapIndex );
}

void Register_UnityEngine_Renderer_INTERNAL_get_lightmapTilingOffset()
{
    scripting_add_internal_call( "UnityEngine.Renderer::INTERNAL_get_lightmapTilingOffset" , (gpointer)& Renderer_CUSTOM_INTERNAL_get_lightmapTilingOffset );
}

void Register_UnityEngine_Renderer_INTERNAL_set_lightmapTilingOffset()
{
    scripting_add_internal_call( "UnityEngine.Renderer::INTERNAL_set_lightmapTilingOffset" , (gpointer)& Renderer_CUSTOM_INTERNAL_set_lightmapTilingOffset );
}

void Register_UnityEngine_Renderer_get_isVisible()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_isVisible" , (gpointer)& Renderer_Get_Custom_PropIsVisible );
}

void Register_UnityEngine_Renderer_get_useLightProbes()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_useLightProbes" , (gpointer)& Renderer_Get_Custom_PropUseLightProbes );
}

void Register_UnityEngine_Renderer_set_useLightProbes()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_useLightProbes" , (gpointer)& Renderer_Set_Custom_PropUseLightProbes );
}

void Register_UnityEngine_Renderer_get_lightProbeAnchor()
{
    scripting_add_internal_call( "UnityEngine.Renderer::get_lightProbeAnchor" , (gpointer)& Renderer_Get_Custom_PropLightProbeAnchor );
}

void Register_UnityEngine_Renderer_set_lightProbeAnchor()
{
    scripting_add_internal_call( "UnityEngine.Renderer::set_lightProbeAnchor" , (gpointer)& Renderer_Set_Custom_PropLightProbeAnchor );
}

void Register_UnityEngine_Renderer_SetPropertyBlock()
{
    scripting_add_internal_call( "UnityEngine.Renderer::SetPropertyBlock" , (gpointer)& Renderer_CUSTOM_SetPropertyBlock );
}

void Register_UnityEngine_Renderer_Render()
{
    scripting_add_internal_call( "UnityEngine.Renderer::Render" , (gpointer)& Renderer_CUSTOM_Render );
}

void Register_UnityEngine_Projector_get_nearClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_nearClipPlane" , (gpointer)& Projector_Get_Custom_PropNearClipPlane );
}

void Register_UnityEngine_Projector_set_nearClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_nearClipPlane" , (gpointer)& Projector_Set_Custom_PropNearClipPlane );
}

void Register_UnityEngine_Projector_get_farClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_farClipPlane" , (gpointer)& Projector_Get_Custom_PropFarClipPlane );
}

void Register_UnityEngine_Projector_set_farClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_farClipPlane" , (gpointer)& Projector_Set_Custom_PropFarClipPlane );
}

void Register_UnityEngine_Projector_get_fieldOfView()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_fieldOfView" , (gpointer)& Projector_Get_Custom_PropFieldOfView );
}

void Register_UnityEngine_Projector_set_fieldOfView()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_fieldOfView" , (gpointer)& Projector_Set_Custom_PropFieldOfView );
}

void Register_UnityEngine_Projector_get_aspectRatio()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_aspectRatio" , (gpointer)& Projector_Get_Custom_PropAspectRatio );
}

void Register_UnityEngine_Projector_set_aspectRatio()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_aspectRatio" , (gpointer)& Projector_Set_Custom_PropAspectRatio );
}

void Register_UnityEngine_Projector_get_orthographic()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_orthographic" , (gpointer)& Projector_Get_Custom_PropOrthographic );
}

void Register_UnityEngine_Projector_set_orthographic()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_orthographic" , (gpointer)& Projector_Set_Custom_PropOrthographic );
}

void Register_UnityEngine_Projector_get_orthographicSize()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_orthographicSize" , (gpointer)& Projector_Get_Custom_PropOrthographicSize );
}

void Register_UnityEngine_Projector_set_orthographicSize()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_orthographicSize" , (gpointer)& Projector_Set_Custom_PropOrthographicSize );
}

void Register_UnityEngine_Projector_get_ignoreLayers()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_ignoreLayers" , (gpointer)& Projector_Get_Custom_PropIgnoreLayers );
}

void Register_UnityEngine_Projector_set_ignoreLayers()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_ignoreLayers" , (gpointer)& Projector_Set_Custom_PropIgnoreLayers );
}

void Register_UnityEngine_Projector_get_material()
{
    scripting_add_internal_call( "UnityEngine.Projector::get_material" , (gpointer)& Projector_Get_Custom_PropMaterial );
}

void Register_UnityEngine_Projector_set_material()
{
    scripting_add_internal_call( "UnityEngine.Projector::set_material" , (gpointer)& Projector_Set_Custom_PropMaterial );
}

void Register_UnityEngine_Skybox_get_material()
{
    scripting_add_internal_call( "UnityEngine.Skybox::get_material" , (gpointer)& Skybox_Get_Custom_PropMaterial );
}

void Register_UnityEngine_Skybox_set_material()
{
    scripting_add_internal_call( "UnityEngine.Skybox::set_material" , (gpointer)& Skybox_Set_Custom_PropMaterial );
}

void Register_UnityEngine_TextMesh_get_text()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_text" , (gpointer)& TextMesh_Get_Custom_PropText );
}

void Register_UnityEngine_TextMesh_set_text()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_text" , (gpointer)& TextMesh_Set_Custom_PropText );
}

void Register_UnityEngine_TextMesh_get_font()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_font" , (gpointer)& TextMesh_Get_Custom_PropFont );
}

void Register_UnityEngine_TextMesh_set_font()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_font" , (gpointer)& TextMesh_Set_Custom_PropFont );
}

void Register_UnityEngine_TextMesh_get_fontSize()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_fontSize" , (gpointer)& TextMesh_Get_Custom_PropFontSize );
}

void Register_UnityEngine_TextMesh_set_fontSize()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_fontSize" , (gpointer)& TextMesh_Set_Custom_PropFontSize );
}

void Register_UnityEngine_TextMesh_get_fontStyle()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_fontStyle" , (gpointer)& TextMesh_Get_Custom_PropFontStyle );
}

void Register_UnityEngine_TextMesh_set_fontStyle()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_fontStyle" , (gpointer)& TextMesh_Set_Custom_PropFontStyle );
}

void Register_UnityEngine_TextMesh_get_offsetZ()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_offsetZ" , (gpointer)& TextMesh_Get_Custom_PropOffsetZ );
}

void Register_UnityEngine_TextMesh_set_offsetZ()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_offsetZ" , (gpointer)& TextMesh_Set_Custom_PropOffsetZ );
}

void Register_UnityEngine_TextMesh_get_alignment()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_alignment" , (gpointer)& TextMesh_Get_Custom_PropAlignment );
}

void Register_UnityEngine_TextMesh_set_alignment()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_alignment" , (gpointer)& TextMesh_Set_Custom_PropAlignment );
}

void Register_UnityEngine_TextMesh_get_anchor()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_anchor" , (gpointer)& TextMesh_Get_Custom_PropAnchor );
}

void Register_UnityEngine_TextMesh_set_anchor()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_anchor" , (gpointer)& TextMesh_Set_Custom_PropAnchor );
}

void Register_UnityEngine_TextMesh_get_characterSize()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_characterSize" , (gpointer)& TextMesh_Get_Custom_PropCharacterSize );
}

void Register_UnityEngine_TextMesh_set_characterSize()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_characterSize" , (gpointer)& TextMesh_Set_Custom_PropCharacterSize );
}

void Register_UnityEngine_TextMesh_get_lineSpacing()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_lineSpacing" , (gpointer)& TextMesh_Get_Custom_PropLineSpacing );
}

void Register_UnityEngine_TextMesh_set_lineSpacing()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_lineSpacing" , (gpointer)& TextMesh_Set_Custom_PropLineSpacing );
}

void Register_UnityEngine_TextMesh_get_tabSize()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_tabSize" , (gpointer)& TextMesh_Get_Custom_PropTabSize );
}

void Register_UnityEngine_TextMesh_set_tabSize()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_tabSize" , (gpointer)& TextMesh_Set_Custom_PropTabSize );
}

void Register_UnityEngine_TextMesh_get_richText()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::get_richText" , (gpointer)& TextMesh_Get_Custom_PropRichText );
}

void Register_UnityEngine_TextMesh_set_richText()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::set_richText" , (gpointer)& TextMesh_Set_Custom_PropRichText );
}

void Register_UnityEngine_TextMesh_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::INTERNAL_get_color" , (gpointer)& TextMesh_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_TextMesh_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.TextMesh::INTERNAL_set_color" , (gpointer)& TextMesh_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_TrailRenderer_get_time()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::get_time" , (gpointer)& TrailRenderer_Get_Custom_PropTime );
}

void Register_UnityEngine_TrailRenderer_set_time()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::set_time" , (gpointer)& TrailRenderer_Set_Custom_PropTime );
}

void Register_UnityEngine_TrailRenderer_get_startWidth()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::get_startWidth" , (gpointer)& TrailRenderer_Get_Custom_PropStartWidth );
}

void Register_UnityEngine_TrailRenderer_set_startWidth()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::set_startWidth" , (gpointer)& TrailRenderer_Set_Custom_PropStartWidth );
}

void Register_UnityEngine_TrailRenderer_get_endWidth()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::get_endWidth" , (gpointer)& TrailRenderer_Get_Custom_PropEndWidth );
}

void Register_UnityEngine_TrailRenderer_set_endWidth()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::set_endWidth" , (gpointer)& TrailRenderer_Set_Custom_PropEndWidth );
}

void Register_UnityEngine_TrailRenderer_get_autodestruct()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::get_autodestruct" , (gpointer)& TrailRenderer_Get_Custom_PropAutodestruct );
}

void Register_UnityEngine_TrailRenderer_set_autodestruct()
{
    scripting_add_internal_call( "UnityEngine.TrailRenderer::set_autodestruct" , (gpointer)& TrailRenderer_Set_Custom_PropAutodestruct );
}

void Register_UnityEngine_LineRenderer_INTERNAL_CALL_SetWidth()
{
    scripting_add_internal_call( "UnityEngine.LineRenderer::INTERNAL_CALL_SetWidth" , (gpointer)& LineRenderer_CUSTOM_INTERNAL_CALL_SetWidth );
}

void Register_UnityEngine_LineRenderer_INTERNAL_CALL_SetColors()
{
    scripting_add_internal_call( "UnityEngine.LineRenderer::INTERNAL_CALL_SetColors" , (gpointer)& LineRenderer_CUSTOM_INTERNAL_CALL_SetColors );
}

void Register_UnityEngine_LineRenderer_INTERNAL_CALL_SetVertexCount()
{
    scripting_add_internal_call( "UnityEngine.LineRenderer::INTERNAL_CALL_SetVertexCount" , (gpointer)& LineRenderer_CUSTOM_INTERNAL_CALL_SetVertexCount );
}

void Register_UnityEngine_LineRenderer_INTERNAL_CALL_SetPosition()
{
    scripting_add_internal_call( "UnityEngine.LineRenderer::INTERNAL_CALL_SetPosition" , (gpointer)& LineRenderer_CUSTOM_INTERNAL_CALL_SetPosition );
}

void Register_UnityEngine_LineRenderer_get_useWorldSpace()
{
    scripting_add_internal_call( "UnityEngine.LineRenderer::get_useWorldSpace" , (gpointer)& LineRenderer_Get_Custom_PropUseWorldSpace );
}

void Register_UnityEngine_LineRenderer_set_useWorldSpace()
{
    scripting_add_internal_call( "UnityEngine.LineRenderer::set_useWorldSpace" , (gpointer)& LineRenderer_Set_Custom_PropUseWorldSpace );
}

void Register_UnityEngine_MaterialPropertyBlock_InitBlock()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::InitBlock" , (gpointer)& MaterialPropertyBlock_CUSTOM_InitBlock );
}

void Register_UnityEngine_MaterialPropertyBlock_DestroyBlock()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::DestroyBlock" , (gpointer)& MaterialPropertyBlock_CUSTOM_DestroyBlock );
}

void Register_UnityEngine_MaterialPropertyBlock_AddFloat()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::AddFloat" , (gpointer)& MaterialPropertyBlock_CUSTOM_AddFloat );
}

void Register_UnityEngine_MaterialPropertyBlock_INTERNAL_CALL_AddVector()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddVector" , (gpointer)& MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddVector );
}

void Register_UnityEngine_MaterialPropertyBlock_INTERNAL_CALL_AddColor()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddColor" , (gpointer)& MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddColor );
}

void Register_UnityEngine_MaterialPropertyBlock_INTERNAL_CALL_AddMatrix()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddMatrix" , (gpointer)& MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddMatrix );
}

void Register_UnityEngine_MaterialPropertyBlock_Clear()
{
    scripting_add_internal_call( "UnityEngine.MaterialPropertyBlock::Clear" , (gpointer)& MaterialPropertyBlock_CUSTOM_Clear );
}

void Register_UnityEngine_Graphics_Internal_DrawMeshTR()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_DrawMeshTR" , (gpointer)& Graphics_CUSTOM_Internal_DrawMeshTR );
}

void Register_UnityEngine_Graphics_Internal_DrawMeshMatrix()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_DrawMeshMatrix" , (gpointer)& Graphics_CUSTOM_Internal_DrawMeshMatrix );
}

void Register_UnityEngine_Graphics_INTERNAL_CALL_Internal_DrawMeshNow1()
{
    scripting_add_internal_call( "UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow1" , (gpointer)& Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow1 );
}

void Register_UnityEngine_Graphics_INTERNAL_CALL_Internal_DrawMeshNow2()
{
    scripting_add_internal_call( "UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow2" , (gpointer)& Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow2 );
}

void Register_UnityEngine_Graphics_DrawProcedural()
{
    scripting_add_internal_call( "UnityEngine.Graphics::DrawProcedural" , (gpointer)& Graphics_CUSTOM_DrawProcedural );
}

#if !UNITY_FLASH
void Register_UnityEngine_Graphics_DrawProceduralIndirect()
{
    scripting_add_internal_call( "UnityEngine.Graphics::DrawProceduralIndirect" , (gpointer)& Graphics_CUSTOM_DrawProceduralIndirect );
}

#endif
void Register_UnityEngine_Graphics_DrawTexture()
{
    scripting_add_internal_call( "UnityEngine.Graphics::DrawTexture" , (gpointer)& Graphics_CUSTOM_DrawTexture );
}

void Register_UnityEngine_Graphics_Blit()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Blit" , (gpointer)& Graphics_CUSTOM_Blit );
}

void Register_UnityEngine_Graphics_Internal_BlitMaterial()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_BlitMaterial" , (gpointer)& Graphics_CUSTOM_Internal_BlitMaterial );
}

void Register_UnityEngine_Graphics_Internal_BlitMultiTap()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_BlitMultiTap" , (gpointer)& Graphics_CUSTOM_Internal_BlitMultiTap );
}

void Register_UnityEngine_Graphics_Internal_SetRT()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_SetRT" , (gpointer)& Graphics_CUSTOM_Internal_SetRT );
}

void Register_UnityEngine_Graphics_Internal_SetRTBuffer()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_SetRTBuffer" , (gpointer)& Graphics_CUSTOM_Internal_SetRTBuffer );
}

void Register_UnityEngine_Graphics_Internal_SetRTBuffers()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_SetRTBuffers" , (gpointer)& Graphics_CUSTOM_Internal_SetRTBuffers );
}

void Register_UnityEngine_Graphics_GetActiveColorBuffer()
{
    scripting_add_internal_call( "UnityEngine.Graphics::GetActiveColorBuffer" , (gpointer)& Graphics_CUSTOM_GetActiveColorBuffer );
}

void Register_UnityEngine_Graphics_GetActiveDepthBuffer()
{
    scripting_add_internal_call( "UnityEngine.Graphics::GetActiveDepthBuffer" , (gpointer)& Graphics_CUSTOM_GetActiveDepthBuffer );
}

void Register_UnityEngine_Graphics_ClearRandomWriteTargets()
{
    scripting_add_internal_call( "UnityEngine.Graphics::ClearRandomWriteTargets" , (gpointer)& Graphics_CUSTOM_ClearRandomWriteTargets );
}

void Register_UnityEngine_Graphics_Internal_SetRandomWriteTargetRT()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_SetRandomWriteTargetRT" , (gpointer)& Graphics_CUSTOM_Internal_SetRandomWriteTargetRT );
}

#if !UNITY_FLASH
void Register_UnityEngine_Graphics_Internal_SetRandomWriteTargetBuffer()
{
    scripting_add_internal_call( "UnityEngine.Graphics::Internal_SetRandomWriteTargetBuffer" , (gpointer)& Graphics_CUSTOM_Internal_SetRandomWriteTargetBuffer );
}

#endif
void Register_UnityEngine_Graphics_SetupVertexLights()
{
    scripting_add_internal_call( "UnityEngine.Graphics::SetupVertexLights" , (gpointer)& Graphics_CUSTOM_SetupVertexLights );
}

#if ENABLE_MONO || PLATFORM_WINRT
void Register_UnityEngine_LightProbes_INTERNAL_CALL_GetInterpolatedLightProbe()
{
    scripting_add_internal_call( "UnityEngine.LightProbes::INTERNAL_CALL_GetInterpolatedLightProbe" , (gpointer)& LightProbes_CUSTOM_INTERNAL_CALL_GetInterpolatedLightProbe );
}

void Register_UnityEngine_LightProbes_get_positions()
{
    scripting_add_internal_call( "UnityEngine.LightProbes::get_positions" , (gpointer)& LightProbes_Get_Custom_PropPositions );
}

void Register_UnityEngine_LightProbes_get_coefficients()
{
    scripting_add_internal_call( "UnityEngine.LightProbes::get_coefficients" , (gpointer)& LightProbes_Get_Custom_PropCoefficients );
}

void Register_UnityEngine_LightProbes_set_coefficients()
{
    scripting_add_internal_call( "UnityEngine.LightProbes::set_coefficients" , (gpointer)& LightProbes_Set_Custom_PropCoefficients );
}

void Register_UnityEngine_LightProbes_get_count()
{
    scripting_add_internal_call( "UnityEngine.LightProbes::get_count" , (gpointer)& LightProbes_Get_Custom_PropCount );
}

void Register_UnityEngine_LightProbes_get_cellCount()
{
    scripting_add_internal_call( "UnityEngine.LightProbes::get_cellCount" , (gpointer)& LightProbes_Get_Custom_PropCellCount );
}

void Register_UnityEngine_LightmapSettings_get_lightmaps()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::get_lightmaps" , (gpointer)& LightmapSettings_Get_Custom_PropLightmaps );
}

void Register_UnityEngine_LightmapSettings_set_lightmaps()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::set_lightmaps" , (gpointer)& LightmapSettings_Set_Custom_PropLightmaps );
}

void Register_UnityEngine_LightmapSettings_get_lightmapsMode()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::get_lightmapsMode" , (gpointer)& LightmapSettings_Get_Custom_PropLightmapsMode );
}

void Register_UnityEngine_LightmapSettings_set_lightmapsMode()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::set_lightmapsMode" , (gpointer)& LightmapSettings_Set_Custom_PropLightmapsMode );
}

void Register_UnityEngine_LightmapSettings_get_bakedColorSpace()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::get_bakedColorSpace" , (gpointer)& LightmapSettings_Get_Custom_PropBakedColorSpace );
}

void Register_UnityEngine_LightmapSettings_set_bakedColorSpace()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::set_bakedColorSpace" , (gpointer)& LightmapSettings_Set_Custom_PropBakedColorSpace );
}

void Register_UnityEngine_LightmapSettings_get_lightProbes()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::get_lightProbes" , (gpointer)& LightmapSettings_Get_Custom_PropLightProbes );
}

void Register_UnityEngine_LightmapSettings_set_lightProbes()
{
    scripting_add_internal_call( "UnityEngine.LightmapSettings::set_lightProbes" , (gpointer)& LightmapSettings_Set_Custom_PropLightProbes );
}

#endif
void Register_UnityEngine_GeometryUtility_INTERNAL_CALL_Internal_ExtractPlanes()
{
    scripting_add_internal_call( "UnityEngine.GeometryUtility::INTERNAL_CALL_Internal_ExtractPlanes" , (gpointer)& GeometryUtility_CUSTOM_INTERNAL_CALL_Internal_ExtractPlanes );
}

void Register_UnityEngine_GeometryUtility_INTERNAL_CALL_TestPlanesAABB()
{
    scripting_add_internal_call( "UnityEngine.GeometryUtility::INTERNAL_CALL_TestPlanesAABB" , (gpointer)& GeometryUtility_CUSTOM_INTERNAL_CALL_TestPlanesAABB );
}

void Register_UnityEngine_Screen_get_resolutions()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_resolutions" , (gpointer)& Screen_Get_Custom_PropResolutions );
}

void Register_UnityEngine_Screen_get_currentResolution()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_currentResolution" , (gpointer)& Screen_Get_Custom_PropCurrentResolution );
}

void Register_UnityEngine_Screen_SetResolution()
{
    scripting_add_internal_call( "UnityEngine.Screen::SetResolution" , (gpointer)& Screen_CUSTOM_SetResolution );
}

void Register_UnityEngine_Screen_get_showCursor()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_showCursor" , (gpointer)& Screen_Get_Custom_PropShowCursor );
}

void Register_UnityEngine_Screen_set_showCursor()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_showCursor" , (gpointer)& Screen_Set_Custom_PropShowCursor );
}

void Register_UnityEngine_Screen_get_lockCursor()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_lockCursor" , (gpointer)& Screen_Get_Custom_PropLockCursor );
}

void Register_UnityEngine_Screen_set_lockCursor()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_lockCursor" , (gpointer)& Screen_Set_Custom_PropLockCursor );
}

void Register_UnityEngine_Screen_get_width()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_width" , (gpointer)& Screen_Get_Custom_PropWidth );
}

void Register_UnityEngine_Screen_get_height()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_height" , (gpointer)& Screen_Get_Custom_PropHeight );
}

void Register_UnityEngine_Screen_get_dpi()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_dpi" , (gpointer)& Screen_Get_Custom_PropDpi );
}

void Register_UnityEngine_Screen_get_fullScreen()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_fullScreen" , (gpointer)& Screen_Get_Custom_PropFullScreen );
}

void Register_UnityEngine_Screen_set_fullScreen()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_fullScreen" , (gpointer)& Screen_Set_Custom_PropFullScreen );
}

void Register_UnityEngine_Screen_get_autorotateToPortrait()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_autorotateToPortrait" , (gpointer)& Screen_Get_Custom_PropAutorotateToPortrait );
}

void Register_UnityEngine_Screen_set_autorotateToPortrait()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_autorotateToPortrait" , (gpointer)& Screen_Set_Custom_PropAutorotateToPortrait );
}

void Register_UnityEngine_Screen_get_autorotateToPortraitUpsideDown()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_autorotateToPortraitUpsideDown" , (gpointer)& Screen_Get_Custom_PropAutorotateToPortraitUpsideDown );
}

void Register_UnityEngine_Screen_set_autorotateToPortraitUpsideDown()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_autorotateToPortraitUpsideDown" , (gpointer)& Screen_Set_Custom_PropAutorotateToPortraitUpsideDown );
}

void Register_UnityEngine_Screen_get_autorotateToLandscapeLeft()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_autorotateToLandscapeLeft" , (gpointer)& Screen_Get_Custom_PropAutorotateToLandscapeLeft );
}

void Register_UnityEngine_Screen_set_autorotateToLandscapeLeft()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_autorotateToLandscapeLeft" , (gpointer)& Screen_Set_Custom_PropAutorotateToLandscapeLeft );
}

void Register_UnityEngine_Screen_get_autorotateToLandscapeRight()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_autorotateToLandscapeRight" , (gpointer)& Screen_Get_Custom_PropAutorotateToLandscapeRight );
}

void Register_UnityEngine_Screen_set_autorotateToLandscapeRight()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_autorotateToLandscapeRight" , (gpointer)& Screen_Set_Custom_PropAutorotateToLandscapeRight );
}

void Register_UnityEngine_Screen_get_orientation()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_orientation" , (gpointer)& Screen_Get_Custom_PropOrientation );
}

void Register_UnityEngine_Screen_set_orientation()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_orientation" , (gpointer)& Screen_Set_Custom_PropOrientation );
}

void Register_UnityEngine_Screen_get_sleepTimeout()
{
    scripting_add_internal_call( "UnityEngine.Screen::get_sleepTimeout" , (gpointer)& Screen_Get_Custom_PropSleepTimeout );
}

void Register_UnityEngine_Screen_set_sleepTimeout()
{
    scripting_add_internal_call( "UnityEngine.Screen::set_sleepTimeout" , (gpointer)& Screen_Set_Custom_PropSleepTimeout );
}

void Register_UnityEngine_RenderTexture_Internal_CreateRenderTexture()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_CreateRenderTexture" , (gpointer)& RenderTexture_CUSTOM_Internal_CreateRenderTexture );
}

void Register_UnityEngine_RenderTexture_GetTemporary()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::GetTemporary" , (gpointer)& RenderTexture_CUSTOM_GetTemporary );
}

void Register_UnityEngine_RenderTexture_ReleaseTemporary()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::ReleaseTemporary" , (gpointer)& RenderTexture_CUSTOM_ReleaseTemporary );
}

void Register_UnityEngine_RenderTexture_Internal_GetWidth()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_GetWidth" , (gpointer)& RenderTexture_CUSTOM_Internal_GetWidth );
}

void Register_UnityEngine_RenderTexture_Internal_SetWidth()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_SetWidth" , (gpointer)& RenderTexture_CUSTOM_Internal_SetWidth );
}

void Register_UnityEngine_RenderTexture_Internal_GetHeight()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_GetHeight" , (gpointer)& RenderTexture_CUSTOM_Internal_GetHeight );
}

void Register_UnityEngine_RenderTexture_Internal_SetHeight()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_SetHeight" , (gpointer)& RenderTexture_CUSTOM_Internal_SetHeight );
}

void Register_UnityEngine_RenderTexture_Internal_SetSRGBReadWrite()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_SetSRGBReadWrite" , (gpointer)& RenderTexture_CUSTOM_Internal_SetSRGBReadWrite );
}

void Register_UnityEngine_RenderTexture_get_depth()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_depth" , (gpointer)& RenderTexture_Get_Custom_PropDepth );
}

void Register_UnityEngine_RenderTexture_set_depth()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_depth" , (gpointer)& RenderTexture_Set_Custom_PropDepth );
}

void Register_UnityEngine_RenderTexture_get_isPowerOfTwo()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_isPowerOfTwo" , (gpointer)& RenderTexture_Get_Custom_PropIsPowerOfTwo );
}

void Register_UnityEngine_RenderTexture_set_isPowerOfTwo()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_isPowerOfTwo" , (gpointer)& RenderTexture_Set_Custom_PropIsPowerOfTwo );
}

void Register_UnityEngine_RenderTexture_get_sRGB()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_sRGB" , (gpointer)& RenderTexture_Get_Custom_PropSRGB );
}

void Register_UnityEngine_RenderTexture_get_format()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_format" , (gpointer)& RenderTexture_Get_Custom_PropFormat );
}

void Register_UnityEngine_RenderTexture_set_format()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_format" , (gpointer)& RenderTexture_Set_Custom_PropFormat );
}

void Register_UnityEngine_RenderTexture_get_useMipMap()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_useMipMap" , (gpointer)& RenderTexture_Get_Custom_PropUseMipMap );
}

void Register_UnityEngine_RenderTexture_set_useMipMap()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_useMipMap" , (gpointer)& RenderTexture_Set_Custom_PropUseMipMap );
}

void Register_UnityEngine_RenderTexture_get_isCubemap()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_isCubemap" , (gpointer)& RenderTexture_Get_Custom_PropIsCubemap );
}

void Register_UnityEngine_RenderTexture_set_isCubemap()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_isCubemap" , (gpointer)& RenderTexture_Set_Custom_PropIsCubemap );
}

void Register_UnityEngine_RenderTexture_get_isVolume()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_isVolume" , (gpointer)& RenderTexture_Get_Custom_PropIsVolume );
}

void Register_UnityEngine_RenderTexture_set_isVolume()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_isVolume" , (gpointer)& RenderTexture_Set_Custom_PropIsVolume );
}

void Register_UnityEngine_RenderTexture_get_volumeDepth()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_volumeDepth" , (gpointer)& RenderTexture_Get_Custom_PropVolumeDepth );
}

void Register_UnityEngine_RenderTexture_set_volumeDepth()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_volumeDepth" , (gpointer)& RenderTexture_Set_Custom_PropVolumeDepth );
}

void Register_UnityEngine_RenderTexture_get_antiAliasing()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_antiAliasing" , (gpointer)& RenderTexture_Get_Custom_PropAntiAliasing );
}

void Register_UnityEngine_RenderTexture_set_antiAliasing()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_antiAliasing" , (gpointer)& RenderTexture_Set_Custom_PropAntiAliasing );
}

void Register_UnityEngine_RenderTexture_get_enableRandomWrite()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_enableRandomWrite" , (gpointer)& RenderTexture_Get_Custom_PropEnableRandomWrite );
}

void Register_UnityEngine_RenderTexture_set_enableRandomWrite()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_enableRandomWrite" , (gpointer)& RenderTexture_Set_Custom_PropEnableRandomWrite );
}

void Register_UnityEngine_RenderTexture_INTERNAL_CALL_Create()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::INTERNAL_CALL_Create" , (gpointer)& RenderTexture_CUSTOM_INTERNAL_CALL_Create );
}

void Register_UnityEngine_RenderTexture_INTERNAL_CALL_Release()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::INTERNAL_CALL_Release" , (gpointer)& RenderTexture_CUSTOM_INTERNAL_CALL_Release );
}

void Register_UnityEngine_RenderTexture_INTERNAL_CALL_IsCreated()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::INTERNAL_CALL_IsCreated" , (gpointer)& RenderTexture_CUSTOM_INTERNAL_CALL_IsCreated );
}

void Register_UnityEngine_RenderTexture_INTERNAL_CALL_DiscardContents()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::INTERNAL_CALL_DiscardContents" , (gpointer)& RenderTexture_CUSTOM_INTERNAL_CALL_DiscardContents );
}

void Register_UnityEngine_RenderTexture_GetColorBuffer()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::GetColorBuffer" , (gpointer)& RenderTexture_CUSTOM_GetColorBuffer );
}

void Register_UnityEngine_RenderTexture_GetDepthBuffer()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::GetDepthBuffer" , (gpointer)& RenderTexture_CUSTOM_GetDepthBuffer );
}

void Register_UnityEngine_RenderTexture_SetGlobalShaderProperty()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::SetGlobalShaderProperty" , (gpointer)& RenderTexture_CUSTOM_SetGlobalShaderProperty );
}

void Register_UnityEngine_RenderTexture_get_active()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_active" , (gpointer)& RenderTexture_Get_Custom_PropActive );
}

void Register_UnityEngine_RenderTexture_set_active()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_active" , (gpointer)& RenderTexture_Set_Custom_PropActive );
}

void Register_UnityEngine_RenderTexture_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::get_enabled" , (gpointer)& RenderTexture_Get_Custom_PropEnabled );
}

void Register_UnityEngine_RenderTexture_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::set_enabled" , (gpointer)& RenderTexture_Set_Custom_PropEnabled );
}

void Register_UnityEngine_RenderTexture_Internal_GetTexelOffset()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::Internal_GetTexelOffset" , (gpointer)& RenderTexture_CUSTOM_Internal_GetTexelOffset );
}

void Register_UnityEngine_RenderTexture_SupportsStencil()
{
    scripting_add_internal_call( "UnityEngine.RenderTexture::SupportsStencil" , (gpointer)& RenderTexture_CUSTOM_SupportsStencil );
}

#if ENABLE_MOVIES
void Register_UnityEngine_MovieTexture_INTERNAL_CALL_Play()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::INTERNAL_CALL_Play" , (gpointer)& MovieTexture_CUSTOM_INTERNAL_CALL_Play );
}

void Register_UnityEngine_MovieTexture_INTERNAL_CALL_Stop()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::INTERNAL_CALL_Stop" , (gpointer)& MovieTexture_CUSTOM_INTERNAL_CALL_Stop );
}

void Register_UnityEngine_MovieTexture_INTERNAL_CALL_Pause()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::INTERNAL_CALL_Pause" , (gpointer)& MovieTexture_CUSTOM_INTERNAL_CALL_Pause );
}

#endif
#if (ENABLE_MOVIES) && (ENABLE_AUDIO)
void Register_UnityEngine_MovieTexture_get_audioClip()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::get_audioClip" , (gpointer)& MovieTexture_Get_Custom_PropAudioClip );
}

#endif
#if ENABLE_MOVIES
void Register_UnityEngine_MovieTexture_get_loop()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::get_loop" , (gpointer)& MovieTexture_Get_Custom_PropLoop );
}

void Register_UnityEngine_MovieTexture_set_loop()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::set_loop" , (gpointer)& MovieTexture_Set_Custom_PropLoop );
}

void Register_UnityEngine_MovieTexture_get_isPlaying()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::get_isPlaying" , (gpointer)& MovieTexture_Get_Custom_PropIsPlaying );
}

void Register_UnityEngine_MovieTexture_get_isReadyToPlay()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::get_isReadyToPlay" , (gpointer)& MovieTexture_Get_Custom_PropIsReadyToPlay );
}

void Register_UnityEngine_MovieTexture_get_duration()
{
    scripting_add_internal_call( "UnityEngine.MovieTexture::get_duration" , (gpointer)& MovieTexture_Get_Custom_PropDuration );
}

#endif
void Register_UnityEngine_GL_Vertex3()
{
    scripting_add_internal_call( "UnityEngine.GL::Vertex3" , (gpointer)& GL_CUSTOM_Vertex3 );
}

void Register_UnityEngine_GL_INTERNAL_CALL_Vertex()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_Vertex" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_Vertex );
}

void Register_UnityEngine_GL_INTERNAL_CALL_Color()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_Color" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_Color );
}

void Register_UnityEngine_GL_INTERNAL_CALL_TexCoord()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_TexCoord" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_TexCoord );
}

void Register_UnityEngine_GL_TexCoord2()
{
    scripting_add_internal_call( "UnityEngine.GL::TexCoord2" , (gpointer)& GL_CUSTOM_TexCoord2 );
}

void Register_UnityEngine_GL_TexCoord3()
{
    scripting_add_internal_call( "UnityEngine.GL::TexCoord3" , (gpointer)& GL_CUSTOM_TexCoord3 );
}

void Register_UnityEngine_GL_MultiTexCoord2()
{
    scripting_add_internal_call( "UnityEngine.GL::MultiTexCoord2" , (gpointer)& GL_CUSTOM_MultiTexCoord2 );
}

void Register_UnityEngine_GL_MultiTexCoord3()
{
    scripting_add_internal_call( "UnityEngine.GL::MultiTexCoord3" , (gpointer)& GL_CUSTOM_MultiTexCoord3 );
}

void Register_UnityEngine_GL_INTERNAL_CALL_MultiTexCoord()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_MultiTexCoord" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_MultiTexCoord );
}

void Register_UnityEngine_GL_Begin()
{
    scripting_add_internal_call( "UnityEngine.GL::Begin" , (gpointer)& GL_CUSTOM_Begin );
}

void Register_UnityEngine_GL_End()
{
    scripting_add_internal_call( "UnityEngine.GL::End" , (gpointer)& GL_CUSTOM_End );
}

void Register_UnityEngine_GL_LoadOrtho()
{
    scripting_add_internal_call( "UnityEngine.GL::LoadOrtho" , (gpointer)& GL_CUSTOM_LoadOrtho );
}

void Register_UnityEngine_GL_LoadPixelMatrix()
{
    scripting_add_internal_call( "UnityEngine.GL::LoadPixelMatrix" , (gpointer)& GL_CUSTOM_LoadPixelMatrix );
}

void Register_UnityEngine_GL_LoadPixelMatrixArgs()
{
    scripting_add_internal_call( "UnityEngine.GL::LoadPixelMatrixArgs" , (gpointer)& GL_CUSTOM_LoadPixelMatrixArgs );
}

void Register_UnityEngine_GL_INTERNAL_CALL_Viewport()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_Viewport" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_Viewport );
}

void Register_UnityEngine_GL_INTERNAL_CALL_LoadProjectionMatrix()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_LoadProjectionMatrix" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_LoadProjectionMatrix );
}

void Register_UnityEngine_GL_LoadIdentity()
{
    scripting_add_internal_call( "UnityEngine.GL::LoadIdentity" , (gpointer)& GL_CUSTOM_LoadIdentity );
}

void Register_UnityEngine_GL_INTERNAL_get_modelview()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_get_modelview" , (gpointer)& GL_CUSTOM_INTERNAL_get_modelview );
}

void Register_UnityEngine_GL_INTERNAL_set_modelview()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_set_modelview" , (gpointer)& GL_CUSTOM_INTERNAL_set_modelview );
}

void Register_UnityEngine_GL_INTERNAL_CALL_MultMatrix()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_MultMatrix" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_MultMatrix );
}

void Register_UnityEngine_GL_PushMatrix()
{
    scripting_add_internal_call( "UnityEngine.GL::PushMatrix" , (gpointer)& GL_CUSTOM_PushMatrix );
}

void Register_UnityEngine_GL_PopMatrix()
{
    scripting_add_internal_call( "UnityEngine.GL::PopMatrix" , (gpointer)& GL_CUSTOM_PopMatrix );
}

void Register_UnityEngine_GL_INTERNAL_CALL_GetGPUProjectionMatrix()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_GetGPUProjectionMatrix" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_GetGPUProjectionMatrix );
}

void Register_UnityEngine_GL_get_wireframe()
{
    scripting_add_internal_call( "UnityEngine.GL::get_wireframe" , (gpointer)& GL_Get_Custom_PropWireframe );
}

void Register_UnityEngine_GL_set_wireframe()
{
    scripting_add_internal_call( "UnityEngine.GL::set_wireframe" , (gpointer)& GL_Set_Custom_PropWireframe );
}

void Register_UnityEngine_GL_SetRevertBackfacing()
{
    scripting_add_internal_call( "UnityEngine.GL::SetRevertBackfacing" , (gpointer)& GL_CUSTOM_SetRevertBackfacing );
}

void Register_UnityEngine_GL_INTERNAL_CALL_Internal_Clear()
{
    scripting_add_internal_call( "UnityEngine.GL::INTERNAL_CALL_Internal_Clear" , (gpointer)& GL_CUSTOM_INTERNAL_CALL_Internal_Clear );
}

void Register_UnityEngine_GL_ClearWithSkybox()
{
    scripting_add_internal_call( "UnityEngine.GL::ClearWithSkybox" , (gpointer)& GL_CUSTOM_ClearWithSkybox );
}

void Register_UnityEngine_GL_InvalidateState()
{
    scripting_add_internal_call( "UnityEngine.GL::InvalidateState" , (gpointer)& GL_CUSTOM_InvalidateState );
}

void Register_UnityEngine_GL_IssuePluginEvent()
{
    scripting_add_internal_call( "UnityEngine.GL::IssuePluginEvent" , (gpointer)& GL_CUSTOM_IssuePluginEvent );
}

void Register_UnityEngine_GUIElement_INTERNAL_CALL_HitTest()
{
    scripting_add_internal_call( "UnityEngine.GUIElement::INTERNAL_CALL_HitTest" , (gpointer)& GUIElement_CUSTOM_INTERNAL_CALL_HitTest );
}

void Register_UnityEngine_GUIElement_INTERNAL_CALL_GetScreenRect()
{
    scripting_add_internal_call( "UnityEngine.GUIElement::INTERNAL_CALL_GetScreenRect" , (gpointer)& GUIElement_CUSTOM_INTERNAL_CALL_GetScreenRect );
}

void Register_UnityEngine_GUITexture_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::INTERNAL_get_color" , (gpointer)& GUITexture_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_GUITexture_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::INTERNAL_set_color" , (gpointer)& GUITexture_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_GUITexture_get_texture()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::get_texture" , (gpointer)& GUITexture_Get_Custom_PropTexture );
}

void Register_UnityEngine_GUITexture_set_texture()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::set_texture" , (gpointer)& GUITexture_Set_Custom_PropTexture );
}

void Register_UnityEngine_GUITexture_INTERNAL_get_pixelInset()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::INTERNAL_get_pixelInset" , (gpointer)& GUITexture_CUSTOM_INTERNAL_get_pixelInset );
}

void Register_UnityEngine_GUITexture_INTERNAL_set_pixelInset()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::INTERNAL_set_pixelInset" , (gpointer)& GUITexture_CUSTOM_INTERNAL_set_pixelInset );
}

void Register_UnityEngine_GUITexture_get_border()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::get_border" , (gpointer)& GUITexture_Get_Custom_PropBorder );
}

void Register_UnityEngine_GUITexture_set_border()
{
    scripting_add_internal_call( "UnityEngine.GUITexture::set_border" , (gpointer)& GUITexture_Set_Custom_PropBorder );
}

void Register_UnityEngine_GUIText_get_text()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_text" , (gpointer)& GUIText_Get_Custom_PropText );
}

void Register_UnityEngine_GUIText_set_text()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_text" , (gpointer)& GUIText_Set_Custom_PropText );
}

void Register_UnityEngine_GUIText_get_material()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_material" , (gpointer)& GUIText_Get_Custom_PropMaterial );
}

void Register_UnityEngine_GUIText_set_material()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_material" , (gpointer)& GUIText_Set_Custom_PropMaterial );
}

void Register_UnityEngine_GUIText_Internal_GetPixelOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIText::Internal_GetPixelOffset" , (gpointer)& GUIText_CUSTOM_Internal_GetPixelOffset );
}

void Register_UnityEngine_GUIText_INTERNAL_CALL_Internal_SetPixelOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIText::INTERNAL_CALL_Internal_SetPixelOffset" , (gpointer)& GUIText_CUSTOM_INTERNAL_CALL_Internal_SetPixelOffset );
}

void Register_UnityEngine_GUIText_get_font()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_font" , (gpointer)& GUIText_Get_Custom_PropFont );
}

void Register_UnityEngine_GUIText_set_font()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_font" , (gpointer)& GUIText_Set_Custom_PropFont );
}

void Register_UnityEngine_GUIText_get_alignment()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_alignment" , (gpointer)& GUIText_Get_Custom_PropAlignment );
}

void Register_UnityEngine_GUIText_set_alignment()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_alignment" , (gpointer)& GUIText_Set_Custom_PropAlignment );
}

void Register_UnityEngine_GUIText_get_anchor()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_anchor" , (gpointer)& GUIText_Get_Custom_PropAnchor );
}

void Register_UnityEngine_GUIText_set_anchor()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_anchor" , (gpointer)& GUIText_Set_Custom_PropAnchor );
}

void Register_UnityEngine_GUIText_get_lineSpacing()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_lineSpacing" , (gpointer)& GUIText_Get_Custom_PropLineSpacing );
}

void Register_UnityEngine_GUIText_set_lineSpacing()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_lineSpacing" , (gpointer)& GUIText_Set_Custom_PropLineSpacing );
}

void Register_UnityEngine_GUIText_get_tabSize()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_tabSize" , (gpointer)& GUIText_Get_Custom_PropTabSize );
}

void Register_UnityEngine_GUIText_set_tabSize()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_tabSize" , (gpointer)& GUIText_Set_Custom_PropTabSize );
}

void Register_UnityEngine_GUIText_get_fontSize()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_fontSize" , (gpointer)& GUIText_Get_Custom_PropFontSize );
}

void Register_UnityEngine_GUIText_set_fontSize()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_fontSize" , (gpointer)& GUIText_Set_Custom_PropFontSize );
}

void Register_UnityEngine_GUIText_get_fontStyle()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_fontStyle" , (gpointer)& GUIText_Get_Custom_PropFontStyle );
}

void Register_UnityEngine_GUIText_set_fontStyle()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_fontStyle" , (gpointer)& GUIText_Set_Custom_PropFontStyle );
}

void Register_UnityEngine_GUIText_get_richText()
{
    scripting_add_internal_call( "UnityEngine.GUIText::get_richText" , (gpointer)& GUIText_Get_Custom_PropRichText );
}

void Register_UnityEngine_GUIText_set_richText()
{
    scripting_add_internal_call( "UnityEngine.GUIText::set_richText" , (gpointer)& GUIText_Set_Custom_PropRichText );
}

void Register_UnityEngine_GUIText_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.GUIText::INTERNAL_get_color" , (gpointer)& GUIText_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_GUIText_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.GUIText::INTERNAL_set_color" , (gpointer)& GUIText_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_Font_Internal_CreateFont()
{
    scripting_add_internal_call( "UnityEngine.Font::Internal_CreateFont" , (gpointer)& Font_CUSTOM_Internal_CreateFont );
}

void Register_UnityEngine_Font_get_material()
{
    scripting_add_internal_call( "UnityEngine.Font::get_material" , (gpointer)& Font_Get_Custom_PropMaterial );
}

void Register_UnityEngine_Font_set_material()
{
    scripting_add_internal_call( "UnityEngine.Font::set_material" , (gpointer)& Font_Set_Custom_PropMaterial );
}

void Register_UnityEngine_Font_HasCharacter()
{
    scripting_add_internal_call( "UnityEngine.Font::HasCharacter" , (gpointer)& Font_CUSTOM_HasCharacter );
}

#if ENABLE_MONO
void Register_UnityEngine_Font_get_fontNames()
{
    scripting_add_internal_call( "UnityEngine.Font::get_fontNames" , (gpointer)& Font_Get_Custom_PropFontNames );
}

void Register_UnityEngine_Font_set_fontNames()
{
    scripting_add_internal_call( "UnityEngine.Font::set_fontNames" , (gpointer)& Font_Set_Custom_PropFontNames );
}

#endif
void Register_UnityEngine_Font_get_characterInfo()
{
    scripting_add_internal_call( "UnityEngine.Font::get_characterInfo" , (gpointer)& Font_Get_Custom_PropCharacterInfo );
}

void Register_UnityEngine_Font_set_characterInfo()
{
    scripting_add_internal_call( "UnityEngine.Font::set_characterInfo" , (gpointer)& Font_Set_Custom_PropCharacterInfo );
}

void Register_UnityEngine_Font_RequestCharactersInTexture()
{
    scripting_add_internal_call( "UnityEngine.Font::RequestCharactersInTexture" , (gpointer)& Font_CUSTOM_RequestCharactersInTexture );
}

void Register_UnityEngine_Font_GetCharacterInfo()
{
    scripting_add_internal_call( "UnityEngine.Font::GetCharacterInfo" , (gpointer)& Font_CUSTOM_GetCharacterInfo );
}

void Register_UnityEngine_GUILayer_INTERNAL_CALL_HitTest()
{
    scripting_add_internal_call( "UnityEngine.GUILayer::INTERNAL_CALL_HitTest" , (gpointer)& GUILayer_CUSTOM_INTERNAL_CALL_HitTest );
}

#if !PLATFORM_WEBGL && !UNITY_FLASH
void Register_UnityEngine_StaticBatchingUtility_InternalCombineVertices()
{
    scripting_add_internal_call( "UnityEngine.StaticBatchingUtility::InternalCombineVertices" , (gpointer)& StaticBatchingUtility_CUSTOM_InternalCombineVertices );
}

void Register_UnityEngine_StaticBatchingUtility_InternalCombineIndices()
{
    scripting_add_internal_call( "UnityEngine.StaticBatchingUtility::InternalCombineIndices" , (gpointer)& StaticBatchingUtility_CUSTOM_InternalCombineIndices );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Graphics_IcallNames [] =
{
    "UnityEngine.OcclusionArea::INTERNAL_get_center",    // -> OcclusionArea_CUSTOM_INTERNAL_get_center
    "UnityEngine.OcclusionArea::INTERNAL_set_center",    // -> OcclusionArea_CUSTOM_INTERNAL_set_center
    "UnityEngine.OcclusionArea::INTERNAL_get_size",    // -> OcclusionArea_CUSTOM_INTERNAL_get_size
    "UnityEngine.OcclusionArea::INTERNAL_set_size",    // -> OcclusionArea_CUSTOM_INTERNAL_set_size
    "UnityEngine.OcclusionPortal::get_open" ,    // -> OcclusionPortal_Get_Custom_PropOpen
    "UnityEngine.OcclusionPortal::set_open" ,    // -> OcclusionPortal_Set_Custom_PropOpen
    "UnityEngine.RenderSettings::get_fog"   ,    // -> RenderSettings_Get_Custom_PropFog
    "UnityEngine.RenderSettings::set_fog"   ,    // -> RenderSettings_Set_Custom_PropFog
    "UnityEngine.RenderSettings::get_fogMode",    // -> RenderSettings_Get_Custom_PropFogMode
    "UnityEngine.RenderSettings::set_fogMode",    // -> RenderSettings_Set_Custom_PropFogMode
    "UnityEngine.RenderSettings::INTERNAL_get_fogColor",    // -> RenderSettings_CUSTOM_INTERNAL_get_fogColor
    "UnityEngine.RenderSettings::INTERNAL_set_fogColor",    // -> RenderSettings_CUSTOM_INTERNAL_set_fogColor
    "UnityEngine.RenderSettings::get_fogDensity",    // -> RenderSettings_Get_Custom_PropFogDensity
    "UnityEngine.RenderSettings::set_fogDensity",    // -> RenderSettings_Set_Custom_PropFogDensity
    "UnityEngine.RenderSettings::get_fogStartDistance",    // -> RenderSettings_Get_Custom_PropFogStartDistance
    "UnityEngine.RenderSettings::set_fogStartDistance",    // -> RenderSettings_Set_Custom_PropFogStartDistance
    "UnityEngine.RenderSettings::get_fogEndDistance",    // -> RenderSettings_Get_Custom_PropFogEndDistance
    "UnityEngine.RenderSettings::set_fogEndDistance",    // -> RenderSettings_Set_Custom_PropFogEndDistance
    "UnityEngine.RenderSettings::INTERNAL_get_ambientLight",    // -> RenderSettings_CUSTOM_INTERNAL_get_ambientLight
    "UnityEngine.RenderSettings::INTERNAL_set_ambientLight",    // -> RenderSettings_CUSTOM_INTERNAL_set_ambientLight
    "UnityEngine.RenderSettings::get_haloStrength",    // -> RenderSettings_Get_Custom_PropHaloStrength
    "UnityEngine.RenderSettings::set_haloStrength",    // -> RenderSettings_Set_Custom_PropHaloStrength
    "UnityEngine.RenderSettings::get_flareStrength",    // -> RenderSettings_Get_Custom_PropFlareStrength
    "UnityEngine.RenderSettings::set_flareStrength",    // -> RenderSettings_Set_Custom_PropFlareStrength
    "UnityEngine.RenderSettings::get_skybox",    // -> RenderSettings_Get_Custom_PropSkybox
    "UnityEngine.RenderSettings::set_skybox",    // -> RenderSettings_Set_Custom_PropSkybox
#if ENABLE_MONO || PLATFORM_WINRT
    "UnityEngine.QualitySettings::get_names",    // -> QualitySettings_Get_Custom_PropNames
#endif
    "UnityEngine.QualitySettings::GetQualityLevel",    // -> QualitySettings_CUSTOM_GetQualityLevel
    "UnityEngine.QualitySettings::SetQualityLevel",    // -> QualitySettings_CUSTOM_SetQualityLevel
    "UnityEngine.QualitySettings::get_currentLevel",    // -> QualitySettings_Get_Custom_PropCurrentLevel
    "UnityEngine.QualitySettings::set_currentLevel",    // -> QualitySettings_Set_Custom_PropCurrentLevel
    "UnityEngine.QualitySettings::IncreaseLevel",    // -> QualitySettings_CUSTOM_IncreaseLevel
    "UnityEngine.QualitySettings::DecreaseLevel",    // -> QualitySettings_CUSTOM_DecreaseLevel
    "UnityEngine.QualitySettings::get_pixelLightCount",    // -> QualitySettings_Get_Custom_PropPixelLightCount
    "UnityEngine.QualitySettings::set_pixelLightCount",    // -> QualitySettings_Set_Custom_PropPixelLightCount
    "UnityEngine.QualitySettings::get_shadowProjection",    // -> QualitySettings_Get_Custom_PropShadowProjection
    "UnityEngine.QualitySettings::set_shadowProjection",    // -> QualitySettings_Set_Custom_PropShadowProjection
    "UnityEngine.QualitySettings::get_shadowCascades",    // -> QualitySettings_Get_Custom_PropShadowCascades
    "UnityEngine.QualitySettings::set_shadowCascades",    // -> QualitySettings_Set_Custom_PropShadowCascades
    "UnityEngine.QualitySettings::get_shadowDistance",    // -> QualitySettings_Get_Custom_PropShadowDistance
    "UnityEngine.QualitySettings::set_shadowDistance",    // -> QualitySettings_Set_Custom_PropShadowDistance
    "UnityEngine.QualitySettings::get_masterTextureLimit",    // -> QualitySettings_Get_Custom_PropMasterTextureLimit
    "UnityEngine.QualitySettings::set_masterTextureLimit",    // -> QualitySettings_Set_Custom_PropMasterTextureLimit
    "UnityEngine.QualitySettings::get_anisotropicFiltering",    // -> QualitySettings_Get_Custom_PropAnisotropicFiltering
    "UnityEngine.QualitySettings::set_anisotropicFiltering",    // -> QualitySettings_Set_Custom_PropAnisotropicFiltering
    "UnityEngine.QualitySettings::get_lodBias",    // -> QualitySettings_Get_Custom_PropLodBias
    "UnityEngine.QualitySettings::set_lodBias",    // -> QualitySettings_Set_Custom_PropLodBias
    "UnityEngine.QualitySettings::get_maximumLODLevel",    // -> QualitySettings_Get_Custom_PropMaximumLODLevel
    "UnityEngine.QualitySettings::set_maximumLODLevel",    // -> QualitySettings_Set_Custom_PropMaximumLODLevel
    "UnityEngine.QualitySettings::get_particleRaycastBudget",    // -> QualitySettings_Get_Custom_PropParticleRaycastBudget
    "UnityEngine.QualitySettings::set_particleRaycastBudget",    // -> QualitySettings_Set_Custom_PropParticleRaycastBudget
    "UnityEngine.QualitySettings::get_softVegetation",    // -> QualitySettings_Get_Custom_PropSoftVegetation
    "UnityEngine.QualitySettings::set_softVegetation",    // -> QualitySettings_Set_Custom_PropSoftVegetation
    "UnityEngine.QualitySettings::get_maxQueuedFrames",    // -> QualitySettings_Get_Custom_PropMaxQueuedFrames
    "UnityEngine.QualitySettings::set_maxQueuedFrames",    // -> QualitySettings_Set_Custom_PropMaxQueuedFrames
    "UnityEngine.QualitySettings::get_vSyncCount",    // -> QualitySettings_Get_Custom_PropVSyncCount
    "UnityEngine.QualitySettings::set_vSyncCount",    // -> QualitySettings_Set_Custom_PropVSyncCount
    "UnityEngine.QualitySettings::get_antiAliasing",    // -> QualitySettings_Get_Custom_PropAntiAliasing
    "UnityEngine.QualitySettings::set_antiAliasing",    // -> QualitySettings_Set_Custom_PropAntiAliasing
    "UnityEngine.QualitySettings::get_desiredColorSpace",    // -> QualitySettings_Get_Custom_PropDesiredColorSpace
    "UnityEngine.QualitySettings::get_activeColorSpace",    // -> QualitySettings_Get_Custom_PropActiveColorSpace
    "UnityEngine.QualitySettings::get_blendWeights",    // -> QualitySettings_Get_Custom_PropBlendWeights
    "UnityEngine.QualitySettings::set_blendWeights",    // -> QualitySettings_Set_Custom_PropBlendWeights
    "UnityEngine.Shader::Find"              ,    // -> Shader_CUSTOM_Find
    "UnityEngine.Shader::FindBuiltin"       ,    // -> Shader_CUSTOM_FindBuiltin
    "UnityEngine.Shader::get_isSupported"   ,    // -> Shader_Get_Custom_PropIsSupported
#if UNITY_EDITOR
    "UnityEngine.Shader::get_customEditor"  ,    // -> Shader_Get_Custom_PropCustomEditor
#endif
    "UnityEngine.Shader::EnableKeyword"     ,    // -> Shader_CUSTOM_EnableKeyword
    "UnityEngine.Shader::DisableKeyword"    ,    // -> Shader_CUSTOM_DisableKeyword
    "UnityEngine.Shader::get_maximumLOD"    ,    // -> Shader_Get_Custom_PropMaximumLOD
    "UnityEngine.Shader::set_maximumLOD"    ,    // -> Shader_Set_Custom_PropMaximumLOD
    "UnityEngine.Shader::get_globalMaximumLOD",    // -> Shader_Get_Custom_PropGlobalMaximumLOD
    "UnityEngine.Shader::set_globalMaximumLOD",    // -> Shader_Set_Custom_PropGlobalMaximumLOD
    "UnityEngine.Shader::get_renderQueue"   ,    // -> Shader_Get_Custom_PropRenderQueue
    "UnityEngine.Shader::INTERNAL_CALL_SetGlobalColor",    // -> Shader_CUSTOM_INTERNAL_CALL_SetGlobalColor
    "UnityEngine.Shader::SetGlobalFloat"    ,    // -> Shader_CUSTOM_SetGlobalFloat
    "UnityEngine.Shader::SetGlobalTexture"  ,    // -> Shader_CUSTOM_SetGlobalTexture
    "UnityEngine.Shader::INTERNAL_CALL_SetGlobalMatrix",    // -> Shader_CUSTOM_INTERNAL_CALL_SetGlobalMatrix
    "UnityEngine.Shader::SetGlobalTexGenMode",    // -> Shader_CUSTOM_SetGlobalTexGenMode
    "UnityEngine.Shader::SetGlobalTextureMatrixName",    // -> Shader_CUSTOM_SetGlobalTextureMatrixName
#if !UNITY_FLASH
    "UnityEngine.Shader::SetGlobalBuffer"   ,    // -> Shader_CUSTOM_SetGlobalBuffer
#endif
    "UnityEngine.Shader::PropertyToID"      ,    // -> Shader_CUSTOM_PropertyToID
    "UnityEngine.Shader::WarmupAllShaders"  ,    // -> Shader_CUSTOM_WarmupAllShaders
    "UnityEngine.Texture::get_masterTextureLimit",    // -> Texture_Get_Custom_PropMasterTextureLimit
    "UnityEngine.Texture::set_masterTextureLimit",    // -> Texture_Set_Custom_PropMasterTextureLimit
    "UnityEngine.Texture::get_anisotropicFiltering",    // -> Texture_Get_Custom_PropAnisotropicFiltering
    "UnityEngine.Texture::set_anisotropicFiltering",    // -> Texture_Set_Custom_PropAnisotropicFiltering
    "UnityEngine.Texture::SetGlobalAnisotropicFilteringLimits",    // -> Texture_CUSTOM_SetGlobalAnisotropicFilteringLimits
    "UnityEngine.Texture::Internal_GetWidth",    // -> Texture_CUSTOM_Internal_GetWidth
    "UnityEngine.Texture::Internal_GetHeight",    // -> Texture_CUSTOM_Internal_GetHeight
    "UnityEngine.Texture::get_filterMode"   ,    // -> Texture_Get_Custom_PropFilterMode
    "UnityEngine.Texture::set_filterMode"   ,    // -> Texture_Set_Custom_PropFilterMode
    "UnityEngine.Texture::get_anisoLevel"   ,    // -> Texture_Get_Custom_PropAnisoLevel
    "UnityEngine.Texture::set_anisoLevel"   ,    // -> Texture_Set_Custom_PropAnisoLevel
    "UnityEngine.Texture::get_wrapMode"     ,    // -> Texture_Get_Custom_PropWrapMode
    "UnityEngine.Texture::set_wrapMode"     ,    // -> Texture_Set_Custom_PropWrapMode
    "UnityEngine.Texture::get_mipMapBias"   ,    // -> Texture_Get_Custom_PropMipMapBias
    "UnityEngine.Texture::set_mipMapBias"   ,    // -> Texture_Set_Custom_PropMipMapBias
    "UnityEngine.Texture::Internal_GetTexelSize",    // -> Texture_CUSTOM_Internal_GetTexelSize
    "UnityEngine.Texture::INTERNAL_CALL_GetNativeTexturePtr",    // -> Texture_CUSTOM_INTERNAL_CALL_GetNativeTexturePtr
    "UnityEngine.Texture::GetNativeTextureID",    // -> Texture_CUSTOM_GetNativeTextureID
    "UnityEngine.Texture2D::get_mipmapCount",    // -> Texture2D_Get_Custom_PropMipmapCount
    "UnityEngine.Texture2D::Internal_Create",    // -> Texture2D_CUSTOM_Internal_Create
#if ENABLE_TEXTUREID_MAP
    "UnityEngine.Texture2D::UpdateExternalTexture",    // -> Texture2D_CUSTOM_UpdateExternalTexture
#endif
    "UnityEngine.Texture2D::get_format"     ,    // -> Texture2D_Get_Custom_PropFormat
    "UnityEngine.Texture2D::INTERNAL_CALL_SetPixel",    // -> Texture2D_CUSTOM_INTERNAL_CALL_SetPixel
    "UnityEngine.Texture2D::INTERNAL_CALL_GetPixel",    // -> Texture2D_CUSTOM_INTERNAL_CALL_GetPixel
    "UnityEngine.Texture2D::INTERNAL_CALL_GetPixelBilinear",    // -> Texture2D_CUSTOM_INTERNAL_CALL_GetPixelBilinear
    "UnityEngine.Texture2D::SetPixels"      ,    // -> Texture2D_CUSTOM_SetPixels
    "UnityEngine.Texture2D::SetPixels32"    ,    // -> Texture2D_CUSTOM_SetPixels32
    "UnityEngine.Texture2D::LoadImage"      ,    // -> Texture2D_CUSTOM_LoadImage
    "UnityEngine.Texture2D::GetPixels"      ,    // -> Texture2D_CUSTOM_GetPixels
    "UnityEngine.Texture2D::GetPixels32"    ,    // -> Texture2D_CUSTOM_GetPixels32
    "UnityEngine.Texture2D::Apply"          ,    // -> Texture2D_CUSTOM_Apply
    "UnityEngine.Texture2D::Resize"         ,    // -> Texture2D_CUSTOM_Resize
    "UnityEngine.Texture2D::Internal_ResizeWH",    // -> Texture2D_CUSTOM_Internal_ResizeWH
    "UnityEngine.Texture2D::INTERNAL_CALL_Compress",    // -> Texture2D_CUSTOM_INTERNAL_CALL_Compress
    "UnityEngine.Texture2D::PackTextures"   ,    // -> Texture2D_CUSTOM_PackTextures
    "UnityEngine.Texture2D::INTERNAL_CALL_ReadPixels",    // -> Texture2D_CUSTOM_INTERNAL_CALL_ReadPixels
    "UnityEngine.Texture2D::EncodeToPNG"    ,    // -> Texture2D_CUSTOM_EncodeToPNG
#if UNITY_EDITOR
    "UnityEngine.Texture2D::get_alphaIsTransparency",    // -> Texture2D_Get_Custom_PropAlphaIsTransparency
    "UnityEngine.Texture2D::set_alphaIsTransparency",    // -> Texture2D_Set_Custom_PropAlphaIsTransparency
#endif
    "UnityEngine.Cubemap::INTERNAL_CALL_SetPixel",    // -> Cubemap_CUSTOM_INTERNAL_CALL_SetPixel
    "UnityEngine.Cubemap::INTERNAL_CALL_GetPixel",    // -> Cubemap_CUSTOM_INTERNAL_CALL_GetPixel
    "UnityEngine.Cubemap::GetPixels"        ,    // -> Cubemap_CUSTOM_GetPixels
    "UnityEngine.Cubemap::SetPixels"        ,    // -> Cubemap_CUSTOM_SetPixels
    "UnityEngine.Cubemap::Apply"            ,    // -> Cubemap_CUSTOM_Apply
    "UnityEngine.Cubemap::get_format"       ,    // -> Cubemap_Get_Custom_PropFormat
    "UnityEngine.Cubemap::Internal_Create"  ,    // -> Cubemap_CUSTOM_Internal_Create
    "UnityEngine.Cubemap::SmoothEdges"      ,    // -> Cubemap_CUSTOM_SmoothEdges
    "UnityEngine.Texture3D::get_depth"      ,    // -> Texture3D_Get_Custom_PropDepth
    "UnityEngine.Texture3D::GetPixels"      ,    // -> Texture3D_CUSTOM_GetPixels
    "UnityEngine.Texture3D::SetPixels"      ,    // -> Texture3D_CUSTOM_SetPixels
    "UnityEngine.Texture3D::Apply"          ,    // -> Texture3D_CUSTOM_Apply
    "UnityEngine.Texture3D::get_format"     ,    // -> Texture3D_Get_Custom_PropFormat
    "UnityEngine.Texture3D::Internal_Create",    // -> Texture3D_CUSTOM_Internal_Create
    "UnityEngine.MeshFilter::get_mesh"      ,    // -> MeshFilter_Get_Custom_PropMesh
    "UnityEngine.MeshFilter::set_mesh"      ,    // -> MeshFilter_Set_Custom_PropMesh
    "UnityEngine.MeshFilter::get_sharedMesh",    // -> MeshFilter_Get_Custom_PropSharedMesh
    "UnityEngine.MeshFilter::set_sharedMesh",    // -> MeshFilter_Set_Custom_PropSharedMesh
    "UnityEngine.CombineInstance::InternalGetMesh",    // -> CombineInstance_CUSTOM_InternalGetMesh
    "UnityEngine.Mesh::Internal_Create"     ,    // -> Mesh_CUSTOM_Internal_Create
    "UnityEngine.Mesh::Clear"               ,    // -> Mesh_CUSTOM_Clear
    "UnityEngine.Mesh::get_isReadable"      ,    // -> Mesh_Get_Custom_PropIsReadable
    "UnityEngine.Mesh::get_canAccess"       ,    // -> Mesh_Get_Custom_PropCanAccess
    "UnityEngine.Mesh::get_vertices"        ,    // -> Mesh_Get_Custom_PropVertices
    "UnityEngine.Mesh::set_vertices"        ,    // -> Mesh_Set_Custom_PropVertices
    "UnityEngine.Mesh::get_normals"         ,    // -> Mesh_Get_Custom_PropNormals
    "UnityEngine.Mesh::set_normals"         ,    // -> Mesh_Set_Custom_PropNormals
    "UnityEngine.Mesh::get_tangents"        ,    // -> Mesh_Get_Custom_PropTangents
    "UnityEngine.Mesh::set_tangents"        ,    // -> Mesh_Set_Custom_PropTangents
    "UnityEngine.Mesh::get_uv"              ,    // -> Mesh_Get_Custom_PropUv
    "UnityEngine.Mesh::set_uv"              ,    // -> Mesh_Set_Custom_PropUv
    "UnityEngine.Mesh::get_uv2"             ,    // -> Mesh_Get_Custom_PropUv2
    "UnityEngine.Mesh::set_uv2"             ,    // -> Mesh_Set_Custom_PropUv2
    "UnityEngine.Mesh::INTERNAL_get_bounds" ,    // -> Mesh_CUSTOM_INTERNAL_get_bounds
    "UnityEngine.Mesh::INTERNAL_set_bounds" ,    // -> Mesh_CUSTOM_INTERNAL_set_bounds
    "UnityEngine.Mesh::get_colors"          ,    // -> Mesh_Get_Custom_PropColors
    "UnityEngine.Mesh::set_colors"          ,    // -> Mesh_Set_Custom_PropColors
    "UnityEngine.Mesh::get_colors32"        ,    // -> Mesh_Get_Custom_PropColors32
    "UnityEngine.Mesh::set_colors32"        ,    // -> Mesh_Set_Custom_PropColors32
    "UnityEngine.Mesh::RecalculateBounds"   ,    // -> Mesh_CUSTOM_RecalculateBounds
    "UnityEngine.Mesh::RecalculateNormals"  ,    // -> Mesh_CUSTOM_RecalculateNormals
    "UnityEngine.Mesh::Optimize"            ,    // -> Mesh_CUSTOM_Optimize
    "UnityEngine.Mesh::get_triangles"       ,    // -> Mesh_Get_Custom_PropTriangles
    "UnityEngine.Mesh::set_triangles"       ,    // -> Mesh_Set_Custom_PropTriangles
    "UnityEngine.Mesh::GetTriangles"        ,    // -> Mesh_CUSTOM_GetTriangles
    "UnityEngine.Mesh::SetTriangles"        ,    // -> Mesh_CUSTOM_SetTriangles
    "UnityEngine.Mesh::GetIndices"          ,    // -> Mesh_CUSTOM_GetIndices
    "UnityEngine.Mesh::SetIndices"          ,    // -> Mesh_CUSTOM_SetIndices
    "UnityEngine.Mesh::GetTopology"         ,    // -> Mesh_CUSTOM_GetTopology
    "UnityEngine.Mesh::get_vertexCount"     ,    // -> Mesh_Get_Custom_PropVertexCount
    "UnityEngine.Mesh::get_subMeshCount"    ,    // -> Mesh_Get_Custom_PropSubMeshCount
    "UnityEngine.Mesh::set_subMeshCount"    ,    // -> Mesh_Set_Custom_PropSubMeshCount
    "UnityEngine.Mesh::SetTriangleStrip"    ,    // -> Mesh_CUSTOM_SetTriangleStrip
    "UnityEngine.Mesh::GetTriangleStrip"    ,    // -> Mesh_CUSTOM_GetTriangleStrip
    "UnityEngine.Mesh::CombineMeshes"       ,    // -> Mesh_CUSTOM_CombineMeshes
    "UnityEngine.Mesh::get_boneWeights"     ,    // -> Mesh_Get_Custom_PropBoneWeights
    "UnityEngine.Mesh::set_boneWeights"     ,    // -> Mesh_Set_Custom_PropBoneWeights
    "UnityEngine.Mesh::get_bindposes"       ,    // -> Mesh_Get_Custom_PropBindposes
    "UnityEngine.Mesh::set_bindposes"       ,    // -> Mesh_Set_Custom_PropBindposes
    "UnityEngine.Mesh::MarkDynamic"         ,    // -> Mesh_CUSTOM_MarkDynamic
#if USE_BLENDSHAPES
    "UnityEngine.Mesh::get_blendShapes"     ,    // -> Mesh_Get_Custom_PropBlendShapes
    "UnityEngine.Mesh::set_blendShapes"     ,    // -> Mesh_Set_Custom_PropBlendShapes
    "UnityEngine.Mesh::get_blendShapeCount" ,    // -> Mesh_Get_Custom_PropBlendShapeCount
    "UnityEngine.Mesh::GetBlendShapeName"   ,    // -> Mesh_CUSTOM_GetBlendShapeName
#endif
    "UnityEngine.SkinnedMeshRenderer::get_bones",    // -> SkinnedMeshRenderer_Get_Custom_PropBones
    "UnityEngine.SkinnedMeshRenderer::set_bones",    // -> SkinnedMeshRenderer_Set_Custom_PropBones
    "UnityEngine.SkinnedMeshRenderer::get_rootBone",    // -> SkinnedMeshRenderer_Get_Custom_PropRootBone
    "UnityEngine.SkinnedMeshRenderer::set_rootBone",    // -> SkinnedMeshRenderer_Set_Custom_PropRootBone
    "UnityEngine.SkinnedMeshRenderer::get_quality",    // -> SkinnedMeshRenderer_Get_Custom_PropQuality
    "UnityEngine.SkinnedMeshRenderer::set_quality",    // -> SkinnedMeshRenderer_Set_Custom_PropQuality
    "UnityEngine.SkinnedMeshRenderer::get_sharedMesh",    // -> SkinnedMeshRenderer_Get_Custom_PropSharedMesh
    "UnityEngine.SkinnedMeshRenderer::set_sharedMesh",    // -> SkinnedMeshRenderer_Set_Custom_PropSharedMesh
    "UnityEngine.SkinnedMeshRenderer::get_updateWhenOffscreen",    // -> SkinnedMeshRenderer_Get_Custom_PropUpdateWhenOffscreen
    "UnityEngine.SkinnedMeshRenderer::set_updateWhenOffscreen",    // -> SkinnedMeshRenderer_Set_Custom_PropUpdateWhenOffscreen
    "UnityEngine.SkinnedMeshRenderer::INTERNAL_get_localBounds",    // -> SkinnedMeshRenderer_CUSTOM_INTERNAL_get_localBounds
    "UnityEngine.SkinnedMeshRenderer::INTERNAL_set_localBounds",    // -> SkinnedMeshRenderer_CUSTOM_INTERNAL_set_localBounds
    "UnityEngine.SkinnedMeshRenderer::BakeMesh",    // -> SkinnedMeshRenderer_CUSTOM_BakeMesh
#if UNITY_EDITOR
    "UnityEngine.SkinnedMeshRenderer::get_actualRootBone",    // -> SkinnedMeshRenderer_Get_Custom_PropActualRootBone
#endif
#if USE_BLENDSHAPES
    "UnityEngine.SkinnedMeshRenderer::GetBlendShapeWeight",    // -> SkinnedMeshRenderer_CUSTOM_GetBlendShapeWeight
    "UnityEngine.SkinnedMeshRenderer::SetBlendShapeWeight",    // -> SkinnedMeshRenderer_CUSTOM_SetBlendShapeWeight
#endif
    "UnityEngine.Material::get_shader"      ,    // -> Material_Get_Custom_PropShader
    "UnityEngine.Material::set_shader"      ,    // -> Material_Set_Custom_PropShader
    "UnityEngine.Material::INTERNAL_CALL_SetColor",    // -> Material_CUSTOM_INTERNAL_CALL_SetColor
    "UnityEngine.Material::INTERNAL_CALL_GetColor",    // -> Material_CUSTOM_INTERNAL_CALL_GetColor
    "UnityEngine.Material::SetTexture"      ,    // -> Material_CUSTOM_SetTexture
    "UnityEngine.Material::GetTexture"      ,    // -> Material_CUSTOM_GetTexture
    "UnityEngine.Material::Internal_GetTextureOffset",    // -> Material_CUSTOM_Internal_GetTextureOffset
    "UnityEngine.Material::Internal_GetTextureScale",    // -> Material_CUSTOM_Internal_GetTextureScale
    "UnityEngine.Material::INTERNAL_CALL_SetTextureOffset",    // -> Material_CUSTOM_INTERNAL_CALL_SetTextureOffset
    "UnityEngine.Material::INTERNAL_CALL_SetTextureScale",    // -> Material_CUSTOM_INTERNAL_CALL_SetTextureScale
    "UnityEngine.Material::INTERNAL_CALL_SetMatrix",    // -> Material_CUSTOM_INTERNAL_CALL_SetMatrix
    "UnityEngine.Material::INTERNAL_CALL_GetMatrix",    // -> Material_CUSTOM_INTERNAL_CALL_GetMatrix
    "UnityEngine.Material::SetFloat"        ,    // -> Material_CUSTOM_SetFloat
    "UnityEngine.Material::GetFloat"        ,    // -> Material_CUSTOM_GetFloat
#if !UNITY_FLASH
    "UnityEngine.Material::SetBuffer"       ,    // -> Material_CUSTOM_SetBuffer
#endif
    "UnityEngine.Material::HasProperty"     ,    // -> Material_CUSTOM_HasProperty
    "UnityEngine.Material::GetTag"          ,    // -> Material_CUSTOM_GetTag
    "UnityEngine.Material::Lerp"            ,    // -> Material_CUSTOM_Lerp
    "UnityEngine.Material::get_passCount"   ,    // -> Material_Get_Custom_PropPassCount
    "UnityEngine.Material::SetPass"         ,    // -> Material_CUSTOM_SetPass
    "UnityEngine.Material::get_renderQueue" ,    // -> Material_Get_Custom_PropRenderQueue
    "UnityEngine.Material::set_renderQueue" ,    // -> Material_Set_Custom_PropRenderQueue
    "UnityEngine.Material::Internal_CreateWithString",    // -> Material_CUSTOM_Internal_CreateWithString
    "UnityEngine.Material::Internal_CreateWithShader",    // -> Material_CUSTOM_Internal_CreateWithShader
    "UnityEngine.Material::Internal_CreateWithMaterial",    // -> Material_CUSTOM_Internal_CreateWithMaterial
    "UnityEngine.Material::CopyPropertiesFromMaterial",    // -> Material_CUSTOM_CopyPropertiesFromMaterial
#if ENABLE_MONO || PLATFORM_WINRT
    "UnityEngine.Material::get_shaderKeywords",    // -> Material_Get_Custom_PropShaderKeywords
    "UnityEngine.Material::set_shaderKeywords",    // -> Material_Set_Custom_PropShaderKeywords
#endif
    "UnityEngine.LensFlare::get_flare"      ,    // -> LensFlare_Get_Custom_PropFlare
    "UnityEngine.LensFlare::set_flare"      ,    // -> LensFlare_Set_Custom_PropFlare
    "UnityEngine.LensFlare::get_brightness" ,    // -> LensFlare_Get_Custom_PropBrightness
    "UnityEngine.LensFlare::set_brightness" ,    // -> LensFlare_Set_Custom_PropBrightness
    "UnityEngine.LensFlare::INTERNAL_get_color",    // -> LensFlare_CUSTOM_INTERNAL_get_color
    "UnityEngine.LensFlare::INTERNAL_set_color",    // -> LensFlare_CUSTOM_INTERNAL_set_color
    "UnityEngine.Renderer::get_staticBatchRootTransform",    // -> Renderer_Get_Custom_PropStaticBatchRootTransform
    "UnityEngine.Renderer::set_staticBatchRootTransform",    // -> Renderer_Set_Custom_PropStaticBatchRootTransform
    "UnityEngine.Renderer::get_staticBatchIndex",    // -> Renderer_Get_Custom_PropStaticBatchIndex
    "UnityEngine.Renderer::SetSubsetIndex"  ,    // -> Renderer_CUSTOM_SetSubsetIndex
    "UnityEngine.Renderer::get_isPartOfStaticBatch",    // -> Renderer_Get_Custom_PropIsPartOfStaticBatch
    "UnityEngine.Renderer::INTERNAL_get_worldToLocalMatrix",    // -> Renderer_CUSTOM_INTERNAL_get_worldToLocalMatrix
    "UnityEngine.Renderer::INTERNAL_get_localToWorldMatrix",    // -> Renderer_CUSTOM_INTERNAL_get_localToWorldMatrix
    "UnityEngine.Renderer::get_enabled"     ,    // -> Renderer_Get_Custom_PropEnabled
    "UnityEngine.Renderer::set_enabled"     ,    // -> Renderer_Set_Custom_PropEnabled
    "UnityEngine.Renderer::get_castShadows" ,    // -> Renderer_Get_Custom_PropCastShadows
    "UnityEngine.Renderer::set_castShadows" ,    // -> Renderer_Set_Custom_PropCastShadows
    "UnityEngine.Renderer::get_receiveShadows",    // -> Renderer_Get_Custom_PropReceiveShadows
    "UnityEngine.Renderer::set_receiveShadows",    // -> Renderer_Set_Custom_PropReceiveShadows
    "UnityEngine.Renderer::get_material"    ,    // -> Renderer_Get_Custom_PropMaterial
    "UnityEngine.Renderer::set_material"    ,    // -> Renderer_Set_Custom_PropMaterial
    "UnityEngine.Renderer::get_sharedMaterial",    // -> Renderer_Get_Custom_PropSharedMaterial
    "UnityEngine.Renderer::set_sharedMaterial",    // -> Renderer_Set_Custom_PropSharedMaterial
    "UnityEngine.Renderer::get_sharedMaterials",    // -> Renderer_Get_Custom_PropSharedMaterials
    "UnityEngine.Renderer::set_sharedMaterials",    // -> Renderer_Set_Custom_PropSharedMaterials
    "UnityEngine.Renderer::get_materials"   ,    // -> Renderer_Get_Custom_PropMaterials
    "UnityEngine.Renderer::set_materials"   ,    // -> Renderer_Set_Custom_PropMaterials
    "UnityEngine.Renderer::INTERNAL_get_bounds",    // -> Renderer_CUSTOM_INTERNAL_get_bounds
    "UnityEngine.Renderer::get_lightmapIndex",    // -> Renderer_Get_Custom_PropLightmapIndex
    "UnityEngine.Renderer::set_lightmapIndex",    // -> Renderer_Set_Custom_PropLightmapIndex
    "UnityEngine.Renderer::INTERNAL_get_lightmapTilingOffset",    // -> Renderer_CUSTOM_INTERNAL_get_lightmapTilingOffset
    "UnityEngine.Renderer::INTERNAL_set_lightmapTilingOffset",    // -> Renderer_CUSTOM_INTERNAL_set_lightmapTilingOffset
    "UnityEngine.Renderer::get_isVisible"   ,    // -> Renderer_Get_Custom_PropIsVisible
    "UnityEngine.Renderer::get_useLightProbes",    // -> Renderer_Get_Custom_PropUseLightProbes
    "UnityEngine.Renderer::set_useLightProbes",    // -> Renderer_Set_Custom_PropUseLightProbes
    "UnityEngine.Renderer::get_lightProbeAnchor",    // -> Renderer_Get_Custom_PropLightProbeAnchor
    "UnityEngine.Renderer::set_lightProbeAnchor",    // -> Renderer_Set_Custom_PropLightProbeAnchor
    "UnityEngine.Renderer::SetPropertyBlock",    // -> Renderer_CUSTOM_SetPropertyBlock
    "UnityEngine.Renderer::Render"          ,    // -> Renderer_CUSTOM_Render
    "UnityEngine.Projector::get_nearClipPlane",    // -> Projector_Get_Custom_PropNearClipPlane
    "UnityEngine.Projector::set_nearClipPlane",    // -> Projector_Set_Custom_PropNearClipPlane
    "UnityEngine.Projector::get_farClipPlane",    // -> Projector_Get_Custom_PropFarClipPlane
    "UnityEngine.Projector::set_farClipPlane",    // -> Projector_Set_Custom_PropFarClipPlane
    "UnityEngine.Projector::get_fieldOfView",    // -> Projector_Get_Custom_PropFieldOfView
    "UnityEngine.Projector::set_fieldOfView",    // -> Projector_Set_Custom_PropFieldOfView
    "UnityEngine.Projector::get_aspectRatio",    // -> Projector_Get_Custom_PropAspectRatio
    "UnityEngine.Projector::set_aspectRatio",    // -> Projector_Set_Custom_PropAspectRatio
    "UnityEngine.Projector::get_orthographic",    // -> Projector_Get_Custom_PropOrthographic
    "UnityEngine.Projector::set_orthographic",    // -> Projector_Set_Custom_PropOrthographic
    "UnityEngine.Projector::get_orthographicSize",    // -> Projector_Get_Custom_PropOrthographicSize
    "UnityEngine.Projector::set_orthographicSize",    // -> Projector_Set_Custom_PropOrthographicSize
    "UnityEngine.Projector::get_ignoreLayers",    // -> Projector_Get_Custom_PropIgnoreLayers
    "UnityEngine.Projector::set_ignoreLayers",    // -> Projector_Set_Custom_PropIgnoreLayers
    "UnityEngine.Projector::get_material"   ,    // -> Projector_Get_Custom_PropMaterial
    "UnityEngine.Projector::set_material"   ,    // -> Projector_Set_Custom_PropMaterial
    "UnityEngine.Skybox::get_material"      ,    // -> Skybox_Get_Custom_PropMaterial
    "UnityEngine.Skybox::set_material"      ,    // -> Skybox_Set_Custom_PropMaterial
    "UnityEngine.TextMesh::get_text"        ,    // -> TextMesh_Get_Custom_PropText
    "UnityEngine.TextMesh::set_text"        ,    // -> TextMesh_Set_Custom_PropText
    "UnityEngine.TextMesh::get_font"        ,    // -> TextMesh_Get_Custom_PropFont
    "UnityEngine.TextMesh::set_font"        ,    // -> TextMesh_Set_Custom_PropFont
    "UnityEngine.TextMesh::get_fontSize"    ,    // -> TextMesh_Get_Custom_PropFontSize
    "UnityEngine.TextMesh::set_fontSize"    ,    // -> TextMesh_Set_Custom_PropFontSize
    "UnityEngine.TextMesh::get_fontStyle"   ,    // -> TextMesh_Get_Custom_PropFontStyle
    "UnityEngine.TextMesh::set_fontStyle"   ,    // -> TextMesh_Set_Custom_PropFontStyle
    "UnityEngine.TextMesh::get_offsetZ"     ,    // -> TextMesh_Get_Custom_PropOffsetZ
    "UnityEngine.TextMesh::set_offsetZ"     ,    // -> TextMesh_Set_Custom_PropOffsetZ
    "UnityEngine.TextMesh::get_alignment"   ,    // -> TextMesh_Get_Custom_PropAlignment
    "UnityEngine.TextMesh::set_alignment"   ,    // -> TextMesh_Set_Custom_PropAlignment
    "UnityEngine.TextMesh::get_anchor"      ,    // -> TextMesh_Get_Custom_PropAnchor
    "UnityEngine.TextMesh::set_anchor"      ,    // -> TextMesh_Set_Custom_PropAnchor
    "UnityEngine.TextMesh::get_characterSize",    // -> TextMesh_Get_Custom_PropCharacterSize
    "UnityEngine.TextMesh::set_characterSize",    // -> TextMesh_Set_Custom_PropCharacterSize
    "UnityEngine.TextMesh::get_lineSpacing" ,    // -> TextMesh_Get_Custom_PropLineSpacing
    "UnityEngine.TextMesh::set_lineSpacing" ,    // -> TextMesh_Set_Custom_PropLineSpacing
    "UnityEngine.TextMesh::get_tabSize"     ,    // -> TextMesh_Get_Custom_PropTabSize
    "UnityEngine.TextMesh::set_tabSize"     ,    // -> TextMesh_Set_Custom_PropTabSize
    "UnityEngine.TextMesh::get_richText"    ,    // -> TextMesh_Get_Custom_PropRichText
    "UnityEngine.TextMesh::set_richText"    ,    // -> TextMesh_Set_Custom_PropRichText
    "UnityEngine.TextMesh::INTERNAL_get_color",    // -> TextMesh_CUSTOM_INTERNAL_get_color
    "UnityEngine.TextMesh::INTERNAL_set_color",    // -> TextMesh_CUSTOM_INTERNAL_set_color
    "UnityEngine.TrailRenderer::get_time"   ,    // -> TrailRenderer_Get_Custom_PropTime
    "UnityEngine.TrailRenderer::set_time"   ,    // -> TrailRenderer_Set_Custom_PropTime
    "UnityEngine.TrailRenderer::get_startWidth",    // -> TrailRenderer_Get_Custom_PropStartWidth
    "UnityEngine.TrailRenderer::set_startWidth",    // -> TrailRenderer_Set_Custom_PropStartWidth
    "UnityEngine.TrailRenderer::get_endWidth",    // -> TrailRenderer_Get_Custom_PropEndWidth
    "UnityEngine.TrailRenderer::set_endWidth",    // -> TrailRenderer_Set_Custom_PropEndWidth
    "UnityEngine.TrailRenderer::get_autodestruct",    // -> TrailRenderer_Get_Custom_PropAutodestruct
    "UnityEngine.TrailRenderer::set_autodestruct",    // -> TrailRenderer_Set_Custom_PropAutodestruct
    "UnityEngine.LineRenderer::INTERNAL_CALL_SetWidth",    // -> LineRenderer_CUSTOM_INTERNAL_CALL_SetWidth
    "UnityEngine.LineRenderer::INTERNAL_CALL_SetColors",    // -> LineRenderer_CUSTOM_INTERNAL_CALL_SetColors
    "UnityEngine.LineRenderer::INTERNAL_CALL_SetVertexCount",    // -> LineRenderer_CUSTOM_INTERNAL_CALL_SetVertexCount
    "UnityEngine.LineRenderer::INTERNAL_CALL_SetPosition",    // -> LineRenderer_CUSTOM_INTERNAL_CALL_SetPosition
    "UnityEngine.LineRenderer::get_useWorldSpace",    // -> LineRenderer_Get_Custom_PropUseWorldSpace
    "UnityEngine.LineRenderer::set_useWorldSpace",    // -> LineRenderer_Set_Custom_PropUseWorldSpace
    "UnityEngine.MaterialPropertyBlock::InitBlock",    // -> MaterialPropertyBlock_CUSTOM_InitBlock
    "UnityEngine.MaterialPropertyBlock::DestroyBlock",    // -> MaterialPropertyBlock_CUSTOM_DestroyBlock
    "UnityEngine.MaterialPropertyBlock::AddFloat",    // -> MaterialPropertyBlock_CUSTOM_AddFloat
    "UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddVector",    // -> MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddVector
    "UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddColor",    // -> MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddColor
    "UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddMatrix",    // -> MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddMatrix
    "UnityEngine.MaterialPropertyBlock::Clear",    // -> MaterialPropertyBlock_CUSTOM_Clear
    "UnityEngine.Graphics::Internal_DrawMeshTR",    // -> Graphics_CUSTOM_Internal_DrawMeshTR
    "UnityEngine.Graphics::Internal_DrawMeshMatrix",    // -> Graphics_CUSTOM_Internal_DrawMeshMatrix
    "UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow1",    // -> Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow1
    "UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow2",    // -> Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow2
    "UnityEngine.Graphics::DrawProcedural"  ,    // -> Graphics_CUSTOM_DrawProcedural
#if !UNITY_FLASH
    "UnityEngine.Graphics::DrawProceduralIndirect",    // -> Graphics_CUSTOM_DrawProceduralIndirect
#endif
    "UnityEngine.Graphics::DrawTexture"     ,    // -> Graphics_CUSTOM_DrawTexture
    "UnityEngine.Graphics::Blit"            ,    // -> Graphics_CUSTOM_Blit
    "UnityEngine.Graphics::Internal_BlitMaterial",    // -> Graphics_CUSTOM_Internal_BlitMaterial
    "UnityEngine.Graphics::Internal_BlitMultiTap",    // -> Graphics_CUSTOM_Internal_BlitMultiTap
    "UnityEngine.Graphics::Internal_SetRT"  ,    // -> Graphics_CUSTOM_Internal_SetRT
    "UnityEngine.Graphics::Internal_SetRTBuffer",    // -> Graphics_CUSTOM_Internal_SetRTBuffer
    "UnityEngine.Graphics::Internal_SetRTBuffers",    // -> Graphics_CUSTOM_Internal_SetRTBuffers
    "UnityEngine.Graphics::GetActiveColorBuffer",    // -> Graphics_CUSTOM_GetActiveColorBuffer
    "UnityEngine.Graphics::GetActiveDepthBuffer",    // -> Graphics_CUSTOM_GetActiveDepthBuffer
    "UnityEngine.Graphics::ClearRandomWriteTargets",    // -> Graphics_CUSTOM_ClearRandomWriteTargets
    "UnityEngine.Graphics::Internal_SetRandomWriteTargetRT",    // -> Graphics_CUSTOM_Internal_SetRandomWriteTargetRT
#if !UNITY_FLASH
    "UnityEngine.Graphics::Internal_SetRandomWriteTargetBuffer",    // -> Graphics_CUSTOM_Internal_SetRandomWriteTargetBuffer
#endif
    "UnityEngine.Graphics::SetupVertexLights",    // -> Graphics_CUSTOM_SetupVertexLights
#if ENABLE_MONO || PLATFORM_WINRT
    "UnityEngine.LightProbes::INTERNAL_CALL_GetInterpolatedLightProbe",    // -> LightProbes_CUSTOM_INTERNAL_CALL_GetInterpolatedLightProbe
    "UnityEngine.LightProbes::get_positions",    // -> LightProbes_Get_Custom_PropPositions
    "UnityEngine.LightProbes::get_coefficients",    // -> LightProbes_Get_Custom_PropCoefficients
    "UnityEngine.LightProbes::set_coefficients",    // -> LightProbes_Set_Custom_PropCoefficients
    "UnityEngine.LightProbes::get_count"    ,    // -> LightProbes_Get_Custom_PropCount
    "UnityEngine.LightProbes::get_cellCount",    // -> LightProbes_Get_Custom_PropCellCount
    "UnityEngine.LightmapSettings::get_lightmaps",    // -> LightmapSettings_Get_Custom_PropLightmaps
    "UnityEngine.LightmapSettings::set_lightmaps",    // -> LightmapSettings_Set_Custom_PropLightmaps
    "UnityEngine.LightmapSettings::get_lightmapsMode",    // -> LightmapSettings_Get_Custom_PropLightmapsMode
    "UnityEngine.LightmapSettings::set_lightmapsMode",    // -> LightmapSettings_Set_Custom_PropLightmapsMode
    "UnityEngine.LightmapSettings::get_bakedColorSpace",    // -> LightmapSettings_Get_Custom_PropBakedColorSpace
    "UnityEngine.LightmapSettings::set_bakedColorSpace",    // -> LightmapSettings_Set_Custom_PropBakedColorSpace
    "UnityEngine.LightmapSettings::get_lightProbes",    // -> LightmapSettings_Get_Custom_PropLightProbes
    "UnityEngine.LightmapSettings::set_lightProbes",    // -> LightmapSettings_Set_Custom_PropLightProbes
#endif
    "UnityEngine.GeometryUtility::INTERNAL_CALL_Internal_ExtractPlanes",    // -> GeometryUtility_CUSTOM_INTERNAL_CALL_Internal_ExtractPlanes
    "UnityEngine.GeometryUtility::INTERNAL_CALL_TestPlanesAABB",    // -> GeometryUtility_CUSTOM_INTERNAL_CALL_TestPlanesAABB
    "UnityEngine.Screen::get_resolutions"   ,    // -> Screen_Get_Custom_PropResolutions
    "UnityEngine.Screen::get_currentResolution",    // -> Screen_Get_Custom_PropCurrentResolution
    "UnityEngine.Screen::SetResolution"     ,    // -> Screen_CUSTOM_SetResolution
    "UnityEngine.Screen::get_showCursor"    ,    // -> Screen_Get_Custom_PropShowCursor
    "UnityEngine.Screen::set_showCursor"    ,    // -> Screen_Set_Custom_PropShowCursor
    "UnityEngine.Screen::get_lockCursor"    ,    // -> Screen_Get_Custom_PropLockCursor
    "UnityEngine.Screen::set_lockCursor"    ,    // -> Screen_Set_Custom_PropLockCursor
    "UnityEngine.Screen::get_width"         ,    // -> Screen_Get_Custom_PropWidth
    "UnityEngine.Screen::get_height"        ,    // -> Screen_Get_Custom_PropHeight
    "UnityEngine.Screen::get_dpi"           ,    // -> Screen_Get_Custom_PropDpi
    "UnityEngine.Screen::get_fullScreen"    ,    // -> Screen_Get_Custom_PropFullScreen
    "UnityEngine.Screen::set_fullScreen"    ,    // -> Screen_Set_Custom_PropFullScreen
    "UnityEngine.Screen::get_autorotateToPortrait",    // -> Screen_Get_Custom_PropAutorotateToPortrait
    "UnityEngine.Screen::set_autorotateToPortrait",    // -> Screen_Set_Custom_PropAutorotateToPortrait
    "UnityEngine.Screen::get_autorotateToPortraitUpsideDown",    // -> Screen_Get_Custom_PropAutorotateToPortraitUpsideDown
    "UnityEngine.Screen::set_autorotateToPortraitUpsideDown",    // -> Screen_Set_Custom_PropAutorotateToPortraitUpsideDown
    "UnityEngine.Screen::get_autorotateToLandscapeLeft",    // -> Screen_Get_Custom_PropAutorotateToLandscapeLeft
    "UnityEngine.Screen::set_autorotateToLandscapeLeft",    // -> Screen_Set_Custom_PropAutorotateToLandscapeLeft
    "UnityEngine.Screen::get_autorotateToLandscapeRight",    // -> Screen_Get_Custom_PropAutorotateToLandscapeRight
    "UnityEngine.Screen::set_autorotateToLandscapeRight",    // -> Screen_Set_Custom_PropAutorotateToLandscapeRight
    "UnityEngine.Screen::get_orientation"   ,    // -> Screen_Get_Custom_PropOrientation
    "UnityEngine.Screen::set_orientation"   ,    // -> Screen_Set_Custom_PropOrientation
    "UnityEngine.Screen::get_sleepTimeout"  ,    // -> Screen_Get_Custom_PropSleepTimeout
    "UnityEngine.Screen::set_sleepTimeout"  ,    // -> Screen_Set_Custom_PropSleepTimeout
    "UnityEngine.RenderTexture::Internal_CreateRenderTexture",    // -> RenderTexture_CUSTOM_Internal_CreateRenderTexture
    "UnityEngine.RenderTexture::GetTemporary",    // -> RenderTexture_CUSTOM_GetTemporary
    "UnityEngine.RenderTexture::ReleaseTemporary",    // -> RenderTexture_CUSTOM_ReleaseTemporary
    "UnityEngine.RenderTexture::Internal_GetWidth",    // -> RenderTexture_CUSTOM_Internal_GetWidth
    "UnityEngine.RenderTexture::Internal_SetWidth",    // -> RenderTexture_CUSTOM_Internal_SetWidth
    "UnityEngine.RenderTexture::Internal_GetHeight",    // -> RenderTexture_CUSTOM_Internal_GetHeight
    "UnityEngine.RenderTexture::Internal_SetHeight",    // -> RenderTexture_CUSTOM_Internal_SetHeight
    "UnityEngine.RenderTexture::Internal_SetSRGBReadWrite",    // -> RenderTexture_CUSTOM_Internal_SetSRGBReadWrite
    "UnityEngine.RenderTexture::get_depth"  ,    // -> RenderTexture_Get_Custom_PropDepth
    "UnityEngine.RenderTexture::set_depth"  ,    // -> RenderTexture_Set_Custom_PropDepth
    "UnityEngine.RenderTexture::get_isPowerOfTwo",    // -> RenderTexture_Get_Custom_PropIsPowerOfTwo
    "UnityEngine.RenderTexture::set_isPowerOfTwo",    // -> RenderTexture_Set_Custom_PropIsPowerOfTwo
    "UnityEngine.RenderTexture::get_sRGB"   ,    // -> RenderTexture_Get_Custom_PropSRGB
    "UnityEngine.RenderTexture::get_format" ,    // -> RenderTexture_Get_Custom_PropFormat
    "UnityEngine.RenderTexture::set_format" ,    // -> RenderTexture_Set_Custom_PropFormat
    "UnityEngine.RenderTexture::get_useMipMap",    // -> RenderTexture_Get_Custom_PropUseMipMap
    "UnityEngine.RenderTexture::set_useMipMap",    // -> RenderTexture_Set_Custom_PropUseMipMap
    "UnityEngine.RenderTexture::get_isCubemap",    // -> RenderTexture_Get_Custom_PropIsCubemap
    "UnityEngine.RenderTexture::set_isCubemap",    // -> RenderTexture_Set_Custom_PropIsCubemap
    "UnityEngine.RenderTexture::get_isVolume",    // -> RenderTexture_Get_Custom_PropIsVolume
    "UnityEngine.RenderTexture::set_isVolume",    // -> RenderTexture_Set_Custom_PropIsVolume
    "UnityEngine.RenderTexture::get_volumeDepth",    // -> RenderTexture_Get_Custom_PropVolumeDepth
    "UnityEngine.RenderTexture::set_volumeDepth",    // -> RenderTexture_Set_Custom_PropVolumeDepth
    "UnityEngine.RenderTexture::get_antiAliasing",    // -> RenderTexture_Get_Custom_PropAntiAliasing
    "UnityEngine.RenderTexture::set_antiAliasing",    // -> RenderTexture_Set_Custom_PropAntiAliasing
    "UnityEngine.RenderTexture::get_enableRandomWrite",    // -> RenderTexture_Get_Custom_PropEnableRandomWrite
    "UnityEngine.RenderTexture::set_enableRandomWrite",    // -> RenderTexture_Set_Custom_PropEnableRandomWrite
    "UnityEngine.RenderTexture::INTERNAL_CALL_Create",    // -> RenderTexture_CUSTOM_INTERNAL_CALL_Create
    "UnityEngine.RenderTexture::INTERNAL_CALL_Release",    // -> RenderTexture_CUSTOM_INTERNAL_CALL_Release
    "UnityEngine.RenderTexture::INTERNAL_CALL_IsCreated",    // -> RenderTexture_CUSTOM_INTERNAL_CALL_IsCreated
    "UnityEngine.RenderTexture::INTERNAL_CALL_DiscardContents",    // -> RenderTexture_CUSTOM_INTERNAL_CALL_DiscardContents
    "UnityEngine.RenderTexture::GetColorBuffer",    // -> RenderTexture_CUSTOM_GetColorBuffer
    "UnityEngine.RenderTexture::GetDepthBuffer",    // -> RenderTexture_CUSTOM_GetDepthBuffer
    "UnityEngine.RenderTexture::SetGlobalShaderProperty",    // -> RenderTexture_CUSTOM_SetGlobalShaderProperty
    "UnityEngine.RenderTexture::get_active" ,    // -> RenderTexture_Get_Custom_PropActive
    "UnityEngine.RenderTexture::set_active" ,    // -> RenderTexture_Set_Custom_PropActive
    "UnityEngine.RenderTexture::get_enabled",    // -> RenderTexture_Get_Custom_PropEnabled
    "UnityEngine.RenderTexture::set_enabled",    // -> RenderTexture_Set_Custom_PropEnabled
    "UnityEngine.RenderTexture::Internal_GetTexelOffset",    // -> RenderTexture_CUSTOM_Internal_GetTexelOffset
    "UnityEngine.RenderTexture::SupportsStencil",    // -> RenderTexture_CUSTOM_SupportsStencil
#if ENABLE_MOVIES
    "UnityEngine.MovieTexture::INTERNAL_CALL_Play",    // -> MovieTexture_CUSTOM_INTERNAL_CALL_Play
    "UnityEngine.MovieTexture::INTERNAL_CALL_Stop",    // -> MovieTexture_CUSTOM_INTERNAL_CALL_Stop
    "UnityEngine.MovieTexture::INTERNAL_CALL_Pause",    // -> MovieTexture_CUSTOM_INTERNAL_CALL_Pause
#endif
#if (ENABLE_MOVIES) && (ENABLE_AUDIO)
    "UnityEngine.MovieTexture::get_audioClip",    // -> MovieTexture_Get_Custom_PropAudioClip
#endif
#if ENABLE_MOVIES
    "UnityEngine.MovieTexture::get_loop"    ,    // -> MovieTexture_Get_Custom_PropLoop
    "UnityEngine.MovieTexture::set_loop"    ,    // -> MovieTexture_Set_Custom_PropLoop
    "UnityEngine.MovieTexture::get_isPlaying",    // -> MovieTexture_Get_Custom_PropIsPlaying
    "UnityEngine.MovieTexture::get_isReadyToPlay",    // -> MovieTexture_Get_Custom_PropIsReadyToPlay
    "UnityEngine.MovieTexture::get_duration",    // -> MovieTexture_Get_Custom_PropDuration
#endif
    "UnityEngine.GL::Vertex3"               ,    // -> GL_CUSTOM_Vertex3
    "UnityEngine.GL::INTERNAL_CALL_Vertex"  ,    // -> GL_CUSTOM_INTERNAL_CALL_Vertex
    "UnityEngine.GL::INTERNAL_CALL_Color"   ,    // -> GL_CUSTOM_INTERNAL_CALL_Color
    "UnityEngine.GL::INTERNAL_CALL_TexCoord",    // -> GL_CUSTOM_INTERNAL_CALL_TexCoord
    "UnityEngine.GL::TexCoord2"             ,    // -> GL_CUSTOM_TexCoord2
    "UnityEngine.GL::TexCoord3"             ,    // -> GL_CUSTOM_TexCoord3
    "UnityEngine.GL::MultiTexCoord2"        ,    // -> GL_CUSTOM_MultiTexCoord2
    "UnityEngine.GL::MultiTexCoord3"        ,    // -> GL_CUSTOM_MultiTexCoord3
    "UnityEngine.GL::INTERNAL_CALL_MultiTexCoord",    // -> GL_CUSTOM_INTERNAL_CALL_MultiTexCoord
    "UnityEngine.GL::Begin"                 ,    // -> GL_CUSTOM_Begin
    "UnityEngine.GL::End"                   ,    // -> GL_CUSTOM_End
    "UnityEngine.GL::LoadOrtho"             ,    // -> GL_CUSTOM_LoadOrtho
    "UnityEngine.GL::LoadPixelMatrix"       ,    // -> GL_CUSTOM_LoadPixelMatrix
    "UnityEngine.GL::LoadPixelMatrixArgs"   ,    // -> GL_CUSTOM_LoadPixelMatrixArgs
    "UnityEngine.GL::INTERNAL_CALL_Viewport",    // -> GL_CUSTOM_INTERNAL_CALL_Viewport
    "UnityEngine.GL::INTERNAL_CALL_LoadProjectionMatrix",    // -> GL_CUSTOM_INTERNAL_CALL_LoadProjectionMatrix
    "UnityEngine.GL::LoadIdentity"          ,    // -> GL_CUSTOM_LoadIdentity
    "UnityEngine.GL::INTERNAL_get_modelview",    // -> GL_CUSTOM_INTERNAL_get_modelview
    "UnityEngine.GL::INTERNAL_set_modelview",    // -> GL_CUSTOM_INTERNAL_set_modelview
    "UnityEngine.GL::INTERNAL_CALL_MultMatrix",    // -> GL_CUSTOM_INTERNAL_CALL_MultMatrix
    "UnityEngine.GL::PushMatrix"            ,    // -> GL_CUSTOM_PushMatrix
    "UnityEngine.GL::PopMatrix"             ,    // -> GL_CUSTOM_PopMatrix
    "UnityEngine.GL::INTERNAL_CALL_GetGPUProjectionMatrix",    // -> GL_CUSTOM_INTERNAL_CALL_GetGPUProjectionMatrix
    "UnityEngine.GL::get_wireframe"         ,    // -> GL_Get_Custom_PropWireframe
    "UnityEngine.GL::set_wireframe"         ,    // -> GL_Set_Custom_PropWireframe
    "UnityEngine.GL::SetRevertBackfacing"   ,    // -> GL_CUSTOM_SetRevertBackfacing
    "UnityEngine.GL::INTERNAL_CALL_Internal_Clear",    // -> GL_CUSTOM_INTERNAL_CALL_Internal_Clear
    "UnityEngine.GL::ClearWithSkybox"       ,    // -> GL_CUSTOM_ClearWithSkybox
    "UnityEngine.GL::InvalidateState"       ,    // -> GL_CUSTOM_InvalidateState
    "UnityEngine.GL::IssuePluginEvent"      ,    // -> GL_CUSTOM_IssuePluginEvent
    "UnityEngine.GUIElement::INTERNAL_CALL_HitTest",    // -> GUIElement_CUSTOM_INTERNAL_CALL_HitTest
    "UnityEngine.GUIElement::INTERNAL_CALL_GetScreenRect",    // -> GUIElement_CUSTOM_INTERNAL_CALL_GetScreenRect
    "UnityEngine.GUITexture::INTERNAL_get_color",    // -> GUITexture_CUSTOM_INTERNAL_get_color
    "UnityEngine.GUITexture::INTERNAL_set_color",    // -> GUITexture_CUSTOM_INTERNAL_set_color
    "UnityEngine.GUITexture::get_texture"   ,    // -> GUITexture_Get_Custom_PropTexture
    "UnityEngine.GUITexture::set_texture"   ,    // -> GUITexture_Set_Custom_PropTexture
    "UnityEngine.GUITexture::INTERNAL_get_pixelInset",    // -> GUITexture_CUSTOM_INTERNAL_get_pixelInset
    "UnityEngine.GUITexture::INTERNAL_set_pixelInset",    // -> GUITexture_CUSTOM_INTERNAL_set_pixelInset
    "UnityEngine.GUITexture::get_border"    ,    // -> GUITexture_Get_Custom_PropBorder
    "UnityEngine.GUITexture::set_border"    ,    // -> GUITexture_Set_Custom_PropBorder
    "UnityEngine.GUIText::get_text"         ,    // -> GUIText_Get_Custom_PropText
    "UnityEngine.GUIText::set_text"         ,    // -> GUIText_Set_Custom_PropText
    "UnityEngine.GUIText::get_material"     ,    // -> GUIText_Get_Custom_PropMaterial
    "UnityEngine.GUIText::set_material"     ,    // -> GUIText_Set_Custom_PropMaterial
    "UnityEngine.GUIText::Internal_GetPixelOffset",    // -> GUIText_CUSTOM_Internal_GetPixelOffset
    "UnityEngine.GUIText::INTERNAL_CALL_Internal_SetPixelOffset",    // -> GUIText_CUSTOM_INTERNAL_CALL_Internal_SetPixelOffset
    "UnityEngine.GUIText::get_font"         ,    // -> GUIText_Get_Custom_PropFont
    "UnityEngine.GUIText::set_font"         ,    // -> GUIText_Set_Custom_PropFont
    "UnityEngine.GUIText::get_alignment"    ,    // -> GUIText_Get_Custom_PropAlignment
    "UnityEngine.GUIText::set_alignment"    ,    // -> GUIText_Set_Custom_PropAlignment
    "UnityEngine.GUIText::get_anchor"       ,    // -> GUIText_Get_Custom_PropAnchor
    "UnityEngine.GUIText::set_anchor"       ,    // -> GUIText_Set_Custom_PropAnchor
    "UnityEngine.GUIText::get_lineSpacing"  ,    // -> GUIText_Get_Custom_PropLineSpacing
    "UnityEngine.GUIText::set_lineSpacing"  ,    // -> GUIText_Set_Custom_PropLineSpacing
    "UnityEngine.GUIText::get_tabSize"      ,    // -> GUIText_Get_Custom_PropTabSize
    "UnityEngine.GUIText::set_tabSize"      ,    // -> GUIText_Set_Custom_PropTabSize
    "UnityEngine.GUIText::get_fontSize"     ,    // -> GUIText_Get_Custom_PropFontSize
    "UnityEngine.GUIText::set_fontSize"     ,    // -> GUIText_Set_Custom_PropFontSize
    "UnityEngine.GUIText::get_fontStyle"    ,    // -> GUIText_Get_Custom_PropFontStyle
    "UnityEngine.GUIText::set_fontStyle"    ,    // -> GUIText_Set_Custom_PropFontStyle
    "UnityEngine.GUIText::get_richText"     ,    // -> GUIText_Get_Custom_PropRichText
    "UnityEngine.GUIText::set_richText"     ,    // -> GUIText_Set_Custom_PropRichText
    "UnityEngine.GUIText::INTERNAL_get_color",    // -> GUIText_CUSTOM_INTERNAL_get_color
    "UnityEngine.GUIText::INTERNAL_set_color",    // -> GUIText_CUSTOM_INTERNAL_set_color
    "UnityEngine.Font::Internal_CreateFont" ,    // -> Font_CUSTOM_Internal_CreateFont
    "UnityEngine.Font::get_material"        ,    // -> Font_Get_Custom_PropMaterial
    "UnityEngine.Font::set_material"        ,    // -> Font_Set_Custom_PropMaterial
    "UnityEngine.Font::HasCharacter"        ,    // -> Font_CUSTOM_HasCharacter
#if ENABLE_MONO
    "UnityEngine.Font::get_fontNames"       ,    // -> Font_Get_Custom_PropFontNames
    "UnityEngine.Font::set_fontNames"       ,    // -> Font_Set_Custom_PropFontNames
#endif
    "UnityEngine.Font::get_characterInfo"   ,    // -> Font_Get_Custom_PropCharacterInfo
    "UnityEngine.Font::set_characterInfo"   ,    // -> Font_Set_Custom_PropCharacterInfo
    "UnityEngine.Font::RequestCharactersInTexture",    // -> Font_CUSTOM_RequestCharactersInTexture
    "UnityEngine.Font::GetCharacterInfo"    ,    // -> Font_CUSTOM_GetCharacterInfo
    "UnityEngine.GUILayer::INTERNAL_CALL_HitTest",    // -> GUILayer_CUSTOM_INTERNAL_CALL_HitTest
#if !PLATFORM_WEBGL && !UNITY_FLASH
    "UnityEngine.StaticBatchingUtility::InternalCombineVertices",    // -> StaticBatchingUtility_CUSTOM_InternalCombineVertices
    "UnityEngine.StaticBatchingUtility::InternalCombineIndices",    // -> StaticBatchingUtility_CUSTOM_InternalCombineIndices
#endif
    NULL
};

static const void* s_Graphics_IcallFuncs [] =
{
    (const void*)&OcclusionArea_CUSTOM_INTERNAL_get_center,  //  <- UnityEngine.OcclusionArea::INTERNAL_get_center
    (const void*)&OcclusionArea_CUSTOM_INTERNAL_set_center,  //  <- UnityEngine.OcclusionArea::INTERNAL_set_center
    (const void*)&OcclusionArea_CUSTOM_INTERNAL_get_size  ,  //  <- UnityEngine.OcclusionArea::INTERNAL_get_size
    (const void*)&OcclusionArea_CUSTOM_INTERNAL_set_size  ,  //  <- UnityEngine.OcclusionArea::INTERNAL_set_size
    (const void*)&OcclusionPortal_Get_Custom_PropOpen     ,  //  <- UnityEngine.OcclusionPortal::get_open
    (const void*)&OcclusionPortal_Set_Custom_PropOpen     ,  //  <- UnityEngine.OcclusionPortal::set_open
    (const void*)&RenderSettings_Get_Custom_PropFog       ,  //  <- UnityEngine.RenderSettings::get_fog
    (const void*)&RenderSettings_Set_Custom_PropFog       ,  //  <- UnityEngine.RenderSettings::set_fog
    (const void*)&RenderSettings_Get_Custom_PropFogMode   ,  //  <- UnityEngine.RenderSettings::get_fogMode
    (const void*)&RenderSettings_Set_Custom_PropFogMode   ,  //  <- UnityEngine.RenderSettings::set_fogMode
    (const void*)&RenderSettings_CUSTOM_INTERNAL_get_fogColor,  //  <- UnityEngine.RenderSettings::INTERNAL_get_fogColor
    (const void*)&RenderSettings_CUSTOM_INTERNAL_set_fogColor,  //  <- UnityEngine.RenderSettings::INTERNAL_set_fogColor
    (const void*)&RenderSettings_Get_Custom_PropFogDensity,  //  <- UnityEngine.RenderSettings::get_fogDensity
    (const void*)&RenderSettings_Set_Custom_PropFogDensity,  //  <- UnityEngine.RenderSettings::set_fogDensity
    (const void*)&RenderSettings_Get_Custom_PropFogStartDistance,  //  <- UnityEngine.RenderSettings::get_fogStartDistance
    (const void*)&RenderSettings_Set_Custom_PropFogStartDistance,  //  <- UnityEngine.RenderSettings::set_fogStartDistance
    (const void*)&RenderSettings_Get_Custom_PropFogEndDistance,  //  <- UnityEngine.RenderSettings::get_fogEndDistance
    (const void*)&RenderSettings_Set_Custom_PropFogEndDistance,  //  <- UnityEngine.RenderSettings::set_fogEndDistance
    (const void*)&RenderSettings_CUSTOM_INTERNAL_get_ambientLight,  //  <- UnityEngine.RenderSettings::INTERNAL_get_ambientLight
    (const void*)&RenderSettings_CUSTOM_INTERNAL_set_ambientLight,  //  <- UnityEngine.RenderSettings::INTERNAL_set_ambientLight
    (const void*)&RenderSettings_Get_Custom_PropHaloStrength,  //  <- UnityEngine.RenderSettings::get_haloStrength
    (const void*)&RenderSettings_Set_Custom_PropHaloStrength,  //  <- UnityEngine.RenderSettings::set_haloStrength
    (const void*)&RenderSettings_Get_Custom_PropFlareStrength,  //  <- UnityEngine.RenderSettings::get_flareStrength
    (const void*)&RenderSettings_Set_Custom_PropFlareStrength,  //  <- UnityEngine.RenderSettings::set_flareStrength
    (const void*)&RenderSettings_Get_Custom_PropSkybox    ,  //  <- UnityEngine.RenderSettings::get_skybox
    (const void*)&RenderSettings_Set_Custom_PropSkybox    ,  //  <- UnityEngine.RenderSettings::set_skybox
#if ENABLE_MONO || PLATFORM_WINRT
    (const void*)&QualitySettings_Get_Custom_PropNames    ,  //  <- UnityEngine.QualitySettings::get_names
#endif
    (const void*)&QualitySettings_CUSTOM_GetQualityLevel  ,  //  <- UnityEngine.QualitySettings::GetQualityLevel
    (const void*)&QualitySettings_CUSTOM_SetQualityLevel  ,  //  <- UnityEngine.QualitySettings::SetQualityLevel
    (const void*)&QualitySettings_Get_Custom_PropCurrentLevel,  //  <- UnityEngine.QualitySettings::get_currentLevel
    (const void*)&QualitySettings_Set_Custom_PropCurrentLevel,  //  <- UnityEngine.QualitySettings::set_currentLevel
    (const void*)&QualitySettings_CUSTOM_IncreaseLevel    ,  //  <- UnityEngine.QualitySettings::IncreaseLevel
    (const void*)&QualitySettings_CUSTOM_DecreaseLevel    ,  //  <- UnityEngine.QualitySettings::DecreaseLevel
    (const void*)&QualitySettings_Get_Custom_PropPixelLightCount,  //  <- UnityEngine.QualitySettings::get_pixelLightCount
    (const void*)&QualitySettings_Set_Custom_PropPixelLightCount,  //  <- UnityEngine.QualitySettings::set_pixelLightCount
    (const void*)&QualitySettings_Get_Custom_PropShadowProjection,  //  <- UnityEngine.QualitySettings::get_shadowProjection
    (const void*)&QualitySettings_Set_Custom_PropShadowProjection,  //  <- UnityEngine.QualitySettings::set_shadowProjection
    (const void*)&QualitySettings_Get_Custom_PropShadowCascades,  //  <- UnityEngine.QualitySettings::get_shadowCascades
    (const void*)&QualitySettings_Set_Custom_PropShadowCascades,  //  <- UnityEngine.QualitySettings::set_shadowCascades
    (const void*)&QualitySettings_Get_Custom_PropShadowDistance,  //  <- UnityEngine.QualitySettings::get_shadowDistance
    (const void*)&QualitySettings_Set_Custom_PropShadowDistance,  //  <- UnityEngine.QualitySettings::set_shadowDistance
    (const void*)&QualitySettings_Get_Custom_PropMasterTextureLimit,  //  <- UnityEngine.QualitySettings::get_masterTextureLimit
    (const void*)&QualitySettings_Set_Custom_PropMasterTextureLimit,  //  <- UnityEngine.QualitySettings::set_masterTextureLimit
    (const void*)&QualitySettings_Get_Custom_PropAnisotropicFiltering,  //  <- UnityEngine.QualitySettings::get_anisotropicFiltering
    (const void*)&QualitySettings_Set_Custom_PropAnisotropicFiltering,  //  <- UnityEngine.QualitySettings::set_anisotropicFiltering
    (const void*)&QualitySettings_Get_Custom_PropLodBias  ,  //  <- UnityEngine.QualitySettings::get_lodBias
    (const void*)&QualitySettings_Set_Custom_PropLodBias  ,  //  <- UnityEngine.QualitySettings::set_lodBias
    (const void*)&QualitySettings_Get_Custom_PropMaximumLODLevel,  //  <- UnityEngine.QualitySettings::get_maximumLODLevel
    (const void*)&QualitySettings_Set_Custom_PropMaximumLODLevel,  //  <- UnityEngine.QualitySettings::set_maximumLODLevel
    (const void*)&QualitySettings_Get_Custom_PropParticleRaycastBudget,  //  <- UnityEngine.QualitySettings::get_particleRaycastBudget
    (const void*)&QualitySettings_Set_Custom_PropParticleRaycastBudget,  //  <- UnityEngine.QualitySettings::set_particleRaycastBudget
    (const void*)&QualitySettings_Get_Custom_PropSoftVegetation,  //  <- UnityEngine.QualitySettings::get_softVegetation
    (const void*)&QualitySettings_Set_Custom_PropSoftVegetation,  //  <- UnityEngine.QualitySettings::set_softVegetation
    (const void*)&QualitySettings_Get_Custom_PropMaxQueuedFrames,  //  <- UnityEngine.QualitySettings::get_maxQueuedFrames
    (const void*)&QualitySettings_Set_Custom_PropMaxQueuedFrames,  //  <- UnityEngine.QualitySettings::set_maxQueuedFrames
    (const void*)&QualitySettings_Get_Custom_PropVSyncCount,  //  <- UnityEngine.QualitySettings::get_vSyncCount
    (const void*)&QualitySettings_Set_Custom_PropVSyncCount,  //  <- UnityEngine.QualitySettings::set_vSyncCount
    (const void*)&QualitySettings_Get_Custom_PropAntiAliasing,  //  <- UnityEngine.QualitySettings::get_antiAliasing
    (const void*)&QualitySettings_Set_Custom_PropAntiAliasing,  //  <- UnityEngine.QualitySettings::set_antiAliasing
    (const void*)&QualitySettings_Get_Custom_PropDesiredColorSpace,  //  <- UnityEngine.QualitySettings::get_desiredColorSpace
    (const void*)&QualitySettings_Get_Custom_PropActiveColorSpace,  //  <- UnityEngine.QualitySettings::get_activeColorSpace
    (const void*)&QualitySettings_Get_Custom_PropBlendWeights,  //  <- UnityEngine.QualitySettings::get_blendWeights
    (const void*)&QualitySettings_Set_Custom_PropBlendWeights,  //  <- UnityEngine.QualitySettings::set_blendWeights
    (const void*)&Shader_CUSTOM_Find                      ,  //  <- UnityEngine.Shader::Find
    (const void*)&Shader_CUSTOM_FindBuiltin               ,  //  <- UnityEngine.Shader::FindBuiltin
    (const void*)&Shader_Get_Custom_PropIsSupported       ,  //  <- UnityEngine.Shader::get_isSupported
#if UNITY_EDITOR
    (const void*)&Shader_Get_Custom_PropCustomEditor      ,  //  <- UnityEngine.Shader::get_customEditor
#endif
    (const void*)&Shader_CUSTOM_EnableKeyword             ,  //  <- UnityEngine.Shader::EnableKeyword
    (const void*)&Shader_CUSTOM_DisableKeyword            ,  //  <- UnityEngine.Shader::DisableKeyword
    (const void*)&Shader_Get_Custom_PropMaximumLOD        ,  //  <- UnityEngine.Shader::get_maximumLOD
    (const void*)&Shader_Set_Custom_PropMaximumLOD        ,  //  <- UnityEngine.Shader::set_maximumLOD
    (const void*)&Shader_Get_Custom_PropGlobalMaximumLOD  ,  //  <- UnityEngine.Shader::get_globalMaximumLOD
    (const void*)&Shader_Set_Custom_PropGlobalMaximumLOD  ,  //  <- UnityEngine.Shader::set_globalMaximumLOD
    (const void*)&Shader_Get_Custom_PropRenderQueue       ,  //  <- UnityEngine.Shader::get_renderQueue
    (const void*)&Shader_CUSTOM_INTERNAL_CALL_SetGlobalColor,  //  <- UnityEngine.Shader::INTERNAL_CALL_SetGlobalColor
    (const void*)&Shader_CUSTOM_SetGlobalFloat            ,  //  <- UnityEngine.Shader::SetGlobalFloat
    (const void*)&Shader_CUSTOM_SetGlobalTexture          ,  //  <- UnityEngine.Shader::SetGlobalTexture
    (const void*)&Shader_CUSTOM_INTERNAL_CALL_SetGlobalMatrix,  //  <- UnityEngine.Shader::INTERNAL_CALL_SetGlobalMatrix
    (const void*)&Shader_CUSTOM_SetGlobalTexGenMode       ,  //  <- UnityEngine.Shader::SetGlobalTexGenMode
    (const void*)&Shader_CUSTOM_SetGlobalTextureMatrixName,  //  <- UnityEngine.Shader::SetGlobalTextureMatrixName
#if !UNITY_FLASH
    (const void*)&Shader_CUSTOM_SetGlobalBuffer           ,  //  <- UnityEngine.Shader::SetGlobalBuffer
#endif
    (const void*)&Shader_CUSTOM_PropertyToID              ,  //  <- UnityEngine.Shader::PropertyToID
    (const void*)&Shader_CUSTOM_WarmupAllShaders          ,  //  <- UnityEngine.Shader::WarmupAllShaders
    (const void*)&Texture_Get_Custom_PropMasterTextureLimit,  //  <- UnityEngine.Texture::get_masterTextureLimit
    (const void*)&Texture_Set_Custom_PropMasterTextureLimit,  //  <- UnityEngine.Texture::set_masterTextureLimit
    (const void*)&Texture_Get_Custom_PropAnisotropicFiltering,  //  <- UnityEngine.Texture::get_anisotropicFiltering
    (const void*)&Texture_Set_Custom_PropAnisotropicFiltering,  //  <- UnityEngine.Texture::set_anisotropicFiltering
    (const void*)&Texture_CUSTOM_SetGlobalAnisotropicFilteringLimits,  //  <- UnityEngine.Texture::SetGlobalAnisotropicFilteringLimits
    (const void*)&Texture_CUSTOM_Internal_GetWidth        ,  //  <- UnityEngine.Texture::Internal_GetWidth
    (const void*)&Texture_CUSTOM_Internal_GetHeight       ,  //  <- UnityEngine.Texture::Internal_GetHeight
    (const void*)&Texture_Get_Custom_PropFilterMode       ,  //  <- UnityEngine.Texture::get_filterMode
    (const void*)&Texture_Set_Custom_PropFilterMode       ,  //  <- UnityEngine.Texture::set_filterMode
    (const void*)&Texture_Get_Custom_PropAnisoLevel       ,  //  <- UnityEngine.Texture::get_anisoLevel
    (const void*)&Texture_Set_Custom_PropAnisoLevel       ,  //  <- UnityEngine.Texture::set_anisoLevel
    (const void*)&Texture_Get_Custom_PropWrapMode         ,  //  <- UnityEngine.Texture::get_wrapMode
    (const void*)&Texture_Set_Custom_PropWrapMode         ,  //  <- UnityEngine.Texture::set_wrapMode
    (const void*)&Texture_Get_Custom_PropMipMapBias       ,  //  <- UnityEngine.Texture::get_mipMapBias
    (const void*)&Texture_Set_Custom_PropMipMapBias       ,  //  <- UnityEngine.Texture::set_mipMapBias
    (const void*)&Texture_CUSTOM_Internal_GetTexelSize    ,  //  <- UnityEngine.Texture::Internal_GetTexelSize
    (const void*)&Texture_CUSTOM_INTERNAL_CALL_GetNativeTexturePtr,  //  <- UnityEngine.Texture::INTERNAL_CALL_GetNativeTexturePtr
    (const void*)&Texture_CUSTOM_GetNativeTextureID       ,  //  <- UnityEngine.Texture::GetNativeTextureID
    (const void*)&Texture2D_Get_Custom_PropMipmapCount    ,  //  <- UnityEngine.Texture2D::get_mipmapCount
    (const void*)&Texture2D_CUSTOM_Internal_Create        ,  //  <- UnityEngine.Texture2D::Internal_Create
#if ENABLE_TEXTUREID_MAP
    (const void*)&Texture2D_CUSTOM_UpdateExternalTexture  ,  //  <- UnityEngine.Texture2D::UpdateExternalTexture
#endif
    (const void*)&Texture2D_Get_Custom_PropFormat         ,  //  <- UnityEngine.Texture2D::get_format
    (const void*)&Texture2D_CUSTOM_INTERNAL_CALL_SetPixel ,  //  <- UnityEngine.Texture2D::INTERNAL_CALL_SetPixel
    (const void*)&Texture2D_CUSTOM_INTERNAL_CALL_GetPixel ,  //  <- UnityEngine.Texture2D::INTERNAL_CALL_GetPixel
    (const void*)&Texture2D_CUSTOM_INTERNAL_CALL_GetPixelBilinear,  //  <- UnityEngine.Texture2D::INTERNAL_CALL_GetPixelBilinear
    (const void*)&Texture2D_CUSTOM_SetPixels              ,  //  <- UnityEngine.Texture2D::SetPixels
    (const void*)&Texture2D_CUSTOM_SetPixels32            ,  //  <- UnityEngine.Texture2D::SetPixels32
    (const void*)&Texture2D_CUSTOM_LoadImage              ,  //  <- UnityEngine.Texture2D::LoadImage
    (const void*)&Texture2D_CUSTOM_GetPixels              ,  //  <- UnityEngine.Texture2D::GetPixels
    (const void*)&Texture2D_CUSTOM_GetPixels32            ,  //  <- UnityEngine.Texture2D::GetPixels32
    (const void*)&Texture2D_CUSTOM_Apply                  ,  //  <- UnityEngine.Texture2D::Apply
    (const void*)&Texture2D_CUSTOM_Resize                 ,  //  <- UnityEngine.Texture2D::Resize
    (const void*)&Texture2D_CUSTOM_Internal_ResizeWH      ,  //  <- UnityEngine.Texture2D::Internal_ResizeWH
    (const void*)&Texture2D_CUSTOM_INTERNAL_CALL_Compress ,  //  <- UnityEngine.Texture2D::INTERNAL_CALL_Compress
    (const void*)&Texture2D_CUSTOM_PackTextures           ,  //  <- UnityEngine.Texture2D::PackTextures
    (const void*)&Texture2D_CUSTOM_INTERNAL_CALL_ReadPixels,  //  <- UnityEngine.Texture2D::INTERNAL_CALL_ReadPixels
    (const void*)&Texture2D_CUSTOM_EncodeToPNG            ,  //  <- UnityEngine.Texture2D::EncodeToPNG
#if UNITY_EDITOR
    (const void*)&Texture2D_Get_Custom_PropAlphaIsTransparency,  //  <- UnityEngine.Texture2D::get_alphaIsTransparency
    (const void*)&Texture2D_Set_Custom_PropAlphaIsTransparency,  //  <- UnityEngine.Texture2D::set_alphaIsTransparency
#endif
    (const void*)&Cubemap_CUSTOM_INTERNAL_CALL_SetPixel   ,  //  <- UnityEngine.Cubemap::INTERNAL_CALL_SetPixel
    (const void*)&Cubemap_CUSTOM_INTERNAL_CALL_GetPixel   ,  //  <- UnityEngine.Cubemap::INTERNAL_CALL_GetPixel
    (const void*)&Cubemap_CUSTOM_GetPixels                ,  //  <- UnityEngine.Cubemap::GetPixels
    (const void*)&Cubemap_CUSTOM_SetPixels                ,  //  <- UnityEngine.Cubemap::SetPixels
    (const void*)&Cubemap_CUSTOM_Apply                    ,  //  <- UnityEngine.Cubemap::Apply
    (const void*)&Cubemap_Get_Custom_PropFormat           ,  //  <- UnityEngine.Cubemap::get_format
    (const void*)&Cubemap_CUSTOM_Internal_Create          ,  //  <- UnityEngine.Cubemap::Internal_Create
    (const void*)&Cubemap_CUSTOM_SmoothEdges              ,  //  <- UnityEngine.Cubemap::SmoothEdges
    (const void*)&Texture3D_Get_Custom_PropDepth          ,  //  <- UnityEngine.Texture3D::get_depth
    (const void*)&Texture3D_CUSTOM_GetPixels              ,  //  <- UnityEngine.Texture3D::GetPixels
    (const void*)&Texture3D_CUSTOM_SetPixels              ,  //  <- UnityEngine.Texture3D::SetPixels
    (const void*)&Texture3D_CUSTOM_Apply                  ,  //  <- UnityEngine.Texture3D::Apply
    (const void*)&Texture3D_Get_Custom_PropFormat         ,  //  <- UnityEngine.Texture3D::get_format
    (const void*)&Texture3D_CUSTOM_Internal_Create        ,  //  <- UnityEngine.Texture3D::Internal_Create
    (const void*)&MeshFilter_Get_Custom_PropMesh          ,  //  <- UnityEngine.MeshFilter::get_mesh
    (const void*)&MeshFilter_Set_Custom_PropMesh          ,  //  <- UnityEngine.MeshFilter::set_mesh
    (const void*)&MeshFilter_Get_Custom_PropSharedMesh    ,  //  <- UnityEngine.MeshFilter::get_sharedMesh
    (const void*)&MeshFilter_Set_Custom_PropSharedMesh    ,  //  <- UnityEngine.MeshFilter::set_sharedMesh
    (const void*)&CombineInstance_CUSTOM_InternalGetMesh  ,  //  <- UnityEngine.CombineInstance::InternalGetMesh
    (const void*)&Mesh_CUSTOM_Internal_Create             ,  //  <- UnityEngine.Mesh::Internal_Create
    (const void*)&Mesh_CUSTOM_Clear                       ,  //  <- UnityEngine.Mesh::Clear
    (const void*)&Mesh_Get_Custom_PropIsReadable          ,  //  <- UnityEngine.Mesh::get_isReadable
    (const void*)&Mesh_Get_Custom_PropCanAccess           ,  //  <- UnityEngine.Mesh::get_canAccess
    (const void*)&Mesh_Get_Custom_PropVertices            ,  //  <- UnityEngine.Mesh::get_vertices
    (const void*)&Mesh_Set_Custom_PropVertices            ,  //  <- UnityEngine.Mesh::set_vertices
    (const void*)&Mesh_Get_Custom_PropNormals             ,  //  <- UnityEngine.Mesh::get_normals
    (const void*)&Mesh_Set_Custom_PropNormals             ,  //  <- UnityEngine.Mesh::set_normals
    (const void*)&Mesh_Get_Custom_PropTangents            ,  //  <- UnityEngine.Mesh::get_tangents
    (const void*)&Mesh_Set_Custom_PropTangents            ,  //  <- UnityEngine.Mesh::set_tangents
    (const void*)&Mesh_Get_Custom_PropUv                  ,  //  <- UnityEngine.Mesh::get_uv
    (const void*)&Mesh_Set_Custom_PropUv                  ,  //  <- UnityEngine.Mesh::set_uv
    (const void*)&Mesh_Get_Custom_PropUv2                 ,  //  <- UnityEngine.Mesh::get_uv2
    (const void*)&Mesh_Set_Custom_PropUv2                 ,  //  <- UnityEngine.Mesh::set_uv2
    (const void*)&Mesh_CUSTOM_INTERNAL_get_bounds         ,  //  <- UnityEngine.Mesh::INTERNAL_get_bounds
    (const void*)&Mesh_CUSTOM_INTERNAL_set_bounds         ,  //  <- UnityEngine.Mesh::INTERNAL_set_bounds
    (const void*)&Mesh_Get_Custom_PropColors              ,  //  <- UnityEngine.Mesh::get_colors
    (const void*)&Mesh_Set_Custom_PropColors              ,  //  <- UnityEngine.Mesh::set_colors
    (const void*)&Mesh_Get_Custom_PropColors32            ,  //  <- UnityEngine.Mesh::get_colors32
    (const void*)&Mesh_Set_Custom_PropColors32            ,  //  <- UnityEngine.Mesh::set_colors32
    (const void*)&Mesh_CUSTOM_RecalculateBounds           ,  //  <- UnityEngine.Mesh::RecalculateBounds
    (const void*)&Mesh_CUSTOM_RecalculateNormals          ,  //  <- UnityEngine.Mesh::RecalculateNormals
    (const void*)&Mesh_CUSTOM_Optimize                    ,  //  <- UnityEngine.Mesh::Optimize
    (const void*)&Mesh_Get_Custom_PropTriangles           ,  //  <- UnityEngine.Mesh::get_triangles
    (const void*)&Mesh_Set_Custom_PropTriangles           ,  //  <- UnityEngine.Mesh::set_triangles
    (const void*)&Mesh_CUSTOM_GetTriangles                ,  //  <- UnityEngine.Mesh::GetTriangles
    (const void*)&Mesh_CUSTOM_SetTriangles                ,  //  <- UnityEngine.Mesh::SetTriangles
    (const void*)&Mesh_CUSTOM_GetIndices                  ,  //  <- UnityEngine.Mesh::GetIndices
    (const void*)&Mesh_CUSTOM_SetIndices                  ,  //  <- UnityEngine.Mesh::SetIndices
    (const void*)&Mesh_CUSTOM_GetTopology                 ,  //  <- UnityEngine.Mesh::GetTopology
    (const void*)&Mesh_Get_Custom_PropVertexCount         ,  //  <- UnityEngine.Mesh::get_vertexCount
    (const void*)&Mesh_Get_Custom_PropSubMeshCount        ,  //  <- UnityEngine.Mesh::get_subMeshCount
    (const void*)&Mesh_Set_Custom_PropSubMeshCount        ,  //  <- UnityEngine.Mesh::set_subMeshCount
    (const void*)&Mesh_CUSTOM_SetTriangleStrip            ,  //  <- UnityEngine.Mesh::SetTriangleStrip
    (const void*)&Mesh_CUSTOM_GetTriangleStrip            ,  //  <- UnityEngine.Mesh::GetTriangleStrip
    (const void*)&Mesh_CUSTOM_CombineMeshes               ,  //  <- UnityEngine.Mesh::CombineMeshes
    (const void*)&Mesh_Get_Custom_PropBoneWeights         ,  //  <- UnityEngine.Mesh::get_boneWeights
    (const void*)&Mesh_Set_Custom_PropBoneWeights         ,  //  <- UnityEngine.Mesh::set_boneWeights
    (const void*)&Mesh_Get_Custom_PropBindposes           ,  //  <- UnityEngine.Mesh::get_bindposes
    (const void*)&Mesh_Set_Custom_PropBindposes           ,  //  <- UnityEngine.Mesh::set_bindposes
    (const void*)&Mesh_CUSTOM_MarkDynamic                 ,  //  <- UnityEngine.Mesh::MarkDynamic
#if USE_BLENDSHAPES
    (const void*)&Mesh_Get_Custom_PropBlendShapes         ,  //  <- UnityEngine.Mesh::get_blendShapes
    (const void*)&Mesh_Set_Custom_PropBlendShapes         ,  //  <- UnityEngine.Mesh::set_blendShapes
    (const void*)&Mesh_Get_Custom_PropBlendShapeCount     ,  //  <- UnityEngine.Mesh::get_blendShapeCount
    (const void*)&Mesh_CUSTOM_GetBlendShapeName           ,  //  <- UnityEngine.Mesh::GetBlendShapeName
#endif
    (const void*)&SkinnedMeshRenderer_Get_Custom_PropBones,  //  <- UnityEngine.SkinnedMeshRenderer::get_bones
    (const void*)&SkinnedMeshRenderer_Set_Custom_PropBones,  //  <- UnityEngine.SkinnedMeshRenderer::set_bones
    (const void*)&SkinnedMeshRenderer_Get_Custom_PropRootBone,  //  <- UnityEngine.SkinnedMeshRenderer::get_rootBone
    (const void*)&SkinnedMeshRenderer_Set_Custom_PropRootBone,  //  <- UnityEngine.SkinnedMeshRenderer::set_rootBone
    (const void*)&SkinnedMeshRenderer_Get_Custom_PropQuality,  //  <- UnityEngine.SkinnedMeshRenderer::get_quality
    (const void*)&SkinnedMeshRenderer_Set_Custom_PropQuality,  //  <- UnityEngine.SkinnedMeshRenderer::set_quality
    (const void*)&SkinnedMeshRenderer_Get_Custom_PropSharedMesh,  //  <- UnityEngine.SkinnedMeshRenderer::get_sharedMesh
    (const void*)&SkinnedMeshRenderer_Set_Custom_PropSharedMesh,  //  <- UnityEngine.SkinnedMeshRenderer::set_sharedMesh
    (const void*)&SkinnedMeshRenderer_Get_Custom_PropUpdateWhenOffscreen,  //  <- UnityEngine.SkinnedMeshRenderer::get_updateWhenOffscreen
    (const void*)&SkinnedMeshRenderer_Set_Custom_PropUpdateWhenOffscreen,  //  <- UnityEngine.SkinnedMeshRenderer::set_updateWhenOffscreen
    (const void*)&SkinnedMeshRenderer_CUSTOM_INTERNAL_get_localBounds,  //  <- UnityEngine.SkinnedMeshRenderer::INTERNAL_get_localBounds
    (const void*)&SkinnedMeshRenderer_CUSTOM_INTERNAL_set_localBounds,  //  <- UnityEngine.SkinnedMeshRenderer::INTERNAL_set_localBounds
    (const void*)&SkinnedMeshRenderer_CUSTOM_BakeMesh     ,  //  <- UnityEngine.SkinnedMeshRenderer::BakeMesh
#if UNITY_EDITOR
    (const void*)&SkinnedMeshRenderer_Get_Custom_PropActualRootBone,  //  <- UnityEngine.SkinnedMeshRenderer::get_actualRootBone
#endif
#if USE_BLENDSHAPES
    (const void*)&SkinnedMeshRenderer_CUSTOM_GetBlendShapeWeight,  //  <- UnityEngine.SkinnedMeshRenderer::GetBlendShapeWeight
    (const void*)&SkinnedMeshRenderer_CUSTOM_SetBlendShapeWeight,  //  <- UnityEngine.SkinnedMeshRenderer::SetBlendShapeWeight
#endif
    (const void*)&Material_Get_Custom_PropShader          ,  //  <- UnityEngine.Material::get_shader
    (const void*)&Material_Set_Custom_PropShader          ,  //  <- UnityEngine.Material::set_shader
    (const void*)&Material_CUSTOM_INTERNAL_CALL_SetColor  ,  //  <- UnityEngine.Material::INTERNAL_CALL_SetColor
    (const void*)&Material_CUSTOM_INTERNAL_CALL_GetColor  ,  //  <- UnityEngine.Material::INTERNAL_CALL_GetColor
    (const void*)&Material_CUSTOM_SetTexture              ,  //  <- UnityEngine.Material::SetTexture
    (const void*)&Material_CUSTOM_GetTexture              ,  //  <- UnityEngine.Material::GetTexture
    (const void*)&Material_CUSTOM_Internal_GetTextureOffset,  //  <- UnityEngine.Material::Internal_GetTextureOffset
    (const void*)&Material_CUSTOM_Internal_GetTextureScale,  //  <- UnityEngine.Material::Internal_GetTextureScale
    (const void*)&Material_CUSTOM_INTERNAL_CALL_SetTextureOffset,  //  <- UnityEngine.Material::INTERNAL_CALL_SetTextureOffset
    (const void*)&Material_CUSTOM_INTERNAL_CALL_SetTextureScale,  //  <- UnityEngine.Material::INTERNAL_CALL_SetTextureScale
    (const void*)&Material_CUSTOM_INTERNAL_CALL_SetMatrix ,  //  <- UnityEngine.Material::INTERNAL_CALL_SetMatrix
    (const void*)&Material_CUSTOM_INTERNAL_CALL_GetMatrix ,  //  <- UnityEngine.Material::INTERNAL_CALL_GetMatrix
    (const void*)&Material_CUSTOM_SetFloat                ,  //  <- UnityEngine.Material::SetFloat
    (const void*)&Material_CUSTOM_GetFloat                ,  //  <- UnityEngine.Material::GetFloat
#if !UNITY_FLASH
    (const void*)&Material_CUSTOM_SetBuffer               ,  //  <- UnityEngine.Material::SetBuffer
#endif
    (const void*)&Material_CUSTOM_HasProperty             ,  //  <- UnityEngine.Material::HasProperty
    (const void*)&Material_CUSTOM_GetTag                  ,  //  <- UnityEngine.Material::GetTag
    (const void*)&Material_CUSTOM_Lerp                    ,  //  <- UnityEngine.Material::Lerp
    (const void*)&Material_Get_Custom_PropPassCount       ,  //  <- UnityEngine.Material::get_passCount
    (const void*)&Material_CUSTOM_SetPass                 ,  //  <- UnityEngine.Material::SetPass
    (const void*)&Material_Get_Custom_PropRenderQueue     ,  //  <- UnityEngine.Material::get_renderQueue
    (const void*)&Material_Set_Custom_PropRenderQueue     ,  //  <- UnityEngine.Material::set_renderQueue
    (const void*)&Material_CUSTOM_Internal_CreateWithString,  //  <- UnityEngine.Material::Internal_CreateWithString
    (const void*)&Material_CUSTOM_Internal_CreateWithShader,  //  <- UnityEngine.Material::Internal_CreateWithShader
    (const void*)&Material_CUSTOM_Internal_CreateWithMaterial,  //  <- UnityEngine.Material::Internal_CreateWithMaterial
    (const void*)&Material_CUSTOM_CopyPropertiesFromMaterial,  //  <- UnityEngine.Material::CopyPropertiesFromMaterial
#if ENABLE_MONO || PLATFORM_WINRT
    (const void*)&Material_Get_Custom_PropShaderKeywords  ,  //  <- UnityEngine.Material::get_shaderKeywords
    (const void*)&Material_Set_Custom_PropShaderKeywords  ,  //  <- UnityEngine.Material::set_shaderKeywords
#endif
    (const void*)&LensFlare_Get_Custom_PropFlare          ,  //  <- UnityEngine.LensFlare::get_flare
    (const void*)&LensFlare_Set_Custom_PropFlare          ,  //  <- UnityEngine.LensFlare::set_flare
    (const void*)&LensFlare_Get_Custom_PropBrightness     ,  //  <- UnityEngine.LensFlare::get_brightness
    (const void*)&LensFlare_Set_Custom_PropBrightness     ,  //  <- UnityEngine.LensFlare::set_brightness
    (const void*)&LensFlare_CUSTOM_INTERNAL_get_color     ,  //  <- UnityEngine.LensFlare::INTERNAL_get_color
    (const void*)&LensFlare_CUSTOM_INTERNAL_set_color     ,  //  <- UnityEngine.LensFlare::INTERNAL_set_color
    (const void*)&Renderer_Get_Custom_PropStaticBatchRootTransform,  //  <- UnityEngine.Renderer::get_staticBatchRootTransform
    (const void*)&Renderer_Set_Custom_PropStaticBatchRootTransform,  //  <- UnityEngine.Renderer::set_staticBatchRootTransform
    (const void*)&Renderer_Get_Custom_PropStaticBatchIndex,  //  <- UnityEngine.Renderer::get_staticBatchIndex
    (const void*)&Renderer_CUSTOM_SetSubsetIndex          ,  //  <- UnityEngine.Renderer::SetSubsetIndex
    (const void*)&Renderer_Get_Custom_PropIsPartOfStaticBatch,  //  <- UnityEngine.Renderer::get_isPartOfStaticBatch
    (const void*)&Renderer_CUSTOM_INTERNAL_get_worldToLocalMatrix,  //  <- UnityEngine.Renderer::INTERNAL_get_worldToLocalMatrix
    (const void*)&Renderer_CUSTOM_INTERNAL_get_localToWorldMatrix,  //  <- UnityEngine.Renderer::INTERNAL_get_localToWorldMatrix
    (const void*)&Renderer_Get_Custom_PropEnabled         ,  //  <- UnityEngine.Renderer::get_enabled
    (const void*)&Renderer_Set_Custom_PropEnabled         ,  //  <- UnityEngine.Renderer::set_enabled
    (const void*)&Renderer_Get_Custom_PropCastShadows     ,  //  <- UnityEngine.Renderer::get_castShadows
    (const void*)&Renderer_Set_Custom_PropCastShadows     ,  //  <- UnityEngine.Renderer::set_castShadows
    (const void*)&Renderer_Get_Custom_PropReceiveShadows  ,  //  <- UnityEngine.Renderer::get_receiveShadows
    (const void*)&Renderer_Set_Custom_PropReceiveShadows  ,  //  <- UnityEngine.Renderer::set_receiveShadows
    (const void*)&Renderer_Get_Custom_PropMaterial        ,  //  <- UnityEngine.Renderer::get_material
    (const void*)&Renderer_Set_Custom_PropMaterial        ,  //  <- UnityEngine.Renderer::set_material
    (const void*)&Renderer_Get_Custom_PropSharedMaterial  ,  //  <- UnityEngine.Renderer::get_sharedMaterial
    (const void*)&Renderer_Set_Custom_PropSharedMaterial  ,  //  <- UnityEngine.Renderer::set_sharedMaterial
    (const void*)&Renderer_Get_Custom_PropSharedMaterials ,  //  <- UnityEngine.Renderer::get_sharedMaterials
    (const void*)&Renderer_Set_Custom_PropSharedMaterials ,  //  <- UnityEngine.Renderer::set_sharedMaterials
    (const void*)&Renderer_Get_Custom_PropMaterials       ,  //  <- UnityEngine.Renderer::get_materials
    (const void*)&Renderer_Set_Custom_PropMaterials       ,  //  <- UnityEngine.Renderer::set_materials
    (const void*)&Renderer_CUSTOM_INTERNAL_get_bounds     ,  //  <- UnityEngine.Renderer::INTERNAL_get_bounds
    (const void*)&Renderer_Get_Custom_PropLightmapIndex   ,  //  <- UnityEngine.Renderer::get_lightmapIndex
    (const void*)&Renderer_Set_Custom_PropLightmapIndex   ,  //  <- UnityEngine.Renderer::set_lightmapIndex
    (const void*)&Renderer_CUSTOM_INTERNAL_get_lightmapTilingOffset,  //  <- UnityEngine.Renderer::INTERNAL_get_lightmapTilingOffset
    (const void*)&Renderer_CUSTOM_INTERNAL_set_lightmapTilingOffset,  //  <- UnityEngine.Renderer::INTERNAL_set_lightmapTilingOffset
    (const void*)&Renderer_Get_Custom_PropIsVisible       ,  //  <- UnityEngine.Renderer::get_isVisible
    (const void*)&Renderer_Get_Custom_PropUseLightProbes  ,  //  <- UnityEngine.Renderer::get_useLightProbes
    (const void*)&Renderer_Set_Custom_PropUseLightProbes  ,  //  <- UnityEngine.Renderer::set_useLightProbes
    (const void*)&Renderer_Get_Custom_PropLightProbeAnchor,  //  <- UnityEngine.Renderer::get_lightProbeAnchor
    (const void*)&Renderer_Set_Custom_PropLightProbeAnchor,  //  <- UnityEngine.Renderer::set_lightProbeAnchor
    (const void*)&Renderer_CUSTOM_SetPropertyBlock        ,  //  <- UnityEngine.Renderer::SetPropertyBlock
    (const void*)&Renderer_CUSTOM_Render                  ,  //  <- UnityEngine.Renderer::Render
    (const void*)&Projector_Get_Custom_PropNearClipPlane  ,  //  <- UnityEngine.Projector::get_nearClipPlane
    (const void*)&Projector_Set_Custom_PropNearClipPlane  ,  //  <- UnityEngine.Projector::set_nearClipPlane
    (const void*)&Projector_Get_Custom_PropFarClipPlane   ,  //  <- UnityEngine.Projector::get_farClipPlane
    (const void*)&Projector_Set_Custom_PropFarClipPlane   ,  //  <- UnityEngine.Projector::set_farClipPlane
    (const void*)&Projector_Get_Custom_PropFieldOfView    ,  //  <- UnityEngine.Projector::get_fieldOfView
    (const void*)&Projector_Set_Custom_PropFieldOfView    ,  //  <- UnityEngine.Projector::set_fieldOfView
    (const void*)&Projector_Get_Custom_PropAspectRatio    ,  //  <- UnityEngine.Projector::get_aspectRatio
    (const void*)&Projector_Set_Custom_PropAspectRatio    ,  //  <- UnityEngine.Projector::set_aspectRatio
    (const void*)&Projector_Get_Custom_PropOrthographic   ,  //  <- UnityEngine.Projector::get_orthographic
    (const void*)&Projector_Set_Custom_PropOrthographic   ,  //  <- UnityEngine.Projector::set_orthographic
    (const void*)&Projector_Get_Custom_PropOrthographicSize,  //  <- UnityEngine.Projector::get_orthographicSize
    (const void*)&Projector_Set_Custom_PropOrthographicSize,  //  <- UnityEngine.Projector::set_orthographicSize
    (const void*)&Projector_Get_Custom_PropIgnoreLayers   ,  //  <- UnityEngine.Projector::get_ignoreLayers
    (const void*)&Projector_Set_Custom_PropIgnoreLayers   ,  //  <- UnityEngine.Projector::set_ignoreLayers
    (const void*)&Projector_Get_Custom_PropMaterial       ,  //  <- UnityEngine.Projector::get_material
    (const void*)&Projector_Set_Custom_PropMaterial       ,  //  <- UnityEngine.Projector::set_material
    (const void*)&Skybox_Get_Custom_PropMaterial          ,  //  <- UnityEngine.Skybox::get_material
    (const void*)&Skybox_Set_Custom_PropMaterial          ,  //  <- UnityEngine.Skybox::set_material
    (const void*)&TextMesh_Get_Custom_PropText            ,  //  <- UnityEngine.TextMesh::get_text
    (const void*)&TextMesh_Set_Custom_PropText            ,  //  <- UnityEngine.TextMesh::set_text
    (const void*)&TextMesh_Get_Custom_PropFont            ,  //  <- UnityEngine.TextMesh::get_font
    (const void*)&TextMesh_Set_Custom_PropFont            ,  //  <- UnityEngine.TextMesh::set_font
    (const void*)&TextMesh_Get_Custom_PropFontSize        ,  //  <- UnityEngine.TextMesh::get_fontSize
    (const void*)&TextMesh_Set_Custom_PropFontSize        ,  //  <- UnityEngine.TextMesh::set_fontSize
    (const void*)&TextMesh_Get_Custom_PropFontStyle       ,  //  <- UnityEngine.TextMesh::get_fontStyle
    (const void*)&TextMesh_Set_Custom_PropFontStyle       ,  //  <- UnityEngine.TextMesh::set_fontStyle
    (const void*)&TextMesh_Get_Custom_PropOffsetZ         ,  //  <- UnityEngine.TextMesh::get_offsetZ
    (const void*)&TextMesh_Set_Custom_PropOffsetZ         ,  //  <- UnityEngine.TextMesh::set_offsetZ
    (const void*)&TextMesh_Get_Custom_PropAlignment       ,  //  <- UnityEngine.TextMesh::get_alignment
    (const void*)&TextMesh_Set_Custom_PropAlignment       ,  //  <- UnityEngine.TextMesh::set_alignment
    (const void*)&TextMesh_Get_Custom_PropAnchor          ,  //  <- UnityEngine.TextMesh::get_anchor
    (const void*)&TextMesh_Set_Custom_PropAnchor          ,  //  <- UnityEngine.TextMesh::set_anchor
    (const void*)&TextMesh_Get_Custom_PropCharacterSize   ,  //  <- UnityEngine.TextMesh::get_characterSize
    (const void*)&TextMesh_Set_Custom_PropCharacterSize   ,  //  <- UnityEngine.TextMesh::set_characterSize
    (const void*)&TextMesh_Get_Custom_PropLineSpacing     ,  //  <- UnityEngine.TextMesh::get_lineSpacing
    (const void*)&TextMesh_Set_Custom_PropLineSpacing     ,  //  <- UnityEngine.TextMesh::set_lineSpacing
    (const void*)&TextMesh_Get_Custom_PropTabSize         ,  //  <- UnityEngine.TextMesh::get_tabSize
    (const void*)&TextMesh_Set_Custom_PropTabSize         ,  //  <- UnityEngine.TextMesh::set_tabSize
    (const void*)&TextMesh_Get_Custom_PropRichText        ,  //  <- UnityEngine.TextMesh::get_richText
    (const void*)&TextMesh_Set_Custom_PropRichText        ,  //  <- UnityEngine.TextMesh::set_richText
    (const void*)&TextMesh_CUSTOM_INTERNAL_get_color      ,  //  <- UnityEngine.TextMesh::INTERNAL_get_color
    (const void*)&TextMesh_CUSTOM_INTERNAL_set_color      ,  //  <- UnityEngine.TextMesh::INTERNAL_set_color
    (const void*)&TrailRenderer_Get_Custom_PropTime       ,  //  <- UnityEngine.TrailRenderer::get_time
    (const void*)&TrailRenderer_Set_Custom_PropTime       ,  //  <- UnityEngine.TrailRenderer::set_time
    (const void*)&TrailRenderer_Get_Custom_PropStartWidth ,  //  <- UnityEngine.TrailRenderer::get_startWidth
    (const void*)&TrailRenderer_Set_Custom_PropStartWidth ,  //  <- UnityEngine.TrailRenderer::set_startWidth
    (const void*)&TrailRenderer_Get_Custom_PropEndWidth   ,  //  <- UnityEngine.TrailRenderer::get_endWidth
    (const void*)&TrailRenderer_Set_Custom_PropEndWidth   ,  //  <- UnityEngine.TrailRenderer::set_endWidth
    (const void*)&TrailRenderer_Get_Custom_PropAutodestruct,  //  <- UnityEngine.TrailRenderer::get_autodestruct
    (const void*)&TrailRenderer_Set_Custom_PropAutodestruct,  //  <- UnityEngine.TrailRenderer::set_autodestruct
    (const void*)&LineRenderer_CUSTOM_INTERNAL_CALL_SetWidth,  //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetWidth
    (const void*)&LineRenderer_CUSTOM_INTERNAL_CALL_SetColors,  //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetColors
    (const void*)&LineRenderer_CUSTOM_INTERNAL_CALL_SetVertexCount,  //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetVertexCount
    (const void*)&LineRenderer_CUSTOM_INTERNAL_CALL_SetPosition,  //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetPosition
    (const void*)&LineRenderer_Get_Custom_PropUseWorldSpace,  //  <- UnityEngine.LineRenderer::get_useWorldSpace
    (const void*)&LineRenderer_Set_Custom_PropUseWorldSpace,  //  <- UnityEngine.LineRenderer::set_useWorldSpace
    (const void*)&MaterialPropertyBlock_CUSTOM_InitBlock  ,  //  <- UnityEngine.MaterialPropertyBlock::InitBlock
    (const void*)&MaterialPropertyBlock_CUSTOM_DestroyBlock,  //  <- UnityEngine.MaterialPropertyBlock::DestroyBlock
    (const void*)&MaterialPropertyBlock_CUSTOM_AddFloat   ,  //  <- UnityEngine.MaterialPropertyBlock::AddFloat
    (const void*)&MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddVector,  //  <- UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddVector
    (const void*)&MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddColor,  //  <- UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddColor
    (const void*)&MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddMatrix,  //  <- UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddMatrix
    (const void*)&MaterialPropertyBlock_CUSTOM_Clear      ,  //  <- UnityEngine.MaterialPropertyBlock::Clear
    (const void*)&Graphics_CUSTOM_Internal_DrawMeshTR     ,  //  <- UnityEngine.Graphics::Internal_DrawMeshTR
    (const void*)&Graphics_CUSTOM_Internal_DrawMeshMatrix ,  //  <- UnityEngine.Graphics::Internal_DrawMeshMatrix
    (const void*)&Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow1,  //  <- UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow1
    (const void*)&Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow2,  //  <- UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow2
    (const void*)&Graphics_CUSTOM_DrawProcedural          ,  //  <- UnityEngine.Graphics::DrawProcedural
#if !UNITY_FLASH
    (const void*)&Graphics_CUSTOM_DrawProceduralIndirect  ,  //  <- UnityEngine.Graphics::DrawProceduralIndirect
#endif
    (const void*)&Graphics_CUSTOM_DrawTexture             ,  //  <- UnityEngine.Graphics::DrawTexture
    (const void*)&Graphics_CUSTOM_Blit                    ,  //  <- UnityEngine.Graphics::Blit
    (const void*)&Graphics_CUSTOM_Internal_BlitMaterial   ,  //  <- UnityEngine.Graphics::Internal_BlitMaterial
    (const void*)&Graphics_CUSTOM_Internal_BlitMultiTap   ,  //  <- UnityEngine.Graphics::Internal_BlitMultiTap
    (const void*)&Graphics_CUSTOM_Internal_SetRT          ,  //  <- UnityEngine.Graphics::Internal_SetRT
    (const void*)&Graphics_CUSTOM_Internal_SetRTBuffer    ,  //  <- UnityEngine.Graphics::Internal_SetRTBuffer
    (const void*)&Graphics_CUSTOM_Internal_SetRTBuffers   ,  //  <- UnityEngine.Graphics::Internal_SetRTBuffers
    (const void*)&Graphics_CUSTOM_GetActiveColorBuffer    ,  //  <- UnityEngine.Graphics::GetActiveColorBuffer
    (const void*)&Graphics_CUSTOM_GetActiveDepthBuffer    ,  //  <- UnityEngine.Graphics::GetActiveDepthBuffer
    (const void*)&Graphics_CUSTOM_ClearRandomWriteTargets ,  //  <- UnityEngine.Graphics::ClearRandomWriteTargets
    (const void*)&Graphics_CUSTOM_Internal_SetRandomWriteTargetRT,  //  <- UnityEngine.Graphics::Internal_SetRandomWriteTargetRT
#if !UNITY_FLASH
    (const void*)&Graphics_CUSTOM_Internal_SetRandomWriteTargetBuffer,  //  <- UnityEngine.Graphics::Internal_SetRandomWriteTargetBuffer
#endif
    (const void*)&Graphics_CUSTOM_SetupVertexLights       ,  //  <- UnityEngine.Graphics::SetupVertexLights
#if ENABLE_MONO || PLATFORM_WINRT
    (const void*)&LightProbes_CUSTOM_INTERNAL_CALL_GetInterpolatedLightProbe,  //  <- UnityEngine.LightProbes::INTERNAL_CALL_GetInterpolatedLightProbe
    (const void*)&LightProbes_Get_Custom_PropPositions    ,  //  <- UnityEngine.LightProbes::get_positions
    (const void*)&LightProbes_Get_Custom_PropCoefficients ,  //  <- UnityEngine.LightProbes::get_coefficients
    (const void*)&LightProbes_Set_Custom_PropCoefficients ,  //  <- UnityEngine.LightProbes::set_coefficients
    (const void*)&LightProbes_Get_Custom_PropCount        ,  //  <- UnityEngine.LightProbes::get_count
    (const void*)&LightProbes_Get_Custom_PropCellCount    ,  //  <- UnityEngine.LightProbes::get_cellCount
    (const void*)&LightmapSettings_Get_Custom_PropLightmaps,  //  <- UnityEngine.LightmapSettings::get_lightmaps
    (const void*)&LightmapSettings_Set_Custom_PropLightmaps,  //  <- UnityEngine.LightmapSettings::set_lightmaps
    (const void*)&LightmapSettings_Get_Custom_PropLightmapsMode,  //  <- UnityEngine.LightmapSettings::get_lightmapsMode
    (const void*)&LightmapSettings_Set_Custom_PropLightmapsMode,  //  <- UnityEngine.LightmapSettings::set_lightmapsMode
    (const void*)&LightmapSettings_Get_Custom_PropBakedColorSpace,  //  <- UnityEngine.LightmapSettings::get_bakedColorSpace
    (const void*)&LightmapSettings_Set_Custom_PropBakedColorSpace,  //  <- UnityEngine.LightmapSettings::set_bakedColorSpace
    (const void*)&LightmapSettings_Get_Custom_PropLightProbes,  //  <- UnityEngine.LightmapSettings::get_lightProbes
    (const void*)&LightmapSettings_Set_Custom_PropLightProbes,  //  <- UnityEngine.LightmapSettings::set_lightProbes
#endif
    (const void*)&GeometryUtility_CUSTOM_INTERNAL_CALL_Internal_ExtractPlanes,  //  <- UnityEngine.GeometryUtility::INTERNAL_CALL_Internal_ExtractPlanes
    (const void*)&GeometryUtility_CUSTOM_INTERNAL_CALL_TestPlanesAABB,  //  <- UnityEngine.GeometryUtility::INTERNAL_CALL_TestPlanesAABB
    (const void*)&Screen_Get_Custom_PropResolutions       ,  //  <- UnityEngine.Screen::get_resolutions
    (const void*)&Screen_Get_Custom_PropCurrentResolution ,  //  <- UnityEngine.Screen::get_currentResolution
    (const void*)&Screen_CUSTOM_SetResolution             ,  //  <- UnityEngine.Screen::SetResolution
    (const void*)&Screen_Get_Custom_PropShowCursor        ,  //  <- UnityEngine.Screen::get_showCursor
    (const void*)&Screen_Set_Custom_PropShowCursor        ,  //  <- UnityEngine.Screen::set_showCursor
    (const void*)&Screen_Get_Custom_PropLockCursor        ,  //  <- UnityEngine.Screen::get_lockCursor
    (const void*)&Screen_Set_Custom_PropLockCursor        ,  //  <- UnityEngine.Screen::set_lockCursor
    (const void*)&Screen_Get_Custom_PropWidth             ,  //  <- UnityEngine.Screen::get_width
    (const void*)&Screen_Get_Custom_PropHeight            ,  //  <- UnityEngine.Screen::get_height
    (const void*)&Screen_Get_Custom_PropDpi               ,  //  <- UnityEngine.Screen::get_dpi
    (const void*)&Screen_Get_Custom_PropFullScreen        ,  //  <- UnityEngine.Screen::get_fullScreen
    (const void*)&Screen_Set_Custom_PropFullScreen        ,  //  <- UnityEngine.Screen::set_fullScreen
    (const void*)&Screen_Get_Custom_PropAutorotateToPortrait,  //  <- UnityEngine.Screen::get_autorotateToPortrait
    (const void*)&Screen_Set_Custom_PropAutorotateToPortrait,  //  <- UnityEngine.Screen::set_autorotateToPortrait
    (const void*)&Screen_Get_Custom_PropAutorotateToPortraitUpsideDown,  //  <- UnityEngine.Screen::get_autorotateToPortraitUpsideDown
    (const void*)&Screen_Set_Custom_PropAutorotateToPortraitUpsideDown,  //  <- UnityEngine.Screen::set_autorotateToPortraitUpsideDown
    (const void*)&Screen_Get_Custom_PropAutorotateToLandscapeLeft,  //  <- UnityEngine.Screen::get_autorotateToLandscapeLeft
    (const void*)&Screen_Set_Custom_PropAutorotateToLandscapeLeft,  //  <- UnityEngine.Screen::set_autorotateToLandscapeLeft
    (const void*)&Screen_Get_Custom_PropAutorotateToLandscapeRight,  //  <- UnityEngine.Screen::get_autorotateToLandscapeRight
    (const void*)&Screen_Set_Custom_PropAutorotateToLandscapeRight,  //  <- UnityEngine.Screen::set_autorotateToLandscapeRight
    (const void*)&Screen_Get_Custom_PropOrientation       ,  //  <- UnityEngine.Screen::get_orientation
    (const void*)&Screen_Set_Custom_PropOrientation       ,  //  <- UnityEngine.Screen::set_orientation
    (const void*)&Screen_Get_Custom_PropSleepTimeout      ,  //  <- UnityEngine.Screen::get_sleepTimeout
    (const void*)&Screen_Set_Custom_PropSleepTimeout      ,  //  <- UnityEngine.Screen::set_sleepTimeout
    (const void*)&RenderTexture_CUSTOM_Internal_CreateRenderTexture,  //  <- UnityEngine.RenderTexture::Internal_CreateRenderTexture
    (const void*)&RenderTexture_CUSTOM_GetTemporary       ,  //  <- UnityEngine.RenderTexture::GetTemporary
    (const void*)&RenderTexture_CUSTOM_ReleaseTemporary   ,  //  <- UnityEngine.RenderTexture::ReleaseTemporary
    (const void*)&RenderTexture_CUSTOM_Internal_GetWidth  ,  //  <- UnityEngine.RenderTexture::Internal_GetWidth
    (const void*)&RenderTexture_CUSTOM_Internal_SetWidth  ,  //  <- UnityEngine.RenderTexture::Internal_SetWidth
    (const void*)&RenderTexture_CUSTOM_Internal_GetHeight ,  //  <- UnityEngine.RenderTexture::Internal_GetHeight
    (const void*)&RenderTexture_CUSTOM_Internal_SetHeight ,  //  <- UnityEngine.RenderTexture::Internal_SetHeight
    (const void*)&RenderTexture_CUSTOM_Internal_SetSRGBReadWrite,  //  <- UnityEngine.RenderTexture::Internal_SetSRGBReadWrite
    (const void*)&RenderTexture_Get_Custom_PropDepth      ,  //  <- UnityEngine.RenderTexture::get_depth
    (const void*)&RenderTexture_Set_Custom_PropDepth      ,  //  <- UnityEngine.RenderTexture::set_depth
    (const void*)&RenderTexture_Get_Custom_PropIsPowerOfTwo,  //  <- UnityEngine.RenderTexture::get_isPowerOfTwo
    (const void*)&RenderTexture_Set_Custom_PropIsPowerOfTwo,  //  <- UnityEngine.RenderTexture::set_isPowerOfTwo
    (const void*)&RenderTexture_Get_Custom_PropSRGB       ,  //  <- UnityEngine.RenderTexture::get_sRGB
    (const void*)&RenderTexture_Get_Custom_PropFormat     ,  //  <- UnityEngine.RenderTexture::get_format
    (const void*)&RenderTexture_Set_Custom_PropFormat     ,  //  <- UnityEngine.RenderTexture::set_format
    (const void*)&RenderTexture_Get_Custom_PropUseMipMap  ,  //  <- UnityEngine.RenderTexture::get_useMipMap
    (const void*)&RenderTexture_Set_Custom_PropUseMipMap  ,  //  <- UnityEngine.RenderTexture::set_useMipMap
    (const void*)&RenderTexture_Get_Custom_PropIsCubemap  ,  //  <- UnityEngine.RenderTexture::get_isCubemap
    (const void*)&RenderTexture_Set_Custom_PropIsCubemap  ,  //  <- UnityEngine.RenderTexture::set_isCubemap
    (const void*)&RenderTexture_Get_Custom_PropIsVolume   ,  //  <- UnityEngine.RenderTexture::get_isVolume
    (const void*)&RenderTexture_Set_Custom_PropIsVolume   ,  //  <- UnityEngine.RenderTexture::set_isVolume
    (const void*)&RenderTexture_Get_Custom_PropVolumeDepth,  //  <- UnityEngine.RenderTexture::get_volumeDepth
    (const void*)&RenderTexture_Set_Custom_PropVolumeDepth,  //  <- UnityEngine.RenderTexture::set_volumeDepth
    (const void*)&RenderTexture_Get_Custom_PropAntiAliasing,  //  <- UnityEngine.RenderTexture::get_antiAliasing
    (const void*)&RenderTexture_Set_Custom_PropAntiAliasing,  //  <- UnityEngine.RenderTexture::set_antiAliasing
    (const void*)&RenderTexture_Get_Custom_PropEnableRandomWrite,  //  <- UnityEngine.RenderTexture::get_enableRandomWrite
    (const void*)&RenderTexture_Set_Custom_PropEnableRandomWrite,  //  <- UnityEngine.RenderTexture::set_enableRandomWrite
    (const void*)&RenderTexture_CUSTOM_INTERNAL_CALL_Create,  //  <- UnityEngine.RenderTexture::INTERNAL_CALL_Create
    (const void*)&RenderTexture_CUSTOM_INTERNAL_CALL_Release,  //  <- UnityEngine.RenderTexture::INTERNAL_CALL_Release
    (const void*)&RenderTexture_CUSTOM_INTERNAL_CALL_IsCreated,  //  <- UnityEngine.RenderTexture::INTERNAL_CALL_IsCreated
    (const void*)&RenderTexture_CUSTOM_INTERNAL_CALL_DiscardContents,  //  <- UnityEngine.RenderTexture::INTERNAL_CALL_DiscardContents
    (const void*)&RenderTexture_CUSTOM_GetColorBuffer     ,  //  <- UnityEngine.RenderTexture::GetColorBuffer
    (const void*)&RenderTexture_CUSTOM_GetDepthBuffer     ,  //  <- UnityEngine.RenderTexture::GetDepthBuffer
    (const void*)&RenderTexture_CUSTOM_SetGlobalShaderProperty,  //  <- UnityEngine.RenderTexture::SetGlobalShaderProperty
    (const void*)&RenderTexture_Get_Custom_PropActive     ,  //  <- UnityEngine.RenderTexture::get_active
    (const void*)&RenderTexture_Set_Custom_PropActive     ,  //  <- UnityEngine.RenderTexture::set_active
    (const void*)&RenderTexture_Get_Custom_PropEnabled    ,  //  <- UnityEngine.RenderTexture::get_enabled
    (const void*)&RenderTexture_Set_Custom_PropEnabled    ,  //  <- UnityEngine.RenderTexture::set_enabled
    (const void*)&RenderTexture_CUSTOM_Internal_GetTexelOffset,  //  <- UnityEngine.RenderTexture::Internal_GetTexelOffset
    (const void*)&RenderTexture_CUSTOM_SupportsStencil    ,  //  <- UnityEngine.RenderTexture::SupportsStencil
#if ENABLE_MOVIES
    (const void*)&MovieTexture_CUSTOM_INTERNAL_CALL_Play  ,  //  <- UnityEngine.MovieTexture::INTERNAL_CALL_Play
    (const void*)&MovieTexture_CUSTOM_INTERNAL_CALL_Stop  ,  //  <- UnityEngine.MovieTexture::INTERNAL_CALL_Stop
    (const void*)&MovieTexture_CUSTOM_INTERNAL_CALL_Pause ,  //  <- UnityEngine.MovieTexture::INTERNAL_CALL_Pause
#endif
#if (ENABLE_MOVIES) && (ENABLE_AUDIO)
    (const void*)&MovieTexture_Get_Custom_PropAudioClip   ,  //  <- UnityEngine.MovieTexture::get_audioClip
#endif
#if ENABLE_MOVIES
    (const void*)&MovieTexture_Get_Custom_PropLoop        ,  //  <- UnityEngine.MovieTexture::get_loop
    (const void*)&MovieTexture_Set_Custom_PropLoop        ,  //  <- UnityEngine.MovieTexture::set_loop
    (const void*)&MovieTexture_Get_Custom_PropIsPlaying   ,  //  <- UnityEngine.MovieTexture::get_isPlaying
    (const void*)&MovieTexture_Get_Custom_PropIsReadyToPlay,  //  <- UnityEngine.MovieTexture::get_isReadyToPlay
    (const void*)&MovieTexture_Get_Custom_PropDuration    ,  //  <- UnityEngine.MovieTexture::get_duration
#endif
    (const void*)&GL_CUSTOM_Vertex3                       ,  //  <- UnityEngine.GL::Vertex3
    (const void*)&GL_CUSTOM_INTERNAL_CALL_Vertex          ,  //  <- UnityEngine.GL::INTERNAL_CALL_Vertex
    (const void*)&GL_CUSTOM_INTERNAL_CALL_Color           ,  //  <- UnityEngine.GL::INTERNAL_CALL_Color
    (const void*)&GL_CUSTOM_INTERNAL_CALL_TexCoord        ,  //  <- UnityEngine.GL::INTERNAL_CALL_TexCoord
    (const void*)&GL_CUSTOM_TexCoord2                     ,  //  <- UnityEngine.GL::TexCoord2
    (const void*)&GL_CUSTOM_TexCoord3                     ,  //  <- UnityEngine.GL::TexCoord3
    (const void*)&GL_CUSTOM_MultiTexCoord2                ,  //  <- UnityEngine.GL::MultiTexCoord2
    (const void*)&GL_CUSTOM_MultiTexCoord3                ,  //  <- UnityEngine.GL::MultiTexCoord3
    (const void*)&GL_CUSTOM_INTERNAL_CALL_MultiTexCoord   ,  //  <- UnityEngine.GL::INTERNAL_CALL_MultiTexCoord
    (const void*)&GL_CUSTOM_Begin                         ,  //  <- UnityEngine.GL::Begin
    (const void*)&GL_CUSTOM_End                           ,  //  <- UnityEngine.GL::End
    (const void*)&GL_CUSTOM_LoadOrtho                     ,  //  <- UnityEngine.GL::LoadOrtho
    (const void*)&GL_CUSTOM_LoadPixelMatrix               ,  //  <- UnityEngine.GL::LoadPixelMatrix
    (const void*)&GL_CUSTOM_LoadPixelMatrixArgs           ,  //  <- UnityEngine.GL::LoadPixelMatrixArgs
    (const void*)&GL_CUSTOM_INTERNAL_CALL_Viewport        ,  //  <- UnityEngine.GL::INTERNAL_CALL_Viewport
    (const void*)&GL_CUSTOM_INTERNAL_CALL_LoadProjectionMatrix,  //  <- UnityEngine.GL::INTERNAL_CALL_LoadProjectionMatrix
    (const void*)&GL_CUSTOM_LoadIdentity                  ,  //  <- UnityEngine.GL::LoadIdentity
    (const void*)&GL_CUSTOM_INTERNAL_get_modelview        ,  //  <- UnityEngine.GL::INTERNAL_get_modelview
    (const void*)&GL_CUSTOM_INTERNAL_set_modelview        ,  //  <- UnityEngine.GL::INTERNAL_set_modelview
    (const void*)&GL_CUSTOM_INTERNAL_CALL_MultMatrix      ,  //  <- UnityEngine.GL::INTERNAL_CALL_MultMatrix
    (const void*)&GL_CUSTOM_PushMatrix                    ,  //  <- UnityEngine.GL::PushMatrix
    (const void*)&GL_CUSTOM_PopMatrix                     ,  //  <- UnityEngine.GL::PopMatrix
    (const void*)&GL_CUSTOM_INTERNAL_CALL_GetGPUProjectionMatrix,  //  <- UnityEngine.GL::INTERNAL_CALL_GetGPUProjectionMatrix
    (const void*)&GL_Get_Custom_PropWireframe             ,  //  <- UnityEngine.GL::get_wireframe
    (const void*)&GL_Set_Custom_PropWireframe             ,  //  <- UnityEngine.GL::set_wireframe
    (const void*)&GL_CUSTOM_SetRevertBackfacing           ,  //  <- UnityEngine.GL::SetRevertBackfacing
    (const void*)&GL_CUSTOM_INTERNAL_CALL_Internal_Clear  ,  //  <- UnityEngine.GL::INTERNAL_CALL_Internal_Clear
    (const void*)&GL_CUSTOM_ClearWithSkybox               ,  //  <- UnityEngine.GL::ClearWithSkybox
    (const void*)&GL_CUSTOM_InvalidateState               ,  //  <- UnityEngine.GL::InvalidateState
    (const void*)&GL_CUSTOM_IssuePluginEvent              ,  //  <- UnityEngine.GL::IssuePluginEvent
    (const void*)&GUIElement_CUSTOM_INTERNAL_CALL_HitTest ,  //  <- UnityEngine.GUIElement::INTERNAL_CALL_HitTest
    (const void*)&GUIElement_CUSTOM_INTERNAL_CALL_GetScreenRect,  //  <- UnityEngine.GUIElement::INTERNAL_CALL_GetScreenRect
    (const void*)&GUITexture_CUSTOM_INTERNAL_get_color    ,  //  <- UnityEngine.GUITexture::INTERNAL_get_color
    (const void*)&GUITexture_CUSTOM_INTERNAL_set_color    ,  //  <- UnityEngine.GUITexture::INTERNAL_set_color
    (const void*)&GUITexture_Get_Custom_PropTexture       ,  //  <- UnityEngine.GUITexture::get_texture
    (const void*)&GUITexture_Set_Custom_PropTexture       ,  //  <- UnityEngine.GUITexture::set_texture
    (const void*)&GUITexture_CUSTOM_INTERNAL_get_pixelInset,  //  <- UnityEngine.GUITexture::INTERNAL_get_pixelInset
    (const void*)&GUITexture_CUSTOM_INTERNAL_set_pixelInset,  //  <- UnityEngine.GUITexture::INTERNAL_set_pixelInset
    (const void*)&GUITexture_Get_Custom_PropBorder        ,  //  <- UnityEngine.GUITexture::get_border
    (const void*)&GUITexture_Set_Custom_PropBorder        ,  //  <- UnityEngine.GUITexture::set_border
    (const void*)&GUIText_Get_Custom_PropText             ,  //  <- UnityEngine.GUIText::get_text
    (const void*)&GUIText_Set_Custom_PropText             ,  //  <- UnityEngine.GUIText::set_text
    (const void*)&GUIText_Get_Custom_PropMaterial         ,  //  <- UnityEngine.GUIText::get_material
    (const void*)&GUIText_Set_Custom_PropMaterial         ,  //  <- UnityEngine.GUIText::set_material
    (const void*)&GUIText_CUSTOM_Internal_GetPixelOffset  ,  //  <- UnityEngine.GUIText::Internal_GetPixelOffset
    (const void*)&GUIText_CUSTOM_INTERNAL_CALL_Internal_SetPixelOffset,  //  <- UnityEngine.GUIText::INTERNAL_CALL_Internal_SetPixelOffset
    (const void*)&GUIText_Get_Custom_PropFont             ,  //  <- UnityEngine.GUIText::get_font
    (const void*)&GUIText_Set_Custom_PropFont             ,  //  <- UnityEngine.GUIText::set_font
    (const void*)&GUIText_Get_Custom_PropAlignment        ,  //  <- UnityEngine.GUIText::get_alignment
    (const void*)&GUIText_Set_Custom_PropAlignment        ,  //  <- UnityEngine.GUIText::set_alignment
    (const void*)&GUIText_Get_Custom_PropAnchor           ,  //  <- UnityEngine.GUIText::get_anchor
    (const void*)&GUIText_Set_Custom_PropAnchor           ,  //  <- UnityEngine.GUIText::set_anchor
    (const void*)&GUIText_Get_Custom_PropLineSpacing      ,  //  <- UnityEngine.GUIText::get_lineSpacing
    (const void*)&GUIText_Set_Custom_PropLineSpacing      ,  //  <- UnityEngine.GUIText::set_lineSpacing
    (const void*)&GUIText_Get_Custom_PropTabSize          ,  //  <- UnityEngine.GUIText::get_tabSize
    (const void*)&GUIText_Set_Custom_PropTabSize          ,  //  <- UnityEngine.GUIText::set_tabSize
    (const void*)&GUIText_Get_Custom_PropFontSize         ,  //  <- UnityEngine.GUIText::get_fontSize
    (const void*)&GUIText_Set_Custom_PropFontSize         ,  //  <- UnityEngine.GUIText::set_fontSize
    (const void*)&GUIText_Get_Custom_PropFontStyle        ,  //  <- UnityEngine.GUIText::get_fontStyle
    (const void*)&GUIText_Set_Custom_PropFontStyle        ,  //  <- UnityEngine.GUIText::set_fontStyle
    (const void*)&GUIText_Get_Custom_PropRichText         ,  //  <- UnityEngine.GUIText::get_richText
    (const void*)&GUIText_Set_Custom_PropRichText         ,  //  <- UnityEngine.GUIText::set_richText
    (const void*)&GUIText_CUSTOM_INTERNAL_get_color       ,  //  <- UnityEngine.GUIText::INTERNAL_get_color
    (const void*)&GUIText_CUSTOM_INTERNAL_set_color       ,  //  <- UnityEngine.GUIText::INTERNAL_set_color
    (const void*)&Font_CUSTOM_Internal_CreateFont         ,  //  <- UnityEngine.Font::Internal_CreateFont
    (const void*)&Font_Get_Custom_PropMaterial            ,  //  <- UnityEngine.Font::get_material
    (const void*)&Font_Set_Custom_PropMaterial            ,  //  <- UnityEngine.Font::set_material
    (const void*)&Font_CUSTOM_HasCharacter                ,  //  <- UnityEngine.Font::HasCharacter
#if ENABLE_MONO
    (const void*)&Font_Get_Custom_PropFontNames           ,  //  <- UnityEngine.Font::get_fontNames
    (const void*)&Font_Set_Custom_PropFontNames           ,  //  <- UnityEngine.Font::set_fontNames
#endif
    (const void*)&Font_Get_Custom_PropCharacterInfo       ,  //  <- UnityEngine.Font::get_characterInfo
    (const void*)&Font_Set_Custom_PropCharacterInfo       ,  //  <- UnityEngine.Font::set_characterInfo
    (const void*)&Font_CUSTOM_RequestCharactersInTexture  ,  //  <- UnityEngine.Font::RequestCharactersInTexture
    (const void*)&Font_CUSTOM_GetCharacterInfo            ,  //  <- UnityEngine.Font::GetCharacterInfo
    (const void*)&GUILayer_CUSTOM_INTERNAL_CALL_HitTest   ,  //  <- UnityEngine.GUILayer::INTERNAL_CALL_HitTest
#if !PLATFORM_WEBGL && !UNITY_FLASH
    (const void*)&StaticBatchingUtility_CUSTOM_InternalCombineVertices,  //  <- UnityEngine.StaticBatchingUtility::InternalCombineVertices
    (const void*)&StaticBatchingUtility_CUSTOM_InternalCombineIndices,  //  <- UnityEngine.StaticBatchingUtility::InternalCombineIndices
#endif
    NULL
};

void ExportGraphicsBindings();
void ExportGraphicsBindings()
{
    for (int i = 0; s_Graphics_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Graphics_IcallNames [i], s_Graphics_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportGraphicsBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(OcclusionArea_CUSTOM_INTERNAL_get_center); //  <- UnityEngine.OcclusionArea::INTERNAL_get_center
    SET_METRO_BINDING(OcclusionArea_CUSTOM_INTERNAL_set_center); //  <- UnityEngine.OcclusionArea::INTERNAL_set_center
    SET_METRO_BINDING(OcclusionArea_CUSTOM_INTERNAL_get_size); //  <- UnityEngine.OcclusionArea::INTERNAL_get_size
    SET_METRO_BINDING(OcclusionArea_CUSTOM_INTERNAL_set_size); //  <- UnityEngine.OcclusionArea::INTERNAL_set_size
    SET_METRO_BINDING(OcclusionPortal_Get_Custom_PropOpen); //  <- UnityEngine.OcclusionPortal::get_open
    SET_METRO_BINDING(OcclusionPortal_Set_Custom_PropOpen); //  <- UnityEngine.OcclusionPortal::set_open
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropFog); //  <- UnityEngine.RenderSettings::get_fog
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropFog); //  <- UnityEngine.RenderSettings::set_fog
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropFogMode); //  <- UnityEngine.RenderSettings::get_fogMode
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropFogMode); //  <- UnityEngine.RenderSettings::set_fogMode
    SET_METRO_BINDING(RenderSettings_CUSTOM_INTERNAL_get_fogColor); //  <- UnityEngine.RenderSettings::INTERNAL_get_fogColor
    SET_METRO_BINDING(RenderSettings_CUSTOM_INTERNAL_set_fogColor); //  <- UnityEngine.RenderSettings::INTERNAL_set_fogColor
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropFogDensity); //  <- UnityEngine.RenderSettings::get_fogDensity
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropFogDensity); //  <- UnityEngine.RenderSettings::set_fogDensity
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropFogStartDistance); //  <- UnityEngine.RenderSettings::get_fogStartDistance
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropFogStartDistance); //  <- UnityEngine.RenderSettings::set_fogStartDistance
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropFogEndDistance); //  <- UnityEngine.RenderSettings::get_fogEndDistance
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropFogEndDistance); //  <- UnityEngine.RenderSettings::set_fogEndDistance
    SET_METRO_BINDING(RenderSettings_CUSTOM_INTERNAL_get_ambientLight); //  <- UnityEngine.RenderSettings::INTERNAL_get_ambientLight
    SET_METRO_BINDING(RenderSettings_CUSTOM_INTERNAL_set_ambientLight); //  <- UnityEngine.RenderSettings::INTERNAL_set_ambientLight
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropHaloStrength); //  <- UnityEngine.RenderSettings::get_haloStrength
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropHaloStrength); //  <- UnityEngine.RenderSettings::set_haloStrength
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropFlareStrength); //  <- UnityEngine.RenderSettings::get_flareStrength
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropFlareStrength); //  <- UnityEngine.RenderSettings::set_flareStrength
    SET_METRO_BINDING(RenderSettings_Get_Custom_PropSkybox); //  <- UnityEngine.RenderSettings::get_skybox
    SET_METRO_BINDING(RenderSettings_Set_Custom_PropSkybox); //  <- UnityEngine.RenderSettings::set_skybox
#if ENABLE_MONO || PLATFORM_WINRT
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropNames); //  <- UnityEngine.QualitySettings::get_names
#endif
    SET_METRO_BINDING(QualitySettings_CUSTOM_GetQualityLevel); //  <- UnityEngine.QualitySettings::GetQualityLevel
    SET_METRO_BINDING(QualitySettings_CUSTOM_SetQualityLevel); //  <- UnityEngine.QualitySettings::SetQualityLevel
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropCurrentLevel); //  <- UnityEngine.QualitySettings::get_currentLevel
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropCurrentLevel); //  <- UnityEngine.QualitySettings::set_currentLevel
    SET_METRO_BINDING(QualitySettings_CUSTOM_IncreaseLevel); //  <- UnityEngine.QualitySettings::IncreaseLevel
    SET_METRO_BINDING(QualitySettings_CUSTOM_DecreaseLevel); //  <- UnityEngine.QualitySettings::DecreaseLevel
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropPixelLightCount); //  <- UnityEngine.QualitySettings::get_pixelLightCount
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropPixelLightCount); //  <- UnityEngine.QualitySettings::set_pixelLightCount
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropShadowProjection); //  <- UnityEngine.QualitySettings::get_shadowProjection
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropShadowProjection); //  <- UnityEngine.QualitySettings::set_shadowProjection
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropShadowCascades); //  <- UnityEngine.QualitySettings::get_shadowCascades
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropShadowCascades); //  <- UnityEngine.QualitySettings::set_shadowCascades
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropShadowDistance); //  <- UnityEngine.QualitySettings::get_shadowDistance
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropShadowDistance); //  <- UnityEngine.QualitySettings::set_shadowDistance
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropMasterTextureLimit); //  <- UnityEngine.QualitySettings::get_masterTextureLimit
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropMasterTextureLimit); //  <- UnityEngine.QualitySettings::set_masterTextureLimit
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropAnisotropicFiltering); //  <- UnityEngine.QualitySettings::get_anisotropicFiltering
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropAnisotropicFiltering); //  <- UnityEngine.QualitySettings::set_anisotropicFiltering
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropLodBias); //  <- UnityEngine.QualitySettings::get_lodBias
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropLodBias); //  <- UnityEngine.QualitySettings::set_lodBias
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropMaximumLODLevel); //  <- UnityEngine.QualitySettings::get_maximumLODLevel
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropMaximumLODLevel); //  <- UnityEngine.QualitySettings::set_maximumLODLevel
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropParticleRaycastBudget); //  <- UnityEngine.QualitySettings::get_particleRaycastBudget
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropParticleRaycastBudget); //  <- UnityEngine.QualitySettings::set_particleRaycastBudget
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropSoftVegetation); //  <- UnityEngine.QualitySettings::get_softVegetation
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropSoftVegetation); //  <- UnityEngine.QualitySettings::set_softVegetation
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropMaxQueuedFrames); //  <- UnityEngine.QualitySettings::get_maxQueuedFrames
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropMaxQueuedFrames); //  <- UnityEngine.QualitySettings::set_maxQueuedFrames
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropVSyncCount); //  <- UnityEngine.QualitySettings::get_vSyncCount
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropVSyncCount); //  <- UnityEngine.QualitySettings::set_vSyncCount
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropAntiAliasing); //  <- UnityEngine.QualitySettings::get_antiAliasing
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropAntiAliasing); //  <- UnityEngine.QualitySettings::set_antiAliasing
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropDesiredColorSpace); //  <- UnityEngine.QualitySettings::get_desiredColorSpace
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropActiveColorSpace); //  <- UnityEngine.QualitySettings::get_activeColorSpace
    SET_METRO_BINDING(QualitySettings_Get_Custom_PropBlendWeights); //  <- UnityEngine.QualitySettings::get_blendWeights
    SET_METRO_BINDING(QualitySettings_Set_Custom_PropBlendWeights); //  <- UnityEngine.QualitySettings::set_blendWeights
    SET_METRO_BINDING(Shader_CUSTOM_Find); //  <- UnityEngine.Shader::Find
    SET_METRO_BINDING(Shader_CUSTOM_FindBuiltin); //  <- UnityEngine.Shader::FindBuiltin
    SET_METRO_BINDING(Shader_Get_Custom_PropIsSupported); //  <- UnityEngine.Shader::get_isSupported
#if UNITY_EDITOR
    SET_METRO_BINDING(Shader_Get_Custom_PropCustomEditor); //  <- UnityEngine.Shader::get_customEditor
#endif
    SET_METRO_BINDING(Shader_CUSTOM_EnableKeyword); //  <- UnityEngine.Shader::EnableKeyword
    SET_METRO_BINDING(Shader_CUSTOM_DisableKeyword); //  <- UnityEngine.Shader::DisableKeyword
    SET_METRO_BINDING(Shader_Get_Custom_PropMaximumLOD); //  <- UnityEngine.Shader::get_maximumLOD
    SET_METRO_BINDING(Shader_Set_Custom_PropMaximumLOD); //  <- UnityEngine.Shader::set_maximumLOD
    SET_METRO_BINDING(Shader_Get_Custom_PropGlobalMaximumLOD); //  <- UnityEngine.Shader::get_globalMaximumLOD
    SET_METRO_BINDING(Shader_Set_Custom_PropGlobalMaximumLOD); //  <- UnityEngine.Shader::set_globalMaximumLOD
    SET_METRO_BINDING(Shader_Get_Custom_PropRenderQueue); //  <- UnityEngine.Shader::get_renderQueue
    SET_METRO_BINDING(Shader_CUSTOM_INTERNAL_CALL_SetGlobalColor); //  <- UnityEngine.Shader::INTERNAL_CALL_SetGlobalColor
    SET_METRO_BINDING(Shader_CUSTOM_SetGlobalFloat); //  <- UnityEngine.Shader::SetGlobalFloat
    SET_METRO_BINDING(Shader_CUSTOM_SetGlobalTexture); //  <- UnityEngine.Shader::SetGlobalTexture
    SET_METRO_BINDING(Shader_CUSTOM_INTERNAL_CALL_SetGlobalMatrix); //  <- UnityEngine.Shader::INTERNAL_CALL_SetGlobalMatrix
    SET_METRO_BINDING(Shader_CUSTOM_SetGlobalTexGenMode); //  <- UnityEngine.Shader::SetGlobalTexGenMode
    SET_METRO_BINDING(Shader_CUSTOM_SetGlobalTextureMatrixName); //  <- UnityEngine.Shader::SetGlobalTextureMatrixName
#if !UNITY_FLASH
    SET_METRO_BINDING(Shader_CUSTOM_SetGlobalBuffer); //  <- UnityEngine.Shader::SetGlobalBuffer
#endif
    SET_METRO_BINDING(Shader_CUSTOM_PropertyToID); //  <- UnityEngine.Shader::PropertyToID
    SET_METRO_BINDING(Shader_CUSTOM_WarmupAllShaders); //  <- UnityEngine.Shader::WarmupAllShaders
    SET_METRO_BINDING(Texture_Get_Custom_PropMasterTextureLimit); //  <- UnityEngine.Texture::get_masterTextureLimit
    SET_METRO_BINDING(Texture_Set_Custom_PropMasterTextureLimit); //  <- UnityEngine.Texture::set_masterTextureLimit
    SET_METRO_BINDING(Texture_Get_Custom_PropAnisotropicFiltering); //  <- UnityEngine.Texture::get_anisotropicFiltering
    SET_METRO_BINDING(Texture_Set_Custom_PropAnisotropicFiltering); //  <- UnityEngine.Texture::set_anisotropicFiltering
    SET_METRO_BINDING(Texture_CUSTOM_SetGlobalAnisotropicFilteringLimits); //  <- UnityEngine.Texture::SetGlobalAnisotropicFilteringLimits
    SET_METRO_BINDING(Texture_CUSTOM_Internal_GetWidth); //  <- UnityEngine.Texture::Internal_GetWidth
    SET_METRO_BINDING(Texture_CUSTOM_Internal_GetHeight); //  <- UnityEngine.Texture::Internal_GetHeight
    SET_METRO_BINDING(Texture_Get_Custom_PropFilterMode); //  <- UnityEngine.Texture::get_filterMode
    SET_METRO_BINDING(Texture_Set_Custom_PropFilterMode); //  <- UnityEngine.Texture::set_filterMode
    SET_METRO_BINDING(Texture_Get_Custom_PropAnisoLevel); //  <- UnityEngine.Texture::get_anisoLevel
    SET_METRO_BINDING(Texture_Set_Custom_PropAnisoLevel); //  <- UnityEngine.Texture::set_anisoLevel
    SET_METRO_BINDING(Texture_Get_Custom_PropWrapMode); //  <- UnityEngine.Texture::get_wrapMode
    SET_METRO_BINDING(Texture_Set_Custom_PropWrapMode); //  <- UnityEngine.Texture::set_wrapMode
    SET_METRO_BINDING(Texture_Get_Custom_PropMipMapBias); //  <- UnityEngine.Texture::get_mipMapBias
    SET_METRO_BINDING(Texture_Set_Custom_PropMipMapBias); //  <- UnityEngine.Texture::set_mipMapBias
    SET_METRO_BINDING(Texture_CUSTOM_Internal_GetTexelSize); //  <- UnityEngine.Texture::Internal_GetTexelSize
    SET_METRO_BINDING(Texture_CUSTOM_INTERNAL_CALL_GetNativeTexturePtr); //  <- UnityEngine.Texture::INTERNAL_CALL_GetNativeTexturePtr
    SET_METRO_BINDING(Texture_CUSTOM_GetNativeTextureID); //  <- UnityEngine.Texture::GetNativeTextureID
    SET_METRO_BINDING(Texture2D_Get_Custom_PropMipmapCount); //  <- UnityEngine.Texture2D::get_mipmapCount
    SET_METRO_BINDING(Texture2D_CUSTOM_Internal_Create); //  <- UnityEngine.Texture2D::Internal_Create
#if ENABLE_TEXTUREID_MAP
    SET_METRO_BINDING(Texture2D_CUSTOM_UpdateExternalTexture); //  <- UnityEngine.Texture2D::UpdateExternalTexture
#endif
    SET_METRO_BINDING(Texture2D_Get_Custom_PropFormat); //  <- UnityEngine.Texture2D::get_format
    SET_METRO_BINDING(Texture2D_CUSTOM_INTERNAL_CALL_SetPixel); //  <- UnityEngine.Texture2D::INTERNAL_CALL_SetPixel
    SET_METRO_BINDING(Texture2D_CUSTOM_INTERNAL_CALL_GetPixel); //  <- UnityEngine.Texture2D::INTERNAL_CALL_GetPixel
    SET_METRO_BINDING(Texture2D_CUSTOM_INTERNAL_CALL_GetPixelBilinear); //  <- UnityEngine.Texture2D::INTERNAL_CALL_GetPixelBilinear
    SET_METRO_BINDING(Texture2D_CUSTOM_SetPixels); //  <- UnityEngine.Texture2D::SetPixels
    SET_METRO_BINDING(Texture2D_CUSTOM_SetPixels32); //  <- UnityEngine.Texture2D::SetPixels32
    SET_METRO_BINDING(Texture2D_CUSTOM_LoadImage); //  <- UnityEngine.Texture2D::LoadImage
    SET_METRO_BINDING(Texture2D_CUSTOM_GetPixels); //  <- UnityEngine.Texture2D::GetPixels
    SET_METRO_BINDING(Texture2D_CUSTOM_GetPixels32); //  <- UnityEngine.Texture2D::GetPixels32
    SET_METRO_BINDING(Texture2D_CUSTOM_Apply); //  <- UnityEngine.Texture2D::Apply
    SET_METRO_BINDING(Texture2D_CUSTOM_Resize); //  <- UnityEngine.Texture2D::Resize
    SET_METRO_BINDING(Texture2D_CUSTOM_Internal_ResizeWH); //  <- UnityEngine.Texture2D::Internal_ResizeWH
    SET_METRO_BINDING(Texture2D_CUSTOM_INTERNAL_CALL_Compress); //  <- UnityEngine.Texture2D::INTERNAL_CALL_Compress
    SET_METRO_BINDING(Texture2D_CUSTOM_PackTextures); //  <- UnityEngine.Texture2D::PackTextures
    SET_METRO_BINDING(Texture2D_CUSTOM_INTERNAL_CALL_ReadPixels); //  <- UnityEngine.Texture2D::INTERNAL_CALL_ReadPixels
    SET_METRO_BINDING(Texture2D_CUSTOM_EncodeToPNG); //  <- UnityEngine.Texture2D::EncodeToPNG
#if UNITY_EDITOR
    SET_METRO_BINDING(Texture2D_Get_Custom_PropAlphaIsTransparency); //  <- UnityEngine.Texture2D::get_alphaIsTransparency
    SET_METRO_BINDING(Texture2D_Set_Custom_PropAlphaIsTransparency); //  <- UnityEngine.Texture2D::set_alphaIsTransparency
#endif
    SET_METRO_BINDING(Cubemap_CUSTOM_INTERNAL_CALL_SetPixel); //  <- UnityEngine.Cubemap::INTERNAL_CALL_SetPixel
    SET_METRO_BINDING(Cubemap_CUSTOM_INTERNAL_CALL_GetPixel); //  <- UnityEngine.Cubemap::INTERNAL_CALL_GetPixel
    SET_METRO_BINDING(Cubemap_CUSTOM_GetPixels); //  <- UnityEngine.Cubemap::GetPixels
    SET_METRO_BINDING(Cubemap_CUSTOM_SetPixels); //  <- UnityEngine.Cubemap::SetPixels
    SET_METRO_BINDING(Cubemap_CUSTOM_Apply); //  <- UnityEngine.Cubemap::Apply
    SET_METRO_BINDING(Cubemap_Get_Custom_PropFormat); //  <- UnityEngine.Cubemap::get_format
    SET_METRO_BINDING(Cubemap_CUSTOM_Internal_Create); //  <- UnityEngine.Cubemap::Internal_Create
    SET_METRO_BINDING(Cubemap_CUSTOM_SmoothEdges); //  <- UnityEngine.Cubemap::SmoothEdges
    SET_METRO_BINDING(Texture3D_Get_Custom_PropDepth); //  <- UnityEngine.Texture3D::get_depth
    SET_METRO_BINDING(Texture3D_CUSTOM_GetPixels); //  <- UnityEngine.Texture3D::GetPixels
    SET_METRO_BINDING(Texture3D_CUSTOM_SetPixels); //  <- UnityEngine.Texture3D::SetPixels
    SET_METRO_BINDING(Texture3D_CUSTOM_Apply); //  <- UnityEngine.Texture3D::Apply
    SET_METRO_BINDING(Texture3D_Get_Custom_PropFormat); //  <- UnityEngine.Texture3D::get_format
    SET_METRO_BINDING(Texture3D_CUSTOM_Internal_Create); //  <- UnityEngine.Texture3D::Internal_Create
    SET_METRO_BINDING(MeshFilter_Get_Custom_PropMesh); //  <- UnityEngine.MeshFilter::get_mesh
    SET_METRO_BINDING(MeshFilter_Set_Custom_PropMesh); //  <- UnityEngine.MeshFilter::set_mesh
    SET_METRO_BINDING(MeshFilter_Get_Custom_PropSharedMesh); //  <- UnityEngine.MeshFilter::get_sharedMesh
    SET_METRO_BINDING(MeshFilter_Set_Custom_PropSharedMesh); //  <- UnityEngine.MeshFilter::set_sharedMesh
    SET_METRO_BINDING(CombineInstance_CUSTOM_InternalGetMesh); //  <- UnityEngine.CombineInstance::InternalGetMesh
    SET_METRO_BINDING(Mesh_CUSTOM_Internal_Create); //  <- UnityEngine.Mesh::Internal_Create
    SET_METRO_BINDING(Mesh_CUSTOM_Clear); //  <- UnityEngine.Mesh::Clear
    SET_METRO_BINDING(Mesh_Get_Custom_PropIsReadable); //  <- UnityEngine.Mesh::get_isReadable
    SET_METRO_BINDING(Mesh_Get_Custom_PropCanAccess); //  <- UnityEngine.Mesh::get_canAccess
    SET_METRO_BINDING(Mesh_Get_Custom_PropVertices); //  <- UnityEngine.Mesh::get_vertices
    SET_METRO_BINDING(Mesh_Set_Custom_PropVertices); //  <- UnityEngine.Mesh::set_vertices
    SET_METRO_BINDING(Mesh_Get_Custom_PropNormals); //  <- UnityEngine.Mesh::get_normals
    SET_METRO_BINDING(Mesh_Set_Custom_PropNormals); //  <- UnityEngine.Mesh::set_normals
    SET_METRO_BINDING(Mesh_Get_Custom_PropTangents); //  <- UnityEngine.Mesh::get_tangents
    SET_METRO_BINDING(Mesh_Set_Custom_PropTangents); //  <- UnityEngine.Mesh::set_tangents
    SET_METRO_BINDING(Mesh_Get_Custom_PropUv); //  <- UnityEngine.Mesh::get_uv
    SET_METRO_BINDING(Mesh_Set_Custom_PropUv); //  <- UnityEngine.Mesh::set_uv
    SET_METRO_BINDING(Mesh_Get_Custom_PropUv2); //  <- UnityEngine.Mesh::get_uv2
    SET_METRO_BINDING(Mesh_Set_Custom_PropUv2); //  <- UnityEngine.Mesh::set_uv2
    SET_METRO_BINDING(Mesh_CUSTOM_INTERNAL_get_bounds); //  <- UnityEngine.Mesh::INTERNAL_get_bounds
    SET_METRO_BINDING(Mesh_CUSTOM_INTERNAL_set_bounds); //  <- UnityEngine.Mesh::INTERNAL_set_bounds
    SET_METRO_BINDING(Mesh_Get_Custom_PropColors); //  <- UnityEngine.Mesh::get_colors
    SET_METRO_BINDING(Mesh_Set_Custom_PropColors); //  <- UnityEngine.Mesh::set_colors
    SET_METRO_BINDING(Mesh_Get_Custom_PropColors32); //  <- UnityEngine.Mesh::get_colors32
    SET_METRO_BINDING(Mesh_Set_Custom_PropColors32); //  <- UnityEngine.Mesh::set_colors32
    SET_METRO_BINDING(Mesh_CUSTOM_RecalculateBounds); //  <- UnityEngine.Mesh::RecalculateBounds
    SET_METRO_BINDING(Mesh_CUSTOM_RecalculateNormals); //  <- UnityEngine.Mesh::RecalculateNormals
    SET_METRO_BINDING(Mesh_CUSTOM_Optimize); //  <- UnityEngine.Mesh::Optimize
    SET_METRO_BINDING(Mesh_Get_Custom_PropTriangles); //  <- UnityEngine.Mesh::get_triangles
    SET_METRO_BINDING(Mesh_Set_Custom_PropTriangles); //  <- UnityEngine.Mesh::set_triangles
    SET_METRO_BINDING(Mesh_CUSTOM_GetTriangles); //  <- UnityEngine.Mesh::GetTriangles
    SET_METRO_BINDING(Mesh_CUSTOM_SetTriangles); //  <- UnityEngine.Mesh::SetTriangles
    SET_METRO_BINDING(Mesh_CUSTOM_GetIndices); //  <- UnityEngine.Mesh::GetIndices
    SET_METRO_BINDING(Mesh_CUSTOM_SetIndices); //  <- UnityEngine.Mesh::SetIndices
    SET_METRO_BINDING(Mesh_CUSTOM_GetTopology); //  <- UnityEngine.Mesh::GetTopology
    SET_METRO_BINDING(Mesh_Get_Custom_PropVertexCount); //  <- UnityEngine.Mesh::get_vertexCount
    SET_METRO_BINDING(Mesh_Get_Custom_PropSubMeshCount); //  <- UnityEngine.Mesh::get_subMeshCount
    SET_METRO_BINDING(Mesh_Set_Custom_PropSubMeshCount); //  <- UnityEngine.Mesh::set_subMeshCount
    SET_METRO_BINDING(Mesh_CUSTOM_SetTriangleStrip); //  <- UnityEngine.Mesh::SetTriangleStrip
    SET_METRO_BINDING(Mesh_CUSTOM_GetTriangleStrip); //  <- UnityEngine.Mesh::GetTriangleStrip
    SET_METRO_BINDING(Mesh_CUSTOM_CombineMeshes); //  <- UnityEngine.Mesh::CombineMeshes
    SET_METRO_BINDING(Mesh_Get_Custom_PropBoneWeights); //  <- UnityEngine.Mesh::get_boneWeights
    SET_METRO_BINDING(Mesh_Set_Custom_PropBoneWeights); //  <- UnityEngine.Mesh::set_boneWeights
    SET_METRO_BINDING(Mesh_Get_Custom_PropBindposes); //  <- UnityEngine.Mesh::get_bindposes
    SET_METRO_BINDING(Mesh_Set_Custom_PropBindposes); //  <- UnityEngine.Mesh::set_bindposes
    SET_METRO_BINDING(Mesh_CUSTOM_MarkDynamic); //  <- UnityEngine.Mesh::MarkDynamic
#if USE_BLENDSHAPES
    SET_METRO_BINDING(Mesh_Get_Custom_PropBlendShapes); //  <- UnityEngine.Mesh::get_blendShapes
    SET_METRO_BINDING(Mesh_Set_Custom_PropBlendShapes); //  <- UnityEngine.Mesh::set_blendShapes
    SET_METRO_BINDING(Mesh_Get_Custom_PropBlendShapeCount); //  <- UnityEngine.Mesh::get_blendShapeCount
    SET_METRO_BINDING(Mesh_CUSTOM_GetBlendShapeName); //  <- UnityEngine.Mesh::GetBlendShapeName
#endif
    SET_METRO_BINDING(SkinnedMeshRenderer_Get_Custom_PropBones); //  <- UnityEngine.SkinnedMeshRenderer::get_bones
    SET_METRO_BINDING(SkinnedMeshRenderer_Set_Custom_PropBones); //  <- UnityEngine.SkinnedMeshRenderer::set_bones
    SET_METRO_BINDING(SkinnedMeshRenderer_Get_Custom_PropRootBone); //  <- UnityEngine.SkinnedMeshRenderer::get_rootBone
    SET_METRO_BINDING(SkinnedMeshRenderer_Set_Custom_PropRootBone); //  <- UnityEngine.SkinnedMeshRenderer::set_rootBone
    SET_METRO_BINDING(SkinnedMeshRenderer_Get_Custom_PropQuality); //  <- UnityEngine.SkinnedMeshRenderer::get_quality
    SET_METRO_BINDING(SkinnedMeshRenderer_Set_Custom_PropQuality); //  <- UnityEngine.SkinnedMeshRenderer::set_quality
    SET_METRO_BINDING(SkinnedMeshRenderer_Get_Custom_PropSharedMesh); //  <- UnityEngine.SkinnedMeshRenderer::get_sharedMesh
    SET_METRO_BINDING(SkinnedMeshRenderer_Set_Custom_PropSharedMesh); //  <- UnityEngine.SkinnedMeshRenderer::set_sharedMesh
    SET_METRO_BINDING(SkinnedMeshRenderer_Get_Custom_PropUpdateWhenOffscreen); //  <- UnityEngine.SkinnedMeshRenderer::get_updateWhenOffscreen
    SET_METRO_BINDING(SkinnedMeshRenderer_Set_Custom_PropUpdateWhenOffscreen); //  <- UnityEngine.SkinnedMeshRenderer::set_updateWhenOffscreen
    SET_METRO_BINDING(SkinnedMeshRenderer_CUSTOM_INTERNAL_get_localBounds); //  <- UnityEngine.SkinnedMeshRenderer::INTERNAL_get_localBounds
    SET_METRO_BINDING(SkinnedMeshRenderer_CUSTOM_INTERNAL_set_localBounds); //  <- UnityEngine.SkinnedMeshRenderer::INTERNAL_set_localBounds
    SET_METRO_BINDING(SkinnedMeshRenderer_CUSTOM_BakeMesh); //  <- UnityEngine.SkinnedMeshRenderer::BakeMesh
#if UNITY_EDITOR
    SET_METRO_BINDING(SkinnedMeshRenderer_Get_Custom_PropActualRootBone); //  <- UnityEngine.SkinnedMeshRenderer::get_actualRootBone
#endif
#if USE_BLENDSHAPES
    SET_METRO_BINDING(SkinnedMeshRenderer_CUSTOM_GetBlendShapeWeight); //  <- UnityEngine.SkinnedMeshRenderer::GetBlendShapeWeight
    SET_METRO_BINDING(SkinnedMeshRenderer_CUSTOM_SetBlendShapeWeight); //  <- UnityEngine.SkinnedMeshRenderer::SetBlendShapeWeight
#endif
    SET_METRO_BINDING(Material_Get_Custom_PropShader); //  <- UnityEngine.Material::get_shader
    SET_METRO_BINDING(Material_Set_Custom_PropShader); //  <- UnityEngine.Material::set_shader
    SET_METRO_BINDING(Material_CUSTOM_INTERNAL_CALL_SetColor); //  <- UnityEngine.Material::INTERNAL_CALL_SetColor
    SET_METRO_BINDING(Material_CUSTOM_INTERNAL_CALL_GetColor); //  <- UnityEngine.Material::INTERNAL_CALL_GetColor
    SET_METRO_BINDING(Material_CUSTOM_SetTexture); //  <- UnityEngine.Material::SetTexture
    SET_METRO_BINDING(Material_CUSTOM_GetTexture); //  <- UnityEngine.Material::GetTexture
    SET_METRO_BINDING(Material_CUSTOM_Internal_GetTextureOffset); //  <- UnityEngine.Material::Internal_GetTextureOffset
    SET_METRO_BINDING(Material_CUSTOM_Internal_GetTextureScale); //  <- UnityEngine.Material::Internal_GetTextureScale
    SET_METRO_BINDING(Material_CUSTOM_INTERNAL_CALL_SetTextureOffset); //  <- UnityEngine.Material::INTERNAL_CALL_SetTextureOffset
    SET_METRO_BINDING(Material_CUSTOM_INTERNAL_CALL_SetTextureScale); //  <- UnityEngine.Material::INTERNAL_CALL_SetTextureScale
    SET_METRO_BINDING(Material_CUSTOM_INTERNAL_CALL_SetMatrix); //  <- UnityEngine.Material::INTERNAL_CALL_SetMatrix
    SET_METRO_BINDING(Material_CUSTOM_INTERNAL_CALL_GetMatrix); //  <- UnityEngine.Material::INTERNAL_CALL_GetMatrix
    SET_METRO_BINDING(Material_CUSTOM_SetFloat); //  <- UnityEngine.Material::SetFloat
    SET_METRO_BINDING(Material_CUSTOM_GetFloat); //  <- UnityEngine.Material::GetFloat
#if !UNITY_FLASH
    SET_METRO_BINDING(Material_CUSTOM_SetBuffer); //  <- UnityEngine.Material::SetBuffer
#endif
    SET_METRO_BINDING(Material_CUSTOM_HasProperty); //  <- UnityEngine.Material::HasProperty
    SET_METRO_BINDING(Material_CUSTOM_GetTag); //  <- UnityEngine.Material::GetTag
    SET_METRO_BINDING(Material_CUSTOM_Lerp); //  <- UnityEngine.Material::Lerp
    SET_METRO_BINDING(Material_Get_Custom_PropPassCount); //  <- UnityEngine.Material::get_passCount
    SET_METRO_BINDING(Material_CUSTOM_SetPass); //  <- UnityEngine.Material::SetPass
    SET_METRO_BINDING(Material_Get_Custom_PropRenderQueue); //  <- UnityEngine.Material::get_renderQueue
    SET_METRO_BINDING(Material_Set_Custom_PropRenderQueue); //  <- UnityEngine.Material::set_renderQueue
    SET_METRO_BINDING(Material_CUSTOM_Internal_CreateWithString); //  <- UnityEngine.Material::Internal_CreateWithString
    SET_METRO_BINDING(Material_CUSTOM_Internal_CreateWithShader); //  <- UnityEngine.Material::Internal_CreateWithShader
    SET_METRO_BINDING(Material_CUSTOM_Internal_CreateWithMaterial); //  <- UnityEngine.Material::Internal_CreateWithMaterial
    SET_METRO_BINDING(Material_CUSTOM_CopyPropertiesFromMaterial); //  <- UnityEngine.Material::CopyPropertiesFromMaterial
#if ENABLE_MONO || PLATFORM_WINRT
    SET_METRO_BINDING(Material_Get_Custom_PropShaderKeywords); //  <- UnityEngine.Material::get_shaderKeywords
    SET_METRO_BINDING(Material_Set_Custom_PropShaderKeywords); //  <- UnityEngine.Material::set_shaderKeywords
#endif
    SET_METRO_BINDING(LensFlare_Get_Custom_PropFlare); //  <- UnityEngine.LensFlare::get_flare
    SET_METRO_BINDING(LensFlare_Set_Custom_PropFlare); //  <- UnityEngine.LensFlare::set_flare
    SET_METRO_BINDING(LensFlare_Get_Custom_PropBrightness); //  <- UnityEngine.LensFlare::get_brightness
    SET_METRO_BINDING(LensFlare_Set_Custom_PropBrightness); //  <- UnityEngine.LensFlare::set_brightness
    SET_METRO_BINDING(LensFlare_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.LensFlare::INTERNAL_get_color
    SET_METRO_BINDING(LensFlare_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.LensFlare::INTERNAL_set_color
    SET_METRO_BINDING(Renderer_Get_Custom_PropStaticBatchRootTransform); //  <- UnityEngine.Renderer::get_staticBatchRootTransform
    SET_METRO_BINDING(Renderer_Set_Custom_PropStaticBatchRootTransform); //  <- UnityEngine.Renderer::set_staticBatchRootTransform
    SET_METRO_BINDING(Renderer_Get_Custom_PropStaticBatchIndex); //  <- UnityEngine.Renderer::get_staticBatchIndex
    SET_METRO_BINDING(Renderer_CUSTOM_SetSubsetIndex); //  <- UnityEngine.Renderer::SetSubsetIndex
    SET_METRO_BINDING(Renderer_Get_Custom_PropIsPartOfStaticBatch); //  <- UnityEngine.Renderer::get_isPartOfStaticBatch
    SET_METRO_BINDING(Renderer_CUSTOM_INTERNAL_get_worldToLocalMatrix); //  <- UnityEngine.Renderer::INTERNAL_get_worldToLocalMatrix
    SET_METRO_BINDING(Renderer_CUSTOM_INTERNAL_get_localToWorldMatrix); //  <- UnityEngine.Renderer::INTERNAL_get_localToWorldMatrix
    SET_METRO_BINDING(Renderer_Get_Custom_PropEnabled); //  <- UnityEngine.Renderer::get_enabled
    SET_METRO_BINDING(Renderer_Set_Custom_PropEnabled); //  <- UnityEngine.Renderer::set_enabled
    SET_METRO_BINDING(Renderer_Get_Custom_PropCastShadows); //  <- UnityEngine.Renderer::get_castShadows
    SET_METRO_BINDING(Renderer_Set_Custom_PropCastShadows); //  <- UnityEngine.Renderer::set_castShadows
    SET_METRO_BINDING(Renderer_Get_Custom_PropReceiveShadows); //  <- UnityEngine.Renderer::get_receiveShadows
    SET_METRO_BINDING(Renderer_Set_Custom_PropReceiveShadows); //  <- UnityEngine.Renderer::set_receiveShadows
    SET_METRO_BINDING(Renderer_Get_Custom_PropMaterial); //  <- UnityEngine.Renderer::get_material
    SET_METRO_BINDING(Renderer_Set_Custom_PropMaterial); //  <- UnityEngine.Renderer::set_material
    SET_METRO_BINDING(Renderer_Get_Custom_PropSharedMaterial); //  <- UnityEngine.Renderer::get_sharedMaterial
    SET_METRO_BINDING(Renderer_Set_Custom_PropSharedMaterial); //  <- UnityEngine.Renderer::set_sharedMaterial
    SET_METRO_BINDING(Renderer_Get_Custom_PropSharedMaterials); //  <- UnityEngine.Renderer::get_sharedMaterials
    SET_METRO_BINDING(Renderer_Set_Custom_PropSharedMaterials); //  <- UnityEngine.Renderer::set_sharedMaterials
    SET_METRO_BINDING(Renderer_Get_Custom_PropMaterials); //  <- UnityEngine.Renderer::get_materials
    SET_METRO_BINDING(Renderer_Set_Custom_PropMaterials); //  <- UnityEngine.Renderer::set_materials
    SET_METRO_BINDING(Renderer_CUSTOM_INTERNAL_get_bounds); //  <- UnityEngine.Renderer::INTERNAL_get_bounds
    SET_METRO_BINDING(Renderer_Get_Custom_PropLightmapIndex); //  <- UnityEngine.Renderer::get_lightmapIndex
    SET_METRO_BINDING(Renderer_Set_Custom_PropLightmapIndex); //  <- UnityEngine.Renderer::set_lightmapIndex
    SET_METRO_BINDING(Renderer_CUSTOM_INTERNAL_get_lightmapTilingOffset); //  <- UnityEngine.Renderer::INTERNAL_get_lightmapTilingOffset
    SET_METRO_BINDING(Renderer_CUSTOM_INTERNAL_set_lightmapTilingOffset); //  <- UnityEngine.Renderer::INTERNAL_set_lightmapTilingOffset
    SET_METRO_BINDING(Renderer_Get_Custom_PropIsVisible); //  <- UnityEngine.Renderer::get_isVisible
    SET_METRO_BINDING(Renderer_Get_Custom_PropUseLightProbes); //  <- UnityEngine.Renderer::get_useLightProbes
    SET_METRO_BINDING(Renderer_Set_Custom_PropUseLightProbes); //  <- UnityEngine.Renderer::set_useLightProbes
    SET_METRO_BINDING(Renderer_Get_Custom_PropLightProbeAnchor); //  <- UnityEngine.Renderer::get_lightProbeAnchor
    SET_METRO_BINDING(Renderer_Set_Custom_PropLightProbeAnchor); //  <- UnityEngine.Renderer::set_lightProbeAnchor
    SET_METRO_BINDING(Renderer_CUSTOM_SetPropertyBlock); //  <- UnityEngine.Renderer::SetPropertyBlock
    SET_METRO_BINDING(Renderer_CUSTOM_Render); //  <- UnityEngine.Renderer::Render
    SET_METRO_BINDING(Projector_Get_Custom_PropNearClipPlane); //  <- UnityEngine.Projector::get_nearClipPlane
    SET_METRO_BINDING(Projector_Set_Custom_PropNearClipPlane); //  <- UnityEngine.Projector::set_nearClipPlane
    SET_METRO_BINDING(Projector_Get_Custom_PropFarClipPlane); //  <- UnityEngine.Projector::get_farClipPlane
    SET_METRO_BINDING(Projector_Set_Custom_PropFarClipPlane); //  <- UnityEngine.Projector::set_farClipPlane
    SET_METRO_BINDING(Projector_Get_Custom_PropFieldOfView); //  <- UnityEngine.Projector::get_fieldOfView
    SET_METRO_BINDING(Projector_Set_Custom_PropFieldOfView); //  <- UnityEngine.Projector::set_fieldOfView
    SET_METRO_BINDING(Projector_Get_Custom_PropAspectRatio); //  <- UnityEngine.Projector::get_aspectRatio
    SET_METRO_BINDING(Projector_Set_Custom_PropAspectRatio); //  <- UnityEngine.Projector::set_aspectRatio
    SET_METRO_BINDING(Projector_Get_Custom_PropOrthographic); //  <- UnityEngine.Projector::get_orthographic
    SET_METRO_BINDING(Projector_Set_Custom_PropOrthographic); //  <- UnityEngine.Projector::set_orthographic
    SET_METRO_BINDING(Projector_Get_Custom_PropOrthographicSize); //  <- UnityEngine.Projector::get_orthographicSize
    SET_METRO_BINDING(Projector_Set_Custom_PropOrthographicSize); //  <- UnityEngine.Projector::set_orthographicSize
    SET_METRO_BINDING(Projector_Get_Custom_PropIgnoreLayers); //  <- UnityEngine.Projector::get_ignoreLayers
    SET_METRO_BINDING(Projector_Set_Custom_PropIgnoreLayers); //  <- UnityEngine.Projector::set_ignoreLayers
    SET_METRO_BINDING(Projector_Get_Custom_PropMaterial); //  <- UnityEngine.Projector::get_material
    SET_METRO_BINDING(Projector_Set_Custom_PropMaterial); //  <- UnityEngine.Projector::set_material
    SET_METRO_BINDING(Skybox_Get_Custom_PropMaterial); //  <- UnityEngine.Skybox::get_material
    SET_METRO_BINDING(Skybox_Set_Custom_PropMaterial); //  <- UnityEngine.Skybox::set_material
    SET_METRO_BINDING(TextMesh_Get_Custom_PropText); //  <- UnityEngine.TextMesh::get_text
    SET_METRO_BINDING(TextMesh_Set_Custom_PropText); //  <- UnityEngine.TextMesh::set_text
    SET_METRO_BINDING(TextMesh_Get_Custom_PropFont); //  <- UnityEngine.TextMesh::get_font
    SET_METRO_BINDING(TextMesh_Set_Custom_PropFont); //  <- UnityEngine.TextMesh::set_font
    SET_METRO_BINDING(TextMesh_Get_Custom_PropFontSize); //  <- UnityEngine.TextMesh::get_fontSize
    SET_METRO_BINDING(TextMesh_Set_Custom_PropFontSize); //  <- UnityEngine.TextMesh::set_fontSize
    SET_METRO_BINDING(TextMesh_Get_Custom_PropFontStyle); //  <- UnityEngine.TextMesh::get_fontStyle
    SET_METRO_BINDING(TextMesh_Set_Custom_PropFontStyle); //  <- UnityEngine.TextMesh::set_fontStyle
    SET_METRO_BINDING(TextMesh_Get_Custom_PropOffsetZ); //  <- UnityEngine.TextMesh::get_offsetZ
    SET_METRO_BINDING(TextMesh_Set_Custom_PropOffsetZ); //  <- UnityEngine.TextMesh::set_offsetZ
    SET_METRO_BINDING(TextMesh_Get_Custom_PropAlignment); //  <- UnityEngine.TextMesh::get_alignment
    SET_METRO_BINDING(TextMesh_Set_Custom_PropAlignment); //  <- UnityEngine.TextMesh::set_alignment
    SET_METRO_BINDING(TextMesh_Get_Custom_PropAnchor); //  <- UnityEngine.TextMesh::get_anchor
    SET_METRO_BINDING(TextMesh_Set_Custom_PropAnchor); //  <- UnityEngine.TextMesh::set_anchor
    SET_METRO_BINDING(TextMesh_Get_Custom_PropCharacterSize); //  <- UnityEngine.TextMesh::get_characterSize
    SET_METRO_BINDING(TextMesh_Set_Custom_PropCharacterSize); //  <- UnityEngine.TextMesh::set_characterSize
    SET_METRO_BINDING(TextMesh_Get_Custom_PropLineSpacing); //  <- UnityEngine.TextMesh::get_lineSpacing
    SET_METRO_BINDING(TextMesh_Set_Custom_PropLineSpacing); //  <- UnityEngine.TextMesh::set_lineSpacing
    SET_METRO_BINDING(TextMesh_Get_Custom_PropTabSize); //  <- UnityEngine.TextMesh::get_tabSize
    SET_METRO_BINDING(TextMesh_Set_Custom_PropTabSize); //  <- UnityEngine.TextMesh::set_tabSize
    SET_METRO_BINDING(TextMesh_Get_Custom_PropRichText); //  <- UnityEngine.TextMesh::get_richText
    SET_METRO_BINDING(TextMesh_Set_Custom_PropRichText); //  <- UnityEngine.TextMesh::set_richText
    SET_METRO_BINDING(TextMesh_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.TextMesh::INTERNAL_get_color
    SET_METRO_BINDING(TextMesh_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.TextMesh::INTERNAL_set_color
    SET_METRO_BINDING(TrailRenderer_Get_Custom_PropTime); //  <- UnityEngine.TrailRenderer::get_time
    SET_METRO_BINDING(TrailRenderer_Set_Custom_PropTime); //  <- UnityEngine.TrailRenderer::set_time
    SET_METRO_BINDING(TrailRenderer_Get_Custom_PropStartWidth); //  <- UnityEngine.TrailRenderer::get_startWidth
    SET_METRO_BINDING(TrailRenderer_Set_Custom_PropStartWidth); //  <- UnityEngine.TrailRenderer::set_startWidth
    SET_METRO_BINDING(TrailRenderer_Get_Custom_PropEndWidth); //  <- UnityEngine.TrailRenderer::get_endWidth
    SET_METRO_BINDING(TrailRenderer_Set_Custom_PropEndWidth); //  <- UnityEngine.TrailRenderer::set_endWidth
    SET_METRO_BINDING(TrailRenderer_Get_Custom_PropAutodestruct); //  <- UnityEngine.TrailRenderer::get_autodestruct
    SET_METRO_BINDING(TrailRenderer_Set_Custom_PropAutodestruct); //  <- UnityEngine.TrailRenderer::set_autodestruct
    SET_METRO_BINDING(LineRenderer_CUSTOM_INTERNAL_CALL_SetWidth); //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetWidth
    SET_METRO_BINDING(LineRenderer_CUSTOM_INTERNAL_CALL_SetColors); //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetColors
    SET_METRO_BINDING(LineRenderer_CUSTOM_INTERNAL_CALL_SetVertexCount); //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetVertexCount
    SET_METRO_BINDING(LineRenderer_CUSTOM_INTERNAL_CALL_SetPosition); //  <- UnityEngine.LineRenderer::INTERNAL_CALL_SetPosition
    SET_METRO_BINDING(LineRenderer_Get_Custom_PropUseWorldSpace); //  <- UnityEngine.LineRenderer::get_useWorldSpace
    SET_METRO_BINDING(LineRenderer_Set_Custom_PropUseWorldSpace); //  <- UnityEngine.LineRenderer::set_useWorldSpace
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_InitBlock); //  <- UnityEngine.MaterialPropertyBlock::InitBlock
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_DestroyBlock); //  <- UnityEngine.MaterialPropertyBlock::DestroyBlock
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_AddFloat); //  <- UnityEngine.MaterialPropertyBlock::AddFloat
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddVector); //  <- UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddVector
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddColor); //  <- UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddColor
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_INTERNAL_CALL_AddMatrix); //  <- UnityEngine.MaterialPropertyBlock::INTERNAL_CALL_AddMatrix
    SET_METRO_BINDING(MaterialPropertyBlock_CUSTOM_Clear); //  <- UnityEngine.MaterialPropertyBlock::Clear
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_DrawMeshTR); //  <- UnityEngine.Graphics::Internal_DrawMeshTR
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_DrawMeshMatrix); //  <- UnityEngine.Graphics::Internal_DrawMeshMatrix
    SET_METRO_BINDING(Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow1); //  <- UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow1
    SET_METRO_BINDING(Graphics_CUSTOM_INTERNAL_CALL_Internal_DrawMeshNow2); //  <- UnityEngine.Graphics::INTERNAL_CALL_Internal_DrawMeshNow2
    SET_METRO_BINDING(Graphics_CUSTOM_DrawProcedural); //  <- UnityEngine.Graphics::DrawProcedural
#if !UNITY_FLASH
    SET_METRO_BINDING(Graphics_CUSTOM_DrawProceduralIndirect); //  <- UnityEngine.Graphics::DrawProceduralIndirect
#endif
    SET_METRO_BINDING(Graphics_CUSTOM_DrawTexture); //  <- UnityEngine.Graphics::DrawTexture
    SET_METRO_BINDING(Graphics_CUSTOM_Blit); //  <- UnityEngine.Graphics::Blit
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_BlitMaterial); //  <- UnityEngine.Graphics::Internal_BlitMaterial
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_BlitMultiTap); //  <- UnityEngine.Graphics::Internal_BlitMultiTap
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_SetRT); //  <- UnityEngine.Graphics::Internal_SetRT
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_SetRTBuffer); //  <- UnityEngine.Graphics::Internal_SetRTBuffer
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_SetRTBuffers); //  <- UnityEngine.Graphics::Internal_SetRTBuffers
    SET_METRO_BINDING(Graphics_CUSTOM_GetActiveColorBuffer); //  <- UnityEngine.Graphics::GetActiveColorBuffer
    SET_METRO_BINDING(Graphics_CUSTOM_GetActiveDepthBuffer); //  <- UnityEngine.Graphics::GetActiveDepthBuffer
    SET_METRO_BINDING(Graphics_CUSTOM_ClearRandomWriteTargets); //  <- UnityEngine.Graphics::ClearRandomWriteTargets
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_SetRandomWriteTargetRT); //  <- UnityEngine.Graphics::Internal_SetRandomWriteTargetRT
#if !UNITY_FLASH
    SET_METRO_BINDING(Graphics_CUSTOM_Internal_SetRandomWriteTargetBuffer); //  <- UnityEngine.Graphics::Internal_SetRandomWriteTargetBuffer
#endif
    SET_METRO_BINDING(Graphics_CUSTOM_SetupVertexLights); //  <- UnityEngine.Graphics::SetupVertexLights
#if ENABLE_MONO || PLATFORM_WINRT
    SET_METRO_BINDING(LightProbes_CUSTOM_INTERNAL_CALL_GetInterpolatedLightProbe); //  <- UnityEngine.LightProbes::INTERNAL_CALL_GetInterpolatedLightProbe
    SET_METRO_BINDING(LightProbes_Get_Custom_PropPositions); //  <- UnityEngine.LightProbes::get_positions
    SET_METRO_BINDING(LightProbes_Get_Custom_PropCoefficients); //  <- UnityEngine.LightProbes::get_coefficients
    SET_METRO_BINDING(LightProbes_Set_Custom_PropCoefficients); //  <- UnityEngine.LightProbes::set_coefficients
    SET_METRO_BINDING(LightProbes_Get_Custom_PropCount); //  <- UnityEngine.LightProbes::get_count
    SET_METRO_BINDING(LightProbes_Get_Custom_PropCellCount); //  <- UnityEngine.LightProbes::get_cellCount
    SET_METRO_BINDING(LightmapSettings_Get_Custom_PropLightmaps); //  <- UnityEngine.LightmapSettings::get_lightmaps
    SET_METRO_BINDING(LightmapSettings_Set_Custom_PropLightmaps); //  <- UnityEngine.LightmapSettings::set_lightmaps
    SET_METRO_BINDING(LightmapSettings_Get_Custom_PropLightmapsMode); //  <- UnityEngine.LightmapSettings::get_lightmapsMode
    SET_METRO_BINDING(LightmapSettings_Set_Custom_PropLightmapsMode); //  <- UnityEngine.LightmapSettings::set_lightmapsMode
    SET_METRO_BINDING(LightmapSettings_Get_Custom_PropBakedColorSpace); //  <- UnityEngine.LightmapSettings::get_bakedColorSpace
    SET_METRO_BINDING(LightmapSettings_Set_Custom_PropBakedColorSpace); //  <- UnityEngine.LightmapSettings::set_bakedColorSpace
    SET_METRO_BINDING(LightmapSettings_Get_Custom_PropLightProbes); //  <- UnityEngine.LightmapSettings::get_lightProbes
    SET_METRO_BINDING(LightmapSettings_Set_Custom_PropLightProbes); //  <- UnityEngine.LightmapSettings::set_lightProbes
#endif
    SET_METRO_BINDING(GeometryUtility_CUSTOM_INTERNAL_CALL_Internal_ExtractPlanes); //  <- UnityEngine.GeometryUtility::INTERNAL_CALL_Internal_ExtractPlanes
    SET_METRO_BINDING(GeometryUtility_CUSTOM_INTERNAL_CALL_TestPlanesAABB); //  <- UnityEngine.GeometryUtility::INTERNAL_CALL_TestPlanesAABB
    SET_METRO_BINDING(Screen_Get_Custom_PropResolutions); //  <- UnityEngine.Screen::get_resolutions
    SET_METRO_BINDING(Screen_Get_Custom_PropCurrentResolution); //  <- UnityEngine.Screen::get_currentResolution
    SET_METRO_BINDING(Screen_CUSTOM_SetResolution); //  <- UnityEngine.Screen::SetResolution
    SET_METRO_BINDING(Screen_Get_Custom_PropShowCursor); //  <- UnityEngine.Screen::get_showCursor
    SET_METRO_BINDING(Screen_Set_Custom_PropShowCursor); //  <- UnityEngine.Screen::set_showCursor
    SET_METRO_BINDING(Screen_Get_Custom_PropLockCursor); //  <- UnityEngine.Screen::get_lockCursor
    SET_METRO_BINDING(Screen_Set_Custom_PropLockCursor); //  <- UnityEngine.Screen::set_lockCursor
    SET_METRO_BINDING(Screen_Get_Custom_PropWidth); //  <- UnityEngine.Screen::get_width
    SET_METRO_BINDING(Screen_Get_Custom_PropHeight); //  <- UnityEngine.Screen::get_height
    SET_METRO_BINDING(Screen_Get_Custom_PropDpi); //  <- UnityEngine.Screen::get_dpi
    SET_METRO_BINDING(Screen_Get_Custom_PropFullScreen); //  <- UnityEngine.Screen::get_fullScreen
    SET_METRO_BINDING(Screen_Set_Custom_PropFullScreen); //  <- UnityEngine.Screen::set_fullScreen
    SET_METRO_BINDING(Screen_Get_Custom_PropAutorotateToPortrait); //  <- UnityEngine.Screen::get_autorotateToPortrait
    SET_METRO_BINDING(Screen_Set_Custom_PropAutorotateToPortrait); //  <- UnityEngine.Screen::set_autorotateToPortrait
    SET_METRO_BINDING(Screen_Get_Custom_PropAutorotateToPortraitUpsideDown); //  <- UnityEngine.Screen::get_autorotateToPortraitUpsideDown
    SET_METRO_BINDING(Screen_Set_Custom_PropAutorotateToPortraitUpsideDown); //  <- UnityEngine.Screen::set_autorotateToPortraitUpsideDown
    SET_METRO_BINDING(Screen_Get_Custom_PropAutorotateToLandscapeLeft); //  <- UnityEngine.Screen::get_autorotateToLandscapeLeft
    SET_METRO_BINDING(Screen_Set_Custom_PropAutorotateToLandscapeLeft); //  <- UnityEngine.Screen::set_autorotateToLandscapeLeft
    SET_METRO_BINDING(Screen_Get_Custom_PropAutorotateToLandscapeRight); //  <- UnityEngine.Screen::get_autorotateToLandscapeRight
    SET_METRO_BINDING(Screen_Set_Custom_PropAutorotateToLandscapeRight); //  <- UnityEngine.Screen::set_autorotateToLandscapeRight
    SET_METRO_BINDING(Screen_Get_Custom_PropOrientation); //  <- UnityEngine.Screen::get_orientation
    SET_METRO_BINDING(Screen_Set_Custom_PropOrientation); //  <- UnityEngine.Screen::set_orientation
    SET_METRO_BINDING(Screen_Get_Custom_PropSleepTimeout); //  <- UnityEngine.Screen::get_sleepTimeout
    SET_METRO_BINDING(Screen_Set_Custom_PropSleepTimeout); //  <- UnityEngine.Screen::set_sleepTimeout
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_CreateRenderTexture); //  <- UnityEngine.RenderTexture::Internal_CreateRenderTexture
    SET_METRO_BINDING(RenderTexture_CUSTOM_GetTemporary); //  <- UnityEngine.RenderTexture::GetTemporary
    SET_METRO_BINDING(RenderTexture_CUSTOM_ReleaseTemporary); //  <- UnityEngine.RenderTexture::ReleaseTemporary
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_GetWidth); //  <- UnityEngine.RenderTexture::Internal_GetWidth
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_SetWidth); //  <- UnityEngine.RenderTexture::Internal_SetWidth
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_GetHeight); //  <- UnityEngine.RenderTexture::Internal_GetHeight
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_SetHeight); //  <- UnityEngine.RenderTexture::Internal_SetHeight
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_SetSRGBReadWrite); //  <- UnityEngine.RenderTexture::Internal_SetSRGBReadWrite
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropDepth); //  <- UnityEngine.RenderTexture::get_depth
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropDepth); //  <- UnityEngine.RenderTexture::set_depth
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropIsPowerOfTwo); //  <- UnityEngine.RenderTexture::get_isPowerOfTwo
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropIsPowerOfTwo); //  <- UnityEngine.RenderTexture::set_isPowerOfTwo
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropSRGB); //  <- UnityEngine.RenderTexture::get_sRGB
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropFormat); //  <- UnityEngine.RenderTexture::get_format
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropFormat); //  <- UnityEngine.RenderTexture::set_format
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropUseMipMap); //  <- UnityEngine.RenderTexture::get_useMipMap
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropUseMipMap); //  <- UnityEngine.RenderTexture::set_useMipMap
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropIsCubemap); //  <- UnityEngine.RenderTexture::get_isCubemap
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropIsCubemap); //  <- UnityEngine.RenderTexture::set_isCubemap
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropIsVolume); //  <- UnityEngine.RenderTexture::get_isVolume
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropIsVolume); //  <- UnityEngine.RenderTexture::set_isVolume
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropVolumeDepth); //  <- UnityEngine.RenderTexture::get_volumeDepth
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropVolumeDepth); //  <- UnityEngine.RenderTexture::set_volumeDepth
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropAntiAliasing); //  <- UnityEngine.RenderTexture::get_antiAliasing
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropAntiAliasing); //  <- UnityEngine.RenderTexture::set_antiAliasing
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropEnableRandomWrite); //  <- UnityEngine.RenderTexture::get_enableRandomWrite
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropEnableRandomWrite); //  <- UnityEngine.RenderTexture::set_enableRandomWrite
    SET_METRO_BINDING(RenderTexture_CUSTOM_INTERNAL_CALL_Create); //  <- UnityEngine.RenderTexture::INTERNAL_CALL_Create
    SET_METRO_BINDING(RenderTexture_CUSTOM_INTERNAL_CALL_Release); //  <- UnityEngine.RenderTexture::INTERNAL_CALL_Release
    SET_METRO_BINDING(RenderTexture_CUSTOM_INTERNAL_CALL_IsCreated); //  <- UnityEngine.RenderTexture::INTERNAL_CALL_IsCreated
    SET_METRO_BINDING(RenderTexture_CUSTOM_INTERNAL_CALL_DiscardContents); //  <- UnityEngine.RenderTexture::INTERNAL_CALL_DiscardContents
    SET_METRO_BINDING(RenderTexture_CUSTOM_GetColorBuffer); //  <- UnityEngine.RenderTexture::GetColorBuffer
    SET_METRO_BINDING(RenderTexture_CUSTOM_GetDepthBuffer); //  <- UnityEngine.RenderTexture::GetDepthBuffer
    SET_METRO_BINDING(RenderTexture_CUSTOM_SetGlobalShaderProperty); //  <- UnityEngine.RenderTexture::SetGlobalShaderProperty
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropActive); //  <- UnityEngine.RenderTexture::get_active
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropActive); //  <- UnityEngine.RenderTexture::set_active
    SET_METRO_BINDING(RenderTexture_Get_Custom_PropEnabled); //  <- UnityEngine.RenderTexture::get_enabled
    SET_METRO_BINDING(RenderTexture_Set_Custom_PropEnabled); //  <- UnityEngine.RenderTexture::set_enabled
    SET_METRO_BINDING(RenderTexture_CUSTOM_Internal_GetTexelOffset); //  <- UnityEngine.RenderTexture::Internal_GetTexelOffset
    SET_METRO_BINDING(RenderTexture_CUSTOM_SupportsStencil); //  <- UnityEngine.RenderTexture::SupportsStencil
#if ENABLE_MOVIES
    SET_METRO_BINDING(MovieTexture_CUSTOM_INTERNAL_CALL_Play); //  <- UnityEngine.MovieTexture::INTERNAL_CALL_Play
    SET_METRO_BINDING(MovieTexture_CUSTOM_INTERNAL_CALL_Stop); //  <- UnityEngine.MovieTexture::INTERNAL_CALL_Stop
    SET_METRO_BINDING(MovieTexture_CUSTOM_INTERNAL_CALL_Pause); //  <- UnityEngine.MovieTexture::INTERNAL_CALL_Pause
#endif
#if (ENABLE_MOVIES) && (ENABLE_AUDIO)
    SET_METRO_BINDING(MovieTexture_Get_Custom_PropAudioClip); //  <- UnityEngine.MovieTexture::get_audioClip
#endif
#if ENABLE_MOVIES
    SET_METRO_BINDING(MovieTexture_Get_Custom_PropLoop); //  <- UnityEngine.MovieTexture::get_loop
    SET_METRO_BINDING(MovieTexture_Set_Custom_PropLoop); //  <- UnityEngine.MovieTexture::set_loop
    SET_METRO_BINDING(MovieTexture_Get_Custom_PropIsPlaying); //  <- UnityEngine.MovieTexture::get_isPlaying
    SET_METRO_BINDING(MovieTexture_Get_Custom_PropIsReadyToPlay); //  <- UnityEngine.MovieTexture::get_isReadyToPlay
    SET_METRO_BINDING(MovieTexture_Get_Custom_PropDuration); //  <- UnityEngine.MovieTexture::get_duration
#endif
    SET_METRO_BINDING(GL_CUSTOM_Vertex3); //  <- UnityEngine.GL::Vertex3
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_Vertex); //  <- UnityEngine.GL::INTERNAL_CALL_Vertex
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_Color); //  <- UnityEngine.GL::INTERNAL_CALL_Color
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_TexCoord); //  <- UnityEngine.GL::INTERNAL_CALL_TexCoord
    SET_METRO_BINDING(GL_CUSTOM_TexCoord2); //  <- UnityEngine.GL::TexCoord2
    SET_METRO_BINDING(GL_CUSTOM_TexCoord3); //  <- UnityEngine.GL::TexCoord3
    SET_METRO_BINDING(GL_CUSTOM_MultiTexCoord2); //  <- UnityEngine.GL::MultiTexCoord2
    SET_METRO_BINDING(GL_CUSTOM_MultiTexCoord3); //  <- UnityEngine.GL::MultiTexCoord3
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_MultiTexCoord); //  <- UnityEngine.GL::INTERNAL_CALL_MultiTexCoord
    SET_METRO_BINDING(GL_CUSTOM_Begin); //  <- UnityEngine.GL::Begin
    SET_METRO_BINDING(GL_CUSTOM_End); //  <- UnityEngine.GL::End
    SET_METRO_BINDING(GL_CUSTOM_LoadOrtho); //  <- UnityEngine.GL::LoadOrtho
    SET_METRO_BINDING(GL_CUSTOM_LoadPixelMatrix); //  <- UnityEngine.GL::LoadPixelMatrix
    SET_METRO_BINDING(GL_CUSTOM_LoadPixelMatrixArgs); //  <- UnityEngine.GL::LoadPixelMatrixArgs
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_Viewport); //  <- UnityEngine.GL::INTERNAL_CALL_Viewport
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_LoadProjectionMatrix); //  <- UnityEngine.GL::INTERNAL_CALL_LoadProjectionMatrix
    SET_METRO_BINDING(GL_CUSTOM_LoadIdentity); //  <- UnityEngine.GL::LoadIdentity
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_get_modelview); //  <- UnityEngine.GL::INTERNAL_get_modelview
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_set_modelview); //  <- UnityEngine.GL::INTERNAL_set_modelview
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_MultMatrix); //  <- UnityEngine.GL::INTERNAL_CALL_MultMatrix
    SET_METRO_BINDING(GL_CUSTOM_PushMatrix); //  <- UnityEngine.GL::PushMatrix
    SET_METRO_BINDING(GL_CUSTOM_PopMatrix); //  <- UnityEngine.GL::PopMatrix
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_GetGPUProjectionMatrix); //  <- UnityEngine.GL::INTERNAL_CALL_GetGPUProjectionMatrix
    SET_METRO_BINDING(GL_Get_Custom_PropWireframe); //  <- UnityEngine.GL::get_wireframe
    SET_METRO_BINDING(GL_Set_Custom_PropWireframe); //  <- UnityEngine.GL::set_wireframe
    SET_METRO_BINDING(GL_CUSTOM_SetRevertBackfacing); //  <- UnityEngine.GL::SetRevertBackfacing
    SET_METRO_BINDING(GL_CUSTOM_INTERNAL_CALL_Internal_Clear); //  <- UnityEngine.GL::INTERNAL_CALL_Internal_Clear
    SET_METRO_BINDING(GL_CUSTOM_ClearWithSkybox); //  <- UnityEngine.GL::ClearWithSkybox
    SET_METRO_BINDING(GL_CUSTOM_InvalidateState); //  <- UnityEngine.GL::InvalidateState
    SET_METRO_BINDING(GL_CUSTOM_IssuePluginEvent); //  <- UnityEngine.GL::IssuePluginEvent
    SET_METRO_BINDING(GUIElement_CUSTOM_INTERNAL_CALL_HitTest); //  <- UnityEngine.GUIElement::INTERNAL_CALL_HitTest
    SET_METRO_BINDING(GUIElement_CUSTOM_INTERNAL_CALL_GetScreenRect); //  <- UnityEngine.GUIElement::INTERNAL_CALL_GetScreenRect
    SET_METRO_BINDING(GUITexture_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.GUITexture::INTERNAL_get_color
    SET_METRO_BINDING(GUITexture_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.GUITexture::INTERNAL_set_color
    SET_METRO_BINDING(GUITexture_Get_Custom_PropTexture); //  <- UnityEngine.GUITexture::get_texture
    SET_METRO_BINDING(GUITexture_Set_Custom_PropTexture); //  <- UnityEngine.GUITexture::set_texture
    SET_METRO_BINDING(GUITexture_CUSTOM_INTERNAL_get_pixelInset); //  <- UnityEngine.GUITexture::INTERNAL_get_pixelInset
    SET_METRO_BINDING(GUITexture_CUSTOM_INTERNAL_set_pixelInset); //  <- UnityEngine.GUITexture::INTERNAL_set_pixelInset
    SET_METRO_BINDING(GUITexture_Get_Custom_PropBorder); //  <- UnityEngine.GUITexture::get_border
    SET_METRO_BINDING(GUITexture_Set_Custom_PropBorder); //  <- UnityEngine.GUITexture::set_border
    SET_METRO_BINDING(GUIText_Get_Custom_PropText); //  <- UnityEngine.GUIText::get_text
    SET_METRO_BINDING(GUIText_Set_Custom_PropText); //  <- UnityEngine.GUIText::set_text
    SET_METRO_BINDING(GUIText_Get_Custom_PropMaterial); //  <- UnityEngine.GUIText::get_material
    SET_METRO_BINDING(GUIText_Set_Custom_PropMaterial); //  <- UnityEngine.GUIText::set_material
    SET_METRO_BINDING(GUIText_CUSTOM_Internal_GetPixelOffset); //  <- UnityEngine.GUIText::Internal_GetPixelOffset
    SET_METRO_BINDING(GUIText_CUSTOM_INTERNAL_CALL_Internal_SetPixelOffset); //  <- UnityEngine.GUIText::INTERNAL_CALL_Internal_SetPixelOffset
    SET_METRO_BINDING(GUIText_Get_Custom_PropFont); //  <- UnityEngine.GUIText::get_font
    SET_METRO_BINDING(GUIText_Set_Custom_PropFont); //  <- UnityEngine.GUIText::set_font
    SET_METRO_BINDING(GUIText_Get_Custom_PropAlignment); //  <- UnityEngine.GUIText::get_alignment
    SET_METRO_BINDING(GUIText_Set_Custom_PropAlignment); //  <- UnityEngine.GUIText::set_alignment
    SET_METRO_BINDING(GUIText_Get_Custom_PropAnchor); //  <- UnityEngine.GUIText::get_anchor
    SET_METRO_BINDING(GUIText_Set_Custom_PropAnchor); //  <- UnityEngine.GUIText::set_anchor
    SET_METRO_BINDING(GUIText_Get_Custom_PropLineSpacing); //  <- UnityEngine.GUIText::get_lineSpacing
    SET_METRO_BINDING(GUIText_Set_Custom_PropLineSpacing); //  <- UnityEngine.GUIText::set_lineSpacing
    SET_METRO_BINDING(GUIText_Get_Custom_PropTabSize); //  <- UnityEngine.GUIText::get_tabSize
    SET_METRO_BINDING(GUIText_Set_Custom_PropTabSize); //  <- UnityEngine.GUIText::set_tabSize
    SET_METRO_BINDING(GUIText_Get_Custom_PropFontSize); //  <- UnityEngine.GUIText::get_fontSize
    SET_METRO_BINDING(GUIText_Set_Custom_PropFontSize); //  <- UnityEngine.GUIText::set_fontSize
    SET_METRO_BINDING(GUIText_Get_Custom_PropFontStyle); //  <- UnityEngine.GUIText::get_fontStyle
    SET_METRO_BINDING(GUIText_Set_Custom_PropFontStyle); //  <- UnityEngine.GUIText::set_fontStyle
    SET_METRO_BINDING(GUIText_Get_Custom_PropRichText); //  <- UnityEngine.GUIText::get_richText
    SET_METRO_BINDING(GUIText_Set_Custom_PropRichText); //  <- UnityEngine.GUIText::set_richText
    SET_METRO_BINDING(GUIText_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.GUIText::INTERNAL_get_color
    SET_METRO_BINDING(GUIText_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.GUIText::INTERNAL_set_color
    SET_METRO_BINDING(Font_CUSTOM_Internal_CreateFont); //  <- UnityEngine.Font::Internal_CreateFont
    SET_METRO_BINDING(Font_Get_Custom_PropMaterial); //  <- UnityEngine.Font::get_material
    SET_METRO_BINDING(Font_Set_Custom_PropMaterial); //  <- UnityEngine.Font::set_material
    SET_METRO_BINDING(Font_CUSTOM_HasCharacter); //  <- UnityEngine.Font::HasCharacter
#if ENABLE_MONO
    SET_METRO_BINDING(Font_Get_Custom_PropFontNames); //  <- UnityEngine.Font::get_fontNames
    SET_METRO_BINDING(Font_Set_Custom_PropFontNames); //  <- UnityEngine.Font::set_fontNames
#endif
    SET_METRO_BINDING(Font_Get_Custom_PropCharacterInfo); //  <- UnityEngine.Font::get_characterInfo
    SET_METRO_BINDING(Font_Set_Custom_PropCharacterInfo); //  <- UnityEngine.Font::set_characterInfo
    SET_METRO_BINDING(Font_CUSTOM_RequestCharactersInTexture); //  <- UnityEngine.Font::RequestCharactersInTexture
    SET_METRO_BINDING(Font_CUSTOM_GetCharacterInfo); //  <- UnityEngine.Font::GetCharacterInfo
    SET_METRO_BINDING(GUILayer_CUSTOM_INTERNAL_CALL_HitTest); //  <- UnityEngine.GUILayer::INTERNAL_CALL_HitTest
#if !PLATFORM_WEBGL && !UNITY_FLASH
    SET_METRO_BINDING(StaticBatchingUtility_CUSTOM_InternalCombineVertices); //  <- UnityEngine.StaticBatchingUtility::InternalCombineVertices
    SET_METRO_BINDING(StaticBatchingUtility_CUSTOM_InternalCombineIndices); //  <- UnityEngine.StaticBatchingUtility::InternalCombineIndices
#endif
}

#endif
