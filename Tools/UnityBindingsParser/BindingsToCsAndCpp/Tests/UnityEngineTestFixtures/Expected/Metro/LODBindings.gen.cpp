#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Camera/LODGroup.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Utilities/LODUtility.h"
#include "Runtime/Camera/LODGroupManager.h"
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_CUSTOM_INTERNAL_get_localReferencePoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_CUSTOM_INTERNAL_get_localReferencePoint)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_localReferencePoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_localReferencePoint)
    *returnValue = self->GetLocalReferencePoint();
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_CUSTOM_INTERNAL_set_localReferencePoint(ICallType_ReadOnlyUnityEngineObject_Argument self_, const Vector3f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_CUSTOM_INTERNAL_set_localReferencePoint)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_localReferencePoint)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_localReferencePoint)
    
    self->SetLocalReferencePoint (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION LODGroup_Get_Custom_PropSize(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_Get_Custom_PropSize)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_size)
    return self->GetSize ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_Set_Custom_PropSize(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_Set_Custom_PropSize)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_size)
    
    self->SetSize (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION LODGroup_Get_Custom_PropLodCount(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_Get_Custom_PropLodCount)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lodCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lodCount)
    return self->GetLODCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION LODGroup_Get_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_Get_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return self->GetEnabled ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_Set_Custom_PropEnabled(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_Set_Custom_PropEnabled)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
     self->SetEnabled (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_CUSTOM_RecalculateBounds(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_CUSTOM_RecalculateBounds)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(RecalculateBounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RecalculateBounds)
    
    		CalculateLODGroupBoundingBox (*self);
    	
}

#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_CUSTOM_SetLODS(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument scriptingLODs_)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_CUSTOM_SetLODS)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    ICallType_Array_Local scriptingLODs(scriptingLODs_);
    UNUSED(scriptingLODs);
    SCRIPTINGAPI_STACK_CHECK(SetLODS)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLODS)
    
    		int size = GetScriptingArraySize (scriptingLODs);
    
    		if (size > kMaximumLODLevels)
    		{
    			WarningString(Format("SetLODs: Attempting to set more than the maximum number of LODS (%i) clamping", kMaximumLODLevels));
    			size = kMaximumLODLevels;
    		}
    
    		LODGroup::LODArray lods;
    		lods.resize(size);
    		
    		for (int i = 0; i < size; i++)
    		{
    		#if PLATFORM_WINRT
    			MonoLOD monoLOD;
    			MarshallManagedStructIntoNative(GetScriptingArrayElementNoRef<ScriptingObjectPtr>(scriptingLODs, i), &monoLOD);
    		#else
    			MonoLOD& monoLOD = GetScriptingArrayElement<MonoLOD> (scriptingLODs, i);
    		#endif
    			
    			lods[i].screenRelativeHeight = monoLOD.screenRelativeTransitionHeight;
    			
    			lods[i].screenRelativeHeight = clamp01 (lods[i].screenRelativeHeight);
    
    			if (i > 0 && (lods[i].screenRelativeHeight >= lods[i-1].screenRelativeHeight))
    			{
    				ErrorString("SetLODs: Attempting to set LOD where the screen relative size is greater then or equal to a higher detail LOD level.");
    				return;
    			}
    
    			int renderersSize = GetScriptingArraySize (monoLOD.renderers);
    			lods[i].renderers.resize_initialized(renderersSize);
    			for (int r = 0; r < renderersSize; r++)
    			{
    				ScriptingObjectPtr o = GetScriptingArrayElementNoRef<ScriptingObjectPtr>(monoLOD.renderers,r);
    				lods[i].renderers[r].renderer = ScriptingObjectToObject<Renderer> (o);;
    			}
    		}
    
    		self->SetLODArray (lods);
    		CalculateLODGroupBoundingBox (*self);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION LODGroup_CUSTOM_ForceLOD(ICallType_ReadOnlyUnityEngineObject_Argument self_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(LODGroup_CUSTOM_ForceLOD)
    ReadOnlyScriptingObjectOfType<LODGroup> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ForceLOD)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ForceLOD)
    
    		ForceLODLevel (*self, index);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_LODGroup_INTERNAL_get_localReferencePoint()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::INTERNAL_get_localReferencePoint" , (gpointer)& LODGroup_CUSTOM_INTERNAL_get_localReferencePoint );
}

void Register_UnityEngine_LODGroup_INTERNAL_set_localReferencePoint()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::INTERNAL_set_localReferencePoint" , (gpointer)& LODGroup_CUSTOM_INTERNAL_set_localReferencePoint );
}

void Register_UnityEngine_LODGroup_get_size()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::get_size" , (gpointer)& LODGroup_Get_Custom_PropSize );
}

void Register_UnityEngine_LODGroup_set_size()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::set_size" , (gpointer)& LODGroup_Set_Custom_PropSize );
}

void Register_UnityEngine_LODGroup_get_lodCount()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::get_lodCount" , (gpointer)& LODGroup_Get_Custom_PropLodCount );
}

void Register_UnityEngine_LODGroup_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::get_enabled" , (gpointer)& LODGroup_Get_Custom_PropEnabled );
}

void Register_UnityEngine_LODGroup_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::set_enabled" , (gpointer)& LODGroup_Set_Custom_PropEnabled );
}

