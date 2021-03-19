#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Graphics/Texture2D.h"
#include "Runtime/Graphics/ImageConversion.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/Export/WWW.h"
#include "Runtime/Misc/WWWCached.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/ScriptingTypes.h"
#include "Runtime/Scripting/ScriptingManager.h"

#if WEBPLUG
#include "PlatformDependent/CommonWebPlugin/UnityWebStream.h"
#include "PlatformDependent/CommonWebPlugin/CompressedFileStreamMemory.h"
#endif
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Serialize/PersistentManager.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/Utilities/GUID.h"
#include "Runtime/Misc/CachingManager.h"
#include "Runtime/Misc/AssetBundleUtility.h"

#if UNITY_EDITOR
#include "Editor/Src/LicenseInfo.h"
#endif

#if UNITY_FLASH
#include "PlatformDependent/FlashSupport/cpp/WWWFlash.h"
#endif

#if ENABLE_MOVIES
#include "Runtime/Graphics/MovieTexture.h"
#endif
#if ENABLE_AUDIO
#include "Runtime/Audio/AudioClip.h"
#endif
#include "Runtime/Misc/Player.h"
#include "Runtime/Utilities/PathNameUtility.h"


	#if ENABLE_WWW
	inline WWW* GetWWWChecked (ScriptingObjectWithIntPtrField<WWW>& self)
	{
		WWW* www = self.GetPtr();
		
		if (!www)
			RaiseNullException("WWW class has already been disposed.");

		return www;
	}
	#endif

 #define GET GetWWWChecked (self)
#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WWW_CUSTOM_DestroyWWW(ICallType_Object_Argument self_, ScriptingBool cancel)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_DestroyWWW)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(DestroyWWW)
    
    		WWW* www = self.GetPtr();
    
    		self.SetPtr(0);
    
    		if (www)
    		{
    			if (cancel)
    				www->Cancel();
    			www->Release();
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WWW_CUSTOM_InitWWW(ICallType_Object_Argument self_, ICallType_String_Argument url_, ICallType_Array_Argument postData_, ICallType_Array_Argument iHeaders_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_InitWWW)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    ICallType_String_Local url(url_);
    UNUSED(url);
    ICallType_Array_Local postData(postData_);
    UNUSED(postData);
    ICallType_Array_Local iHeaders(iHeaders_);
    UNUSED(iHeaders);
    SCRIPTINGAPI_STACK_CHECK(InitWWW)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitWWW)
    
    		string cpp_string = url;
    		map<string,string> headers;
    		int headersSize = GetScriptingArraySize(iHeaders);
    		for(int i=0; i < headersSize-1 ; i += 2) {
    			headers[scripting_cpp_string_for(GetScriptingArrayElementNoRef<ScriptingStringPtr>(iHeaders,i))] = scripting_cpp_string_for(GetScriptingArrayElementNoRef<ScriptingStringPtr>(iHeaders,i+1));
    		}
    		int rawPostDataLength = -1;
    		char* rawPostDataPtr = NULL;
    		if(postData != SCRIPTING_NULL) {
    			rawPostDataPtr = GetScriptingArrayStart<char>(postData); 
    			rawPostDataLength = GetScriptingArraySize(postData);
    		}
    		WWW* www = WWW::Create (cpp_string.c_str(), rawPostDataPtr, rawPostDataLength, headers);
    		self.SetPtr(www);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropResponseHeadersString(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropResponseHeadersString)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_responseHeadersString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_responseHeadersString)
    WWW& www = *GET;
    ScriptingStringPtr res = CreateScriptingString(www.GetResponseHeaders().c_str());
    return res;
}

#endif
#if (ENABLE_WWW) && (UNITY_FLASH)
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropStringFromByteArray(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropStringFromByteArray)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stringFromByteArray)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stringFromByteArray)
    WWWFlash *wwwflash = (WWWFlash*)GET;
    ScriptingString* str = CreateScriptingString("");
    __asm __volatile__("var heapPos:int = heap.position;");
    __asm __volatile__("heap.position = %0;"::"r"(wwwflash->m_Buffer));
    __asm __volatile__("returnString = heap.readUTFBytes(%0); "::"r"(wwwflash->m_SizeLoaded));
    __asm __volatile__("heap.position = heapPos;");
    return str;
}

#endif
#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropBytes(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropBytes)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bytes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bytes)
    WWW& www = *GET;
    if (www.GetType() == kWWWTypeCached)
    {
    ErrorString(kWWWCachedAccessError);
    return SCRIPTING_NULL;
    }
    if (www.GetSecurityPolicy() != WWW::kSecurityPolicyAllowAccess)
    RaiseSecurityException("No valid crossdomain policy available to allow access");
    if (!www.HasDownloadedOrMayBlock ())
    return CreateEmptyStructArray(GetScriptingManager().GetCommonClasses().byte);
    return CreateScriptingArray<UInt8>( www.GetData(), www.GetSize(), GetScriptingManager().GetCommonClasses().byte );
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropSize(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropSize)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_size)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_size)
    WWW& www = *GET;
    if (!www.HasDownloadedOrMayBlock ())
    return 0;
    return www.GetSize();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropError(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropError)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_error)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_error)
    WWW *www = GET;
    if (www)
    {
    const char* e = self->GetError();
    if (e)
    return CreateScriptingString(e);
    else
    return SCRIPTING_NULL;
    }
    else
    return CreateScriptingString(kWWWErrCancelled);
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION WWW_CUSTOM_GetTexture(ICallType_Object_Argument self_, ScriptingBool markNonReadable)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_GetTexture)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTexture)
    
    		WWW& www = *GET;
    		if (www.GetType() == kWWWTypeCached)
    		{
    			ErrorString(kWWWCachedAccessError);
    			return SCRIPTING_NULL;
    		}
    
    		Texture2D* tex = CreateObjectFromCode<Texture2D>();
    
    		if (www.HasDownloadedOrMayBlock ())
    		{
    			LoadMemoryBufferIntoTexture( *tex, self->GetData(), self->GetSize(), kLoadImageUncompressed, markNonReadable );
    			WWW::SecurityPolicy policy = self->GetSecurityPolicy();
    			if (policy != WWW::kSecurityPolicyAllowAccess)
    				tex->SetReadAllowed(false);
    		}
    		else
    		{
    			LoadMemoryBufferIntoTexture( *tex, NULL, 0, kLoadImageUncompressed, markNonReadable );
    		}
    
    		return Scripting::ScriptingWrapperFor(tex);
    	
}

