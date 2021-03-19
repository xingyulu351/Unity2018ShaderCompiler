#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/IMGUI/GUIStyle.h"
#include "Runtime/Filters/Misc/Font.h"
#include "Runtime/Misc/InputEvent.h"
#include "Runtime/GfxDevice/GfxDevice.h"
#include "Runtime/Camera/CameraUtil.h"
#include "Runtime/IMGUI/GUIManager.h"
#include "Runtime/Utilities/CopyPaste.h"
#include "Runtime/IMGUI/GUIState.h"
#include "Runtime/IMGUI/GUIClip.h"
#include "Runtime/IMGUI/IMGUIUtils.h"
#include "Runtime/Input/InputManager.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#if UNITY_EDITOR

#include "Editor/Src/EditorHelper.h"
#include "Editor/Mono/MonoEditorUtility.h"
#include "Editor/Src/OptimizedGUIBlock.h"
#include "Editor/Src/AssetPipeline/AssetDatabase.h"
#include "Editor/Src/Undo.h"

extern float s_GUIStyleIconSizeX;
extern float s_GUIStyleIconSizeY;

#include "Editor/Src/EditorResources.h"
#endif

using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_GetControlID(int hint, FocusType focus)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_GetControlID)
    SCRIPTINGAPI_STACK_CHECK(GetControlID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetControlID)
    
    		return GetGUIState().GetControlID (hint, focus);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_INTERNAL_CALL_Internal_GetNextControlID2(int hint, FocusType focusType, const Rectf& rect)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_INTERNAL_CALL_Internal_GetNextControlID2)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_GetNextControlID2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_GetNextControlID2)
    
    		return GetGUIState().GetControlID (hint, focusType, rect);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_GetPermanentControlID()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_GetPermanentControlID)
    SCRIPTINGAPI_STACK_CHECK(GetPermanentControlID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPermanentControlID)
    
    		return GetGUIState().m_EternalGUIState->GetNextUniqueID ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_Internal_GetHotControl()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_Internal_GetHotControl)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetHotControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetHotControl)
    
    		return IMGUI::GetHotControl (GetGUIState());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_Internal_SetHotControl(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_Internal_SetHotControl)
    SCRIPTINGAPI_STACK_CHECK(Internal_SetHotControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetHotControl)
    
    		IMGUI::SetHotControl (GetGUIState(), value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_UpdateUndoName()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_UpdateUndoName)
    SCRIPTINGAPI_STACK_CHECK(UpdateUndoName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UpdateUndoName)
    
    		#if UNITY_EDITOR
    		GetUndoManager().UpdateUndoName ();
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIUtility_Get_Custom_PropKeyboardControl()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Get_Custom_PropKeyboardControl)
    SCRIPTINGAPI_STACK_CHECK(get_keyboardControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_keyboardControl)
    return IMGUI::GetKeyboardControl (GetGUIState());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_Set_Custom_PropKeyboardControl(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Set_Custom_PropKeyboardControl)
    SCRIPTINGAPI_STACK_CHECK(set_keyboardControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_keyboardControl)
     IMGUI::SetKeyboardControl (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_SetDidGUIWindowsEatLastEvent(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_SetDidGUIWindowsEatLastEvent)
    SCRIPTINGAPI_STACK_CHECK(SetDidGUIWindowsEatLastEvent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetDidGUIWindowsEatLastEvent)
    
    		GUIManager::SetDidGUIWindowsEatLastEvent (value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GUIUtility_Get_Custom_PropSystemCopyBuffer()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Get_Custom_PropSystemCopyBuffer)
    SCRIPTINGAPI_STACK_CHECK(get_systemCopyBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_systemCopyBuffer)
    return CreateScriptingString (GetCopyBuffer ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_Set_Custom_PropSystemCopyBuffer(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Set_Custom_PropSystemCopyBuffer)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_systemCopyBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_systemCopyBuffer)
     SetCopyBuffer (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_Internal_LoadSkin(int skinNo, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_Internal_LoadSkin)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadSkin)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadSkin)
    
    		static PPtr<MonoBehaviour> skinObject[3] = { NULL, NULL, NULL };
    
    		#if UNITY_EDITOR
    			if (!skinObject[0] || !skinObject[1] || !skinObject[2])
    			{
    				if (IsDeveloperBuild ()) 
    				{
    					skinObject[0] = dynamic_pptr_cast<MonoBehaviour*> (GetMainAsset ("Assets/DefaultResources/GameSkin/GameSkin.GUISkin"));
    					skinObject[1] = dynamic_pptr_cast<MonoBehaviour*> (GetMainAsset (AppendPathName ("Assets/Editor Default Resources/", EditorResources::kLightSkinPath)));
    					skinObject[2] = dynamic_pptr_cast<MonoBehaviour*> (GetMainAsset (AppendPathName ("Assets/Editor Default Resources/", EditorResources::kDarkSkinPath)));
    				}
    				
    				if (!skinObject[0]) 
    				{
    					Object *obj = GetBuiltinResourceManager ().GetResource (ClassID(MonoBehaviour), "GameSkin/GameSkin.guiskin");
    					skinObject[0] = static_cast<MonoBehaviour*> (obj);
    				}
    				
    				if (!skinObject[1])
    					skinObject[1] = GetEditorAssetBundle()->Get<MonoBehaviour>(EditorResources::kLightSkinPath);
    				
    				if (!skinObject[2])
    					skinObject[2] = GetEditorAssetBundle()->Get<MonoBehaviour>(EditorResources::kDarkSkinPath);
    			}
    			
    			if (skinNo == 1 && GetEditorResources().GetSkinIdx () == 1) 
    				skinNo = 2;
    		#else
    			if (!skinObject[0]) 
    			{
    				Object *obj = GetBuiltinResourceManager ().GetResource (ClassID(MonoBehaviour), "GameSkin/GameSkin.guiskin");
    				skinObject[0] = static_cast<MonoBehaviour*> (obj);
    			}
    		#endif
    		return skinObject[skinNo]->GetInstance ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_Internal_ExitGUI()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_Internal_ExitGUI)
    SCRIPTINGAPI_STACK_CHECK(Internal_ExitGUI)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ExitGUI)
    
    		#if UNITY_EDITOR	
    		OptimizedGUIBlock::Abandon ();
    		s_GUIStyleIconSizeX = 0.0f;
    		s_GUIStyleIconSizeY = 0.0f;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIUtility_CUSTOM_Internal_GetGUIDepth()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_CUSTOM_Internal_GetGUIDepth)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetGUIDepth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetGUIDepth)
    
    		return GetGUIState().m_OnGUIDepth;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIUtility_Get_Custom_PropMouseUsed()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Get_Custom_PropMouseUsed)
    SCRIPTINGAPI_STACK_CHECK(get_mouseUsed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mouseUsed)
    return GetGUIState().m_CanvasGUIState.m_IsMouseUsed != 0;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_Set_Custom_PropMouseUsed(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Set_Custom_PropMouseUsed)
    SCRIPTINGAPI_STACK_CHECK(set_mouseUsed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mouseUsed)
     GetGUIState().m_CanvasGUIState.m_IsMouseUsed = value ? 1 : 0; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIUtility_Get_Custom_PropTextFieldInput()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Get_Custom_PropTextFieldInput)
    SCRIPTINGAPI_STACK_CHECK(get_textFieldInput)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_textFieldInput)
    return GetInputManager().GetTextFieldInput ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIUtility_Set_Custom_PropTextFieldInput(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIUtility_Set_Custom_PropTextFieldInput)
    SCRIPTINGAPI_STACK_CHECK(set_textFieldInput)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_textFieldInput)
     GetInputManager().SetTextFieldInput (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_Push(const Rectf& screenRect, const Vector2fIcall& scrollOffset, const Vector2fIcall& renderOffset, ScriptingBool resetOffset)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_Push)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Push)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Push)
    
    		GetGUIState().m_CanvasGUIState.m_GUIClipState.Push (IMGUI::GetCurrentEvent(GetGUIState()), screenRect, scrollOffset, renderOffset, resetOffset);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_Pop()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_Pop)
    SCRIPTINGAPI_STACK_CHECK(Pop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Pop)
    
    		GetGUIState().m_CanvasGUIState.m_GUIClipState.Pop (IMGUI::GetCurrentEvent(GetGUIState()));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_GetTopRect(Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_GetTopRect)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetTopRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetTopRect)
    
    		{ returnValue =(GetGUIState().m_CanvasGUIState.m_GUIClipState.GetTopRect ()); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIClip_Get_Custom_PropEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_Get_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return GetGUIState().m_CanvasGUIState.m_GUIClipState.GetEnabled();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Vector2(Vector2fIcall& pos)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Vector2)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Unclip_Vector2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Unclip_Vector2)
    
    		pos = GetGUIState().m_CanvasGUIState.m_GUIClipState.Unclip (pos);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_get_topmostRect(Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_get_topmostRect)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_topmostRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_topmostRect)
    { *returnValue =(GetGUIState().m_CanvasGUIState.m_GUIClipState.GetTopMostPhysicalRect ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Rect(Rectf& rect)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Rect)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Unclip_Rect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Unclip_Rect)
    
    		rect = GetGUIState().m_CanvasGUIState.m_GUIClipState.Unclip(rect);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_Clip_Vector2(Vector2fIcall& absolutePos)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_Clip_Vector2)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Clip_Vector2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Clip_Vector2)
    
    		absolutePos = GetGUIState().m_CanvasGUIState.m_GUIClipState.Clip(absolutePos);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_Internal_Clip_Rect(Rectf& absoluteRect)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_Internal_Clip_Rect)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Clip_Rect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Clip_Rect)
    
    		absoluteRect = GetGUIState().m_CanvasGUIState.m_GUIClipState.Clip (absoluteRect);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_Reapply()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_Reapply)
    SCRIPTINGAPI_STACK_CHECK(Reapply)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Reapply)
    
    		GetGUIState().m_CanvasGUIState.m_GUIClipState.Reapply (IMGUI::GetCurrentEvent(GetGUIState()));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_GetMatrix(Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_GetMatrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetMatrix)
     
    		{ returnValue =(GetGUIState().m_CanvasGUIState.m_GUIClipState.GetMatrix()); return; } 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_CALL_SetMatrix(const Matrix4x4f& m)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_CALL_SetMatrix)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetMatrix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetMatrix)
    
    		GetGUIState().m_CanvasGUIState.m_GUIClipState.SetMatrix(IMGUI::GetCurrentEvent(GetGUIState()), m); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_INTERNAL_get_visibleRect(Rectf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_INTERNAL_get_visibleRect)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_visibleRect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_visibleRect)
    { *returnValue =(GetGUIState().m_CanvasGUIState.m_GUIClipState.GetVisibleRect ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIClip_CUSTOM_Internal_GetAbsoluteMousePosition(Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIClip_CUSTOM_Internal_GetAbsoluteMousePosition)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetAbsoluteMousePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetAbsoluteMousePosition)
    
    		*output = GetGUIState().m_CanvasGUIState.m_GUIClipState.GetAbsoluteMousePosition();
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_GUIUtility_GetControlID()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::GetControlID" , (gpointer)& GUIUtility_CUSTOM_GetControlID );
}

