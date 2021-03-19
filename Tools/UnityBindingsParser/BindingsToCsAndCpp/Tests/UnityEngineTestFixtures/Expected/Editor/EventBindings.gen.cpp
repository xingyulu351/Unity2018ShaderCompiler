#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/IMGUI/IMGUIUtils.h"
#include "Runtime/Scripting/ScriptingUtility.h"

 


void PauseEditor ();
using namespace std;

 static void CleanupInputEvent(void* inputEvent){ delete (InputEvent*)inputEvent; };
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_Init(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_Init)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Init)
    
    		InputEvent* newEvent = new InputEvent ();
    		self.SetPtr(newEvent,CleanupInputEvent);
    		newEvent->Init();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_Cleanup)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
    
    		CleanupInputEvent(self.GetPtr());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_InitCopy(ICallType_Object_Argument self_, ICallType_Object_Argument other_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_InitCopy)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    ScriptingObjectWithIntPtrField<InputEvent> other(other_);
    UNUSED(other);
    SCRIPTINGAPI_STACK_CHECK(InitCopy)
    
    		self.SetPtr(new InputEvent (*other), CleanupInputEvent);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_InitPtr(ICallType_Object_Argument self_, ICallType_IntPtr_Argument ptr_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_InitPtr)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    void* ptr(ptr_);
    UNUSED(ptr);
    SCRIPTINGAPI_STACK_CHECK(InitPtr)
    
    		self.SetPtr((InputEvent*)ptr, CleanupInputEvent);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_Get_Custom_PropRawType(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropRawType)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rawType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rawType)
    return self->type;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_Get_Custom_PropType(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropType)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_type)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_type)
    return IMGUI::GetEventType (GetGUIState(), *self);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropType(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropType)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_type)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_type)
     self->type = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_CUSTOM_GetTypeForControl(ICallType_Object_Argument self_, int controlID)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_GetTypeForControl)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetTypeForControl)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTypeForControl)
     return IMGUI::GetEventTypeForControl (GetGUIState(), *self, controlID); 
}

