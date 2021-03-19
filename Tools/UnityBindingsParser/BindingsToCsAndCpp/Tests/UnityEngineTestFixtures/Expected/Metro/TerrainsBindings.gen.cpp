#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Graphics/Heightmap.h"
#include "Runtime/Filters/Mesh/LodMesh.h"
#include "Runtime/Terrain/DetailDatabase.h"
#include "Runtime/Terrain/SplatDatabase.h"
#include "Runtime/Terrain/TerrainData.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/BaseClasses/GameObject.h"
#include "Runtime/Terrain/TerrainRenderer.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Terrain/DetailRenderer.h"
#include "Runtime/Terrain/ImposterRenderTexture.h"
#include "Runtime/Terrain/TreeRenderer.h"
#include "Runtime/Terrain/Wind.h"
#include "Runtime/Terrain/Tree.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

using namespace Unity;
using namespace std;


	#if ENABLE_TERRAIN
	inline TerrainRenderer* GetTerrainRenderer(ScriptingObjectPtr self)
	{
		TerrainRenderer* r;
		MarshallManagedStructIntoNative(self,&r);
		return r;
	}
	#define GET	GetTerrainRenderer(self)
	#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_INTERNAL_CALL_TerrainRenderer(ICallType_ReadOnlyUnityEngineObject_Argument self_, int instanceID, ICallType_ReadOnlyUnityEngineObject_Argument terrainData_, const Vector3f& position, int lightmapIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_INTERNAL_CALL_TerrainRenderer)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<TerrainData> terrainData(terrainData_);
    UNUSED(terrainData);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TerrainRenderer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_TerrainRenderer)
    
    		TerrainRenderer* t = new TerrainRenderer (instanceID, terrainData, position, lightmapIndex);
    		MarshallNativeStructIntoManaged(t,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_Dispose(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_Dispose)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Dispose)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Dispose)
    
    		GET->Cleanup();
    		delete GET;
    		
    		TerrainRenderer* t = NULL;
    		MarshallNativeStructIntoManaged(t,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_ReloadPrecomputedError(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_ReloadPrecomputedError)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReloadPrecomputedError)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReloadPrecomputedError)
     GET->ReloadPrecomputedError(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_ReloadBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_ReloadBounds)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReloadBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReloadBounds)
     GET->ReloadBounds(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_ReloadAll(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_ReloadAll)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReloadAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReloadAll)
     GET->ReloadAll(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_Internal_RenderStep1(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument camera_, int maxLodLevel, float tau, float splatDistance)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_Internal_RenderStep1)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(Internal_RenderStep1)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_RenderStep1)
    
    		GET->RenderStep1 (camera, maxLodLevel, tau, splatDistance);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_RenderStep2(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_RenderStep2)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RenderStep2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RenderStep2)
     GET->RenderStep2(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_RenderStep3(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument camera_, int layer, ScriptingBool castShadows, ICallType_ReadOnlyUnityEngineObject_Argument mat_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_RenderStep3)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    SCRIPTINGAPI_STACK_CHECK(RenderStep3)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RenderStep3)
     GET->RenderStep3(camera, layer, castShadows, mat); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_SetNeighbors(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument left_, ICallType_ReadOnlyUnityEngineObject_Argument top_, ICallType_ReadOnlyUnityEngineObject_Argument right_, ICallType_ReadOnlyUnityEngineObject_Argument bottom_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_SetNeighbors)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<TerrainRenderer> left(left_);
    UNUSED(left);
    ReadOnlyScriptingObjectOfType<TerrainRenderer> top(top_);
    UNUSED(top);
    ReadOnlyScriptingObjectOfType<TerrainRenderer> right(right_);
    UNUSED(right);
    ReadOnlyScriptingObjectOfType<TerrainRenderer> bottom(bottom_);
    UNUSED(bottom);
    SCRIPTINGAPI_STACK_CHECK(SetNeighbors)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetNeighbors)
    
    		TerrainRenderer* nativeLeft = NULL;
    		TerrainRenderer* nativeTop = NULL;
    		TerrainRenderer* nativeRight = NULL;
    		TerrainRenderer* nativeBottom = NULL;
    		if (left)  MarshallManagedStructIntoNative(left,&nativeLeft);
    		if (top)   MarshallManagedStructIntoNative(top,&nativeTop);
    		if (right) MarshallManagedStructIntoNative(right,&nativeRight);
    		if (bottom)MarshallManagedStructIntoNative(bottom,&nativeBottom);
    
    		GET->SetNeighbors (nativeLeft, nativeTop, nativeRight, nativeBottom);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION TerrainRenderer_Get_Custom_PropTerrainData(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_Get_Custom_PropTerrainData)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_terrainData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_terrainData)
    return Scripting::ScriptingWrapperFor(GET->GetTerrainData ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_Set_Custom_PropTerrainData(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_Set_Custom_PropTerrainData)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<TerrainData> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_terrainData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_terrainData)
     GET->SetTerrainData (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainRenderer_Get_Custom_PropLightmapIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_Get_Custom_PropLightmapIndex)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightmapIndex)
    return GET->GetLightmapIndex ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_Set_Custom_PropLightmapIndex(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_Set_Custom_PropLightmapIndex)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightmapIndex)
     GET->SetLightmapIndex (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainRenderer_CUSTOM_SetLightmapSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, int lightmapSize)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainRenderer_CUSTOM_SetLightmapSize)
    ReadOnlyScriptingObjectOfType<TerrainRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetLightmapSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLightmapSize)
     
    		#if UNITY_EDITOR 
    		GET->SetLightmapSize (lightmapSize); 
    		#endif
    	
}

#endif

 
	inline DetailRenderer* GetDetailRenderer(ScriptingObjectPtr self)
	{
		DetailRenderer* r;
		MarshallManagedStructIntoNative(self,&r);
		return r;
	}
	#define GETDETAILRENDERER	GetDetailRenderer(self)

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION DetailRenderer_CUSTOM_INTERNAL_CALL_DetailRenderer(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument terrainData_, const Vector3f& position, int lightmapIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_CUSTOM_INTERNAL_CALL_DetailRenderer)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<TerrainData> terrainData(terrainData_);
    UNUSED(terrainData);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DetailRenderer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DetailRenderer)
    
    		DetailRenderer* r = new DetailRenderer (terrainData, position, lightmapIndex);
    		MarshallNativeStructIntoManaged(r,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION DetailRenderer_CUSTOM_Dispose(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_CUSTOM_Dispose)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Dispose)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Dispose)
    
    		GETDETAILRENDERER->Cleanup();
    		delete GETDETAILRENDERER;
    		
    		DetailRenderer* r = NULL;
    		MarshallNativeStructIntoManaged(r,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION DetailRenderer_CUSTOM_Render(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument camera_, float viewDistance, int layer, float detailDensity)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_CUSTOM_Render)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(Render)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Render)
     
    		GETDETAILRENDERER->Render(camera, viewDistance, layer, detailDensity);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION DetailRenderer_Get_Custom_PropLightmapIndex(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_Get_Custom_PropLightmapIndex)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightmapIndex)
    return GETDETAILRENDERER->GetLightmapIndex ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION DetailRenderer_Set_Custom_PropLightmapIndex(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_Set_Custom_PropLightmapIndex)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightmapIndex)
     GETDETAILRENDERER->SetLightmapIndex (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION DetailRenderer_CUSTOM_ReloadAllDetails(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_CUSTOM_ReloadAllDetails)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReloadAllDetails)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReloadAllDetails)
     GETDETAILRENDERER->ReloadAllDetails(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION DetailRenderer_CUSTOM_ReloadDirtyDetails(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(DetailRenderer_CUSTOM_ReloadDirtyDetails)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReloadDirtyDetails)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReloadDirtyDetails)
     GETDETAILRENDERER->ReloadDirtyDetails(); 
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_Internal_Create(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument terrainData_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_Internal_Create)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ScriptingObjectOfType<TerrainData> terrainData(terrainData_);
    UNUSED(terrainData);
    SCRIPTINGAPI_STACK_CHECK(Internal_Create)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Create)
    
    		TerrainData* td = NEW_OBJECT (TerrainData);
    		td->Reset();
    
    		if (td)
    			td->GetHeightmap().SetResolution(0);
    
    		ConnectScriptingWrapperToObject (terrainData.GetScriptingObject(), td);
    		td->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_HasUser(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument user_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_HasUser)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<GameObject> user(user_);
    UNUSED(user);
    SCRIPTINGAPI_STACK_CHECK(HasUser)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasUser)
     return self->HasUser (user); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_AddUser(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument user_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_AddUser)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<GameObject> user(user_);
    UNUSED(user);
    SCRIPTINGAPI_STACK_CHECK(AddUser)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddUser)
     self->AddUser (user); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_RemoveUser(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument user_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_RemoveUser)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<GameObject> user(user_);
    UNUSED(user);
    SCRIPTINGAPI_STACK_CHECK(RemoveUser)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RemoveUser)
     self->RemoveUser (user); 
}

#endif

 #define GETHEIGHT (&(self->GetHeightmap()))
	

 #define GETDETAIL (&(self->GetDetailDatabase()))

 #define GETTREEDATABASE (&(self->GetTreeDatabase()))
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropHeightmapWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropHeightmapWidth)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_heightmapWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_heightmapWidth)
    return GETHEIGHT->GetWidth ();
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropHeightmapHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropHeightmapHeight)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_heightmapHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_heightmapHeight)
    return GETHEIGHT->GetHeight ();
}

#endif

	
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropHeightmapResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropHeightmapResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_heightmapResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_heightmapResolution)
    return GETHEIGHT->GetResolution ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropHeightmapResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropHeightmapResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_heightmapResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_heightmapResolution)
     GETHEIGHT->SetResolution (value); 
}

#endif

	
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_get_heightmapScale(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_get_heightmapScale)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_heightmapScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_heightmapScale)
    { *returnValue =(GETHEIGHT->GetScale ()); return;};
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_get_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_get_size)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_size)
    { *returnValue =(GETHEIGHT->GetSize ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_set_size(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_set_size)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_size)
     GETHEIGHT->SetSize (value); 
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetHeight)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetHeight)
     return GETHEIGHT->GetHeight (x,y); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetInterpolatedHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, float x, float y)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetInterpolatedHeight)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetInterpolatedHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetInterpolatedHeight)
     return GETHEIGHT->GetInterpolatedHeight (x,y); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetHeights(ICallType_ReadOnlyUnityEngineObject_Argument self_, int xBase, int yBase, int width, int height)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetHeights)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetHeights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetHeights)
    
    	
    		if(xBase < 0 || yBase < 0 || xBase+width > GETHEIGHT->GetWidth() || yBase+height > GETHEIGHT->GetHeight ())
    		{
    			RaiseMonoException ("Trying to access out-of-bounds terrain height information.");
    			return SCRIPTING_NULL;
    		}
    
    		ScriptingArrayPtr map = CreateScriptingArray2D<float> (GetCommonScriptingClasses ().floatSingle, height, width);
    		GETHEIGHT->GetHeights (xBase, yBase, width, height, &GetScriptingArrayElement<float>(map, 0));
    		return map;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_Internal_SetHeights(ICallType_ReadOnlyUnityEngineObject_Argument self_, int xBase, int yBase, int width, int height, ICallType_Array_Argument heights_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_Internal_SetHeights)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local heights(heights_);
    UNUSED(heights);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetHeights)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetHeights)
    
    		GETHEIGHT->SetHeights(xBase, yBase, width, height, &GetScriptingArrayElement<float>(heights, 0), false);
    		GETTREEDATABASE->RecalculateTreePositions();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_Internal_SetHeightsDelayLOD(ICallType_ReadOnlyUnityEngineObject_Argument self_, int xBase, int yBase, int width, int height, ICallType_Array_Argument heights_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_Internal_SetHeightsDelayLOD)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local heights(heights_);
    UNUSED(heights);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetHeightsDelayLOD)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetHeightsDelayLOD)
    
    		GETHEIGHT->SetHeights(xBase, yBase, width, height, &GetScriptingArrayElement<float>(heights, 0), true);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_ComputeDelayedLod(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_ComputeDelayedLod)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ComputeDelayedLod)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ComputeDelayedLod)
    
    		vector<int> invalid;
    		GETHEIGHT->RecomputeInvalidPatches(invalid);
    		
    		return CreateScriptingArray(&invalid[0], invalid.size(), GetScriptingManager().GetCommonClasses().int_32);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetSteepness(ICallType_ReadOnlyUnityEngineObject_Argument self_, float x, float y)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetSteepness)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetSteepness)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetSteepness)
     return GETHEIGHT->GetSteepness (x,y); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_CALL_GetInterpolatedNormal(ICallType_ReadOnlyUnityEngineObject_Argument self_, float x, float y, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_CALL_GetInterpolatedNormal)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetInterpolatedNormal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetInterpolatedNormal)
     { returnValue =(GETHEIGHT->GetInterpolatedNormal (x,y)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetAdjustedSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, int size)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetAdjustedSize)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetAdjustedSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAdjustedSize)
     return GETHEIGHT->GetAdjustedSize (size); 
}

