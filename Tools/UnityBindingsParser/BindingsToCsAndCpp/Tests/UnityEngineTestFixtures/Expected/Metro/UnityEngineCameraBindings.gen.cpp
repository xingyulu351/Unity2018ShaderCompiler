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
#include "External/shaderlab/Library/intshader.h"
#include "Runtime/Camera/Renderqueue.h"
#include "Runtime/Camera/Flare.h"
#include "Runtime/GfxDevice/GfxDevice.h"
#include "External/shaderlab/Library/texenv.h"
#include "External/shaderlab/Library/properties.h"
#include "Runtime/Geometry/Ray.h"
#include "Runtime/Graphics/Texture2D.h"
#include "Runtime/Graphics/CubemapTexture.h"
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
#include "Runtime/Graphics/RenderBufferManager.h"
#include "Runtime/Camera/RenderManager.h"
#include "Runtime/Shaders/GraphicsCaps.h"
#include "Runtime/Graphics/ScreenManager.h"
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Geometry/Plane.h"
#include "Runtime/Filters/Particles/ParticleAnimator.h"
#include "Runtime/Filters/Particles/ParticleRenderer.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Filters/AABBUtility.h"
#include <list>
#include <vector>
#include "Runtime/Misc/QualitySettings.h"
#include "Runtime/Filters/Misc/Font.h"
#include "Runtime/Camera/RenderSettings.h"
#include "Runtime/Shaders/ShaderKeywords.h"
#include "Runtime/Geometry/TextureAtlas.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Camera/CameraUtil.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/Graphics/ImageConversion.h"
#include "Runtime/Filters/Mesh/MeshCombiner.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Camera/Culler.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Camera/PVSVolume.h"
#include "Runtime/Graphics/DrawSplashScreenAndWatermarks.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#if UNITY_EDITOR
#include "Editor/Src/Gizmos/GizmoManager.h"
#include "Editor/Src/Gizmos/GizmoUtil.h"
#endif
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropFieldOfView(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropFieldOfView)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fieldOfView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fieldOfView)
    return self->GetFov ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropFieldOfView(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropFieldOfView)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fieldOfView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fieldOfView)
    
    self->SetFov (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropNearClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropNearClipPlane)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_nearClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_nearClipPlane)
    return self->GetNear ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropNearClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropNearClipPlane)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_nearClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_nearClipPlane)
    
    self->SetNear (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropFarClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropFarClipPlane)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_farClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_farClipPlane)
    return self->GetFar ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropFarClipPlane(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropFarClipPlane)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_farClipPlane)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_farClipPlane)
    
    self->SetFar (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
RenderingPath SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropRenderingPath(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropRenderingPath)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_renderingPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderingPath)
    return self->GetRenderingPath ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropRenderingPath(ICallType_ReadOnlyUnityEngineObject_Argument self_, RenderingPath value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropRenderingPath)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_renderingPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_renderingPath)
    
    self->SetRenderingPath (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
RenderingPath SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropActualRenderingPath(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropActualRenderingPath)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_actualRenderingPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_actualRenderingPath)
    return self->CalculateRenderingPath();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropHdr(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropHdr)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hdr)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hdr)
    return self->CalculateUsingHDR ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropHdr(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropHdr)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_hdr)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hdr)
    
    self->SetHDR (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropOrthographicSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropOrthographicSize)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_orthographicSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_orthographicSize)
    return self->GetOrthographicSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropOrthographicSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropOrthographicSize)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_orthographicSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_orthographicSize)
    
    self->SetOrthographicSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropOrthographic(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropOrthographic)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_orthographic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_orthographic)
    return self->GetOrthographic ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropOrthographic(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropOrthographic)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_orthographic)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_orthographic)
    
    self->SetOrthographic (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
Camera::TransparencySortMode SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropTransparencySortMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropTransparencySortMode)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_transparencySortMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_transparencySortMode)
    return self->GetSortMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropTransparencySortMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, Camera::TransparencySortMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropTransparencySortMode)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_transparencySortMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_transparencySortMode)
    
    self->SetSortMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_depth)
    return self->GetDepth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_depth)
    
    self->SetDepth (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropAspect(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropAspect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_aspect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_aspect)
    return self->GetAspect ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropAspect(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropAspect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_aspect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_aspect)
    
    self->SetAspect (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropCullingMask(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropCullingMask)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cullingMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cullingMask)
    return self->GetCullingMask ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropCullingMask(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropCullingMask)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cullingMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cullingMask)
    
    self->SetCullingMask (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropEventMask(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropEventMask)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_eventMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_eventMask)
    return self->GetEventMask ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropEventMask(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropEventMask)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_eventMask)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_eventMask)
    
    self->SetEventMask (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_backgroundColor(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_backgroundColor)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_backgroundColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_backgroundColor)
    *returnValue = self->GetBackgroundColor();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_set_backgroundColor(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_set_backgroundColor)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_backgroundColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_backgroundColor)
    
    self->SetBackgroundColor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_rect(ICallType_ReadOnlyUnityEngineObject_Argument self_, Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_rect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rect)
    *returnValue = self->GetNormalizedViewportRect();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_set_rect(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Rectf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_set_rect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_rect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_rect)
    
    self->SetNormalizedViewportRect (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_pixelRect(ICallType_ReadOnlyUnityEngineObject_Argument self_, Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_pixelRect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_pixelRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_pixelRect)
    *returnValue = self->GetScreenViewportRect();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_set_pixelRect(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Rectf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_set_pixelRect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_pixelRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_pixelRect)
    
    self->SetScreenViewportRect (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropTargetTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropTargetTexture)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_targetTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_targetTexture)
    return Scripting::ScriptingWrapperFor(self->GetTargetTexture());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropTargetTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropTargetTexture)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<RenderTexture> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_targetTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_targetTexture)
    
    self->SetTargetTexture (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_SetTargetBuffersImpl(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingRenderBuffer* colorBuffer, ScriptingRenderBuffer* depthBuffer)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_SetTargetBuffersImpl)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetTargetBuffersImpl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetTargetBuffersImpl)
    
    		RenderSurfaceHandle color = RenderSurfaceHandle(colorBuffer->m_BufferPtr);
    		RenderSurfaceHandle depth = RenderSurfaceHandle(depthBuffer->m_BufferPtr);
    
    		{
    			int colorW=0, colorH=0;
    			GetGfxDevice().GetRenderSurfaceExt(color, &colorW, &colorH);
    
    			int depthW=0, depthH=0;
    			GetGfxDevice().GetRenderSurfaceExt(depth, &depthW, &depthH);
    
    			if(colorW != depthW || colorH != depthH)
    			{
    				ErrorString ("Camera.SetTargetBuffers can only accept RenderBuffers with same ext.");
    				return;
    			}
    		}
    
    		RenderTexture* rtC = PPtr<RenderTexture>(colorBuffer->m_RenderTextureInstanceID);
    		RenderTexture* rtD = PPtr<RenderTexture>(depthBuffer->m_RenderTextureInstanceID);
    		if (rtC && !rtD)
    		{
    			ErrorString ("You're trying to set color buffer from the RenderTexture and depth buffer from the screen.");
    			return;
    		}
    		if (!rtC && rtD)
    		{
    			ErrorString ("You're trying to set color buffer from the screen and depth buffer from the RenderTexture.");
    			return;
    		}
    
    		self->SetTargetBuffers(color, depth, rtC);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropPixelWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropPixelWidth)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pixelWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pixelWidth)
    return self->GetScreenViewportRect ().Width();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropPixelHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropPixelHeight)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pixelHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pixelHeight)
    return self->GetScreenViewportRect ().Height();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_cameraToWorldMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_cameraToWorldMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_cameraToWorldMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_cameraToWorldMatrix)
    *returnValue = self->GetCameraToWorldMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_worldToCameraMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_worldToCameraMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_worldToCameraMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_worldToCameraMatrix)
    *returnValue = self->GetWorldToCameraMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_set_worldToCameraMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Matrix4x4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_set_worldToCameraMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_worldToCameraMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_worldToCameraMatrix)
    
    self->SetWorldToCameraMatrix (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ResetWorldToCameraMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ResetWorldToCameraMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ResetWorldToCameraMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ResetWorldToCameraMatrix)
    return self->ResetWorldToCameraMatrix();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_projectionMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_projectionMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_projectionMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_projectionMatrix)
    *returnValue = self->GetProjectionMatrix();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_set_projectionMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Matrix4x4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_set_projectionMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_projectionMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_projectionMatrix)
    
    self->SetProjectionMatrix (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ResetProjectionMatrix(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ResetProjectionMatrix)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ResetProjectionMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ResetProjectionMatrix)
    return self->ResetProjectionMatrix();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ResetAspect(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ResetAspect)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ResetAspect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ResetAspect)
    return self->ResetAspect();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_get_velocity(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_get_velocity)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_velocity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_velocity)
    *returnValue = self->GetVelocity();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropClearFlags(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropClearFlags)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clearFlags)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clearFlags)
    return self->GetClearFlags ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropClearFlags(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropClearFlags)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_clearFlags)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_clearFlags)
    
    self->SetClearFlags (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_WorldToScreenPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_WorldToScreenPoint)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_WorldToScreenPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_WorldToScreenPoint)
    returnValue = self->WorldToScreenPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_WorldToViewportPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_WorldToViewportPoint)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_WorldToViewportPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_WorldToViewportPoint)
    returnValue = self->WorldToViewportPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ViewportToWorldPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ViewportToWorldPoint)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ViewportToWorldPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ViewportToWorldPoint)
    returnValue = self->ViewportToWorldPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ScreenToWorldPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ScreenToWorldPoint)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ScreenToWorldPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ScreenToWorldPoint)
    returnValue = self->ScreenToWorldPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ScreenToViewportPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ScreenToViewportPoint)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ScreenToViewportPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ScreenToViewportPoint)
    returnValue = self->ScreenToViewportPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ViewportToScreenPoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ViewportToScreenPoint)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ViewportToScreenPoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ViewportToScreenPoint)
    returnValue = self->ViewportToScreenPoint(position);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ViewportPointToRay(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Ray& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ViewportPointToRay)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ViewportPointToRay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ViewportPointToRay)
     { returnValue =(self->ViewportPointToRay (Vector2f (position.x, position.y))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ScreenPointToRay(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& position, Ray& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ScreenPointToRay)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ScreenPointToRay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ScreenPointToRay)
     { returnValue =(self->ScreenPointToRay (Vector2f (position.x, position.y))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropMain()
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropMain)
    SCRIPTINGAPI_STACK_CHECK(get_main)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_main)
    return Scripting::ScriptingWrapperFor(FindMainCamera());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropCurrent()
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropCurrent)
    SCRIPTINGAPI_STACK_CHECK(get_current)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_current)
    return Scripting::ScriptingWrapperFor (GetCurrentCameraPtr());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropAllCameras()
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropAllCameras)
    SCRIPTINGAPI_STACK_CHECK(get_allCameras)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_allCameras)
    const RenderManager::CameraContainer& onscreen  = GetRenderManager ().GetOnscreenCameras ();
    const RenderManager::CameraContainer& offscreen = GetRenderManager ().GetOffscreenCameras ();
    unsigned camCount = onscreen.size () + offscreen.size();
    int curCameraI = 0;
    ScriptingArrayPtr scriptingcams = CreateScriptingArray<ScriptingObjectPtr>(GetScriptingManager ().GetCommonClasses ().camera, camCount);
    for (   RenderManager::CameraContainer::const_iterator camIter = onscreen.begin () ;
    camIter != onscreen.end() ;
    ++camIter, ++curCameraI
    )
    {
    SetScriptingArrayElement (scriptingcams, curCameraI, Scripting::ScriptingWrapperForImpl (*camIter));
    }
    for (   RenderManager::CameraContainer::const_iterator camIter = offscreen.begin () ;
    camIter != offscreen.end() ;
    ++camIter, ++curCameraI
    )
    {
    SetScriptingArrayElement (scriptingcams, curCameraI, Scripting::ScriptingWrapperForImpl (*camIter));
    }
    return scriptingcams;
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_CUSTOM_GetScreenWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_GetScreenWidth)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetScreenWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetScreenWidth)
     return GetScreenManager ().GetWidth ();	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Camera_CUSTOM_GetScreenHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_GetScreenHeight)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetScreenHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetScreenHeight)
     return GetScreenManager ().GetHeight (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_DoClear(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_DoClear)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(DoClear)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DoClear)
    
    		self->Clear ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_Render(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_Render)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Render)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Render)
    
    		self->StandaloneRender( Camera::kFlagSetRenderTarget, NULL, "" );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_RenderWithShader(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument shader_, ICallType_String_Argument replacementTag_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_RenderWithShader)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Shader> shader(shader_);
    UNUSED(shader);
    ICallType_String_Local replacementTag(replacementTag_);
    UNUSED(replacementTag);
    SCRIPTINGAPI_STACK_CHECK(RenderWithShader)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RenderWithShader)
    
    		self->StandaloneRender( Camera::kFlagSetRenderTarget, shader, replacementTag );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_SetReplacementShader(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument shader_, ICallType_String_Argument replacementTag_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_SetReplacementShader)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Shader> shader(shader_);
    UNUSED(shader);
    ICallType_String_Local replacementTag(replacementTag_);
    UNUSED(replacementTag);
    SCRIPTINGAPI_STACK_CHECK(SetReplacementShader)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetReplacementShader)
    
    		self->SetReplacementShader( shader, replacementTag );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_INTERNAL_CALL_ResetReplacementShader(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_INTERNAL_CALL_ResetReplacementShader)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ResetReplacementShader)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_ResetReplacementShader)
    return self->ResetReplacementShader();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropUseOcclusionCulling(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropUseOcclusionCulling)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useOcclusionCulling)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useOcclusionCulling)
    return self->GetUseOcclusionCulling ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropUseOcclusionCulling(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropUseOcclusionCulling)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useOcclusionCulling)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useOcclusionCulling)
    
    self->SetUseOcclusionCulling (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_RenderDontRestore(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_RenderDontRestore)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RenderDontRestore)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RenderDontRestore)
    
    		self->StandaloneRender( Camera::kFlagDontRestoreRenderState | Camera::kFlagSetRenderTarget, NULL, "" );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_SetupCurrent(ICallType_ReadOnlyUnityEngineObject_Argument cur_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_SetupCurrent)
    ReadOnlyScriptingObjectOfType<Camera> cur(cur_);
    UNUSED(cur);
    SCRIPTINGAPI_STACK_CHECK(SetupCurrent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetupCurrent)
    
    		if (cur)
    		{
    			cur->StandaloneSetup();
    		}
    		else
    		{
    			GetRenderManager ().SetCurrentCamera (NULL);
    			RenderTexture::SetActive(NULL);
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_CUSTOM_Internal_RenderToCubemapRT(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument cubemap_, int faceMask)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_Internal_RenderToCubemapRT)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<RenderTexture> cubemap(cubemap_);
    UNUSED(cubemap);
    SCRIPTINGAPI_STACK_CHECK(Internal_RenderToCubemapRT)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_RenderToCubemapRT)
    
    		RenderTexture* rt = cubemap;
    		if( !rt )
    		{
    			ErrorString( "Cubemap must not be null" );
    			return false;
    		}
    		return self->StandaloneRenderToCubemap( rt, faceMask );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_CUSTOM_Internal_RenderToCubemapTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument cubemap_, int faceMask)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_Internal_RenderToCubemapTexture)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Cubemap> cubemap(cubemap_);
    UNUSED(cubemap);
    SCRIPTINGAPI_STACK_CHECK(Internal_RenderToCubemapTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_RenderToCubemapTexture)
    
    		Cubemap* cube = cubemap;
    		if( !cube )
    		{
    			ErrorString( "Cubemap must not be null" );
    			return false;
    		}
    		return self->StandaloneRenderToCubemap( cube, faceMask );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropLayerCullDistances(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropLayerCullDistances)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_layerCullDistances)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_layerCullDistances)
    return CreateScriptingArray(self->GetLayerCullDistances(), 32, GetScriptingManager().GetCommonClasses().floatSingle);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropLayerCullDistances(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropLayerCullDistances)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_layerCullDistances)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_layerCullDistances)
    
    #if ENABLE_MONO
    MonoRaiseIfNull(value);
    #endif
    if(GetScriptingArraySize(value) != 32)
    {
    #if ENABLE_MONO
    RaiseMonoException(" Array needs to contain exactly 32 floats for layerCullDistances.");
    #endif
    return;
    }
    self->SetLayerCullDistances(GetScriptingArrayStart<float> (value));
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropLayerCullSpherical(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropLayerCullSpherical)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_layerCullSpherical)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_layerCullSpherical)
    return self->GetLayerCullSpherical();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropLayerCullSpherical(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropLayerCullSpherical)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_layerCullSpherical)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_layerCullSpherical)
     self->SetLayerCullSpherical(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_CUSTOM_CopyFrom(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument other_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_CopyFrom)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> other(other_);
    UNUSED(other);
    SCRIPTINGAPI_STACK_CHECK(CopyFrom)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CopyFrom)
    
    		const Camera* otherCam = other;
    		if(!otherCam)
    		{
    			ErrorString( "Camera to copy from must not be null" );
    			return;
    		}
    
    		self->CopyFrom (*otherCam);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropDepthTextureMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropDepthTextureMode)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_depthTextureMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_depthTextureMode)
    return self->GetDepthTextureMode();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropDepthTextureMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropDepthTextureMode)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_depthTextureMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_depthTextureMode)
     self->SetDepthTextureMode (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_Get_Custom_PropClearStencilAfterLightingPass(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Get_Custom_PropClearStencilAfterLightingPass)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clearStencilAfterLightingPass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clearStencilAfterLightingPass)
    return self->GetClearStencilAfterLightingPass();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Camera_Set_Custom_PropClearStencilAfterLightingPass(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_Set_Custom_PropClearStencilAfterLightingPass)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_clearStencilAfterLightingPass)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_clearStencilAfterLightingPass)
     self->SetClearStencilAfterLightingPass (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Camera_CUSTOM_IsFiltered(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument go_)
{
    SCRIPTINGAPI_ETW_ENTRY(Camera_CUSTOM_IsFiltered)
    ReadOnlyScriptingObjectOfType<Camera> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<GameObject> go(go_);
    UNUSED(go);
    SCRIPTINGAPI_STACK_CHECK(IsFiltered)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsFiltered)
    
    	#if UNITY_EDITOR
    		return self->GetCuller().IsFiltered(*go);
    	#else
    		return true;
    	#endif
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Camera_get_fieldOfView()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_fieldOfView" , (gpointer)& Camera_Get_Custom_PropFieldOfView );
}