void Register_UnityEngine_GUIUtility_INTERNAL_CALL_Internal_GetNextControlID2()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::INTERNAL_CALL_Internal_GetNextControlID2" , (gpointer)& GUIUtility_CUSTOM_INTERNAL_CALL_Internal_GetNextControlID2 );
}

void Register_UnityEngine_GUIUtility_GetPermanentControlID()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::GetPermanentControlID" , (gpointer)& GUIUtility_CUSTOM_GetPermanentControlID );
}

void Register_UnityEngine_GUIUtility_Internal_GetHotControl()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::Internal_GetHotControl" , (gpointer)& GUIUtility_CUSTOM_Internal_GetHotControl );
}

void Register_UnityEngine_GUIUtility_Internal_SetHotControl()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::Internal_SetHotControl" , (gpointer)& GUIUtility_CUSTOM_Internal_SetHotControl );
}

void Register_UnityEngine_GUIUtility_UpdateUndoName()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::UpdateUndoName" , (gpointer)& GUIUtility_CUSTOM_UpdateUndoName );
}

void Register_UnityEngine_GUIUtility_get_keyboardControl()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::get_keyboardControl" , (gpointer)& GUIUtility_Get_Custom_PropKeyboardControl );
}

void Register_UnityEngine_GUIUtility_set_keyboardControl()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::set_keyboardControl" , (gpointer)& GUIUtility_Set_Custom_PropKeyboardControl );
}