#endif

 #undef GETHEIGHT
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropWavingGrassStrength(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropWavingGrassStrength)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wavingGrassStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wavingGrassStrength)
    return GETDETAIL->GetWavingGrassStrength();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropWavingGrassStrength(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropWavingGrassStrength)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wavingGrassStrength)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wavingGrassStrength)
     GETDETAIL->SetWavingGrassStrength (value); self->SetDirty(); 
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropWavingGrassAmount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropWavingGrassAmount)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wavingGrassAmount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wavingGrassAmount)
    return GETDETAIL->GetWavingGrassAmount();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropWavingGrassAmount(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropWavingGrassAmount)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wavingGrassAmount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wavingGrassAmount)
     GETDETAIL-> SetWavingGrassAmount (value); self->SetDirty(); 
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropWavingGrassSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropWavingGrassSpeed)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wavingGrassSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wavingGrassSpeed)
    return GETDETAIL->GetWavingGrassSpeed();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropWavingGrassSpeed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropWavingGrassSpeed)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wavingGrassSpeed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wavingGrassSpeed)
     GETDETAIL-> SetWavingGrassSpeed (value); self->SetDirty(); 
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_get_wavingGrassTint(ICallType_ReadOnlyUnityEngineObject_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_get_wavingGrassTint)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_wavingGrassTint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_wavingGrassTint)
    { *returnValue =(GETDETAIL->GetWavingGrassTint()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_set_wavingGrassTint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_set_wavingGrassTint)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_wavingGrassTint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_wavingGrassTint)
     GETDETAIL-> SetWavingGrassTint (value); self->SetDirty(); 
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropDetailWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropDetailWidth)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detailWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detailWidth)
    return GETDETAIL->GetWidth ();
}

#endif

	
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropDetailHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropDetailHeight)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detailHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detailHeight)
    return GETDETAIL->GetHeight ();
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_SetDetailResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_, int detailResolution, int resolutionPerPatch)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_SetDetailResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetDetailResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetDetailResolution)
    
    		GETDETAIL->SetDetailResolution(detailResolution, resolutionPerPatch);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropDetailResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropDetailResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detailResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detailResolution)
    return GETDETAIL->GetResolution ();
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropDetailResolutionPerPatch(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropDetailResolutionPerPatch)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detailResolutionPerPatch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detailResolutionPerPatch)
    return GETDETAIL->GetResolutionPerPatch ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_ResetDirtyDetails(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_ResetDirtyDetails)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ResetDirtyDetails)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ResetDirtyDetails)
     GETDETAIL->ResetDirtyDetails (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_RefreshPrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_RefreshPrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RefreshPrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RefreshPrototypes)
    
    		GETDETAIL->RefreshPrototypes ();
    		GETTREEDATABASE->RefreshPrototypes ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropDetailPrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropDetailPrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_detailPrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_detailPrototypes)
    return VectorToScriptingClassArray<DetailPrototype, MonoDetailPrototype> (GETDETAIL->GetDetailPrototypes(), GetCommonScriptingClasses ().detailPrototype, DetailPrototypeToMono);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropDetailPrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropDetailPrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_detailPrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_detailPrototypes)
    
    GETDETAIL->SetDetailPrototypes (ScriptingClassArrayToVector<DetailPrototype, MonoDetailPrototype> (value, DetailPrototypeToCpp));
    
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetSupportedLayers(ICallType_ReadOnlyUnityEngineObject_Argument self_, int xBase, int yBase, int totalWidth, int totalHeight)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetSupportedLayers)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetSupportedLayers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetSupportedLayers)
    
    		int size = GETDETAIL->GetSupportedLayers (xBase, yBase, totalWidth, totalHeight, NULL);  
    		ScriptingArrayPtr arr = CreateScriptingArray<int>(GetCommonScriptingClasses ().int_32, size);
    		GETDETAIL->GetSupportedLayers (xBase, yBase, totalWidth, totalHeight, &GetScriptingArrayElement<int> (arr, 0));
    		return arr;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetDetailLayer(ICallType_ReadOnlyUnityEngineObject_Argument self_, int xBase, int yBase, int width, int height, int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetDetailLayer)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetDetailLayer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetDetailLayer)
    
    		ScriptingArrayPtr map = CreateScriptingArray2D<int> (GetCommonScriptingClasses ().int_32, height, width);
    		GETDETAIL->GetLayer (xBase, yBase, width, height, layer, &GetScriptingArrayElement<int> (map, 0));
    		return map;	
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_Internal_SetDetailLayer(ICallType_ReadOnlyUnityEngineObject_Argument self_, int xBase, int yBase, int totalWidth, int totalHeight, int detailIndex, ICallType_Array_Argument data_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_Internal_SetDetailLayer)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetDetailLayer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetDetailLayer)
    
    		GETDETAIL->SetLayer (xBase, yBase, totalWidth, totalHeight, detailIndex, &GetScriptingArrayElement<int> (data, 0));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropTreeInstances(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropTreeInstances)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_treeInstances)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_treeInstances)
    return CreateScriptingArray(&GETTREEDATABASE->GetInstances()[0], GETTREEDATABASE->GetInstances().size(), GetCommonScriptingClasses ().treeInstance);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropTreeInstances(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropTreeInstances)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_treeInstances)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_treeInstances)
    
    MonoRaiseIfNull((ScriptingObjectPtr)value);
    TreeInstance *first = &GetScriptingArrayElement<TreeInstance> (value, 0);
    GETTREEDATABASE->GetInstances().assign (first, first + GetScriptingArraySize(value));
    GETTREEDATABASE->UpdateTreeInstances();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropTreePrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropTreePrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_treePrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_treePrototypes)
    return VectorToScriptingClassArray<TreePrototype, MonoTreePrototype> (GETTREEDATABASE->GetTreePrototypes(), GetCommonScriptingClasses ().treePrototype, TreePrototypeToMono);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropTreePrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropTreePrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_treePrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_treePrototypes)
     GETTREEDATABASE->SetTreePrototypes (ScriptingClassArrayToVector<TreePrototype, MonoTreePrototype> (value, TreePrototypeToCpp)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_RemoveTreePrototype(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_RemoveTreePrototype)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RemoveTreePrototype)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RemoveTreePrototype)
    
    		GETTREEDATABASE->RemoveTreePrototype (index);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_RecalculateTreePositions(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_RecalculateTreePositions)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RecalculateTreePositions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RecalculateTreePositions)
    
    		GETTREEDATABASE->RecalculateTreePositions ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_RemoveDetailPrototype(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_RemoveDetailPrototype)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RemoveDetailPrototype)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RemoveDetailPrototype)
    
    		GETDETAIL->RemoveDetailPrototype (index);
    	
}

#endif

 #undef GETDETAIL
	
	

 #define GETSPLAT (&(self->GetSplatDatabase()))
	
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropAlphamapLayers(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropAlphamapLayers)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphamapLayers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alphamapLayers)
    return GETSPLAT->GetDepth();
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetAlphamaps(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, int width, int height)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetAlphamaps)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetAlphamaps)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAlphamaps)
    
    		ScriptingArrayPtr map = CreateScriptingArray3D<float> (GetCommonScriptingClasses ().floatSingle, height, width, GETSPLAT->GetDepth ());
    		GETSPLAT->GetAlphamaps (x, y, width, height, &GetScriptingArrayElement<float>(map, 0));
    		return map;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropAlphamapResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropAlphamapResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphamapResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alphamapResolution)
    return GETSPLAT->GetAlphamapResolution();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropAlphamapResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropAlphamapResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_alphamapResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alphamapResolution)
     return GETSPLAT->SetAlphamapResolution(value); 
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropAlphamapWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropAlphamapWidth)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphamapWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alphamapWidth)
    return GETSPLAT->GetAlphamapResolution();
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropAlphamapHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropAlphamapHeight)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphamapHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alphamapHeight)
    return GETSPLAT->GetAlphamapResolution();
}

#endif
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropBaseMapResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropBaseMapResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_baseMapResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_baseMapResolution)
    return GETSPLAT->GetBaseMapResolution();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropBaseMapResolution(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropBaseMapResolution)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_baseMapResolution)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_baseMapResolution)
     return GETSPLAT->SetBaseMapResolution(value); 
}

#endif

#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_Internal_SetAlphamaps(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, int width, int height, ICallType_Array_Argument map_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_Internal_SetAlphamaps)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local map(map_);
    UNUSED(map);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetAlphamaps)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetAlphamaps)
    
    		GETSPLAT->SetAlphamaps (x, y, width, height, &GetScriptingArrayElement<float>(map, 0));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_RecalculateBasemapIfDirty(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_RecalculateBasemapIfDirty)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RecalculateBasemapIfDirty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RecalculateBasemapIfDirty)
    
    		GETSPLAT->RecalculateBasemapIfDirty();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_SetBasemapDirty(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool dirty)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_SetBasemapDirty)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetBasemapDirty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetBasemapDirty)
    
    		GETSPLAT->SetBasemapDirty(dirty);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_GetAlphamapTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_GetAlphamapTexture)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetAlphamapTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAlphamapTexture)
    
    		return Scripting::ScriptingWrapperFor (GETSPLAT->GetAlphaTexture(index)); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropAlphamapTextureCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropAlphamapTextureCount)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphamapTextureCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alphamapTextureCount)
    return GETSPLAT->GetAlphaTextureCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TerrainData_Get_Custom_PropSplatPrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Get_Custom_PropSplatPrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_splatPrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_splatPrototypes)
    return VectorToScriptingClassArray<SplatPrototype, MonoSplatPrototype> (GETSPLAT->GetSplatPrototypes(), GetCommonScriptingClasses ().splatPrototype, SplatPrototypeToMono);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_Set_Custom_PropSplatPrototypes(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_Set_Custom_PropSplatPrototypes)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_splatPrototypes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_splatPrototypes)
     GETSPLAT->SetSplatPrototypes (ScriptingClassArrayToVector<SplatPrototype, MonoSplatPrototype> (value, SplatPrototypeToCpp)); 
}

#endif

 #undef GET
	
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_HasTreeInstances(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_HasTreeInstances)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(HasTreeInstances)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasTreeInstances)
    
    		return !self->GetTreeDatabase().GetInstances().empty();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_AddTree(ICallType_ReadOnlyUnityEngineObject_Argument self_, TreeInstance* tree)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_AddTree)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(AddTree)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddTree)
    
    		self->GetTreeDatabase().AddTree (*tree);	
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TerrainData_CUSTOM_INTERNAL_CALL_RemoveTrees(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector2fIcall& position, float radius, int prototypeIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainData_CUSTOM_INTERNAL_CALL_RemoveTrees)
    ReadOnlyScriptingObjectOfType<TerrainData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RemoveTrees)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_RemoveTrees)
    
    		return self->GetTreeDatabase().RemoveTrees (position, radius, prototypeIndex);
    	
}

#endif



struct MonoRenderArguments {
	float meshTreeDistance;
	float billboardTreeDistance;
	float crossFadeLength;
	int maximumMeshTrees;
	int layer;
};


	inline TreeRenderer* GetTreeRenderer(ScriptingObjectPtr self)
	{
		TreeRenderer* r;
		MarshallManagedStructIntoNative(self,&r);
		return r;
	}
	#define GETTREERENDERER	GetTreeRenderer(self)
	