#endif
#if (ENABLE_WWW) && (!PLATFORM_WEBGL && ENABLE_AUDIO)
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION WWW_CUSTOM_GetAudioClip(ICallType_Object_Argument self_, ScriptingBool threeD, ScriptingBool stream, int audioType)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_GetAudioClip)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(GetAudioClip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetAudioClip)
    
    	#if ENABLE_AUDIO
    
    		WWW& www = *GET;
    		if (www.GetType() == kWWWTypeCached)
    		{
    			ErrorString(kWWWCachedAccessError);
    			return SCRIPTING_NULL;
    		}
    
    
    		AudioClip* clip = NEW_OBJECT(AudioClip);
    
    		WWW::SecurityPolicy policy = www.GetSecurityPolicy();
    		if (policy != WWW::kSecurityPolicyAllowAccess)
    			clip->SetReadAllowed(false);
    		
    		clip->Reset();		
    		clip->Set3D(threeD);
    #if ENABLE_WWW
    #if !UNITY_FLASH
    		if (!clip->InitStream(GET, NULL, stream, audioType))
    #else
    		if (!clip->InitStream(GET, NULL, stream))
    #endif
    
    		{
    			DestroySingleObject(clip);
    			return SCRIPTING_NULL;
    		}
    #endif
    
    		return Scripting::ScriptingWrapperFor (clip);
    	#else
    		return 0;
    	#endif
    
    	
}

#endif
#if (ENABLE_WWW) && (ENABLE_MOVIES)
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropMovie(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropMovie)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_movie)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_movie)
    WWW& www = *GET;
    if (www.GetType() == kWWWTypeCached)
    {
    ErrorString(kWWWCachedAccessError);
    return SCRIPTING_NULL;
    }
    #if UNITY_EDITOR
    if (!LicenseInfo::Flag(lf_pro_version))
    {
    ErrorString("Movie playback is only possible with Unity Pro");
    return SCRIPTING_NULL;
    }
    #endif
    MovieTexture* tex = NEW_OBJECT(MovieTexture);
    tex->Reset();
    tex->InitStream(GET);
    return Scripting::ScriptingWrapperFor (tex);
}

#endif
#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WWW_CUSTOM_LoadImageIntoTexture(ICallType_Object_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument tex_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_LoadImageIntoTexture)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Texture2D> tex(tex_);
    UNUSED(tex);
    SCRIPTINGAPI_STACK_CHECK(LoadImageIntoTexture)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadImageIntoTexture)
    
    		WWW& www = *GET;
    		if (www.GetType() == kWWWTypeCached)
    		{
    			ErrorString(kWWWCachedAccessError);
    			return;
    		}
    
    		if (!www.HasDownloadedOrMayBlock ())
    			return;
    
    		LoadMemoryBufferIntoTexture( *tex, www.GetData(), www.GetSize(), IsCompressedDXTTextureFormat(tex->GetTextureFormat())?kLoadImageDXTCompressDithered:kLoadImageUncompressed );
    		if (www.GetSecurityPolicy() != WWW::kSecurityPolicyAllowAccess) tex->SetReadAllowed(false);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropIsDone(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropIsDone)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isDone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isDone)
    return (short)GET->IsDone();
}

#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION WWW_CUSTOM_GetURL(ICallType_String_Argument url_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_GetURL)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(GetURL)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetURL)
    
    		map<string,string> headers;
    		const char* c_string = url.ToUTF8().c_str();
    		WWW* fetcher = WWW::Create(c_string, NULL, -1, headers);
    
    		MonoString* result;
    
    		if (fetcher->GetSecurityPolicy() != WWW::kSecurityPolicyAllowAccess)
    		{
    			RaiseSecurityException("No valid crossdomain policy available to allow access");
    			return CreateScriptingString("");
    		}
    
    		if (!fetcher->HasDownloadedOrMayBlock ())
    			result = CreateScriptingString("");
    		else
    			result = CreateScriptingStringLength((char*)fetcher->GetData(), fetcher->GetSize());
    
    		fetcher->Release();
    		return result;
    	
}

#endif
#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropProgress(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropProgress)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_progress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_progress)
    return GET->GetProgress();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropUploadProgress(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropUploadProgress)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_uploadProgress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_uploadProgress)
    return GET->GetUploadProgress();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WWW_CUSTOM_LoadUnityWeb(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_LoadUnityWeb)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(LoadUnityWeb)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadUnityWeb)
    
    		WWW& www = *GET;
    		if (!www.HasDownloadedOrMayBlock ())
    			return;
    
    		if (www.GetSecurityPolicy() != WWW::kSecurityPolicyAllowAccess)
    		{
    			RaiseSecurityException("No valid crossdomain policy available to allow access");
    			return; 
    		}
    
    		#if WEBPLUG
    		QueuePlayerLoadWebData (www.GetUnityWebStream());
    		www.GetUnityWebStream()->RetainDownload (&www);
    		#else
    		LogString (Format("Requested loading unity web file %s. This will only be loaded in the web player.", www.GetUrl ()));
    		#endif
    	
}

#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT && ENABLE_AUDIO)
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropOggVorbis(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropOggVorbis)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_oggVorbis)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_oggVorbis)
    #if ENABLE_AUDIO
    if (AudioClip::IsFormatSupportedByPlatform("ogg"))
    {
    ErrorString("Streaming of 'ogg' on this platform is not supported");
    return SCRIPTING_NULL;
    }
    WWW& www = *GET;
    if (www.GetType() == kWWWTypeCached)
    {
    ErrorString(kWWWCachedAccessError);
    return SCRIPTING_NULL;
    }
    if (!www.HasDownloadedOrMayBlock ())
    {
    return Scripting::ScriptingWrapperFor<AudioClip>(NULL);
    }
    AudioClip* clip = NEW_OBJECT (AudioClip);
    clip->Reset();
    clip->SetName(GetLastPathNameComponent(www.GetUrl()).c_str());
    if (!clip->SetAudioData(www.GetData(), www.GetSize(), false, false,true, false, FMOD_SOUND_TYPE_OGGVORBIS, FMOD_SOUND_FORMAT_PCM16))
    {
    DestroySingleObject(clip);
    clip = NULL;
    }
    return Scripting::ScriptingWrapperFor(clip);
    #else
    return 0;
    #endif
}

