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
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_Align()
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_Align)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_Align)
    
    		NativeExt_MonoBehaviourSerialization_WriterAlign();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteByte(UInt8 value)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteByte)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteByte)
    
    		NativeExt_MonoBehaviourSerialization_WriteByte(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteInt32(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteInt32)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteInt32)
    
    		NativeExt_MonoBehaviourSerialization_WriteInt(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteSingle(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteSingle)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteSingle)
    
    		NativeExt_MonoBehaviourSerialization_WriteFloat(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteDouble(double value)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteDouble)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteDouble)
    
    		NativeExt_MonoBehaviourSerialization_WriteDouble(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteBoolean(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteBoolean)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteBoolean)
    
    		NativeExt_MonoBehaviourSerialization_WriteBool(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteString(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteString)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteString)
    
    		NativeExt_MonoBehaviourSerialization_WriteString(const_cast<char*>(value.ToUTF8().c_str()), value.Length());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteUnityEngineObject(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteUnityEngineObject)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteUnityEngineObject)
    
    		NativeExt_MonoBehaviourSerialization_WriteUnityEngineObject(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteAnimationCurve(ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteAnimationCurve)
    ScriptingObjectWithIntPtrField<AnimationCurve> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteAnimationCurve)
    
    		NativeExt_MonoBehaviourSerialization_WriteAnimationCurve(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteGradient(ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteGradient)
    ScriptingObjectWithIntPtrField<Gradient> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteGradient)
    
    		NativeExt_MonoBehaviourSerialization_WriteGradient(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteGUIStyle(ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteGUIStyle)
    ScriptingObjectWithIntPtrField<GUIStyle> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteGUIStyle)
    
    		NativeExt_MonoBehaviourSerialization_WriteGUIStyle(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION SerializedStateWriter_CUSTOM_INTERNAL_WriteRectOffset(ICallType_Object_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(SerializedStateWriter_CUSTOM_INTERNAL_WriteRectOffset)
    ScriptingObjectWithIntPtrField<RectOffset> value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_WriteRectOffset)
    
    		NativeExt_MonoBehaviourSerialization_WriteRectOffset(value);
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_Align()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_Align" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_Align );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteByte()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteByte" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteByte );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteInt32()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteInt32" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteInt32 );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteSingle()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteSingle" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteSingle );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteDouble()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteDouble" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteDouble );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteBoolean()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteBoolean" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteBoolean );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteString()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteString" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteString );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteUnityEngineObject()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteUnityEngineObject" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteUnityEngineObject );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteAnimationCurve()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteAnimationCurve" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteAnimationCurve );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteGradient()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGradient" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteGradient );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteGUIStyle()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGUIStyle" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteGUIStyle );
}

void Register_UnityEngine_Serialization_SerializedStateWriter_INTERNAL_WriteRectOffset()
{
    scripting_add_internal_call( "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteRectOffset" , (gpointer)& SerializedStateWriter_CUSTOM_INTERNAL_WriteRectOffset );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_SerializedStateWriter_IcallNames [] =
{
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_Align",    // -> SerializedStateWriter_CUSTOM_INTERNAL_Align
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteByte",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteByte
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteInt32",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteInt32
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteSingle",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteSingle
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteDouble",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteDouble
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteBoolean",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteBoolean
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteString",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteString
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteUnityEngineObject",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteUnityEngineObject
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteAnimationCurve",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteAnimationCurve
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGradient",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteGradient
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGUIStyle",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteGUIStyle
    "UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteRectOffset",    // -> SerializedStateWriter_CUSTOM_INTERNAL_WriteRectOffset
#endif
    NULL
};

static const void* s_SerializedStateWriter_IcallFuncs [] =
{
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_Align,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_Align
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteByte,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteByte
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteInt32,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteInt32
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteSingle,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteSingle
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteDouble,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteDouble
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteBoolean,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteBoolean
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteString,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteString
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteUnityEngineObject,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteUnityEngineObject
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteAnimationCurve,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteAnimationCurve
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteGradient,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGradient
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteGUIStyle,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGUIStyle
    (const void*)&SerializedStateWriter_CUSTOM_INTERNAL_WriteRectOffset,  //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteRectOffset
#endif
    NULL
};

void ExportSerializedStateWriterBindings();
void ExportSerializedStateWriterBindings()
{
    for (int i = 0; s_SerializedStateWriter_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_SerializedStateWriter_IcallNames [i], s_SerializedStateWriter_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportSerializedStateWriterBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_SERIALIZATION_BY_CODEGENERATION
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_Align); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_Align
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteByte); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteByte
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteInt32); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteInt32
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteSingle); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteSingle
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteDouble); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteDouble
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteBoolean); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteBoolean
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteString); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteString
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteUnityEngineObject); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteUnityEngineObject
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteAnimationCurve); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteAnimationCurve
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteGradient); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGradient
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteGUIStyle); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteGUIStyle
    SET_METRO_BINDING(SerializedStateWriter_CUSTOM_INTERNAL_WriteRectOffset); //  <- UnityEngine.Serialization.SerializedStateWriter::INTERNAL_WriteRectOffset
#endif
}

#endif
