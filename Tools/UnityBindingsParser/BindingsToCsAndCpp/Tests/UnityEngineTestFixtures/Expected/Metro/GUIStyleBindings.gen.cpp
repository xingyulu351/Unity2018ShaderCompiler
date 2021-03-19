#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/IMGUI/GUIStyle.h"
#include "Runtime/IMGUI/GUIContent.h"
#include "Runtime/IMGUI/GUIState.h"
#include "Runtime/IMGUI/GUIManager.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Filters/Misc/Font.h"
#include "Runtime/Scripting/ScriptingUtility.h"

#if UNITY_EDITOR

#endif

using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyleState_CUSTOM_Init(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyleState_CUSTOM_Init)
    ScriptingObjectWithIntPtrField<GUIStyleState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Init)
    
    		self.SetPtr(new GUIStyleState());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyleState_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyleState_CUSTOM_Cleanup)
    ScriptingObjectWithIntPtrField<GUIStyleState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
    
    		delete self.GetPtr();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUIStyleState_Get_Custom_PropBackground(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyleState_Get_Custom_PropBackground)
    ScriptingObjectWithIntPtrField<GUIStyleState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_background)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_background)
    return Scripting::ScriptingWrapperFor (self->background);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyleState_Set_Custom_PropBackground(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyleState_Set_Custom_PropBackground)
    ScriptingObjectWithIntPtrField<GUIStyleState> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Texture2D> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_background)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_background)
     self->background = (Texture2D*) (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyleState_CUSTOM_INTERNAL_get_textColor(ICallType_Object_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyleState_CUSTOM_INTERNAL_get_textColor)
    ScriptingObjectWithIntPtrField<GUIStyleState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_textColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_textColor)
    { *returnValue =(self->textColor); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyleState_CUSTOM_INTERNAL_set_textColor(ICallType_Object_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyleState_CUSTOM_INTERNAL_set_textColor)
    ScriptingObjectWithIntPtrField<GUIStyleState> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_textColor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_textColor)
     self->textColor = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_CUSTOM_Init(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_CUSTOM_Init)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Init)
    
    		self.SetPtr(new RectOffset());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_CUSTOM_Cleanup)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
    
    		delete self.GetPtr();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RectOffset_Get_Custom_PropLeft(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Get_Custom_PropLeft)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_left)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_left)
    return self->left;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_Set_Custom_PropLeft(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Set_Custom_PropLeft)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_left)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_left)
     self->left = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RectOffset_Get_Custom_PropRight(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Get_Custom_PropRight)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_right)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_right)
    return self->right;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_Set_Custom_PropRight(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Set_Custom_PropRight)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_right)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_right)
     self->right = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RectOffset_Get_Custom_PropTop(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Get_Custom_PropTop)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_top)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_top)
    return self->top;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_Set_Custom_PropTop(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Set_Custom_PropTop)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_top)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_top)
     self->top = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RectOffset_Get_Custom_PropBottom(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Get_Custom_PropBottom)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bottom)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bottom)
    return self->bottom;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_Set_Custom_PropBottom(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Set_Custom_PropBottom)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bottom)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bottom)
     self->bottom = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RectOffset_Get_Custom_PropHorizontal(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Get_Custom_PropHorizontal)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_horizontal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_horizontal)
    return self->left + self->right;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION RectOffset_Get_Custom_PropVertical(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_Get_Custom_PropVertical)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_vertical)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_vertical)
    return self->top + self->bottom;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_CUSTOM_INTERNAL_CALL_Add(ICallType_Object_Argument self_, const Rectf& rect, Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_CUSTOM_INTERNAL_CALL_Add)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Add)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Add)
     { returnValue =(Rectf (rect.x - self->left, rect.y - self->top, rect.width + self->left + self->right, rect.height + self->top + self->bottom)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION RectOffset_CUSTOM_INTERNAL_CALL_Remove(ICallType_Object_Argument self_, const Rectf& rect, Rectf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(RectOffset_CUSTOM_INTERNAL_CALL_Remove)
    ScriptingObjectWithIntPtrField<RectOffset> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Remove)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Remove)
     { returnValue =(Rectf (rect.x + self->left, rect.y + self->top, rect.width - self->left - self->right, rect.height - self->top - self->bottom)); return; } 
}




struct MonoInternal_DrawArguments {
	void* target;
	const Rectf position;
	int isHover;
	int isActive;
	int on;
	int hasKeyboardFocus;
};