#if ENABLE_TERRAIN
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_INTERNAL_CALL_TreeRenderer(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument data_, const Vector3f& position, int lightmapIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_INTERNAL_CALL_TreeRenderer)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<TerrainData> data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TreeRenderer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_TreeRenderer)
    
    		TreeRenderer* r = new TreeRenderer(data->GetTreeDatabase(), position, lightmapIndex);
    		MarshallNativeStructIntoManaged(r,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_ReloadTrees(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_ReloadTrees)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReloadTrees)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ReloadTrees)
    
    		GETTREERENDERER->ReloadTrees();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_Render(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument camera_, ICallType_Array_Argument lights_, MonoRenderArguments& arguments)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_Render)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    ICallType_Array_Local lights(lights_);
    UNUSED(lights);
    SCRIPTINGAPI_STACK_CHECK(Render)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Render)
    
    		const int maxLights = 16;
    		int lightCount = GetScriptingArraySize(lights);
    		if(lightCount > maxLights)
    			lightCount = maxLights;
    		
    		std::vector<Light*> lightPtrs(lightCount);
    		for( int i = 0; i < lightCount; ++i )
    		{
    			Light* light = ScriptingObjectToObject<Light>( GetScriptingArrayElementNoRef<ScriptingObjectPtr>( lights, i ) );
    			lightPtrs[i] = light;
    		}
    	
    		GETTREERENDERER->Render(*(camera), lightPtrs, arguments.meshTreeDistance, arguments.billboardTreeDistance, arguments.crossFadeLength, arguments.maximumMeshTrees, arguments.layer);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_RenderShadowCasters(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument light_, ICallType_ReadOnlyUnityEngineObject_Argument camera_, float meshTreeDistance, int maximumMeshTrees, int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_RenderShadowCasters)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Light> light(light_);
    UNUSED(light);
    ReadOnlyScriptingObjectOfType<Camera> camera(camera_);
    UNUSED(camera);
    SCRIPTINGAPI_STACK_CHECK(RenderShadowCasters)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RenderShadowCasters)
    
    		GETTREERENDERER->RenderShadowCasters(*light, *camera, meshTreeDistance, maximumMeshTrees, layer);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_Cleanup)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Cleanup)
    
    		delete GETTREERENDERER;
    		TreeRenderer* r = NULL;
    		MarshallNativeStructIntoManaged(r,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_InjectTree(ICallType_Object_Argument self_, TreeInstance* newTree)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_InjectTree)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InjectTree)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InjectTree)
    
    		GETTREERENDERER->InjectTree (*newTree);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_INTERNAL_CALL_RemoveTrees(ICallType_Object_Argument self_, const Vector3f& pos, float radius, int prototypeIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_INTERNAL_CALL_RemoveTrees)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RemoveTrees)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_RemoveTrees)
    
    		GETTREERENDERER->RemoveTrees(pos, radius, prototypeIndex);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_CUSTOM_InvalidateImposters(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_CUSTOM_InvalidateImposters)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InvalidateImposters)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InvalidateImposters)
    
    		GETTREERENDERER->InvalidateImposter ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION TreeRenderer_Get_Custom_PropLightmapIndex(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_Get_Custom_PropLightmapIndex)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lightmapIndex)
    return GETTREERENDERER->GetLightmapIndex ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TreeRenderer_Set_Custom_PropLightmapIndex(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(TreeRenderer_Set_Custom_PropLightmapIndex)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lightmapIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lightmapIndex)
     GETTREERENDERER->SetLightmapIndex (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
WindZone::WindZoneMode SCRIPT_CALL_CONVENTION WindZone_Get_Custom_PropMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Get_Custom_PropMode)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mode)
    return self->GetMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WindZone_Set_Custom_PropMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, WindZone::WindZoneMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Set_Custom_PropMode)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_mode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mode)
    
    self->SetMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WindZone_Get_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Get_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_radius)
    return self->GetRadius ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WindZone_Set_Custom_PropRadius(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Set_Custom_PropRadius)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_radius)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_radius)
    
    self->SetRadius (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WindZone_Get_Custom_PropWindMain(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Get_Custom_PropWindMain)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_windMain)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_windMain)
    return self->GetWindMain ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WindZone_Set_Custom_PropWindMain(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Set_Custom_PropWindMain)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_windMain)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_windMain)
    
    self->SetWindMain (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WindZone_Get_Custom_PropWindTurbulence(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Get_Custom_PropWindTurbulence)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_windTurbulence)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_windTurbulence)
    return self->GetWindTurbulence ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WindZone_Set_Custom_PropWindTurbulence(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Set_Custom_PropWindTurbulence)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_windTurbulence)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_windTurbulence)
    
    self->SetWindTurbulence (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WindZone_Get_Custom_PropWindPulseMagnitude(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Get_Custom_PropWindPulseMagnitude)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_windPulseMagnitude)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_windPulseMagnitude)
    return self->GetWindPulseMagnitude ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WindZone_Set_Custom_PropWindPulseMagnitude(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Set_Custom_PropWindPulseMagnitude)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_windPulseMagnitude)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_windPulseMagnitude)
    
    self->SetWindPulseMagnitude (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WindZone_Get_Custom_PropWindPulseFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Get_Custom_PropWindPulseFrequency)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_windPulseFrequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_windPulseFrequency)
    return self->GetWindPulseFrequency ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WindZone_Set_Custom_PropWindPulseFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WindZone_Set_Custom_PropWindPulseFrequency)
    ReadOnlyScriptingObjectOfType<WindZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_windPulseFrequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_windPulseFrequency)
    
    self->SetWindPulseFrequency (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Tree_Get_Custom_PropData(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Tree_Get_Custom_PropData)
    ReadOnlyScriptingObjectOfType<Tree> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_data)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_data)
    return Scripting::ScriptingWrapperFor(self->GetTreeData());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Tree_Set_Custom_PropData(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(Tree_Set_Custom_PropData)
    ReadOnlyScriptingObjectOfType<Tree> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<MonoBehaviour> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_data)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_data)
    
    self->SetTreeData (val);
}

#endif
#if ENABLE_TERRAIN && ENABLE_PHYSICS
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION TerrainCollider_Get_Custom_PropTerrainData(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainCollider_Get_Custom_PropTerrainData)
    ReadOnlyScriptingObjectOfType<TerrainCollider> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_terrainData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_terrainData)
    return Scripting::ScriptingWrapperFor(self->GetTerrainData());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION TerrainCollider_Set_Custom_PropTerrainData(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(TerrainCollider_Set_Custom_PropTerrainData)
    ReadOnlyScriptingObjectOfType<TerrainCollider> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<TerrainData> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_terrainData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_terrainData)
    
    self->SetTerrainData (val);
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_TERRAIN
void Register_UnityEngine_TerrainRenderer_INTERNAL_CALL_TerrainRenderer()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::INTERNAL_CALL_TerrainRenderer" , (gpointer)& TerrainRenderer_CUSTOM_INTERNAL_CALL_TerrainRenderer );
}

void Register_UnityEngine_TerrainRenderer_Dispose()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::Dispose" , (gpointer)& TerrainRenderer_CUSTOM_Dispose );
}

void Register_UnityEngine_TerrainRenderer_ReloadPrecomputedError()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::ReloadPrecomputedError" , (gpointer)& TerrainRenderer_CUSTOM_ReloadPrecomputedError );
}

void Register_UnityEngine_TerrainRenderer_ReloadBounds()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::ReloadBounds" , (gpointer)& TerrainRenderer_CUSTOM_ReloadBounds );
}

void Register_UnityEngine_TerrainRenderer_ReloadAll()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::ReloadAll" , (gpointer)& TerrainRenderer_CUSTOM_ReloadAll );
}

void Register_UnityEngine_TerrainRenderer_Internal_RenderStep1()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::Internal_RenderStep1" , (gpointer)& TerrainRenderer_CUSTOM_Internal_RenderStep1 );
}

void Register_UnityEngine_TerrainRenderer_RenderStep2()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::RenderStep2" , (gpointer)& TerrainRenderer_CUSTOM_RenderStep2 );
}

void Register_UnityEngine_TerrainRenderer_RenderStep3()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::RenderStep3" , (gpointer)& TerrainRenderer_CUSTOM_RenderStep3 );
}

void Register_UnityEngine_TerrainRenderer_SetNeighbors()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::SetNeighbors" , (gpointer)& TerrainRenderer_CUSTOM_SetNeighbors );
}

void Register_UnityEngine_TerrainRenderer_get_terrainData()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::get_terrainData" , (gpointer)& TerrainRenderer_Get_Custom_PropTerrainData );
}

void Register_UnityEngine_TerrainRenderer_set_terrainData()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::set_terrainData" , (gpointer)& TerrainRenderer_Set_Custom_PropTerrainData );
}

void Register_UnityEngine_TerrainRenderer_get_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::get_lightmapIndex" , (gpointer)& TerrainRenderer_Get_Custom_PropLightmapIndex );
}

void Register_UnityEngine_TerrainRenderer_set_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::set_lightmapIndex" , (gpointer)& TerrainRenderer_Set_Custom_PropLightmapIndex );
}

void Register_UnityEngine_TerrainRenderer_SetLightmapSize()
{
    scripting_add_internal_call( "UnityEngine.TerrainRenderer::SetLightmapSize" , (gpointer)& TerrainRenderer_CUSTOM_SetLightmapSize );
}

void Register_UnityEngine_DetailRenderer_INTERNAL_CALL_DetailRenderer()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::INTERNAL_CALL_DetailRenderer" , (gpointer)& DetailRenderer_CUSTOM_INTERNAL_CALL_DetailRenderer );
}

void Register_UnityEngine_DetailRenderer_Dispose()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::Dispose" , (gpointer)& DetailRenderer_CUSTOM_Dispose );
}

void Register_UnityEngine_DetailRenderer_Render()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::Render" , (gpointer)& DetailRenderer_CUSTOM_Render );
}

void Register_UnityEngine_DetailRenderer_get_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::get_lightmapIndex" , (gpointer)& DetailRenderer_Get_Custom_PropLightmapIndex );
}

void Register_UnityEngine_DetailRenderer_set_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::set_lightmapIndex" , (gpointer)& DetailRenderer_Set_Custom_PropLightmapIndex );
}

void Register_UnityEngine_DetailRenderer_ReloadAllDetails()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::ReloadAllDetails" , (gpointer)& DetailRenderer_CUSTOM_ReloadAllDetails );
}

void Register_UnityEngine_DetailRenderer_ReloadDirtyDetails()
{
    scripting_add_internal_call( "UnityEngine.DetailRenderer::ReloadDirtyDetails" , (gpointer)& DetailRenderer_CUSTOM_ReloadDirtyDetails );
}

void Register_UnityEngine_TerrainData_Internal_Create()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::Internal_Create" , (gpointer)& TerrainData_CUSTOM_Internal_Create );
}

void Register_UnityEngine_TerrainData_HasUser()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::HasUser" , (gpointer)& TerrainData_CUSTOM_HasUser );
}

void Register_UnityEngine_TerrainData_AddUser()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::AddUser" , (gpointer)& TerrainData_CUSTOM_AddUser );
}

void Register_UnityEngine_TerrainData_RemoveUser()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::RemoveUser" , (gpointer)& TerrainData_CUSTOM_RemoveUser );
}

void Register_UnityEngine_TerrainData_get_heightmapWidth()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_heightmapWidth" , (gpointer)& TerrainData_Get_Custom_PropHeightmapWidth );
}

void Register_UnityEngine_TerrainData_get_heightmapHeight()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_heightmapHeight" , (gpointer)& TerrainData_Get_Custom_PropHeightmapHeight );
}

void Register_UnityEngine_TerrainData_get_heightmapResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_heightmapResolution" , (gpointer)& TerrainData_Get_Custom_PropHeightmapResolution );
}

void Register_UnityEngine_TerrainData_set_heightmapResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_heightmapResolution" , (gpointer)& TerrainData_Set_Custom_PropHeightmapResolution );
}

void Register_UnityEngine_TerrainData_INTERNAL_get_heightmapScale()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_get_heightmapScale" , (gpointer)& TerrainData_CUSTOM_INTERNAL_get_heightmapScale );
}

void Register_UnityEngine_TerrainData_INTERNAL_get_size()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_get_size" , (gpointer)& TerrainData_CUSTOM_INTERNAL_get_size );
}

void Register_UnityEngine_TerrainData_INTERNAL_set_size()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_set_size" , (gpointer)& TerrainData_CUSTOM_INTERNAL_set_size );
}

void Register_UnityEngine_TerrainData_GetHeight()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetHeight" , (gpointer)& TerrainData_CUSTOM_GetHeight );
}

void Register_UnityEngine_TerrainData_GetInterpolatedHeight()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetInterpolatedHeight" , (gpointer)& TerrainData_CUSTOM_GetInterpolatedHeight );
}

void Register_UnityEngine_TerrainData_GetHeights()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetHeights" , (gpointer)& TerrainData_CUSTOM_GetHeights );
}

void Register_UnityEngine_TerrainData_Internal_SetHeights()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::Internal_SetHeights" , (gpointer)& TerrainData_CUSTOM_Internal_SetHeights );
}

void Register_UnityEngine_TerrainData_Internal_SetHeightsDelayLOD()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::Internal_SetHeightsDelayLOD" , (gpointer)& TerrainData_CUSTOM_Internal_SetHeightsDelayLOD );
}

