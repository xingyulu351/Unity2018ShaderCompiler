#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/DelayedCallUtility.h"

using namespace Unity;
using namespace std;
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_MonoBehaviour(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_MonoBehaviour)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(.ctor)
    
    		#if UNITY_EDITOR
    		if (self.GetInstanceID() == 0)
    		{
    			ScriptWarning ("You are trying to create a MonoBehaviour using the 'new' keyword.  This is not allowed.  MonoBehaviours can only be added using AddComponent().  Alternatively, your script can inherit from ScriptableObject or no base class at all", NULL);
    		}
    		#endif
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_Internal_CancelInvokeAll(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_Internal_CancelInvokeAll)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_CancelInvokeAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CancelInvokeAll)
    
    		CancelInvoke (*self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_Internal_IsInvokingAll(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_Internal_IsInvokingAll)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_IsInvokingAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_IsInvokingAll)
    
    		return IsInvoking (*self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_Invoke(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, float time)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_Invoke)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    SCRIPTINGAPI_STACK_CHECK(Invoke)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Invoke)
    
    		InvokeDelayed (*self, methodName, time, 0.0F);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_InvokeRepeating(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, float time, float repeatRate)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_InvokeRepeating)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    SCRIPTINGAPI_STACK_CHECK(InvokeRepeating)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InvokeRepeating)
     
    		InvokeDelayed (*self, methodName, time, repeatRate); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_CancelInvoke(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_CancelInvoke)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    SCRIPTINGAPI_STACK_CHECK(CancelInvoke)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CancelInvoke)
     CancelInvoke (*self, methodName); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_IsInvoking(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_IsInvoking)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    SCRIPTINGAPI_STACK_CHECK(IsInvoking)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsInvoking)
     return IsInvoking (*self, methodName); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_StartCoroutine_Auto(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Object_Argument routine_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_StartCoroutine_Auto)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_Object_Local routine(routine_);
    UNUSED(routine);
    SCRIPTINGAPI_STACK_CHECK(StartCoroutine_Auto)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StartCoroutine_Auto)
    
    		MonoRaiseIfNull(routine);
    		return self->StartCoroutineManaged2(routine);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_StartCoroutine(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_, ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_StartCoroutine)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    ICallType_Object_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(StartCoroutine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StartCoroutine)
    
    		MonoRaiseIfNull((void*)(!methodName.IsNull()));
    		char* cString = ScriptingStringToAllocatedChars (methodName);
    		ScriptingObjectPtr coroutine = self->StartCoroutineManaged (cString, value);
    		ScriptingStringToAllocatedChars_Free (cString);
    		return coroutine;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_StopCoroutine(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument methodName_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_StopCoroutine)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    ICallType_String_Local methodName(methodName_);
    UNUSED(methodName);
    SCRIPTINGAPI_STACK_CHECK(StopCoroutine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopCoroutine)
     self->StopCoroutine(methodName.ToUTF8().c_str()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_CUSTOM_StopAllCoroutines(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_CUSTOM_StopAllCoroutines)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(StopAllCoroutines)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(StopAllCoroutines)
     self->StopAllCoroutines(); 
}

#if !PLATFORM_WEBGL
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MonoBehaviour_Get_Custom_PropUseGUILayout(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_Get_Custom_PropUseGUILayout)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_useGUILayout)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useGUILayout)
    return self->GetUseGUILayout ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MonoBehaviour_Set_Custom_PropUseGUILayout(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(MonoBehaviour_Set_Custom_PropUseGUILayout)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_useGUILayout)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useGUILayout)
    
    self->SetUseGUILayout (value);
    
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
void Register_UnityEngine_MonoBehaviour__ctor()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::.ctor" , (gpointer)& MonoBehaviour_CUSTOM_MonoBehaviour );
}

#endif
void Register_UnityEngine_MonoBehaviour_Internal_CancelInvokeAll()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::Internal_CancelInvokeAll" , (gpointer)& MonoBehaviour_CUSTOM_Internal_CancelInvokeAll );
}

void Register_UnityEngine_MonoBehaviour_Internal_IsInvokingAll()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::Internal_IsInvokingAll" , (gpointer)& MonoBehaviour_CUSTOM_Internal_IsInvokingAll );
}