struct MonoInternal_DrawWithTextSelectionArguments {
	void* target;
	Rectf position;
	int firstPos;
	int lastPos;
	const ColorRGBAf cursorColor;
	const ColorRGBAf selectionColor;
	int isHover;
	int isActive;
	int on;
	int hasKeyboardFocus;
	int drawSelectionAsComposition;
};

 static void CleanupGUIStyle(void* guiStyle){ delete ((GUIStyle*)guiStyle); }
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Init(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Init)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Init)
    
    		self.SetPtr(new GUIStyle(), CleanupGUIStyle);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_InitCopy(ICallType_Object_Argument self_, ICallType_Object_Argument other_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_InitCopy)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    ScriptingObjectWithIntPtrField<GUIStyle> other(other_);
    UNUSED(other);
    SCRIPTINGAPI_STACK_CHECK(InitCopy)
    
    		self.SetPtr(new GUIStyle (*other), CleanupGUIStyle);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Cleanup)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
    
    		CleanupGUIStyle(self.GetPtr());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropName(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropName)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_name)
    return CreateScriptingString(self->m_Name);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropName(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropName)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_name)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_name)
     self->m_Name = value.ToUTF8(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_CALL_GetStyleStatePtr(ICallType_Object_Argument self_, int idx, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_CALL_GetStyleStatePtr)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetStyleStatePtr)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetStyleStatePtr)
    
    		GUIStyleState* gss = &(self->m_Normal);
    		{ returnValue =(gss+idx); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_AssignStyleState(ICallType_Object_Argument self_, int idx, ICallType_IntPtr_Argument srcStyleState_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_AssignStyleState)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    void* srcStyleState(srcStyleState_);
    UNUSED(srcStyleState);
    SCRIPTINGAPI_STACK_CHECK(AssignStyleState)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AssignStyleState)
     
    		GUIStyleState* gss = &(self->m_Normal);
    		gss += idx;
    		*gss = *((GUIStyleState*)srcStyleState);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_CALL_GetRectOffsetPtr(ICallType_Object_Argument self_, int idx, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_CALL_GetRectOffsetPtr)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetRectOffsetPtr)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetRectOffsetPtr)
    
    		RectOffset* ro = &(self->m_Border);
    		{ returnValue =(ro+idx); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_AssignRectOffset(ICallType_Object_Argument self_, int idx, ICallType_IntPtr_Argument srcRectOffset_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_AssignRectOffset)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    void* srcRectOffset(srcRectOffset_);
    UNUSED(srcRectOffset);
    SCRIPTINGAPI_STACK_CHECK(AssignRectOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AssignRectOffset)
     
    		RectOffset* ro = &(self->m_Border);
    		ro += idx;
    		*ro = *((RectOffset*)srcRectOffset);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropImagePosition(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropImagePosition)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_imagePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_imagePosition)
    return self->m_ImagePosition;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropImagePosition(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropImagePosition)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_imagePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_imagePosition)
     self->m_ImagePosition = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropAlignment(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropAlignment)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alignment)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_alignment)
    return self->m_Alignment;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropAlignment(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropAlignment)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_alignment)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_alignment)
     self->m_Alignment = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropWordWrap(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropWordWrap)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wordWrap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wordWrap)
    return self->m_WordWrap;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropWordWrap(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropWordWrap)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wordWrap)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wordWrap)
     self->m_WordWrap = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropClipping(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropClipping)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clipping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clipping)
    return self->m_Clipping;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropClipping(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropClipping)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_clipping)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_clipping)
     self->m_Clipping = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_get_contentOffset(ICallType_Object_Argument self_, Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_get_contentOffset)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_contentOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_contentOffset)
    { *returnValue =(self->m_ContentOffset); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_set_contentOffset(ICallType_Object_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_set_contentOffset)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_contentOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_contentOffset)
     self->m_ContentOffset = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_get_Internal_clipOffset(ICallType_Object_Argument self_, Vector2fIcall* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_get_Internal_clipOffset)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_Internal_clipOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_Internal_clipOffset)
    { *returnValue =(self->m_ClipOffset); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_set_Internal_clipOffset(ICallType_Object_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_set_Internal_clipOffset)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_Internal_clipOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_set_Internal_clipOffset)
     self->m_ClipOffset = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropFixedWidth(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropFixedWidth)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fixedWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fixedWidth)
    return self->m_FixedWidth;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropFixedWidth(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropFixedWidth)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fixedWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fixedWidth)
     self->m_FixedWidth = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropFixedHeight(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropFixedHeight)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fixedHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fixedHeight)
    return self->m_FixedHeight;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropFixedHeight(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropFixedHeight)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fixedHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fixedHeight)
     self->m_FixedHeight = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropStretchWidth(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropStretchWidth)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stretchWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stretchWidth)
    return self->m_StretchWidth;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropStretchWidth(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropStretchWidth)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stretchWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stretchWidth)
     self->m_StretchWidth = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropStretchHeight(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropStretchHeight)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stretchHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stretchHeight)
    return self->m_StretchHeight;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropStretchHeight(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropStretchHeight)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stretchHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stretchHeight)
     self->m_StretchHeight = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_GetLineHeight(ICallType_IntPtr_Argument target_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_GetLineHeight)
    void* target(target_);
    UNUSED(target);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetLineHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetLineHeight)
    
    		GUIStyle *cache = reinterpret_cast<GUIStyle*> (target);
    		return cache->GetLineHeight ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropFont(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropFont)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_font)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_font)
    return Scripting::ScriptingWrapperFor (self->m_Font);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropFont(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropFont)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Font> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_font)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_font)
     self->m_Font = (Font*)value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropFontSize(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropFontSize)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontSize)
    return self->m_FontSize;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropFontSize(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropFontSize)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fontSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontSize)
     self->m_FontSize = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropFontStyle(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropFontStyle)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_fontStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fontStyle)
    return self->m_FontStyle;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropFontStyle(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropFontStyle)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_fontStyle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fontStyle)
     self->m_FontStyle = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GUIStyle_Get_Custom_PropRichText(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Get_Custom_PropRichText)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_richText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_richText)
    return self->m_RichText;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_Set_Custom_PropRichText(ICallType_Object_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_Set_Custom_PropRichText)
    ScriptingObjectWithIntPtrField<GUIStyle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_richText)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_richText)
     self->m_RichText = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_Draw(ICallType_Object_Argument content_, MonoInternal_DrawArguments& arguments)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_Draw)
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(Internal_Draw)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Draw)
    
    		reinterpret_cast<GUIStyle*> (arguments.target)->Draw (GetGUIState(), arguments.position, MonoGUIContentToTempNative(content), arguments.isHover, arguments.isActive, arguments.on, arguments.hasKeyboardFocus);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_CALL_Internal_Draw2(ICallType_IntPtr_Argument style_, const Rectf& position, ICallType_Object_Argument content_, int controlID, ScriptingBool on)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_Draw2)
    void* style(style_);
    UNUSED(style);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Draw2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Draw2)
    
    		GUIStyle *_style = reinterpret_cast<GUIStyle*>(style);
    		_style->Draw (GetGUIState(), position, MonoGUIContentToTempNative (content), controlID, on);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_GetCursorFlashOffset()
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_GetCursorFlashOffset)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetCursorFlashOffset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetCursorFlashOffset)
    
    		return GUIManager::GetCursorFlashTime ();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_CALL_Internal_DrawCursor(ICallType_IntPtr_Argument target_, const Rectf& position, ICallType_Object_Argument content_, int pos, const ColorRGBAf& cursorColor)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_DrawCursor)
    void* target(target_);
    UNUSED(target);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_DrawCursor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_DrawCursor)
     reinterpret_cast<GUIStyle*> (target)->DrawCursor (GetGUIState(), position, MonoGUIContentToTempNative (content), pos, cursorColor); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_DrawWithTextSelection(ICallType_Object_Argument content_, MonoInternal_DrawWithTextSelectionArguments& arguments)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_DrawWithTextSelection)
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(Internal_DrawWithTextSelection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_DrawWithTextSelection)
     
    		reinterpret_cast<GUIStyle*> (arguments.target)->DrawWithTextSelection (
    			GetGUIState (), 
    			arguments.position, 
    			MonoGUIContentToTempNative(content), 
    			arguments.isHover, 
    			arguments.isActive, 
    			arguments.on, 
    			arguments.hasKeyboardFocus, 
    			arguments.drawSelectionAsComposition, 
    			arguments.firstPos, 
    			arguments.lastPos, 
    			arguments.cursorColor, 
    			arguments.selectionColor
    		); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_SetDefaultFont(ICallType_ReadOnlyUnityEngineObject_Argument font_)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_SetDefaultFont)
    ReadOnlyScriptingObjectOfType<Font> font(font_);
    UNUSED(font);
    SCRIPTINGAPI_STACK_CHECK(SetDefaultFont)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetDefaultFont)
     GUIStyle::SetDefaultFont (font); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorPixelPosition(ICallType_IntPtr_Argument target_, const Rectf& position, ICallType_Object_Argument content_, int cursorStringIndex, Vector2fIcall* ret)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorPixelPosition)
    void* target(target_);
    UNUSED(target);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_GetCursorPixelPosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_GetCursorPixelPosition)
    
    			*ret = reinterpret_cast<GUIStyle*> (target)->GetCursorPixelPosition (position, MonoGUIContentToTempNative (content), cursorStringIndex);
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorStringIndex(ICallType_IntPtr_Argument target_, const Rectf& position, ICallType_Object_Argument content_, const Vector2fIcall& cursorPixelPosition)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorStringIndex)
    void* target(target_);
    UNUSED(target);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_GetCursorStringIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_GetCursorStringIndex)
    
    			return reinterpret_cast<GUIStyle*> (target)->GetCursorStringIndex (position, MonoGUIContentToTempNative (content), cursorPixelPosition);
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_GetNumCharactersThatFitWithinWidth(ICallType_IntPtr_Argument target_, ICallType_String_Argument text_, float width)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_GetNumCharactersThatFitWithinWidth)
    void* target(target_);
    UNUSED(target);
    ICallType_String_Local text(text_);
    UNUSED(text);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetNumCharactersThatFitWithinWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetNumCharactersThatFitWithinWidth)
    
    #if UNITY_FLASH
    		return 5;
    #else	
    		return reinterpret_cast<GUIStyle*> (target)->GetNumCharactersThatFitWithinWidth (UTF16String (text.ToUTF8().c_str()), width);
    #endif		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_CalcSize(ICallType_IntPtr_Argument target_, ICallType_Object_Argument content_, Vector2fIcall* ret)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_CalcSize)
    void* target(target_);
    UNUSED(target);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(Internal_CalcSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CalcSize)
    
    			*ret = reinterpret_cast<GUIStyle*> (target)->CalcSize (MonoGUIContentToTempNative (content));
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_CalcHeight(ICallType_IntPtr_Argument target_, ICallType_Object_Argument content_, float width)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_CalcHeight)
    void* target(target_);
    UNUSED(target);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(Internal_CalcHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CalcHeight)
    
    		return reinterpret_cast<GUIStyle*> (target)->CalcHeight (MonoGUIContentToTempNative (content), width);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GUIStyle_CUSTOM_Internal_CalcMinMaxWidth(ICallType_IntPtr_Argument target_, ICallType_Object_Argument content_, float* minWidth, float* maxWidth)
{
    SCRIPTINGAPI_ETW_ENTRY(GUIStyle_CUSTOM_Internal_CalcMinMaxWidth)
    void* target(target_);
    UNUSED(target);
    ICallType_Object_Local content(content_);
    UNUSED(content);
    SCRIPTINGAPI_STACK_CHECK(Internal_CalcMinMaxWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CalcMinMaxWidth)
    
    			reinterpret_cast<GUIStyle*> (target)->CalcMinMaxWidth (MonoGUIContentToTempNative (content), minWidth, maxWidth);
    		
}


 #undef GET

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_GUIStyleState_Init()
{
    scripting_add_internal_call( "UnityEngine.GUIStyleState::Init" , (gpointer)& GUIStyleState_CUSTOM_Init );
}