#endif
#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropUrl(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropUrl)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_url)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_url)
    return CreateScriptingString(GET->GetUrl());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropAssetBundle(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropAssetBundle)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_assetBundle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_assetBundle)
    return Scripting::ScriptingWrapperFor (ExtractAssetBundle (*GET));
}

#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
SCRIPT_BINDINGS_EXPORT_DECL
ThreadPriority SCRIPT_CALL_CONVENTION WWW_Get_Custom_PropThreadPriority(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Get_Custom_PropThreadPriority)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_threadPriority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_threadPriority)
    return GET->GetThreadPriority();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WWW_Set_Custom_PropThreadPriority(ICallType_Object_Argument self_, ThreadPriority value)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_Set_Custom_PropThreadPriority)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_threadPriority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_threadPriority)
     GET->SetThreadPriority(value); 
}

#endif
#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION WWW_CUSTOM_WWW(ICallType_Object_Argument self_, ICallType_String_Argument url_, int version, UInt32 crc)
{
    SCRIPTINGAPI_ETW_ENTRY(WWW_CUSTOM_WWW)
    ScriptingObjectWithIntPtrField<WWW> self(self_);
    UNUSED(self);
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(.ctor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(.ctor)
    
    		string cpp_string = url;
    		#if ENABLE_CACHING
    		WWW* www = new WWWCached(cpp_string.c_str(), version, crc);
    		#else
    		WWW* www = WWW::Create (cpp_string.c_str(), NULL, 0, WWW::WWWHeaders(), 0, crc);
    		#endif
    		self.SetPtr(www);
    	
}

#endif

 #undef GET





struct MonoCacheIndex {
	ScriptingStringPtr name;
	int bytesUsed;
	int expires;
};
#if ENABLE_CACHING
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_CUSTOM_Authorize(ICallType_String_Argument name_, ICallType_String_Argument domain_, SInt64 size, int expiration, ICallType_String_Argument signature_)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_Authorize)
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_String_Local domain(domain_);
    UNUSED(domain);
    ICallType_String_Local signature(signature_);
    UNUSED(signature);
    SCRIPTINGAPI_STACK_CHECK(Authorize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Authorize)
    
    	return GetCachingManager().Authorize(name, domain, size, expiration, signature);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_CUSTOM_CleanCache()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_CleanCache)
    SCRIPTINGAPI_STACK_CHECK(CleanCache)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CleanCache)
    
    	if (GetCachingManager().GetAuthorizationLevel() >= CachingManager::kAuthorizationUser)
    		return GetCachingManager().GetCurrentCache().CleanCache();
    	else
    	{
    		ErrorString("Unauthorized use of Caching API.");
    		return false;
    	}
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_CUSTOM_CleanNamedCache(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_CleanNamedCache)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(CleanNamedCache)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CleanNamedCache)
    
    	if (GetCachingManager().GetAuthorizationLevel() >= CachingManager::kAuthorizationAdmin)
    		return GetCachingManager().CleanCache(name);
    	else
    	{
    		ErrorString("Unauthorized use of Caching API.");
    		return false;
    	}
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_CUSTOM_DeleteFromCache(ICallType_String_Argument url_)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_DeleteFromCache)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(DeleteFromCache)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DeleteFromCache)
    
    	return false;
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Caching_CUSTOM_GetVersionFromCache(ICallType_String_Argument url_)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_GetVersionFromCache)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(GetVersionFromCache)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetVersionFromCache)
    
    	return -1;
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_CUSTOM_IsVersionCached(ICallType_String_Argument url_, int version)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_IsVersionCached)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(IsVersionCached)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsVersionCached)
    
    	return GetCachingManager().IsCached(url, version);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_CUSTOM_MarkAsUsed(ICallType_String_Argument url_, int version)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_MarkAsUsed)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(MarkAsUsed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(MarkAsUsed)
    
    	return GetCachingManager().MarkAsUsed(url, version);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropIndex()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropIndex)
    SCRIPTINGAPI_STACK_CHECK(get_index)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_index)
    #if !PLATFORM_WINRT
    if (GetCachingManager().GetAuthorizationLevel() < CachingManager::kAuthorizationAdmin)
    {
    ErrorString("Unauthorized use of Caching API.");
    return NULL;
    }
    vector<Cache*> &indices = GetGlobalCachingManager().GetCacheIndices();
    MonoArray *monoIndices = mono_array_new (mono_domain_get (), GetMonoManager().GetCommonClasses().cacheIndex, indices.size() );
    for(int i=0; i<indices.size(); i++)
    {
    GetMonoArrayElement<MonoCacheIndex>(monoIndices, i).name = CreateScriptingString (indices[i]->m_Name);
    GetMonoArrayElement<MonoCacheIndex>(monoIndices, i).bytesUsed = indices[i]->m_BytesUsed;
    GetMonoArrayElement<MonoCacheIndex>(monoIndices, i).expires = indices[i]->m_Expires;
    }
    return monoIndices;
    #else
    return SCRIPTING_NULL;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropSpaceFree()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropSpaceFree)
    SCRIPTINGAPI_STACK_CHECK(get_spaceFree)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spaceFree)
    return GetCachingManager().GetCachingDiskSpaceFree();
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropMaximumAvailableDiskSpace()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropMaximumAvailableDiskSpace)
    SCRIPTINGAPI_STACK_CHECK(get_maximumAvailableDiskSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maximumAvailableDiskSpace)
    return GetCachingManager().GetMaximumDiskSpaceAvailable();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Caching_Set_Custom_PropMaximumAvailableDiskSpace(SInt64 value)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Set_Custom_PropMaximumAvailableDiskSpace)
    SCRIPTINGAPI_STACK_CHECK(set_maximumAvailableDiskSpace)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maximumAvailableDiskSpace)
    
    GetCachingManager().SetMaximumDiskSpaceAvailable(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropSpaceOccupied()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropSpaceOccupied)
    SCRIPTINGAPI_STACK_CHECK(get_spaceOccupied)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spaceOccupied)
    return GetCachingManager().GetCachingDiskSpaceUsed();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropSpaceAvailable()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropSpaceAvailable)
    SCRIPTINGAPI_STACK_CHECK(get_spaceAvailable)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spaceAvailable)
    return GetCachingManager().GetCachingDiskSpaceFree();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropSpaceUsed()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropSpaceUsed)
    SCRIPTINGAPI_STACK_CHECK(get_spaceUsed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spaceUsed)
    return GetCachingManager().GetCachingDiskSpaceUsed();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropExpirationDelay()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropExpirationDelay)
    SCRIPTINGAPI_STACK_CHECK(get_expirationDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_expirationDelay)
    return GetCachingManager().GetExpirationDelay();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Caching_Set_Custom_PropExpirationDelay(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Set_Custom_PropExpirationDelay)
    SCRIPTINGAPI_STACK_CHECK(set_expirationDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_expirationDelay)
    
    if (GetCachingManager().GetAuthorizationLevel() >= CachingManager::kAuthorizationUser)
    GetCachingManager().GetCurrentCache().SetExpirationDelay(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    return GetCachingManager().GetEnabled();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Caching_Set_Custom_PropEnabled(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Set_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    if (GetCachingManager().GetAuthorizationLevel() >= CachingManager::kAuthorizationAdmin)
    GetCachingManager().SetEnabled(value);
    else
    ErrorString("Unable to assign a value to Caching.enabled. This property is read-only.");
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Caching_Get_Custom_PropReady()
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_Get_Custom_PropReady)
    SCRIPTINGAPI_STACK_CHECK(get_ready)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_ready)
    return GetCachingManager().GetIsReady();
}