void Register_UnityEngine_Camera_set_fieldOfView()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_fieldOfView" , (gpointer)& Camera_Set_Custom_PropFieldOfView );
}

void Register_UnityEngine_Camera_get_nearClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_nearClipPlane" , (gpointer)& Camera_Get_Custom_PropNearClipPlane );
}

void Register_UnityEngine_Camera_set_nearClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_nearClipPlane" , (gpointer)& Camera_Set_Custom_PropNearClipPlane );
}

void Register_UnityEngine_Camera_get_farClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_farClipPlane" , (gpointer)& Camera_Get_Custom_PropFarClipPlane );
}

void Register_UnityEngine_Camera_set_farClipPlane()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_farClipPlane" , (gpointer)& Camera_Set_Custom_PropFarClipPlane );
}

void Register_UnityEngine_Camera_get_renderingPath()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_renderingPath" , (gpointer)& Camera_Get_Custom_PropRenderingPath );
}

void Register_UnityEngine_Camera_set_renderingPath()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_renderingPath" , (gpointer)& Camera_Set_Custom_PropRenderingPath );
}

void Register_UnityEngine_Camera_get_actualRenderingPath()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_actualRenderingPath" , (gpointer)& Camera_Get_Custom_PropActualRenderingPath );
}

void Register_UnityEngine_Camera_get_hdr()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_hdr" , (gpointer)& Camera_Get_Custom_PropHdr );
}