void Register_UnityEngine_GUIStyleState_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.GUIStyleState::Cleanup" , (gpointer)& GUIStyleState_CUSTOM_Cleanup );
}

void Register_UnityEngine_GUIStyleState_get_background()
{
    scripting_add_internal_call( "UnityEngine.GUIStyleState::get_background" , (gpointer)& GUIStyleState_Get_Custom_PropBackground );
}

void Register_UnityEngine_GUIStyleState_set_background()
{
    scripting_add_internal_call( "UnityEngine.GUIStyleState::set_background" , (gpointer)& GUIStyleState_Set_Custom_PropBackground );
}

void Register_UnityEngine_GUIStyleState_INTERNAL_get_textColor()
{
    scripting_add_internal_call( "UnityEngine.GUIStyleState::INTERNAL_get_textColor" , (gpointer)& GUIStyleState_CUSTOM_INTERNAL_get_textColor );
}

void Register_UnityEngine_GUIStyleState_INTERNAL_set_textColor()
{
    scripting_add_internal_call( "UnityEngine.GUIStyleState::INTERNAL_set_textColor" , (gpointer)& GUIStyleState_CUSTOM_INTERNAL_set_textColor );
}

void Register_UnityEngine_RectOffset_Init()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::Init" , (gpointer)& RectOffset_CUSTOM_Init );
}

void Register_UnityEngine_RectOffset_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::Cleanup" , (gpointer)& RectOffset_CUSTOM_Cleanup );
}

void Register_UnityEngine_RectOffset_get_left()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::get_left" , (gpointer)& RectOffset_Get_Custom_PropLeft );
}

void Register_UnityEngine_RectOffset_set_left()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::set_left" , (gpointer)& RectOffset_Set_Custom_PropLeft );
}

void Register_UnityEngine_RectOffset_get_right()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::get_right" , (gpointer)& RectOffset_Get_Custom_PropRight );
}

void Register_UnityEngine_RectOffset_set_right()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::set_right" , (gpointer)& RectOffset_Set_Custom_PropRight );
}

void Register_UnityEngine_RectOffset_get_top()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::get_top" , (gpointer)& RectOffset_Get_Custom_PropTop );
}

void Register_UnityEngine_RectOffset_set_top()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::set_top" , (gpointer)& RectOffset_Set_Custom_PropTop );
}

void Register_UnityEngine_RectOffset_get_bottom()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::get_bottom" , (gpointer)& RectOffset_Get_Custom_PropBottom );
}

void Register_UnityEngine_RectOffset_set_bottom()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::set_bottom" , (gpointer)& RectOffset_Set_Custom_PropBottom );
}

void Register_UnityEngine_RectOffset_get_horizontal()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::get_horizontal" , (gpointer)& RectOffset_Get_Custom_PropHorizontal );
}

void Register_UnityEngine_RectOffset_get_vertical()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::get_vertical" , (gpointer)& RectOffset_Get_Custom_PropVertical );
}

void Register_UnityEngine_RectOffset_INTERNAL_CALL_Add()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::INTERNAL_CALL_Add" , (gpointer)& RectOffset_CUSTOM_INTERNAL_CALL_Add );
}

void Register_UnityEngine_RectOffset_INTERNAL_CALL_Remove()
{
    scripting_add_internal_call( "UnityEngine.RectOffset::INTERNAL_CALL_Remove" , (gpointer)& RectOffset_CUSTOM_INTERNAL_CALL_Remove );
}

void Register_UnityEngine_GUIStyle_Init()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Init" , (gpointer)& GUIStyle_CUSTOM_Init );
}

void Register_UnityEngine_GUIStyle_InitCopy()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::InitCopy" , (gpointer)& GUIStyle_CUSTOM_InitCopy );
}

void Register_UnityEngine_GUIStyle_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Cleanup" , (gpointer)& GUIStyle_CUSTOM_Cleanup );
}

void Register_UnityEngine_GUIStyle_get_name()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_name" , (gpointer)& GUIStyle_Get_Custom_PropName );
}

void Register_UnityEngine_GUIStyle_set_name()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_name" , (gpointer)& GUIStyle_Set_Custom_PropName );
}

void Register_UnityEngine_GUIStyle_INTERNAL_CALL_GetStyleStatePtr()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_CALL_GetStyleStatePtr" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_CALL_GetStyleStatePtr );
}

void Register_UnityEngine_GUIStyle_AssignStyleState()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::AssignStyleState" , (gpointer)& GUIStyle_CUSTOM_AssignStyleState );
}