void Register_UnityEngine_TerrainData_ComputeDelayedLod()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::ComputeDelayedLod" , (gpointer)& TerrainData_CUSTOM_ComputeDelayedLod );
}

void Register_UnityEngine_TerrainData_GetSteepness()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetSteepness" , (gpointer)& TerrainData_CUSTOM_GetSteepness );
}

void Register_UnityEngine_TerrainData_INTERNAL_CALL_GetInterpolatedNormal()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_CALL_GetInterpolatedNormal" , (gpointer)& TerrainData_CUSTOM_INTERNAL_CALL_GetInterpolatedNormal );
}

void Register_UnityEngine_TerrainData_GetAdjustedSize()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetAdjustedSize" , (gpointer)& TerrainData_CUSTOM_GetAdjustedSize );
}

void Register_UnityEngine_TerrainData_get_wavingGrassStrength()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_wavingGrassStrength" , (gpointer)& TerrainData_Get_Custom_PropWavingGrassStrength );
}

void Register_UnityEngine_TerrainData_set_wavingGrassStrength()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_wavingGrassStrength" , (gpointer)& TerrainData_Set_Custom_PropWavingGrassStrength );
}

void Register_UnityEngine_TerrainData_get_wavingGrassAmount()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_wavingGrassAmount" , (gpointer)& TerrainData_Get_Custom_PropWavingGrassAmount );
}

void Register_UnityEngine_TerrainData_set_wavingGrassAmount()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_wavingGrassAmount" , (gpointer)& TerrainData_Set_Custom_PropWavingGrassAmount );
}

void Register_UnityEngine_TerrainData_get_wavingGrassSpeed()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_wavingGrassSpeed" , (gpointer)& TerrainData_Get_Custom_PropWavingGrassSpeed );
}

void Register_UnityEngine_TerrainData_set_wavingGrassSpeed()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_wavingGrassSpeed" , (gpointer)& TerrainData_Set_Custom_PropWavingGrassSpeed );
}

void Register_UnityEngine_TerrainData_INTERNAL_get_wavingGrassTint()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_get_wavingGrassTint" , (gpointer)& TerrainData_CUSTOM_INTERNAL_get_wavingGrassTint );
}

void Register_UnityEngine_TerrainData_INTERNAL_set_wavingGrassTint()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_set_wavingGrassTint" , (gpointer)& TerrainData_CUSTOM_INTERNAL_set_wavingGrassTint );
}

void Register_UnityEngine_TerrainData_get_detailWidth()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_detailWidth" , (gpointer)& TerrainData_Get_Custom_PropDetailWidth );
}

void Register_UnityEngine_TerrainData_get_detailHeight()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_detailHeight" , (gpointer)& TerrainData_Get_Custom_PropDetailHeight );
}

void Register_UnityEngine_TerrainData_SetDetailResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::SetDetailResolution" , (gpointer)& TerrainData_CUSTOM_SetDetailResolution );
}

void Register_UnityEngine_TerrainData_get_detailResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_detailResolution" , (gpointer)& TerrainData_Get_Custom_PropDetailResolution );
}

void Register_UnityEngine_TerrainData_get_detailResolutionPerPatch()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_detailResolutionPerPatch" , (gpointer)& TerrainData_Get_Custom_PropDetailResolutionPerPatch );
}

void Register_UnityEngine_TerrainData_ResetDirtyDetails()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::ResetDirtyDetails" , (gpointer)& TerrainData_CUSTOM_ResetDirtyDetails );
}

void Register_UnityEngine_TerrainData_RefreshPrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::RefreshPrototypes" , (gpointer)& TerrainData_CUSTOM_RefreshPrototypes );
}

void Register_UnityEngine_TerrainData_get_detailPrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_detailPrototypes" , (gpointer)& TerrainData_Get_Custom_PropDetailPrototypes );
}

void Register_UnityEngine_TerrainData_set_detailPrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_detailPrototypes" , (gpointer)& TerrainData_Set_Custom_PropDetailPrototypes );
}

void Register_UnityEngine_TerrainData_GetSupportedLayers()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetSupportedLayers" , (gpointer)& TerrainData_CUSTOM_GetSupportedLayers );
}

void Register_UnityEngine_TerrainData_GetDetailLayer()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetDetailLayer" , (gpointer)& TerrainData_CUSTOM_GetDetailLayer );
}

void Register_UnityEngine_TerrainData_Internal_SetDetailLayer()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::Internal_SetDetailLayer" , (gpointer)& TerrainData_CUSTOM_Internal_SetDetailLayer );
}

void Register_UnityEngine_TerrainData_get_treeInstances()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_treeInstances" , (gpointer)& TerrainData_Get_Custom_PropTreeInstances );
}

void Register_UnityEngine_TerrainData_set_treeInstances()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_treeInstances" , (gpointer)& TerrainData_Set_Custom_PropTreeInstances );
}

void Register_UnityEngine_TerrainData_get_treePrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_treePrototypes" , (gpointer)& TerrainData_Get_Custom_PropTreePrototypes );
}

void Register_UnityEngine_TerrainData_set_treePrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_treePrototypes" , (gpointer)& TerrainData_Set_Custom_PropTreePrototypes );
}

void Register_UnityEngine_TerrainData_RemoveTreePrototype()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::RemoveTreePrototype" , (gpointer)& TerrainData_CUSTOM_RemoveTreePrototype );
}

void Register_UnityEngine_TerrainData_RecalculateTreePositions()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::RecalculateTreePositions" , (gpointer)& TerrainData_CUSTOM_RecalculateTreePositions );
}

void Register_UnityEngine_TerrainData_RemoveDetailPrototype()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::RemoveDetailPrototype" , (gpointer)& TerrainData_CUSTOM_RemoveDetailPrototype );
}

void Register_UnityEngine_TerrainData_get_alphamapLayers()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_alphamapLayers" , (gpointer)& TerrainData_Get_Custom_PropAlphamapLayers );
}

void Register_UnityEngine_TerrainData_GetAlphamaps()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetAlphamaps" , (gpointer)& TerrainData_CUSTOM_GetAlphamaps );
}

void Register_UnityEngine_TerrainData_get_alphamapResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_alphamapResolution" , (gpointer)& TerrainData_Get_Custom_PropAlphamapResolution );
}

void Register_UnityEngine_TerrainData_set_alphamapResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_alphamapResolution" , (gpointer)& TerrainData_Set_Custom_PropAlphamapResolution );
}

void Register_UnityEngine_TerrainData_get_alphamapWidth()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_alphamapWidth" , (gpointer)& TerrainData_Get_Custom_PropAlphamapWidth );
}

void Register_UnityEngine_TerrainData_get_alphamapHeight()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_alphamapHeight" , (gpointer)& TerrainData_Get_Custom_PropAlphamapHeight );
}

void Register_UnityEngine_TerrainData_get_baseMapResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_baseMapResolution" , (gpointer)& TerrainData_Get_Custom_PropBaseMapResolution );
}

void Register_UnityEngine_TerrainData_set_baseMapResolution()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_baseMapResolution" , (gpointer)& TerrainData_Set_Custom_PropBaseMapResolution );
}

void Register_UnityEngine_TerrainData_Internal_SetAlphamaps()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::Internal_SetAlphamaps" , (gpointer)& TerrainData_CUSTOM_Internal_SetAlphamaps );
}

void Register_UnityEngine_TerrainData_RecalculateBasemapIfDirty()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::RecalculateBasemapIfDirty" , (gpointer)& TerrainData_CUSTOM_RecalculateBasemapIfDirty );
}

void Register_UnityEngine_TerrainData_SetBasemapDirty()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::SetBasemapDirty" , (gpointer)& TerrainData_CUSTOM_SetBasemapDirty );
}

void Register_UnityEngine_TerrainData_GetAlphamapTexture()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::GetAlphamapTexture" , (gpointer)& TerrainData_CUSTOM_GetAlphamapTexture );
}

void Register_UnityEngine_TerrainData_get_alphamapTextureCount()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_alphamapTextureCount" , (gpointer)& TerrainData_Get_Custom_PropAlphamapTextureCount );
}

void Register_UnityEngine_TerrainData_get_splatPrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::get_splatPrototypes" , (gpointer)& TerrainData_Get_Custom_PropSplatPrototypes );
}

void Register_UnityEngine_TerrainData_set_splatPrototypes()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::set_splatPrototypes" , (gpointer)& TerrainData_Set_Custom_PropSplatPrototypes );
}

void Register_UnityEngine_TerrainData_HasTreeInstances()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::HasTreeInstances" , (gpointer)& TerrainData_CUSTOM_HasTreeInstances );
}

void Register_UnityEngine_TerrainData_AddTree()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::AddTree" , (gpointer)& TerrainData_CUSTOM_AddTree );
}

void Register_UnityEngine_TerrainData_INTERNAL_CALL_RemoveTrees()
{
    scripting_add_internal_call( "UnityEngine.TerrainData::INTERNAL_CALL_RemoveTrees" , (gpointer)& TerrainData_CUSTOM_INTERNAL_CALL_RemoveTrees );
}

void Register_UnityEngine_TreeRenderer_INTERNAL_CALL_TreeRenderer()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::INTERNAL_CALL_TreeRenderer" , (gpointer)& TreeRenderer_CUSTOM_INTERNAL_CALL_TreeRenderer );
}

void Register_UnityEngine_TreeRenderer_ReloadTrees()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::ReloadTrees" , (gpointer)& TreeRenderer_CUSTOM_ReloadTrees );
}

void Register_UnityEngine_TreeRenderer_Render()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::Render" , (gpointer)& TreeRenderer_CUSTOM_Render );
}

void Register_UnityEngine_TreeRenderer_RenderShadowCasters()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::RenderShadowCasters" , (gpointer)& TreeRenderer_CUSTOM_RenderShadowCasters );
}

void Register_UnityEngine_TreeRenderer_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::Cleanup" , (gpointer)& TreeRenderer_CUSTOM_Cleanup );
}

void Register_UnityEngine_TreeRenderer_InjectTree()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::InjectTree" , (gpointer)& TreeRenderer_CUSTOM_InjectTree );
}

void Register_UnityEngine_TreeRenderer_INTERNAL_CALL_RemoveTrees()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::INTERNAL_CALL_RemoveTrees" , (gpointer)& TreeRenderer_CUSTOM_INTERNAL_CALL_RemoveTrees );
}

void Register_UnityEngine_TreeRenderer_InvalidateImposters()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::InvalidateImposters" , (gpointer)& TreeRenderer_CUSTOM_InvalidateImposters );
}

void Register_UnityEngine_TreeRenderer_get_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::get_lightmapIndex" , (gpointer)& TreeRenderer_Get_Custom_PropLightmapIndex );
}

void Register_UnityEngine_TreeRenderer_set_lightmapIndex()
{
    scripting_add_internal_call( "UnityEngine.TreeRenderer::set_lightmapIndex" , (gpointer)& TreeRenderer_Set_Custom_PropLightmapIndex );
}

void Register_UnityEngine_WindZone_get_mode()
{
    scripting_add_internal_call( "UnityEngine.WindZone::get_mode" , (gpointer)& WindZone_Get_Custom_PropMode );
}

void Register_UnityEngine_WindZone_set_mode()
{
    scripting_add_internal_call( "UnityEngine.WindZone::set_mode" , (gpointer)& WindZone_Set_Custom_PropMode );
}

void Register_UnityEngine_WindZone_get_radius()
{
    scripting_add_internal_call( "UnityEngine.WindZone::get_radius" , (gpointer)& WindZone_Get_Custom_PropRadius );
}

void Register_UnityEngine_WindZone_set_radius()
{
    scripting_add_internal_call( "UnityEngine.WindZone::set_radius" , (gpointer)& WindZone_Set_Custom_PropRadius );
}

void Register_UnityEngine_WindZone_get_windMain()
{
    scripting_add_internal_call( "UnityEngine.WindZone::get_windMain" , (gpointer)& WindZone_Get_Custom_PropWindMain );
}

void Register_UnityEngine_WindZone_set_windMain()
{
    scripting_add_internal_call( "UnityEngine.WindZone::set_windMain" , (gpointer)& WindZone_Set_Custom_PropWindMain );
}

void Register_UnityEngine_WindZone_get_windTurbulence()
{
    scripting_add_internal_call( "UnityEngine.WindZone::get_windTurbulence" , (gpointer)& WindZone_Get_Custom_PropWindTurbulence );
}

void Register_UnityEngine_WindZone_set_windTurbulence()
{
    scripting_add_internal_call( "UnityEngine.WindZone::set_windTurbulence" , (gpointer)& WindZone_Set_Custom_PropWindTurbulence );
}

