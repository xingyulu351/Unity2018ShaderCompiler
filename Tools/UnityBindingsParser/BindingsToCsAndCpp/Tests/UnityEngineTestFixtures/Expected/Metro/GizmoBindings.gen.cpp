#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Math/Vector3.h"
#include "Runtime/Geometry/Ray.h"
#include "Runtime/Math/Rect.h"
#include "Runtime/Math/Matrix4x4.h"
#include "Runtime/Graphics/Texture.h"
#include "Runtime/Filters/Misc/Font.h"
#include "Runtime/Shaders/Material.h"
#include "Runtime/IMGUI/TextUtil.h"
#include "Runtime/Camera/RenderLayers/GUITexture.h"
#include "Runtime/Misc/InputEvent.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/ScriptingUtility.h"

#if UNITY_EDITOR
#include "Editor/Src/Gizmos/GizmoManager.h"
#include "Editor/Src/Gizmos/GizmoUtil.h"
#include "Editor/Src/Gizmos/GizmoRenderer.h"
#endif

using namespace Unity;

 


void PauseEditor ();
using namespace std;

 
	static void CheckGizmoDrawing ();
	static void CheckGizmoDrawing ()
	{
		#if UNITY_EDITOR
		if (!GizmoManager::Get ().IsDrawingGizmos ())
			RaiseMonoException ("Gizmo drawing functions can only be used in OnDrawGizmos and OnDrawGizmosSelected.");
		#endif
	}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawLine(const Vector3f& from, const Vector3f& to)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawLine)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawLine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawLine)
    
    		CheckGizmoDrawing ();
    		#if UNITY_EDITOR
    		DrawLine (from, to);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawWireSphere(const Vector3f& center, float radius)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawWireSphere)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawWireSphere)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawWireSphere)
    
    		#if UNITY_EDITOR
    		CheckGizmoDrawing ();
    		DrawWireSphere (center, radius);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawSphere(const Vector3f& center, float radius)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawSphere)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawSphere)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawSphere)
    
    		#if UNITY_EDITOR
    		CheckGizmoDrawing ();
    		DrawSphere (center, radius);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawWireCube(const Vector3f& center, const Vector3f& size)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawWireCube)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawWireCube)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawWireCube)
    
    		#if UNITY_EDITOR
    		CheckGizmoDrawing ();
    		DrawWireCube (center, size);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawCube(const Vector3f& center, const Vector3f& size)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawCube)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawCube)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawCube)
    
    		#if UNITY_EDITOR
    		CheckGizmoDrawing ();
    		DrawCube (center, size);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawIcon(const Vector3f& center, ICallType_String_Argument name_, ScriptingBool allowScaling)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawIcon)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawIcon)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawIcon)
    
    		#if UNITY_EDITOR
    		CheckGizmoDrawing ();
    		DrawIcon (center, name, allowScaling);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawGUITexture(const Rectf& screenRect, ICallType_ReadOnlyUnityEngineObject_Argument texture_, int leftBorder, int rightBorder, int topBorder, int bottomBorder, ICallType_ReadOnlyUnityEngineObject_Argument mat_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawGUITexture)
    ReadOnlyScriptingObjectOfType<Texture> texture(texture_);
    UNUSED(texture);
    ReadOnlyScriptingObjectOfType<Material> mat(mat_);
    UNUSED(mat);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawGUITexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawGUITexture)
    
    		#if UNITY_EDITOR
    		CheckGizmoDrawing ();
    		DrawGUITexture (screenRect, texture, leftBorder, rightBorder, topBorder, bottomBorder, ColorRGBA32(128,128,128,128), mat);
    		#endif	
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_get_color(ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_get_color)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    #if UNITY_EDITOR
    { *returnValue =(gizmos::g_GizmoColor); return;};
    #endif
    { *returnValue =(ColorRGBAf (1,1,1,1)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_set_color(const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_set_color)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
    
    #if UNITY_EDITOR
    gizmos::g_GizmoColor = value;
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_get_matrix(Matrix4x4f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_get_matrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_matrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_matrix)
    #if UNITY_EDITOR
    { *returnValue =(GetGizmoMatrix ()); return;};
    #endif
    { *returnValue =(Matrix4x4f::identity); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_set_matrix(const Matrix4x4f& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_set_matrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_matrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_matrix)
    
    #if UNITY_EDITOR
    SetGizmoMatrix (value);
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gizmos_CUSTOM_INTERNAL_CALL_DrawFrustum(const Vector3f& center, float fov, float maxRange, float minRange, float aspect)
{
    SCRIPTINGAPI_ETW_ENTRY(Gizmos_CUSTOM_INTERNAL_CALL_DrawFrustum)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DrawFrustum)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DrawFrustum)
    
    		#if UNITY_EDITOR
    		DrawFrustum (center, fov, maxRange, minRange, aspect);
    		#endif
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawLine()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawLine" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawLine );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawWireSphere()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawWireSphere" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawWireSphere );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawSphere()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawSphere" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawSphere );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawWireCube()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawWireCube" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawWireCube );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawCube()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawCube" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawCube );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawIcon()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawIcon" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawIcon );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawGUITexture()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawGUITexture" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawGUITexture );
}

