#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#if ENABLE_SPRITES
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Filters/Mesh/SpriteRenderer.h"
#include "Runtime/Graphics/SpriteFrame.h"
#include "Runtime/Graphics/Texture2D.h"
#endif 

#include "Editor/Src/AssetPipeline/SpriteFrameMetaData.h"
#if ENABLE_SPRITES
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpriteFrame_CUSTOM_Internal_GetSize(ICallType_ReadOnlyUnityEngineObject_Argument spriteFrame_, Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteFrame_CUSTOM_Internal_GetSize)
    ReadOnlyScriptingObjectOfType<SpriteFrame> spriteFrame(spriteFrame_);
    UNUSED(spriteFrame);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetSize)
    
    		SpriteFrame& sf = *spriteFrame;
    		output->x = sf.GetSize().x/kTexturePixelsPerUnit;
    		output->y = sf.GetSize().y/kTexturePixelsPerUnit;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpriteFrame_CUSTOM_GenerateMesh(ICallType_ReadOnlyUnityEngineObject_Argument self_, float quality, UInt8 alphaTolerance, ScriptingBool holeDetection, UInt32 extrude)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteFrame_CUSTOM_GenerateMesh)
    ReadOnlyScriptingObjectOfType<SpriteFrame> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GenerateMesh)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GenerateMesh)
    
    		quality = clamp(1.0f - quality, 0.0f, 1.0f); 
    		self->GenerateMesh(quality, alphaTolerance, holeDetection, extrude);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpriteFrame_CUSTOM_INTERNAL_get_bounds(ICallType_ReadOnlyUnityEngineObject_Argument self_, AABB* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteFrame_CUSTOM_INTERNAL_get_bounds)
    ReadOnlyScriptingObjectOfType<SpriteFrame> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_bounds)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_bounds)
    { *returnValue =(self->GetBounds()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpriteFrame_CUSTOM_INTERNAL_get_uv(ICallType_ReadOnlyUnityEngineObject_Argument self_, Vector4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteFrame_CUSTOM_INTERNAL_get_uv)
    ReadOnlyScriptingObjectOfType<SpriteFrame> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_uv)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_uv)
    { *returnValue =(self->GetRD().uv); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpriteFrame_CUSTOM_INTERNAL_get_rect(ICallType_ReadOnlyUnityEngineObject_Argument self_, Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteFrame_CUSTOM_INTERNAL_get_rect)
    ReadOnlyScriptingObjectOfType<SpriteFrame> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rect)
    PPtr<Texture2D> texture = self->GetRD().GetTexture();
    int width = texture->GetDataWidth();
    int height = texture->GetDataHeight();
    const Vector4f& uv = self->GetRD().uv;
    { *returnValue =(Rectf(uv.x * width, (1.0 - uv.y) * height, (uv.z - uv.x) * width, (uv.y - uv.w) * height)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION SpriteFrame_Get_Custom_PropMaterial(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteFrame_Get_Custom_PropMaterial)
    ReadOnlyScriptingObjectOfType<SpriteFrame> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_material)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_material)
    return Scripting::ScriptingWrapperFor(self->GetRD().material);
}

SCRIPT_BINDINGS_EXPORT_DECL
SpriteFrame SCRIPT_CALL_CONVENTION SpriteRenderer_CUSTOM_GetSpriteFrame_INTERNAL(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteRenderer_CUSTOM_GetSpriteFrame_INTERNAL)
    ReadOnlyScriptingObjectOfType<SpriteRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetSpriteFrame_INTERNAL)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetSpriteFrame_INTERNAL)
    
    		return Scripting::ScriptingWrapperFor(self->GetSpriteFrame());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SpriteRenderer_CUSTOM_SetSpriteFrame_INTERNAL(ICallType_ReadOnlyUnityEngineObject_Argument self_, SpriteFrame spriteFrame)
{
    SCRIPTINGAPI_ETW_ENTRY(SpriteRenderer_CUSTOM_SetSpriteFrame_INTERNAL)
    ReadOnlyScriptingObjectOfType<SpriteRenderer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetSpriteFrame_INTERNAL)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetSpriteFrame_INTERNAL)
    
    		self->SetSpriteFrame(spriteFrame);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_SPRITES
void Register_UnityEngine_SpriteFrame_Internal_GetSize()
{
    scripting_add_internal_call( "UnityEngine.SpriteFrame::Internal_GetSize" , (gpointer)& SpriteFrame_CUSTOM_Internal_GetSize );
}

void Register_UnityEngine_SpriteFrame_GenerateMesh()
{
    scripting_add_internal_call( "UnityEngine.SpriteFrame::GenerateMesh" , (gpointer)& SpriteFrame_CUSTOM_GenerateMesh );
}

void Register_UnityEngine_SpriteFrame_INTERNAL_get_bounds()
{
    scripting_add_internal_call( "UnityEngine.SpriteFrame::INTERNAL_get_bounds" , (gpointer)& SpriteFrame_CUSTOM_INTERNAL_get_bounds );
}

void Register_UnityEngine_SpriteFrame_INTERNAL_get_uv()
{
    scripting_add_internal_call( "UnityEngine.SpriteFrame::INTERNAL_get_uv" , (gpointer)& SpriteFrame_CUSTOM_INTERNAL_get_uv );
}

void Register_UnityEngine_SpriteFrame_INTERNAL_get_rect()
{
    scripting_add_internal_call( "UnityEngine.SpriteFrame::INTERNAL_get_rect" , (gpointer)& SpriteFrame_CUSTOM_INTERNAL_get_rect );
}

