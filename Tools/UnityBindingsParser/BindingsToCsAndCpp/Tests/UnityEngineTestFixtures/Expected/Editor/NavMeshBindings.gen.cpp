#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoManager.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/NavMesh/NavMesh.h"
#include "Runtime/NavMesh/NavMeshAgent.h"
#include "Runtime/NavMesh/OffMeshLink.h"
#include "Runtime/NavMesh/NavMeshSettings.h"
#include "Runtime/NavMesh/NavMeshLayers.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "External/Recast/Detour/Include/DetourNavMeshQuery.h"
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION OffMeshLinkData_CUSTOM_GetOffMeshLinkInternal(ICallType_ReadOnlyUnityEngineObject_Argument self_, int instanceID)
{
    SCRIPTINGAPI_ETW_ENTRY(OffMeshLinkData_CUSTOM_GetOffMeshLinkInternal)
    ReadOnlyScriptingObjectOfType<OffMeshLinkData> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetOffMeshLinkInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetOffMeshLinkInternal)
    
    		return Scripting::ScriptingWrapperFor (dynamic_instanceID_cast<OffMeshLink*> (instanceID));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_INTERNAL_CALL_Raycast(const Vector3f& sourcePosition, const Vector3f& targetPosition, NavMeshHit* hit, int passableMask)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_INTERNAL_CALL_Raycast)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Raycast)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Raycast)
    
    		NavMesh* navMesh = GetNavMeshSettings ().GetNavMesh ();
    		if (navMesh == NULL) {
    			hit->position = targetPosition;
    			hit->hit = 0;
    			hit->normal = Vector3f::zero;
    			hit->distance = Magnitude (targetPosition-sourcePosition);
    			return false;
    		}
    		const dtQueryFilter filter = dtQueryFilter::createFilterForIncludeFlags (passableMask);
    		return navMesh->Raycast (sourcePosition, targetPosition, hit, &filter);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_INTERNAL_CALL_CalculatePathInternal(const Vector3f& sourcePosition, const Vector3f& targetPosition, int passableMask, ICallType_Object_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_INTERNAL_CALL_CalculatePathInternal)
    ICallType_Object_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CalculatePathInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CalculatePathInternal)
    
    		NavMesh* navMesh = GetNavMeshSettings ().GetNavMesh ();
    		if (navMesh == NULL)
    			return false;
    
    		MonoNavMeshPath monopath;
    		MarshallManagedStructIntoNative (path, &monopath);
    
    		const dtQueryFilter filter = dtQueryFilter::createFilterForIncludeFlags (passableMask);
    		int actualSize = navMesh->CalculatePolygonPath (sourcePosition, targetPosition, &filter, monopath.native);
    		return actualSize>0;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_INTERNAL_CALL_FindClosestEdge(const Vector3f& sourcePosition, NavMeshHit* hit, int passableMask)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_INTERNAL_CALL_FindClosestEdge)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_FindClosestEdge)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_FindClosestEdge)
    
    		NavMesh* navMesh = GetNavMeshSettings ().GetNavMesh ();
    		if (navMesh == NULL) {
    			hit->position = Vector3f::infinityVec;
    			hit->hit = 0;
    			hit->normal = Vector3f::zero;
    			hit->distance = Vector3f::infinity;
    			return false;
    		}
    		const dtQueryFilter filter = dtQueryFilter::createFilterForIncludeFlags (passableMask);
    		return navMesh->DistanceToEdge (sourcePosition, hit, &filter);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_INTERNAL_CALL_SamplePosition(const Vector3f& sourcePosition, NavMeshHit* hit, float maxDistance, int allowedMask)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_INTERNAL_CALL_SamplePosition)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SamplePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SamplePosition)
    
    		NavMesh* navMesh = GetNavMeshSettings ().GetNavMesh ();
    		if (navMesh == NULL) {
    			hit->position = Vector3f::infinityVec;
    			hit->hit = 0;
    			hit->normal = Vector3f::zero;
    			hit->distance = Vector3f::infinity;
    			return false;
    		}
    		const dtQueryFilter filter = dtQueryFilter::createFilterForIncludeFlags (allowedMask);
    		return navMesh->SamplePosition (sourcePosition, hit, &filter, maxDistance);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_SetLayerCost(int layer, float cost)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_SetLayerCost)
    SCRIPTINGAPI_STACK_CHECK(SetLayerCost)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLayerCost)
    
    		GetNavMeshLayers ().SetLayerCost (layer, cost);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_GetLayerCost(int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_GetLayerCost)
    SCRIPTINGAPI_STACK_CHECK(GetLayerCost)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetLayerCost)
    
    		return GetNavMeshLayers ().GetLayerCost (layer);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_GetNavMeshLayerFromName(ICallType_String_Argument layerName_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_GetNavMeshLayerFromName)
    ICallType_String_Local layerName(layerName_);
    UNUSED(layerName);
    SCRIPTINGAPI_STACK_CHECK(GetNavMeshLayerFromName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNavMeshLayerFromName)
    
    		return GetNavMeshLayers ().GetNavMeshLayerFromName (layerName.ToUTF8());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_AddOffMeshLinks()
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_AddOffMeshLinks)
    SCRIPTINGAPI_STACK_CHECK(AddOffMeshLinks)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddOffMeshLinks)
     GetNavMeshSettings ().AddOffMeshLinks (); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_RestoreNavMesh()
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_RestoreNavMesh)
    SCRIPTINGAPI_STACK_CHECK(RestoreNavMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RestoreNavMesh)
     GetNavMeshSettings ().RestoreNavMesh (); 
}