#if ENABLE_NEW_EVENT_SYSTEM
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_INTERNAL_get_touch(ICallType_Object_Argument self_, Touch* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_INTERNAL_get_touch)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_touch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_touch)
    { *returnValue =(self->touch); return;};
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_INTERNAL_CALL_Internal_SetMousePosition(ICallType_Object_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_INTERNAL_CALL_Internal_SetMousePosition)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_SetMousePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_SetMousePosition)
    
    #if ENABLE_NEW_EVENT_SYSTEM
    		self->touch.pos = value;
    #else
    		self->mousePosition = value;
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_Internal_GetMousePosition(ICallType_Object_Argument self_, Vector2fIcall* value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_Internal_GetMousePosition)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetMousePosition)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetMousePosition)
    
    #if ENABLE_NEW_EVENT_SYSTEM
    		*value = self->touch.pos;
    #else
    		*value = self->mousePosition;
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_INTERNAL_CALL_Internal_SetMouseDelta(ICallType_Object_Argument self_, const Vector2fIcall& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_INTERNAL_CALL_Internal_SetMouseDelta)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_SetMouseDelta)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_SetMouseDelta)
    
    #if ENABLE_NEW_EVENT_SYSTEM
    		self->touch.deltaPos = value;
    #else
    		self->delta = value;
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_Internal_GetMouseDelta(ICallType_Object_Argument self_, Vector2fIcall* value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_Internal_GetMouseDelta)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetMouseDelta)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetMouseDelta)
    
    #if ENABLE_NEW_EVENT_SYSTEM
    		*value = self->touch.deltaPos;
    #else
    		*value = self->delta;
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_Get_Custom_PropButton(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropButton)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_button)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_button)
    return self->button;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropButton(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropButton)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_button)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_button)
     self->button = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_Get_Custom_PropModifiers(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropModifiers)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_modifiers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_modifiers)
    return self->modifiers;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropModifiers(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropModifiers)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_modifiers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_modifiers)
     self->modifiers = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Event_Get_Custom_PropPressure(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropPressure)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pressure)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pressure)
    return self->pressure;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropPressure(ICallType_Object_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropPressure)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_pressure)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pressure)
     self->pressure = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_Get_Custom_PropClickCount(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropClickCount)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clickCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clickCount)
    return self->clickCount;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropClickCount(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropClickCount)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_clickCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_clickCount)
     self->clickCount = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt16 SCRIPT_CALL_CONVENTION Event_Get_Custom_PropCharacter(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropCharacter)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_character)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_character)
    return self->character;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropCharacter(ICallType_Object_Argument self_, UInt16 value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropCharacter)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_character)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_character)
     self->character = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Event_Get_Custom_PropCommandName(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropCommandName)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_commandName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_commandName)
    char* commandString = self->commandString;
    return CreateScriptingString (commandString == NULL ? "" : commandString);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropCommandName(ICallType_Object_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropCommandName)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_commandName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_commandName)
    
    #if ENABLE_MONO
    char* oldPtr = reinterpret_cast<char*>(self->commandString);
    delete[] oldPtr;
    char *str = mono_string_to_utf8 (value.str);
    self->commandString = new char[strlen (str) + 1];
    strcpy (self->commandString, str);
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Event_Get_Custom_PropKeyCode(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Get_Custom_PropKeyCode)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_keyCode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_keyCode)
    return self->keycode;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_Set_Custom_PropKeyCode(ICallType_Object_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_Set_Custom_PropKeyCode)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_keyCode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_keyCode)
     self->keycode = value; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_Internal_SetNativeEvent(ICallType_IntPtr_Argument ptr_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_Internal_SetNativeEvent)
    void* ptr(ptr_);
    UNUSED(ptr);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetNativeEvent)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetNativeEvent)
    
    		GetGUIState().Internal_SetManagedEvent (ptr);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Event_CUSTOM_Use(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Event_CUSTOM_Use)
    ScriptingObjectWithIntPtrField<InputEvent> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Use)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Use)
     self->Use(); 
}




#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Event_Init()
{
    scripting_add_internal_call( "UnityEngine.Event::Init" , (gpointer)& Event_CUSTOM_Init );
}

void Register_UnityEngine_Event_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.Event::Cleanup" , (gpointer)& Event_CUSTOM_Cleanup );
}

void Register_UnityEngine_Event_InitCopy()
{
    scripting_add_internal_call( "UnityEngine.Event::InitCopy" , (gpointer)& Event_CUSTOM_InitCopy );
}

void Register_UnityEngine_Event_InitPtr()
{
    scripting_add_internal_call( "UnityEngine.Event::InitPtr" , (gpointer)& Event_CUSTOM_InitPtr );
}

void Register_UnityEngine_Event_get_rawType()
{
    scripting_add_internal_call( "UnityEngine.Event::get_rawType" , (gpointer)& Event_Get_Custom_PropRawType );
}

void Register_UnityEngine_Event_get_type()
{
    scripting_add_internal_call( "UnityEngine.Event::get_type" , (gpointer)& Event_Get_Custom_PropType );
}

void Register_UnityEngine_Event_set_type()
{
    scripting_add_internal_call( "UnityEngine.Event::set_type" , (gpointer)& Event_Set_Custom_PropType );
}

void Register_UnityEngine_Event_GetTypeForControl()
{
    scripting_add_internal_call( "UnityEngine.Event::GetTypeForControl" , (gpointer)& Event_CUSTOM_GetTypeForControl );
}

#if ENABLE_NEW_EVENT_SYSTEM
void Register_UnityEngine_Event_INTERNAL_get_touch()
{
    scripting_add_internal_call( "UnityEngine.Event::INTERNAL_get_touch" , (gpointer)& Event_CUSTOM_INTERNAL_get_touch );
}

