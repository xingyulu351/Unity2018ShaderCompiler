#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Scripting/ScriptingManager.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Graphics/VideoTexture.h"
#include "Runtime/Mono/MonoBehaviour.h"
#if ENABLE_WEBCAM
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_Internal_CreateWebCamTexture(ICallType_Object_Argument self_, ICallType_String_Argument device_, int requestedWidth, int requestedHeight, int maxFramerate)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_Internal_CreateWebCamTexture)
    ScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    ICallType_String_Local device(device_);
    UNUSED(device);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateWebCamTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_CreateWebCamTexture)
    
    		WebCamTexture* texture = NEW_OBJECT_MAIN_THREAD (WebCamTexture);
    		texture->Reset();
    		ConnectScriptingWrapperToObject (self.GetScriptingObject(), texture);
    		texture->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    		texture->SetRequestedWidth (requestedWidth);
    		texture->SetRequestedHeight (requestedHeight);
    		texture->SetRequestedFPS (maxFramerate);
    		texture->SetDevice (device);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_INTERNAL_CALL_Play(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_INTERNAL_CALL_Play)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Play)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Play)
    return self->Play();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_INTERNAL_CALL_Pause(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_INTERNAL_CALL_Pause)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Pause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Pause)
    return self->Pause();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_INTERNAL_CALL_Stop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_INTERNAL_CALL_Stop)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Stop)
    return self->Stop();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropIsPlaying(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropIsPlaying)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPlaying)
    return self->IsPlaying ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropDeviceName(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropDeviceName)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_deviceName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceName)
    return CreateScriptingString (self->GetDevice ());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_Set_Custom_PropDeviceName(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Set_Custom_PropDeviceName)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_deviceName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_deviceName)
     self->SetDevice (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropRequestedFPS(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropRequestedFPS)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_requestedFPS)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_requestedFPS)
    return self->GetRequestedFPS ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_Set_Custom_PropRequestedFPS(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Set_Custom_PropRequestedFPS)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_requestedFPS)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_requestedFPS)
    
    self->SetRequestedFPS (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropRequestedWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropRequestedWidth)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_requestedWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_requestedWidth)
    return self->GetRequestedWidth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_Set_Custom_PropRequestedWidth(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Set_Custom_PropRequestedWidth)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_requestedWidth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_requestedWidth)
    
    self->SetRequestedWidth (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropRequestedHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropRequestedHeight)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_requestedHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_requestedHeight)
    return self->GetRequestedHeight ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_Set_Custom_PropRequestedHeight(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Set_Custom_PropRequestedHeight)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_requestedHeight)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_requestedHeight)
    
    self->SetRequestedHeight (value);
    
}

#endif
#if (ENABLE_WEBCAM) && (UNITY_IPHONE_API)
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropIsReadable(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropIsReadable)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isReadable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isReadable)
    return self->IsReadable();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_MarkNonReadable(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_MarkNonReadable)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(MarkNonReadable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(MarkNonReadable)
     self->SetReadable(false); 
}