#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMesh_CUSTOM_Triangulate(ICallType_Array_Argument_Out vertices_, ICallType_Array_Argument_Out indices_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMesh_CUSTOM_Triangulate)
    ICallType_Array_Local_Out vertices(vertices_);
    UNUSED(vertices);
    ICallType_Array_Local_Out indices(indices_);
    UNUSED(indices);
    SCRIPTINGAPI_STACK_CHECK(Triangulate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Triangulate)
    
    		NavMesh::Triangulation triangulation;
    		NavMesh* navMesh = GetNavMeshSettings ().GetNavMesh ();
    		if (navMesh != NULL) {
    			navMesh->Triangulate (triangulation);
    		}
    		if (vertices) {
    			*vertices = CreateScriptingArray (triangulation.vertices.size() > 0 ? &triangulation.vertices[0] : NULL, triangulation.vertices.size (), GetCommonScriptingClasses ().vector3 );
    		}
    		if (indices) {
    			*indices = CreateScriptingArray (triangulation.indices.size() > 0 ? &triangulation.indices[0] : NULL, triangulation.indices.size (), GetCommonScriptingClasses ().int_32 );
    		}
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION OffMeshLink_Get_Custom_PropActivated(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(OffMeshLink_Get_Custom_PropActivated)
    ReadOnlyScriptingObjectOfType<OffMeshLink> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_activated)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_activated)
    return self->GetActivated ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OffMeshLink_Set_Custom_PropActivated(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(OffMeshLink_Set_Custom_PropActivated)
    ReadOnlyScriptingObjectOfType<OffMeshLink> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_activated)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_activated)
    
    self->SetActivated (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION OffMeshLink_Get_Custom_PropOccupied(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(OffMeshLink_Get_Custom_PropOccupied)
    ReadOnlyScriptingObjectOfType<OffMeshLink> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_occupied)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_occupied)
    return self->GetOccupied ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION OffMeshLink_Get_Custom_PropCostOverride(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(OffMeshLink_Get_Custom_PropCostOverride)
    ReadOnlyScriptingObjectOfType<OffMeshLink> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_costOverride)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_costOverride)
    return self->GetCostOverride ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION OffMeshLink_Set_Custom_PropCostOverride(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(OffMeshLink_Set_Custom_PropCostOverride)
    ReadOnlyScriptingObjectOfType<OffMeshLink> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_costOverride)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_costOverride)
    
    self->SetCostOverride (value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_OffMeshLinkData_GetOffMeshLinkInternal()
{
    scripting_add_internal_call( "UnityEngine.OffMeshLinkData::GetOffMeshLinkInternal" , (gpointer)& OffMeshLinkData_CUSTOM_GetOffMeshLinkInternal );
}

void Register_UnityEngine_NavMesh_INTERNAL_CALL_Raycast()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::INTERNAL_CALL_Raycast" , (gpointer)& NavMesh_CUSTOM_INTERNAL_CALL_Raycast );
}

