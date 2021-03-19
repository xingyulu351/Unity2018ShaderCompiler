#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/IMGUI/GUIManager.h"
#include "Runtime/Math/Color.h"
#include "Runtime/IMGUI/GUIStyle.h"
#include "External/shaderlab/Library/FastPropertyName.h"
#include "External/shaderlab/Library/properties.h"
#include "Runtime/Shaders/Material.h"
#include "Runtime/IMGUI/GUIState.h"
#include "Runtime/IMGUI/IDList.h"
#include "Runtime/IMGUI/GUIContent.h"
#include "Runtime/IMGUI/GUILabel.h"
#include "Runtime/IMGUI/GUIToggle.h"
#include "Runtime/IMGUI/GUIButton.h"
#include "Runtime/IMGUI/GUIWindows.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/IMGUI/IMGUIUtils.h"

#if UNITY_EDITOR
#include "Editor/Platform/Interface/EditorWindows.h"
#endif

using namespace Unity;
using namespace std;

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_get_color(ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_get_color)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_color)
    { *returnValue =(IMGUI::GetColor (GetGUIState())); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_set_color(const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_set_color)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_color)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_color)
     IMGUI::SetColor (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_get_backgroundColor(ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_get_backgroundColor)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_backgroundColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_backgroundColor)
    { *returnValue =(IMGUI::GetBackgroundColor (GetGUIState())); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_set_backgroundColor(const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_set_backgroundColor)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_backgroundColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_backgroundColor)
     IMGUI::SetBackgroundColor (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_get_contentColor(ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_get_contentColor)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_contentColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_contentColor)
    { *returnValue =(IMGUI::GetContentColor (GetGUIState())); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_set_contentColor(const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_set_contentColor)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_contentColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_contentColor)
     IMGUI::SetContentColor (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUI_Get_Custom_PropChanged()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Get_Custom_PropChanged)
    SCRIPTINGAPI_STACK_CHECK(get_changed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_changed)
    return IMGUI::GetChanged (GetGUIState());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_Set_Custom_PropChanged(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Set_Custom_PropChanged)
    SCRIPTINGAPI_STACK_CHECK(set_changed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_changed)
     IMGUI::SetChanged (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUI_Get_Custom_PropEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Get_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return IMGUI::GetEnabled (GetGUIState());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_Set_Custom_PropEnabled(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Set_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
     IMGUI::SetEnabled (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GUI_CUSTOM_Internal_GetTooltip()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_Internal_GetTooltip)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetTooltip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetTooltip)
    
    		GUIState &cState = GetGUIState();
    		UTF16String *mouseTooltip = cState.m_OnGUIState.m_MouseTooltip;
    		UTF16String *keyTooltip = cState.m_OnGUIState.m_KeyTooltip;
    		UTF16String *tooltip = NULL;
    		if (mouseTooltip)
    		{
    			tooltip = mouseTooltip;
    		}
    		else if (keyTooltip)
    		{
    			tooltip = keyTooltip;
    		}
    		if (tooltip)
    			return tooltip->GetScriptingString ();
    		return SCRIPTING_NULL;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_Internal_SetTooltip(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_Internal_SetTooltip)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetTooltip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetTooltip)
    
    #if ENABLE_MONO
    		UTF16String str (value.ToUTF8().c_str());
    		GUIState &cState = GetGUIState();
    		cState.m_OnGUIState.SetMouseTooltip (str);
    		cState.m_OnGUIState.SetKeyTooltip (str);
    #endif		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GUI_CUSTOM_Internal_GetMouseTooltip()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_Internal_GetMouseTooltip)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetMouseTooltip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetMouseTooltip)
    
    		GUIState &cState = GetGUIState();
    		UTF16String *mouseTooltip = cState.m_OnGUIState.m_MouseTooltip;
    		return mouseTooltip ? mouseTooltip->GetScriptingString () : SCRIPTING_NULL;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUI_Get_Custom_PropDepth()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Get_Custom_PropDepth)
    SCRIPTINGAPI_STACK_CHECK(get_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_depth)
    return IMGUI::GetDepth (GetGUIState());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_Set_Custom_PropDepth(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Set_Custom_PropDepth)
    SCRIPTINGAPI_STACK_CHECK(set_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_depth)
     IMGUI::SetDepth (GetGUIState(), value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_CALL_DoLabel(const Rectf& position, ICallType_Object_Argument content_, ICallType_IntPtr_Argument style_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_CALL_DoLabel)
    ICallType_Object_Local content(content_);
    UNUSED(content);
    void* style(style_);
    UNUSED(style);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DoLabel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DoLabel)
    
    		IMGUI::GUILabel (
    			GetGUIState(), 
    			position, 
    			MonoGUIContentToTempNative (content), 
    			*reinterpret_cast<GUIStyle*>(style)
    		);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_InitializeGUIClipTexture()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_InitializeGUIClipTexture)
    SCRIPTINGAPI_STACK_CHECK(InitializeGUIClipTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitializeGUIClipTexture)
     InitializeGUIClipTexture(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUI_Get_Custom_PropBlendMaterial()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Get_Custom_PropBlendMaterial)
    SCRIPTINGAPI_STACK_CHECK(get_blendMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_blendMaterial)
    return Scripting::ScriptingWrapperFor(GetGUIBlendMaterial());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUI_Get_Custom_PropBlitMaterial()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Get_Custom_PropBlitMaterial)
    SCRIPTINGAPI_STACK_CHECK(get_blitMaterial)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_blitMaterial)
    return Scripting::ScriptingWrapperFor(GetGUIBlitMaterial());
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_CALL_DoButton(const Rectf& position, ICallType_Object_Argument content_, ICallType_IntPtr_Argument style_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_CALL_DoButton)
    ICallType_Object_Local content(content_);
    UNUSED(content);
    void* style(style_);
    UNUSED(style);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DoButton)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DoButton)
    
    		return IMGUI::GUIButton (
    			GetGUIState(), 
    			position, 
    			MonoGUIContentToTempNative (content), 
    			*reinterpret_cast<GUIStyle*>(style)
    		);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_SetNextControlName(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_SetNextControlName)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetNextControlName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetNextControlName)
    
    		GetGUIState().SetNameOfNextKeyboardControl (name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GUI_CUSTOM_GetNameOfFocusedControl()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_GetNameOfFocusedControl)
    SCRIPTINGAPI_STACK_CHECK(GetNameOfFocusedControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetNameOfFocusedControl)
    
    		return CreateScriptingString (GetGUIState().GetNameOfFocusedControl ().c_str());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_FocusControl(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_FocusControl)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(FocusControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FocusControl)
    
    		GetGUIState().FocusKeyboardControl (name);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_CALL_DoToggle(const Rectf& position, int id, ScriptingBool value, ICallType_Object_Argument content_, ICallType_IntPtr_Argument style_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_CALL_DoToggle)
    ICallType_Object_Local content(content_);
    UNUSED(content);
    void* style(style_);
    UNUSED(style);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DoToggle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DoToggle)
    
    		return IMGUI::GUIToggle (
    			GetGUIState(), 
    			position, 
    			value, 
    			MonoGUIContentToTempNative (content), 
    			*reinterpret_cast<GUIStyle*>(style), 
    			id
    		);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUI_Get_Custom_PropUsePageScrollbars()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_Get_Custom_PropUsePageScrollbars)
    SCRIPTINGAPI_STACK_CHECK(get_usePageScrollbars)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_usePageScrollbars)
    #if PLATFORM_OSX
    Boolean exists;
    Boolean scroll = CFPreferencesGetAppBooleanValue(CFSTR("AppleScrollerPagingBehavior"),CFSTR("Apple Global Domain"),&exists);
    return !scroll;
    #else
    return true;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_InternalRepaintEditorWindow()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_InternalRepaintEditorWindow)
    SCRIPTINGAPI_STACK_CHECK(InternalRepaintEditorWindow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalRepaintEditorWindow)
    
    		#if UNITY_EDITOR
    		GUIView *view = GUIView::GetCurrent ();
    		if (view) {
    			view->RequestRepaint ();
    		} else {
    			ErrorString ("InternalRepaint called outside an editor OnGUI");
    		}
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_CALL_DoWindow(int id, const Rectf& clientRect, ICallType_Object_Argument func_, ICallType_Object_Argument title_, ICallType_Object_Argument style_, ICallType_Object_Argument skin_, ScriptingBool forceRectOnLayout, Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_CALL_DoWindow)
    ICallType_Object_Local func(func_);
    UNUSED(func);
    ICallType_Object_Local title(title_);
    UNUSED(title);
    ScriptingObjectWithIntPtrField<GUIStyle> style(style_);
    UNUSED(style);
    ICallType_Object_Local skin(skin_);
    UNUSED(skin);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DoWindow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DoWindow)
    
    		{ returnValue =(IMGUI::DoWindow (GetGUIState (), id, clientRect, func, MonoGUIContentToTempNative (title), style.GetScriptingObject(), skin, forceRectOnLayout)); return; }		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_INTERNAL_CALL_DragWindow(const Rectf& position)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_INTERNAL_CALL_DragWindow)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DragWindow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DragWindow)
     IMGUI::DragWindow (GetGUIState(), position); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_BringWindowToFront(int windowID)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_BringWindowToFront)
    SCRIPTINGAPI_STACK_CHECK(BringWindowToFront)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BringWindowToFront)
    
    		IMGUI::BringWindowToFront (GetGUIState(), windowID);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_BringWindowToBack(int windowID)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_BringWindowToBack)
    SCRIPTINGAPI_STACK_CHECK(BringWindowToBack)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BringWindowToBack)
    
    		IMGUI::BringWindowToBack (GetGUIState(), windowID);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_FocusWindow(int windowID)
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_FocusWindow)
    SCRIPTINGAPI_STACK_CHECK(FocusWindow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FocusWindow)
    
    		IMGUI::FocusWindow (GetGUIState(), windowID);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_UnfocusWindow()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_UnfocusWindow)
    SCRIPTINGAPI_STACK_CHECK(UnfocusWindow)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UnfocusWindow)
    
    		IMGUI::FocusWindow (GetGUIState(), -1);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_Internal_BeginWindows()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_Internal_BeginWindows)
    SCRIPTINGAPI_STACK_CHECK(Internal_BeginWindows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_BeginWindows)
    
    		IMGUI::BeginWindows (GetGUIState(), false);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUI_CUSTOM_Internal_EndWindows()
{
    SCRIPTINGAPI_ETW_ENTRY(GUI_CUSTOM_Internal_EndWindows)
    SCRIPTINGAPI_STACK_CHECK(Internal_EndWindows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_EndWindows)
    
    		IMGUI::EndWindows (GetGUIState());
    	 
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_GUI_INTERNAL_get_color()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_get_color" , (gpointer)& GUI_CUSTOM_INTERNAL_get_color );
}

