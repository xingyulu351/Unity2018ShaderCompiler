#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Misc/SaveAndLoadHelper.h"
#include "Runtime/GameCode/CloneObject.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Scripting/ScriptingUtility.h"


using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Object_CUSTOM_CompareBaseObjectsInternal(ICallType_Object_Argument lhs_, ICallType_Object_Argument rhs_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_CompareBaseObjectsInternal)
    ScriptingObjectOfType<Object> lhs(lhs_);
    UNUSED(lhs);
    ScriptingObjectOfType<Object> rhs(rhs_);
    UNUSED(rhs);
    SCRIPTINGAPI_STACK_CHECK(CompareBaseObjectsInternal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CompareBaseObjectsInternal)
    
    		return CompareBaseObjects (lhs.GetScriptingObject(), rhs.GetScriptingObject()); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Object_CUSTOM_Internal_CloneSingle(ICallType_ReadOnlyUnityEngineObject_Argument data_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_Internal_CloneSingle)
    ReadOnlyScriptingObjectOfType<Object> data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(Internal_CloneSingle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CloneSingle)
    
    		return Scripting::ScriptingWrapperFor (&CloneObject (*data));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Object_CUSTOM_INTERNAL_CALL_Internal_InstantiateSingle(ICallType_ReadOnlyUnityEngineObject_Argument data_, const Vector3f& pos, const Quaternionf& rot)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_INTERNAL_CALL_Internal_InstantiateSingle)
    ReadOnlyScriptingObjectOfType<Object> data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_InstantiateSingle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_InstantiateSingle)
    
    		Object* obj = &InstantiateObject (*data, pos, rot);
    #if UNITY_FLASH 
    		return Scripting::ScriptingWrapperForImpl (obj);
    #else
    		return Scripting::ScriptingWrapperFor(obj);
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Object_CUSTOM_Destroy(ICallType_ReadOnlyUnityEngineObject_Argument obj_, float t)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_Destroy)
    ReadOnlyScriptingObjectOfType<Object> obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(Destroy)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Destroy)
     DestroyObjectFromScripting (obj, t); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Object_CUSTOM_DestroyImmediate(ICallType_ReadOnlyUnityEngineObject_Argument obj_, ScriptingBool allowDestroyingAssets)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_DestroyImmediate)
    ReadOnlyScriptingObjectOfType<Object> obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(DestroyImmediate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DestroyImmediate)
     DestroyObjectFromScriptingImmediate (obj, allowDestroyingAssets); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Object_CUSTOM_FindObjectsOfType(ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_FindObjectsOfType)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(FindObjectsOfType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindObjectsOfType)
     return ScriptingFindObjectsOfType (type, kFindActiveSceneObjects); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Object_Get_Custom_PropName(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_Get_Custom_PropName)
    ReadOnlyScriptingObjectOfType<Object> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_name)
    return CreateScriptingString (self->GetName ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Object_Set_Custom_PropName(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_Set_Custom_PropName)
    ReadOnlyScriptingObjectOfType<Object> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_name)
     self->SetName ( value.ToUTF8().c_str()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Object_CUSTOM_DontDestroyOnLoad(ICallType_ReadOnlyUnityEngineObject_Argument target_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_DontDestroyOnLoad)
    ReadOnlyScriptingObjectOfType<Object> target(target_);
    UNUSED(target);
    SCRIPTINGAPI_STACK_CHECK(DontDestroyOnLoad)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DontDestroyOnLoad)
    
    		Object* o = target;
    		if (o)
    			DontDestroyOnLoad (*o);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
Object::HideFlags SCRIPT_CALL_CONVENTION Object_Get_Custom_PropHideFlags(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_Get_Custom_PropHideFlags)
    ReadOnlyScriptingObjectOfType<Object> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hideFlags)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hideFlags)
    return self->GetHideFlags ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Object_Set_Custom_PropHideFlags(ICallType_ReadOnlyUnityEngineObject_Argument self_, Object::HideFlags value)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_Set_Custom_PropHideFlags)
    ReadOnlyScriptingObjectOfType<Object> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_hideFlags)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hideFlags)
    
    self->SetHideFlags (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Object_CUSTOM_DestroyObject(ICallType_ReadOnlyUnityEngineObject_Argument obj_, float t)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_DestroyObject)
    ReadOnlyScriptingObjectOfType<Object> obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(DestroyObject)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DestroyObject)
     DestroyObjectFromScripting (obj, t); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Object_CUSTOM_FindSceneObjectsOfType(ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_FindSceneObjectsOfType)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(FindSceneObjectsOfType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindSceneObjectsOfType)
     return ScriptingFindObjectsOfType (type, kFindActiveSceneObjects); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Object_CUSTOM_FindObjectsOfTypeIncludingAssets(ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_FindObjectsOfTypeIncludingAssets)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(FindObjectsOfTypeIncludingAssets)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindObjectsOfTypeIncludingAssets)
     return ScriptingFindObjectsOfType (type, kFindAssets); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Object_CUSTOM_ToString(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Object_CUSTOM_ToString)
    ReadOnlyScriptingObjectOfType<Object> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ToString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ToString)
    
    		return CreateScriptingString(GetSafeString(BaseObject, UnityObjectToString (self)));
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Object_CompareBaseObjectsInternal()
{
    scripting_add_internal_call( "UnityEngine.Object::CompareBaseObjectsInternal" , (gpointer)& Object_CUSTOM_CompareBaseObjectsInternal );
}