void Register_UnityEngine_NavMesh_INTERNAL_CALL_CalculatePathInternal()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::INTERNAL_CALL_CalculatePathInternal" , (gpointer)& NavMesh_CUSTOM_INTERNAL_CALL_CalculatePathInternal );
}

void Register_UnityEngine_NavMesh_INTERNAL_CALL_FindClosestEdge()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::INTERNAL_CALL_FindClosestEdge" , (gpointer)& NavMesh_CUSTOM_INTERNAL_CALL_FindClosestEdge );
}

void Register_UnityEngine_NavMesh_INTERNAL_CALL_SamplePosition()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::INTERNAL_CALL_SamplePosition" , (gpointer)& NavMesh_CUSTOM_INTERNAL_CALL_SamplePosition );
}

void Register_UnityEngine_NavMesh_SetLayerCost()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::SetLayerCost" , (gpointer)& NavMesh_CUSTOM_SetLayerCost );
}

void Register_UnityEngine_NavMesh_GetLayerCost()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::GetLayerCost" , (gpointer)& NavMesh_CUSTOM_GetLayerCost );
}

void Register_UnityEngine_NavMesh_GetNavMeshLayerFromName()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::GetNavMeshLayerFromName" , (gpointer)& NavMesh_CUSTOM_GetNavMeshLayerFromName );
}

void Register_UnityEngine_NavMesh_AddOffMeshLinks()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::AddOffMeshLinks" , (gpointer)& NavMesh_CUSTOM_AddOffMeshLinks );
}

void Register_UnityEngine_NavMesh_RestoreNavMesh()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::RestoreNavMesh" , (gpointer)& NavMesh_CUSTOM_RestoreNavMesh );
}

#if !UNITY_FLASH
void Register_UnityEngine_NavMesh_Triangulate()
{
    scripting_add_internal_call( "UnityEngine.NavMesh::Triangulate" , (gpointer)& NavMesh_CUSTOM_Triangulate );
}

#endif
void Register_UnityEngine_OffMeshLink_get_activated()
{
    scripting_add_internal_call( "UnityEngine.OffMeshLink::get_activated" , (gpointer)& OffMeshLink_Get_Custom_PropActivated );
}

void Register_UnityEngine_OffMeshLink_set_activated()
{
    scripting_add_internal_call( "UnityEngine.OffMeshLink::set_activated" , (gpointer)& OffMeshLink_Set_Custom_PropActivated );
}

void Register_UnityEngine_OffMeshLink_get_occupied()
{
    scripting_add_internal_call( "UnityEngine.OffMeshLink::get_occupied" , (gpointer)& OffMeshLink_Get_Custom_PropOccupied );
}

void Register_UnityEngine_OffMeshLink_get_costOverride()
{
    scripting_add_internal_call( "UnityEngine.OffMeshLink::get_costOverride" , (gpointer)& OffMeshLink_Get_Custom_PropCostOverride );
}