void Register_UnityEngine_GUIUtility_SetDidGUIWindowsEatLastEvent()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::SetDidGUIWindowsEatLastEvent" , (gpointer)& GUIUtility_CUSTOM_SetDidGUIWindowsEatLastEvent );
}

void Register_UnityEngine_GUIUtility_get_systemCopyBuffer()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::get_systemCopyBuffer" , (gpointer)& GUIUtility_Get_Custom_PropSystemCopyBuffer );
}

void Register_UnityEngine_GUIUtility_set_systemCopyBuffer()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::set_systemCopyBuffer" , (gpointer)& GUIUtility_Set_Custom_PropSystemCopyBuffer );
}

void Register_UnityEngine_GUIUtility_Internal_LoadSkin()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::Internal_LoadSkin" , (gpointer)& GUIUtility_CUSTOM_Internal_LoadSkin );
}

void Register_UnityEngine_GUIUtility_Internal_ExitGUI()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::Internal_ExitGUI" , (gpointer)& GUIUtility_CUSTOM_Internal_ExitGUI );
}

void Register_UnityEngine_GUIUtility_Internal_GetGUIDepth()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::Internal_GetGUIDepth" , (gpointer)& GUIUtility_CUSTOM_Internal_GetGUIDepth );
}

void Register_UnityEngine_GUIUtility_get_mouseUsed()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::get_mouseUsed" , (gpointer)& GUIUtility_Get_Custom_PropMouseUsed );
}

