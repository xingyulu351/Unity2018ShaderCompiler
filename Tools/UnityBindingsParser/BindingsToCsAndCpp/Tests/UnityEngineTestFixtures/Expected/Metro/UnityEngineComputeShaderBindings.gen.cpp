#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Shaders/ComputeShader.h"
#include "Runtime/Graphics/Texture.h"
#include "Runtime/Math/Vector4.h"
#include "Runtime/Threads/AtomicOps.h"

#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"


SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_FindKernel(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_FindKernel)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(FindKernel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindKernel)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		return self->FindKernel (fpName);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_SetFloat(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, float val)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_SetFloat)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetFloat)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		self->SetValueParam (fpName, 4, &val);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_SetInt(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, int val)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_SetInt)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(SetInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetInt)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		self->SetValueParam (fpName, 4, &val);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_INTERNAL_CALL_SetVector(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, const Vector4f& val)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_INTERNAL_CALL_SetVector)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetVector)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetVector)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		self->SetValueParam (fpName, 16, &val.x);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_Internal_SetFloats(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, ICallType_Array_Argument values_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_Internal_SetFloats)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_Array_Local values(values_);
    UNUSED(values);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetFloats)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetFloats)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		int size = GetScriptingArraySize (values);
    		const float* arr = GetScriptingArrayStart<float> (values);
    		self->SetValueParam (fpName, size*4, arr);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_Internal_SetInts(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, ICallType_Array_Argument values_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_Internal_SetInts)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_Array_Local values(values_);
    UNUSED(values);
    SCRIPTINGAPI_STACK_CHECK(Internal_SetInts)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetInts)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		int size = GetScriptingArraySize (values);
    		const int* arr = GetScriptingArrayStart<int> (values);
    		self->SetValueParam (fpName, size*4, arr);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_SetTexture(ICallType_ReadOnlyUnityEngineObject_Argument self_, int kernelIndex, ICallType_String_Argument name_, ICallType_ReadOnlyUnityEngineObject_Argument texture_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_SetTexture)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ReadOnlyScriptingObjectOfType<Texture> texture(texture_);
    UNUSED(texture);
    SCRIPTINGAPI_STACK_CHECK(SetTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetTexture)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		Texture& tex = *texture;
    		self->SetTextureParam (kernelIndex, fpName, tex.GetTextureID());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_SetBuffer(ICallType_ReadOnlyUnityEngineObject_Argument self_, int kernelIndex, ICallType_String_Argument name_, ICallType_Object_Argument buffer_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_SetBuffer)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ScriptingObjectWithIntPtrField<ComputeBuffer> buffer(buffer_);
    UNUSED(buffer);
    SCRIPTINGAPI_STACK_CHECK(SetBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetBuffer)
    
    		FastPropertyName fpName = ScriptingStringToProperty(name);
    		self->SetBufferParam (kernelIndex, fpName, buffer->GetBufferHandle());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeShader_CUSTOM_Dispatch(ICallType_ReadOnlyUnityEngineObject_Argument self_, int kernelIndex, int threadsX, int threadsY, int threadsZ)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeShader_CUSTOM_Dispatch)
    ReadOnlyScriptingObjectOfType<ComputeShader> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Dispatch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Dispatch)
    
    		self->DispatchComputeShader (kernelIndex, threadsX, threadsY, threadsZ);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeBuffer_CUSTOM_InitBuffer(ICallType_Object_Argument buf_, int count, int stride, UInt32 type)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_CUSTOM_InitBuffer)
    ScriptingObjectWithIntPtrField<ComputeBuffer> buf(buf_);
    UNUSED(buf);
    SCRIPTINGAPI_STACK_CHECK(InitBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitBuffer)
    
    		buf.SetPtr(new ComputeBuffer(count, stride, type));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeBuffer_CUSTOM_DestroyBuffer(ICallType_Object_Argument buf_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_CUSTOM_DestroyBuffer)
    ScriptingObjectWithIntPtrField<ComputeBuffer> buf(buf_);
    UNUSED(buf);
    SCRIPTINGAPI_STACK_CHECK(DestroyBuffer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DestroyBuffer)
    
    		delete buf.GetPtr();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ComputeBuffer_Get_Custom_PropCount(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_Get_Custom_PropCount)
    ScriptingObjectWithIntPtrField<ComputeBuffer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_count)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_count)
    return self->GetCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION ComputeBuffer_Get_Custom_PropStride(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_Get_Custom_PropStride)
    ScriptingObjectWithIntPtrField<ComputeBuffer> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stride)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stride)
    return self->GetStride();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeBuffer_CUSTOM_InternalSetData(ICallType_Object_Argument self_, ICallType_Array_Argument data_, int elemSize)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_CUSTOM_InternalSetData)
    ScriptingObjectWithIntPtrField<ComputeBuffer> self(self_);
    UNUSED(self);
    ICallType_Array_Local data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(InternalSetData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalSetData)
    
    		self->SetData (GetScriptingArrayStart<char>(data), GetScriptingArraySize(data) * elemSize);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeBuffer_CUSTOM_InternalGetData(ICallType_Object_Argument self_, ICallType_Array_Argument data_, int elemSize)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_CUSTOM_InternalGetData)
    ScriptingObjectWithIntPtrField<ComputeBuffer> self(self_);
    UNUSED(self);
    ICallType_Array_Local data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(InternalGetData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InternalGetData)
    
    		self->GetData (GetScriptingArrayStart<char>(data), GetScriptingArraySize(data) * elemSize);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ComputeBuffer_CUSTOM_CopyCount(ICallType_Object_Argument src_, ICallType_Object_Argument dst_, int dstOffset)
{
    SCRIPTINGAPI_ETW_ENTRY(ComputeBuffer_CUSTOM_CopyCount)
    ScriptingObjectWithIntPtrField<ComputeBuffer> src(src_);
    UNUSED(src);
    ScriptingObjectWithIntPtrField<ComputeBuffer> dst(dst_);
    UNUSED(dst);
    SCRIPTINGAPI_STACK_CHECK(CopyCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CopyCount)
    
    		ComputeBuffer::CopyCount (src.GetPtr(), dst.GetPtr(), dstOffset);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_ComputeShader_FindKernel()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::FindKernel" , (gpointer)& ComputeShader_CUSTOM_FindKernel );
}