#endif
#if ENABLE_WEBCAM
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropDevices()
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropDevices)
    SCRIPTINGAPI_STACK_CHECK(get_devices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_devices)
    MonoWebCamDevices devs;
    WebCamTexture::GetDeviceNames(devs, true);
    ScriptingClassPtr klass = GetScriptingManager().GetCommonClasses().webCamDevice;
    ScriptingArrayPtr array = CreateScriptingArray<MonoWebCamDevice>(klass, devs.size());
    for (MonoWebCamDevices::size_type i = 0; i < devs.size(); ++i)
    {
    #if PLATFORM_WINRT
    ScriptingObjectPtr dev = CreateScriptingObjectFromNativeStruct(klass, devs[i]);
    SetScriptingArrayElement<ScriptingObjectPtr>(array, i, dev);
    #else
    SetScriptingArrayElement<MonoWebCamDevice>(array, i, devs[i]);
    #endif
    }
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_INTERNAL_CALL_GetPixel(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, ColorRGBAf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_INTERNAL_CALL_GetPixel)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetPixel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetPixel)
    
    		{ returnValue =(self->GetPixel (x, y)); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_GetPixels(ICallType_ReadOnlyUnityEngineObject_Argument self_, int x, int y, int blockWidth, int blockHeight)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_GetPixels)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetPixels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPixels)
    
    		int res = blockWidth * blockHeight;
    		if (blockWidth != 0 && blockHeight != res / blockWidth) {
    			return SCRIPTING_NULL;
    		}
    		ScriptingArrayPtr colors = CreateScriptingArray<ColorRGBAf>(GetScriptingManager().GetCommonClasses().color, res );
    		self->GetPixels( x, y, blockWidth, blockHeight, &GetScriptingArrayElement<ColorRGBAf>(colors, 0));
    		return colors;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION WebCamTexture_CUSTOM_GetPixels32(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument colors_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_CUSTOM_GetPixels32)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    ICallType_Array_Local colors(colors_);
    UNUSED(colors);
    SCRIPTINGAPI_STACK_CHECK(GetPixels32)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetPixels32)
    
    		int w = self->GetDataWidth();
    		int h = self->GetDataHeight();
    		if (colors != SCRIPTING_NULL)
    		{
    			int size = GetScriptingArraySize(colors);
    			if (size != w * h)
    			{
    				ErrorStringMsg ("Input color array length needs to match width * height, but %d != %d * %d", size, w, h);
    				return SCRIPTING_NULL;
    			}
    		}
    		else
    			colors = CreateScriptingArray<ColorRGBA32>(GetScriptingManager().GetCommonClasses().color32, w * h);
    		self->GetPixels(kTexFormatRGBA32, &GetScriptingArrayElement<ColorRGBA32>(colors, 0), GetScriptingArraySize(colors) * 4);
    		return colors;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropVideoRotationAngle(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropVideoRotationAngle)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_videoRotationAngle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_videoRotationAngle)
    return self->GetVideoRotationAngle();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION WebCamTexture_Get_Custom_PropDidUpdateThisFrame(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WebCamTexture_Get_Custom_PropDidUpdateThisFrame)
    ReadOnlyScriptingObjectOfType<BaseWebCamTexture> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_didUpdateThisFrame)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_didUpdateThisFrame)
    return self->DidUpdateThisFrame ();
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_WEBCAM
void Register_UnityEngine_WebCamTexture_Internal_CreateWebCamTexture()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::Internal_CreateWebCamTexture" , (gpointer)& WebCamTexture_CUSTOM_Internal_CreateWebCamTexture );
}

void Register_UnityEngine_WebCamTexture_INTERNAL_CALL_Play()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::INTERNAL_CALL_Play" , (gpointer)& WebCamTexture_CUSTOM_INTERNAL_CALL_Play );
}

void Register_UnityEngine_WebCamTexture_INTERNAL_CALL_Pause()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::INTERNAL_CALL_Pause" , (gpointer)& WebCamTexture_CUSTOM_INTERNAL_CALL_Pause );
}

void Register_UnityEngine_WebCamTexture_INTERNAL_CALL_Stop()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::INTERNAL_CALL_Stop" , (gpointer)& WebCamTexture_CUSTOM_INTERNAL_CALL_Stop );
}

void Register_UnityEngine_WebCamTexture_get_isPlaying()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_isPlaying" , (gpointer)& WebCamTexture_Get_Custom_PropIsPlaying );
}

void Register_UnityEngine_WebCamTexture_get_deviceName()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_deviceName" , (gpointer)& WebCamTexture_Get_Custom_PropDeviceName );
}

void Register_UnityEngine_WebCamTexture_set_deviceName()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::set_deviceName" , (gpointer)& WebCamTexture_Set_Custom_PropDeviceName );
}

void Register_UnityEngine_WebCamTexture_get_requestedFPS()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_requestedFPS" , (gpointer)& WebCamTexture_Get_Custom_PropRequestedFPS );
}

void Register_UnityEngine_WebCamTexture_set_requestedFPS()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::set_requestedFPS" , (gpointer)& WebCamTexture_Set_Custom_PropRequestedFPS );
}

