#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/TextAsset.h"
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION TextAsset_Get_Custom_PropText(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextAsset_Get_Custom_PropText)
    ReadOnlyScriptingObjectOfType<TextAsset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_text)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_text)
    return CreateScriptingString(self->GetScript().c_str());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION TextAsset_Get_Custom_PropBytes(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(TextAsset_Get_Custom_PropBytes)
    ReadOnlyScriptingObjectOfType<TextAsset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bytes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bytes)
    return CreateScriptingArray( self->GetScript().c_str(), self->GetScript().size(), GetScriptingManager().GetCommonClasses().byte );
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_TextAsset_get_text()
{
    scripting_add_internal_call( "UnityEngine.TextAsset::get_text" , (gpointer)& TextAsset_Get_Custom_PropText );
}

void Register_UnityEngine_TextAsset_get_bytes()
{
    scripting_add_internal_call( "UnityEngine.TextAsset::get_bytes" , (gpointer)& TextAsset_Get_Custom_PropBytes );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_ScriptAssets_IcallNames [] =
{
    "UnityEngine.TextAsset::get_text"       ,    // -> TextAsset_Get_Custom_PropText
    "UnityEngine.TextAsset::get_bytes"      ,    // -> TextAsset_Get_Custom_PropBytes
    NULL
};

static const void* s_ScriptAssets_IcallFuncs [] =
{
    (const void*)&TextAsset_Get_Custom_PropText           ,  //  <- UnityEngine.TextAsset::get_text
    (const void*)&TextAsset_Get_Custom_PropBytes          ,  //  <- UnityEngine.TextAsset::get_bytes
    NULL
};

void ExportScriptAssetsBindings();
void ExportScriptAssetsBindings()
{
    for (int i = 0; s_ScriptAssets_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_ScriptAssets_IcallNames [i], s_ScriptAssets_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportScriptAssetsBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(TextAsset_Get_Custom_PropText); //  <- UnityEngine.TextAsset::get_text
    SET_METRO_BINDING(TextAsset_Get_Custom_PropBytes); //  <- UnityEngine.TextAsset::get_bytes
}

#endif