#endif
#if (ENABLE_CACHING) && (UNITY_IPHONE_API)
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Caching_CUSTOM_SetNoBackupFlag(ICallType_String_Argument url_, int version)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_SetNoBackupFlag)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(SetNoBackupFlag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetNoBackupFlag)
    
    #if PLATFORM_IPHONE
    	GetCachingManager().SetNoBackupFlag(url, version);
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Caching_CUSTOM_ResetNoBackupFlag(ICallType_String_Argument url_, int version)
{
    SCRIPTINGAPI_ETW_ENTRY(Caching_CUSTOM_ResetNoBackupFlag)
    ICallType_String_Local url(url_);
    UNUSED(url);
    SCRIPTINGAPI_STACK_CHECK(ResetNoBackupFlag)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ResetNoBackupFlag)
    
    #if PLATFORM_IPHONE
    	GetCachingManager().ResetNoBackupFlag(url, version);
    #endif
    
}

#endif
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION UnityLogWriter_CUSTOM_WriteStringToUnityLog(ICallType_String_Argument s_)
{
    SCRIPTINGAPI_ETW_ENTRY(UnityLogWriter_CUSTOM_WriteStringToUnityLog)
    ICallType_String_Local s(s_);
    UNUSED(s);
    SCRIPTINGAPI_STACK_CHECK(WriteStringToUnityLog)
    
    		if (s.IsNull())
    			return;
    
    		std::string utf8 = s.ToUTF8();
    		printf_console("%s",utf8.c_str());
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_WWW
void Register_UnityEngine_WWW_DestroyWWW()
{
    scripting_add_internal_call( "UnityEngine.WWW::DestroyWWW" , (gpointer)& WWW_CUSTOM_DestroyWWW );
}

void Register_UnityEngine_WWW_InitWWW()
{
    scripting_add_internal_call( "UnityEngine.WWW::InitWWW" , (gpointer)& WWW_CUSTOM_InitWWW );
}

void Register_UnityEngine_WWW_get_responseHeadersString()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_responseHeadersString" , (gpointer)& WWW_Get_Custom_PropResponseHeadersString );
}

#endif
#if (ENABLE_WWW) && (UNITY_FLASH)
void Register_UnityEngine_WWW_get_stringFromByteArray()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_stringFromByteArray" , (gpointer)& WWW_Get_Custom_PropStringFromByteArray );
}

#endif
#if ENABLE_WWW
void Register_UnityEngine_WWW_get_bytes()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_bytes" , (gpointer)& WWW_Get_Custom_PropBytes );
}

void Register_UnityEngine_WWW_get_size()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_size" , (gpointer)& WWW_Get_Custom_PropSize );
}

void Register_UnityEngine_WWW_get_error()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_error" , (gpointer)& WWW_Get_Custom_PropError );
}

void Register_UnityEngine_WWW_GetTexture()
{
    scripting_add_internal_call( "UnityEngine.WWW::GetTexture" , (gpointer)& WWW_CUSTOM_GetTexture );
}

#endif
#if (ENABLE_WWW) && (!PLATFORM_WEBGL && ENABLE_AUDIO)
void Register_UnityEngine_WWW_GetAudioClip()
{
    scripting_add_internal_call( "UnityEngine.WWW::GetAudioClip" , (gpointer)& WWW_CUSTOM_GetAudioClip );
}

#endif
#if (ENABLE_WWW) && (ENABLE_MOVIES)
void Register_UnityEngine_WWW_get_movie()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_movie" , (gpointer)& WWW_Get_Custom_PropMovie );
}

#endif
#if ENABLE_WWW
void Register_UnityEngine_WWW_LoadImageIntoTexture()
{
    scripting_add_internal_call( "UnityEngine.WWW::LoadImageIntoTexture" , (gpointer)& WWW_CUSTOM_LoadImageIntoTexture );
}

void Register_UnityEngine_WWW_get_isDone()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_isDone" , (gpointer)& WWW_Get_Custom_PropIsDone );
}

#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
void Register_UnityEngine_WWW_GetURL()
{
    scripting_add_internal_call( "UnityEngine.WWW::GetURL" , (gpointer)& WWW_CUSTOM_GetURL );
}

#endif
#if ENABLE_WWW
void Register_UnityEngine_WWW_get_progress()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_progress" , (gpointer)& WWW_Get_Custom_PropProgress );
}

void Register_UnityEngine_WWW_get_uploadProgress()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_uploadProgress" , (gpointer)& WWW_Get_Custom_PropUploadProgress );
}

void Register_UnityEngine_WWW_LoadUnityWeb()
{
    scripting_add_internal_call( "UnityEngine.WWW::LoadUnityWeb" , (gpointer)& WWW_CUSTOM_LoadUnityWeb );
}