void Register_UnityEngine_Camera_set_hdr()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_hdr" , (gpointer)& Camera_Set_Custom_PropHdr );
}

void Register_UnityEngine_Camera_get_orthographicSize()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_orthographicSize" , (gpointer)& Camera_Get_Custom_PropOrthographicSize );
}

void Register_UnityEngine_Camera_set_orthographicSize()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_orthographicSize" , (gpointer)& Camera_Set_Custom_PropOrthographicSize );
}

void Register_UnityEngine_Camera_get_orthographic()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_orthographic" , (gpointer)& Camera_Get_Custom_PropOrthographic );
}

void Register_UnityEngine_Camera_set_orthographic()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_orthographic" , (gpointer)& Camera_Set_Custom_PropOrthographic );
}

void Register_UnityEngine_Camera_get_transparencySortMode()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_transparencySortMode" , (gpointer)& Camera_Get_Custom_PropTransparencySortMode );
}

void Register_UnityEngine_Camera_set_transparencySortMode()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_transparencySortMode" , (gpointer)& Camera_Set_Custom_PropTransparencySortMode );
}

void Register_UnityEngine_Camera_get_depth()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_depth" , (gpointer)& Camera_Get_Custom_PropDepth );
}

void Register_UnityEngine_Camera_set_depth()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_depth" , (gpointer)& Camera_Set_Custom_PropDepth );
}

void Register_UnityEngine_Camera_get_aspect()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_aspect" , (gpointer)& Camera_Get_Custom_PropAspect );
}

void Register_UnityEngine_Camera_set_aspect()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_aspect" , (gpointer)& Camera_Set_Custom_PropAspect );
}

void Register_UnityEngine_Camera_get_cullingMask()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_cullingMask" , (gpointer)& Camera_Get_Custom_PropCullingMask );
}

void Register_UnityEngine_Camera_set_cullingMask()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_cullingMask" , (gpointer)& Camera_Set_Custom_PropCullingMask );
}

void Register_UnityEngine_Camera_get_eventMask()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_eventMask" , (gpointer)& Camera_Get_Custom_PropEventMask );
}

void Register_UnityEngine_Camera_set_eventMask()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_eventMask" , (gpointer)& Camera_Set_Custom_PropEventMask );
}

void Register_UnityEngine_Camera_INTERNAL_get_backgroundColor()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_backgroundColor" , (gpointer)& Camera_CUSTOM_INTERNAL_get_backgroundColor );
}

void Register_UnityEngine_Camera_INTERNAL_set_backgroundColor()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_set_backgroundColor" , (gpointer)& Camera_CUSTOM_INTERNAL_set_backgroundColor );
}

void Register_UnityEngine_Camera_INTERNAL_get_rect()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_rect" , (gpointer)& Camera_CUSTOM_INTERNAL_get_rect );
}

void Register_UnityEngine_Camera_INTERNAL_set_rect()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_set_rect" , (gpointer)& Camera_CUSTOM_INTERNAL_set_rect );
}

void Register_UnityEngine_Camera_INTERNAL_get_pixelRect()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_pixelRect" , (gpointer)& Camera_CUSTOM_INTERNAL_get_pixelRect );
}

void Register_UnityEngine_Camera_INTERNAL_set_pixelRect()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_set_pixelRect" , (gpointer)& Camera_CUSTOM_INTERNAL_set_pixelRect );
}

void Register_UnityEngine_Camera_get_targetTexture()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_targetTexture" , (gpointer)& Camera_Get_Custom_PropTargetTexture );
}

void Register_UnityEngine_Camera_set_targetTexture()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_targetTexture" , (gpointer)& Camera_Set_Custom_PropTargetTexture );
}