void Register_UnityEngine_GUI_INTERNAL_set_color()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_set_color" , (gpointer)& GUI_CUSTOM_INTERNAL_set_color );
}

void Register_UnityEngine_GUI_INTERNAL_get_backgroundColor()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_get_backgroundColor" , (gpointer)& GUI_CUSTOM_INTERNAL_get_backgroundColor );
}

void Register_UnityEngine_GUI_INTERNAL_set_backgroundColor()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_set_backgroundColor" , (gpointer)& GUI_CUSTOM_INTERNAL_set_backgroundColor );
}

void Register_UnityEngine_GUI_INTERNAL_get_contentColor()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_get_contentColor" , (gpointer)& GUI_CUSTOM_INTERNAL_get_contentColor );
}

void Register_UnityEngine_GUI_INTERNAL_set_contentColor()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_set_contentColor" , (gpointer)& GUI_CUSTOM_INTERNAL_set_contentColor );
}

void Register_UnityEngine_GUI_get_changed()
{
    scripting_add_internal_call( "UnityEngine.GUI::get_changed" , (gpointer)& GUI_Get_Custom_PropChanged );
}

void Register_UnityEngine_GUI_set_changed()
{
    scripting_add_internal_call( "UnityEngine.GUI::set_changed" , (gpointer)& GUI_Set_Custom_PropChanged );
}