#endif
void Register_UnityEngine_Event_INTERNAL_CALL_Internal_SetMousePosition()
{
    scripting_add_internal_call( "UnityEngine.Event::INTERNAL_CALL_Internal_SetMousePosition" , (gpointer)& Event_CUSTOM_INTERNAL_CALL_Internal_SetMousePosition );
}

void Register_UnityEngine_Event_Internal_GetMousePosition()
{
    scripting_add_internal_call( "UnityEngine.Event::Internal_GetMousePosition" , (gpointer)& Event_CUSTOM_Internal_GetMousePosition );
}

void Register_UnityEngine_Event_INTERNAL_CALL_Internal_SetMouseDelta()
{
    scripting_add_internal_call( "UnityEngine.Event::INTERNAL_CALL_Internal_SetMouseDelta" , (gpointer)& Event_CUSTOM_INTERNAL_CALL_Internal_SetMouseDelta );
}

void Register_UnityEngine_Event_Internal_GetMouseDelta()
{
    scripting_add_internal_call( "UnityEngine.Event::Internal_GetMouseDelta" , (gpointer)& Event_CUSTOM_Internal_GetMouseDelta );
}

void Register_UnityEngine_Event_get_button()
{
    scripting_add_internal_call( "UnityEngine.Event::get_button" , (gpointer)& Event_Get_Custom_PropButton );
}

void Register_UnityEngine_Event_set_button()
{
    scripting_add_internal_call( "UnityEngine.Event::set_button" , (gpointer)& Event_Set_Custom_PropButton );
}

void Register_UnityEngine_Event_get_modifiers()
{
    scripting_add_internal_call( "UnityEngine.Event::get_modifiers" , (gpointer)& Event_Get_Custom_PropModifiers );
}

void Register_UnityEngine_Event_set_modifiers()
{
    scripting_add_internal_call( "UnityEngine.Event::set_modifiers" , (gpointer)& Event_Set_Custom_PropModifiers );
}

void Register_UnityEngine_Event_get_pressure()
{
    scripting_add_internal_call( "UnityEngine.Event::get_pressure" , (gpointer)& Event_Get_Custom_PropPressure );
}

void Register_UnityEngine_Event_set_pressure()
{
    scripting_add_internal_call( "UnityEngine.Event::set_pressure" , (gpointer)& Event_Set_Custom_PropPressure );
}

void Register_UnityEngine_Event_get_clickCount()
{
    scripting_add_internal_call( "UnityEngine.Event::get_clickCount" , (gpointer)& Event_Get_Custom_PropClickCount );
}

void Register_UnityEngine_Event_set_clickCount()
{
    scripting_add_internal_call( "UnityEngine.Event::set_clickCount" , (gpointer)& Event_Set_Custom_PropClickCount );
}

void Register_UnityEngine_Event_get_character()
{
    scripting_add_internal_call( "UnityEngine.Event::get_character" , (gpointer)& Event_Get_Custom_PropCharacter );
}

void Register_UnityEngine_Event_set_character()
{
    scripting_add_internal_call( "UnityEngine.Event::set_character" , (gpointer)& Event_Set_Custom_PropCharacter );
}

void Register_UnityEngine_Event_get_commandName()
{
    scripting_add_internal_call( "UnityEngine.Event::get_commandName" , (gpointer)& Event_Get_Custom_PropCommandName );
}

void Register_UnityEngine_Event_set_commandName()
{
    scripting_add_internal_call( "UnityEngine.Event::set_commandName" , (gpointer)& Event_Set_Custom_PropCommandName );
}

void Register_UnityEngine_Event_get_keyCode()
{
    scripting_add_internal_call( "UnityEngine.Event::get_keyCode" , (gpointer)& Event_Get_Custom_PropKeyCode );
}

void Register_UnityEngine_Event_set_keyCode()
{
    scripting_add_internal_call( "UnityEngine.Event::set_keyCode" , (gpointer)& Event_Set_Custom_PropKeyCode );
}