void Register_UnityEngine_GUIUtility_set_mouseUsed()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::set_mouseUsed" , (gpointer)& GUIUtility_Set_Custom_PropMouseUsed );
}

void Register_UnityEngine_GUIUtility_get_textFieldInput()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::get_textFieldInput" , (gpointer)& GUIUtility_Get_Custom_PropTextFieldInput );
}

void Register_UnityEngine_GUIUtility_set_textFieldInput()
{
    scripting_add_internal_call( "UnityEngine.GUIUtility::set_textFieldInput" , (gpointer)& GUIUtility_Set_Custom_PropTextFieldInput );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_Push()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_Push" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_Push );
}

void Register_UnityEngine_GUIClip_Pop()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::Pop" , (gpointer)& GUIClip_CUSTOM_Pop );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_GetTopRect()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_GetTopRect" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_GetTopRect );
}

void Register_UnityEngine_GUIClip_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::get_enabled" , (gpointer)& GUIClip_Get_Custom_PropEnabled );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_Unclip_Vector2()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Vector2" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Vector2 );
}

void Register_UnityEngine_GUIClip_INTERNAL_get_topmostRect()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_get_topmostRect" , (gpointer)& GUIClip_CUSTOM_INTERNAL_get_topmostRect );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_Unclip_Rect()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Rect" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Rect );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_Clip_Vector2()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_Clip_Vector2" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_Clip_Vector2 );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_Internal_Clip_Rect()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_Internal_Clip_Rect" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_Internal_Clip_Rect );
}

void Register_UnityEngine_GUIClip_Reapply()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::Reapply" , (gpointer)& GUIClip_CUSTOM_Reapply );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_GetMatrix()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_GetMatrix" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_GetMatrix );
}

void Register_UnityEngine_GUIClip_INTERNAL_CALL_SetMatrix()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_CALL_SetMatrix" , (gpointer)& GUIClip_CUSTOM_INTERNAL_CALL_SetMatrix );
}

void Register_UnityEngine_GUIClip_INTERNAL_get_visibleRect()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::INTERNAL_get_visibleRect" , (gpointer)& GUIClip_CUSTOM_INTERNAL_get_visibleRect );
}