void Register_UnityEngine_GUI_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.GUI::get_enabled" , (gpointer)& GUI_Get_Custom_PropEnabled );
}

void Register_UnityEngine_GUI_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.GUI::set_enabled" , (gpointer)& GUI_Set_Custom_PropEnabled );
}

void Register_UnityEngine_GUI_Internal_GetTooltip()
{
    scripting_add_internal_call( "UnityEngine.GUI::Internal_GetTooltip" , (gpointer)& GUI_CUSTOM_Internal_GetTooltip );
}

void Register_UnityEngine_GUI_Internal_SetTooltip()
{
    scripting_add_internal_call( "UnityEngine.GUI::Internal_SetTooltip" , (gpointer)& GUI_CUSTOM_Internal_SetTooltip );
}

void Register_UnityEngine_GUI_Internal_GetMouseTooltip()
{
    scripting_add_internal_call( "UnityEngine.GUI::Internal_GetMouseTooltip" , (gpointer)& GUI_CUSTOM_Internal_GetMouseTooltip );
}

void Register_UnityEngine_GUI_get_depth()
{
    scripting_add_internal_call( "UnityEngine.GUI::get_depth" , (gpointer)& GUI_Get_Custom_PropDepth );
}

void Register_UnityEngine_GUI_set_depth()
{
    scripting_add_internal_call( "UnityEngine.GUI::set_depth" , (gpointer)& GUI_Set_Custom_PropDepth );
}