void Register_UnityEngine_GUIStyle_INTERNAL_CALL_GetRectOffsetPtr()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_CALL_GetRectOffsetPtr" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_CALL_GetRectOffsetPtr );
}

void Register_UnityEngine_GUIStyle_AssignRectOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::AssignRectOffset" , (gpointer)& GUIStyle_CUSTOM_AssignRectOffset );
}

void Register_UnityEngine_GUIStyle_get_imagePosition()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_imagePosition" , (gpointer)& GUIStyle_Get_Custom_PropImagePosition );
}

void Register_UnityEngine_GUIStyle_set_imagePosition()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_imagePosition" , (gpointer)& GUIStyle_Set_Custom_PropImagePosition );
}

void Register_UnityEngine_GUIStyle_get_alignment()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_alignment" , (gpointer)& GUIStyle_Get_Custom_PropAlignment );
}

void Register_UnityEngine_GUIStyle_set_alignment()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_alignment" , (gpointer)& GUIStyle_Set_Custom_PropAlignment );
}

void Register_UnityEngine_GUIStyle_get_wordWrap()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_wordWrap" , (gpointer)& GUIStyle_Get_Custom_PropWordWrap );
}

void Register_UnityEngine_GUIStyle_set_wordWrap()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_wordWrap" , (gpointer)& GUIStyle_Set_Custom_PropWordWrap );
}

void Register_UnityEngine_GUIStyle_get_clipping()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_clipping" , (gpointer)& GUIStyle_Get_Custom_PropClipping );
}

void Register_UnityEngine_GUIStyle_set_clipping()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_clipping" , (gpointer)& GUIStyle_Set_Custom_PropClipping );
}

void Register_UnityEngine_GUIStyle_INTERNAL_get_contentOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_get_contentOffset" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_get_contentOffset );
}

void Register_UnityEngine_GUIStyle_INTERNAL_set_contentOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_set_contentOffset" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_set_contentOffset );
}

void Register_UnityEngine_GUIStyle_INTERNAL_get_Internal_clipOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_get_Internal_clipOffset" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_get_Internal_clipOffset );
}

void Register_UnityEngine_GUIStyle_INTERNAL_set_Internal_clipOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_set_Internal_clipOffset" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_set_Internal_clipOffset );
}

void Register_UnityEngine_GUIStyle_get_fixedWidth()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_fixedWidth" , (gpointer)& GUIStyle_Get_Custom_PropFixedWidth );
}

void Register_UnityEngine_GUIStyle_set_fixedWidth()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_fixedWidth" , (gpointer)& GUIStyle_Set_Custom_PropFixedWidth );
}

void Register_UnityEngine_GUIStyle_get_fixedHeight()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_fixedHeight" , (gpointer)& GUIStyle_Get_Custom_PropFixedHeight );
}

void Register_UnityEngine_GUIStyle_set_fixedHeight()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_fixedHeight" , (gpointer)& GUIStyle_Set_Custom_PropFixedHeight );
}

void Register_UnityEngine_GUIStyle_get_stretchWidth()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_stretchWidth" , (gpointer)& GUIStyle_Get_Custom_PropStretchWidth );
}

void Register_UnityEngine_GUIStyle_set_stretchWidth()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_stretchWidth" , (gpointer)& GUIStyle_Set_Custom_PropStretchWidth );
}

void Register_UnityEngine_GUIStyle_get_stretchHeight()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_stretchHeight" , (gpointer)& GUIStyle_Get_Custom_PropStretchHeight );
}

void Register_UnityEngine_GUIStyle_set_stretchHeight()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_stretchHeight" , (gpointer)& GUIStyle_Set_Custom_PropStretchHeight );
}

void Register_UnityEngine_GUIStyle_Internal_GetLineHeight()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_GetLineHeight" , (gpointer)& GUIStyle_CUSTOM_Internal_GetLineHeight );
}

void Register_UnityEngine_GUIStyle_get_font()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_font" , (gpointer)& GUIStyle_Get_Custom_PropFont );
}

void Register_UnityEngine_GUIStyle_set_font()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_font" , (gpointer)& GUIStyle_Set_Custom_PropFont );
}

void Register_UnityEngine_GUIStyle_get_fontSize()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_fontSize" , (gpointer)& GUIStyle_Get_Custom_PropFontSize );
}

void Register_UnityEngine_GUIStyle_set_fontSize()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_fontSize" , (gpointer)& GUIStyle_Set_Custom_PropFontSize );
}

void Register_UnityEngine_GUIStyle_get_fontStyle()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_fontStyle" , (gpointer)& GUIStyle_Get_Custom_PropFontStyle );
}

void Register_UnityEngine_GUIStyle_set_fontStyle()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_fontStyle" , (gpointer)& GUIStyle_Set_Custom_PropFontStyle );
}

void Register_UnityEngine_GUIStyle_get_richText()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::get_richText" , (gpointer)& GUIStyle_Get_Custom_PropRichText );
}

void Register_UnityEngine_GUIStyle_set_richText()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::set_richText" , (gpointer)& GUIStyle_Set_Custom_PropRichText );
}

void Register_UnityEngine_GUIStyle_Internal_Draw()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_Draw" , (gpointer)& GUIStyle_CUSTOM_Internal_Draw );
}

void Register_UnityEngine_GUIStyle_INTERNAL_CALL_Internal_Draw2()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_Draw2" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_CALL_Internal_Draw2 );
}

void Register_UnityEngine_GUIStyle_Internal_GetCursorFlashOffset()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_GetCursorFlashOffset" , (gpointer)& GUIStyle_CUSTOM_Internal_GetCursorFlashOffset );
}

void Register_UnityEngine_GUIStyle_INTERNAL_CALL_Internal_DrawCursor()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_DrawCursor" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_CALL_Internal_DrawCursor );
}

void Register_UnityEngine_GUIStyle_Internal_DrawWithTextSelection()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_DrawWithTextSelection" , (gpointer)& GUIStyle_CUSTOM_Internal_DrawWithTextSelection );
}

void Register_UnityEngine_GUIStyle_SetDefaultFont()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::SetDefaultFont" , (gpointer)& GUIStyle_CUSTOM_SetDefaultFont );
}

void Register_UnityEngine_GUIStyle_INTERNAL_CALL_Internal_GetCursorPixelPosition()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorPixelPosition" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorPixelPosition );
}

void Register_UnityEngine_GUIStyle_INTERNAL_CALL_Internal_GetCursorStringIndex()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorStringIndex" , (gpointer)& GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorStringIndex );
}

void Register_UnityEngine_GUIStyle_Internal_GetNumCharactersThatFitWithinWidth()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_GetNumCharactersThatFitWithinWidth" , (gpointer)& GUIStyle_CUSTOM_Internal_GetNumCharactersThatFitWithinWidth );
}

void Register_UnityEngine_GUIStyle_Internal_CalcSize()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_CalcSize" , (gpointer)& GUIStyle_CUSTOM_Internal_CalcSize );
}