void Register_UnityEngine_ComputeShader_SetFloat()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::SetFloat" , (gpointer)& ComputeShader_CUSTOM_SetFloat );
}

void Register_UnityEngine_ComputeShader_SetInt()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::SetInt" , (gpointer)& ComputeShader_CUSTOM_SetInt );
}

void Register_UnityEngine_ComputeShader_INTERNAL_CALL_SetVector()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::INTERNAL_CALL_SetVector" , (gpointer)& ComputeShader_CUSTOM_INTERNAL_CALL_SetVector );
}

void Register_UnityEngine_ComputeShader_Internal_SetFloats()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::Internal_SetFloats" , (gpointer)& ComputeShader_CUSTOM_Internal_SetFloats );
}

void Register_UnityEngine_ComputeShader_Internal_SetInts()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::Internal_SetInts" , (gpointer)& ComputeShader_CUSTOM_Internal_SetInts );
}

void Register_UnityEngine_ComputeShader_SetTexture()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::SetTexture" , (gpointer)& ComputeShader_CUSTOM_SetTexture );
}

void Register_UnityEngine_ComputeShader_SetBuffer()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::SetBuffer" , (gpointer)& ComputeShader_CUSTOM_SetBuffer );
}

void Register_UnityEngine_ComputeShader_Dispatch()
{
    scripting_add_internal_call( "UnityEngine.ComputeShader::Dispatch" , (gpointer)& ComputeShader_CUSTOM_Dispatch );
}

void Register_UnityEngine_ComputeBuffer_InitBuffer()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::InitBuffer" , (gpointer)& ComputeBuffer_CUSTOM_InitBuffer );
}

void Register_UnityEngine_ComputeBuffer_DestroyBuffer()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::DestroyBuffer" , (gpointer)& ComputeBuffer_CUSTOM_DestroyBuffer );
}