void Register_UnityEngine_GUIClip_Internal_GetAbsoluteMousePosition()
{
    scripting_add_internal_call( "UnityEngine.GUIClip::Internal_GetAbsoluteMousePosition" , (gpointer)& GUIClip_CUSTOM_Internal_GetAbsoluteMousePosition );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GUIUtility_IcallNames [] =
{
    "UnityEngine.GUIUtility::GetControlID"  ,    // -> GUIUtility_CUSTOM_GetControlID
    "UnityEngine.GUIUtility::INTERNAL_CALL_Internal_GetNextControlID2",    // -> GUIUtility_CUSTOM_INTERNAL_CALL_Internal_GetNextControlID2
    "UnityEngine.GUIUtility::GetPermanentControlID",    // -> GUIUtility_CUSTOM_GetPermanentControlID
    "UnityEngine.GUIUtility::Internal_GetHotControl",    // -> GUIUtility_CUSTOM_Internal_GetHotControl
    "UnityEngine.GUIUtility::Internal_SetHotControl",    // -> GUIUtility_CUSTOM_Internal_SetHotControl
    "UnityEngine.GUIUtility::UpdateUndoName",    // -> GUIUtility_CUSTOM_UpdateUndoName
    "UnityEngine.GUIUtility::get_keyboardControl",    // -> GUIUtility_Get_Custom_PropKeyboardControl
    "UnityEngine.GUIUtility::set_keyboardControl",    // -> GUIUtility_Set_Custom_PropKeyboardControl
    "UnityEngine.GUIUtility::SetDidGUIWindowsEatLastEvent",    // -> GUIUtility_CUSTOM_SetDidGUIWindowsEatLastEvent
    "UnityEngine.GUIUtility::get_systemCopyBuffer",    // -> GUIUtility_Get_Custom_PropSystemCopyBuffer
    "UnityEngine.GUIUtility::set_systemCopyBuffer",    // -> GUIUtility_Set_Custom_PropSystemCopyBuffer
    "UnityEngine.GUIUtility::Internal_LoadSkin",    // -> GUIUtility_CUSTOM_Internal_LoadSkin
    "UnityEngine.GUIUtility::Internal_ExitGUI",    // -> GUIUtility_CUSTOM_Internal_ExitGUI
    "UnityEngine.GUIUtility::Internal_GetGUIDepth",    // -> GUIUtility_CUSTOM_Internal_GetGUIDepth
    "UnityEngine.GUIUtility::get_mouseUsed" ,    // -> GUIUtility_Get_Custom_PropMouseUsed
    "UnityEngine.GUIUtility::set_mouseUsed" ,    // -> GUIUtility_Set_Custom_PropMouseUsed
    "UnityEngine.GUIUtility::get_textFieldInput",    // -> GUIUtility_Get_Custom_PropTextFieldInput
    "UnityEngine.GUIUtility::set_textFieldInput",    // -> GUIUtility_Set_Custom_PropTextFieldInput
    "UnityEngine.GUIClip::INTERNAL_CALL_Push",    // -> GUIClip_CUSTOM_INTERNAL_CALL_Push
    "UnityEngine.GUIClip::Pop"              ,    // -> GUIClip_CUSTOM_Pop
    "UnityEngine.GUIClip::INTERNAL_CALL_GetTopRect",    // -> GUIClip_CUSTOM_INTERNAL_CALL_GetTopRect
    "UnityEngine.GUIClip::get_enabled"      ,    // -> GUIClip_Get_Custom_PropEnabled
    "UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Vector2",    // -> GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Vector2
    "UnityEngine.GUIClip::INTERNAL_get_topmostRect",    // -> GUIClip_CUSTOM_INTERNAL_get_topmostRect
    "UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Rect",    // -> GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Rect
    "UnityEngine.GUIClip::INTERNAL_CALL_Clip_Vector2",    // -> GUIClip_CUSTOM_INTERNAL_CALL_Clip_Vector2
    "UnityEngine.GUIClip::INTERNAL_CALL_Internal_Clip_Rect",    // -> GUIClip_CUSTOM_INTERNAL_CALL_Internal_Clip_Rect
    "UnityEngine.GUIClip::Reapply"          ,    // -> GUIClip_CUSTOM_Reapply
    "UnityEngine.GUIClip::INTERNAL_CALL_GetMatrix",    // -> GUIClip_CUSTOM_INTERNAL_CALL_GetMatrix
    "UnityEngine.GUIClip::INTERNAL_CALL_SetMatrix",    // -> GUIClip_CUSTOM_INTERNAL_CALL_SetMatrix
    "UnityEngine.GUIClip::INTERNAL_get_visibleRect",    // -> GUIClip_CUSTOM_INTERNAL_get_visibleRect
    "UnityEngine.GUIClip::Internal_GetAbsoluteMousePosition",    // -> GUIClip_CUSTOM_Internal_GetAbsoluteMousePosition
    NULL
};

static const void* s_GUIUtility_IcallFuncs [] =
{
    (const void*)&GUIUtility_CUSTOM_GetControlID          ,  //  <- UnityEngine.GUIUtility::GetControlID
    (const void*)&GUIUtility_CUSTOM_INTERNAL_CALL_Internal_GetNextControlID2,  //  <- UnityEngine.GUIUtility::INTERNAL_CALL_Internal_GetNextControlID2
    (const void*)&GUIUtility_CUSTOM_GetPermanentControlID ,  //  <- UnityEngine.GUIUtility::GetPermanentControlID
    (const void*)&GUIUtility_CUSTOM_Internal_GetHotControl,  //  <- UnityEngine.GUIUtility::Internal_GetHotControl
    (const void*)&GUIUtility_CUSTOM_Internal_SetHotControl,  //  <- UnityEngine.GUIUtility::Internal_SetHotControl
    (const void*)&GUIUtility_CUSTOM_UpdateUndoName        ,  //  <- UnityEngine.GUIUtility::UpdateUndoName
    (const void*)&GUIUtility_Get_Custom_PropKeyboardControl,  //  <- UnityEngine.GUIUtility::get_keyboardControl
    (const void*)&GUIUtility_Set_Custom_PropKeyboardControl,  //  <- UnityEngine.GUIUtility::set_keyboardControl
    (const void*)&GUIUtility_CUSTOM_SetDidGUIWindowsEatLastEvent,  //  <- UnityEngine.GUIUtility::SetDidGUIWindowsEatLastEvent
    (const void*)&GUIUtility_Get_Custom_PropSystemCopyBuffer,  //  <- UnityEngine.GUIUtility::get_systemCopyBuffer
    (const void*)&GUIUtility_Set_Custom_PropSystemCopyBuffer,  //  <- UnityEngine.GUIUtility::set_systemCopyBuffer
    (const void*)&GUIUtility_CUSTOM_Internal_LoadSkin     ,  //  <- UnityEngine.GUIUtility::Internal_LoadSkin
    (const void*)&GUIUtility_CUSTOM_Internal_ExitGUI      ,  //  <- UnityEngine.GUIUtility::Internal_ExitGUI
    (const void*)&GUIUtility_CUSTOM_Internal_GetGUIDepth  ,  //  <- UnityEngine.GUIUtility::Internal_GetGUIDepth
    (const void*)&GUIUtility_Get_Custom_PropMouseUsed     ,  //  <- UnityEngine.GUIUtility::get_mouseUsed
    (const void*)&GUIUtility_Set_Custom_PropMouseUsed     ,  //  <- UnityEngine.GUIUtility::set_mouseUsed
    (const void*)&GUIUtility_Get_Custom_PropTextFieldInput,  //  <- UnityEngine.GUIUtility::get_textFieldInput
    (const void*)&GUIUtility_Set_Custom_PropTextFieldInput,  //  <- UnityEngine.GUIUtility::set_textFieldInput
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_Push       ,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_Push
    (const void*)&GUIClip_CUSTOM_Pop                      ,  //  <- UnityEngine.GUIClip::Pop
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_GetTopRect ,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_GetTopRect
    (const void*)&GUIClip_Get_Custom_PropEnabled          ,  //  <- UnityEngine.GUIClip::get_enabled
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Vector2,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Vector2
    (const void*)&GUIClip_CUSTOM_INTERNAL_get_topmostRect ,  //  <- UnityEngine.GUIClip::INTERNAL_get_topmostRect
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Rect,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Rect
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_Clip_Vector2,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_Clip_Vector2
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_Internal_Clip_Rect,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_Internal_Clip_Rect
    (const void*)&GUIClip_CUSTOM_Reapply                  ,  //  <- UnityEngine.GUIClip::Reapply
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_GetMatrix  ,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_GetMatrix
    (const void*)&GUIClip_CUSTOM_INTERNAL_CALL_SetMatrix  ,  //  <- UnityEngine.GUIClip::INTERNAL_CALL_SetMatrix
    (const void*)&GUIClip_CUSTOM_INTERNAL_get_visibleRect ,  //  <- UnityEngine.GUIClip::INTERNAL_get_visibleRect
    (const void*)&GUIClip_CUSTOM_Internal_GetAbsoluteMousePosition,  //  <- UnityEngine.GUIClip::Internal_GetAbsoluteMousePosition
    NULL
};

void ExportGUIUtilityBindings();
void ExportGUIUtilityBindings()
{
    for (int i = 0; s_GUIUtility_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GUIUtility_IcallNames [i], s_GUIUtility_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportGUIUtilityBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(GUIUtility_CUSTOM_GetControlID); //  <- UnityEngine.GUIUtility::GetControlID
    SET_METRO_BINDING(GUIUtility_CUSTOM_INTERNAL_CALL_Internal_GetNextControlID2); //  <- UnityEngine.GUIUtility::INTERNAL_CALL_Internal_GetNextControlID2
    SET_METRO_BINDING(GUIUtility_CUSTOM_GetPermanentControlID); //  <- UnityEngine.GUIUtility::GetPermanentControlID
    SET_METRO_BINDING(GUIUtility_CUSTOM_Internal_GetHotControl); //  <- UnityEngine.GUIUtility::Internal_GetHotControl
    SET_METRO_BINDING(GUIUtility_CUSTOM_Internal_SetHotControl); //  <- UnityEngine.GUIUtility::Internal_SetHotControl
    SET_METRO_BINDING(GUIUtility_CUSTOM_UpdateUndoName); //  <- UnityEngine.GUIUtility::UpdateUndoName
    SET_METRO_BINDING(GUIUtility_Get_Custom_PropKeyboardControl); //  <- UnityEngine.GUIUtility::get_keyboardControl
    SET_METRO_BINDING(GUIUtility_Set_Custom_PropKeyboardControl); //  <- UnityEngine.GUIUtility::set_keyboardControl
    SET_METRO_BINDING(GUIUtility_CUSTOM_SetDidGUIWindowsEatLastEvent); //  <- UnityEngine.GUIUtility::SetDidGUIWindowsEatLastEvent
    SET_METRO_BINDING(GUIUtility_Get_Custom_PropSystemCopyBuffer); //  <- UnityEngine.GUIUtility::get_systemCopyBuffer
    SET_METRO_BINDING(GUIUtility_Set_Custom_PropSystemCopyBuffer); //  <- UnityEngine.GUIUtility::set_systemCopyBuffer
    SET_METRO_BINDING(GUIUtility_CUSTOM_Internal_LoadSkin); //  <- UnityEngine.GUIUtility::Internal_LoadSkin
    SET_METRO_BINDING(GUIUtility_CUSTOM_Internal_ExitGUI); //  <- UnityEngine.GUIUtility::Internal_ExitGUI
    SET_METRO_BINDING(GUIUtility_CUSTOM_Internal_GetGUIDepth); //  <- UnityEngine.GUIUtility::Internal_GetGUIDepth
    SET_METRO_BINDING(GUIUtility_Get_Custom_PropMouseUsed); //  <- UnityEngine.GUIUtility::get_mouseUsed
    SET_METRO_BINDING(GUIUtility_Set_Custom_PropMouseUsed); //  <- UnityEngine.GUIUtility::set_mouseUsed
    SET_METRO_BINDING(GUIUtility_Get_Custom_PropTextFieldInput); //  <- UnityEngine.GUIUtility::get_textFieldInput
    SET_METRO_BINDING(GUIUtility_Set_Custom_PropTextFieldInput); //  <- UnityEngine.GUIUtility::set_textFieldInput
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_Push); //  <- UnityEngine.GUIClip::INTERNAL_CALL_Push
    SET_METRO_BINDING(GUIClip_CUSTOM_Pop); //  <- UnityEngine.GUIClip::Pop
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_GetTopRect); //  <- UnityEngine.GUIClip::INTERNAL_CALL_GetTopRect
    SET_METRO_BINDING(GUIClip_Get_Custom_PropEnabled); //  <- UnityEngine.GUIClip::get_enabled
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Vector2); //  <- UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Vector2
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_get_topmostRect); //  <- UnityEngine.GUIClip::INTERNAL_get_topmostRect
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_Unclip_Rect); //  <- UnityEngine.GUIClip::INTERNAL_CALL_Unclip_Rect
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_Clip_Vector2); //  <- UnityEngine.GUIClip::INTERNAL_CALL_Clip_Vector2
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_Internal_Clip_Rect); //  <- UnityEngine.GUIClip::INTERNAL_CALL_Internal_Clip_Rect
    SET_METRO_BINDING(GUIClip_CUSTOM_Reapply); //  <- UnityEngine.GUIClip::Reapply
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_GetMatrix); //  <- UnityEngine.GUIClip::INTERNAL_CALL_GetMatrix
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_CALL_SetMatrix); //  <- UnityEngine.GUIClip::INTERNAL_CALL_SetMatrix
    SET_METRO_BINDING(GUIClip_CUSTOM_INTERNAL_get_visibleRect); //  <- UnityEngine.GUIClip::INTERNAL_get_visibleRect
    SET_METRO_BINDING(GUIClip_CUSTOM_Internal_GetAbsoluteMousePosition); //  <- UnityEngine.GUIClip::Internal_GetAbsoluteMousePosition
}

#endif