#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT && ENABLE_AUDIO)
void Register_UnityEngine_WWW_get_oggVorbis()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_oggVorbis" , (gpointer)& WWW_Get_Custom_PropOggVorbis );
}

#endif
#if ENABLE_WWW
void Register_UnityEngine_WWW_get_url()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_url" , (gpointer)& WWW_Get_Custom_PropUrl );
}

void Register_UnityEngine_WWW_get_assetBundle()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_assetBundle" , (gpointer)& WWW_Get_Custom_PropAssetBundle );
}

#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
void Register_UnityEngine_WWW_get_threadPriority()
{
    scripting_add_internal_call( "UnityEngine.WWW::get_threadPriority" , (gpointer)& WWW_Get_Custom_PropThreadPriority );
}

void Register_UnityEngine_WWW_set_threadPriority()
{
    scripting_add_internal_call( "UnityEngine.WWW::set_threadPriority" , (gpointer)& WWW_Set_Custom_PropThreadPriority );
}

#endif
#if ENABLE_WWW
void Register_UnityEngine_WWW__ctor()
{
    scripting_add_internal_call( "UnityEngine.WWW::.ctor" , (gpointer)& WWW_CUSTOM_WWW );
}

#endif
#if ENABLE_CACHING
void Register_UnityEngine_Caching_Authorize()
{
    scripting_add_internal_call( "UnityEngine.Caching::Authorize" , (gpointer)& Caching_CUSTOM_Authorize );
}

void Register_UnityEngine_Caching_CleanCache()
{
    scripting_add_internal_call( "UnityEngine.Caching::CleanCache" , (gpointer)& Caching_CUSTOM_CleanCache );
}

void Register_UnityEngine_Caching_CleanNamedCache()
{
    scripting_add_internal_call( "UnityEngine.Caching::CleanNamedCache" , (gpointer)& Caching_CUSTOM_CleanNamedCache );
}

void Register_UnityEngine_Caching_DeleteFromCache()
{
    scripting_add_internal_call( "UnityEngine.Caching::DeleteFromCache" , (gpointer)& Caching_CUSTOM_DeleteFromCache );
}

void Register_UnityEngine_Caching_GetVersionFromCache()
{
    scripting_add_internal_call( "UnityEngine.Caching::GetVersionFromCache" , (gpointer)& Caching_CUSTOM_GetVersionFromCache );
}

void Register_UnityEngine_Caching_IsVersionCached()
{
    scripting_add_internal_call( "UnityEngine.Caching::IsVersionCached" , (gpointer)& Caching_CUSTOM_IsVersionCached );
}

void Register_UnityEngine_Caching_MarkAsUsed()
{
    scripting_add_internal_call( "UnityEngine.Caching::MarkAsUsed" , (gpointer)& Caching_CUSTOM_MarkAsUsed );
}

void Register_UnityEngine_Caching_get_index()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_index" , (gpointer)& Caching_Get_Custom_PropIndex );
}

void Register_UnityEngine_Caching_get_spaceFree()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_spaceFree" , (gpointer)& Caching_Get_Custom_PropSpaceFree );
}

void Register_UnityEngine_Caching_get_maximumAvailableDiskSpace()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_maximumAvailableDiskSpace" , (gpointer)& Caching_Get_Custom_PropMaximumAvailableDiskSpace );
}

void Register_UnityEngine_Caching_set_maximumAvailableDiskSpace()
{
    scripting_add_internal_call( "UnityEngine.Caching::set_maximumAvailableDiskSpace" , (gpointer)& Caching_Set_Custom_PropMaximumAvailableDiskSpace );
}

void Register_UnityEngine_Caching_get_spaceOccupied()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_spaceOccupied" , (gpointer)& Caching_Get_Custom_PropSpaceOccupied );
}

void Register_UnityEngine_Caching_get_spaceAvailable()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_spaceAvailable" , (gpointer)& Caching_Get_Custom_PropSpaceAvailable );
}

void Register_UnityEngine_Caching_get_spaceUsed()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_spaceUsed" , (gpointer)& Caching_Get_Custom_PropSpaceUsed );
}

void Register_UnityEngine_Caching_get_expirationDelay()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_expirationDelay" , (gpointer)& Caching_Get_Custom_PropExpirationDelay );
}

void Register_UnityEngine_Caching_set_expirationDelay()
{
    scripting_add_internal_call( "UnityEngine.Caching::set_expirationDelay" , (gpointer)& Caching_Set_Custom_PropExpirationDelay );
}

void Register_UnityEngine_Caching_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_enabled" , (gpointer)& Caching_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Caching_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Caching::set_enabled" , (gpointer)& Caching_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Caching_get_ready()
{
    scripting_add_internal_call( "UnityEngine.Caching::get_ready" , (gpointer)& Caching_Get_Custom_PropReady );
}

#endif
#if (ENABLE_CACHING) && (UNITY_IPHONE_API)
void Register_UnityEngine_Caching_SetNoBackupFlag()
{
    scripting_add_internal_call( "UnityEngine.Caching::SetNoBackupFlag" , (gpointer)& Caching_CUSTOM_SetNoBackupFlag );
}

void Register_UnityEngine_Caching_ResetNoBackupFlag()
{
    scripting_add_internal_call( "UnityEngine.Caching::ResetNoBackupFlag" , (gpointer)& Caching_CUSTOM_ResetNoBackupFlag );
}