void Register_UnityEngine_WindZone_get_windPulseMagnitude()
{
    scripting_add_internal_call( "UnityEngine.WindZone::get_windPulseMagnitude" , (gpointer)& WindZone_Get_Custom_PropWindPulseMagnitude );
}

void Register_UnityEngine_WindZone_set_windPulseMagnitude()
{
    scripting_add_internal_call( "UnityEngine.WindZone::set_windPulseMagnitude" , (gpointer)& WindZone_Set_Custom_PropWindPulseMagnitude );
}

void Register_UnityEngine_WindZone_get_windPulseFrequency()
{
    scripting_add_internal_call( "UnityEngine.WindZone::get_windPulseFrequency" , (gpointer)& WindZone_Get_Custom_PropWindPulseFrequency );
}

void Register_UnityEngine_WindZone_set_windPulseFrequency()
{
    scripting_add_internal_call( "UnityEngine.WindZone::set_windPulseFrequency" , (gpointer)& WindZone_Set_Custom_PropWindPulseFrequency );
}

void Register_UnityEngine_Tree_get_data()
{
    scripting_add_internal_call( "UnityEngine.Tree::get_data" , (gpointer)& Tree_Get_Custom_PropData );
}

void Register_UnityEngine_Tree_set_data()
{
    scripting_add_internal_call( "UnityEngine.Tree::set_data" , (gpointer)& Tree_Set_Custom_PropData );
}

#endif
#if ENABLE_TERRAIN && ENABLE_PHYSICS
void Register_UnityEngine_TerrainCollider_get_terrainData()
{
    scripting_add_internal_call( "UnityEngine.TerrainCollider::get_terrainData" , (gpointer)& TerrainCollider_Get_Custom_PropTerrainData );
}