void Register_UnityEngine_ComputeBuffer_get_count()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::get_count" , (gpointer)& ComputeBuffer_Get_Custom_PropCount );
}

void Register_UnityEngine_ComputeBuffer_get_stride()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::get_stride" , (gpointer)& ComputeBuffer_Get_Custom_PropStride );
}

void Register_UnityEngine_ComputeBuffer_InternalSetData()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::InternalSetData" , (gpointer)& ComputeBuffer_CUSTOM_InternalSetData );
}

void Register_UnityEngine_ComputeBuffer_InternalGetData()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::InternalGetData" , (gpointer)& ComputeBuffer_CUSTOM_InternalGetData );
}

void Register_UnityEngine_ComputeBuffer_CopyCount()
{
    scripting_add_internal_call( "UnityEngine.ComputeBuffer::CopyCount" , (gpointer)& ComputeBuffer_CUSTOM_CopyCount );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineComputeShader_IcallNames [] =
{
    "UnityEngine.ComputeShader::FindKernel" ,    // -> ComputeShader_CUSTOM_FindKernel
    "UnityEngine.ComputeShader::SetFloat"   ,    // -> ComputeShader_CUSTOM_SetFloat
    "UnityEngine.ComputeShader::SetInt"     ,    // -> ComputeShader_CUSTOM_SetInt
    "UnityEngine.ComputeShader::INTERNAL_CALL_SetVector",    // -> ComputeShader_CUSTOM_INTERNAL_CALL_SetVector
    "UnityEngine.ComputeShader::Internal_SetFloats",    // -> ComputeShader_CUSTOM_Internal_SetFloats
    "UnityEngine.ComputeShader::Internal_SetInts",    // -> ComputeShader_CUSTOM_Internal_SetInts
    "UnityEngine.ComputeShader::SetTexture" ,    // -> ComputeShader_CUSTOM_SetTexture
    "UnityEngine.ComputeShader::SetBuffer"  ,    // -> ComputeShader_CUSTOM_SetBuffer
    "UnityEngine.ComputeShader::Dispatch"   ,    // -> ComputeShader_CUSTOM_Dispatch
    "UnityEngine.ComputeBuffer::InitBuffer" ,    // -> ComputeBuffer_CUSTOM_InitBuffer
    "UnityEngine.ComputeBuffer::DestroyBuffer",    // -> ComputeBuffer_CUSTOM_DestroyBuffer
    "UnityEngine.ComputeBuffer::get_count"  ,    // -> ComputeBuffer_Get_Custom_PropCount
    "UnityEngine.ComputeBuffer::get_stride" ,    // -> ComputeBuffer_Get_Custom_PropStride
    "UnityEngine.ComputeBuffer::InternalSetData",    // -> ComputeBuffer_CUSTOM_InternalSetData
    "UnityEngine.ComputeBuffer::InternalGetData",    // -> ComputeBuffer_CUSTOM_InternalGetData
    "UnityEngine.ComputeBuffer::CopyCount"  ,    // -> ComputeBuffer_CUSTOM_CopyCount
    NULL
};

static const void* s_UnityEngineComputeShader_IcallFuncs [] =
{
    (const void*)&ComputeShader_CUSTOM_FindKernel         ,  //  <- UnityEngine.ComputeShader::FindKernel
    (const void*)&ComputeShader_CUSTOM_SetFloat           ,  //  <- UnityEngine.ComputeShader::SetFloat
    (const void*)&ComputeShader_CUSTOM_SetInt             ,  //  <- UnityEngine.ComputeShader::SetInt
    (const void*)&ComputeShader_CUSTOM_INTERNAL_CALL_SetVector,  //  <- UnityEngine.ComputeShader::INTERNAL_CALL_SetVector
    (const void*)&ComputeShader_CUSTOM_Internal_SetFloats ,  //  <- UnityEngine.ComputeShader::Internal_SetFloats
    (const void*)&ComputeShader_CUSTOM_Internal_SetInts   ,  //  <- UnityEngine.ComputeShader::Internal_SetInts
    (const void*)&ComputeShader_CUSTOM_SetTexture         ,  //  <- UnityEngine.ComputeShader::SetTexture
    (const void*)&ComputeShader_CUSTOM_SetBuffer          ,  //  <- UnityEngine.ComputeShader::SetBuffer
    (const void*)&ComputeShader_CUSTOM_Dispatch           ,  //  <- UnityEngine.ComputeShader::Dispatch
    (const void*)&ComputeBuffer_CUSTOM_InitBuffer         ,  //  <- UnityEngine.ComputeBuffer::InitBuffer
    (const void*)&ComputeBuffer_CUSTOM_DestroyBuffer      ,  //  <- UnityEngine.ComputeBuffer::DestroyBuffer
    (const void*)&ComputeBuffer_Get_Custom_PropCount      ,  //  <- UnityEngine.ComputeBuffer::get_count
    (const void*)&ComputeBuffer_Get_Custom_PropStride     ,  //  <- UnityEngine.ComputeBuffer::get_stride
    (const void*)&ComputeBuffer_CUSTOM_InternalSetData    ,  //  <- UnityEngine.ComputeBuffer::InternalSetData
    (const void*)&ComputeBuffer_CUSTOM_InternalGetData    ,  //  <- UnityEngine.ComputeBuffer::InternalGetData
    (const void*)&ComputeBuffer_CUSTOM_CopyCount          ,  //  <- UnityEngine.ComputeBuffer::CopyCount
    NULL
};

void ExportUnityEngineComputeShaderBindings();
void ExportUnityEngineComputeShaderBindings()
{
    for (int i = 0; s_UnityEngineComputeShader_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineComputeShader_IcallNames [i], s_UnityEngineComputeShader_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineComputeShaderBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(ComputeShader_CUSTOM_FindKernel); //  <- UnityEngine.ComputeShader::FindKernel
    SET_METRO_BINDING(ComputeShader_CUSTOM_SetFloat); //  <- UnityEngine.ComputeShader::SetFloat
    SET_METRO_BINDING(ComputeShader_CUSTOM_SetInt); //  <- UnityEngine.ComputeShader::SetInt
    SET_METRO_BINDING(ComputeShader_CUSTOM_INTERNAL_CALL_SetVector); //  <- UnityEngine.ComputeShader::INTERNAL_CALL_SetVector
    SET_METRO_BINDING(ComputeShader_CUSTOM_Internal_SetFloats); //  <- UnityEngine.ComputeShader::Internal_SetFloats
    SET_METRO_BINDING(ComputeShader_CUSTOM_Internal_SetInts); //  <- UnityEngine.ComputeShader::Internal_SetInts
    SET_METRO_BINDING(ComputeShader_CUSTOM_SetTexture); //  <- UnityEngine.ComputeShader::SetTexture
    SET_METRO_BINDING(ComputeShader_CUSTOM_SetBuffer); //  <- UnityEngine.ComputeShader::SetBuffer
    SET_METRO_BINDING(ComputeShader_CUSTOM_Dispatch); //  <- UnityEngine.ComputeShader::Dispatch
    SET_METRO_BINDING(ComputeBuffer_CUSTOM_InitBuffer); //  <- UnityEngine.ComputeBuffer::InitBuffer
    SET_METRO_BINDING(ComputeBuffer_CUSTOM_DestroyBuffer); //  <- UnityEngine.ComputeBuffer::DestroyBuffer
    SET_METRO_BINDING(ComputeBuffer_Get_Custom_PropCount); //  <- UnityEngine.ComputeBuffer::get_count
    SET_METRO_BINDING(ComputeBuffer_Get_Custom_PropStride); //  <- UnityEngine.ComputeBuffer::get_stride
    SET_METRO_BINDING(ComputeBuffer_CUSTOM_InternalSetData); //  <- UnityEngine.ComputeBuffer::InternalSetData
    SET_METRO_BINDING(ComputeBuffer_CUSTOM_InternalGetData); //  <- UnityEngine.ComputeBuffer::InternalGetData
    SET_METRO_BINDING(ComputeBuffer_CUSTOM_CopyCount); //  <- UnityEngine.ComputeBuffer::CopyCount
}

#endif