void Register_UnityEngine_OffMeshLink_set_costOverride()
{
    scripting_add_internal_call( "UnityEngine.OffMeshLink::set_costOverride" , (gpointer)& OffMeshLink_Set_Custom_PropCostOverride );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_NavMesh_IcallNames [] =
{
    "UnityEngine.OffMeshLinkData::GetOffMeshLinkInternal",    // -> OffMeshLinkData_CUSTOM_GetOffMeshLinkInternal
    "UnityEngine.NavMesh::INTERNAL_CALL_Raycast",    // -> NavMesh_CUSTOM_INTERNAL_CALL_Raycast
    "UnityEngine.NavMesh::INTERNAL_CALL_CalculatePathInternal",    // -> NavMesh_CUSTOM_INTERNAL_CALL_CalculatePathInternal
    "UnityEngine.NavMesh::INTERNAL_CALL_FindClosestEdge",    // -> NavMesh_CUSTOM_INTERNAL_CALL_FindClosestEdge
    "UnityEngine.NavMesh::INTERNAL_CALL_SamplePosition",    // -> NavMesh_CUSTOM_INTERNAL_CALL_SamplePosition
    "UnityEngine.NavMesh::SetLayerCost"     ,    // -> NavMesh_CUSTOM_SetLayerCost
    "UnityEngine.NavMesh::GetLayerCost"     ,    // -> NavMesh_CUSTOM_GetLayerCost
    "UnityEngine.NavMesh::GetNavMeshLayerFromName",    // -> NavMesh_CUSTOM_GetNavMeshLayerFromName
    "UnityEngine.NavMesh::AddOffMeshLinks"  ,    // -> NavMesh_CUSTOM_AddOffMeshLinks
    "UnityEngine.NavMesh::RestoreNavMesh"   ,    // -> NavMesh_CUSTOM_RestoreNavMesh
#if !UNITY_FLASH
    "UnityEngine.NavMesh::Triangulate"      ,    // -> NavMesh_CUSTOM_Triangulate
#endif
    "UnityEngine.OffMeshLink::get_activated",    // -> OffMeshLink_Get_Custom_PropActivated
    "UnityEngine.OffMeshLink::set_activated",    // -> OffMeshLink_Set_Custom_PropActivated
    "UnityEngine.OffMeshLink::get_occupied" ,    // -> OffMeshLink_Get_Custom_PropOccupied
    "UnityEngine.OffMeshLink::get_costOverride",    // -> OffMeshLink_Get_Custom_PropCostOverride
    "UnityEngine.OffMeshLink::set_costOverride",    // -> OffMeshLink_Set_Custom_PropCostOverride
    NULL
};

static const void* s_NavMesh_IcallFuncs [] =
{
    (const void*)&OffMeshLinkData_CUSTOM_GetOffMeshLinkInternal,  //  <- UnityEngine.OffMeshLinkData::GetOffMeshLinkInternal
    (const void*)&NavMesh_CUSTOM_INTERNAL_CALL_Raycast    ,  //  <- UnityEngine.NavMesh::INTERNAL_CALL_Raycast
    (const void*)&NavMesh_CUSTOM_INTERNAL_CALL_CalculatePathInternal,  //  <- UnityEngine.NavMesh::INTERNAL_CALL_CalculatePathInternal
    (const void*)&NavMesh_CUSTOM_INTERNAL_CALL_FindClosestEdge,  //  <- UnityEngine.NavMesh::INTERNAL_CALL_FindClosestEdge
    (const void*)&NavMesh_CUSTOM_INTERNAL_CALL_SamplePosition,  //  <- UnityEngine.NavMesh::INTERNAL_CALL_SamplePosition
    (const void*)&NavMesh_CUSTOM_SetLayerCost             ,  //  <- UnityEngine.NavMesh::SetLayerCost
    (const void*)&NavMesh_CUSTOM_GetLayerCost             ,  //  <- UnityEngine.NavMesh::GetLayerCost
    (const void*)&NavMesh_CUSTOM_GetNavMeshLayerFromName  ,  //  <- UnityEngine.NavMesh::GetNavMeshLayerFromName
    (const void*)&NavMesh_CUSTOM_AddOffMeshLinks          ,  //  <- UnityEngine.NavMesh::AddOffMeshLinks
    (const void*)&NavMesh_CUSTOM_RestoreNavMesh           ,  //  <- UnityEngine.NavMesh::RestoreNavMesh
#if !UNITY_FLASH
    (const void*)&NavMesh_CUSTOM_Triangulate              ,  //  <- UnityEngine.NavMesh::Triangulate
#endif
    (const void*)&OffMeshLink_Get_Custom_PropActivated    ,  //  <- UnityEngine.OffMeshLink::get_activated
    (const void*)&OffMeshLink_Set_Custom_PropActivated    ,  //  <- UnityEngine.OffMeshLink::set_activated
    (const void*)&OffMeshLink_Get_Custom_PropOccupied     ,  //  <- UnityEngine.OffMeshLink::get_occupied
    (const void*)&OffMeshLink_Get_Custom_PropCostOverride ,  //  <- UnityEngine.OffMeshLink::get_costOverride
    (const void*)&OffMeshLink_Set_Custom_PropCostOverride ,  //  <- UnityEngine.OffMeshLink::set_costOverride
    NULL
};

void ExportNavMeshBindings();
void ExportNavMeshBindings()
{
    for (int i = 0; s_NavMesh_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_NavMesh_IcallNames [i], s_NavMesh_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportNavMeshBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(OffMeshLinkData_CUSTOM_GetOffMeshLinkInternal); //  <- UnityEngine.OffMeshLinkData::GetOffMeshLinkInternal
    SET_METRO_BINDING(NavMesh_CUSTOM_INTERNAL_CALL_Raycast); //  <- UnityEngine.NavMesh::INTERNAL_CALL_Raycast
    SET_METRO_BINDING(NavMesh_CUSTOM_INTERNAL_CALL_CalculatePathInternal); //  <- UnityEngine.NavMesh::INTERNAL_CALL_CalculatePathInternal
    SET_METRO_BINDING(NavMesh_CUSTOM_INTERNAL_CALL_FindClosestEdge); //  <- UnityEngine.NavMesh::INTERNAL_CALL_FindClosestEdge
    SET_METRO_BINDING(NavMesh_CUSTOM_INTERNAL_CALL_SamplePosition); //  <- UnityEngine.NavMesh::INTERNAL_CALL_SamplePosition
    SET_METRO_BINDING(NavMesh_CUSTOM_SetLayerCost); //  <- UnityEngine.NavMesh::SetLayerCost
    SET_METRO_BINDING(NavMesh_CUSTOM_GetLayerCost); //  <- UnityEngine.NavMesh::GetLayerCost
    SET_METRO_BINDING(NavMesh_CUSTOM_GetNavMeshLayerFromName); //  <- UnityEngine.NavMesh::GetNavMeshLayerFromName
    SET_METRO_BINDING(NavMesh_CUSTOM_AddOffMeshLinks); //  <- UnityEngine.NavMesh::AddOffMeshLinks
    SET_METRO_BINDING(NavMesh_CUSTOM_RestoreNavMesh); //  <- UnityEngine.NavMesh::RestoreNavMesh
#if !UNITY_FLASH
    SET_METRO_BINDING(NavMesh_CUSTOM_Triangulate); //  <- UnityEngine.NavMesh::Triangulate
#endif
    SET_METRO_BINDING(OffMeshLink_Get_Custom_PropActivated); //  <- UnityEngine.OffMeshLink::get_activated
    SET_METRO_BINDING(OffMeshLink_Set_Custom_PropActivated); //  <- UnityEngine.OffMeshLink::set_activated
    SET_METRO_BINDING(OffMeshLink_Get_Custom_PropOccupied); //  <- UnityEngine.OffMeshLink::get_occupied
    SET_METRO_BINDING(OffMeshLink_Get_Custom_PropCostOverride); //  <- UnityEngine.OffMeshLink::get_costOverride
    SET_METRO_BINDING(OffMeshLink_Set_Custom_PropCostOverride); //  <- UnityEngine.OffMeshLink::set_costOverride
}

#endif