void Register_UnityEngine_MonoBehaviour_Invoke()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::Invoke" , (gpointer)& MonoBehaviour_CUSTOM_Invoke );
}

void Register_UnityEngine_MonoBehaviour_InvokeRepeating()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::InvokeRepeating" , (gpointer)& MonoBehaviour_CUSTOM_InvokeRepeating );
}

void Register_UnityEngine_MonoBehaviour_CancelInvoke()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::CancelInvoke" , (gpointer)& MonoBehaviour_CUSTOM_CancelInvoke );
}

void Register_UnityEngine_MonoBehaviour_IsInvoking()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::IsInvoking" , (gpointer)& MonoBehaviour_CUSTOM_IsInvoking );
}

void Register_UnityEngine_MonoBehaviour_StartCoroutine_Auto()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::StartCoroutine_Auto" , (gpointer)& MonoBehaviour_CUSTOM_StartCoroutine_Auto );
}

void Register_UnityEngine_MonoBehaviour_StartCoroutine()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::StartCoroutine" , (gpointer)& MonoBehaviour_CUSTOM_StartCoroutine );
}

void Register_UnityEngine_MonoBehaviour_StopCoroutine()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::StopCoroutine" , (gpointer)& MonoBehaviour_CUSTOM_StopCoroutine );
}

void Register_UnityEngine_MonoBehaviour_StopAllCoroutines()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::StopAllCoroutines" , (gpointer)& MonoBehaviour_CUSTOM_StopAllCoroutines );
}

#if !PLATFORM_WEBGL
void Register_UnityEngine_MonoBehaviour_get_useGUILayout()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::get_useGUILayout" , (gpointer)& MonoBehaviour_Get_Custom_PropUseGUILayout );
}