#endif
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
void Register_UnityEngine_UnityLogWriter_WriteStringToUnityLog()
{
    scripting_add_internal_call( "UnityEngine.UnityLogWriter::WriteStringToUnityLog" , (gpointer)& UnityLogWriter_CUSTOM_WriteStringToUnityLog );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Utils_IcallNames [] =
{
#if ENABLE_WWW
    "UnityEngine.WWW::DestroyWWW"           ,    // -> WWW_CUSTOM_DestroyWWW
    "UnityEngine.WWW::InitWWW"              ,    // -> WWW_CUSTOM_InitWWW
    "UnityEngine.WWW::get_responseHeadersString",    // -> WWW_Get_Custom_PropResponseHeadersString
#endif
#if (ENABLE_WWW) && (UNITY_FLASH)
    "UnityEngine.WWW::get_stringFromByteArray",    // -> WWW_Get_Custom_PropStringFromByteArray
#endif
#if ENABLE_WWW
    "UnityEngine.WWW::get_bytes"            ,    // -> WWW_Get_Custom_PropBytes
    "UnityEngine.WWW::get_size"             ,    // -> WWW_Get_Custom_PropSize
    "UnityEngine.WWW::get_error"            ,    // -> WWW_Get_Custom_PropError
    "UnityEngine.WWW::GetTexture"           ,    // -> WWW_CUSTOM_GetTexture
#endif
#if (ENABLE_WWW) && (!PLATFORM_WEBGL && ENABLE_AUDIO)
    "UnityEngine.WWW::GetAudioClip"         ,    // -> WWW_CUSTOM_GetAudioClip
#endif
#if (ENABLE_WWW) && (ENABLE_MOVIES)
    "UnityEngine.WWW::get_movie"            ,    // -> WWW_Get_Custom_PropMovie
#endif
#if ENABLE_WWW
    "UnityEngine.WWW::LoadImageIntoTexture" ,    // -> WWW_CUSTOM_LoadImageIntoTexture
    "UnityEngine.WWW::get_isDone"           ,    // -> WWW_Get_Custom_PropIsDone
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
    "UnityEngine.WWW::GetURL"               ,    // -> WWW_CUSTOM_GetURL
#endif
#if ENABLE_WWW
    "UnityEngine.WWW::get_progress"         ,    // -> WWW_Get_Custom_PropProgress
    "UnityEngine.WWW::get_uploadProgress"   ,    // -> WWW_Get_Custom_PropUploadProgress
    "UnityEngine.WWW::LoadUnityWeb"         ,    // -> WWW_CUSTOM_LoadUnityWeb
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT && ENABLE_AUDIO)
    "UnityEngine.WWW::get_oggVorbis"        ,    // -> WWW_Get_Custom_PropOggVorbis
#endif
#if ENABLE_WWW
    "UnityEngine.WWW::get_url"              ,    // -> WWW_Get_Custom_PropUrl
    "UnityEngine.WWW::get_assetBundle"      ,    // -> WWW_Get_Custom_PropAssetBundle
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
    "UnityEngine.WWW::get_threadPriority"   ,    // -> WWW_Get_Custom_PropThreadPriority
    "UnityEngine.WWW::set_threadPriority"   ,    // -> WWW_Set_Custom_PropThreadPriority
#endif
#if ENABLE_WWW
    "UnityEngine.WWW::.ctor"                ,    // -> WWW_CUSTOM_WWW
#endif
#if ENABLE_CACHING
    "UnityEngine.Caching::Authorize"        ,    // -> Caching_CUSTOM_Authorize
    "UnityEngine.Caching::CleanCache"       ,    // -> Caching_CUSTOM_CleanCache
    "UnityEngine.Caching::CleanNamedCache"  ,    // -> Caching_CUSTOM_CleanNamedCache
    "UnityEngine.Caching::DeleteFromCache"  ,    // -> Caching_CUSTOM_DeleteFromCache
    "UnityEngine.Caching::GetVersionFromCache",    // -> Caching_CUSTOM_GetVersionFromCache
    "UnityEngine.Caching::IsVersionCached"  ,    // -> Caching_CUSTOM_IsVersionCached
    "UnityEngine.Caching::MarkAsUsed"       ,    // -> Caching_CUSTOM_MarkAsUsed
    "UnityEngine.Caching::get_index"        ,    // -> Caching_Get_Custom_PropIndex
    "UnityEngine.Caching::get_spaceFree"    ,    // -> Caching_Get_Custom_PropSpaceFree
    "UnityEngine.Caching::get_maximumAvailableDiskSpace",    // -> Caching_Get_Custom_PropMaximumAvailableDiskSpace
    "UnityEngine.Caching::set_maximumAvailableDiskSpace",    // -> Caching_Set_Custom_PropMaximumAvailableDiskSpace
    "UnityEngine.Caching::get_spaceOccupied",    // -> Caching_Get_Custom_PropSpaceOccupied
    "UnityEngine.Caching::get_spaceAvailable",    // -> Caching_Get_Custom_PropSpaceAvailable
    "UnityEngine.Caching::get_spaceUsed"    ,    // -> Caching_Get_Custom_PropSpaceUsed
    "UnityEngine.Caching::get_expirationDelay",    // -> Caching_Get_Custom_PropExpirationDelay
    "UnityEngine.Caching::set_expirationDelay",    // -> Caching_Set_Custom_PropExpirationDelay
    "UnityEngine.Caching::get_enabled"      ,    // -> Caching_Get_Custom_PropEnabled
    "UnityEngine.Caching::set_enabled"      ,    // -> Caching_Set_Custom_PropEnabled
    "UnityEngine.Caching::get_ready"        ,    // -> Caching_Get_Custom_PropReady
#endif
#if (ENABLE_CACHING) && (UNITY_IPHONE_API)
    "UnityEngine.Caching::SetNoBackupFlag"  ,    // -> Caching_CUSTOM_SetNoBackupFlag
    "UnityEngine.Caching::ResetNoBackupFlag",    // -> Caching_CUSTOM_ResetNoBackupFlag
#endif
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
    "UnityEngine.UnityLogWriter::WriteStringToUnityLog",    // -> UnityLogWriter_CUSTOM_WriteStringToUnityLog
#endif
    NULL
};