void Register_UnityEngine_WebCamTexture_get_requestedWidth()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_requestedWidth" , (gpointer)& WebCamTexture_Get_Custom_PropRequestedWidth );
}

void Register_UnityEngine_WebCamTexture_set_requestedWidth()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::set_requestedWidth" , (gpointer)& WebCamTexture_Set_Custom_PropRequestedWidth );
}

void Register_UnityEngine_WebCamTexture_get_requestedHeight()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_requestedHeight" , (gpointer)& WebCamTexture_Get_Custom_PropRequestedHeight );
}

void Register_UnityEngine_WebCamTexture_set_requestedHeight()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::set_requestedHeight" , (gpointer)& WebCamTexture_Set_Custom_PropRequestedHeight );
}

#endif
#if (ENABLE_WEBCAM) && (UNITY_IPHONE_API)
void Register_UnityEngine_WebCamTexture_get_isReadable()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_isReadable" , (gpointer)& WebCamTexture_Get_Custom_PropIsReadable );
}

void Register_UnityEngine_WebCamTexture_MarkNonReadable()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::MarkNonReadable" , (gpointer)& WebCamTexture_CUSTOM_MarkNonReadable );
}

#endif
#if ENABLE_WEBCAM
void Register_UnityEngine_WebCamTexture_get_devices()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_devices" , (gpointer)& WebCamTexture_Get_Custom_PropDevices );
}

void Register_UnityEngine_WebCamTexture_INTERNAL_CALL_GetPixel()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::INTERNAL_CALL_GetPixel" , (gpointer)& WebCamTexture_CUSTOM_INTERNAL_CALL_GetPixel );
}

void Register_UnityEngine_WebCamTexture_GetPixels()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::GetPixels" , (gpointer)& WebCamTexture_CUSTOM_GetPixels );
}

void Register_UnityEngine_WebCamTexture_GetPixels32()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::GetPixels32" , (gpointer)& WebCamTexture_CUSTOM_GetPixels32 );
}

void Register_UnityEngine_WebCamTexture_get_videoRotationAngle()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_videoRotationAngle" , (gpointer)& WebCamTexture_Get_Custom_PropVideoRotationAngle );
}