void Register_UnityEngine_LODGroup_RecalculateBounds()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::RecalculateBounds" , (gpointer)& LODGroup_CUSTOM_RecalculateBounds );
}

#if !UNITY_FLASH
void Register_UnityEngine_LODGroup_SetLODS()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::SetLODS" , (gpointer)& LODGroup_CUSTOM_SetLODS );
}

#endif
void Register_UnityEngine_LODGroup_ForceLOD()
{
    scripting_add_internal_call( "UnityEngine.LODGroup::ForceLOD" , (gpointer)& LODGroup_CUSTOM_ForceLOD );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_LOD_IcallNames [] =
{
    "UnityEngine.LODGroup::INTERNAL_get_localReferencePoint",    // -> LODGroup_CUSTOM_INTERNAL_get_localReferencePoint
    "UnityEngine.LODGroup::INTERNAL_set_localReferencePoint",    // -> LODGroup_CUSTOM_INTERNAL_set_localReferencePoint
    "UnityEngine.LODGroup::get_size"        ,    // -> LODGroup_Get_Custom_PropSize
    "UnityEngine.LODGroup::set_size"        ,    // -> LODGroup_Set_Custom_PropSize
    "UnityEngine.LODGroup::get_lodCount"    ,    // -> LODGroup_Get_Custom_PropLodCount
    "UnityEngine.LODGroup::get_enabled"     ,    // -> LODGroup_Get_Custom_PropEnabled
    "UnityEngine.LODGroup::set_enabled"     ,    // -> LODGroup_Set_Custom_PropEnabled
    "UnityEngine.LODGroup::RecalculateBounds",    // -> LODGroup_CUSTOM_RecalculateBounds
#if !UNITY_FLASH
    "UnityEngine.LODGroup::SetLODS"         ,    // -> LODGroup_CUSTOM_SetLODS
#endif
    "UnityEngine.LODGroup::ForceLOD"        ,    // -> LODGroup_CUSTOM_ForceLOD
    NULL
};

static const void* s_LOD_IcallFuncs [] =
{
    (const void*)&LODGroup_CUSTOM_INTERNAL_get_localReferencePoint,  //  <- UnityEngine.LODGroup::INTERNAL_get_localReferencePoint
    (const void*)&LODGroup_CUSTOM_INTERNAL_set_localReferencePoint,  //  <- UnityEngine.LODGroup::INTERNAL_set_localReferencePoint
    (const void*)&LODGroup_Get_Custom_PropSize            ,  //  <- UnityEngine.LODGroup::get_size
    (const void*)&LODGroup_Set_Custom_PropSize            ,  //  <- UnityEngine.LODGroup::set_size
    (const void*)&LODGroup_Get_Custom_PropLodCount        ,  //  <- UnityEngine.LODGroup::get_lodCount
    (const void*)&LODGroup_Get_Custom_PropEnabled         ,  //  <- UnityEngine.LODGroup::get_enabled
    (const void*)&LODGroup_Set_Custom_PropEnabled         ,  //  <- UnityEngine.LODGroup::set_enabled
    (const void*)&LODGroup_CUSTOM_RecalculateBounds       ,  //  <- UnityEngine.LODGroup::RecalculateBounds
#if !UNITY_FLASH
    (const void*)&LODGroup_CUSTOM_SetLODS                 ,  //  <- UnityEngine.LODGroup::SetLODS
#endif
    (const void*)&LODGroup_CUSTOM_ForceLOD                ,  //  <- UnityEngine.LODGroup::ForceLOD
    NULL
};

void ExportLODBindings();
void ExportLODBindings()
{
    for (int i = 0; s_LOD_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_LOD_IcallNames [i], s_LOD_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportLODBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(LODGroup_CUSTOM_INTERNAL_get_localReferencePoint); //  <- UnityEngine.LODGroup::INTERNAL_get_localReferencePoint
    SET_METRO_BINDING(LODGroup_CUSTOM_INTERNAL_set_localReferencePoint); //  <- UnityEngine.LODGroup::INTERNAL_set_localReferencePoint
    SET_METRO_BINDING(LODGroup_Get_Custom_PropSize); //  <- UnityEngine.LODGroup::get_size
    SET_METRO_BINDING(LODGroup_Set_Custom_PropSize); //  <- UnityEngine.LODGroup::set_size
    SET_METRO_BINDING(LODGroup_Get_Custom_PropLodCount); //  <- UnityEngine.LODGroup::get_lodCount
    SET_METRO_BINDING(LODGroup_Get_Custom_PropEnabled); //  <- UnityEngine.LODGroup::get_enabled
    SET_METRO_BINDING(LODGroup_Set_Custom_PropEnabled); //  <- UnityEngine.LODGroup::set_enabled
    SET_METRO_BINDING(LODGroup_CUSTOM_RecalculateBounds); //  <- UnityEngine.LODGroup::RecalculateBounds
#if !UNITY_FLASH
    SET_METRO_BINDING(LODGroup_CUSTOM_SetLODS); //  <- UnityEngine.LODGroup::SetLODS
#endif
    SET_METRO_BINDING(LODGroup_CUSTOM_ForceLOD); //  <- UnityEngine.LODGroup::ForceLOD
}

#endif