void Register_UnityEngine_Camera_SetTargetBuffersImpl()
{
    scripting_add_internal_call( "UnityEngine.Camera::SetTargetBuffersImpl" , (gpointer)& Camera_CUSTOM_SetTargetBuffersImpl );
}

void Register_UnityEngine_Camera_get_pixelWidth()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_pixelWidth" , (gpointer)& Camera_Get_Custom_PropPixelWidth );
}

void Register_UnityEngine_Camera_get_pixelHeight()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_pixelHeight" , (gpointer)& Camera_Get_Custom_PropPixelHeight );
}

void Register_UnityEngine_Camera_INTERNAL_get_cameraToWorldMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_cameraToWorldMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_get_cameraToWorldMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_get_worldToCameraMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_worldToCameraMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_get_worldToCameraMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_set_worldToCameraMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_set_worldToCameraMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_set_worldToCameraMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ResetWorldToCameraMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ResetWorldToCameraMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ResetWorldToCameraMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_get_projectionMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_projectionMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_get_projectionMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_set_projectionMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_set_projectionMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_set_projectionMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ResetProjectionMatrix()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ResetProjectionMatrix" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ResetProjectionMatrix );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ResetAspect()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ResetAspect" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ResetAspect );
}

void Register_UnityEngine_Camera_INTERNAL_get_velocity()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_get_velocity" , (gpointer)& Camera_CUSTOM_INTERNAL_get_velocity );
}

void Register_UnityEngine_Camera_get_clearFlags()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_clearFlags" , (gpointer)& Camera_Get_Custom_PropClearFlags );
}

void Register_UnityEngine_Camera_set_clearFlags()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_clearFlags" , (gpointer)& Camera_Set_Custom_PropClearFlags );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_WorldToScreenPoint()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_WorldToScreenPoint" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_WorldToScreenPoint );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_WorldToViewportPoint()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_WorldToViewportPoint" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_WorldToViewportPoint );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ViewportToWorldPoint()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ViewportToWorldPoint" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ViewportToWorldPoint );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ScreenToWorldPoint()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ScreenToWorldPoint" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ScreenToWorldPoint );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ScreenToViewportPoint()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ScreenToViewportPoint" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ScreenToViewportPoint );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ViewportToScreenPoint()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ViewportToScreenPoint" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ViewportToScreenPoint );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ViewportPointToRay()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ViewportPointToRay" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ViewportPointToRay );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ScreenPointToRay()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ScreenPointToRay" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ScreenPointToRay );
}

void Register_UnityEngine_Camera_get_main()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_main" , (gpointer)& Camera_Get_Custom_PropMain );
}

void Register_UnityEngine_Camera_get_current()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_current" , (gpointer)& Camera_Get_Custom_PropCurrent );
}

void Register_UnityEngine_Camera_get_allCameras()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_allCameras" , (gpointer)& Camera_Get_Custom_PropAllCameras );
}

void Register_UnityEngine_Camera_GetScreenWidth()
{
    scripting_add_internal_call( "UnityEngine.Camera::GetScreenWidth" , (gpointer)& Camera_CUSTOM_GetScreenWidth );
}

void Register_UnityEngine_Camera_GetScreenHeight()
{
    scripting_add_internal_call( "UnityEngine.Camera::GetScreenHeight" , (gpointer)& Camera_CUSTOM_GetScreenHeight );
}

void Register_UnityEngine_Camera_DoClear()
{
    scripting_add_internal_call( "UnityEngine.Camera::DoClear" , (gpointer)& Camera_CUSTOM_DoClear );
}

void Register_UnityEngine_Camera_Render()
{
    scripting_add_internal_call( "UnityEngine.Camera::Render" , (gpointer)& Camera_CUSTOM_Render );
}

void Register_UnityEngine_Camera_RenderWithShader()
{
    scripting_add_internal_call( "UnityEngine.Camera::RenderWithShader" , (gpointer)& Camera_CUSTOM_RenderWithShader );
}

void Register_UnityEngine_Camera_SetReplacementShader()
{
    scripting_add_internal_call( "UnityEngine.Camera::SetReplacementShader" , (gpointer)& Camera_CUSTOM_SetReplacementShader );
}

void Register_UnityEngine_Camera_INTERNAL_CALL_ResetReplacementShader()
{
    scripting_add_internal_call( "UnityEngine.Camera::INTERNAL_CALL_ResetReplacementShader" , (gpointer)& Camera_CUSTOM_INTERNAL_CALL_ResetReplacementShader );
}

void Register_UnityEngine_Camera_get_useOcclusionCulling()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_useOcclusionCulling" , (gpointer)& Camera_Get_Custom_PropUseOcclusionCulling );
}

void Register_UnityEngine_Camera_set_useOcclusionCulling()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_useOcclusionCulling" , (gpointer)& Camera_Set_Custom_PropUseOcclusionCulling );
}

void Register_UnityEngine_Camera_RenderDontRestore()
{
    scripting_add_internal_call( "UnityEngine.Camera::RenderDontRestore" , (gpointer)& Camera_CUSTOM_RenderDontRestore );
}

void Register_UnityEngine_Camera_SetupCurrent()
{
    scripting_add_internal_call( "UnityEngine.Camera::SetupCurrent" , (gpointer)& Camera_CUSTOM_SetupCurrent );
}

void Register_UnityEngine_Camera_Internal_RenderToCubemapRT()
{
    scripting_add_internal_call( "UnityEngine.Camera::Internal_RenderToCubemapRT" , (gpointer)& Camera_CUSTOM_Internal_RenderToCubemapRT );
}

void Register_UnityEngine_Camera_Internal_RenderToCubemapTexture()
{
    scripting_add_internal_call( "UnityEngine.Camera::Internal_RenderToCubemapTexture" , (gpointer)& Camera_CUSTOM_Internal_RenderToCubemapTexture );
}

void Register_UnityEngine_Camera_get_layerCullDistances()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_layerCullDistances" , (gpointer)& Camera_Get_Custom_PropLayerCullDistances );
}

void Register_UnityEngine_Camera_set_layerCullDistances()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_layerCullDistances" , (gpointer)& Camera_Set_Custom_PropLayerCullDistances );
}

void Register_UnityEngine_Camera_get_layerCullSpherical()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_layerCullSpherical" , (gpointer)& Camera_Get_Custom_PropLayerCullSpherical );
}

void Register_UnityEngine_Camera_set_layerCullSpherical()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_layerCullSpherical" , (gpointer)& Camera_Set_Custom_PropLayerCullSpherical );
}

void Register_UnityEngine_Camera_CopyFrom()
{
    scripting_add_internal_call( "UnityEngine.Camera::CopyFrom" , (gpointer)& Camera_CUSTOM_CopyFrom );
}

void Register_UnityEngine_Camera_get_depthTextureMode()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_depthTextureMode" , (gpointer)& Camera_Get_Custom_PropDepthTextureMode );
}

void Register_UnityEngine_Camera_set_depthTextureMode()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_depthTextureMode" , (gpointer)& Camera_Set_Custom_PropDepthTextureMode );
}

void Register_UnityEngine_Camera_get_clearStencilAfterLightingPass()
{
    scripting_add_internal_call( "UnityEngine.Camera::get_clearStencilAfterLightingPass" , (gpointer)& Camera_Get_Custom_PropClearStencilAfterLightingPass );
}

void Register_UnityEngine_Camera_set_clearStencilAfterLightingPass()
{
    scripting_add_internal_call( "UnityEngine.Camera::set_clearStencilAfterLightingPass" , (gpointer)& Camera_Set_Custom_PropClearStencilAfterLightingPass );
}