void Register_UnityEngine_Event_Internal_SetNativeEvent()
{
    scripting_add_internal_call( "UnityEngine.Event::Internal_SetNativeEvent" , (gpointer)& Event_CUSTOM_Internal_SetNativeEvent );
}

void Register_UnityEngine_Event_Use()
{
    scripting_add_internal_call( "UnityEngine.Event::Use" , (gpointer)& Event_CUSTOM_Use );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Event_IcallNames [] =
{
    "UnityEngine.Event::Init"               ,    // -> Event_CUSTOM_Init
    "UnityEngine.Event::Cleanup"            ,    // -> Event_CUSTOM_Cleanup
    "UnityEngine.Event::InitCopy"           ,    // -> Event_CUSTOM_InitCopy
    "UnityEngine.Event::InitPtr"            ,    // -> Event_CUSTOM_InitPtr
    "UnityEngine.Event::get_rawType"        ,    // -> Event_Get_Custom_PropRawType
    "UnityEngine.Event::get_type"           ,    // -> Event_Get_Custom_PropType
    "UnityEngine.Event::set_type"           ,    // -> Event_Set_Custom_PropType
    "UnityEngine.Event::GetTypeForControl"  ,    // -> Event_CUSTOM_GetTypeForControl
#if ENABLE_NEW_EVENT_SYSTEM
    "UnityEngine.Event::INTERNAL_get_touch" ,    // -> Event_CUSTOM_INTERNAL_get_touch
#endif
    "UnityEngine.Event::INTERNAL_CALL_Internal_SetMousePosition",    // -> Event_CUSTOM_INTERNAL_CALL_Internal_SetMousePosition
    "UnityEngine.Event::Internal_GetMousePosition",    // -> Event_CUSTOM_Internal_GetMousePosition
    "UnityEngine.Event::INTERNAL_CALL_Internal_SetMouseDelta",    // -> Event_CUSTOM_INTERNAL_CALL_Internal_SetMouseDelta
    "UnityEngine.Event::Internal_GetMouseDelta",    // -> Event_CUSTOM_Internal_GetMouseDelta
    "UnityEngine.Event::get_button"         ,    // -> Event_Get_Custom_PropButton
    "UnityEngine.Event::set_button"         ,    // -> Event_Set_Custom_PropButton
    "UnityEngine.Event::get_modifiers"      ,    // -> Event_Get_Custom_PropModifiers
    "UnityEngine.Event::set_modifiers"      ,    // -> Event_Set_Custom_PropModifiers
    "UnityEngine.Event::get_pressure"       ,    // -> Event_Get_Custom_PropPressure
    "UnityEngine.Event::set_pressure"       ,    // -> Event_Set_Custom_PropPressure
    "UnityEngine.Event::get_clickCount"     ,    // -> Event_Get_Custom_PropClickCount
    "UnityEngine.Event::set_clickCount"     ,    // -> Event_Set_Custom_PropClickCount
    "UnityEngine.Event::get_character"      ,    // -> Event_Get_Custom_PropCharacter
    "UnityEngine.Event::set_character"      ,    // -> Event_Set_Custom_PropCharacter
    "UnityEngine.Event::get_commandName"    ,    // -> Event_Get_Custom_PropCommandName
    "UnityEngine.Event::set_commandName"    ,    // -> Event_Set_Custom_PropCommandName
    "UnityEngine.Event::get_keyCode"        ,    // -> Event_Get_Custom_PropKeyCode
    "UnityEngine.Event::set_keyCode"        ,    // -> Event_Set_Custom_PropKeyCode
    "UnityEngine.Event::Internal_SetNativeEvent",    // -> Event_CUSTOM_Internal_SetNativeEvent
    "UnityEngine.Event::Use"                ,    // -> Event_CUSTOM_Use
    NULL
};

static const void* s_Event_IcallFuncs [] =
{
    (const void*)&Event_CUSTOM_Init                       ,  //  <- UnityEngine.Event::Init
    (const void*)&Event_CUSTOM_Cleanup                    ,  //  <- UnityEngine.Event::Cleanup
    (const void*)&Event_CUSTOM_InitCopy                   ,  //  <- UnityEngine.Event::InitCopy
    (const void*)&Event_CUSTOM_InitPtr                    ,  //  <- UnityEngine.Event::InitPtr
    (const void*)&Event_Get_Custom_PropRawType            ,  //  <- UnityEngine.Event::get_rawType
    (const void*)&Event_Get_Custom_PropType               ,  //  <- UnityEngine.Event::get_type
    (const void*)&Event_Set_Custom_PropType               ,  //  <- UnityEngine.Event::set_type
    (const void*)&Event_CUSTOM_GetTypeForControl          ,  //  <- UnityEngine.Event::GetTypeForControl
#if ENABLE_NEW_EVENT_SYSTEM
    (const void*)&Event_CUSTOM_INTERNAL_get_touch         ,  //  <- UnityEngine.Event::INTERNAL_get_touch
#endif
    (const void*)&Event_CUSTOM_INTERNAL_CALL_Internal_SetMousePosition,  //  <- UnityEngine.Event::INTERNAL_CALL_Internal_SetMousePosition
    (const void*)&Event_CUSTOM_Internal_GetMousePosition  ,  //  <- UnityEngine.Event::Internal_GetMousePosition
    (const void*)&Event_CUSTOM_INTERNAL_CALL_Internal_SetMouseDelta,  //  <- UnityEngine.Event::INTERNAL_CALL_Internal_SetMouseDelta
    (const void*)&Event_CUSTOM_Internal_GetMouseDelta     ,  //  <- UnityEngine.Event::Internal_GetMouseDelta
    (const void*)&Event_Get_Custom_PropButton             ,  //  <- UnityEngine.Event::get_button
    (const void*)&Event_Set_Custom_PropButton             ,  //  <- UnityEngine.Event::set_button
    (const void*)&Event_Get_Custom_PropModifiers          ,  //  <- UnityEngine.Event::get_modifiers
    (const void*)&Event_Set_Custom_PropModifiers          ,  //  <- UnityEngine.Event::set_modifiers
    (const void*)&Event_Get_Custom_PropPressure           ,  //  <- UnityEngine.Event::get_pressure
    (const void*)&Event_Set_Custom_PropPressure           ,  //  <- UnityEngine.Event::set_pressure
    (const void*)&Event_Get_Custom_PropClickCount         ,  //  <- UnityEngine.Event::get_clickCount
    (const void*)&Event_Set_Custom_PropClickCount         ,  //  <- UnityEngine.Event::set_clickCount
    (const void*)&Event_Get_Custom_PropCharacter          ,  //  <- UnityEngine.Event::get_character
    (const void*)&Event_Set_Custom_PropCharacter          ,  //  <- UnityEngine.Event::set_character
    (const void*)&Event_Get_Custom_PropCommandName        ,  //  <- UnityEngine.Event::get_commandName
    (const void*)&Event_Set_Custom_PropCommandName        ,  //  <- UnityEngine.Event::set_commandName
    (const void*)&Event_Get_Custom_PropKeyCode            ,  //  <- UnityEngine.Event::get_keyCode
    (const void*)&Event_Set_Custom_PropKeyCode            ,  //  <- UnityEngine.Event::set_keyCode
    (const void*)&Event_CUSTOM_Internal_SetNativeEvent    ,  //  <- UnityEngine.Event::Internal_SetNativeEvent
    (const void*)&Event_CUSTOM_Use                        ,  //  <- UnityEngine.Event::Use
    NULL
};

void ExportEventBindings();
void ExportEventBindings()
{
    for (int i = 0; s_Event_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Event_IcallNames [i], s_Event_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportEventBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Event_CUSTOM_Init); //  <- UnityEngine.Event::Init
    SET_METRO_BINDING(Event_CUSTOM_Cleanup); //  <- UnityEngine.Event::Cleanup
    SET_METRO_BINDING(Event_CUSTOM_InitCopy); //  <- UnityEngine.Event::InitCopy
    SET_METRO_BINDING(Event_CUSTOM_InitPtr); //  <- UnityEngine.Event::InitPtr
    SET_METRO_BINDING(Event_Get_Custom_PropRawType); //  <- UnityEngine.Event::get_rawType
    SET_METRO_BINDING(Event_Get_Custom_PropType); //  <- UnityEngine.Event::get_type
    SET_METRO_BINDING(Event_Set_Custom_PropType); //  <- UnityEngine.Event::set_type
    SET_METRO_BINDING(Event_CUSTOM_GetTypeForControl); //  <- UnityEngine.Event::GetTypeForControl
#if ENABLE_NEW_EVENT_SYSTEM
    SET_METRO_BINDING(Event_CUSTOM_INTERNAL_get_touch); //  <- UnityEngine.Event::INTERNAL_get_touch
#endif
    SET_METRO_BINDING(Event_CUSTOM_INTERNAL_CALL_Internal_SetMousePosition); //  <- UnityEngine.Event::INTERNAL_CALL_Internal_SetMousePosition
    SET_METRO_BINDING(Event_CUSTOM_Internal_GetMousePosition); //  <- UnityEngine.Event::Internal_GetMousePosition
    SET_METRO_BINDING(Event_CUSTOM_INTERNAL_CALL_Internal_SetMouseDelta); //  <- UnityEngine.Event::INTERNAL_CALL_Internal_SetMouseDelta
    SET_METRO_BINDING(Event_CUSTOM_Internal_GetMouseDelta); //  <- UnityEngine.Event::Internal_GetMouseDelta
    SET_METRO_BINDING(Event_Get_Custom_PropButton); //  <- UnityEngine.Event::get_button
    SET_METRO_BINDING(Event_Set_Custom_PropButton); //  <- UnityEngine.Event::set_button
    SET_METRO_BINDING(Event_Get_Custom_PropModifiers); //  <- UnityEngine.Event::get_modifiers
    SET_METRO_BINDING(Event_Set_Custom_PropModifiers); //  <- UnityEngine.Event::set_modifiers
    SET_METRO_BINDING(Event_Get_Custom_PropPressure); //  <- UnityEngine.Event::get_pressure
    SET_METRO_BINDING(Event_Set_Custom_PropPressure); //  <- UnityEngine.Event::set_pressure
    SET_METRO_BINDING(Event_Get_Custom_PropClickCount); //  <- UnityEngine.Event::get_clickCount
    SET_METRO_BINDING(Event_Set_Custom_PropClickCount); //  <- UnityEngine.Event::set_clickCount
    SET_METRO_BINDING(Event_Get_Custom_PropCharacter); //  <- UnityEngine.Event::get_character
    SET_METRO_BINDING(Event_Set_Custom_PropCharacter); //  <- UnityEngine.Event::set_character
    SET_METRO_BINDING(Event_Get_Custom_PropCommandName); //  <- UnityEngine.Event::get_commandName
    SET_METRO_BINDING(Event_Set_Custom_PropCommandName); //  <- UnityEngine.Event::set_commandName
    SET_METRO_BINDING(Event_Get_Custom_PropKeyCode); //  <- UnityEngine.Event::get_keyCode
    SET_METRO_BINDING(Event_Set_Custom_PropKeyCode); //  <- UnityEngine.Event::set_keyCode
    SET_METRO_BINDING(Event_CUSTOM_Internal_SetNativeEvent); //  <- UnityEngine.Event::Internal_SetNativeEvent
    SET_METRO_BINDING(Event_CUSTOM_Use); //  <- UnityEngine.Event::Use
}

#endif