void Register_UnityEngine_Object_Internal_CloneSingle()
{
    scripting_add_internal_call( "UnityEngine.Object::Internal_CloneSingle" , (gpointer)& Object_CUSTOM_Internal_CloneSingle );
}

void Register_UnityEngine_Object_INTERNAL_CALL_Internal_InstantiateSingle()
{
    scripting_add_internal_call( "UnityEngine.Object::INTERNAL_CALL_Internal_InstantiateSingle" , (gpointer)& Object_CUSTOM_INTERNAL_CALL_Internal_InstantiateSingle );
}

void Register_UnityEngine_Object_Destroy()
{
    scripting_add_internal_call( "UnityEngine.Object::Destroy" , (gpointer)& Object_CUSTOM_Destroy );
}

void Register_UnityEngine_Object_DestroyImmediate()
{
    scripting_add_internal_call( "UnityEngine.Object::DestroyImmediate" , (gpointer)& Object_CUSTOM_DestroyImmediate );
}

void Register_UnityEngine_Object_FindObjectsOfType()
{
    scripting_add_internal_call( "UnityEngine.Object::FindObjectsOfType" , (gpointer)& Object_CUSTOM_FindObjectsOfType );
}

void Register_UnityEngine_Object_get_name()
{
    scripting_add_internal_call( "UnityEngine.Object::get_name" , (gpointer)& Object_Get_Custom_PropName );
}

void Register_UnityEngine_Object_set_name()
{
    scripting_add_internal_call( "UnityEngine.Object::set_name" , (gpointer)& Object_Set_Custom_PropName );
}

void Register_UnityEngine_Object_DontDestroyOnLoad()
{
    scripting_add_internal_call( "UnityEngine.Object::DontDestroyOnLoad" , (gpointer)& Object_CUSTOM_DontDestroyOnLoad );
}

void Register_UnityEngine_Object_get_hideFlags()
{
    scripting_add_internal_call( "UnityEngine.Object::get_hideFlags" , (gpointer)& Object_Get_Custom_PropHideFlags );
}

void Register_UnityEngine_Object_set_hideFlags()
{
    scripting_add_internal_call( "UnityEngine.Object::set_hideFlags" , (gpointer)& Object_Set_Custom_PropHideFlags );
}

void Register_UnityEngine_Object_DestroyObject()
{
    scripting_add_internal_call( "UnityEngine.Object::DestroyObject" , (gpointer)& Object_CUSTOM_DestroyObject );
}

void Register_UnityEngine_Object_FindSceneObjectsOfType()
{
    scripting_add_internal_call( "UnityEngine.Object::FindSceneObjectsOfType" , (gpointer)& Object_CUSTOM_FindSceneObjectsOfType );
}

void Register_UnityEngine_Object_FindObjectsOfTypeIncludingAssets()
{
    scripting_add_internal_call( "UnityEngine.Object::FindObjectsOfTypeIncludingAssets" , (gpointer)& Object_CUSTOM_FindObjectsOfTypeIncludingAssets );
}