void Register_UnityEngine_Camera_IsFiltered()
{
    scripting_add_internal_call( "UnityEngine.Camera::IsFiltered" , (gpointer)& Camera_CUSTOM_IsFiltered );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineCamera_IcallNames [] =
{
    "UnityEngine.Camera::get_fieldOfView"   ,    // -> Camera_Get_Custom_PropFieldOfView
    "UnityEngine.Camera::set_fieldOfView"   ,    // -> Camera_Set_Custom_PropFieldOfView
    "UnityEngine.Camera::get_nearClipPlane" ,    // -> Camera_Get_Custom_PropNearClipPlane
    "UnityEngine.Camera::set_nearClipPlane" ,    // -> Camera_Set_Custom_PropNearClipPlane
    "UnityEngine.Camera::get_farClipPlane"  ,    // -> Camera_Get_Custom_PropFarClipPlane
    "UnityEngine.Camera::set_farClipPlane"  ,    // -> Camera_Set_Custom_PropFarClipPlane
    "UnityEngine.Camera::get_renderingPath" ,    // -> Camera_Get_Custom_PropRenderingPath
    "UnityEngine.Camera::set_renderingPath" ,    // -> Camera_Set_Custom_PropRenderingPath
    "UnityEngine.Camera::get_actualRenderingPath",    // -> Camera_Get_Custom_PropActualRenderingPath
    "UnityEngine.Camera::get_hdr"           ,    // -> Camera_Get_Custom_PropHdr
    "UnityEngine.Camera::set_hdr"           ,    // -> Camera_Set_Custom_PropHdr
    "UnityEngine.Camera::get_orthographicSize",    // -> Camera_Get_Custom_PropOrthographicSize
    "UnityEngine.Camera::set_orthographicSize",    // -> Camera_Set_Custom_PropOrthographicSize
    "UnityEngine.Camera::get_orthographic"  ,    // -> Camera_Get_Custom_PropOrthographic
    "UnityEngine.Camera::set_orthographic"  ,    // -> Camera_Set_Custom_PropOrthographic
    "UnityEngine.Camera::get_transparencySortMode",    // -> Camera_Get_Custom_PropTransparencySortMode
    "UnityEngine.Camera::set_transparencySortMode",    // -> Camera_Set_Custom_PropTransparencySortMode
    "UnityEngine.Camera::get_depth"         ,    // -> Camera_Get_Custom_PropDepth
    "UnityEngine.Camera::set_depth"         ,    // -> Camera_Set_Custom_PropDepth
    "UnityEngine.Camera::get_aspect"        ,    // -> Camera_Get_Custom_PropAspect
    "UnityEngine.Camera::set_aspect"        ,    // -> Camera_Set_Custom_PropAspect
    "UnityEngine.Camera::get_cullingMask"   ,    // -> Camera_Get_Custom_PropCullingMask
    "UnityEngine.Camera::set_cullingMask"   ,    // -> Camera_Set_Custom_PropCullingMask
    "UnityEngine.Camera::get_eventMask"     ,    // -> Camera_Get_Custom_PropEventMask
    "UnityEngine.Camera::set_eventMask"     ,    // -> Camera_Set_Custom_PropEventMask
    "UnityEngine.Camera::INTERNAL_get_backgroundColor",    // -> Camera_CUSTOM_INTERNAL_get_backgroundColor
    "UnityEngine.Camera::INTERNAL_set_backgroundColor",    // -> Camera_CUSTOM_INTERNAL_set_backgroundColor
    "UnityEngine.Camera::INTERNAL_get_rect" ,    // -> Camera_CUSTOM_INTERNAL_get_rect
    "UnityEngine.Camera::INTERNAL_set_rect" ,    // -> Camera_CUSTOM_INTERNAL_set_rect
    "UnityEngine.Camera::INTERNAL_get_pixelRect",    // -> Camera_CUSTOM_INTERNAL_get_pixelRect
    "UnityEngine.Camera::INTERNAL_set_pixelRect",    // -> Camera_CUSTOM_INTERNAL_set_pixelRect
    "UnityEngine.Camera::get_targetTexture" ,    // -> Camera_Get_Custom_PropTargetTexture
    "UnityEngine.Camera::set_targetTexture" ,    // -> Camera_Set_Custom_PropTargetTexture
    "UnityEngine.Camera::SetTargetBuffersImpl",    // -> Camera_CUSTOM_SetTargetBuffersImpl
    "UnityEngine.Camera::get_pixelWidth"    ,    // -> Camera_Get_Custom_PropPixelWidth
    "UnityEngine.Camera::get_pixelHeight"   ,    // -> Camera_Get_Custom_PropPixelHeight
    "UnityEngine.Camera::INTERNAL_get_cameraToWorldMatrix",    // -> Camera_CUSTOM_INTERNAL_get_cameraToWorldMatrix
    "UnityEngine.Camera::INTERNAL_get_worldToCameraMatrix",    // -> Camera_CUSTOM_INTERNAL_get_worldToCameraMatrix
    "UnityEngine.Camera::INTERNAL_set_worldToCameraMatrix",    // -> Camera_CUSTOM_INTERNAL_set_worldToCameraMatrix
    "UnityEngine.Camera::INTERNAL_CALL_ResetWorldToCameraMatrix",    // -> Camera_CUSTOM_INTERNAL_CALL_ResetWorldToCameraMatrix
    "UnityEngine.Camera::INTERNAL_get_projectionMatrix",    // -> Camera_CUSTOM_INTERNAL_get_projectionMatrix
    "UnityEngine.Camera::INTERNAL_set_projectionMatrix",    // -> Camera_CUSTOM_INTERNAL_set_projectionMatrix
    "UnityEngine.Camera::INTERNAL_CALL_ResetProjectionMatrix",    // -> Camera_CUSTOM_INTERNAL_CALL_ResetProjectionMatrix
    "UnityEngine.Camera::INTERNAL_CALL_ResetAspect",    // -> Camera_CUSTOM_INTERNAL_CALL_ResetAspect
    "UnityEngine.Camera::INTERNAL_get_velocity",    // -> Camera_CUSTOM_INTERNAL_get_velocity
    "UnityEngine.Camera::get_clearFlags"    ,    // -> Camera_Get_Custom_PropClearFlags
    "UnityEngine.Camera::set_clearFlags"    ,    // -> Camera_Set_Custom_PropClearFlags
    "UnityEngine.Camera::INTERNAL_CALL_WorldToScreenPoint",    // -> Camera_CUSTOM_INTERNAL_CALL_WorldToScreenPoint
    "UnityEngine.Camera::INTERNAL_CALL_WorldToViewportPoint",    // -> Camera_CUSTOM_INTERNAL_CALL_WorldToViewportPoint
    "UnityEngine.Camera::INTERNAL_CALL_ViewportToWorldPoint",    // -> Camera_CUSTOM_INTERNAL_CALL_ViewportToWorldPoint
    "UnityEngine.Camera::INTERNAL_CALL_ScreenToWorldPoint",    // -> Camera_CUSTOM_INTERNAL_CALL_ScreenToWorldPoint
    "UnityEngine.Camera::INTERNAL_CALL_ScreenToViewportPoint",    // -> Camera_CUSTOM_INTERNAL_CALL_ScreenToViewportPoint
    "UnityEngine.Camera::INTERNAL_CALL_ViewportToScreenPoint",    // -> Camera_CUSTOM_INTERNAL_CALL_ViewportToScreenPoint
    "UnityEngine.Camera::INTERNAL_CALL_ViewportPointToRay",    // -> Camera_CUSTOM_INTERNAL_CALL_ViewportPointToRay
    "UnityEngine.Camera::INTERNAL_CALL_ScreenPointToRay",    // -> Camera_CUSTOM_INTERNAL_CALL_ScreenPointToRay
    "UnityEngine.Camera::get_main"          ,    // -> Camera_Get_Custom_PropMain
    "UnityEngine.Camera::get_current"       ,    // -> Camera_Get_Custom_PropCurrent
    "UnityEngine.Camera::get_allCameras"    ,    // -> Camera_Get_Custom_PropAllCameras
    "UnityEngine.Camera::GetScreenWidth"    ,    // -> Camera_CUSTOM_GetScreenWidth
    "UnityEngine.Camera::GetScreenHeight"   ,    // -> Camera_CUSTOM_GetScreenHeight
    "UnityEngine.Camera::DoClear"           ,    // -> Camera_CUSTOM_DoClear
    "UnityEngine.Camera::Render"            ,    // -> Camera_CUSTOM_Render
    "UnityEngine.Camera::RenderWithShader"  ,    // -> Camera_CUSTOM_RenderWithShader
    "UnityEngine.Camera::SetReplacementShader",    // -> Camera_CUSTOM_SetReplacementShader
    "UnityEngine.Camera::INTERNAL_CALL_ResetReplacementShader",    // -> Camera_CUSTOM_INTERNAL_CALL_ResetReplacementShader
    "UnityEngine.Camera::get_useOcclusionCulling",    // -> Camera_Get_Custom_PropUseOcclusionCulling
    "UnityEngine.Camera::set_useOcclusionCulling",    // -> Camera_Set_Custom_PropUseOcclusionCulling
    "UnityEngine.Camera::RenderDontRestore" ,    // -> Camera_CUSTOM_RenderDontRestore
    "UnityEngine.Camera::SetupCurrent"      ,    // -> Camera_CUSTOM_SetupCurrent
    "UnityEngine.Camera::Internal_RenderToCubemapRT",    // -> Camera_CUSTOM_Internal_RenderToCubemapRT
    "UnityEngine.Camera::Internal_RenderToCubemapTexture",    // -> Camera_CUSTOM_Internal_RenderToCubemapTexture
    "UnityEngine.Camera::get_layerCullDistances",    // -> Camera_Get_Custom_PropLayerCullDistances
    "UnityEngine.Camera::set_layerCullDistances",    // -> Camera_Set_Custom_PropLayerCullDistances
    "UnityEngine.Camera::get_layerCullSpherical",    // -> Camera_Get_Custom_PropLayerCullSpherical
    "UnityEngine.Camera::set_layerCullSpherical",    // -> Camera_Set_Custom_PropLayerCullSpherical
    "UnityEngine.Camera::CopyFrom"          ,    // -> Camera_CUSTOM_CopyFrom
    "UnityEngine.Camera::get_depthTextureMode",    // -> Camera_Get_Custom_PropDepthTextureMode
    "UnityEngine.Camera::set_depthTextureMode",    // -> Camera_Set_Custom_PropDepthTextureMode
    "UnityEngine.Camera::get_clearStencilAfterLightingPass",    // -> Camera_Get_Custom_PropClearStencilAfterLightingPass
    "UnityEngine.Camera::set_clearStencilAfterLightingPass",    // -> Camera_Set_Custom_PropClearStencilAfterLightingPass
    "UnityEngine.Camera::IsFiltered"        ,    // -> Camera_CUSTOM_IsFiltered
    NULL
};

static const void* s_UnityEngineCamera_IcallFuncs [] =
{
    (const void*)&Camera_Get_Custom_PropFieldOfView       ,  //  <- UnityEngine.Camera::get_fieldOfView
    (const void*)&Camera_Set_Custom_PropFieldOfView       ,  //  <- UnityEngine.Camera::set_fieldOfView
    (const void*)&Camera_Get_Custom_PropNearClipPlane     ,  //  <- UnityEngine.Camera::get_nearClipPlane
    (const void*)&Camera_Set_Custom_PropNearClipPlane     ,  //  <- UnityEngine.Camera::set_nearClipPlane
    (const void*)&Camera_Get_Custom_PropFarClipPlane      ,  //  <- UnityEngine.Camera::get_farClipPlane
    (const void*)&Camera_Set_Custom_PropFarClipPlane      ,  //  <- UnityEngine.Camera::set_farClipPlane
    (const void*)&Camera_Get_Custom_PropRenderingPath     ,  //  <- UnityEngine.Camera::get_renderingPath
    (const void*)&Camera_Set_Custom_PropRenderingPath     ,  //  <- UnityEngine.Camera::set_renderingPath
    (const void*)&Camera_Get_Custom_PropActualRenderingPath,  //  <- UnityEngine.Camera::get_actualRenderingPath
    (const void*)&Camera_Get_Custom_PropHdr               ,  //  <- UnityEngine.Camera::get_hdr
    (const void*)&Camera_Set_Custom_PropHdr               ,  //  <- UnityEngine.Camera::set_hdr
    (const void*)&Camera_Get_Custom_PropOrthographicSize  ,  //  <- UnityEngine.Camera::get_orthographicSize
    (const void*)&Camera_Set_Custom_PropOrthographicSize  ,  //  <- UnityEngine.Camera::set_orthographicSize
    (const void*)&Camera_Get_Custom_PropOrthographic      ,  //  <- UnityEngine.Camera::get_orthographic
    (const void*)&Camera_Set_Custom_PropOrthographic      ,  //  <- UnityEngine.Camera::set_orthographic
    (const void*)&Camera_Get_Custom_PropTransparencySortMode,  //  <- UnityEngine.Camera::get_transparencySortMode
    (const void*)&Camera_Set_Custom_PropTransparencySortMode,  //  <- UnityEngine.Camera::set_transparencySortMode
    (const void*)&Camera_Get_Custom_PropDepth             ,  //  <- UnityEngine.Camera::get_depth
    (const void*)&Camera_Set_Custom_PropDepth             ,  //  <- UnityEngine.Camera::set_depth
    (const void*)&Camera_Get_Custom_PropAspect            ,  //  <- UnityEngine.Camera::get_aspect
    (const void*)&Camera_Set_Custom_PropAspect            ,  //  <- UnityEngine.Camera::set_aspect
    (const void*)&Camera_Get_Custom_PropCullingMask       ,  //  <- UnityEngine.Camera::get_cullingMask
    (const void*)&Camera_Set_Custom_PropCullingMask       ,  //  <- UnityEngine.Camera::set_cullingMask
    (const void*)&Camera_Get_Custom_PropEventMask         ,  //  <- UnityEngine.Camera::get_eventMask
    (const void*)&Camera_Set_Custom_PropEventMask         ,  //  <- UnityEngine.Camera::set_eventMask
    (const void*)&Camera_CUSTOM_INTERNAL_get_backgroundColor,  //  <- UnityEngine.Camera::INTERNAL_get_backgroundColor
    (const void*)&Camera_CUSTOM_INTERNAL_set_backgroundColor,  //  <- UnityEngine.Camera::INTERNAL_set_backgroundColor
    (const void*)&Camera_CUSTOM_INTERNAL_get_rect         ,  //  <- UnityEngine.Camera::INTERNAL_get_rect
    (const void*)&Camera_CUSTOM_INTERNAL_set_rect         ,  //  <- UnityEngine.Camera::INTERNAL_set_rect
    (const void*)&Camera_CUSTOM_INTERNAL_get_pixelRect    ,  //  <- UnityEngine.Camera::INTERNAL_get_pixelRect
    (const void*)&Camera_CUSTOM_INTERNAL_set_pixelRect    ,  //  <- UnityEngine.Camera::INTERNAL_set_pixelRect
    (const void*)&Camera_Get_Custom_PropTargetTexture     ,  //  <- UnityEngine.Camera::get_targetTexture
    (const void*)&Camera_Set_Custom_PropTargetTexture     ,  //  <- UnityEngine.Camera::set_targetTexture
    (const void*)&Camera_CUSTOM_SetTargetBuffersImpl      ,  //  <- UnityEngine.Camera::SetTargetBuffersImpl
    (const void*)&Camera_Get_Custom_PropPixelWidth        ,  //  <- UnityEngine.Camera::get_pixelWidth
    (const void*)&Camera_Get_Custom_PropPixelHeight       ,  //  <- UnityEngine.Camera::get_pixelHeight
    (const void*)&Camera_CUSTOM_INTERNAL_get_cameraToWorldMatrix,  //  <- UnityEngine.Camera::INTERNAL_get_cameraToWorldMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_get_worldToCameraMatrix,  //  <- UnityEngine.Camera::INTERNAL_get_worldToCameraMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_set_worldToCameraMatrix,  //  <- UnityEngine.Camera::INTERNAL_set_worldToCameraMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ResetWorldToCameraMatrix,  //  <- UnityEngine.Camera::INTERNAL_CALL_ResetWorldToCameraMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_get_projectionMatrix,  //  <- UnityEngine.Camera::INTERNAL_get_projectionMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_set_projectionMatrix,  //  <- UnityEngine.Camera::INTERNAL_set_projectionMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ResetProjectionMatrix,  //  <- UnityEngine.Camera::INTERNAL_CALL_ResetProjectionMatrix
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ResetAspect ,  //  <- UnityEngine.Camera::INTERNAL_CALL_ResetAspect
    (const void*)&Camera_CUSTOM_INTERNAL_get_velocity     ,  //  <- UnityEngine.Camera::INTERNAL_get_velocity
    (const void*)&Camera_Get_Custom_PropClearFlags        ,  //  <- UnityEngine.Camera::get_clearFlags
    (const void*)&Camera_Set_Custom_PropClearFlags        ,  //  <- UnityEngine.Camera::set_clearFlags
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_WorldToScreenPoint,  //  <- UnityEngine.Camera::INTERNAL_CALL_WorldToScreenPoint
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_WorldToViewportPoint,  //  <- UnityEngine.Camera::INTERNAL_CALL_WorldToViewportPoint
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ViewportToWorldPoint,  //  <- UnityEngine.Camera::INTERNAL_CALL_ViewportToWorldPoint
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ScreenToWorldPoint,  //  <- UnityEngine.Camera::INTERNAL_CALL_ScreenToWorldPoint
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ScreenToViewportPoint,  //  <- UnityEngine.Camera::INTERNAL_CALL_ScreenToViewportPoint
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ViewportToScreenPoint,  //  <- UnityEngine.Camera::INTERNAL_CALL_ViewportToScreenPoint
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ViewportPointToRay,  //  <- UnityEngine.Camera::INTERNAL_CALL_ViewportPointToRay
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ScreenPointToRay,  //  <- UnityEngine.Camera::INTERNAL_CALL_ScreenPointToRay
    (const void*)&Camera_Get_Custom_PropMain              ,  //  <- UnityEngine.Camera::get_main
    (const void*)&Camera_Get_Custom_PropCurrent           ,  //  <- UnityEngine.Camera::get_current
    (const void*)&Camera_Get_Custom_PropAllCameras        ,  //  <- UnityEngine.Camera::get_allCameras
    (const void*)&Camera_CUSTOM_GetScreenWidth            ,  //  <- UnityEngine.Camera::GetScreenWidth
    (const void*)&Camera_CUSTOM_GetScreenHeight           ,  //  <- UnityEngine.Camera::GetScreenHeight
    (const void*)&Camera_CUSTOM_DoClear                   ,  //  <- UnityEngine.Camera::DoClear
    (const void*)&Camera_CUSTOM_Render                    ,  //  <- UnityEngine.Camera::Render
    (const void*)&Camera_CUSTOM_RenderWithShader          ,  //  <- UnityEngine.Camera::RenderWithShader
    (const void*)&Camera_CUSTOM_SetReplacementShader      ,  //  <- UnityEngine.Camera::SetReplacementShader
    (const void*)&Camera_CUSTOM_INTERNAL_CALL_ResetReplacementShader,  //  <- UnityEngine.Camera::INTERNAL_CALL_ResetReplacementShader
    (const void*)&Camera_Get_Custom_PropUseOcclusionCulling,  //  <- UnityEngine.Camera::get_useOcclusionCulling
    (const void*)&Camera_Set_Custom_PropUseOcclusionCulling,  //  <- UnityEngine.Camera::set_useOcclusionCulling
    (const void*)&Camera_CUSTOM_RenderDontRestore         ,  //  <- UnityEngine.Camera::RenderDontRestore
    (const void*)&Camera_CUSTOM_SetupCurrent              ,  //  <- UnityEngine.Camera::SetupCurrent
    (const void*)&Camera_CUSTOM_Internal_RenderToCubemapRT,  //  <- UnityEngine.Camera::Internal_RenderToCubemapRT
    (const void*)&Camera_CUSTOM_Internal_RenderToCubemapTexture,  //  <- UnityEngine.Camera::Internal_RenderToCubemapTexture
    (const void*)&Camera_Get_Custom_PropLayerCullDistances,  //  <- UnityEngine.Camera::get_layerCullDistances
    (const void*)&Camera_Set_Custom_PropLayerCullDistances,  //  <- UnityEngine.Camera::set_layerCullDistances
    (const void*)&Camera_Get_Custom_PropLayerCullSpherical,  //  <- UnityEngine.Camera::get_layerCullSpherical
    (const void*)&Camera_Set_Custom_PropLayerCullSpherical,  //  <- UnityEngine.Camera::set_layerCullSpherical
    (const void*)&Camera_CUSTOM_CopyFrom                  ,  //  <- UnityEngine.Camera::CopyFrom
    (const void*)&Camera_Get_Custom_PropDepthTextureMode  ,  //  <- UnityEngine.Camera::get_depthTextureMode
    (const void*)&Camera_Set_Custom_PropDepthTextureMode  ,  //  <- UnityEngine.Camera::set_depthTextureMode
    (const void*)&Camera_Get_Custom_PropClearStencilAfterLightingPass,  //  <- UnityEngine.Camera::get_clearStencilAfterLightingPass
    (const void*)&Camera_Set_Custom_PropClearStencilAfterLightingPass,  //  <- UnityEngine.Camera::set_clearStencilAfterLightingPass
    (const void*)&Camera_CUSTOM_IsFiltered                ,  //  <- UnityEngine.Camera::IsFiltered
    NULL
};

void ExportUnityEngineCameraBindings();
void ExportUnityEngineCameraBindings()
{
    for (int i = 0; s_UnityEngineCamera_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineCamera_IcallNames [i], s_UnityEngineCamera_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineCameraBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Camera_Get_Custom_PropFieldOfView); //  <- UnityEngine.Camera::get_fieldOfView
    SET_METRO_BINDING(Camera_Set_Custom_PropFieldOfView); //  <- UnityEngine.Camera::set_fieldOfView
    SET_METRO_BINDING(Camera_Get_Custom_PropNearClipPlane); //  <- UnityEngine.Camera::get_nearClipPlane
    SET_METRO_BINDING(Camera_Set_Custom_PropNearClipPlane); //  <- UnityEngine.Camera::set_nearClipPlane
    SET_METRO_BINDING(Camera_Get_Custom_PropFarClipPlane); //  <- UnityEngine.Camera::get_farClipPlane
    SET_METRO_BINDING(Camera_Set_Custom_PropFarClipPlane); //  <- UnityEngine.Camera::set_farClipPlane
    SET_METRO_BINDING(Camera_Get_Custom_PropRenderingPath); //  <- UnityEngine.Camera::get_renderingPath
    SET_METRO_BINDING(Camera_Set_Custom_PropRenderingPath); //  <- UnityEngine.Camera::set_renderingPath
    SET_METRO_BINDING(Camera_Get_Custom_PropActualRenderingPath); //  <- UnityEngine.Camera::get_actualRenderingPath
    SET_METRO_BINDING(Camera_Get_Custom_PropHdr); //  <- UnityEngine.Camera::get_hdr
    SET_METRO_BINDING(Camera_Set_Custom_PropHdr); //  <- UnityEngine.Camera::set_hdr
    SET_METRO_BINDING(Camera_Get_Custom_PropOrthographicSize); //  <- UnityEngine.Camera::get_orthographicSize
    SET_METRO_BINDING(Camera_Set_Custom_PropOrthographicSize); //  <- UnityEngine.Camera::set_orthographicSize
    SET_METRO_BINDING(Camera_Get_Custom_PropOrthographic); //  <- UnityEngine.Camera::get_orthographic
    SET_METRO_BINDING(Camera_Set_Custom_PropOrthographic); //  <- UnityEngine.Camera::set_orthographic
    SET_METRO_BINDING(Camera_Get_Custom_PropTransparencySortMode); //  <- UnityEngine.Camera::get_transparencySortMode
    SET_METRO_BINDING(Camera_Set_Custom_PropTransparencySortMode); //  <- UnityEngine.Camera::set_transparencySortMode
    SET_METRO_BINDING(Camera_Get_Custom_PropDepth); //  <- UnityEngine.Camera::get_depth
    SET_METRO_BINDING(Camera_Set_Custom_PropDepth); //  <- UnityEngine.Camera::set_depth
    SET_METRO_BINDING(Camera_Get_Custom_PropAspect); //  <- UnityEngine.Camera::get_aspect
    SET_METRO_BINDING(Camera_Set_Custom_PropAspect); //  <- UnityEngine.Camera::set_aspect
    SET_METRO_BINDING(Camera_Get_Custom_PropCullingMask); //  <- UnityEngine.Camera::get_cullingMask
    SET_METRO_BINDING(Camera_Set_Custom_PropCullingMask); //  <- UnityEngine.Camera::set_cullingMask
    SET_METRO_BINDING(Camera_Get_Custom_PropEventMask); //  <- UnityEngine.Camera::get_eventMask
    SET_METRO_BINDING(Camera_Set_Custom_PropEventMask); //  <- UnityEngine.Camera::set_eventMask
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_backgroundColor); //  <- UnityEngine.Camera::INTERNAL_get_backgroundColor
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_set_backgroundColor); //  <- UnityEngine.Camera::INTERNAL_set_backgroundColor
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_rect); //  <- UnityEngine.Camera::INTERNAL_get_rect
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_set_rect); //  <- UnityEngine.Camera::INTERNAL_set_rect
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_pixelRect); //  <- UnityEngine.Camera::INTERNAL_get_pixelRect
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_set_pixelRect); //  <- UnityEngine.Camera::INTERNAL_set_pixelRect
    SET_METRO_BINDING(Camera_Get_Custom_PropTargetTexture); //  <- UnityEngine.Camera::get_targetTexture
    SET_METRO_BINDING(Camera_Set_Custom_PropTargetTexture); //  <- UnityEngine.Camera::set_targetTexture
    SET_METRO_BINDING(Camera_CUSTOM_SetTargetBuffersImpl); //  <- UnityEngine.Camera::SetTargetBuffersImpl
    SET_METRO_BINDING(Camera_Get_Custom_PropPixelWidth); //  <- UnityEngine.Camera::get_pixelWidth
    SET_METRO_BINDING(Camera_Get_Custom_PropPixelHeight); //  <- UnityEngine.Camera::get_pixelHeight
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_cameraToWorldMatrix); //  <- UnityEngine.Camera::INTERNAL_get_cameraToWorldMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_worldToCameraMatrix); //  <- UnityEngine.Camera::INTERNAL_get_worldToCameraMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_set_worldToCameraMatrix); //  <- UnityEngine.Camera::INTERNAL_set_worldToCameraMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ResetWorldToCameraMatrix); //  <- UnityEngine.Camera::INTERNAL_CALL_ResetWorldToCameraMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_projectionMatrix); //  <- UnityEngine.Camera::INTERNAL_get_projectionMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_set_projectionMatrix); //  <- UnityEngine.Camera::INTERNAL_set_projectionMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ResetProjectionMatrix); //  <- UnityEngine.Camera::INTERNAL_CALL_ResetProjectionMatrix
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ResetAspect); //  <- UnityEngine.Camera::INTERNAL_CALL_ResetAspect
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_get_velocity); //  <- UnityEngine.Camera::INTERNAL_get_velocity
    SET_METRO_BINDING(Camera_Get_Custom_PropClearFlags); //  <- UnityEngine.Camera::get_clearFlags
    SET_METRO_BINDING(Camera_Set_Custom_PropClearFlags); //  <- UnityEngine.Camera::set_clearFlags
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_WorldToScreenPoint); //  <- UnityEngine.Camera::INTERNAL_CALL_WorldToScreenPoint
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_WorldToViewportPoint); //  <- UnityEngine.Camera::INTERNAL_CALL_WorldToViewportPoint
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ViewportToWorldPoint); //  <- UnityEngine.Camera::INTERNAL_CALL_ViewportToWorldPoint
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ScreenToWorldPoint); //  <- UnityEngine.Camera::INTERNAL_CALL_ScreenToWorldPoint
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ScreenToViewportPoint); //  <- UnityEngine.Camera::INTERNAL_CALL_ScreenToViewportPoint
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ViewportToScreenPoint); //  <- UnityEngine.Camera::INTERNAL_CALL_ViewportToScreenPoint
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ViewportPointToRay); //  <- UnityEngine.Camera::INTERNAL_CALL_ViewportPointToRay
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ScreenPointToRay); //  <- UnityEngine.Camera::INTERNAL_CALL_ScreenPointToRay
    SET_METRO_BINDING(Camera_Get_Custom_PropMain); //  <- UnityEngine.Camera::get_main
    SET_METRO_BINDING(Camera_Get_Custom_PropCurrent); //  <- UnityEngine.Camera::get_current
    SET_METRO_BINDING(Camera_Get_Custom_PropAllCameras); //  <- UnityEngine.Camera::get_allCameras
    SET_METRO_BINDING(Camera_CUSTOM_GetScreenWidth); //  <- UnityEngine.Camera::GetScreenWidth
    SET_METRO_BINDING(Camera_CUSTOM_GetScreenHeight); //  <- UnityEngine.Camera::GetScreenHeight
    SET_METRO_BINDING(Camera_CUSTOM_DoClear); //  <- UnityEngine.Camera::DoClear
    SET_METRO_BINDING(Camera_CUSTOM_Render); //  <- UnityEngine.Camera::Render
    SET_METRO_BINDING(Camera_CUSTOM_RenderWithShader); //  <- UnityEngine.Camera::RenderWithShader
    SET_METRO_BINDING(Camera_CUSTOM_SetReplacementShader); //  <- UnityEngine.Camera::SetReplacementShader
    SET_METRO_BINDING(Camera_CUSTOM_INTERNAL_CALL_ResetReplacementShader); //  <- UnityEngine.Camera::INTERNAL_CALL_ResetReplacementShader
    SET_METRO_BINDING(Camera_Get_Custom_PropUseOcclusionCulling); //  <- UnityEngine.Camera::get_useOcclusionCulling
    SET_METRO_BINDING(Camera_Set_Custom_PropUseOcclusionCulling); //  <- UnityEngine.Camera::set_useOcclusionCulling
    SET_METRO_BINDING(Camera_CUSTOM_RenderDontRestore); //  <- UnityEngine.Camera::RenderDontRestore
    SET_METRO_BINDING(Camera_CUSTOM_SetupCurrent); //  <- UnityEngine.Camera::SetupCurrent
    SET_METRO_BINDING(Camera_CUSTOM_Internal_RenderToCubemapRT); //  <- UnityEngine.Camera::Internal_RenderToCubemapRT
    SET_METRO_BINDING(Camera_CUSTOM_Internal_RenderToCubemapTexture); //  <- UnityEngine.Camera::Internal_RenderToCubemapTexture
    SET_METRO_BINDING(Camera_Get_Custom_PropLayerCullDistances); //  <- UnityEngine.Camera::get_layerCullDistances
    SET_METRO_BINDING(Camera_Set_Custom_PropLayerCullDistances); //  <- UnityEngine.Camera::set_layerCullDistances
    SET_METRO_BINDING(Camera_Get_Custom_PropLayerCullSpherical); //  <- UnityEngine.Camera::get_layerCullSpherical
    SET_METRO_BINDING(Camera_Set_Custom_PropLayerCullSpherical); //  <- UnityEngine.Camera::set_layerCullSpherical
    SET_METRO_BINDING(Camera_CUSTOM_CopyFrom); //  <- UnityEngine.Camera::CopyFrom
    SET_METRO_BINDING(Camera_Get_Custom_PropDepthTextureMode); //  <- UnityEngine.Camera::get_depthTextureMode
    SET_METRO_BINDING(Camera_Set_Custom_PropDepthTextureMode); //  <- UnityEngine.Camera::set_depthTextureMode
    SET_METRO_BINDING(Camera_Get_Custom_PropClearStencilAfterLightingPass); //  <- UnityEngine.Camera::get_clearStencilAfterLightingPass
    SET_METRO_BINDING(Camera_Set_Custom_PropClearStencilAfterLightingPass); //  <- UnityEngine.Camera::set_clearStencilAfterLightingPass
    SET_METRO_BINDING(Camera_CUSTOM_IsFiltered); //  <- UnityEngine.Camera::IsFiltered
}

#endif