static const void* s_Utils_IcallFuncs [] =
{
#if ENABLE_WWW
    (const void*)&WWW_CUSTOM_DestroyWWW                   ,  //  <- UnityEngine.WWW::DestroyWWW
    (const void*)&WWW_CUSTOM_InitWWW                      ,  //  <- UnityEngine.WWW::InitWWW
    (const void*)&WWW_Get_Custom_PropResponseHeadersString,  //  <- UnityEngine.WWW::get_responseHeadersString
#endif
#if (ENABLE_WWW) && (UNITY_FLASH)
    (const void*)&WWW_Get_Custom_PropStringFromByteArray  ,  //  <- UnityEngine.WWW::get_stringFromByteArray
#endif
#if ENABLE_WWW
    (const void*)&WWW_Get_Custom_PropBytes                ,  //  <- UnityEngine.WWW::get_bytes
    (const void*)&WWW_Get_Custom_PropSize                 ,  //  <- UnityEngine.WWW::get_size
    (const void*)&WWW_Get_Custom_PropError                ,  //  <- UnityEngine.WWW::get_error
    (const void*)&WWW_CUSTOM_GetTexture                   ,  //  <- UnityEngine.WWW::GetTexture
#endif
#if (ENABLE_WWW) && (!PLATFORM_WEBGL && ENABLE_AUDIO)
    (const void*)&WWW_CUSTOM_GetAudioClip                 ,  //  <- UnityEngine.WWW::GetAudioClip
#endif
#if (ENABLE_WWW) && (ENABLE_MOVIES)
    (const void*)&WWW_Get_Custom_PropMovie                ,  //  <- UnityEngine.WWW::get_movie
#endif
#if ENABLE_WWW
    (const void*)&WWW_CUSTOM_LoadImageIntoTexture         ,  //  <- UnityEngine.WWW::LoadImageIntoTexture
    (const void*)&WWW_Get_Custom_PropIsDone               ,  //  <- UnityEngine.WWW::get_isDone
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
    (const void*)&WWW_CUSTOM_GetURL                       ,  //  <- UnityEngine.WWW::GetURL
#endif
#if ENABLE_WWW
    (const void*)&WWW_Get_Custom_PropProgress             ,  //  <- UnityEngine.WWW::get_progress
    (const void*)&WWW_Get_Custom_PropUploadProgress       ,  //  <- UnityEngine.WWW::get_uploadProgress
    (const void*)&WWW_CUSTOM_LoadUnityWeb                 ,  //  <- UnityEngine.WWW::LoadUnityWeb
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT && ENABLE_AUDIO)
    (const void*)&WWW_Get_Custom_PropOggVorbis            ,  //  <- UnityEngine.WWW::get_oggVorbis
#endif
#if ENABLE_WWW
    (const void*)&WWW_Get_Custom_PropUrl                  ,  //  <- UnityEngine.WWW::get_url
    (const void*)&WWW_Get_Custom_PropAssetBundle          ,  //  <- UnityEngine.WWW::get_assetBundle
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
    (const void*)&WWW_Get_Custom_PropThreadPriority       ,  //  <- UnityEngine.WWW::get_threadPriority
    (const void*)&WWW_Set_Custom_PropThreadPriority       ,  //  <- UnityEngine.WWW::set_threadPriority
#endif
#if ENABLE_WWW
    (const void*)&WWW_CUSTOM_WWW                          ,  //  <- UnityEngine.WWW::.ctor
#endif
#if ENABLE_CACHING
    (const void*)&Caching_CUSTOM_Authorize                ,  //  <- UnityEngine.Caching::Authorize
    (const void*)&Caching_CUSTOM_CleanCache               ,  //  <- UnityEngine.Caching::CleanCache
    (const void*)&Caching_CUSTOM_CleanNamedCache          ,  //  <- UnityEngine.Caching::CleanNamedCache
    (const void*)&Caching_CUSTOM_DeleteFromCache          ,  //  <- UnityEngine.Caching::DeleteFromCache
    (const void*)&Caching_CUSTOM_GetVersionFromCache      ,  //  <- UnityEngine.Caching::GetVersionFromCache
    (const void*)&Caching_CUSTOM_IsVersionCached          ,  //  <- UnityEngine.Caching::IsVersionCached
    (const void*)&Caching_CUSTOM_MarkAsUsed               ,  //  <- UnityEngine.Caching::MarkAsUsed
    (const void*)&Caching_Get_Custom_PropIndex            ,  //  <- UnityEngine.Caching::get_index
    (const void*)&Caching_Get_Custom_PropSpaceFree        ,  //  <- UnityEngine.Caching::get_spaceFree
    (const void*)&Caching_Get_Custom_PropMaximumAvailableDiskSpace,  //  <- UnityEngine.Caching::get_maximumAvailableDiskSpace
    (const void*)&Caching_Set_Custom_PropMaximumAvailableDiskSpace,  //  <- UnityEngine.Caching::set_maximumAvailableDiskSpace
    (const void*)&Caching_Get_Custom_PropSpaceOccupied    ,  //  <- UnityEngine.Caching::get_spaceOccupied
    (const void*)&Caching_Get_Custom_PropSpaceAvailable   ,  //  <- UnityEngine.Caching::get_spaceAvailable
    (const void*)&Caching_Get_Custom_PropSpaceUsed        ,  //  <- UnityEngine.Caching::get_spaceUsed
    (const void*)&Caching_Get_Custom_PropExpirationDelay  ,  //  <- UnityEngine.Caching::get_expirationDelay
    (const void*)&Caching_Set_Custom_PropExpirationDelay  ,  //  <- UnityEngine.Caching::set_expirationDelay
    (const void*)&Caching_Get_Custom_PropEnabled          ,  //  <- UnityEngine.Caching::get_enabled
    (const void*)&Caching_Set_Custom_PropEnabled          ,  //  <- UnityEngine.Caching::set_enabled
    (const void*)&Caching_Get_Custom_PropReady            ,  //  <- UnityEngine.Caching::get_ready
#endif
#if (ENABLE_CACHING) && (UNITY_IPHONE_API)
    (const void*)&Caching_CUSTOM_SetNoBackupFlag          ,  //  <- UnityEngine.Caching::SetNoBackupFlag
    (const void*)&Caching_CUSTOM_ResetNoBackupFlag        ,  //  <- UnityEngine.Caching::ResetNoBackupFlag
#endif
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
    (const void*)&UnityLogWriter_CUSTOM_WriteStringToUnityLog,  //  <- UnityEngine.UnityLogWriter::WriteStringToUnityLog
#endif
    NULL
};