void Register_UnityEngine_WebCamTexture_get_didUpdateThisFrame()
{
    scripting_add_internal_call( "UnityEngine.WebCamTexture::get_didUpdateThisFrame" , (gpointer)& WebCamTexture_Get_Custom_PropDidUpdateThisFrame );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineWebCamTexture_IcallNames [] =
{
#if ENABLE_WEBCAM
    "UnityEngine.WebCamTexture::Internal_CreateWebCamTexture",    // -> WebCamTexture_CUSTOM_Internal_CreateWebCamTexture
    "UnityEngine.WebCamTexture::INTERNAL_CALL_Play",    // -> WebCamTexture_CUSTOM_INTERNAL_CALL_Play
    "UnityEngine.WebCamTexture::INTERNAL_CALL_Pause",    // -> WebCamTexture_CUSTOM_INTERNAL_CALL_Pause
    "UnityEngine.WebCamTexture::INTERNAL_CALL_Stop",    // -> WebCamTexture_CUSTOM_INTERNAL_CALL_Stop
    "UnityEngine.WebCamTexture::get_isPlaying",    // -> WebCamTexture_Get_Custom_PropIsPlaying
    "UnityEngine.WebCamTexture::get_deviceName",    // -> WebCamTexture_Get_Custom_PropDeviceName
    "UnityEngine.WebCamTexture::set_deviceName",    // -> WebCamTexture_Set_Custom_PropDeviceName
    "UnityEngine.WebCamTexture::get_requestedFPS",    // -> WebCamTexture_Get_Custom_PropRequestedFPS
    "UnityEngine.WebCamTexture::set_requestedFPS",    // -> WebCamTexture_Set_Custom_PropRequestedFPS
    "UnityEngine.WebCamTexture::get_requestedWidth",    // -> WebCamTexture_Get_Custom_PropRequestedWidth
    "UnityEngine.WebCamTexture::set_requestedWidth",    // -> WebCamTexture_Set_Custom_PropRequestedWidth
    "UnityEngine.WebCamTexture::get_requestedHeight",    // -> WebCamTexture_Get_Custom_PropRequestedHeight
    "UnityEngine.WebCamTexture::set_requestedHeight",    // -> WebCamTexture_Set_Custom_PropRequestedHeight
#endif
#if (ENABLE_WEBCAM) && (UNITY_IPHONE_API)
    "UnityEngine.WebCamTexture::get_isReadable",    // -> WebCamTexture_Get_Custom_PropIsReadable
    "UnityEngine.WebCamTexture::MarkNonReadable",    // -> WebCamTexture_CUSTOM_MarkNonReadable
#endif
#if ENABLE_WEBCAM
    "UnityEngine.WebCamTexture::get_devices",    // -> WebCamTexture_Get_Custom_PropDevices
    "UnityEngine.WebCamTexture::INTERNAL_CALL_GetPixel",    // -> WebCamTexture_CUSTOM_INTERNAL_CALL_GetPixel
    "UnityEngine.WebCamTexture::GetPixels"  ,    // -> WebCamTexture_CUSTOM_GetPixels
    "UnityEngine.WebCamTexture::GetPixels32",    // -> WebCamTexture_CUSTOM_GetPixels32
    "UnityEngine.WebCamTexture::get_videoRotationAngle",    // -> WebCamTexture_Get_Custom_PropVideoRotationAngle
    "UnityEngine.WebCamTexture::get_didUpdateThisFrame",    // -> WebCamTexture_Get_Custom_PropDidUpdateThisFrame
#endif
    NULL
};

static const void* s_UnityEngineWebCamTexture_IcallFuncs [] =
{
#if ENABLE_WEBCAM
    (const void*)&WebCamTexture_CUSTOM_Internal_CreateWebCamTexture,  //  <- UnityEngine.WebCamTexture::Internal_CreateWebCamTexture
    (const void*)&WebCamTexture_CUSTOM_INTERNAL_CALL_Play ,  //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_Play
    (const void*)&WebCamTexture_CUSTOM_INTERNAL_CALL_Pause,  //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_Pause
    (const void*)&WebCamTexture_CUSTOM_INTERNAL_CALL_Stop ,  //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_Stop
    (const void*)&WebCamTexture_Get_Custom_PropIsPlaying  ,  //  <- UnityEngine.WebCamTexture::get_isPlaying
    (const void*)&WebCamTexture_Get_Custom_PropDeviceName ,  //  <- UnityEngine.WebCamTexture::get_deviceName
    (const void*)&WebCamTexture_Set_Custom_PropDeviceName ,  //  <- UnityEngine.WebCamTexture::set_deviceName
    (const void*)&WebCamTexture_Get_Custom_PropRequestedFPS,  //  <- UnityEngine.WebCamTexture::get_requestedFPS
    (const void*)&WebCamTexture_Set_Custom_PropRequestedFPS,  //  <- UnityEngine.WebCamTexture::set_requestedFPS
    (const void*)&WebCamTexture_Get_Custom_PropRequestedWidth,  //  <- UnityEngine.WebCamTexture::get_requestedWidth
    (const void*)&WebCamTexture_Set_Custom_PropRequestedWidth,  //  <- UnityEngine.WebCamTexture::set_requestedWidth
    (const void*)&WebCamTexture_Get_Custom_PropRequestedHeight,  //  <- UnityEngine.WebCamTexture::get_requestedHeight
    (const void*)&WebCamTexture_Set_Custom_PropRequestedHeight,  //  <- UnityEngine.WebCamTexture::set_requestedHeight
#endif
#if (ENABLE_WEBCAM) && (UNITY_IPHONE_API)
    (const void*)&WebCamTexture_Get_Custom_PropIsReadable ,  //  <- UnityEngine.WebCamTexture::get_isReadable
    (const void*)&WebCamTexture_CUSTOM_MarkNonReadable    ,  //  <- UnityEngine.WebCamTexture::MarkNonReadable
#endif
#if ENABLE_WEBCAM
    (const void*)&WebCamTexture_Get_Custom_PropDevices    ,  //  <- UnityEngine.WebCamTexture::get_devices
    (const void*)&WebCamTexture_CUSTOM_INTERNAL_CALL_GetPixel,  //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_GetPixel
    (const void*)&WebCamTexture_CUSTOM_GetPixels          ,  //  <- UnityEngine.WebCamTexture::GetPixels
    (const void*)&WebCamTexture_CUSTOM_GetPixels32        ,  //  <- UnityEngine.WebCamTexture::GetPixels32
    (const void*)&WebCamTexture_Get_Custom_PropVideoRotationAngle,  //  <- UnityEngine.WebCamTexture::get_videoRotationAngle
    (const void*)&WebCamTexture_Get_Custom_PropDidUpdateThisFrame,  //  <- UnityEngine.WebCamTexture::get_didUpdateThisFrame
#endif
    NULL
};

void ExportUnityEngineWebCamTextureBindings();
void ExportUnityEngineWebCamTextureBindings()
{
    for (int i = 0; s_UnityEngineWebCamTexture_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineWebCamTexture_IcallNames [i], s_UnityEngineWebCamTexture_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineWebCamTextureBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_WEBCAM
    SET_METRO_BINDING(WebCamTexture_CUSTOM_Internal_CreateWebCamTexture); //  <- UnityEngine.WebCamTexture::Internal_CreateWebCamTexture
    SET_METRO_BINDING(WebCamTexture_CUSTOM_INTERNAL_CALL_Play); //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_Play
    SET_METRO_BINDING(WebCamTexture_CUSTOM_INTERNAL_CALL_Pause); //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_Pause
    SET_METRO_BINDING(WebCamTexture_CUSTOM_INTERNAL_CALL_Stop); //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_Stop
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropIsPlaying); //  <- UnityEngine.WebCamTexture::get_isPlaying
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropDeviceName); //  <- UnityEngine.WebCamTexture::get_deviceName
    SET_METRO_BINDING(WebCamTexture_Set_Custom_PropDeviceName); //  <- UnityEngine.WebCamTexture::set_deviceName
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropRequestedFPS); //  <- UnityEngine.WebCamTexture::get_requestedFPS
    SET_METRO_BINDING(WebCamTexture_Set_Custom_PropRequestedFPS); //  <- UnityEngine.WebCamTexture::set_requestedFPS
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropRequestedWidth); //  <- UnityEngine.WebCamTexture::get_requestedWidth
    SET_METRO_BINDING(WebCamTexture_Set_Custom_PropRequestedWidth); //  <- UnityEngine.WebCamTexture::set_requestedWidth
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropRequestedHeight); //  <- UnityEngine.WebCamTexture::get_requestedHeight
    SET_METRO_BINDING(WebCamTexture_Set_Custom_PropRequestedHeight); //  <- UnityEngine.WebCamTexture::set_requestedHeight
