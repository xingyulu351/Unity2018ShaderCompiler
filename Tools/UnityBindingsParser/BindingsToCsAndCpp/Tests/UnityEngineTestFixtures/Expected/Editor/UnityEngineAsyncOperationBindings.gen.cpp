#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Misc/AsyncOperation.h"
	
using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AsyncOperation_CUSTOM_InternalDestroy(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_CUSTOM_InternalDestroy)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(InternalDestroy)
    
    		self->Release();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AsyncOperation_Get_Custom_PropIsDone(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_Get_Custom_PropIsDone)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isDone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isDone)
    return self->IsDone();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AsyncOperation_Get_Custom_PropProgress(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_Get_Custom_PropProgress)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_progress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_progress)
    return self->GetProgress();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AsyncOperation_Get_Custom_PropPriority(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_Get_Custom_PropPriority)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_priority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_priority)
    return self->GetPriority();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AsyncOperation_Set_Custom_PropPriority(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_Set_Custom_PropPriority)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_priority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_priority)
    
    if (value < 0)
    {
    value = 0;
    ErrorString ("Priority can't be set to negative value");
    }
    return self->SetPriority(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AsyncOperation_Get_Custom_PropAllowSceneActivation(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_Get_Custom_PropAllowSceneActivation)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_allowSceneActivation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_allowSceneActivation)
    return self->GetAllowSceneActivation();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AsyncOperation_Set_Custom_PropAllowSceneActivation(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AsyncOperation_Set_Custom_PropAllowSceneActivation)
    ScriptingObjectWithIntPtrField<AsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_allowSceneActivation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_allowSceneActivation)
    
    return self->SetAllowSceneActivation(value);
    
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_AsyncOperation_InternalDestroy()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::InternalDestroy" , (gpointer)& AsyncOperation_CUSTOM_InternalDestroy );
}

void Register_UnityEngine_AsyncOperation_get_isDone()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::get_isDone" , (gpointer)& AsyncOperation_Get_Custom_PropIsDone );
}

void Register_UnityEngine_AsyncOperation_get_progress()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::get_progress" , (gpointer)& AsyncOperation_Get_Custom_PropProgress );
}

void Register_UnityEngine_AsyncOperation_get_priority()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::get_priority" , (gpointer)& AsyncOperation_Get_Custom_PropPriority );
}

void Register_UnityEngine_AsyncOperation_set_priority()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::set_priority" , (gpointer)& AsyncOperation_Set_Custom_PropPriority );
}

void Register_UnityEngine_AsyncOperation_get_allowSceneActivation()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::get_allowSceneActivation" , (gpointer)& AsyncOperation_Get_Custom_PropAllowSceneActivation );
}

void Register_UnityEngine_AsyncOperation_set_allowSceneActivation()
{
    scripting_add_internal_call( "UnityEngine.AsyncOperation::set_allowSceneActivation" , (gpointer)& AsyncOperation_Set_Custom_PropAllowSceneActivation );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineAsyncOperation_IcallNames [] =
{
    "UnityEngine.AsyncOperation::InternalDestroy",    // -> AsyncOperation_CUSTOM_InternalDestroy
    "UnityEngine.AsyncOperation::get_isDone",    // -> AsyncOperation_Get_Custom_PropIsDone
    "UnityEngine.AsyncOperation::get_progress",    // -> AsyncOperation_Get_Custom_PropProgress
    "UnityEngine.AsyncOperation::get_priority",    // -> AsyncOperation_Get_Custom_PropPriority
    "UnityEngine.AsyncOperation::set_priority",    // -> AsyncOperation_Set_Custom_PropPriority
    "UnityEngine.AsyncOperation::get_allowSceneActivation",    // -> AsyncOperation_Get_Custom_PropAllowSceneActivation
    "UnityEngine.AsyncOperation::set_allowSceneActivation",    // -> AsyncOperation_Set_Custom_PropAllowSceneActivation
    NULL
};

static const void* s_UnityEngineAsyncOperation_IcallFuncs [] =
{
    (const void*)&AsyncOperation_CUSTOM_InternalDestroy   ,  //  <- UnityEngine.AsyncOperation::InternalDestroy
    (const void*)&AsyncOperation_Get_Custom_PropIsDone    ,  //  <- UnityEngine.AsyncOperation::get_isDone
    (const void*)&AsyncOperation_Get_Custom_PropProgress  ,  //  <- UnityEngine.AsyncOperation::get_progress
    (const void*)&AsyncOperation_Get_Custom_PropPriority  ,  //  <- UnityEngine.AsyncOperation::get_priority
    (const void*)&AsyncOperation_Set_Custom_PropPriority  ,  //  <- UnityEngine.AsyncOperation::set_priority
    (const void*)&AsyncOperation_Get_Custom_PropAllowSceneActivation,  //  <- UnityEngine.AsyncOperation::get_allowSceneActivation
    (const void*)&AsyncOperation_Set_Custom_PropAllowSceneActivation,  //  <- UnityEngine.AsyncOperation::set_allowSceneActivation
    NULL
};

void ExportUnityEngineAsyncOperationBindings();
void ExportUnityEngineAsyncOperationBindings()
{
    for (int i = 0; s_UnityEngineAsyncOperation_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineAsyncOperation_IcallNames [i], s_UnityEngineAsyncOperation_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineAsyncOperationBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(AsyncOperation_CUSTOM_InternalDestroy); //  <- UnityEngine.AsyncOperation::InternalDestroy
    SET_METRO_BINDING(AsyncOperation_Get_Custom_PropIsDone); //  <- UnityEngine.AsyncOperation::get_isDone
    SET_METRO_BINDING(AsyncOperation_Get_Custom_PropProgress); //  <- UnityEngine.AsyncOperation::get_progress
    SET_METRO_BINDING(AsyncOperation_Get_Custom_PropPriority); //  <- UnityEngine.AsyncOperation::get_priority
    SET_METRO_BINDING(AsyncOperation_Set_Custom_PropPriority); //  <- UnityEngine.AsyncOperation::set_priority
    SET_METRO_BINDING(AsyncOperation_Get_Custom_PropAllowSceneActivation); //  <- UnityEngine.AsyncOperation::get_allowSceneActivation
    SET_METRO_BINDING(AsyncOperation_Set_Custom_PropAllowSceneActivation); //  <- UnityEngine.AsyncOperation::set_allowSceneActivation
}

#endif