void Register_UnityEngine_Object_ToString()
{
    scripting_add_internal_call( "UnityEngine.Object::ToString" , (gpointer)& Object_CUSTOM_ToString );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineObject_IcallNames [] =
{
    "UnityEngine.Object::CompareBaseObjectsInternal",    // -> Object_CUSTOM_CompareBaseObjectsInternal
    "UnityEngine.Object::Internal_CloneSingle",    // -> Object_CUSTOM_Internal_CloneSingle
    "UnityEngine.Object::INTERNAL_CALL_Internal_InstantiateSingle",    // -> Object_CUSTOM_INTERNAL_CALL_Internal_InstantiateSingle
    "UnityEngine.Object::Destroy"           ,    // -> Object_CUSTOM_Destroy
    "UnityEngine.Object::DestroyImmediate"  ,    // -> Object_CUSTOM_DestroyImmediate
    "UnityEngine.Object::FindObjectsOfType" ,    // -> Object_CUSTOM_FindObjectsOfType
    "UnityEngine.Object::get_name"          ,    // -> Object_Get_Custom_PropName
    "UnityEngine.Object::set_name"          ,    // -> Object_Set_Custom_PropName
    "UnityEngine.Object::DontDestroyOnLoad" ,    // -> Object_CUSTOM_DontDestroyOnLoad
    "UnityEngine.Object::get_hideFlags"     ,    // -> Object_Get_Custom_PropHideFlags
    "UnityEngine.Object::set_hideFlags"     ,    // -> Object_Set_Custom_PropHideFlags
    "UnityEngine.Object::DestroyObject"     ,    // -> Object_CUSTOM_DestroyObject
    "UnityEngine.Object::FindSceneObjectsOfType",    // -> Object_CUSTOM_FindSceneObjectsOfType
    "UnityEngine.Object::FindObjectsOfTypeIncludingAssets",    // -> Object_CUSTOM_FindObjectsOfTypeIncludingAssets
    "UnityEngine.Object::ToString"          ,    // -> Object_CUSTOM_ToString
    NULL
};

static const void* s_UnityEngineObject_IcallFuncs [] =
{
    (const void*)&Object_CUSTOM_CompareBaseObjectsInternal,  //  <- UnityEngine.Object::CompareBaseObjectsInternal
    (const void*)&Object_CUSTOM_Internal_CloneSingle      ,  //  <- UnityEngine.Object::Internal_CloneSingle
    (const void*)&Object_CUSTOM_INTERNAL_CALL_Internal_InstantiateSingle,  //  <- UnityEngine.Object::INTERNAL_CALL_Internal_InstantiateSingle
    (const void*)&Object_CUSTOM_Destroy                   ,  //  <- UnityEngine.Object::Destroy
    (const void*)&Object_CUSTOM_DestroyImmediate          ,  //  <- UnityEngine.Object::DestroyImmediate
    (const void*)&Object_CUSTOM_FindObjectsOfType         ,  //  <- UnityEngine.Object::FindObjectsOfType
    (const void*)&Object_Get_Custom_PropName              ,  //  <- UnityEngine.Object::get_name
    (const void*)&Object_Set_Custom_PropName              ,  //  <- UnityEngine.Object::set_name
    (const void*)&Object_CUSTOM_DontDestroyOnLoad         ,  //  <- UnityEngine.Object::DontDestroyOnLoad
    (const void*)&Object_Get_Custom_PropHideFlags         ,  //  <- UnityEngine.Object::get_hideFlags
    (const void*)&Object_Set_Custom_PropHideFlags         ,  //  <- UnityEngine.Object::set_hideFlags
    (const void*)&Object_CUSTOM_DestroyObject             ,  //  <- UnityEngine.Object::DestroyObject
    (const void*)&Object_CUSTOM_FindSceneObjectsOfType    ,  //  <- UnityEngine.Object::FindSceneObjectsOfType
    (const void*)&Object_CUSTOM_FindObjectsOfTypeIncludingAssets,  //  <- UnityEngine.Object::FindObjectsOfTypeIncludingAssets
    (const void*)&Object_CUSTOM_ToString                  ,  //  <- UnityEngine.Object::ToString
    NULL
};

void ExportUnityEngineObjectBindings();
void ExportUnityEngineObjectBindings()
{
    for (int i = 0; s_UnityEngineObject_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineObject_IcallNames [i], s_UnityEngineObject_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineObjectBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Object_CUSTOM_CompareBaseObjectsInternal); //  <- UnityEngine.Object::CompareBaseObjectsInternal
    SET_METRO_BINDING(Object_CUSTOM_Internal_CloneSingle); //  <- UnityEngine.Object::Internal_CloneSingle
    SET_METRO_BINDING(Object_CUSTOM_INTERNAL_CALL_Internal_InstantiateSingle); //  <- UnityEngine.Object::INTERNAL_CALL_Internal_InstantiateSingle
    SET_METRO_BINDING(Object_CUSTOM_Destroy); //  <- UnityEngine.Object::Destroy
    SET_METRO_BINDING(Object_CUSTOM_DestroyImmediate); //  <- UnityEngine.Object::DestroyImmediate
    SET_METRO_BINDING(Object_CUSTOM_FindObjectsOfType); //  <- UnityEngine.Object::FindObjectsOfType
    SET_METRO_BINDING(Object_Get_Custom_PropName); //  <- UnityEngine.Object::get_name
    SET_METRO_BINDING(Object_Set_Custom_PropName); //  <- UnityEngine.Object::set_name
    SET_METRO_BINDING(Object_CUSTOM_DontDestroyOnLoad); //  <- UnityEngine.Object::DontDestroyOnLoad
    SET_METRO_BINDING(Object_Get_Custom_PropHideFlags); //  <- UnityEngine.Object::get_hideFlags
    SET_METRO_BINDING(Object_Set_Custom_PropHideFlags); //  <- UnityEngine.Object::set_hideFlags
    SET_METRO_BINDING(Object_CUSTOM_DestroyObject); //  <- UnityEngine.Object::DestroyObject
    SET_METRO_BINDING(Object_CUSTOM_FindSceneObjectsOfType); //  <- UnityEngine.Object::FindSceneObjectsOfType
    SET_METRO_BINDING(Object_CUSTOM_FindObjectsOfTypeIncludingAssets); //  <- UnityEngine.Object::FindObjectsOfTypeIncludingAssets
    SET_METRO_BINDING(Object_CUSTOM_ToString); //  <- UnityEngine.Object::ToString
}

#endif
