#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/NavMesh/NavMesh.h"
#include "Runtime/NavMesh/NavMeshPath.h"
#include "Runtime/NavMesh/NavMeshSettings.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Mono/MonoManager.h"


#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
	#define GET ExtractMonoObjectData<NavMeshPath*>(self)
#else
	inline NavMeshPath* GetNativeNavMeshPath (ScriptingObjectPtr self)
	{
		MonoNavMeshPath managedpath;
		MarshallManagedStructIntoNative (self, &managedpath);
		return managedpath.native;
	}
	#define GET GetNativeNavMeshPath (self)
#endif
	
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshPath_CUSTOM_NavMeshPath(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshPath_CUSTOM_NavMeshPath)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(.ctor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(.ctor)
    
    		MonoNavMeshPath managedPath;
    		managedPath.native = new NavMeshPath ();
    		MarshallNativeStructIntoManaged (managedPath,self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshPath_CUSTOM_DestroyNavMeshPath(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshPath_CUSTOM_DestroyNavMeshPath)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(DestroyNavMeshPath)
    
    		if (GET)
    		{
    			delete GET;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION NavMeshPath_CUSTOM_CalculateCornersInternal(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshPath_CUSTOM_CalculateCornersInternal)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(CalculateCornersInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CalculateCornersInternal)
    
    		NavMesh* navMesh = GetNavMeshSettings ().GetNavMesh ();
    		if (navMesh == NULL)
    			return CreateEmptyStructArray (GetMonoManager ().GetCommonClasses ().vector3);
    
    		const int polygonCount = GET->GetPolygonCount ();
    		if (polygonCount == 0)
    			return CreateEmptyStructArray (GetMonoManager ().GetCommonClasses ().vector3);
    
    		Vector3f* corners;
    		ALLOC_TEMP_AUTO (corners, Vector3f, 2+polygonCount, kMemNavigation);
    
    		int cornerCount = navMesh->CalculatePathCorners (GET, corners, 2+polygonCount);
    		if (cornerCount == 0)
    			return CreateEmptyStructArray (GetMonoManager ().GetCommonClasses ().vector3);
    
    		return CreateScriptingArray<Vector3f>(corners, cornerCount, GetMonoManager ().GetCommonClasses ().vector3);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NavMeshPath_CUSTOM_ClearCornersInternal(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshPath_CUSTOM_ClearCornersInternal)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ClearCornersInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearCornersInternal)
    
    		GET->SetPolygonCount (0);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
NavMeshPathStatus SCRIPT_CALL_CONVENTION NavMeshPath_Get_Custom_PropStatus(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NavMeshPath_Get_Custom_PropStatus)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_status)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_status)
    return GET->GetStatus ();
}


 #undef GET
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_NavMeshPath__ctor()
{
    scripting_add_internal_call( "UnityEngine.NavMeshPath::.ctor" , (gpointer)& NavMeshPath_CUSTOM_NavMeshPath );
}

void Register_UnityEngine_NavMeshPath_DestroyNavMeshPath()
{
    scripting_add_internal_call( "UnityEngine.NavMeshPath::DestroyNavMeshPath" , (gpointer)& NavMeshPath_CUSTOM_DestroyNavMeshPath );
}

void Register_UnityEngine_NavMeshPath_CalculateCornersInternal()
{
    scripting_add_internal_call( "UnityEngine.NavMeshPath::CalculateCornersInternal" , (gpointer)& NavMeshPath_CUSTOM_CalculateCornersInternal );
}

void Register_UnityEngine_NavMeshPath_ClearCornersInternal()
{
    scripting_add_internal_call( "UnityEngine.NavMeshPath::ClearCornersInternal" , (gpointer)& NavMeshPath_CUSTOM_ClearCornersInternal );
}

void Register_UnityEngine_NavMeshPath_get_status()
{
    scripting_add_internal_call( "UnityEngine.NavMeshPath::get_status" , (gpointer)& NavMeshPath_Get_Custom_PropStatus );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_NavMeshPath_IcallNames [] =
{
    "UnityEngine.NavMeshPath::.ctor"        ,    // -> NavMeshPath_CUSTOM_NavMeshPath
    "UnityEngine.NavMeshPath::DestroyNavMeshPath",    // -> NavMeshPath_CUSTOM_DestroyNavMeshPath
    "UnityEngine.NavMeshPath::CalculateCornersInternal",    // -> NavMeshPath_CUSTOM_CalculateCornersInternal
    "UnityEngine.NavMeshPath::ClearCornersInternal",    // -> NavMeshPath_CUSTOM_ClearCornersInternal
    "UnityEngine.NavMeshPath::get_status"   ,    // -> NavMeshPath_Get_Custom_PropStatus
    NULL
};

static const void* s_NavMeshPath_IcallFuncs [] =
{
    (const void*)&NavMeshPath_CUSTOM_NavMeshPath          ,  //  <- UnityEngine.NavMeshPath::.ctor
    (const void*)&NavMeshPath_CUSTOM_DestroyNavMeshPath   ,  //  <- UnityEngine.NavMeshPath::DestroyNavMeshPath
    (const void*)&NavMeshPath_CUSTOM_CalculateCornersInternal,  //  <- UnityEngine.NavMeshPath::CalculateCornersInternal
    (const void*)&NavMeshPath_CUSTOM_ClearCornersInternal ,  //  <- UnityEngine.NavMeshPath::ClearCornersInternal
    (const void*)&NavMeshPath_Get_Custom_PropStatus       ,  //  <- UnityEngine.NavMeshPath::get_status
    NULL
};

void ExportNavMeshPathBindings();
void ExportNavMeshPathBindings()
{
    for (int i = 0; s_NavMeshPath_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_NavMeshPath_IcallNames [i], s_NavMeshPath_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportNavMeshPathBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(NavMeshPath_CUSTOM_NavMeshPath); //  <- UnityEngine.NavMeshPath::.ctor
    SET_METRO_BINDING(NavMeshPath_CUSTOM_DestroyNavMeshPath); //  <- UnityEngine.NavMeshPath::DestroyNavMeshPath
    SET_METRO_BINDING(NavMeshPath_CUSTOM_CalculateCornersInternal); //  <- UnityEngine.NavMeshPath::CalculateCornersInternal
    SET_METRO_BINDING(NavMeshPath_CUSTOM_ClearCornersInternal); //  <- UnityEngine.NavMeshPath::ClearCornersInternal
    SET_METRO_BINDING(NavMeshPath_Get_Custom_PropStatus); //  <- UnityEngine.NavMeshPath::get_status
}

#endif