void ExportUtilsBindings();
void ExportUtilsBindings()
{
    for (int i = 0; s_Utils_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Utils_IcallNames [i], s_Utils_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUtilsBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_WWW
    SET_METRO_BINDING(WWW_CUSTOM_DestroyWWW); //  <- UnityEngine.WWW::DestroyWWW
    SET_METRO_BINDING(WWW_CUSTOM_InitWWW); //  <- UnityEngine.WWW::InitWWW
    SET_METRO_BINDING(WWW_Get_Custom_PropResponseHeadersString); //  <- UnityEngine.WWW::get_responseHeadersString
#endif
#if (ENABLE_WWW) && (UNITY_FLASH)
    SET_METRO_BINDING(WWW_Get_Custom_PropStringFromByteArray); //  <- UnityEngine.WWW::get_stringFromByteArray
#endif
#if ENABLE_WWW
    SET_METRO_BINDING(WWW_Get_Custom_PropBytes); //  <- UnityEngine.WWW::get_bytes
    SET_METRO_BINDING(WWW_Get_Custom_PropSize); //  <- UnityEngine.WWW::get_size
    SET_METRO_BINDING(WWW_Get_Custom_PropError); //  <- UnityEngine.WWW::get_error
    SET_METRO_BINDING(WWW_CUSTOM_GetTexture); //  <- UnityEngine.WWW::GetTexture
#endif
#if (ENABLE_WWW) && (!PLATFORM_WEBGL && ENABLE_AUDIO)
    SET_METRO_BINDING(WWW_CUSTOM_GetAudioClip); //  <- UnityEngine.WWW::GetAudioClip
#endif
#if (ENABLE_WWW) && (ENABLE_MOVIES)
    SET_METRO_BINDING(WWW_Get_Custom_PropMovie); //  <- UnityEngine.WWW::get_movie
#endif
#if ENABLE_WWW
    SET_METRO_BINDING(WWW_CUSTOM_LoadImageIntoTexture); //  <- UnityEngine.WWW::LoadImageIntoTexture
    SET_METRO_BINDING(WWW_Get_Custom_PropIsDone); //  <- UnityEngine.WWW::get_isDone
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
    SET_METRO_BINDING(WWW_CUSTOM_GetURL); //  <- UnityEngine.WWW::GetURL
#endif
#if ENABLE_WWW
    SET_METRO_BINDING(WWW_Get_Custom_PropProgress); //  <- UnityEngine.WWW::get_progress
    SET_METRO_BINDING(WWW_Get_Custom_PropUploadProgress); //  <- UnityEngine.WWW::get_uploadProgress
    SET_METRO_BINDING(WWW_CUSTOM_LoadUnityWeb); //  <- UnityEngine.WWW::LoadUnityWeb
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT && ENABLE_AUDIO)
    SET_METRO_BINDING(WWW_Get_Custom_PropOggVorbis); //  <- UnityEngine.WWW::get_oggVorbis
#endif
#if ENABLE_WWW
    SET_METRO_BINDING(WWW_Get_Custom_PropUrl); //  <- UnityEngine.WWW::get_url
    SET_METRO_BINDING(WWW_Get_Custom_PropAssetBundle); //  <- UnityEngine.WWW::get_assetBundle
#endif
#if (ENABLE_WWW) && (!UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT)
    SET_METRO_BINDING(WWW_Get_Custom_PropThreadPriority); //  <- UnityEngine.WWW::get_threadPriority
    SET_METRO_BINDING(WWW_Set_Custom_PropThreadPriority); //  <- UnityEngine.WWW::set_threadPriority
#endif
#if ENABLE_WWW
    SET_METRO_BINDING(WWW_CUSTOM_WWW); //  <- UnityEngine.WWW::.ctor
#endif
#if ENABLE_CACHING
    SET_METRO_BINDING(Caching_CUSTOM_Authorize); //  <- UnityEngine.Caching::Authorize
    SET_METRO_BINDING(Caching_CUSTOM_CleanCache); //  <- UnityEngine.Caching::CleanCache
    SET_METRO_BINDING(Caching_CUSTOM_CleanNamedCache); //  <- UnityEngine.Caching::CleanNamedCache
    SET_METRO_BINDING(Caching_CUSTOM_DeleteFromCache); //  <- UnityEngine.Caching::DeleteFromCache
    SET_METRO_BINDING(Caching_CUSTOM_GetVersionFromCache); //  <- UnityEngine.Caching::GetVersionFromCache
    SET_METRO_BINDING(Caching_CUSTOM_IsVersionCached); //  <- UnityEngine.Caching::IsVersionCached
    SET_METRO_BINDING(Caching_CUSTOM_MarkAsUsed); //  <- UnityEngine.Caching::MarkAsUsed
    SET_METRO_BINDING(Caching_Get_Custom_PropIndex); //  <- UnityEngine.Caching::get_index
    SET_METRO_BINDING(Caching_Get_Custom_PropSpaceFree); //  <- UnityEngine.Caching::get_spaceFree
    SET_METRO_BINDING(Caching_Get_Custom_PropMaximumAvailableDiskSpace); //  <- UnityEngine.Caching::get_maximumAvailableDiskSpace
    SET_METRO_BINDING(Caching_Set_Custom_PropMaximumAvailableDiskSpace); //  <- UnityEngine.Caching::set_maximumAvailableDiskSpace
    SET_METRO_BINDING(Caching_Get_Custom_PropSpaceOccupied); //  <- UnityEngine.Caching::get_spaceOccupied
    SET_METRO_BINDING(Caching_Get_Custom_PropSpaceAvailable); //  <- UnityEngine.Caching::get_spaceAvailable
    SET_METRO_BINDING(Caching_Get_Custom_PropSpaceUsed); //  <- UnityEngine.Caching::get_spaceUsed
    SET_METRO_BINDING(Caching_Get_Custom_PropExpirationDelay); //  <- UnityEngine.Caching::get_expirationDelay
    SET_METRO_BINDING(Caching_Set_Custom_PropExpirationDelay); //  <- UnityEngine.Caching::set_expirationDelay
    SET_METRO_BINDING(Caching_Get_Custom_PropEnabled); //  <- UnityEngine.Caching::get_enabled
    SET_METRO_BINDING(Caching_Set_Custom_PropEnabled); //  <- UnityEngine.Caching::set_enabled
    SET_METRO_BINDING(Caching_Get_Custom_PropReady); //  <- UnityEngine.Caching::get_ready
#endif
#if (ENABLE_CACHING) && (UNITY_IPHONE_API)
    SET_METRO_BINDING(Caching_CUSTOM_SetNoBackupFlag); //  <- UnityEngine.Caching::SetNoBackupFlag
    SET_METRO_BINDING(Caching_CUSTOM_ResetNoBackupFlag); //  <- UnityEngine.Caching::ResetNoBackupFlag
#endif
#if !UNITY_FLASH && !PLATFORM_WEBGL && !PLATFORM_WINRT
    SET_METRO_BINDING(UnityLogWriter_CUSTOM_WriteStringToUnityLog); //  <- UnityEngine.UnityLogWriter::WriteStringToUnityLog
#endif
}

#endif