void Register_UnityEngine_MonoBehaviour_set_useGUILayout()
{
    scripting_add_internal_call( "UnityEngine.MonoBehaviour::set_useGUILayout" , (gpointer)& MonoBehaviour_Set_Custom_PropUseGUILayout );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineMonoBehaviour_IcallNames [] =
{
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
    "UnityEngine.MonoBehaviour::.ctor"      ,    // -> MonoBehaviour_CUSTOM_MonoBehaviour
#endif
    "UnityEngine.MonoBehaviour::Internal_CancelInvokeAll",    // -> MonoBehaviour_CUSTOM_Internal_CancelInvokeAll
    "UnityEngine.MonoBehaviour::Internal_IsInvokingAll",    // -> MonoBehaviour_CUSTOM_Internal_IsInvokingAll
    "UnityEngine.MonoBehaviour::Invoke"     ,    // -> MonoBehaviour_CUSTOM_Invoke
    "UnityEngine.MonoBehaviour::InvokeRepeating",    // -> MonoBehaviour_CUSTOM_InvokeRepeating
    "UnityEngine.MonoBehaviour::CancelInvoke",    // -> MonoBehaviour_CUSTOM_CancelInvoke
    "UnityEngine.MonoBehaviour::IsInvoking" ,    // -> MonoBehaviour_CUSTOM_IsInvoking
    "UnityEngine.MonoBehaviour::StartCoroutine_Auto",    // -> MonoBehaviour_CUSTOM_StartCoroutine_Auto
    "UnityEngine.MonoBehaviour::StartCoroutine",    // -> MonoBehaviour_CUSTOM_StartCoroutine
    "UnityEngine.MonoBehaviour::StopCoroutine",    // -> MonoBehaviour_CUSTOM_StopCoroutine
    "UnityEngine.MonoBehaviour::StopAllCoroutines",    // -> MonoBehaviour_CUSTOM_StopAllCoroutines
#if !PLATFORM_WEBGL
    "UnityEngine.MonoBehaviour::get_useGUILayout",    // -> MonoBehaviour_Get_Custom_PropUseGUILayout
    "UnityEngine.MonoBehaviour::set_useGUILayout",    // -> MonoBehaviour_Set_Custom_PropUseGUILayout
#endif
    NULL
};

static const void* s_UnityEngineMonoBehaviour_IcallFuncs [] =
{
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
    (const void*)&MonoBehaviour_CUSTOM_MonoBehaviour      ,  //  <- UnityEngine.MonoBehaviour::.ctor
#endif
    (const void*)&MonoBehaviour_CUSTOM_Internal_CancelInvokeAll,  //  <- UnityEngine.MonoBehaviour::Internal_CancelInvokeAll
    (const void*)&MonoBehaviour_CUSTOM_Internal_IsInvokingAll,  //  <- UnityEngine.MonoBehaviour::Internal_IsInvokingAll
    (const void*)&MonoBehaviour_CUSTOM_Invoke             ,  //  <- UnityEngine.MonoBehaviour::Invoke
    (const void*)&MonoBehaviour_CUSTOM_InvokeRepeating    ,  //  <- UnityEngine.MonoBehaviour::InvokeRepeating
    (const void*)&MonoBehaviour_CUSTOM_CancelInvoke       ,  //  <- UnityEngine.MonoBehaviour::CancelInvoke
    (const void*)&MonoBehaviour_CUSTOM_IsInvoking         ,  //  <- UnityEngine.MonoBehaviour::IsInvoking
    (const void*)&MonoBehaviour_CUSTOM_StartCoroutine_Auto,  //  <- UnityEngine.MonoBehaviour::StartCoroutine_Auto
    (const void*)&MonoBehaviour_CUSTOM_StartCoroutine     ,  //  <- UnityEngine.MonoBehaviour::StartCoroutine
    (const void*)&MonoBehaviour_CUSTOM_StopCoroutine      ,  //  <- UnityEngine.MonoBehaviour::StopCoroutine
    (const void*)&MonoBehaviour_CUSTOM_StopAllCoroutines  ,  //  <- UnityEngine.MonoBehaviour::StopAllCoroutines
#if !PLATFORM_WEBGL
    (const void*)&MonoBehaviour_Get_Custom_PropUseGUILayout,  //  <- UnityEngine.MonoBehaviour::get_useGUILayout
    (const void*)&MonoBehaviour_Set_Custom_PropUseGUILayout,  //  <- UnityEngine.MonoBehaviour::set_useGUILayout
#endif
    NULL
};

void ExportUnityEngineMonoBehaviourBindings();
void ExportUnityEngineMonoBehaviourBindings()
{
    for (int i = 0; s_UnityEngineMonoBehaviour_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineMonoBehaviour_IcallNames [i], s_UnityEngineMonoBehaviour_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineMonoBehaviourBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_MonoBehaviour); //  <- UnityEngine.MonoBehaviour::.ctor
#endif
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_Internal_CancelInvokeAll); //  <- UnityEngine.MonoBehaviour::Internal_CancelInvokeAll
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_Internal_IsInvokingAll); //  <- UnityEngine.MonoBehaviour::Internal_IsInvokingAll
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_Invoke); //  <- UnityEngine.MonoBehaviour::Invoke
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_InvokeRepeating); //  <- UnityEngine.MonoBehaviour::InvokeRepeating
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_CancelInvoke); //  <- UnityEngine.MonoBehaviour::CancelInvoke
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_IsInvoking); //  <- UnityEngine.MonoBehaviour::IsInvoking
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_StartCoroutine_Auto); //  <- UnityEngine.MonoBehaviour::StartCoroutine_Auto
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_StartCoroutine); //  <- UnityEngine.MonoBehaviour::StartCoroutine
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_StopCoroutine); //  <- UnityEngine.MonoBehaviour::StopCoroutine
    SET_METRO_BINDING(MonoBehaviour_CUSTOM_StopAllCoroutines); //  <- UnityEngine.MonoBehaviour::StopAllCoroutines
#if !PLATFORM_WEBGL
    SET_METRO_BINDING(MonoBehaviour_Get_Custom_PropUseGUILayout); //  <- UnityEngine.MonoBehaviour::get_useGUILayout
    SET_METRO_BINDING(MonoBehaviour_Set_Custom_PropUseGUILayout); //  <- UnityEngine.MonoBehaviour::set_useGUILayout
#endif
}

#endif