void Register_UnityEngine_SpriteFrame_get_material()
{
    scripting_add_internal_call( "UnityEngine.SpriteFrame::get_material" , (gpointer)& SpriteFrame_Get_Custom_PropMaterial );
}

void Register_UnityEngine_SpriteRenderer_GetSpriteFrame_INTERNAL()
{
    scripting_add_internal_call( "UnityEngine.SpriteRenderer::GetSpriteFrame_INTERNAL" , (gpointer)& SpriteRenderer_CUSTOM_GetSpriteFrame_INTERNAL );
}

void Register_UnityEngine_SpriteRenderer_SetSpriteFrame_INTERNAL()
{
    scripting_add_internal_call( "UnityEngine.SpriteRenderer::SetSpriteFrame_INTERNAL" , (gpointer)& SpriteRenderer_CUSTOM_SetSpriteFrame_INTERNAL );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Sprites_IcallNames [] =
{
#if ENABLE_SPRITES
    "UnityEngine.SpriteFrame::Internal_GetSize",    // -> SpriteFrame_CUSTOM_Internal_GetSize
    "UnityEngine.SpriteFrame::GenerateMesh" ,    // -> SpriteFrame_CUSTOM_GenerateMesh
    "UnityEngine.SpriteFrame::INTERNAL_get_bounds",    // -> SpriteFrame_CUSTOM_INTERNAL_get_bounds
    "UnityEngine.SpriteFrame::INTERNAL_get_uv",    // -> SpriteFrame_CUSTOM_INTERNAL_get_uv
    "UnityEngine.SpriteFrame::INTERNAL_get_rect",    // -> SpriteFrame_CUSTOM_INTERNAL_get_rect
    "UnityEngine.SpriteFrame::get_material" ,    // -> SpriteFrame_Get_Custom_PropMaterial
    "UnityEngine.SpriteRenderer::GetSpriteFrame_INTERNAL",    // -> SpriteRenderer_CUSTOM_GetSpriteFrame_INTERNAL
    "UnityEngine.SpriteRenderer::SetSpriteFrame_INTERNAL",    // -> SpriteRenderer_CUSTOM_SetSpriteFrame_INTERNAL
#endif
    NULL
};

static const void* s_Sprites_IcallFuncs [] =
{
#if ENABLE_SPRITES
    (const void*)&SpriteFrame_CUSTOM_Internal_GetSize     ,  //  <- UnityEngine.SpriteFrame::Internal_GetSize
    (const void*)&SpriteFrame_CUSTOM_GenerateMesh         ,  //  <- UnityEngine.SpriteFrame::GenerateMesh
    (const void*)&SpriteFrame_CUSTOM_INTERNAL_get_bounds  ,  //  <- UnityEngine.SpriteFrame::INTERNAL_get_bounds
    (const void*)&SpriteFrame_CUSTOM_INTERNAL_get_uv      ,  //  <- UnityEngine.SpriteFrame::INTERNAL_get_uv
    (const void*)&SpriteFrame_CUSTOM_INTERNAL_get_rect    ,  //  <- UnityEngine.SpriteFrame::INTERNAL_get_rect
    (const void*)&SpriteFrame_Get_Custom_PropMaterial     ,  //  <- UnityEngine.SpriteFrame::get_material
    (const void*)&SpriteRenderer_CUSTOM_GetSpriteFrame_INTERNAL,  //  <- UnityEngine.SpriteRenderer::GetSpriteFrame_INTERNAL
    (const void*)&SpriteRenderer_CUSTOM_SetSpriteFrame_INTERNAL,  //  <- UnityEngine.SpriteRenderer::SetSpriteFrame_INTERNAL
#endif
    NULL
};

void ExportSpritesBindings();
void ExportSpritesBindings()
{
    for (int i = 0; s_Sprites_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Sprites_IcallNames [i], s_Sprites_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportSpritesBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_SPRITES
    SET_METRO_BINDING(SpriteFrame_CUSTOM_Internal_GetSize); //  <- UnityEngine.SpriteFrame::Internal_GetSize
    SET_METRO_BINDING(SpriteFrame_CUSTOM_GenerateMesh); //  <- UnityEngine.SpriteFrame::GenerateMesh
    SET_METRO_BINDING(SpriteFrame_CUSTOM_INTERNAL_get_bounds); //  <- UnityEngine.SpriteFrame::INTERNAL_get_bounds
    SET_METRO_BINDING(SpriteFrame_CUSTOM_INTERNAL_get_uv); //  <- UnityEngine.SpriteFrame::INTERNAL_get_uv
    SET_METRO_BINDING(SpriteFrame_CUSTOM_INTERNAL_get_rect); //  <- UnityEngine.SpriteFrame::INTERNAL_get_rect
    SET_METRO_BINDING(SpriteFrame_Get_Custom_PropMaterial); //  <- UnityEngine.SpriteFrame::get_material
    SET_METRO_BINDING(SpriteRenderer_CUSTOM_GetSpriteFrame_INTERNAL); //  <- UnityEngine.SpriteRenderer::GetSpriteFrame_INTERNAL
    SET_METRO_BINDING(SpriteRenderer_CUSTOM_SetSpriteFrame_INTERNAL); //  <- UnityEngine.SpriteRenderer::SetSpriteFrame_INTERNAL
#endif
}

#endif