void Register_UnityEngine_GUI_INTERNAL_CALL_DoLabel()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_CALL_DoLabel" , (gpointer)& GUI_CUSTOM_INTERNAL_CALL_DoLabel );
}

void Register_UnityEngine_GUI_InitializeGUIClipTexture()
{
    scripting_add_internal_call( "UnityEngine.GUI::InitializeGUIClipTexture" , (gpointer)& GUI_CUSTOM_InitializeGUIClipTexture );
}

void Register_UnityEngine_GUI_get_blendMaterial()
{
    scripting_add_internal_call( "UnityEngine.GUI::get_blendMaterial" , (gpointer)& GUI_Get_Custom_PropBlendMaterial );
}

void Register_UnityEngine_GUI_get_blitMaterial()
{
    scripting_add_internal_call( "UnityEngine.GUI::get_blitMaterial" , (gpointer)& GUI_Get_Custom_PropBlitMaterial );
}

void Register_UnityEngine_GUI_INTERNAL_CALL_DoButton()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_CALL_DoButton" , (gpointer)& GUI_CUSTOM_INTERNAL_CALL_DoButton );
}

void Register_UnityEngine_GUI_SetNextControlName()
{
    scripting_add_internal_call( "UnityEngine.GUI::SetNextControlName" , (gpointer)& GUI_CUSTOM_SetNextControlName );
}

void Register_UnityEngine_GUI_GetNameOfFocusedControl()
{
    scripting_add_internal_call( "UnityEngine.GUI::GetNameOfFocusedControl" , (gpointer)& GUI_CUSTOM_GetNameOfFocusedControl );
}

void Register_UnityEngine_GUI_FocusControl()
{
    scripting_add_internal_call( "UnityEngine.GUI::FocusControl" , (gpointer)& GUI_CUSTOM_FocusControl );
}

void Register_UnityEngine_GUI_INTERNAL_CALL_DoToggle()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_CALL_DoToggle" , (gpointer)& GUI_CUSTOM_INTERNAL_CALL_DoToggle );
}

void Register_UnityEngine_GUI_get_usePageScrollbars()
{
    scripting_add_internal_call( "UnityEngine.GUI::get_usePageScrollbars" , (gpointer)& GUI_Get_Custom_PropUsePageScrollbars );
}

void Register_UnityEngine_GUI_InternalRepaintEditorWindow()
{
    scripting_add_internal_call( "UnityEngine.GUI::InternalRepaintEditorWindow" , (gpointer)& GUI_CUSTOM_InternalRepaintEditorWindow );
}