void Register_UnityEngine_Gizmos_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_get_color" , (gpointer)& Gizmos_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_Gizmos_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_set_color" , (gpointer)& Gizmos_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_Gizmos_INTERNAL_get_matrix()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_get_matrix" , (gpointer)& Gizmos_CUSTOM_INTERNAL_get_matrix );
}

void Register_UnityEngine_Gizmos_INTERNAL_set_matrix()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_set_matrix" , (gpointer)& Gizmos_CUSTOM_INTERNAL_set_matrix );
}

void Register_UnityEngine_Gizmos_INTERNAL_CALL_DrawFrustum()
{
    scripting_add_internal_call( "UnityEngine.Gizmos::INTERNAL_CALL_DrawFrustum" , (gpointer)& Gizmos_CUSTOM_INTERNAL_CALL_DrawFrustum );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Gizmo_IcallNames [] =
{
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawLine",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawLine
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawWireSphere",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawWireSphere
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawSphere",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawSphere
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawWireCube",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawWireCube
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawCube",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawCube
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawIcon",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawIcon
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawGUITexture",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawGUITexture
    "UnityEngine.Gizmos::INTERNAL_get_color",    // -> Gizmos_CUSTOM_INTERNAL_get_color
    "UnityEngine.Gizmos::INTERNAL_set_color",    // -> Gizmos_CUSTOM_INTERNAL_set_color
    "UnityEngine.Gizmos::INTERNAL_get_matrix",    // -> Gizmos_CUSTOM_INTERNAL_get_matrix
    "UnityEngine.Gizmos::INTERNAL_set_matrix",    // -> Gizmos_CUSTOM_INTERNAL_set_matrix
    "UnityEngine.Gizmos::INTERNAL_CALL_DrawFrustum",    // -> Gizmos_CUSTOM_INTERNAL_CALL_DrawFrustum
    NULL
};

static const void* s_Gizmo_IcallFuncs [] =
{
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawLine    ,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawLine
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawWireSphere,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawWireSphere
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawSphere  ,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawSphere
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawWireCube,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawWireCube
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawCube    ,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawCube
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawIcon    ,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawIcon
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawGUITexture,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawGUITexture
    (const void*)&Gizmos_CUSTOM_INTERNAL_get_color        ,  //  <- UnityEngine.Gizmos::INTERNAL_get_color
    (const void*)&Gizmos_CUSTOM_INTERNAL_set_color        ,  //  <- UnityEngine.Gizmos::INTERNAL_set_color
    (const void*)&Gizmos_CUSTOM_INTERNAL_get_matrix       ,  //  <- UnityEngine.Gizmos::INTERNAL_get_matrix
    (const void*)&Gizmos_CUSTOM_INTERNAL_set_matrix       ,  //  <- UnityEngine.Gizmos::INTERNAL_set_matrix
    (const void*)&Gizmos_CUSTOM_INTERNAL_CALL_DrawFrustum ,  //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawFrustum
    NULL
};

void ExportGizmoBindings();
void ExportGizmoBindings()
{
    for (int i = 0; s_Gizmo_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Gizmo_IcallNames [i], s_Gizmo_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportGizmoBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawLine); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawLine
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawWireSphere); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawWireSphere
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawSphere); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawSphere
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawWireCube); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawWireCube
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawCube); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawCube
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawIcon); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawIcon
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawGUITexture); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawGUITexture
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.Gizmos::INTERNAL_get_color
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.Gizmos::INTERNAL_set_color
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_get_matrix); //  <- UnityEngine.Gizmos::INTERNAL_get_matrix
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_set_matrix); //  <- UnityEngine.Gizmos::INTERNAL_set_matrix
    SET_METRO_BINDING(Gizmos_CUSTOM_INTERNAL_CALL_DrawFrustum); //  <- UnityEngine.Gizmos::INTERNAL_CALL_DrawFrustum
}

#endif
