#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Graphics/RenderTexture.h"
#include "Runtime/Graphics/DisplayManager.h"
#if !UNITY_FLASH
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Display_CUSTOM_GetSystemExtImpl(ICallType_IntPtr_Argument nativeDisplay_, int* w, int* h)
{
    SCRIPTINGAPI_ETW_ENTRY(Display_CUSTOM_GetSystemExtImpl)
    void* nativeDisplay(nativeDisplay_);
    UNUSED(nativeDisplay);
    SCRIPTINGAPI_STACK_CHECK(GetSystemExtImpl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetSystemExtImpl)
    
            UnityDisplayManager_DisplaySystemResolution(nativeDisplay,w,h);
        
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Display_CUSTOM_GetRenderingExtImpl(ICallType_IntPtr_Argument nativeDisplay_, int* w, int* h)
{
    SCRIPTINGAPI_ETW_ENTRY(Display_CUSTOM_GetRenderingExtImpl)
    void* nativeDisplay(nativeDisplay_);
    UNUSED(nativeDisplay);
    SCRIPTINGAPI_STACK_CHECK(GetRenderingExtImpl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetRenderingExtImpl)
    
            UnityDisplayManager_DisplaySystemResolution(nativeDisplay,w,h);
        
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Display_CUSTOM_GetRenderingBuffersImpl(ICallType_IntPtr_Argument nativeDisplay_, ScriptingRenderBuffer* color, ScriptingRenderBuffer* depth)
{
    SCRIPTINGAPI_ETW_ENTRY(Display_CUSTOM_GetRenderingBuffersImpl)
    void* nativeDisplay(nativeDisplay_);
    UNUSED(nativeDisplay);
    SCRIPTINGAPI_STACK_CHECK(GetRenderingBuffersImpl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetRenderingBuffersImpl)
    
            UnityDisplayManager_DisplayRenderingBuffers(nativeDisplay, &color->m_BufferPtr, &depth->m_BufferPtr);
        
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Display_CUSTOM_SetRenderingResolutionImpl(ICallType_IntPtr_Argument nativeDisplay_, int w, int h)
{
    SCRIPTINGAPI_ETW_ENTRY(Display_CUSTOM_SetRenderingResolutionImpl)
    void* nativeDisplay(nativeDisplay_);
    UNUSED(nativeDisplay);
    SCRIPTINGAPI_STACK_CHECK(SetRenderingResolutionImpl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetRenderingResolutionImpl)
    
            UnityDisplayManager_SetRenderingResolution(nativeDisplay,w,h);
        
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if !UNITY_FLASH
void Register_UnityEngine_Display_GetSystemExtImpl()
{
    scripting_add_internal_call( "UnityEngine.Display::GetSystemExtImpl" , (gpointer)& Display_CUSTOM_GetSystemExtImpl );
}

void Register_UnityEngine_Display_GetRenderingExtImpl()
{
    scripting_add_internal_call( "UnityEngine.Display::GetRenderingExtImpl" , (gpointer)& Display_CUSTOM_GetRenderingExtImpl );
}

void Register_UnityEngine_Display_GetRenderingBuffersImpl()
{
    scripting_add_internal_call( "UnityEngine.Display::GetRenderingBuffersImpl" , (gpointer)& Display_CUSTOM_GetRenderingBuffersImpl );
}

void Register_UnityEngine_Display_SetRenderingResolutionImpl()
{
    scripting_add_internal_call( "UnityEngine.Display::SetRenderingResolutionImpl" , (gpointer)& Display_CUSTOM_SetRenderingResolutionImpl );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineDisplay_IcallNames [] =
{
#if !UNITY_FLASH
    "UnityEngine.Display::GetSystemExtImpl" ,    // -> Display_CUSTOM_GetSystemExtImpl
    "UnityEngine.Display::GetRenderingExtImpl",    // -> Display_CUSTOM_GetRenderingExtImpl
    "UnityEngine.Display::GetRenderingBuffersImpl",    // -> Display_CUSTOM_GetRenderingBuffersImpl
    "UnityEngine.Display::SetRenderingResolutionImpl",    // -> Display_CUSTOM_SetRenderingResolutionImpl
#endif
    NULL
};

static const void* s_UnityEngineDisplay_IcallFuncs [] =
{
#if !UNITY_FLASH
    (const void*)&Display_CUSTOM_GetSystemExtImpl         ,  //  <- UnityEngine.Display::GetSystemExtImpl
    (const void*)&Display_CUSTOM_GetRenderingExtImpl      ,  //  <- UnityEngine.Display::GetRenderingExtImpl
    (const void*)&Display_CUSTOM_GetRenderingBuffersImpl  ,  //  <- UnityEngine.Display::GetRenderingBuffersImpl
    (const void*)&Display_CUSTOM_SetRenderingResolutionImpl,  //  <- UnityEngine.Display::SetRenderingResolutionImpl
#endif
    NULL
};

void ExportUnityEngineDisplayBindings();
void ExportUnityEngineDisplayBindings()
{
    for (int i = 0; s_UnityEngineDisplay_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineDisplay_IcallNames [i], s_UnityEngineDisplay_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineDisplayBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if !UNITY_FLASH
    SET_METRO_BINDING(Display_CUSTOM_GetSystemExtImpl); //  <- UnityEngine.Display::GetSystemExtImpl
    SET_METRO_BINDING(Display_CUSTOM_GetRenderingExtImpl); //  <- UnityEngine.Display::GetRenderingExtImpl
    SET_METRO_BINDING(Display_CUSTOM_GetRenderingBuffersImpl); //  <- UnityEngine.Display::GetRenderingBuffersImpl
    SET_METRO_BINDING(Display_CUSTOM_SetRenderingResolutionImpl); //  <- UnityEngine.Display::SetRenderingResolutionImpl
#endif
}

#endif
