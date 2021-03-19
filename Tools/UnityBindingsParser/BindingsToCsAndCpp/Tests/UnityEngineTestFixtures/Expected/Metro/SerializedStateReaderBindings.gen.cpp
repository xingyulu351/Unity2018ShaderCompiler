#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoBehaviour.h"
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
#include "Runtime/Mono/MonoBehaviourSerialization_ByCodeGeneration.h"
#endif
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_Align()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_Align)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_Align)
    
    		NativeExt_MonoBehaviourSerialization_ReaderAlign();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt8 SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadByte()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadByte)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadByte)
    
    		return NativeExt_MonoBehaviourSerialization_ReadByte();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadInt32()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadInt32)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadInt32)
    
    		return NativeExt_MonoBehaviourSerialization_ReadInt();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadSingle()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadSingle)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadSingle)
    
    		return NativeExt_MonoBehaviourSerialization_ReadFloat();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadDouble()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadDouble)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadDouble)
    
    		return NativeExt_MonoBehaviourSerialization_ReadDouble();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadBoolean()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadBoolean)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadBoolean)
    
    		return NativeExt_MonoBehaviourSerialization_ReadBool() == 1;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadString()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadString)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadString)
    
    		return NativeExt_MonoBehaviourSerialization_ReadString();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadUnityEngineObject()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadUnityEngineObject)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadUnityEngineObject)
    
    		return NativeExt_MonoBehaviourSerialization_ReadUnityEngineObject();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadAnimationCurve(ICallType_Object_Argument animation_curve_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadAnimationCurve)
    ScriptingObjectWithIntPtrField<AnimationCurve> animation_curve(animation_curve_);
    UNUSED(animation_curve);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadAnimationCurve)
    
    		NativeExt_MonoBehaviourSerialization_ReadAnimationCurve(animation_curve);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadGradient(ICallType_Object_Argument gradient_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadGradient)
    ScriptingObjectWithIntPtrField<Gradient> gradient(gradient_);
    UNUSED(gradient);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadGradient)
    
    		NativeExt_MonoBehaviourSerialization_ReadGradient(gradient);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_ReadGUIStyle(ICallType_Object_Argument style_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_ReadGUIStyle)
    ScriptingObjectWithIntPtrField<GUIStyle> style(style_);
    UNUSED(style);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_ReadGUIStyle)
    
    		NativeExt_MonoBehaviourSerialization_ReadGUIStyle(style);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateReader_CUSTOM_INTERNAL_RectOffset(ICallType_Object_Argument rect_offset_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateReader_CUSTOM_INTERNAL_RectOffset)
    ScriptingObjectWithIntPtrField<RectOffset> rect_offset(rect_offset_);
    UNUSED(rect_offset);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_RectOffset)
    
    		NativeExt_MonoBehaviourSerialization_ReadRectOffset(rect_offset);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_Align()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_Align" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_Align );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadByte()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadByte" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadByte );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadInt32()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadInt32" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadInt32 );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadSingle()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadSingle" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadSingle );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadDouble()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadDouble" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadDouble );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadBoolean()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadBoolean" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadBoolean );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadString()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadString" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadString );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadUnityEngineObject()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadUnityEngineObject" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadUnityEngineObject );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadAnimationCurve()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadAnimationCurve" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadAnimationCurve );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadGradient()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGradient" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadGradient );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_ReadGUIStyle()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGUIStyle" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_ReadGUIStyle );
}

void Register_UnityEngine_Serialization_SerializedStateReader_INTERNAL_RectOffset()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateReader::INTERNAL_RectOffset" , (gpointer)& SerializedStateReader_CUSTOM_INTERNAL_RectOffset );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_SerializedStateReader_IcallNames [] =
{
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_Align",    // -> SerializedStateReader_CUSTOM_INTERNAL_Align
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadByte",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadByte
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadInt32",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadInt32
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadSingle",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadSingle
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadDouble",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadDouble
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadBoolean",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadBoolean
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadString",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadString
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadUnityEngineObject",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadUnityEngineObject
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadAnimationCurve",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadAnimationCurve
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGradient",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadGradient
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGUIStyle",    // -> SerializedStateReader_CUSTOM_INTERNAL_ReadGUIStyle
    "UnityEngine.Serialization.SerializedStateReader::INTERNAL_RectOffset",    // -> SerializedStateReader_CUSTOM_INTERNAL_RectOffset
#endif
    NULL
};

static const void* s_SerializedStateReader_IcallFuncs [] =
{
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_Align,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_Align
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadByte,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadByte
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadInt32,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadInt32
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadSingle,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadSingle
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadDouble,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadDouble
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadBoolean,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadBoolean
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadString,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadString
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadUnityEngineObject,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadUnityEngineObject
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadAnimationCurve,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadAnimationCurve
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadGradient,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGradient
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_ReadGUIStyle,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGUIStyle
    (const void*)&SerializedStateReader_CUSTOM_INTERNAL_RectOffset,  //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_RectOffset
#endif
    NULL
};

void ExportSerializedStateReaderBindings();
void ExportSerializedStateReaderBindings()
{
    for (int i = 0; s_SerializedStateReader_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_SerializedStateReader_IcallNames [i], s_SerializedStateReader_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportSerializedStateReaderBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_Align); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_Align
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadByte); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadByte
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadInt32); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadInt32
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadSingle); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadSingle
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadDouble); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadDouble
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadBoolean); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadBoolean
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadString); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadString
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadUnityEngineObject); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadUnityEngineObject
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadAnimationCurve); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadAnimationCurve
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadGradient); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGradient
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_ReadGUIStyle); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_ReadGUIStyle
    SET_METRO_BINDING(SerializedStateReader_CUSTOM_INTERNAL_RectOffset); //  <- UnityEngine.Serialization.SerializedStateReader::INTERNAL_RectOffset
#endif
}

#endif