#endif
#if (ENABLE_WEBCAM) && (UNITY_IPHONE_API)
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropIsReadable); //  <- UnityEngine.WebCamTexture::get_isReadable
    SET_METRO_BINDING(WebCamTexture_CUSTOM_MarkNonReadable); //  <- UnityEngine.WebCamTexture::MarkNonReadable
#endif
#if ENABLE_WEBCAM
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropDevices); //  <- UnityEngine.WebCamTexture::get_devices
    SET_METRO_BINDING(WebCamTexture_CUSTOM_INTERNAL_CALL_GetPixel); //  <- UnityEngine.WebCamTexture::INTERNAL_CALL_GetPixel
    SET_METRO_BINDING(WebCamTexture_CUSTOM_GetPixels); //  <- UnityEngine.WebCamTexture::GetPixels
    SET_METRO_BINDING(WebCamTexture_CUSTOM_GetPixels32); //  <- UnityEngine.WebCamTexture::GetPixels32
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropVideoRotationAngle); //  <- UnityEngine.WebCamTexture::get_videoRotationAngle
    SET_METRO_BINDING(WebCamTexture_Get_Custom_PropDidUpdateThisFrame); //  <- UnityEngine.WebCamTexture::get_didUpdateThisFrame
#endif
}

#endif