void Register_UnityEngine_GUI_INTERNAL_CALL_DoWindow()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_CALL_DoWindow" , (gpointer)& GUI_CUSTOM_INTERNAL_CALL_DoWindow );
}

void Register_UnityEngine_GUI_INTERNAL_CALL_DragWindow()
{
    scripting_add_internal_call( "UnityEngine.GUI::INTERNAL_CALL_DragWindow" , (gpointer)& GUI_CUSTOM_INTERNAL_CALL_DragWindow );
}

void Register_UnityEngine_GUI_BringWindowToFront()
{
    scripting_add_internal_call( "UnityEngine.GUI::BringWindowToFront" , (gpointer)& GUI_CUSTOM_BringWindowToFront );
}

void Register_UnityEngine_GUI_BringWindowToBack()
{
    scripting_add_internal_call( "UnityEngine.GUI::BringWindowToBack" , (gpointer)& GUI_CUSTOM_BringWindowToBack );
}

void Register_UnityEngine_GUI_FocusWindow()
{
    scripting_add_internal_call( "UnityEngine.GUI::FocusWindow" , (gpointer)& GUI_CUSTOM_FocusWindow );
}

void Register_UnityEngine_GUI_UnfocusWindow()
{
    scripting_add_internal_call( "UnityEngine.GUI::UnfocusWindow" , (gpointer)& GUI_CUSTOM_UnfocusWindow );
}

void Register_UnityEngine_GUI_Internal_BeginWindows()
{
    scripting_add_internal_call( "UnityEngine.GUI::Internal_BeginWindows" , (gpointer)& GUI_CUSTOM_Internal_BeginWindows );
}