void Register_UnityEngine_TerrainCollider_set_terrainData()
{
    scripting_add_internal_call( "UnityEngine.TerrainCollider::set_terrainData" , (gpointer)& TerrainCollider_Set_Custom_PropTerrainData );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Terrains_IcallNames [] =
{
#if ENABLE_TERRAIN
    "UnityEngine.TerrainRenderer::INTERNAL_CALL_TerrainRenderer",    // -> TerrainRenderer_CUSTOM_INTERNAL_CALL_TerrainRenderer
    "UnityEngine.TerrainRenderer::Dispose"  ,    // -> TerrainRenderer_CUSTOM_Dispose
    "UnityEngine.TerrainRenderer::ReloadPrecomputedError",    // -> TerrainRenderer_CUSTOM_ReloadPrecomputedError
    "UnityEngine.TerrainRenderer::ReloadBounds",    // -> TerrainRenderer_CUSTOM_ReloadBounds
    "UnityEngine.TerrainRenderer::ReloadAll",    // -> TerrainRenderer_CUSTOM_ReloadAll
    "UnityEngine.TerrainRenderer::Internal_RenderStep1",    // -> TerrainRenderer_CUSTOM_Internal_RenderStep1
    "UnityEngine.TerrainRenderer::RenderStep2",    // -> TerrainRenderer_CUSTOM_RenderStep2
    "UnityEngine.TerrainRenderer::RenderStep3",    // -> TerrainRenderer_CUSTOM_RenderStep3
    "UnityEngine.TerrainRenderer::SetNeighbors",    // -> TerrainRenderer_CUSTOM_SetNeighbors
    "UnityEngine.TerrainRenderer::get_terrainData",    // -> TerrainRenderer_Get_Custom_PropTerrainData
    "UnityEngine.TerrainRenderer::set_terrainData",    // -> TerrainRenderer_Set_Custom_PropTerrainData
    "UnityEngine.TerrainRenderer::get_lightmapIndex",    // -> TerrainRenderer_Get_Custom_PropLightmapIndex
    "UnityEngine.TerrainRenderer::set_lightmapIndex",    // -> TerrainRenderer_Set_Custom_PropLightmapIndex
    "UnityEngine.TerrainRenderer::SetLightmapSize",    // -> TerrainRenderer_CUSTOM_SetLightmapSize
    "UnityEngine.DetailRenderer::INTERNAL_CALL_DetailRenderer",    // -> DetailRenderer_CUSTOM_INTERNAL_CALL_DetailRenderer
    "UnityEngine.DetailRenderer::Dispose"   ,    // -> DetailRenderer_CUSTOM_Dispose
    "UnityEngine.DetailRenderer::Render"    ,    // -> DetailRenderer_CUSTOM_Render
    "UnityEngine.DetailRenderer::get_lightmapIndex",    // -> DetailRenderer_Get_Custom_PropLightmapIndex
    "UnityEngine.DetailRenderer::set_lightmapIndex",    // -> DetailRenderer_Set_Custom_PropLightmapIndex
    "UnityEngine.DetailRenderer::ReloadAllDetails",    // -> DetailRenderer_CUSTOM_ReloadAllDetails
    "UnityEngine.DetailRenderer::ReloadDirtyDetails",    // -> DetailRenderer_CUSTOM_ReloadDirtyDetails
    "UnityEngine.TerrainData::Internal_Create",    // -> TerrainData_CUSTOM_Internal_Create
    "UnityEngine.TerrainData::HasUser"      ,    // -> TerrainData_CUSTOM_HasUser
    "UnityEngine.TerrainData::AddUser"      ,    // -> TerrainData_CUSTOM_AddUser
    "UnityEngine.TerrainData::RemoveUser"   ,    // -> TerrainData_CUSTOM_RemoveUser
    "UnityEngine.TerrainData::get_heightmapWidth",    // -> TerrainData_Get_Custom_PropHeightmapWidth
    "UnityEngine.TerrainData::get_heightmapHeight",    // -> TerrainData_Get_Custom_PropHeightmapHeight
    "UnityEngine.TerrainData::get_heightmapResolution",    // -> TerrainData_Get_Custom_PropHeightmapResolution
    "UnityEngine.TerrainData::set_heightmapResolution",    // -> TerrainData_Set_Custom_PropHeightmapResolution
    "UnityEngine.TerrainData::INTERNAL_get_heightmapScale",    // -> TerrainData_CUSTOM_INTERNAL_get_heightmapScale
    "UnityEngine.TerrainData::INTERNAL_get_size",    // -> TerrainData_CUSTOM_INTERNAL_get_size
    "UnityEngine.TerrainData::INTERNAL_set_size",    // -> TerrainData_CUSTOM_INTERNAL_set_size
    "UnityEngine.TerrainData::GetHeight"    ,    // -> TerrainData_CUSTOM_GetHeight
    "UnityEngine.TerrainData::GetInterpolatedHeight",    // -> TerrainData_CUSTOM_GetInterpolatedHeight
    "UnityEngine.TerrainData::GetHeights"   ,    // -> TerrainData_CUSTOM_GetHeights
    "UnityEngine.TerrainData::Internal_SetHeights",    // -> TerrainData_CUSTOM_Internal_SetHeights
    "UnityEngine.TerrainData::Internal_SetHeightsDelayLOD",    // -> TerrainData_CUSTOM_Internal_SetHeightsDelayLOD
    "UnityEngine.TerrainData::ComputeDelayedLod",    // -> TerrainData_CUSTOM_ComputeDelayedLod
    "UnityEngine.TerrainData::GetSteepness" ,    // -> TerrainData_CUSTOM_GetSteepness
    "UnityEngine.TerrainData::INTERNAL_CALL_GetInterpolatedNormal",    // -> TerrainData_CUSTOM_INTERNAL_CALL_GetInterpolatedNormal
    "UnityEngine.TerrainData::GetAdjustedSize",    // -> TerrainData_CUSTOM_GetAdjustedSize
    "UnityEngine.TerrainData::get_wavingGrassStrength",    // -> TerrainData_Get_Custom_PropWavingGrassStrength
    "UnityEngine.TerrainData::set_wavingGrassStrength",    // -> TerrainData_Set_Custom_PropWavingGrassStrength
    "UnityEngine.TerrainData::get_wavingGrassAmount",    // -> TerrainData_Get_Custom_PropWavingGrassAmount
    "UnityEngine.TerrainData::set_wavingGrassAmount",    // -> TerrainData_Set_Custom_PropWavingGrassAmount
    "UnityEngine.TerrainData::get_wavingGrassSpeed",    // -> TerrainData_Get_Custom_PropWavingGrassSpeed
    "UnityEngine.TerrainData::set_wavingGrassSpeed",    // -> TerrainData_Set_Custom_PropWavingGrassSpeed
    "UnityEngine.TerrainData::INTERNAL_get_wavingGrassTint",    // -> TerrainData_CUSTOM_INTERNAL_get_wavingGrassTint
    "UnityEngine.TerrainData::INTERNAL_set_wavingGrassTint",    // -> TerrainData_CUSTOM_INTERNAL_set_wavingGrassTint
    "UnityEngine.TerrainData::get_detailWidth",    // -> TerrainData_Get_Custom_PropDetailWidth
    "UnityEngine.TerrainData::get_detailHeight",    // -> TerrainData_Get_Custom_PropDetailHeight
    "UnityEngine.TerrainData::SetDetailResolution",    // -> TerrainData_CUSTOM_SetDetailResolution
    "UnityEngine.TerrainData::get_detailResolution",    // -> TerrainData_Get_Custom_PropDetailResolution
    "UnityEngine.TerrainData::get_detailResolutionPerPatch",    // -> TerrainData_Get_Custom_PropDetailResolutionPerPatch
    "UnityEngine.TerrainData::ResetDirtyDetails",    // -> TerrainData_CUSTOM_ResetDirtyDetails
    "UnityEngine.TerrainData::RefreshPrototypes",    // -> TerrainData_CUSTOM_RefreshPrototypes
    "UnityEngine.TerrainData::get_detailPrototypes",    // -> TerrainData_Get_Custom_PropDetailPrototypes
    "UnityEngine.TerrainData::set_detailPrototypes",    // -> TerrainData_Set_Custom_PropDetailPrototypes
    "UnityEngine.TerrainData::GetSupportedLayers",    // -> TerrainData_CUSTOM_GetSupportedLayers
    "UnityEngine.TerrainData::GetDetailLayer",    // -> TerrainData_CUSTOM_GetDetailLayer
    "UnityEngine.TerrainData::Internal_SetDetailLayer",    // -> TerrainData_CUSTOM_Internal_SetDetailLayer
    "UnityEngine.TerrainData::get_treeInstances",    // -> TerrainData_Get_Custom_PropTreeInstances
    "UnityEngine.TerrainData::set_treeInstances",    // -> TerrainData_Set_Custom_PropTreeInstances
    "UnityEngine.TerrainData::get_treePrototypes",    // -> TerrainData_Get_Custom_PropTreePrototypes
    "UnityEngine.TerrainData::set_treePrototypes",    // -> TerrainData_Set_Custom_PropTreePrototypes
    "UnityEngine.TerrainData::RemoveTreePrototype",    // -> TerrainData_CUSTOM_RemoveTreePrototype
    "UnityEngine.TerrainData::RecalculateTreePositions",    // -> TerrainData_CUSTOM_RecalculateTreePositions
    "UnityEngine.TerrainData::RemoveDetailPrototype",    // -> TerrainData_CUSTOM_RemoveDetailPrototype
    "UnityEngine.TerrainData::get_alphamapLayers",    // -> TerrainData_Get_Custom_PropAlphamapLayers
    "UnityEngine.TerrainData::GetAlphamaps" ,    // -> TerrainData_CUSTOM_GetAlphamaps
    "UnityEngine.TerrainData::get_alphamapResolution",    // -> TerrainData_Get_Custom_PropAlphamapResolution
    "UnityEngine.TerrainData::set_alphamapResolution",    // -> TerrainData_Set_Custom_PropAlphamapResolution
    "UnityEngine.TerrainData::get_alphamapWidth",    // -> TerrainData_Get_Custom_PropAlphamapWidth
    "UnityEngine.TerrainData::get_alphamapHeight",    // -> TerrainData_Get_Custom_PropAlphamapHeight
    "UnityEngine.TerrainData::get_baseMapResolution",    // -> TerrainData_Get_Custom_PropBaseMapResolution
    "UnityEngine.TerrainData::set_baseMapResolution",    // -> TerrainData_Set_Custom_PropBaseMapResolution
    "UnityEngine.TerrainData::Internal_SetAlphamaps",    // -> TerrainData_CUSTOM_Internal_SetAlphamaps
    "UnityEngine.TerrainData::RecalculateBasemapIfDirty",    // -> TerrainData_CUSTOM_RecalculateBasemapIfDirty
    "UnityEngine.TerrainData::SetBasemapDirty",    // -> TerrainData_CUSTOM_SetBasemapDirty
    "UnityEngine.TerrainData::GetAlphamapTexture",    // -> TerrainData_CUSTOM_GetAlphamapTexture
    "UnityEngine.TerrainData::get_alphamapTextureCount",    // -> TerrainData_Get_Custom_PropAlphamapTextureCount
    "UnityEngine.TerrainData::get_splatPrototypes",    // -> TerrainData_Get_Custom_PropSplatPrototypes
    "UnityEngine.TerrainData::set_splatPrototypes",    // -> TerrainData_Set_Custom_PropSplatPrototypes
    "UnityEngine.TerrainData::HasTreeInstances",    // -> TerrainData_CUSTOM_HasTreeInstances
    "UnityEngine.TerrainData::AddTree"      ,    // -> TerrainData_CUSTOM_AddTree
    "UnityEngine.TerrainData::INTERNAL_CALL_RemoveTrees",    // -> TerrainData_CUSTOM_INTERNAL_CALL_RemoveTrees
    "UnityEngine.TreeRenderer::INTERNAL_CALL_TreeRenderer",    // -> TreeRenderer_CUSTOM_INTERNAL_CALL_TreeRenderer
    "UnityEngine.TreeRenderer::ReloadTrees" ,    // -> TreeRenderer_CUSTOM_ReloadTrees
    "UnityEngine.TreeRenderer::Render"      ,    // -> TreeRenderer_CUSTOM_Render
    "UnityEngine.TreeRenderer::RenderShadowCasters",    // -> TreeRenderer_CUSTOM_RenderShadowCasters
    "UnityEngine.TreeRenderer::Cleanup"     ,    // -> TreeRenderer_CUSTOM_Cleanup
    "UnityEngine.TreeRenderer::InjectTree"  ,    // -> TreeRenderer_CUSTOM_InjectTree
    "UnityEngine.TreeRenderer::INTERNAL_CALL_RemoveTrees",    // -> TreeRenderer_CUSTOM_INTERNAL_CALL_RemoveTrees
    "UnityEngine.TreeRenderer::InvalidateImposters",    // -> TreeRenderer_CUSTOM_InvalidateImposters
    "UnityEngine.TreeRenderer::get_lightmapIndex",    // -> TreeRenderer_Get_Custom_PropLightmapIndex
    "UnityEngine.TreeRenderer::set_lightmapIndex",    // -> TreeRenderer_Set_Custom_PropLightmapIndex
    "UnityEngine.WindZone::get_mode"        ,    // -> WindZone_Get_Custom_PropMode
    "UnityEngine.WindZone::set_mode"        ,    // -> WindZone_Set_Custom_PropMode
    "UnityEngine.WindZone::get_radius"      ,    // -> WindZone_Get_Custom_PropRadius
    "UnityEngine.WindZone::set_radius"      ,    // -> WindZone_Set_Custom_PropRadius
    "UnityEngine.WindZone::get_windMain"    ,    // -> WindZone_Get_Custom_PropWindMain
    "UnityEngine.WindZone::set_windMain"    ,    // -> WindZone_Set_Custom_PropWindMain
    "UnityEngine.WindZone::get_windTurbulence",    // -> WindZone_Get_Custom_PropWindTurbulence
    "UnityEngine.WindZone::set_windTurbulence",    // -> WindZone_Set_Custom_PropWindTurbulence
    "UnityEngine.WindZone::get_windPulseMagnitude",    // -> WindZone_Get_Custom_PropWindPulseMagnitude
    "UnityEngine.WindZone::set_windPulseMagnitude",    // -> WindZone_Set_Custom_PropWindPulseMagnitude
    "UnityEngine.WindZone::get_windPulseFrequency",    // -> WindZone_Get_Custom_PropWindPulseFrequency
    "UnityEngine.WindZone::set_windPulseFrequency",    // -> WindZone_Set_Custom_PropWindPulseFrequency
    "UnityEngine.Tree::get_data"            ,    // -> Tree_Get_Custom_PropData
    "UnityEngine.Tree::set_data"            ,    // -> Tree_Set_Custom_PropData
#endif
#if ENABLE_TERRAIN && ENABLE_PHYSICS
    "UnityEngine.TerrainCollider::get_terrainData",    // -> TerrainCollider_Get_Custom_PropTerrainData
    "UnityEngine.TerrainCollider::set_terrainData",    // -> TerrainCollider_Set_Custom_PropTerrainData
#endif
    NULL
};

static const void* s_Terrains_IcallFuncs [] =
{
#if ENABLE_TERRAIN
    (const void*)&TerrainRenderer_CUSTOM_INTERNAL_CALL_TerrainRenderer,  //  <- UnityEngine.TerrainRenderer::INTERNAL_CALL_TerrainRenderer
    (const void*)&TerrainRenderer_CUSTOM_Dispose          ,  //  <- UnityEngine.TerrainRenderer::Dispose
    (const void*)&TerrainRenderer_CUSTOM_ReloadPrecomputedError,  //  <- UnityEngine.TerrainRenderer::ReloadPrecomputedError
    (const void*)&TerrainRenderer_CUSTOM_ReloadBounds     ,  //  <- UnityEngine.TerrainRenderer::ReloadBounds
    (const void*)&TerrainRenderer_CUSTOM_ReloadAll        ,  //  <- UnityEngine.TerrainRenderer::ReloadAll
    (const void*)&TerrainRenderer_CUSTOM_Internal_RenderStep1,  //  <- UnityEngine.TerrainRenderer::Internal_RenderStep1
    (const void*)&TerrainRenderer_CUSTOM_RenderStep2      ,  //  <- UnityEngine.TerrainRenderer::RenderStep2
    (const void*)&TerrainRenderer_CUSTOM_RenderStep3      ,  //  <- UnityEngine.TerrainRenderer::RenderStep3
    (const void*)&TerrainRenderer_CUSTOM_SetNeighbors     ,  //  <- UnityEngine.TerrainRenderer::SetNeighbors
    (const void*)&TerrainRenderer_Get_Custom_PropTerrainData,  //  <- UnityEngine.TerrainRenderer::get_terrainData
    (const void*)&TerrainRenderer_Set_Custom_PropTerrainData,  //  <- UnityEngine.TerrainRenderer::set_terrainData
    (const void*)&TerrainRenderer_Get_Custom_PropLightmapIndex,  //  <- UnityEngine.TerrainRenderer::get_lightmapIndex
    (const void*)&TerrainRenderer_Set_Custom_PropLightmapIndex,  //  <- UnityEngine.TerrainRenderer::set_lightmapIndex
    (const void*)&TerrainRenderer_CUSTOM_SetLightmapSize  ,  //  <- UnityEngine.TerrainRenderer::SetLightmapSize
    (const void*)&DetailRenderer_CUSTOM_INTERNAL_CALL_DetailRenderer,  //  <- UnityEngine.DetailRenderer::INTERNAL_CALL_DetailRenderer
    (const void*)&DetailRenderer_CUSTOM_Dispose           ,  //  <- UnityEngine.DetailRenderer::Dispose
    (const void*)&DetailRenderer_CUSTOM_Render            ,  //  <- UnityEngine.DetailRenderer::Render
    (const void*)&DetailRenderer_Get_Custom_PropLightmapIndex,  //  <- UnityEngine.DetailRenderer::get_lightmapIndex
    (const void*)&DetailRenderer_Set_Custom_PropLightmapIndex,  //  <- UnityEngine.DetailRenderer::set_lightmapIndex
    (const void*)&DetailRenderer_CUSTOM_ReloadAllDetails  ,  //  <- UnityEngine.DetailRenderer::ReloadAllDetails
    (const void*)&DetailRenderer_CUSTOM_ReloadDirtyDetails,  //  <- UnityEngine.DetailRenderer::ReloadDirtyDetails
    (const void*)&TerrainData_CUSTOM_Internal_Create      ,  //  <- UnityEngine.TerrainData::Internal_Create
    (const void*)&TerrainData_CUSTOM_HasUser              ,  //  <- UnityEngine.TerrainData::HasUser
    (const void*)&TerrainData_CUSTOM_AddUser              ,  //  <- UnityEngine.TerrainData::AddUser
    (const void*)&TerrainData_CUSTOM_RemoveUser           ,  //  <- UnityEngine.TerrainData::RemoveUser
    (const void*)&TerrainData_Get_Custom_PropHeightmapWidth,  //  <- UnityEngine.TerrainData::get_heightmapWidth
    (const void*)&TerrainData_Get_Custom_PropHeightmapHeight,  //  <- UnityEngine.TerrainData::get_heightmapHeight
    (const void*)&TerrainData_Get_Custom_PropHeightmapResolution,  //  <- UnityEngine.TerrainData::get_heightmapResolution
    (const void*)&TerrainData_Set_Custom_PropHeightmapResolution,  //  <- UnityEngine.TerrainData::set_heightmapResolution
    (const void*)&TerrainData_CUSTOM_INTERNAL_get_heightmapScale,  //  <- UnityEngine.TerrainData::INTERNAL_get_heightmapScale
    (const void*)&TerrainData_CUSTOM_INTERNAL_get_size    ,  //  <- UnityEngine.TerrainData::INTERNAL_get_size
    (const void*)&TerrainData_CUSTOM_INTERNAL_set_size    ,  //  <- UnityEngine.TerrainData::INTERNAL_set_size
    (const void*)&TerrainData_CUSTOM_GetHeight            ,  //  <- UnityEngine.TerrainData::GetHeight
    (const void*)&TerrainData_CUSTOM_GetInterpolatedHeight,  //  <- UnityEngine.TerrainData::GetInterpolatedHeight
    (const void*)&TerrainData_CUSTOM_GetHeights           ,  //  <- UnityEngine.TerrainData::GetHeights
    (const void*)&TerrainData_CUSTOM_Internal_SetHeights  ,  //  <- UnityEngine.TerrainData::Internal_SetHeights
    (const void*)&TerrainData_CUSTOM_Internal_SetHeightsDelayLOD,  //  <- UnityEngine.TerrainData::Internal_SetHeightsDelayLOD
    (const void*)&TerrainData_CUSTOM_ComputeDelayedLod    ,  //  <- UnityEngine.TerrainData::ComputeDelayedLod
    (const void*)&TerrainData_CUSTOM_GetSteepness         ,  //  <- UnityEngine.TerrainData::GetSteepness
    (const void*)&TerrainData_CUSTOM_INTERNAL_CALL_GetInterpolatedNormal,  //  <- UnityEngine.TerrainData::INTERNAL_CALL_GetInterpolatedNormal
    (const void*)&TerrainData_CUSTOM_GetAdjustedSize      ,  //  <- UnityEngine.TerrainData::GetAdjustedSize
    (const void*)&TerrainData_Get_Custom_PropWavingGrassStrength,  //  <- UnityEngine.TerrainData::get_wavingGrassStrength
    (const void*)&TerrainData_Set_Custom_PropWavingGrassStrength,  //  <- UnityEngine.TerrainData::set_wavingGrassStrength
    (const void*)&TerrainData_Get_Custom_PropWavingGrassAmount,  //  <- UnityEngine.TerrainData::get_wavingGrassAmount
    (const void*)&TerrainData_Set_Custom_PropWavingGrassAmount,  //  <- UnityEngine.TerrainData::set_wavingGrassAmount
    (const void*)&TerrainData_Get_Custom_PropWavingGrassSpeed,  //  <- UnityEngine.TerrainData::get_wavingGrassSpeed
    (const void*)&TerrainData_Set_Custom_PropWavingGrassSpeed,  //  <- UnityEngine.TerrainData::set_wavingGrassSpeed
    (const void*)&TerrainData_CUSTOM_INTERNAL_get_wavingGrassTint,  //  <- UnityEngine.TerrainData::INTERNAL_get_wavingGrassTint
    (const void*)&TerrainData_CUSTOM_INTERNAL_set_wavingGrassTint,  //  <- UnityEngine.TerrainData::INTERNAL_set_wavingGrassTint
    (const void*)&TerrainData_Get_Custom_PropDetailWidth  ,  //  <- UnityEngine.TerrainData::get_detailWidth
    (const void*)&TerrainData_Get_Custom_PropDetailHeight ,  //  <- UnityEngine.TerrainData::get_detailHeight
    (const void*)&TerrainData_CUSTOM_SetDetailResolution  ,  //  <- UnityEngine.TerrainData::SetDetailResolution
    (const void*)&TerrainData_Get_Custom_PropDetailResolution,  //  <- UnityEngine.TerrainData::get_detailResolution
    (const void*)&TerrainData_Get_Custom_PropDetailResolutionPerPatch,  //  <- UnityEngine.TerrainData::get_detailResolutionPerPatch
    (const void*)&TerrainData_CUSTOM_ResetDirtyDetails    ,  //  <- UnityEngine.TerrainData::ResetDirtyDetails
    (const void*)&TerrainData_CUSTOM_RefreshPrototypes    ,  //  <- UnityEngine.TerrainData::RefreshPrototypes
    (const void*)&TerrainData_Get_Custom_PropDetailPrototypes,  //  <- UnityEngine.TerrainData::get_detailPrototypes
    (const void*)&TerrainData_Set_Custom_PropDetailPrototypes,  //  <- UnityEngine.TerrainData::set_detailPrototypes
    (const void*)&TerrainData_CUSTOM_GetSupportedLayers   ,  //  <- UnityEngine.TerrainData::GetSupportedLayers
    (const void*)&TerrainData_CUSTOM_GetDetailLayer       ,  //  <- UnityEngine.TerrainData::GetDetailLayer
    (const void*)&TerrainData_CUSTOM_Internal_SetDetailLayer,  //  <- UnityEngine.TerrainData::Internal_SetDetailLayer
    (const void*)&TerrainData_Get_Custom_PropTreeInstances,  //  <- UnityEngine.TerrainData::get_treeInstances
    (const void*)&TerrainData_Set_Custom_PropTreeInstances,  //  <- UnityEngine.TerrainData::set_treeInstances
    (const void*)&TerrainData_Get_Custom_PropTreePrototypes,  //  <- UnityEngine.TerrainData::get_treePrototypes
    (const void*)&TerrainData_Set_Custom_PropTreePrototypes,  //  <- UnityEngine.TerrainData::set_treePrototypes
    (const void*)&TerrainData_CUSTOM_RemoveTreePrototype  ,  //  <- UnityEngine.TerrainData::RemoveTreePrototype
    (const void*)&TerrainData_CUSTOM_RecalculateTreePositions,  //  <- UnityEngine.TerrainData::RecalculateTreePositions
    (const void*)&TerrainData_CUSTOM_RemoveDetailPrototype,  //  <- UnityEngine.TerrainData::RemoveDetailPrototype
    (const void*)&TerrainData_Get_Custom_PropAlphamapLayers,  //  <- UnityEngine.TerrainData::get_alphamapLayers
    (const void*)&TerrainData_CUSTOM_GetAlphamaps         ,  //  <- UnityEngine.TerrainData::GetAlphamaps
    (const void*)&TerrainData_Get_Custom_PropAlphamapResolution,  //  <- UnityEngine.TerrainData::get_alphamapResolution
    (const void*)&TerrainData_Set_Custom_PropAlphamapResolution,  //  <- UnityEngine.TerrainData::set_alphamapResolution
    (const void*)&TerrainData_Get_Custom_PropAlphamapWidth,  //  <- UnityEngine.TerrainData::get_alphamapWidth
    (const void*)&TerrainData_Get_Custom_PropAlphamapHeight,  //  <- UnityEngine.TerrainData::get_alphamapHeight
    (const void*)&TerrainData_Get_Custom_PropBaseMapResolution,  //  <- UnityEngine.TerrainData::get_baseMapResolution
    (const void*)&TerrainData_Set_Custom_PropBaseMapResolution,  //  <- UnityEngine.TerrainData::set_baseMapResolution
    (const void*)&TerrainData_CUSTOM_Internal_SetAlphamaps,  //  <- UnityEngine.TerrainData::Internal_SetAlphamaps
    (const void*)&TerrainData_CUSTOM_RecalculateBasemapIfDirty,  //  <- UnityEngine.TerrainData::RecalculateBasemapIfDirty
    (const void*)&TerrainData_CUSTOM_SetBasemapDirty      ,  //  <- UnityEngine.TerrainData::SetBasemapDirty
    (const void*)&TerrainData_CUSTOM_GetAlphamapTexture   ,  //  <- UnityEngine.TerrainData::GetAlphamapTexture
    (const void*)&TerrainData_Get_Custom_PropAlphamapTextureCount,  //  <- UnityEngine.TerrainData::get_alphamapTextureCount
    (const void*)&TerrainData_Get_Custom_PropSplatPrototypes,  //  <- UnityEngine.TerrainData::get_splatPrototypes
    (const void*)&TerrainData_Set_Custom_PropSplatPrototypes,  //  <- UnityEngine.TerrainData::set_splatPrototypes
    (const void*)&TerrainData_CUSTOM_HasTreeInstances     ,  //  <- UnityEngine.TerrainData::HasTreeInstances
    (const void*)&TerrainData_CUSTOM_AddTree              ,  //  <- UnityEngine.TerrainData::AddTree
    (const void*)&TerrainData_CUSTOM_INTERNAL_CALL_RemoveTrees,  //  <- UnityEngine.TerrainData::INTERNAL_CALL_RemoveTrees
    (const void*)&TreeRenderer_CUSTOM_INTERNAL_CALL_TreeRenderer,  //  <- UnityEngine.TreeRenderer::INTERNAL_CALL_TreeRenderer
    (const void*)&TreeRenderer_CUSTOM_ReloadTrees         ,  //  <- UnityEngine.TreeRenderer::ReloadTrees
    (const void*)&TreeRenderer_CUSTOM_Render              ,  //  <- UnityEngine.TreeRenderer::Render
    (const void*)&TreeRenderer_CUSTOM_RenderShadowCasters ,  //  <- UnityEngine.TreeRenderer::RenderShadowCasters
    (const void*)&TreeRenderer_CUSTOM_Cleanup             ,  //  <- UnityEngine.TreeRenderer::Cleanup
    (const void*)&TreeRenderer_CUSTOM_InjectTree          ,  //  <- UnityEngine.TreeRenderer::InjectTree
    (const void*)&TreeRenderer_CUSTOM_INTERNAL_CALL_RemoveTrees,  //  <- UnityEngine.TreeRenderer::INTERNAL_CALL_RemoveTrees
    (const void*)&TreeRenderer_CUSTOM_InvalidateImposters ,  //  <- UnityEngine.TreeRenderer::InvalidateImposters
    (const void*)&TreeRenderer_Get_Custom_PropLightmapIndex,  //  <- UnityEngine.TreeRenderer::get_lightmapIndex
    (const void*)&TreeRenderer_Set_Custom_PropLightmapIndex,  //  <- UnityEngine.TreeRenderer::set_lightmapIndex
    (const void*)&WindZone_Get_Custom_PropMode            ,  //  <- UnityEngine.WindZone::get_mode
    (const void*)&WindZone_Set_Custom_PropMode            ,  //  <- UnityEngine.WindZone::set_mode
    (const void*)&WindZone_Get_Custom_PropRadius          ,  //  <- UnityEngine.WindZone::get_radius
    (const void*)&WindZone_Set_Custom_PropRadius          ,  //  <- UnityEngine.WindZone::set_radius
    (const void*)&WindZone_Get_Custom_PropWindMain        ,  //  <- UnityEngine.WindZone::get_windMain
    (const void*)&WindZone_Set_Custom_PropWindMain        ,  //  <- UnityEngine.WindZone::set_windMain
    (const void*)&WindZone_Get_Custom_PropWindTurbulence  ,  //  <- UnityEngine.WindZone::get_windTurbulence
    (const void*)&WindZone_Set_Custom_PropWindTurbulence  ,  //  <- UnityEngine.WindZone::set_windTurbulence
    (const void*)&WindZone_Get_Custom_PropWindPulseMagnitude,  //  <- UnityEngine.WindZone::get_windPulseMagnitude
    (const void*)&WindZone_Set_Custom_PropWindPulseMagnitude,  //  <- UnityEngine.WindZone::set_windPulseMagnitude
    (const void*)&WindZone_Get_Custom_PropWindPulseFrequency,  //  <- UnityEngine.WindZone::get_windPulseFrequency
    (const void*)&WindZone_Set_Custom_PropWindPulseFrequency,  //  <- UnityEngine.WindZone::set_windPulseFrequency
    (const void*)&Tree_Get_Custom_PropData                ,  //  <- UnityEngine.Tree::get_data
    (const void*)&Tree_Set_Custom_PropData                ,  //  <- UnityEngine.Tree::set_data
#endif
#if ENABLE_TERRAIN && ENABLE_PHYSICS
    (const void*)&TerrainCollider_Get_Custom_PropTerrainData,  //  <- UnityEngine.TerrainCollider::get_terrainData
    (const void*)&TerrainCollider_Set_Custom_PropTerrainData,  //  <- UnityEngine.TerrainCollider::set_terrainData
#endif
    NULL
};

void ExportTerrainsBindings();
void ExportTerrainsBindings()
{
    for (int i = 0; s_Terrains_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Terrains_IcallNames [i], s_Terrains_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportTerrainsBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_TERRAIN
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_INTERNAL_CALL_TerrainRenderer); //  <- UnityEngine.TerrainRenderer::INTERNAL_CALL_TerrainRenderer
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_Dispose); //  <- UnityEngine.TerrainRenderer::Dispose
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_ReloadPrecomputedError); //  <- UnityEngine.TerrainRenderer::ReloadPrecomputedError
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_ReloadBounds); //  <- UnityEngine.TerrainRenderer::ReloadBounds
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_ReloadAll); //  <- UnityEngine.TerrainRenderer::ReloadAll
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_Internal_RenderStep1); //  <- UnityEngine.TerrainRenderer::Internal_RenderStep1
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_RenderStep2); //  <- UnityEngine.TerrainRenderer::RenderStep2
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_RenderStep3); //  <- UnityEngine.TerrainRenderer::RenderStep3
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_SetNeighbors); //  <- UnityEngine.TerrainRenderer::SetNeighbors
    SET_METRO_BINDING(TerrainRenderer_Get_Custom_PropTerrainData); //  <- UnityEngine.TerrainRenderer::get_terrainData
    SET_METRO_BINDING(TerrainRenderer_Set_Custom_PropTerrainData); //  <- UnityEngine.TerrainRenderer::set_terrainData
    SET_METRO_BINDING(TerrainRenderer_Get_Custom_PropLightmapIndex); //  <- UnityEngine.TerrainRenderer::get_lightmapIndex
    SET_METRO_BINDING(TerrainRenderer_Set_Custom_PropLightmapIndex); //  <- UnityEngine.TerrainRenderer::set_lightmapIndex
    SET_METRO_BINDING(TerrainRenderer_CUSTOM_SetLightmapSize); //  <- UnityEngine.TerrainRenderer::SetLightmapSize
    SET_METRO_BINDING(DetailRenderer_CUSTOM_INTERNAL_CALL_DetailRenderer); //  <- UnityEngine.DetailRenderer::INTERNAL_CALL_DetailRenderer
    SET_METRO_BINDING(DetailRenderer_CUSTOM_Dispose); //  <- UnityEngine.DetailRenderer::Dispose
    SET_METRO_BINDING(DetailRenderer_CUSTOM_Render); //  <- UnityEngine.DetailRenderer::Render
    SET_METRO_BINDING(DetailRenderer_Get_Custom_PropLightmapIndex); //  <- UnityEngine.DetailRenderer::get_lightmapIndex
    SET_METRO_BINDING(DetailRenderer_Set_Custom_PropLightmapIndex); //  <- UnityEngine.DetailRenderer::set_lightmapIndex
    SET_METRO_BINDING(DetailRenderer_CUSTOM_ReloadAllDetails); //  <- UnityEngine.DetailRenderer::ReloadAllDetails
    SET_METRO_BINDING(DetailRenderer_CUSTOM_ReloadDirtyDetails); //  <- UnityEngine.DetailRenderer::ReloadDirtyDetails
    SET_METRO_BINDING(TerrainData_CUSTOM_Internal_Create); //  <- UnityEngine.TerrainData::Internal_Create
    SET_METRO_BINDING(TerrainData_CUSTOM_HasUser); //  <- UnityEngine.TerrainData::HasUser
    SET_METRO_BINDING(TerrainData_CUSTOM_AddUser); //  <- UnityEngine.TerrainData::AddUser
    SET_METRO_BINDING(TerrainData_CUSTOM_RemoveUser); //  <- UnityEngine.TerrainData::RemoveUser
    SET_METRO_BINDING(TerrainData_Get_Custom_PropHeightmapWidth); //  <- UnityEngine.TerrainData::get_heightmapWidth
    SET_METRO_BINDING(TerrainData_Get_Custom_PropHeightmapHeight); //  <- UnityEngine.TerrainData::get_heightmapHeight
    SET_METRO_BINDING(TerrainData_Get_Custom_PropHeightmapResolution); //  <- UnityEngine.TerrainData::get_heightmapResolution
    SET_METRO_BINDING(TerrainData_Set_Custom_PropHeightmapResolution); //  <- UnityEngine.TerrainData::set_heightmapResolution
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_get_heightmapScale); //  <- UnityEngine.TerrainData::INTERNAL_get_heightmapScale
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_get_size); //  <- UnityEngine.TerrainData::INTERNAL_get_size
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_set_size); //  <- UnityEngine.TerrainData::INTERNAL_set_size
    SET_METRO_BINDING(TerrainData_CUSTOM_GetHeight); //  <- UnityEngine.TerrainData::GetHeight
    SET_METRO_BINDING(TerrainData_CUSTOM_GetInterpolatedHeight); //  <- UnityEngine.TerrainData::GetInterpolatedHeight
    SET_METRO_BINDING(TerrainData_CUSTOM_GetHeights); //  <- UnityEngine.TerrainData::GetHeights
    SET_METRO_BINDING(TerrainData_CUSTOM_Internal_SetHeights); //  <- UnityEngine.TerrainData::Internal_SetHeights
    SET_METRO_BINDING(TerrainData_CUSTOM_Internal_SetHeightsDelayLOD); //  <- UnityEngine.TerrainData::Internal_SetHeightsDelayLOD
    SET_METRO_BINDING(TerrainData_CUSTOM_ComputeDelayedLod); //  <- UnityEngine.TerrainData::ComputeDelayedLod
    SET_METRO_BINDING(TerrainData_CUSTOM_GetSteepness); //  <- UnityEngine.TerrainData::GetSteepness
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_CALL_GetInterpolatedNormal); //  <- UnityEngine.TerrainData::INTERNAL_CALL_GetInterpolatedNormal
    SET_METRO_BINDING(TerrainData_CUSTOM_GetAdjustedSize); //  <- UnityEngine.TerrainData::GetAdjustedSize
    SET_METRO_BINDING(TerrainData_Get_Custom_PropWavingGrassStrength); //  <- UnityEngine.TerrainData::get_wavingGrassStrength
    SET_METRO_BINDING(TerrainData_Set_Custom_PropWavingGrassStrength); //  <- UnityEngine.TerrainData::set_wavingGrassStrength
    SET_METRO_BINDING(TerrainData_Get_Custom_PropWavingGrassAmount); //  <- UnityEngine.TerrainData::get_wavingGrassAmount
    SET_METRO_BINDING(TerrainData_Set_Custom_PropWavingGrassAmount); //  <- UnityEngine.TerrainData::set_wavingGrassAmount
    SET_METRO_BINDING(TerrainData_Get_Custom_PropWavingGrassSpeed); //  <- UnityEngine.TerrainData::get_wavingGrassSpeed
    SET_METRO_BINDING(TerrainData_Set_Custom_PropWavingGrassSpeed); //  <- UnityEngine.TerrainData::set_wavingGrassSpeed
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_get_wavingGrassTint); //  <- UnityEngine.TerrainData::INTERNAL_get_wavingGrassTint
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_set_wavingGrassTint); //  <- UnityEngine.TerrainData::INTERNAL_set_wavingGrassTint
    SET_METRO_BINDING(TerrainData_Get_Custom_PropDetailWidth); //  <- UnityEngine.TerrainData::get_detailWidth
    SET_METRO_BINDING(TerrainData_Get_Custom_PropDetailHeight); //  <- UnityEngine.TerrainData::get_detailHeight
    SET_METRO_BINDING(TerrainData_CUSTOM_SetDetailResolution); //  <- UnityEngine.TerrainData::SetDetailResolution
    SET_METRO_BINDING(TerrainData_Get_Custom_PropDetailResolution); //  <- UnityEngine.TerrainData::get_detailResolution
    SET_METRO_BINDING(TerrainData_Get_Custom_PropDetailResolutionPerPatch); //  <- UnityEngine.TerrainData::get_detailResolutionPerPatch
    SET_METRO_BINDING(TerrainData_CUSTOM_ResetDirtyDetails); //  <- UnityEngine.TerrainData::ResetDirtyDetails
    SET_METRO_BINDING(TerrainData_CUSTOM_RefreshPrototypes); //  <- UnityEngine.TerrainData::RefreshPrototypes
    SET_METRO_BINDING(TerrainData_Get_Custom_PropDetailPrototypes); //  <- UnityEngine.TerrainData::get_detailPrototypes
    SET_METRO_BINDING(TerrainData_Set_Custom_PropDetailPrototypes); //  <- UnityEngine.TerrainData::set_detailPrototypes
    SET_METRO_BINDING(TerrainData_CUSTOM_GetSupportedLayers); //  <- UnityEngine.TerrainData::GetSupportedLayers
    SET_METRO_BINDING(TerrainData_CUSTOM_GetDetailLayer); //  <- UnityEngine.TerrainData::GetDetailLayer
    SET_METRO_BINDING(TerrainData_CUSTOM_Internal_SetDetailLayer); //  <- UnityEngine.TerrainData::Internal_SetDetailLayer
    SET_METRO_BINDING(TerrainData_Get_Custom_PropTreeInstances); //  <- UnityEngine.TerrainData::get_treeInstances
    SET_METRO_BINDING(TerrainData_Set_Custom_PropTreeInstances); //  <- UnityEngine.TerrainData::set_treeInstances
    SET_METRO_BINDING(TerrainData_Get_Custom_PropTreePrototypes); //  <- UnityEngine.TerrainData::get_treePrototypes
    SET_METRO_BINDING(TerrainData_Set_Custom_PropTreePrototypes); //  <- UnityEngine.TerrainData::set_treePrototypes
    SET_METRO_BINDING(TerrainData_CUSTOM_RemoveTreePrototype); //  <- UnityEngine.TerrainData::RemoveTreePrototype
    SET_METRO_BINDING(TerrainData_CUSTOM_RecalculateTreePositions); //  <- UnityEngine.TerrainData::RecalculateTreePositions
    SET_METRO_BINDING(TerrainData_CUSTOM_RemoveDetailPrototype); //  <- UnityEngine.TerrainData::RemoveDetailPrototype
    SET_METRO_BINDING(TerrainData_Get_Custom_PropAlphamapLayers); //  <- UnityEngine.TerrainData::get_alphamapLayers
    SET_METRO_BINDING(TerrainData_CUSTOM_GetAlphamaps); //  <- UnityEngine.TerrainData::GetAlphamaps
    SET_METRO_BINDING(TerrainData_Get_Custom_PropAlphamapResolution); //  <- UnityEngine.TerrainData::get_alphamapResolution
    SET_METRO_BINDING(TerrainData_Set_Custom_PropAlphamapResolution); //  <- UnityEngine.TerrainData::set_alphamapResolution
    SET_METRO_BINDING(TerrainData_Get_Custom_PropAlphamapWidth); //  <- UnityEngine.TerrainData::get_alphamapWidth
    SET_METRO_BINDING(TerrainData_Get_Custom_PropAlphamapHeight); //  <- UnityEngine.TerrainData::get_alphamapHeight
    SET_METRO_BINDING(TerrainData_Get_Custom_PropBaseMapResolution); //  <- UnityEngine.TerrainData::get_baseMapResolution
    SET_METRO_BINDING(TerrainData_Set_Custom_PropBaseMapResolution); //  <- UnityEngine.TerrainData::set_baseMapResolution
    SET_METRO_BINDING(TerrainData_CUSTOM_Internal_SetAlphamaps); //  <- UnityEngine.TerrainData::Internal_SetAlphamaps
    SET_METRO_BINDING(TerrainData_CUSTOM_RecalculateBasemapIfDirty); //  <- UnityEngine.TerrainData::RecalculateBasemapIfDirty
    SET_METRO_BINDING(TerrainData_CUSTOM_SetBasemapDirty); //  <- UnityEngine.TerrainData::SetBasemapDirty
    SET_METRO_BINDING(TerrainData_CUSTOM_GetAlphamapTexture); //  <- UnityEngine.TerrainData::GetAlphamapTexture
    SET_METRO_BINDING(TerrainData_Get_Custom_PropAlphamapTextureCount); //  <- UnityEngine.TerrainData::get_alphamapTextureCount
    SET_METRO_BINDING(TerrainData_Get_Custom_PropSplatPrototypes); //  <- UnityEngine.TerrainData::get_splatPrototypes
    SET_METRO_BINDING(TerrainData_Set_Custom_PropSplatPrototypes); //  <- UnityEngine.TerrainData::set_splatPrototypes
    SET_METRO_BINDING(TerrainData_CUSTOM_HasTreeInstances); //  <- UnityEngine.TerrainData::HasTreeInstances
    SET_METRO_BINDING(TerrainData_CUSTOM_AddTree); //  <- UnityEngine.TerrainData::AddTree
    SET_METRO_BINDING(TerrainData_CUSTOM_INTERNAL_CALL_RemoveTrees); //  <- UnityEngine.TerrainData::INTERNAL_CALL_RemoveTrees
    SET_METRO_BINDING(TreeRenderer_CUSTOM_INTERNAL_CALL_TreeRenderer); //  <- UnityEngine.TreeRenderer::INTERNAL_CALL_TreeRenderer
    SET_METRO_BINDING(TreeRenderer_CUSTOM_ReloadTrees); //  <- UnityEngine.TreeRenderer::ReloadTrees
    SET_METRO_BINDING(TreeRenderer_CUSTOM_Render); //  <- UnityEngine.TreeRenderer::Render
    SET_METRO_BINDING(TreeRenderer_CUSTOM_RenderShadowCasters); //  <- UnityEngine.TreeRenderer::RenderShadowCasters
    SET_METRO_BINDING(TreeRenderer_CUSTOM_Cleanup); //  <- UnityEngine.TreeRenderer::Cleanup
    SET_METRO_BINDING(TreeRenderer_CUSTOM_InjectTree); //  <- UnityEngine.TreeRenderer::InjectTree
    SET_METRO_BINDING(TreeRenderer_CUSTOM_INTERNAL_CALL_RemoveTrees); //  <- UnityEngine.TreeRenderer::INTERNAL_CALL_RemoveTrees
    SET_METRO_BINDING(TreeRenderer_CUSTOM_InvalidateImposters); //  <- UnityEngine.TreeRenderer::InvalidateImposters
    SET_METRO_BINDING(TreeRenderer_Get_Custom_PropLightmapIndex); //  <- UnityEngine.TreeRenderer::get_lightmapIndex
    SET_METRO_BINDING(TreeRenderer_Set_Custom_PropLightmapIndex); //  <- UnityEngine.TreeRenderer::set_lightmapIndex
    SET_METRO_BINDING(WindZone_Get_Custom_PropMode); //  <- UnityEngine.WindZone::get_mode
    SET_METRO_BINDING(WindZone_Set_Custom_PropMode); //  <- UnityEngine.WindZone::set_mode
    SET_METRO_BINDING(WindZone_Get_Custom_PropRadius); //  <- UnityEngine.WindZone::get_radius
    SET_METRO_BINDING(WindZone_Set_Custom_PropRadius); //  <- UnityEngine.WindZone::set_radius
    SET_METRO_BINDING(WindZone_Get_Custom_PropWindMain); //  <- UnityEngine.WindZone::get_windMain
    SET_METRO_BINDING(WindZone_Set_Custom_PropWindMain); //  <- UnityEngine.WindZone::set_windMain
    SET_METRO_BINDING(WindZone_Get_Custom_PropWindTurbulence); //  <- UnityEngine.WindZone::get_windTurbulence
    SET_METRO_BINDING(WindZone_Set_Custom_PropWindTurbulence); //  <- UnityEngine.WindZone::set_windTurbulence
    SET_METRO_BINDING(WindZone_Get_Custom_PropWindPulseMagnitude); //  <- UnityEngine.WindZone::get_windPulseMagnitude
    SET_METRO_BINDING(WindZone_Set_Custom_PropWindPulseMagnitude); //  <- UnityEngine.WindZone::set_windPulseMagnitude
    SET_METRO_BINDING(WindZone_Get_Custom_PropWindPulseFrequency); //  <- UnityEngine.WindZone::get_windPulseFrequency
    SET_METRO_BINDING(WindZone_Set_Custom_PropWindPulseFrequency); //  <- UnityEngine.WindZone::set_windPulseFrequency
    SET_METRO_BINDING(Tree_Get_Custom_PropData); //  <- UnityEngine.Tree::get_data
    SET_METRO_BINDING(Tree_Set_Custom_PropData); //  <- UnityEngine.Tree::set_data
#endif
#if ENABLE_TERRAIN && ENABLE_PHYSICS
    SET_METRO_BINDING(TerrainCollider_Get_Custom_PropTerrainData); //  <- UnityEngine.TerrainCollider::get_terrainData
    SET_METRO_BINDING(TerrainCollider_Set_Custom_PropTerrainData); //  <- UnityEngine.TerrainCollider::set_terrainData
#endif
}

#endif