void Register_UnityEngine_GUIStyle_Internal_CalcHeight()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_CalcHeight" , (gpointer)& GUIStyle_CUSTOM_Internal_CalcHeight );
}

void Register_UnityEngine_GUIStyle_Internal_CalcMinMaxWidth()
{
    scripting_add_internal_call( "UnityEngine.GUIStyle::Internal_CalcMinMaxWidth" , (gpointer)& GUIStyle_CUSTOM_Internal_CalcMinMaxWidth );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GUIStyle_IcallNames [] =
{
    "UnityEngine.GUIStyleState::Init"       ,    // -> GUIStyleState_CUSTOM_Init
    "UnityEngine.GUIStyleState::Cleanup"    ,    // -> GUIStyleState_CUSTOM_Cleanup
    "UnityEngine.GUIStyleState::get_background",    // -> GUIStyleState_Get_Custom_PropBackground
    "UnityEngine.GUIStyleState::set_background",    // -> GUIStyleState_Set_Custom_PropBackground
    "UnityEngine.GUIStyleState::INTERNAL_get_textColor",    // -> GUIStyleState_CUSTOM_INTERNAL_get_textColor
    "UnityEngine.GUIStyleState::INTERNAL_set_textColor",    // -> GUIStyleState_CUSTOM_INTERNAL_set_textColor
    "UnityEngine.RectOffset::Init"          ,    // -> RectOffset_CUSTOM_Init
    "UnityEngine.RectOffset::Cleanup"       ,    // -> RectOffset_CUSTOM_Cleanup
    "UnityEngine.RectOffset::get_left"      ,    // -> RectOffset_Get_Custom_PropLeft
    "UnityEngine.RectOffset::set_left"      ,    // -> RectOffset_Set_Custom_PropLeft
    "UnityEngine.RectOffset::get_right"     ,    // -> RectOffset_Get_Custom_PropRight
    "UnityEngine.RectOffset::set_right"     ,    // -> RectOffset_Set_Custom_PropRight
    "UnityEngine.RectOffset::get_top"       ,    // -> RectOffset_Get_Custom_PropTop
    "UnityEngine.RectOffset::set_top"       ,    // -> RectOffset_Set_Custom_PropTop
    "UnityEngine.RectOffset::get_bottom"    ,    // -> RectOffset_Get_Custom_PropBottom
    "UnityEngine.RectOffset::set_bottom"    ,    // -> RectOffset_Set_Custom_PropBottom
    "UnityEngine.RectOffset::get_horizontal",    // -> RectOffset_Get_Custom_PropHorizontal
    "UnityEngine.RectOffset::get_vertical"  ,    // -> RectOffset_Get_Custom_PropVertical
    "UnityEngine.RectOffset::INTERNAL_CALL_Add",    // -> RectOffset_CUSTOM_INTERNAL_CALL_Add
    "UnityEngine.RectOffset::INTERNAL_CALL_Remove",    // -> RectOffset_CUSTOM_INTERNAL_CALL_Remove
    "UnityEngine.GUIStyle::Init"            ,    // -> GUIStyle_CUSTOM_Init
    "UnityEngine.GUIStyle::InitCopy"        ,    // -> GUIStyle_CUSTOM_InitCopy
    "UnityEngine.GUIStyle::Cleanup"         ,    // -> GUIStyle_CUSTOM_Cleanup
    "UnityEngine.GUIStyle::get_name"        ,    // -> GUIStyle_Get_Custom_PropName
    "UnityEngine.GUIStyle::set_name"        ,    // -> GUIStyle_Set_Custom_PropName
    "UnityEngine.GUIStyle::INTERNAL_CALL_GetStyleStatePtr",    // -> GUIStyle_CUSTOM_INTERNAL_CALL_GetStyleStatePtr
    "UnityEngine.GUIStyle::AssignStyleState",    // -> GUIStyle_CUSTOM_AssignStyleState
    "UnityEngine.GUIStyle::INTERNAL_CALL_GetRectOffsetPtr",    // -> GUIStyle_CUSTOM_INTERNAL_CALL_GetRectOffsetPtr
    "UnityEngine.GUIStyle::AssignRectOffset",    // -> GUIStyle_CUSTOM_AssignRectOffset
    "UnityEngine.GUIStyle::get_imagePosition",    // -> GUIStyle_Get_Custom_PropImagePosition
    "UnityEngine.GUIStyle::set_imagePosition",    // -> GUIStyle_Set_Custom_PropImagePosition
    "UnityEngine.GUIStyle::get_alignment"   ,    // -> GUIStyle_Get_Custom_PropAlignment
    "UnityEngine.GUIStyle::set_alignment"   ,    // -> GUIStyle_Set_Custom_PropAlignment
    "UnityEngine.GUIStyle::get_wordWrap"    ,    // -> GUIStyle_Get_Custom_PropWordWrap
    "UnityEngine.GUIStyle::set_wordWrap"    ,    // -> GUIStyle_Set_Custom_PropWordWrap
    "UnityEngine.GUIStyle::get_clipping"    ,    // -> GUIStyle_Get_Custom_PropClipping
    "UnityEngine.GUIStyle::set_clipping"    ,    // -> GUIStyle_Set_Custom_PropClipping
    "UnityEngine.GUIStyle::INTERNAL_get_contentOffset",    // -> GUIStyle_CUSTOM_INTERNAL_get_contentOffset
    "UnityEngine.GUIStyle::INTERNAL_set_contentOffset",    // -> GUIStyle_CUSTOM_INTERNAL_set_contentOffset
    "UnityEngine.GUIStyle::INTERNAL_get_Internal_clipOffset",    // -> GUIStyle_CUSTOM_INTERNAL_get_Internal_clipOffset
    "UnityEngine.GUIStyle::INTERNAL_set_Internal_clipOffset",    // -> GUIStyle_CUSTOM_INTERNAL_set_Internal_clipOffset
    "UnityEngine.GUIStyle::get_fixedWidth"  ,    // -> GUIStyle_Get_Custom_PropFixedWidth
    "UnityEngine.GUIStyle::set_fixedWidth"  ,    // -> GUIStyle_Set_Custom_PropFixedWidth
    "UnityEngine.GUIStyle::get_fixedHeight" ,    // -> GUIStyle_Get_Custom_PropFixedHeight
    "UnityEngine.GUIStyle::set_fixedHeight" ,    // -> GUIStyle_Set_Custom_PropFixedHeight
    "UnityEngine.GUIStyle::get_stretchWidth",    // -> GUIStyle_Get_Custom_PropStretchWidth
    "UnityEngine.GUIStyle::set_stretchWidth",    // -> GUIStyle_Set_Custom_PropStretchWidth
    "UnityEngine.GUIStyle::get_stretchHeight",    // -> GUIStyle_Get_Custom_PropStretchHeight
    "UnityEngine.GUIStyle::set_stretchHeight",    // -> GUIStyle_Set_Custom_PropStretchHeight
    "UnityEngine.GUIStyle::Internal_GetLineHeight",    // -> GUIStyle_CUSTOM_Internal_GetLineHeight
    "UnityEngine.GUIStyle::get_font"        ,    // -> GUIStyle_Get_Custom_PropFont
    "UnityEngine.GUIStyle::set_font"        ,    // -> GUIStyle_Set_Custom_PropFont
    "UnityEngine.GUIStyle::get_fontSize"    ,    // -> GUIStyle_Get_Custom_PropFontSize
    "UnityEngine.GUIStyle::set_fontSize"    ,    // -> GUIStyle_Set_Custom_PropFontSize
    "UnityEngine.GUIStyle::get_fontStyle"   ,    // -> GUIStyle_Get_Custom_PropFontStyle
    "UnityEngine.GUIStyle::set_fontStyle"   ,    // -> GUIStyle_Set_Custom_PropFontStyle
    "UnityEngine.GUIStyle::get_richText"    ,    // -> GUIStyle_Get_Custom_PropRichText
    "UnityEngine.GUIStyle::set_richText"    ,    // -> GUIStyle_Set_Custom_PropRichText
    "UnityEngine.GUIStyle::Internal_Draw"   ,    // -> GUIStyle_CUSTOM_Internal_Draw
    "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_Draw2",    // -> GUIStyle_CUSTOM_INTERNAL_CALL_Internal_Draw2
    "UnityEngine.GUIStyle::Internal_GetCursorFlashOffset",    // -> GUIStyle_CUSTOM_Internal_GetCursorFlashOffset
    "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_DrawCursor",    // -> GUIStyle_CUSTOM_INTERNAL_CALL_Internal_DrawCursor
    "UnityEngine.GUIStyle::Internal_DrawWithTextSelection",    // -> GUIStyle_CUSTOM_Internal_DrawWithTextSelection
    "UnityEngine.GUIStyle::SetDefaultFont"  ,    // -> GUIStyle_CUSTOM_SetDefaultFont
    "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorPixelPosition",    // -> GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorPixelPosition
    "UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorStringIndex",    // -> GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorStringIndex
    "UnityEngine.GUIStyle::Internal_GetNumCharactersThatFitWithinWidth",    // -> GUIStyle_CUSTOM_Internal_GetNumCharactersThatFitWithinWidth
    "UnityEngine.GUIStyle::Internal_CalcSize",    // -> GUIStyle_CUSTOM_Internal_CalcSize
    "UnityEngine.GUIStyle::Internal_CalcHeight",    // -> GUIStyle_CUSTOM_Internal_CalcHeight
    "UnityEngine.GUIStyle::Internal_CalcMinMaxWidth",    // -> GUIStyle_CUSTOM_Internal_CalcMinMaxWidth
    NULL
};

static const void* s_GUIStyle_IcallFuncs [] =
{
    (const void*)&GUIStyleState_CUSTOM_Init               ,  //  <- UnityEngine.GUIStyleState::Init
    (const void*)&GUIStyleState_CUSTOM_Cleanup            ,  //  <- UnityEngine.GUIStyleState::Cleanup
    (const void*)&GUIStyleState_Get_Custom_PropBackground ,  //  <- UnityEngine.GUIStyleState::get_background
    (const void*)&GUIStyleState_Set_Custom_PropBackground ,  //  <- UnityEngine.GUIStyleState::set_background
    (const void*)&GUIStyleState_CUSTOM_INTERNAL_get_textColor,  //  <- UnityEngine.GUIStyleState::INTERNAL_get_textColor
    (const void*)&GUIStyleState_CUSTOM_INTERNAL_set_textColor,  //  <- UnityEngine.GUIStyleState::INTERNAL_set_textColor
    (const void*)&RectOffset_CUSTOM_Init                  ,  //  <- UnityEngine.RectOffset::Init
    (const void*)&RectOffset_CUSTOM_Cleanup               ,  //  <- UnityEngine.RectOffset::Cleanup
    (const void*)&RectOffset_Get_Custom_PropLeft          ,  //  <- UnityEngine.RectOffset::get_left
    (const void*)&RectOffset_Set_Custom_PropLeft          ,  //  <- UnityEngine.RectOffset::set_left
    (const void*)&RectOffset_Get_Custom_PropRight         ,  //  <- UnityEngine.RectOffset::get_right
    (const void*)&RectOffset_Set_Custom_PropRight         ,  //  <- UnityEngine.RectOffset::set_right
    (const void*)&RectOffset_Get_Custom_PropTop           ,  //  <- UnityEngine.RectOffset::get_top
    (const void*)&RectOffset_Set_Custom_PropTop           ,  //  <- UnityEngine.RectOffset::set_top
    (const void*)&RectOffset_Get_Custom_PropBottom        ,  //  <- UnityEngine.RectOffset::get_bottom
    (const void*)&RectOffset_Set_Custom_PropBottom        ,  //  <- UnityEngine.RectOffset::set_bottom
    (const void*)&RectOffset_Get_Custom_PropHorizontal    ,  //  <- UnityEngine.RectOffset::get_horizontal
    (const void*)&RectOffset_Get_Custom_PropVertical      ,  //  <- UnityEngine.RectOffset::get_vertical
    (const void*)&RectOffset_CUSTOM_INTERNAL_CALL_Add     ,  //  <- UnityEngine.RectOffset::INTERNAL_CALL_Add
    (const void*)&RectOffset_CUSTOM_INTERNAL_CALL_Remove  ,  //  <- UnityEngine.RectOffset::INTERNAL_CALL_Remove
    (const void*)&GUIStyle_CUSTOM_Init                    ,  //  <- UnityEngine.GUIStyle::Init
    (const void*)&GUIStyle_CUSTOM_InitCopy                ,  //  <- UnityEngine.GUIStyle::InitCopy
    (const void*)&GUIStyle_CUSTOM_Cleanup                 ,  //  <- UnityEngine.GUIStyle::Cleanup
    (const void*)&GUIStyle_Get_Custom_PropName            ,  //  <- UnityEngine.GUIStyle::get_name
    (const void*)&GUIStyle_Set_Custom_PropName            ,  //  <- UnityEngine.GUIStyle::set_name
    (const void*)&GUIStyle_CUSTOM_INTERNAL_CALL_GetStyleStatePtr,  //  <- UnityEngine.GUIStyle::INTERNAL_CALL_GetStyleStatePtr
    (const void*)&GUIStyle_CUSTOM_AssignStyleState        ,  //  <- UnityEngine.GUIStyle::AssignStyleState
    (const void*)&GUIStyle_CUSTOM_INTERNAL_CALL_GetRectOffsetPtr,  //  <- UnityEngine.GUIStyle::INTERNAL_CALL_GetRectOffsetPtr
    (const void*)&GUIStyle_CUSTOM_AssignRectOffset        ,  //  <- UnityEngine.GUIStyle::AssignRectOffset
    (const void*)&GUIStyle_Get_Custom_PropImagePosition   ,  //  <- UnityEngine.GUIStyle::get_imagePosition
    (const void*)&GUIStyle_Set_Custom_PropImagePosition   ,  //  <- UnityEngine.GUIStyle::set_imagePosition
    (const void*)&GUIStyle_Get_Custom_PropAlignment       ,  //  <- UnityEngine.GUIStyle::get_alignment
    (const void*)&GUIStyle_Set_Custom_PropAlignment       ,  //  <- UnityEngine.GUIStyle::set_alignment
    (const void*)&GUIStyle_Get_Custom_PropWordWrap        ,  //  <- UnityEngine.GUIStyle::get_wordWrap
    (const void*)&GUIStyle_Set_Custom_PropWordWrap        ,  //  <- UnityEngine.GUIStyle::set_wordWrap
    (const void*)&GUIStyle_Get_Custom_PropClipping        ,  //  <- UnityEngine.GUIStyle::get_clipping
    (const void*)&GUIStyle_Set_Custom_PropClipping        ,  //  <- UnityEngine.GUIStyle::set_clipping
    (const void*)&GUIStyle_CUSTOM_INTERNAL_get_contentOffset,  //  <- UnityEngine.GUIStyle::INTERNAL_get_contentOffset
    (const void*)&GUIStyle_CUSTOM_INTERNAL_set_contentOffset,  //  <- UnityEngine.GUIStyle::INTERNAL_set_contentOffset
    (const void*)&GUIStyle_CUSTOM_INTERNAL_get_Internal_clipOffset,  //  <- UnityEngine.GUIStyle::INTERNAL_get_Internal_clipOffset
    (const void*)&GUIStyle_CUSTOM_INTERNAL_set_Internal_clipOffset,  //  <- UnityEngine.GUIStyle::INTERNAL_set_Internal_clipOffset
    (const void*)&GUIStyle_Get_Custom_PropFixedWidth      ,  //  <- UnityEngine.GUIStyle::get_fixedWidth
    (const void*)&GUIStyle_Set_Custom_PropFixedWidth      ,  //  <- UnityEngine.GUIStyle::set_fixedWidth
    (const void*)&GUIStyle_Get_Custom_PropFixedHeight     ,  //  <- UnityEngine.GUIStyle::get_fixedHeight
    (const void*)&GUIStyle_Set_Custom_PropFixedHeight     ,  //  <- UnityEngine.GUIStyle::set_fixedHeight
    (const void*)&GUIStyle_Get_Custom_PropStretchWidth    ,  //  <- UnityEngine.GUIStyle::get_stretchWidth
    (const void*)&GUIStyle_Set_Custom_PropStretchWidth    ,  //  <- UnityEngine.GUIStyle::set_stretchWidth
    (const void*)&GUIStyle_Get_Custom_PropStretchHeight   ,  //  <- UnityEngine.GUIStyle::get_stretchHeight
    (const void*)&GUIStyle_Set_Custom_PropStretchHeight   ,  //  <- UnityEngine.GUIStyle::set_stretchHeight
    (const void*)&GUIStyle_CUSTOM_Internal_GetLineHeight  ,  //  <- UnityEngine.GUIStyle::Internal_GetLineHeight
    (const void*)&GUIStyle_Get_Custom_PropFont            ,  //  <- UnityEngine.GUIStyle::get_font
    (const void*)&GUIStyle_Set_Custom_PropFont            ,  //  <- UnityEngine.GUIStyle::set_font
    (const void*)&GUIStyle_Get_Custom_PropFontSize        ,  //  <- UnityEngine.GUIStyle::get_fontSize
    (const void*)&GUIStyle_Set_Custom_PropFontSize        ,  //  <- UnityEngine.GUIStyle::set_fontSize
    (const void*)&GUIStyle_Get_Custom_PropFontStyle       ,  //  <- UnityEngine.GUIStyle::get_fontStyle
    (const void*)&GUIStyle_Set_Custom_PropFontStyle       ,  //  <- UnityEngine.GUIStyle::set_fontStyle
    (const void*)&GUIStyle_Get_Custom_PropRichText        ,  //  <- UnityEngine.GUIStyle::get_richText
    (const void*)&GUIStyle_Set_Custom_PropRichText        ,  //  <- UnityEngine.GUIStyle::set_richText
    (const void*)&GUIStyle_CUSTOM_Internal_Draw           ,  //  <- UnityEngine.GUIStyle::Internal_Draw
    (const void*)&GUIStyle_CUSTOM_INTERNAL_CALL_Internal_Draw2,  //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_Draw2
    (const void*)&GUIStyle_CUSTOM_Internal_GetCursorFlashOffset,  //  <- UnityEngine.GUIStyle::Internal_GetCursorFlashOffset
    (const void*)&GUIStyle_CUSTOM_INTERNAL_CALL_Internal_DrawCursor,  //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_DrawCursor
    (const void*)&GUIStyle_CUSTOM_Internal_DrawWithTextSelection,  //  <- UnityEngine.GUIStyle::Internal_DrawWithTextSelection
    (const void*)&GUIStyle_CUSTOM_SetDefaultFont          ,  //  <- UnityEngine.GUIStyle::SetDefaultFont
    (const void*)&GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorPixelPosition,  //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorPixelPosition
    (const void*)&GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorStringIndex,  //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorStringIndex
    (const void*)&GUIStyle_CUSTOM_Internal_GetNumCharactersThatFitWithinWidth,  //  <- UnityEngine.GUIStyle::Internal_GetNumCharactersThatFitWithinWidth
    (const void*)&GUIStyle_CUSTOM_Internal_CalcSize       ,  //  <- UnityEngine.GUIStyle::Internal_CalcSize
    (const void*)&GUIStyle_CUSTOM_Internal_CalcHeight     ,  //  <- UnityEngine.GUIStyle::Internal_CalcHeight
    (const void*)&GUIStyle_CUSTOM_Internal_CalcMinMaxWidth,  //  <- UnityEngine.GUIStyle::Internal_CalcMinMaxWidth
    NULL
};

void ExportGUIStyleBindings();
void ExportGUIStyleBindings()
{
    for (int i = 0; s_GUIStyle_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GUIStyle_IcallNames [i], s_GUIStyle_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportGUIStyleBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(GUIStyleState_CUSTOM_Init); //  <- UnityEngine.GUIStyleState::Init
    SET_METRO_BINDING(GUIStyleState_CUSTOM_Cleanup); //  <- UnityEngine.GUIStyleState::Cleanup
    SET_METRO_BINDING(GUIStyleState_Get_Custom_PropBackground); //  <- UnityEngine.GUIStyleState::get_background
    SET_METRO_BINDING(GUIStyleState_Set_Custom_PropBackground); //  <- UnityEngine.GUIStyleState::set_background
    SET_METRO_BINDING(GUIStyleState_CUSTOM_INTERNAL_get_textColor); //  <- UnityEngine.GUIStyleState::INTERNAL_get_textColor
    SET_METRO_BINDING(GUIStyleState_CUSTOM_INTERNAL_set_textColor); //  <- UnityEngine.GUIStyleState::INTERNAL_set_textColor
    SET_METRO_BINDING(RectOffset_CUSTOM_Init); //  <- UnityEngine.RectOffset::Init
    SET_METRO_BINDING(RectOffset_CUSTOM_Cleanup); //  <- UnityEngine.RectOffset::Cleanup
    SET_METRO_BINDING(RectOffset_Get_Custom_PropLeft); //  <- UnityEngine.RectOffset::get_left
    SET_METRO_BINDING(RectOffset_Set_Custom_PropLeft); //  <- UnityEngine.RectOffset::set_left
    SET_METRO_BINDING(RectOffset_Get_Custom_PropRight); //  <- UnityEngine.RectOffset::get_right
    SET_METRO_BINDING(RectOffset_Set_Custom_PropRight); //  <- UnityEngine.RectOffset::set_right
    SET_METRO_BINDING(RectOffset_Get_Custom_PropTop); //  <- UnityEngine.RectOffset::get_top
    SET_METRO_BINDING(RectOffset_Set_Custom_PropTop); //  <- UnityEngine.RectOffset::set_top
    SET_METRO_BINDING(RectOffset_Get_Custom_PropBottom); //  <- UnityEngine.RectOffset::get_bottom
    SET_METRO_BINDING(RectOffset_Set_Custom_PropBottom); //  <- UnityEngine.RectOffset::set_bottom
    SET_METRO_BINDING(RectOffset_Get_Custom_PropHorizontal); //  <- UnityEngine.RectOffset::get_horizontal
    SET_METRO_BINDING(RectOffset_Get_Custom_PropVertical); //  <- UnityEngine.RectOffset::get_vertical
    SET_METRO_BINDING(RectOffset_CUSTOM_INTERNAL_CALL_Add); //  <- UnityEngine.RectOffset::INTERNAL_CALL_Add
    SET_METRO_BINDING(RectOffset_CUSTOM_INTERNAL_CALL_Remove); //  <- UnityEngine.RectOffset::INTERNAL_CALL_Remove
    SET_METRO_BINDING(GUIStyle_CUSTOM_Init); //  <- UnityEngine.GUIStyle::Init
    SET_METRO_BINDING(GUIStyle_CUSTOM_InitCopy); //  <- UnityEngine.GUIStyle::InitCopy
    SET_METRO_BINDING(GUIStyle_CUSTOM_Cleanup); //  <- UnityEngine.GUIStyle::Cleanup
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropName); //  <- UnityEngine.GUIStyle::get_name
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropName); //  <- UnityEngine.GUIStyle::set_name
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_CALL_GetStyleStatePtr); //  <- UnityEngine.GUIStyle::INTERNAL_CALL_GetStyleStatePtr
    SET_METRO_BINDING(GUIStyle_CUSTOM_AssignStyleState); //  <- UnityEngine.GUIStyle::AssignStyleState
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_CALL_GetRectOffsetPtr); //  <- UnityEngine.GUIStyle::INTERNAL_CALL_GetRectOffsetPtr
    SET_METRO_BINDING(GUIStyle_CUSTOM_AssignRectOffset); //  <- UnityEngine.GUIStyle::AssignRectOffset
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropImagePosition); //  <- UnityEngine.GUIStyle::get_imagePosition
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropImagePosition); //  <- UnityEngine.GUIStyle::set_imagePosition
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropAlignment); //  <- UnityEngine.GUIStyle::get_alignment
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropAlignment); //  <- UnityEngine.GUIStyle::set_alignment
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropWordWrap); //  <- UnityEngine.GUIStyle::get_wordWrap
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropWordWrap); //  <- UnityEngine.GUIStyle::set_wordWrap
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropClipping); //  <- UnityEngine.GUIStyle::get_clipping
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropClipping); //  <- UnityEngine.GUIStyle::set_clipping
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_get_contentOffset); //  <- UnityEngine.GUIStyle::INTERNAL_get_contentOffset
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_set_contentOffset); //  <- UnityEngine.GUIStyle::INTERNAL_set_contentOffset
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_get_Internal_clipOffset); //  <- UnityEngine.GUIStyle::INTERNAL_get_Internal_clipOffset
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_set_Internal_clipOffset); //  <- UnityEngine.GUIStyle::INTERNAL_set_Internal_clipOffset
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropFixedWidth); //  <- UnityEngine.GUIStyle::get_fixedWidth
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropFixedWidth); //  <- UnityEngine.GUIStyle::set_fixedWidth
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropFixedHeight); //  <- UnityEngine.GUIStyle::get_fixedHeight
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropFixedHeight); //  <- UnityEngine.GUIStyle::set_fixedHeight
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropStretchWidth); //  <- UnityEngine.GUIStyle::get_stretchWidth
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropStretchWidth); //  <- UnityEngine.GUIStyle::set_stretchWidth
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropStretchHeight); //  <- UnityEngine.GUIStyle::get_stretchHeight
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropStretchHeight); //  <- UnityEngine.GUIStyle::set_stretchHeight
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_GetLineHeight); //  <- UnityEngine.GUIStyle::Internal_GetLineHeight
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropFont); //  <- UnityEngine.GUIStyle::get_font
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropFont); //  <- UnityEngine.GUIStyle::set_font
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropFontSize); //  <- UnityEngine.GUIStyle::get_fontSize
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropFontSize); //  <- UnityEngine.GUIStyle::set_fontSize
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropFontStyle); //  <- UnityEngine.GUIStyle::get_fontStyle
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropFontStyle); //  <- UnityEngine.GUIStyle::set_fontStyle
    SET_METRO_BINDING(GUIStyle_Get_Custom_PropRichText); //  <- UnityEngine.GUIStyle::get_richText
    SET_METRO_BINDING(GUIStyle_Set_Custom_PropRichText); //  <- UnityEngine.GUIStyle::set_richText
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_Draw); //  <- UnityEngine.GUIStyle::Internal_Draw
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_Draw2); //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_Draw2
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_GetCursorFlashOffset); //  <- UnityEngine.GUIStyle::Internal_GetCursorFlashOffset
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_DrawCursor); //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_DrawCursor
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_DrawWithTextSelection); //  <- UnityEngine.GUIStyle::Internal_DrawWithTextSelection
    SET_METRO_BINDING(GUIStyle_CUSTOM_SetDefaultFont); //  <- UnityEngine.GUIStyle::SetDefaultFont
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorPixelPosition); //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorPixelPosition
    SET_METRO_BINDING(GUIStyle_CUSTOM_INTERNAL_CALL_Internal_GetCursorStringIndex); //  <- UnityEngine.GUIStyle::INTERNAL_CALL_Internal_GetCursorStringIndex
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_GetNumCharactersThatFitWithinWidth); //  <- UnityEngine.GUIStyle::Internal_GetNumCharactersThatFitWithinWidth
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_CalcSize); //  <- UnityEngine.GUIStyle::Internal_CalcSize
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_CalcHeight); //  <- UnityEngine.GUIStyle::Internal_CalcHeight
    SET_METRO_BINDING(GUIStyle_CUSTOM_Internal_CalcMinMaxWidth); //  <- UnityEngine.GUIStyle::Internal_CalcMinMaxWidth
}

#endif