void Register_UnityEngine_GUI_Internal_EndWindows()
{
    scripting_add_internal_call( "UnityEngine.GUI::Internal_EndWindows" , (gpointer)& GUI_CUSTOM_Internal_EndWindows );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GUI_IcallNames [] =
{
    "UnityEngine.GUI::INTERNAL_get_color"   ,    // -> GUI_CUSTOM_INTERNAL_get_color
    "UnityEngine.GUI::INTERNAL_set_color"   ,    // -> GUI_CUSTOM_INTERNAL_set_color
    "UnityEngine.GUI::INTERNAL_get_backgroundColor",    // -> GUI_CUSTOM_INTERNAL_get_backgroundColor
    "UnityEngine.GUI::INTERNAL_set_backgroundColor",    // -> GUI_CUSTOM_INTERNAL_set_backgroundColor
    "UnityEngine.GUI::INTERNAL_get_contentColor",    // -> GUI_CUSTOM_INTERNAL_get_contentColor
    "UnityEngine.GUI::INTERNAL_set_contentColor",    // -> GUI_CUSTOM_INTERNAL_set_contentColor
    "UnityEngine.GUI::get_changed"          ,    // -> GUI_Get_Custom_PropChanged
    "UnityEngine.GUI::set_changed"          ,    // -> GUI_Set_Custom_PropChanged
    "UnityEngine.GUI::get_enabled"          ,    // -> GUI_Get_Custom_PropEnabled
    "UnityEngine.GUI::set_enabled"          ,    // -> GUI_Set_Custom_PropEnabled
    "UnityEngine.GUI::Internal_GetTooltip"  ,    // -> GUI_CUSTOM_Internal_GetTooltip
    "UnityEngine.GUI::Internal_SetTooltip"  ,    // -> GUI_CUSTOM_Internal_SetTooltip
    "UnityEngine.GUI::Internal_GetMouseTooltip",    // -> GUI_CUSTOM_Internal_GetMouseTooltip
    "UnityEngine.GUI::get_depth"            ,    // -> GUI_Get_Custom_PropDepth
    "UnityEngine.GUI::set_depth"            ,    // -> GUI_Set_Custom_PropDepth
    "UnityEngine.GUI::INTERNAL_CALL_DoLabel",    // -> GUI_CUSTOM_INTERNAL_CALL_DoLabel
    "UnityEngine.GUI::InitializeGUIClipTexture",    // -> GUI_CUSTOM_InitializeGUIClipTexture
    "UnityEngine.GUI::get_blendMaterial"    ,    // -> GUI_Get_Custom_PropBlendMaterial
    "UnityEngine.GUI::get_blitMaterial"     ,    // -> GUI_Get_Custom_PropBlitMaterial
    "UnityEngine.GUI::INTERNAL_CALL_DoButton",    // -> GUI_CUSTOM_INTERNAL_CALL_DoButton
    "UnityEngine.GUI::SetNextControlName"   ,    // -> GUI_CUSTOM_SetNextControlName
    "UnityEngine.GUI::GetNameOfFocusedControl",    // -> GUI_CUSTOM_GetNameOfFocusedControl
    "UnityEngine.GUI::FocusControl"         ,    // -> GUI_CUSTOM_FocusControl
    "UnityEngine.GUI::INTERNAL_CALL_DoToggle",    // -> GUI_CUSTOM_INTERNAL_CALL_DoToggle
    "UnityEngine.GUI::get_usePageScrollbars",    // -> GUI_Get_Custom_PropUsePageScrollbars
    "UnityEngine.GUI::InternalRepaintEditorWindow",    // -> GUI_CUSTOM_InternalRepaintEditorWindow
    "UnityEngine.GUI::INTERNAL_CALL_DoWindow",    // -> GUI_CUSTOM_INTERNAL_CALL_DoWindow
    "UnityEngine.GUI::INTERNAL_CALL_DragWindow",    // -> GUI_CUSTOM_INTERNAL_CALL_DragWindow
    "UnityEngine.GUI::BringWindowToFront"   ,    // -> GUI_CUSTOM_BringWindowToFront
    "UnityEngine.GUI::BringWindowToBack"    ,    // -> GUI_CUSTOM_BringWindowToBack
    "UnityEngine.GUI::FocusWindow"          ,    // -> GUI_CUSTOM_FocusWindow
    "UnityEngine.GUI::UnfocusWindow"        ,    // -> GUI_CUSTOM_UnfocusWindow
    "UnityEngine.GUI::Internal_BeginWindows",    // -> GUI_CUSTOM_Internal_BeginWindows
    "UnityEngine.GUI::Internal_EndWindows"  ,    // -> GUI_CUSTOM_Internal_EndWindows
    NULL
};

static const void* s_GUI_IcallFuncs [] =
{
    (const void*)&GUI_CUSTOM_INTERNAL_get_color           ,  //  <- UnityEngine.GUI::INTERNAL_get_color
    (const void*)&GUI_CUSTOM_INTERNAL_set_color           ,  //  <- UnityEngine.GUI::INTERNAL_set_color
    (const void*)&GUI_CUSTOM_INTERNAL_get_backgroundColor ,  //  <- UnityEngine.GUI::INTERNAL_get_backgroundColor
    (const void*)&GUI_CUSTOM_INTERNAL_set_backgroundColor ,  //  <- UnityEngine.GUI::INTERNAL_set_backgroundColor
    (const void*)&GUI_CUSTOM_INTERNAL_get_contentColor    ,  //  <- UnityEngine.GUI::INTERNAL_get_contentColor
    (const void*)&GUI_CUSTOM_INTERNAL_set_contentColor    ,  //  <- UnityEngine.GUI::INTERNAL_set_contentColor
    (const void*)&GUI_Get_Custom_PropChanged              ,  //  <- UnityEngine.GUI::get_changed
    (const void*)&GUI_Set_Custom_PropChanged              ,  //  <- UnityEngine.GUI::set_changed
    (const void*)&GUI_Get_Custom_PropEnabled              ,  //  <- UnityEngine.GUI::get_enabled
    (const void*)&GUI_Set_Custom_PropEnabled              ,  //  <- UnityEngine.GUI::set_enabled
    (const void*)&GUI_CUSTOM_Internal_GetTooltip          ,  //  <- UnityEngine.GUI::Internal_GetTooltip
    (const void*)&GUI_CUSTOM_Internal_SetTooltip          ,  //  <- UnityEngine.GUI::Internal_SetTooltip
    (const void*)&GUI_CUSTOM_Internal_GetMouseTooltip     ,  //  <- UnityEngine.GUI::Internal_GetMouseTooltip
    (const void*)&GUI_Get_Custom_PropDepth                ,  //  <- UnityEngine.GUI::get_depth
    (const void*)&GUI_Set_Custom_PropDepth                ,  //  <- UnityEngine.GUI::set_depth
    (const void*)&GUI_CUSTOM_INTERNAL_CALL_DoLabel        ,  //  <- UnityEngine.GUI::INTERNAL_CALL_DoLabel
    (const void*)&GUI_CUSTOM_InitializeGUIClipTexture     ,  //  <- UnityEngine.GUI::InitializeGUIClipTexture
    (const void*)&GUI_Get_Custom_PropBlendMaterial        ,  //  <- UnityEngine.GUI::get_blendMaterial
    (const void*)&GUI_Get_Custom_PropBlitMaterial         ,  //  <- UnityEngine.GUI::get_blitMaterial
    (const void*)&GUI_CUSTOM_INTERNAL_CALL_DoButton       ,  //  <- UnityEngine.GUI::INTERNAL_CALL_DoButton
    (const void*)&GUI_CUSTOM_SetNextControlName           ,  //  <- UnityEngine.GUI::SetNextControlName
    (const void*)&GUI_CUSTOM_GetNameOfFocusedControl      ,  //  <- UnityEngine.GUI::GetNameOfFocusedControl
    (const void*)&GUI_CUSTOM_FocusControl                 ,  //  <- UnityEngine.GUI::FocusControl
    (const void*)&GUI_CUSTOM_INTERNAL_CALL_DoToggle       ,  //  <- UnityEngine.GUI::INTERNAL_CALL_DoToggle
    (const void*)&GUI_Get_Custom_PropUsePageScrollbars    ,  //  <- UnityEngine.GUI::get_usePageScrollbars
    (const void*)&GUI_CUSTOM_InternalRepaintEditorWindow  ,  //  <- UnityEngine.GUI::InternalRepaintEditorWindow
    (const void*)&GUI_CUSTOM_INTERNAL_CALL_DoWindow       ,  //  <- UnityEngine.GUI::INTERNAL_CALL_DoWindow
    (const void*)&GUI_CUSTOM_INTERNAL_CALL_DragWindow     ,  //  <- UnityEngine.GUI::INTERNAL_CALL_DragWindow
    (const void*)&GUI_CUSTOM_BringWindowToFront           ,  //  <- UnityEngine.GUI::BringWindowToFront
    (const void*)&GUI_CUSTOM_BringWindowToBack            ,  //  <- UnityEngine.GUI::BringWindowToBack
    (const void*)&GUI_CUSTOM_FocusWindow                  ,  //  <- UnityEngine.GUI::FocusWindow
    (const void*)&GUI_CUSTOM_UnfocusWindow                ,  //  <- UnityEngine.GUI::UnfocusWindow
    (const void*)&GUI_CUSTOM_Internal_BeginWindows        ,  //  <- UnityEngine.GUI::Internal_BeginWindows
    (const void*)&GUI_CUSTOM_Internal_EndWindows          ,  //  <- UnityEngine.GUI::Internal_EndWindows
    NULL
};

void ExportGUIBindings();
void ExportGUIBindings()
{
    for (int i = 0; s_GUI_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GUI_IcallNames [i], s_GUI_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportGUIBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_get_color); //  <- UnityEngine.GUI::INTERNAL_get_color
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_set_color); //  <- UnityEngine.GUI::INTERNAL_set_color
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_get_backgroundColor); //  <- UnityEngine.GUI::INTERNAL_get_backgroundColor
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_set_backgroundColor); //  <- UnityEngine.GUI::INTERNAL_set_backgroundColor
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_get_contentColor); //  <- UnityEngine.GUI::INTERNAL_get_contentColor
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_set_contentColor); //  <- UnityEngine.GUI::INTERNAL_set_contentColor
    SET_METRO_BINDING(GUI_Get_Custom_PropChanged); //  <- UnityEngine.GUI::get_changed
    SET_METRO_BINDING(GUI_Set_Custom_PropChanged); //  <- UnityEngine.GUI::set_changed
    SET_METRO_BINDING(GUI_Get_Custom_PropEnabled); //  <- UnityEngine.GUI::get_enabled
    SET_METRO_BINDING(GUI_Set_Custom_PropEnabled); //  <- UnityEngine.GUI::set_enabled
    SET_METRO_BINDING(GUI_CUSTOM_Internal_GetTooltip); //  <- UnityEngine.GUI::Internal_GetTooltip
    SET_METRO_BINDING(GUI_CUSTOM_Internal_SetTooltip); //  <- UnityEngine.GUI::Internal_SetTooltip
    SET_METRO_BINDING(GUI_CUSTOM_Internal_GetMouseTooltip); //  <- UnityEngine.GUI::Internal_GetMouseTooltip
    SET_METRO_BINDING(GUI_Get_Custom_PropDepth); //  <- UnityEngine.GUI::get_depth
    SET_METRO_BINDING(GUI_Set_Custom_PropDepth); //  <- UnityEngine.GUI::set_depth
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_CALL_DoLabel); //  <- UnityEngine.GUI::INTERNAL_CALL_DoLabel
    SET_METRO_BINDING(GUI_CUSTOM_InitializeGUIClipTexture); //  <- UnityEngine.GUI::InitializeGUIClipTexture
    SET_METRO_BINDING(GUI_Get_Custom_PropBlendMaterial); //  <- UnityEngine.GUI::get_blendMaterial
    SET_METRO_BINDING(GUI_Get_Custom_PropBlitMaterial); //  <- UnityEngine.GUI::get_blitMaterial
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_CALL_DoButton); //  <- UnityEngine.GUI::INTERNAL_CALL_DoButton
    SET_METRO_BINDING(GUI_CUSTOM_SetNextControlName); //  <- UnityEngine.GUI::SetNextControlName
    SET_METRO_BINDING(GUI_CUSTOM_GetNameOfFocusedControl); //  <- UnityEngine.GUI::GetNameOfFocusedControl
    SET_METRO_BINDING(GUI_CUSTOM_FocusControl); //  <- UnityEngine.GUI::FocusControl
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_CALL_DoToggle); //  <- UnityEngine.GUI::INTERNAL_CALL_DoToggle
    SET_METRO_BINDING(GUI_Get_Custom_PropUsePageScrollbars); //  <- UnityEngine.GUI::get_usePageScrollbars
    SET_METRO_BINDING(GUI_CUSTOM_InternalRepaintEditorWindow); //  <- UnityEngine.GUI::InternalRepaintEditorWindow
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_CALL_DoWindow); //  <- UnityEngine.GUI::INTERNAL_CALL_DoWindow
    SET_METRO_BINDING(GUI_CUSTOM_INTERNAL_CALL_DragWindow); //  <- UnityEngine.GUI::INTERNAL_CALL_DragWindow
    SET_METRO_BINDING(GUI_CUSTOM_BringWindowToFront); //  <- UnityEngine.GUI::BringWindowToFront
    SET_METRO_BINDING(GUI_CUSTOM_BringWindowToBack); //  <- UnityEngine.GUI::BringWindowToBack
    SET_METRO_BINDING(GUI_CUSTOM_FocusWindow); //  <- UnityEngine.GUI::FocusWindow
    SET_METRO_BINDING(GUI_CUSTOM_UnfocusWindow); //  <- UnityEngine.GUI::UnfocusWindow
    SET_METRO_BINDING(GUI_CUSTOM_Internal_BeginWindows); //  <- UnityEngine.GUI::Internal_BeginWindows
    SET_METRO_BINDING(GUI_CUSTOM_Internal_EndWindows); //  <- UnityEngine.GUI::Internal_EndWindows
}

#endif
