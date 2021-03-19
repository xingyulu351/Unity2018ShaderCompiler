#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/BaseClasses/Cursor.h"

using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Cursor_CUSTOM_INTERNAL_CALL_SetCursor(ICallType_ReadOnlyUnityEngineObject_Argument texture_, const Vector2fIcall& hotspot, CursorMode cursorMode)
{
    SCRIPTINGAPI_ETW_ENTRY(Cursor_CUSTOM_INTERNAL_CALL_SetCursor)
    ReadOnlyScriptingObjectOfType<Texture2D> texture(texture_);
    UNUSED(texture);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetCursor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetCursor)
    
    		Cursors::SetCursor (texture, hotspot, cursorMode);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Cursor_INTERNAL_CALL_SetCursor()
{
    scripting_add_internal_call( "UnityEngine.Cursor::INTERNAL_CALL_SetCursor" , (gpointer)& Cursor_CUSTOM_INTERNAL_CALL_SetCursor );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Cursor_IcallNames [] =
{
    "UnityEngine.Cursor::INTERNAL_CALL_SetCursor",    // -> Cursor_CUSTOM_INTERNAL_CALL_SetCursor
    NULL
};

static const void* s_Cursor_IcallFuncs [] =
{
    (const void*)&Cursor_CUSTOM_INTERNAL_CALL_SetCursor   ,  //  <- UnityEngine.Cursor::INTERNAL_CALL_SetCursor
    NULL
};

void ExportCursorBindings();
void ExportCursorBindings()
{
    for (int i = 0; s_Cursor_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Cursor_IcallNames [i], s_Cursor_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportCursorBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Cursor_CUSTOM_INTERNAL_CALL_SetCursor); //  <- UnityEngine.Cursor::INTERNAL_CALL_SetCursor
}

#endif
