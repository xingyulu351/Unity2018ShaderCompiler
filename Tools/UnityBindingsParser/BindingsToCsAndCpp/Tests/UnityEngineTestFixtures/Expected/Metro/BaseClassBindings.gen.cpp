#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Mono/MonoManager.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Input/InputManager.h"
#include "Runtime/Input/TimeManager.h"
#include "Runtime/Misc/ResourceManager.h"
#include "Runtime/Profiler/ProfilerHistory.h"
#include "Runtime/Profiler/ProfilerPrivate.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/BaseClasses/Tags.h"
#include "Runtime/Misc/DebugUtility.h"
#include "Runtime/Misc/PlayerSettings.h"
#include "Runtime/GameCode/CloneObject.h"
#include "Runtime/Math/Random/Random.h"
#include "Runtime/Misc/PreloadManager.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "Runtime/Audio/AudioClip.h"
#if ENABLE_AUDIO
#include "Runtime/Audio/AudioSource.h"
#include "Runtime/Audio/AudioListener.h"
#include "Runtime/Audio/AudioManager.h"
#include "Runtime/Audio/AudioReverbZone.h"
#include "Runtime/Audio/AudioReverbFilter.h"
#include "Runtime/Audio/AudioHighPassFilter.h"
#include "Runtime/Audio/AudioLowPassFilter.h"
#include "Runtime/Audio/AudioChorusFilter.h"
#include "Runtime/Audio/AudioDistortionFilter.h"
#include "Runtime/Audio/AudioEchoFilter.h"
#endif
#include "Runtime/Animation/Animation.h"
#include "Runtime/Math/Color.h"
#include "Runtime/Utilities/PlayerPrefs.h"
#include "Runtime/Camera/Camera.h"
#include "Runtime/Dynamics/Rigidbody.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Camera/Light.h"
#include "Runtime/Filters/Misc/TextMesh.h"
#include "Runtime/Dynamics/ConstantForce.h"
#include "Runtime/Filters/Renderer.h"
#include "Runtime/Misc/SaveAndLoadHelper.h"
#include "Runtime/Network/NetworkView.h"
#include "Runtime/Network/NetworkManager.h"
#include "Runtime/Camera/RenderLayers/GUIText.h"
#include "Runtime/Camera/RenderLayers/GUITexture.h"
#include "Runtime/Dynamics/Collider.h"
#include "Runtime/Dynamics/HingeJoint.h"
#include "Runtime/Misc/AssetBundleUtility.h"
#include "Runtime/Misc/Player.h"
#include "Runtime/BaseClasses/IsPlaying.h"
#include "Runtime/Misc/CaptureScreenshot.h"
#include "Runtime/Misc/GameObjectUtility.h"
#include "Runtime/Misc/Plugins.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/File.h"
#include <ctime>
#include "Runtime/Input/GetInput.h"
#include "Runtime/NavMesh/NavMeshAgent.h"
#include "Runtime/NavMesh/NavMesh.h"
#include "Runtime/NavMesh/OffMeshLink.h"
#include "Runtime/Misc/BuildSettings.h"
#include "Runtime/Animation/AnimationManager.h"
#include "Runtime/Animation/AnimationClip.h"
#include "Runtime/BaseClasses/RefCounted.h"
#include "Runtime/Misc/GOCreation.h"
#include "Runtime/Utilities/URLUtility.h"
#include "Runtime/Graphics/ScreenManager.h"
#include "Runtime/Serialize/PersistentManager.h"
#include "Runtime/Shaders/GraphicsCaps.h"
#include "Runtime/Misc/SystemInfo.h"
#include "Runtime/Utilities/FileUtilities.h"
#include "Runtime/Misc/GraphicsDevicesDB.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include "Runtime/Profiler/CollectProfilerStats.h"
#include "Runtime/File/ApplicationSpecificPersistentDataPath.h"
#include "Runtime/Mono/Coroutine.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
#include "Runtime/Scripting/GetComponent.h"
#include "Runtime/ScriptingBackend/ScriptingApi.h"
#include "Runtime/ScriptingBackend/ScriptingTypeRegistry.h"

#if SUPPORT_REPRODUCE_LOG
#include "Runtime/Misc/ReproductionLog.h"
#endif

#if WEBPLUG
	#include "PlatformDependent/CommonWebPlugin/UnityWebStream.h"
	#include "PlatformDependent/CommonWebPlugin/WebScripting.h"
#endif

#if UNITY_EDITOR
	#include "Editor/Src/EditorSettings.h"
	#include "Editor/Src/EditorUserBuildSettings.h"
	#include "Editor/Mono/MonoEditorUtility.h"
#endif

using namespace Unity;

 


void PauseEditor ();
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION LayerMask_CUSTOM_LayerToName(int layer)
{
    SCRIPTINGAPI_ETW_ENTRY(LayerMask_CUSTOM_LayerToName)
    SCRIPTINGAPI_STACK_CHECK(LayerToName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LayerToName)
     return CreateScriptingString(LayerToString(layer)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION LayerMask_CUSTOM_NameToLayer(ICallType_String_Argument layerName_)
{
    SCRIPTINGAPI_ETW_ENTRY(LayerMask_CUSTOM_NameToLayer)
    ICallType_String_Local layerName(layerName_);
    UNUSED(layerName);
    SCRIPTINGAPI_STACK_CHECK(NameToLayer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(NameToLayer)
     return StringToLayer(layerName); 
}

#if ENABLE_AUDIO_FMOD
SCRIPT_BINDINGS_EXPORT_DECL
FMOD_SPEAKERMODE SCRIPT_CALL_CONVENTION AudioSettings_Get_Custom_PropDriverCaps()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_Get_Custom_PropDriverCaps)
    SCRIPTINGAPI_STACK_CHECK(get_driverCaps)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_driverCaps)
    return GetAudioManager().GetSpeakerModeCaps();
}

SCRIPT_BINDINGS_EXPORT_DECL
FMOD_SPEAKERMODE SCRIPT_CALL_CONVENTION AudioSettings_Get_Custom_PropSpeakerMode()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_Get_Custom_PropSpeakerMode)
    SCRIPTINGAPI_STACK_CHECK(get_speakerMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_speakerMode)
    return GetAudioManager().GetSpeakerMode();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSettings_Set_Custom_PropSpeakerMode(FMOD_SPEAKERMODE value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_Set_Custom_PropSpeakerMode)
    SCRIPTINGAPI_STACK_CHECK(set_speakerMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_speakerMode)
    
    GetAudioManager().SetSpeakerMode(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION AudioSettings_Get_Custom_PropDspTime()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_Get_Custom_PropDspTime)
    SCRIPTINGAPI_STACK_CHECK(get_dspTime)
    return GetAudioManager().GetDSPTime();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioSettings_Get_Custom_PropOutputSampleRate()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_Get_Custom_PropOutputSampleRate)
    SCRIPTINGAPI_STACK_CHECK(get_outputSampleRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_outputSampleRate)
    int sampleRate;
    GetAudioManager().GetFMODSystem()->getSoftwareFormat(
    &sampleRate,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
    );
    return sampleRate;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSettings_Set_Custom_PropOutputSampleRate(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_Set_Custom_PropOutputSampleRate)
    SCRIPTINGAPI_STACK_CHECK(set_outputSampleRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_outputSampleRate)
    
    int currentSampleRate = AudioSettings_Get_Custom_PropOutputSampleRate();
    if (currentSampleRate != value)
    {
    GetAudioManager().CloseFMOD();
    FMOD_RESULT result = GetAudioManager().GetFMODSystem()->setSoftwareFormat(
    value,
    FMOD_SOUND_FORMAT_PCM16,
    0,
    8,
    FMOD_DSP_RESAMPLER_LINEAR
    );
    if (result != FMOD_OK)
    {
    ErrorString(Format("%dHz is an invalid output samplerate for this platform", value));
    }
    GetAudioManager().ReloadFMODSounds();
    }
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSettings_CUSTOM_SetDSPBufferSize(int bufferLength, int numBuffers)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_CUSTOM_SetDSPBufferSize)
    SCRIPTINGAPI_STACK_CHECK(SetDSPBufferSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetDSPBufferSize)
    
    		bufferLength = clamp(bufferLength, 64, 4096);
    
    		GetAudioManager().CloseFMOD();
    		FMOD_RESULT result = GetAudioManager().GetFMODSystem()->setDSPBufferSize(bufferLength, numBuffers);
    		if (result != FMOD_OK)
    		{
    			ErrorString(Format("DSP ringbuffer of %d samples (x %d) is invalid for this platform", bufferLength, numBuffers));
    		}
    		GetAudioManager().ReloadFMODSounds();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSettings_CUSTOM_GetDSPBufferSize(int* bufferLength, int* numBuffers)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSettings_CUSTOM_GetDSPBufferSize)
    SCRIPTINGAPI_STACK_CHECK(GetDSPBufferSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetDSPBufferSize)
    
    		FMOD_RESULT result = GetAudioManager().GetFMODSystem()->getDSPBufferSize((unsigned int*)bufferLength, numBuffers);
    		FMOD_ASSERT( result );
    	
}

#endif

 static void CheckReadAllowedAndThrow(AudioClip *clip)
	{
#if ENABLE_MONO && ENABLE_SECURITY
		if ( clip&&!clip->GetReadAllowed() )
			RaiseSecurityException("No read access to the audioclip data: %s", clip->GetName());
#endif
	}
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioClip_Get_Custom_PropLength(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_Get_Custom_PropLength)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_length)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_length)
    return self->GetLengthSec ();
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioClip_Get_Custom_PropSamples(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_Get_Custom_PropSamples)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_samples)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_samples)
    return self->GetSampleCount ();
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioClip_Get_Custom_PropChannels(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_Get_Custom_PropChannels)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_channels)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_channels)
    return self->GetChannelCount ();
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioClip_Get_Custom_PropFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_Get_Custom_PropFrequency)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_frequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_frequency)
    return self->GetFrequency ();
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioClip_Get_Custom_PropIsReadyToPlay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_Get_Custom_PropIsReadyToPlay)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isReadyToPlay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isReadyToPlay)
    return self->ReadyToPlay ();
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioClip_CUSTOM_GetData(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument data_, int offsetSamples)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_CUSTOM_GetData)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    ICallType_Array_Local data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(GetData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetData)
    
    		CheckReadAllowedAndThrow(self);
    		self->GetData(&GetScriptingArrayElement<float>(data, 0), GetScriptingArraySize (data) / self->GetChannelCount(), offsetSamples);
    	
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioClip_CUSTOM_SetData(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument data_, int offsetSamples)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_CUSTOM_SetData)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    ICallType_Array_Local data(data_);
    UNUSED(data);
    SCRIPTINGAPI_STACK_CHECK(SetData)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetData)
    
    		self->SetData(&GetScriptingArrayElement<float>(data, 0), GetScriptingArraySize (data) / self->GetChannelCount(), offsetSamples);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AudioClip_CUSTOM_Construct_Internal()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_CUSTOM_Construct_Internal)
    SCRIPTINGAPI_STACK_CHECK(Construct_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Construct_Internal)
    
    		AudioClip* clip = NEW_OBJECT(AudioClip);
    		return Scripting::ScriptingWrapperFor ( clip );	
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioClip_CUSTOM_Init_Internal(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, int lengthSamples, int channels, int frequency, ScriptingBool _3D, ScriptingBool stream)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioClip_CUSTOM_Init_Internal)
    ReadOnlyScriptingObjectOfType<AudioClip> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Init_Internal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Init_Internal)
    
    		self->CreateUserSound( name, lengthSamples, channels, frequency, _3D, stream );
    	
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioListener_Get_Custom_PropVolume()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_Get_Custom_PropVolume)
    SCRIPTINGAPI_STACK_CHECK(get_volume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_volume)
    return GetAudioManager ().GetVolume ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioListener_Set_Custom_PropVolume(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_Set_Custom_PropVolume)
    SCRIPTINGAPI_STACK_CHECK(set_volume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_volume)
     GetAudioManager ().SetVolume (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioListener_Get_Custom_PropPause()
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_Get_Custom_PropPause)
    SCRIPTINGAPI_STACK_CHECK(get_pause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pause)
    return GetAudioManager ().GetPause ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioListener_Set_Custom_PropPause(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_Set_Custom_PropPause)
    SCRIPTINGAPI_STACK_CHECK(set_pause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pause)
     GetAudioManager ().SetPause (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioListener_Get_Custom_PropVelocityUpdateMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_Get_Custom_PropVelocityUpdateMode)
    ReadOnlyScriptingObjectOfType<AudioListener> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_velocityUpdateMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_velocityUpdateMode)
    return self->GetVelocityUpdateMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioListener_Set_Custom_PropVelocityUpdateMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_Set_Custom_PropVelocityUpdateMode)
    ReadOnlyScriptingObjectOfType<AudioListener> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_velocityUpdateMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_velocityUpdateMode)
    
    self->SetVelocityUpdateMode (value);
    
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioListener_CUSTOM_GetOutputDataHelper(ICallType_Array_Argument samples_, int channel)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_CUSTOM_GetOutputDataHelper)
    ICallType_Array_Local samples(samples_);
    UNUSED(samples);
    SCRIPTINGAPI_STACK_CHECK(GetOutputDataHelper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetOutputDataHelper)
    
    		FMOD::ChannelGroup* channelGroup;
    		FMOD_RESULT result = GetAudioManager().GetFMODSystem()->getMasterChannelGroup(&channelGroup);
    
    		if (result == FMOD_OK && channelGroup)
    		{
    			channelGroup->getWaveData(GetScriptingArrayStart<float>(samples), GetScriptingArraySize(samples), channel);
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioListener_CUSTOM_GetSpectrumDataHelper(ICallType_Array_Argument samples_, int channel, FMOD_DSP_FFT_WINDOW window)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioListener_CUSTOM_GetSpectrumDataHelper)
    ICallType_Array_Local samples(samples_);
    UNUSED(samples);
    SCRIPTINGAPI_STACK_CHECK(GetSpectrumDataHelper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetSpectrumDataHelper)
    
    		FMOD::ChannelGroup* channelGroup;
    		FMOD_RESULT result = GetAudioManager().GetFMODSystem()->getMasterChannelGroup(&channelGroup);
    
    		if (result == FMOD_OK && channelGroup)
    		{
    			channelGroup->getSpectrum(GetScriptingArrayStart<float>(samples), GetScriptingArraySize(samples), channel, (FMOD_DSP_FFT_WINDOW)window);
    		}
    	
}

#endif
#if ENABLE_MICROPHONE
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Microphone_CUSTOM_Start(ICallType_String_Argument deviceName_, ScriptingBool loop, int lengthSec, int frequency)
{
    SCRIPTINGAPI_ETW_ENTRY(Microphone_CUSTOM_Start)
    ICallType_String_Local deviceName(deviceName_);
    UNUSED(deviceName);
    SCRIPTINGAPI_STACK_CHECK(Start)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Start)
    
    		return Scripting::ScriptingWrapperFor( GetAudioManager().StartRecord( GetAudioManager().GetMicrophoneDeviceIDFromName ( deviceName ) , loop, lengthSec, frequency ) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Microphone_CUSTOM_End(ICallType_String_Argument deviceName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Microphone_CUSTOM_End)
    ICallType_String_Local deviceName(deviceName_);
    UNUSED(deviceName);
    SCRIPTINGAPI_STACK_CHECK(End)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(End)
    
    		GetAudioManager().EndRecord( GetAudioManager().GetMicrophoneDeviceIDFromName ( deviceName ) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Microphone_Get_Custom_PropDevices()
{
    SCRIPTINGAPI_ETW_ENTRY(Microphone_Get_Custom_PropDevices)
    SCRIPTINGAPI_STACK_CHECK(get_devices)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_devices)
    std::vector<std::string> names;
    names = GetAudioManager().GetRecordDevices();
    ScriptingArrayPtr array = CreateScriptingArray<ScriptingStringPtr> (GetMonoManager().GetCommonClasses().string, names.size ());
    for (int i=0;i<names.size ();i++)
    SetScriptingArrayElement (array, i, CreateScriptingString ( (const char*)names[i].c_str() ));
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Microphone_CUSTOM_IsRecording(ICallType_String_Argument deviceName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Microphone_CUSTOM_IsRecording)
    ICallType_String_Local deviceName(deviceName_);
    UNUSED(deviceName);
    SCRIPTINGAPI_STACK_CHECK(IsRecording)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(IsRecording)
    
    		return GetAudioManager().IsRecording( GetAudioManager().GetMicrophoneDeviceIDFromName ( deviceName ) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Microphone_CUSTOM_GetPosition(ICallType_String_Argument deviceName_)
{
    SCRIPTINGAPI_ETW_ENTRY(Microphone_CUSTOM_GetPosition)
    ICallType_String_Local deviceName(deviceName_);
    UNUSED(deviceName);
    SCRIPTINGAPI_STACK_CHECK(GetPosition)
    
    		return GetAudioManager().GetRecordPosition( GetAudioManager().GetMicrophoneDeviceIDFromName ( deviceName ) );
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Microphone_CUSTOM_GetDeviceCaps(ICallType_String_Argument deviceName_, int* minFreq, int* maxFreq)
{
    SCRIPTINGAPI_ETW_ENTRY(Microphone_CUSTOM_GetDeviceCaps)
    ICallType_String_Local deviceName(deviceName_);
    UNUSED(deviceName);
    SCRIPTINGAPI_STACK_CHECK(GetDeviceCaps)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetDeviceCaps)
    
    		GetAudioManager().GetDeviceCaps( GetAudioManager().GetMicrophoneDeviceIDFromName ( deviceName ), minFreq, maxFreq );
    	
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_volume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_volume)
    return self->GetVolume ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_volume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_volume)
    
    self->SetVolume (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropPitch(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropPitch)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pitch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pitch)
    return self->GetPitch();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropPitch(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropPitch)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_pitch)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pitch)
    
    if(!IsFinite(value))
    {
    WarningStringObject("Attempt to set pitch to infinite value from script ignored!", self);
    return;
    }
    if(IsNAN(value))
    {
    WarningStringObject("Attempt to set pitch to NaN value from script ignored!", self);
    return;
    }
    self->SetPitch(value);
    
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropTime)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time)
    return self->GetSecPosition ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropTime)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_time)
    
    self->SetSecPosition (value);
    
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropTimeSamples(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropTimeSamples)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_timeSamples)
    return self->GetSamplePosition ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropTimeSamples(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropTimeSamples)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_timeSamples)
    
    self->SetSamplePosition (value);
    
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropClip(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropClip)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_clip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_clip)
    return Scripting::ScriptingWrapperFor(self->GetAudioClip());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropClip(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropClip)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AudioClip> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_clip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_clip)
    
    self->SetAudioClip (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_Play(ICallType_ReadOnlyUnityEngineObject_Argument self_, UInt64 delay)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_Play)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Play)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Play)
    
    		if (delay > 0 && IS_CONTENT_NEWER_OR_SAME (kUnityVersion4_1_a3))
    		{
    			WarningStringObject("Delayed playback via the optional argument of Play is deprecated. Use PlayDelayed instead!", self);	
    		}
    		self->Play((double)delay * (const double)(-1.0 / 44100.0));		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_PlayDelayed(ICallType_ReadOnlyUnityEngineObject_Argument self_, float delay)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_PlayDelayed)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(PlayDelayed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PlayDelayed)
    
    		self->Play((delay < 0.0f) ? 0.0 : -(double)delay);		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_PlayScheduled(ICallType_ReadOnlyUnityEngineObject_Argument self_, double time)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_PlayScheduled)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(PlayScheduled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PlayScheduled)
    
    		self->Play((time < 0.0) ? 0.0 : time);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_SetScheduledStartTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, double time)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_SetScheduledStartTime)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetScheduledStartTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetScheduledStartTime)
    
    		self->SetScheduledStartTime(time);		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_SetScheduledEndTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, double time)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_SetScheduledEndTime)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetScheduledEndTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetScheduledEndTime)
    
    		self->SetScheduledEndTime(time);		
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_Stop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_Stop)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Stop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Stop)
    
    		self->Stop(true);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_INTERNAL_CALL_Pause(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_INTERNAL_CALL_Pause)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Pause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Pause)
    return self->Pause();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropIsPlaying(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropIsPlaying)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isPlaying)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isPlaying)
    return self->IsPlayingScripting ();
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_PlayOneShot(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument clip_, float volumeScale)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_PlayOneShot)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<AudioClip> clip(clip_);
    UNUSED(clip);
    SCRIPTINGAPI_STACK_CHECK(PlayOneShot)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PlayOneShot)
     if (clip) self->PlayOneShot (*clip, volumeScale); 
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropLoop(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropLoop)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_loop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_loop)
    return self->GetLoop ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropLoop(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropLoop)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_loop)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_loop)
    
    self->SetLoop (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropIgnoreListenerVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropIgnoreListenerVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_ignoreListenerVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_ignoreListenerVolume)
    return self->GetIgnoreListenerVolume();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropIgnoreListenerVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropIgnoreListenerVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_ignoreListenerVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_ignoreListenerVolume)
    
    self->SetIgnoreListenerVolume(value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropPlayOnAwake(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropPlayOnAwake)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_playOnAwake)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_playOnAwake)
    return self->GetPlayOnAwake ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropPlayOnAwake(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropPlayOnAwake)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_playOnAwake)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_playOnAwake)
    
    self->SetPlayOnAwake (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropIgnoreListenerPause(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropIgnoreListenerPause)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_ignoreListenerPause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_ignoreListenerPause)
    return self->GetIgnoreListenerPause ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropIgnoreListenerPause(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropIgnoreListenerPause)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_ignoreListenerPause)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_ignoreListenerPause)
    
    self->SetIgnoreListenerPause (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropVelocityUpdateMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropVelocityUpdateMode)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_velocityUpdateMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_velocityUpdateMode)
    return self->GetVelocityUpdateMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropVelocityUpdateMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropVelocityUpdateMode)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_velocityUpdateMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_velocityUpdateMode)
    
    self->SetVelocityUpdateMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropPanLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropPanLevel)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_panLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_panLevel)
    return self->GetPanLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropPanLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropPanLevel)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_panLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_panLevel)
    
    self->SetPanLevel (value);
    
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropBypassEffects(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropBypassEffects)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bypassEffects)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bypassEffects)
    return self->GetBypassEffects ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropBypassEffects(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropBypassEffects)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bypassEffects)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bypassEffects)
    
    self->SetBypassEffects (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropBypassListenerEffects(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropBypassListenerEffects)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bypassListenerEffects)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bypassListenerEffects)
    return self->GetBypassListenerEffects ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropBypassListenerEffects(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropBypassListenerEffects)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bypassListenerEffects)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bypassListenerEffects)
    
    self->SetBypassListenerEffects (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropBypassReverbZones(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropBypassReverbZones)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_bypassReverbZones)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_bypassReverbZones)
    return self->GetBypassReverbZones ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropBypassReverbZones(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropBypassReverbZones)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_bypassReverbZones)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_bypassReverbZones)
    
    self->SetBypassReverbZones (value);
    
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropDopplerLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropDopplerLevel)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_dopplerLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dopplerLevel)
    return self->GetDopplerLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropDopplerLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropDopplerLevel)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_dopplerLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dopplerLevel)
    
    self->SetDopplerLevel (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropSpread(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropSpread)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_spread)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_spread)
    return self->GetSpread ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropSpread(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropSpread)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_spread)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_spread)
    
    self->SetSpread (value);
    
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropPriority(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropPriority)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_priority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_priority)
    return self->GetPriority ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropPriority(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropPriority)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_priority)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_priority)
    
    self->SetPriority (value);
    
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropMute(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropMute)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mute)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mute)
    return self->GetMute ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropMute(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropMute)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_mute)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_mute)
    
    self->SetMute (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropMinDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropMinDistance)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_minDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_minDistance)
    return self->GetMinDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropMinDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropMinDistance)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_minDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_minDistance)
    
    self->SetMinDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropMaxDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropMaxDistance)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maxDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxDistance)
    return self->GetMaxDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropMaxDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropMaxDistance)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maxDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxDistance)
    
    self->SetMaxDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropPan(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropPan)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_pan)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_pan)
    return self->GetPan ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropPan(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropPan)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_pan)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_pan)
    
    self->SetPan (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
RolloffMode SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropRolloffMode(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropRolloffMode)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rolloffMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rolloffMode)
    return self->GetRolloffMode ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropRolloffMode(ICallType_ReadOnlyUnityEngineObject_Argument self_, RolloffMode value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropRolloffMode)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_rolloffMode)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_rolloffMode)
    
    self->SetRolloffMode (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_GetOutputDataHelper(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument samples_, int channel)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_GetOutputDataHelper)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    ICallType_Array_Local samples(samples_);
    UNUSED(samples);
    SCRIPTINGAPI_STACK_CHECK(GetOutputDataHelper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetOutputDataHelper)
    
    #if ENABLE_AUDIO_FMOD
    		self->GetOutputData(GetScriptingArrayStart<float>(samples), GetScriptingArraySize(samples), channel);
    #endif
    	
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_CUSTOM_GetSpectrumDataHelper(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_Array_Argument samples_, int channel, FMOD_DSP_FFT_WINDOW window)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_CUSTOM_GetSpectrumDataHelper)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    ICallType_Array_Local samples(samples_);
    UNUSED(samples);
    SCRIPTINGAPI_STACK_CHECK(GetSpectrumDataHelper)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetSpectrumDataHelper)
    
    		self->GetSpectrumData(GetScriptingArrayStart<float>(samples), GetScriptingArraySize(samples), channel, (FMOD_DSP_FFT_WINDOW) window);
    	
}

#endif
#if ENABLE_AUDIO
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropMinVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropMinVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_minVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_minVolume)
    ErrorString("minVolume is not supported anymore. Use min-, maxDistance and rolloffMode instead.");
    return 0.0f;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropMinVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropMinVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_minVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_minVolume)
    
    ErrorString("minVolume is not supported anymore. Use min-, maxDistance and rolloffMode instead.");
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropMaxVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropMaxVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maxVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxVolume)
    ErrorString("maxVolume is not supported anymore. Use min-, maxDistance and rolloffMode instead.");
    return 0.0f;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropMaxVolume(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropMaxVolume)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maxVolume)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxVolume)
    
    ErrorString("maxVolume is not supported anymore. Use min-, maxDistance and rolloffMode instead.");
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioSource_Get_Custom_PropRolloffFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Get_Custom_PropRolloffFactor)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rolloffFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rolloffFactor)
    ErrorString("rolloffFactor is not supported anymore. Use min-, maxDistance and rolloffMode instead.");
    return 0.0f;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioSource_Set_Custom_PropRolloffFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioSource_Set_Custom_PropRolloffFactor)
    ReadOnlyScriptingObjectOfType<AudioSource> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_rolloffFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_rolloffFactor)
    
    ErrorString("rolloffFactor is not supported anymore. Use min-, maxDistance and rolloffMode instead.");
    
}

#endif
#if ENABLE_AUDIO_FMOD
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropMinDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropMinDistance)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_minDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_minDistance)
    return self->GetMinDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropMinDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropMinDistance)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_minDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_minDistance)
    
    self->SetMinDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropMaxDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropMaxDistance)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_maxDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxDistance)
    return self->GetMaxDistance ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropMaxDistance(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropMaxDistance)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_maxDistance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxDistance)
    
    self->SetMaxDistance (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropReverbPreset(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropReverbPreset)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reverbPreset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reverbPreset)
    return self->GetReverbPreset ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropReverbPreset(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropReverbPreset)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reverbPreset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reverbPreset)
    
    self->SetReverbPreset (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropRoom(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropRoom)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_room)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_room)
    return self->GetRoom ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropRoom(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropRoom)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_room)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_room)
    
    self->SetRoom (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropRoomHF(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropRoomHF)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_roomHF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_roomHF)
    return self->GetRoomHF ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropRoomHF(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropRoomHF)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_roomHF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_roomHF)
    
    self->SetRoomHF (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropRoomLF(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropRoomLF)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_roomLF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_roomLF)
    return self->GetRoomLF ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropRoomLF(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropRoomLF)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_roomLF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_roomLF)
    
    self->SetRoomLF (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropDecayTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropDecayTime)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_decayTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_decayTime)
    return self->GetDecayTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropDecayTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropDecayTime)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_decayTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_decayTime)
    
    self->SetDecayTime (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropDecayHFRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropDecayHFRatio)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_decayHFRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_decayHFRatio)
    return self->GetDecayHFRatio ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropDecayHFRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropDecayHFRatio)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_decayHFRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_decayHFRatio)
    
    self->SetDecayHFRatio (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropReflections(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropReflections)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reflections)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reflections)
    return self->GetReflections ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropReflections(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropReflections)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reflections)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reflections)
    
    self->SetReflections (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropReflectionsDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropReflectionsDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reflectionsDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reflectionsDelay)
    return self->GetReflectionsDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropReflectionsDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropReflectionsDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reflectionsDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reflectionsDelay)
    
    self->SetReflectionsDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropReverb(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropReverb)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reverb)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reverb)
    return self->GetReverb ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropReverb(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropReverb)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reverb)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reverb)
    
    self->SetReverb (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropReverbDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropReverbDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reverbDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reverbDelay)
    return self->GetReverbDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropReverbDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropReverbDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reverbDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reverbDelay)
    
    self->SetReverbDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropHFReference(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropHFReference)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_HFReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_HFReference)
    return self->GetHFReference ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropHFReference(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropHFReference)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_HFReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_HFReference)
    
    self->SetHFReference (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropLFReference(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropLFReference)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_LFReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_LFReference)
    return self->GetLFReference ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropLFReference(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropLFReference)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_LFReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_LFReference)
    
    self->SetLFReference (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropRoomRolloffFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropRoomRolloffFactor)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_roomRolloffFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_roomRolloffFactor)
    return self->GetRoomRolloffFactor ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropRoomRolloffFactor(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropRoomRolloffFactor)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_roomRolloffFactor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_roomRolloffFactor)
    
    self->SetRoomRolloffFactor (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropDiffusion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropDiffusion)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_diffusion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_diffusion)
    return self->GetDiffusion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropDiffusion(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropDiffusion)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_diffusion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_diffusion)
    
    self->SetDiffusion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbZone_Get_Custom_PropDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Get_Custom_PropDensity)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_density)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_density)
    return self->GetDensity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbZone_Set_Custom_PropDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbZone_Set_Custom_PropDensity)
    ReadOnlyScriptingObjectOfType<AudioReverbZone> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_density)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_density)
    
    self->SetDensity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioLowPassFilter_Get_Custom_PropCutoffFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioLowPassFilter_Get_Custom_PropCutoffFrequency)
    ReadOnlyScriptingObjectOfType<AudioLowPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cutoffFrequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cutoffFrequency)
    return self->GetCutoffFrequency ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioLowPassFilter_Set_Custom_PropCutoffFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioLowPassFilter_Set_Custom_PropCutoffFrequency)
    ReadOnlyScriptingObjectOfType<AudioLowPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cutoffFrequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cutoffFrequency)
    
    self->SetCutoffFrequency (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioLowPassFilter_Get_Custom_PropLowpassResonaceQ(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioLowPassFilter_Get_Custom_PropLowpassResonaceQ)
    ReadOnlyScriptingObjectOfType<AudioLowPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lowpassResonaceQ)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lowpassResonaceQ)
    return self->GetLowpassResonanceQ ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioLowPassFilter_Set_Custom_PropLowpassResonaceQ(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioLowPassFilter_Set_Custom_PropLowpassResonaceQ)
    ReadOnlyScriptingObjectOfType<AudioLowPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lowpassResonaceQ)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lowpassResonaceQ)
    
    self->SetLowpassResonanceQ (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioHighPassFilter_Get_Custom_PropCutoffFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioHighPassFilter_Get_Custom_PropCutoffFrequency)
    ReadOnlyScriptingObjectOfType<AudioHighPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_cutoffFrequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_cutoffFrequency)
    return self->GetCutoffFrequency ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioHighPassFilter_Set_Custom_PropCutoffFrequency(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioHighPassFilter_Set_Custom_PropCutoffFrequency)
    ReadOnlyScriptingObjectOfType<AudioHighPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_cutoffFrequency)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_cutoffFrequency)
    
    self->SetCutoffFrequency (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioHighPassFilter_Get_Custom_PropHighpassResonaceQ(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioHighPassFilter_Get_Custom_PropHighpassResonaceQ)
    ReadOnlyScriptingObjectOfType<AudioHighPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_highpassResonaceQ)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_highpassResonaceQ)
    return self->GetHighpassResonanceQ ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioHighPassFilter_Set_Custom_PropHighpassResonaceQ(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioHighPassFilter_Set_Custom_PropHighpassResonaceQ)
    ReadOnlyScriptingObjectOfType<AudioHighPassFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_highpassResonaceQ)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_highpassResonaceQ)
    
    self->SetHighpassResonanceQ (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioDistortionFilter_Get_Custom_PropDistortionLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioDistortionFilter_Get_Custom_PropDistortionLevel)
    ReadOnlyScriptingObjectOfType<AudioDistortionFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_distortionLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_distortionLevel)
    return self->GetDistortionLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioDistortionFilter_Set_Custom_PropDistortionLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioDistortionFilter_Set_Custom_PropDistortionLevel)
    ReadOnlyScriptingObjectOfType<AudioDistortionFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_distortionLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_distortionLevel)
    
    self->SetDistortionLevel (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioEchoFilter_Get_Custom_PropDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Get_Custom_PropDelay)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_delay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_delay)
    return self->GetDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioEchoFilter_Set_Custom_PropDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Set_Custom_PropDelay)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_delay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_delay)
    
    self->SetDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioEchoFilter_Get_Custom_PropDecayRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Get_Custom_PropDecayRatio)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_decayRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_decayRatio)
    return self->GetDecayRatio ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioEchoFilter_Set_Custom_PropDecayRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Set_Custom_PropDecayRatio)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_decayRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_decayRatio)
    
    self->SetDecayRatio (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioEchoFilter_Get_Custom_PropDryMix(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Get_Custom_PropDryMix)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_dryMix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dryMix)
    return self->GetDryMix ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioEchoFilter_Set_Custom_PropDryMix(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Set_Custom_PropDryMix)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_dryMix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dryMix)
    
    self->SetDryMix (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioEchoFilter_Get_Custom_PropWetMix(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Get_Custom_PropWetMix)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wetMix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wetMix)
    return self->GetWetMix ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioEchoFilter_Set_Custom_PropWetMix(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioEchoFilter_Set_Custom_PropWetMix)
    ReadOnlyScriptingObjectOfType<AudioEchoFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wetMix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wetMix)
    
    self->SetWetMix (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropDryMix(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropDryMix)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_dryMix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dryMix)
    return self->GetDryMix ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropDryMix(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropDryMix)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_dryMix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dryMix)
    
    self->SetDryMix (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropWetMix1(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropWetMix1)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wetMix1)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wetMix1)
    return self->GetWetMix1 ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropWetMix1(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropWetMix1)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wetMix1)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wetMix1)
    
    self->SetWetMix1 (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropWetMix2(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropWetMix2)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wetMix2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wetMix2)
    return self->GetWetMix2 ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropWetMix2(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropWetMix2)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wetMix2)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wetMix2)
    
    self->SetWetMix2 (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropWetMix3(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropWetMix3)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_wetMix3)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_wetMix3)
    return self->GetWetMix3 ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropWetMix3(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropWetMix3)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_wetMix3)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_wetMix3)
    
    self->SetWetMix3 (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropDelay)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_delay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_delay)
    return self->GetDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropDelay)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_delay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_delay)
    
    self->SetDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropRate(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropRate)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_rate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_rate)
    return self->GetRate ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropRate(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropRate)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_rate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_rate)
    
    self->SetRate (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_depth)
    return self->GetDepth ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropDepth(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropDepth)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_depth)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_depth)
    
    self->SetDepth (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioChorusFilter_Get_Custom_PropFeedback(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Get_Custom_PropFeedback)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_feedback)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_feedback)
    return 0.0f;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioChorusFilter_Set_Custom_PropFeedback(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioChorusFilter_Set_Custom_PropFeedback)
    ReadOnlyScriptingObjectOfType<AudioChorusFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_feedback)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_feedback)
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropReverbPreset(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropReverbPreset)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reverbPreset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reverbPreset)
    return self->GetReverbPreset ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropReverbPreset(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropReverbPreset)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reverbPreset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reverbPreset)
    
    self->SetReverbPreset (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropDryLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropDryLevel)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_dryLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dryLevel)
    return self->GetDryLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropDryLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropDryLevel)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_dryLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dryLevel)
    
    self->SetDryLevel (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropRoom(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropRoom)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_room)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_room)
    return self->GetRoom ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropRoom(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropRoom)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_room)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_room)
    
    self->SetRoom (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropRoomHF(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropRoomHF)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_roomHF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_roomHF)
    return self->GetRoomHF ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropRoomHF(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropRoomHF)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_roomHF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_roomHF)
    
    self->SetRoomHF (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropRoomRolloff(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropRoomRolloff)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_roomRolloff)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_roomRolloff)
    return self->GetRoomRolloff ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropRoomRolloff(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropRoomRolloff)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_roomRolloff)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_roomRolloff)
    
    self->SetRoomRolloff (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropDecayTime(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropDecayTime)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_decayTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_decayTime)
    return self->GetDecayTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropDecayTime(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropDecayTime)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_decayTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_decayTime)
    
    self->SetDecayTime (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropDecayHFRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropDecayHFRatio)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_decayHFRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_decayHFRatio)
    return self->GetDecayHFRatio ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropDecayHFRatio(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropDecayHFRatio)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_decayHFRatio)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_decayHFRatio)
    
    self->SetDecayHFRatio (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropReflectionsLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropReflectionsLevel)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reflectionsLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reflectionsLevel)
    return self->GetReflectionsLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropReflectionsLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropReflectionsLevel)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reflectionsLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reflectionsLevel)
    
    self->SetReflectionsLevel (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropReflectionsDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropReflectionsDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reflectionsDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reflectionsDelay)
    return self->GetReflectionsDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropReflectionsDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropReflectionsDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reflectionsDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reflectionsDelay)
    
    self->SetReflectionsDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropReverbLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropReverbLevel)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reverbLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reverbLevel)
    return self->GetReverbLevel ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropReverbLevel(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropReverbLevel)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reverbLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reverbLevel)
    
    self->SetReverbLevel (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropReverbDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropReverbDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_reverbDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_reverbDelay)
    return self->GetReverbDelay ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropReverbDelay(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropReverbDelay)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_reverbDelay)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_reverbDelay)
    
    self->SetReverbDelay (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropDiffusion(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropDiffusion)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_diffusion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_diffusion)
    return self->GetDiffusion ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropDiffusion(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropDiffusion)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_diffusion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_diffusion)
    
    self->SetDiffusion (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropDensity)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_density)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_density)
    return self->GetDensity ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropDensity(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropDensity)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_density)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_density)
    
    self->SetDensity (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropHfReference(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropHfReference)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_hfReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_hfReference)
    return self->GetHFReference ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropHfReference(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropHfReference)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_hfReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_hfReference)
    
    self->SetHFReference (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropRoomLF(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropRoomLF)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_roomLF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_roomLF)
    return self->GetRoomLF ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropRoomLF(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropRoomLF)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_roomLF)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_roomLF)
    
    self->SetRoomLF (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AudioReverbFilter_Get_Custom_PropLFReference(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Get_Custom_PropLFReference)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_lFReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_lFReference)
    return self->GetLFReference ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AudioReverbFilter_Set_Custom_PropLFReference(ICallType_ReadOnlyUnityEngineObject_Argument self_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(AudioReverbFilter_Set_Custom_PropLFReference)
    ReadOnlyScriptingObjectOfType<AudioReverbFilter> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_lFReference)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_lFReference)
    
    self->SetLFReference (value);
    
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_TrySetInt(ICallType_String_Argument key_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_TrySetInt)
    ICallType_String_Local key(key_);
    UNUSED(key);
    SCRIPTINGAPI_STACK_CHECK(TrySetInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TrySetInt)
     return (int)PlayerPrefs::SetInt (key, value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_TrySetFloat(ICallType_String_Argument key_, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_TrySetFloat)
    ICallType_String_Local key(key_);
    UNUSED(key);
    SCRIPTINGAPI_STACK_CHECK(TrySetFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TrySetFloat)
     return (int)PlayerPrefs::SetFloat (key, value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_TrySetSetString(ICallType_String_Argument key_, ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_TrySetSetString)
    ICallType_String_Local key(key_);
    UNUSED(key);
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(TrySetSetString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TrySetSetString)
     return (int)PlayerPrefs::SetString (key, value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_GetInt(ICallType_String_Argument key_, int defaultValue)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_GetInt)
    ICallType_String_Local key(key_);
    UNUSED(key);
    SCRIPTINGAPI_STACK_CHECK(GetInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetInt)
     return PlayerPrefs::GetInt (key, defaultValue); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_GetFloat(ICallType_String_Argument key_, float defaultValue)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_GetFloat)
    ICallType_String_Local key(key_);
    UNUSED(key);
    SCRIPTINGAPI_STACK_CHECK(GetFloat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetFloat)
     return PlayerPrefs::GetFloat (key, defaultValue); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_GetString(ICallType_String_Argument key_, ICallType_String_Argument defaultValue_)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_GetString)
    ICallType_String_Local key(key_);
    UNUSED(key);
    ICallType_String_Local defaultValue(defaultValue_);
    UNUSED(defaultValue);
    SCRIPTINGAPI_STACK_CHECK(GetString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetString)
     return CreateScriptingString (PlayerPrefs::GetString (key, defaultValue)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_HasKey(ICallType_String_Argument key_)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_HasKey)
    ICallType_String_Local key(key_);
    UNUSED(key);
    SCRIPTINGAPI_STACK_CHECK(HasKey)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasKey)
     return (int)PlayerPrefs::HasKey(key); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_DeleteKey(ICallType_String_Argument key_)
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_DeleteKey)
    ICallType_String_Local key(key_);
    UNUSED(key);
    SCRIPTINGAPI_STACK_CHECK(DeleteKey)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DeleteKey)
     PlayerPrefs::DeleteKey(key); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_DeleteAll()
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_DeleteAll)
    SCRIPTINGAPI_STACK_CHECK(DeleteAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(DeleteAll)
     PlayerPrefs::DeleteAll(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION PlayerPrefs_CUSTOM_Save()
{
    SCRIPTINGAPI_ETW_ENTRY(PlayerPrefs_CUSTOM_Save)
    SCRIPTINGAPI_STACK_CHECK(Save)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Save)
     PlayerPrefs::Sync(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropOperatingSystem()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropOperatingSystem)
    SCRIPTINGAPI_STACK_CHECK(get_operatingSystem)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_operatingSystem)
    return CreateScriptingString( systeminfo::GetOperatingSystem() );
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropProcessorType()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropProcessorType)
    SCRIPTINGAPI_STACK_CHECK(get_processorType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_processorType)
    return CreateScriptingString( systeminfo::GetProcessorType() );
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropProcessorCount()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropProcessorCount)
    SCRIPTINGAPI_STACK_CHECK(get_processorCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_processorCount)
    return systeminfo::GetProcessorCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSystemMemorySize()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSystemMemorySize)
    SCRIPTINGAPI_STACK_CHECK(get_systemMemorySize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_systemMemorySize)
    return systeminfo::GetPhysicalMemoryMB();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsMemorySize()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsMemorySize)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsMemorySize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsMemorySize)
    return (int)gGraphicsCaps.videoMemoryMB;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsDeviceName()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsDeviceName)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsDeviceName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsDeviceName)
    return CreateScriptingString(gGraphicsCaps.rendererString.c_str());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsDeviceVendor()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsDeviceVendor)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsDeviceVendor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsDeviceVendor)
    return CreateScriptingString(gGraphicsCaps.vendorString.c_str());
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsDeviceID()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsDeviceID)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsDeviceID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsDeviceID)
    return gGraphicsCaps.rendererID;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsDeviceVendorID()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsDeviceVendorID)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsDeviceVendorID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsDeviceVendorID)
    return gGraphicsCaps.vendorID;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsDeviceVersion()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsDeviceVersion)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsDeviceVersion)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsDeviceVersion)
    return CreateScriptingString(gGraphicsCaps.fixedVersionString.c_str());
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsShaderLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsShaderLevel)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsShaderLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsShaderLevel)
    return gGraphicsCaps.shaderCaps;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropGraphicsPixelFillrate()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropGraphicsPixelFillrate)
    SCRIPTINGAPI_STACK_CHECK(get_graphicsPixelFillrate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_graphicsPixelFillrate)
    return GetGraphicsPixelFillrate (gGraphicsCaps.vendorID, gGraphicsCaps.rendererID);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsShadows()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsShadows)
    SCRIPTINGAPI_STACK_CHECK(get_supportsShadows)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsShadows)
    return RenderTexture::IsEnabled() && GetBuildSettings().hasShadows && gGraphicsCaps.hasFragmentProgram && gGraphicsCaps.supportsRenderTextureFormat[kRTFormatDepth];
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsRenderTextures()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsRenderTextures)
    SCRIPTINGAPI_STACK_CHECK(get_supportsRenderTextures)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsRenderTextures)
    return RenderTexture::IsEnabled();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsImageEffects()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsImageEffects)
    SCRIPTINGAPI_STACK_CHECK(get_supportsImageEffects)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsImageEffects)
    return RenderTexture::IsEnabled() && (gGraphicsCaps.npotRT >= kNPOTRestricted);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupports3DTextures()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupports3DTextures)
    SCRIPTINGAPI_STACK_CHECK(get_supports3DTextures)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supports3DTextures)
    return gGraphicsCaps.has3DTexture;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsComputeShaders()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsComputeShaders)
    SCRIPTINGAPI_STACK_CHECK(get_supportsComputeShaders)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsComputeShaders)
    return gGraphicsCaps.hasComputeShader;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsInstancing()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsInstancing)
    SCRIPTINGAPI_STACK_CHECK(get_supportsInstancing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsInstancing)
    return gGraphicsCaps.hasInstancing;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportedRenderTargetCount()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportedRenderTargetCount)
    SCRIPTINGAPI_STACK_CHECK(get_supportedRenderTargetCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportedRenderTargetCount)
    return RenderTexture::IsEnabled() ? gGraphicsCaps.maxMRTs : 0;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsStencil()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsStencil)
    SCRIPTINGAPI_STACK_CHECK(get_supportsStencil)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsStencil)
    return gGraphicsCaps.hasStencil;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsVertexPrograms()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsVertexPrograms)
    SCRIPTINGAPI_STACK_CHECK(get_supportsVertexPrograms)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsVertexPrograms)
    return gGraphicsCaps.hasVertexProgram;
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_CUSTOM_SupportsRenderTextureFormat(RenderTextureFormat format)
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_CUSTOM_SupportsRenderTextureFormat)
    SCRIPTINGAPI_STACK_CHECK(SupportsRenderTextureFormat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SupportsRenderTextureFormat)
    
    		return RenderTexture::IsEnabled() && gGraphicsCaps.supportsRenderTextureFormat[format];
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropNpotSupport()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropNpotSupport)
    SCRIPTINGAPI_STACK_CHECK(get_npotSupport)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_npotSupport)
    return gGraphicsCaps.npot;
}

#if !UNITY_FLASH && !PLATFORM_WEBGL
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropDeviceUniqueIdentifier()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropDeviceUniqueIdentifier)
    SCRIPTINGAPI_STACK_CHECK(get_deviceUniqueIdentifier)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceUniqueIdentifier)
    return CreateScriptingString (systeminfo::GetDeviceUniqueIdentifier ());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropDeviceName()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropDeviceName)
    SCRIPTINGAPI_STACK_CHECK(get_deviceName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceName)
    return CreateScriptingString (systeminfo::GetDeviceName ());
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropDeviceModel()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropDeviceModel)
    SCRIPTINGAPI_STACK_CHECK(get_deviceModel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceModel)
    return CreateScriptingString (systeminfo::GetDeviceModel ());
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsAccelerometer()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsAccelerometer)
    SCRIPTINGAPI_STACK_CHECK(get_supportsAccelerometer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsAccelerometer)
    return systeminfo::SupportsAccelerometer ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsGyroscope()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsGyroscope)
    SCRIPTINGAPI_STACK_CHECK(get_supportsGyroscope)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsGyroscope)
    return IsGyroAvailable ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsLocationService()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsLocationService)
    SCRIPTINGAPI_STACK_CHECK(get_supportsLocationService)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsLocationService)
    return systeminfo::SupportsLocationService ();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropSupportsVibration()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropSupportsVibration)
    SCRIPTINGAPI_STACK_CHECK(get_supportsVibration)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supportsVibration)
    return systeminfo::SupportsVibration ();
}

SCRIPT_BINDINGS_EXPORT_DECL
DeviceType SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropDeviceType()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropDeviceType)
    SCRIPTINGAPI_STACK_CHECK(get_deviceType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deviceType)
    return systeminfo::DeviceType ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION SystemInfo_Get_Custom_PropMaxTextureSize()
{
    SCRIPTINGAPI_ETW_ENTRY(SystemInfo_Get_Custom_PropMaxTextureSize)
    SCRIPTINGAPI_STACK_CHECK(get_maxTextureSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxTextureSize)
    return gGraphicsCaps.maxTextureSize;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Coroutine_CUSTOM_ReleaseCoroutine(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Coroutine_CUSTOM_ReleaseCoroutine)
    ScriptingObjectWithIntPtrField<Coroutine> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(ReleaseCoroutine)
    
    		Assert (self.GetPtr() != NULL);
    		Coroutine::CleanupCoroutineGC (self);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ScriptableObject_CUSTOM_Internal_CreateScriptableObject(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ScriptableObject_CUSTOM_Internal_CreateScriptableObject)
    ScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_CreateScriptableObject)
    
    		CreateEngineScriptableObject(self.GetScriptingObject());
    	
}

#if ENABLE_MONO
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION ScriptableObject_CUSTOM_INTERNAL_CALL_SetDirty(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(ScriptableObject_CUSTOM_INTERNAL_CALL_SetDirty)
    ReadOnlyScriptingObjectOfType<MonoBehaviour> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetDirty)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetDirty)
    return self->SetDirty();
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION ScriptableObject_CUSTOM_CreateInstance(ICallType_String_Argument className_)
{
    SCRIPTINGAPI_ETW_ENTRY(ScriptableObject_CUSTOM_CreateInstance)
    ICallType_String_Local className(className_);
    UNUSED(className);
    SCRIPTINGAPI_STACK_CHECK(CreateInstance)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CreateInstance)
     return CreateScriptableObject (className.ToUTF8()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION ScriptableObject_CUSTOM_CreateInstanceFromType(ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(ScriptableObject_CUSTOM_CreateInstanceFromType)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(CreateInstanceFromType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CreateInstanceFromType)
     return CreateScriptableObjectWithType (type); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Resources_CUSTOM_FindObjectsOfTypeAll(ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_FindObjectsOfTypeAll)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(FindObjectsOfTypeAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(FindObjectsOfTypeAll)
     return ScriptingFindObjectsOfType (type, kFindAnything); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Resources_CUSTOM_Load(ICallType_String_Argument path_, ICallType_SystemTypeObject_Argument systemTypeInstance_)
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_Load)
    ICallType_String_Local path(path_);
    UNUSED(path);
    ScriptingSystemTypeObjectPtr systemTypeInstance(systemTypeInstance_);
    UNUSED(systemTypeInstance);
    SCRIPTINGAPI_STACK_CHECK(Load)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Load)
    
    		MonoRaiseIfNull (systemTypeInstance);
    
    		ScriptingClassPtr klass = GetScriptingTypeRegistry().GetType(systemTypeInstance);
    		string lowerPath = ToLower(path.ToUTF8());
    		ResourceManager::range found = GetResourceManager().GetPathRange(lowerPath);
    
    		Object* obj = NULL;
    		ScriptingObjectPtr o = SCRIPTING_NULL;
    		for (ResourceManager::iterator i=found.first;i != found.second;i++)
    		{
    			obj = i->second;
    			o = Scripting::ScriptingWrapperForImpl(obj);
    			if (o == SCRIPTING_NULL)
    				continue;
    
    			ScriptingClassPtr k = scripting_object_get_class(o, GetScriptingTypeRegistry());
    			if (o && scripting_class_is_subclass_of(k,klass))
    			{
    				GetResourceManager ().PreloadDependencies (obj->GetInstanceID ());
    				break;
    			}
    
    			GameObject* go = dynamic_pptr_cast<GameObject*> (obj);
    			if (go != NULL)
    			{
    				o = ScriptingGetComponentOfType(*go, systemTypeInstance, false);
    				if (o != SCRIPTING_NULL)
    				{
    					GetResourceManager ().PreloadDependencies (obj->GetInstanceID ());
    					break;
    				}
    			}
    		}
    
    		if (obj && PLATFORM_ANDROID)
    		{
    			PersistentManager& pm = GetPersistentManager();
    			pm.UnloadNonDirtyStreams();
    		}
    
    		return o;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Resources_CUSTOM_LoadAll(ICallType_String_Argument path_, ICallType_SystemTypeObject_Argument systemTypeInstance_)
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_LoadAll)
    ICallType_String_Local path(path_);
    UNUSED(path);
    ScriptingSystemTypeObjectPtr systemTypeInstance(systemTypeInstance_);
    UNUSED(systemTypeInstance);
    SCRIPTINGAPI_STACK_CHECK(LoadAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadAll)
    
    		MonoRaiseIfNull (systemTypeInstance);
    
    		ScriptingClassPtr klass = GetScriptingTypeRegistry().GetType(systemTypeInstance);
    		ResourceManager::range found = GetResourceManager().GetAll();
    		string cpath = ToLower(path.ToUTF8());
    
    		vector<PPtr<Object> > objects;
    
    		for (ResourceManager::iterator i=found.first;i != found.second;i++)
    		{
    			if (!StartsWithPath(i->first, cpath))
    				continue;
    
    			Object* obj = i->second;
    			ScriptingObjectPtr o = Scripting::ScriptingWrapperForImpl(obj);
    			if (o == SCRIPTING_NULL)
    				continue;
    
    			ScriptingClassPtr k = scripting_object_get_class(o, GetScriptingTypeRegistry());
    			if (o && scripting_class_is_subclass_of(k, klass))
    			{
    				objects.push_back(i->second);
    				GetResourceManager ().PreloadDependencies (obj->GetInstanceID ());
    			}
    			else
    			{
    				GameObject* go = dynamic_pptr_cast<GameObject*> (obj);
    				if (go != NULL)
    				{
    					o = ScriptingGetComponentOfType(*go, systemTypeInstance, false);
    					if (o != SCRIPTING_NULL)
    					{
    						objects.push_back(ScriptingObjectToObject<Object>(o));
    						GetResourceManager ().PreloadDependencies (obj->GetInstanceID ());
    					}
    				}
    			}
    		}
    
    		return CreateScriptingArrayFromUnityObjects (objects, ClassID(Object));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Resources_CUSTOM_GetBuiltinResource(ICallType_SystemTypeObject_Argument type_, ICallType_String_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_GetBuiltinResource)
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    ICallType_String_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(GetBuiltinResource)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetBuiltinResource)
    
    		MonoRaiseIfNull(type);
    		return GetScriptingBuiltinResource(type, path.ToUTF8());
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Resources_CUSTOM_LoadAssetAtPath(ICallType_String_Argument assetPath_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_LoadAssetAtPath)
    ICallType_String_Local assetPath(assetPath_);
    UNUSED(assetPath);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(LoadAssetAtPath)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadAssetAtPath)
    
    		#if UNITY_EDITOR
    		return LoadAssetAtPath(assetPath, type);
    		#else
    		return SCRIPTING_NULL;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Resources_CUSTOM_UnloadAsset(ICallType_ReadOnlyUnityEngineObject_Argument assetToUnload_)
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_UnloadAsset)
    ReadOnlyScriptingObjectOfType<Object> assetToUnload(assetToUnload_);
    UNUSED(assetToUnload);
    SCRIPTINGAPI_STACK_CHECK(UnloadAsset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UnloadAsset)
     UnloadAssetFromScripting (assetToUnload); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Resources_CUSTOM_UnloadUnusedAssets()
{
    SCRIPTINGAPI_ETW_ENTRY(Resources_CUSTOM_UnloadUnusedAssets)
    SCRIPTINGAPI_STACK_CHECK(UnloadUnusedAssets)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UnloadUnusedAssets)
    
    		AsyncOperation* result = UnloadUnusedAssetsOperation::UnloadUnusedAssets ();
    		ScriptingObjectPtr o = scripting_object_new(GetCommonScriptingClasses ().asyncOperation);
    		ScriptingObjectWithIntPtrField<AsyncOperation>(o).SetPtr(result);
    		return o;
    	
}

#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AssetBundleCreateRequest_Get_Custom_PropAssetBundle(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundleCreateRequest_Get_Custom_PropAssetBundle)
    ScriptingObjectWithIntPtrField<AssetBundleLoadFromAsyncOperation> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_assetBundle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_assetBundle)
    AssetBundleCreateRequest* ptr;
    MarshallManagedStructIntoNative(self,&ptr);
    return Scripting::ScriptingWrapperFor (ptr->GetAssetBundle());
}

#endif

 #define GET ExtractMonoObjectData<PreloadManagerOperation*> (self)

 #undef GET

#if ENABLE_WWW
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_CreateFromMemory(ICallType_Array_Argument binary_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_CreateFromMemory)
    ICallType_Array_Local binary(binary_);
    UNUSED(binary);
    SCRIPTINGAPI_STACK_CHECK(CreateFromMemory)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CreateFromMemory)
    
    		if (!binary)
    			return SCRIPTING_NULL;
    
    		int dataSize = GetScriptingArraySize(binary);
    		UInt8* dataPtr = GetScriptingArrayStart<UInt8>(binary);
    
    		AsyncOperation* req = new AssetBundleCreateRequest(dataPtr, dataSize);
    
    
    		return CreateScriptingObjectFromNativeStruct(GetCommonScriptingClasses ().assetBundleCreateRequest,req);
    	
}

#endif
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_CreateFromFile(ICallType_String_Argument path_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_CreateFromFile)
    ICallType_String_Local path(path_);
    UNUSED(path);
    SCRIPTINGAPI_STACK_CHECK(CreateFromFile)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CreateFromFile)
    
    		return Scripting::ScriptingWrapperFor(ExtractAssetBundle(path));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AssetBundle_Get_Custom_PropMainAsset(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_Get_Custom_PropMainAsset)
    ReadOnlyScriptingObjectOfType<AssetBundle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_mainAsset)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_mainAsset)
    return Scripting::ScriptingWrapperFor(LoadMainObjectFromAssetBundle(*self));
}


SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_Contains(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_Contains)
    ReadOnlyScriptingObjectOfType<AssetBundle> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(Contains)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Contains)
    
    		string lowerName = ToLower(name.ToUTF8());
    		AssetBundle::range found = self->GetPathRange(lowerName);
    		return found.first != found.second;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_Load(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_Load)
    ReadOnlyScriptingObjectOfType<AssetBundle> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(Load)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Load)
    
    		MonoRaiseIfNull (type);
    		Object* o = LoadNamedObjectFromAssetBundle(*self, name, type);
    		if (o==0) return SCRIPTING_NULL;
    		return Scripting::ScriptingWrapperForImpl(o);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_LoadAsync(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_String_Argument name_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_LoadAsync)
    ReadOnlyScriptingObjectOfType<AssetBundle> self(self_);
    UNUSED(self);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(LoadAsync)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadAsync)
    
    		AsyncOperation* result = LoadSceneOperation::LoadAssetBundle(*self, name);
    		#if ENABLE_MONO || PLATFORM_WINRT
    		ScriptingObjectPtr mono = scripting_object_new(GetCommonScriptingClasses ().assetBundleRequest);
    		AssetBundleRequestMono data;
    		data.m_Result = result;
    		data.m_Path = name.GetNativeString();
    		data.m_Type = type;
    		data.m_AssetBundle = Scripting::ScriptingWrapperFor((AssetBundle*)self);
    		MarshallNativeStructIntoManaged(data, mono);
    
    		return mono;
    		#else
    		return SCRIPTING_NULL;
    		#endif
    
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_LoadAll(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_SystemTypeObject_Argument type_)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_LoadAll)
    ReadOnlyScriptingObjectOfType<AssetBundle> self(self_);
    UNUSED(self);
    ScriptingSystemTypeObjectPtr type(type_);
    UNUSED(type);
    SCRIPTINGAPI_STACK_CHECK(LoadAll)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(LoadAll)
    
    		MonoRaiseIfNull (type);
    
    		vector<Object* > objects;
    		LoadAllFromAssetBundle(*self, type, objects);
    		return CreateScriptingArrayFromUnityObjects(objects, ClassID(Object));
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AssetBundle_CUSTOM_Unload(ICallType_ReadOnlyUnityEngineObject_Argument self_, ScriptingBool unloadAllLoadedObjects)
{
    SCRIPTINGAPI_ETW_ENTRY(AssetBundle_CUSTOM_Unload)
    ReadOnlyScriptingObjectOfType<AssetBundle> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Unload)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Unload)
    
    		AssetBundle& file = *self;
    		UnloadAssetBundle(file, unloadAllLoadedObjects);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Profiler_Get_Custom_PropSupported()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Get_Custom_PropSupported)
    SCRIPTINGAPI_STACK_CHECK(get_supported)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_supported)
    #if ENABLE_PROFILER
    return GetBuildSettings().hasPROVersion;
    #else
    return false;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Profiler_Get_Custom_PropLogFile()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Get_Custom_PropLogFile)
    SCRIPTINGAPI_STACK_CHECK(get_logFile)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_logFile)
    #if ENABLE_PROFILER
    return CreateScriptingString(UnityProfiler::Get().GetLogPath());
    #else
    return SCRIPTING_NULL;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_Set_Custom_PropLogFile(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Set_Custom_PropLogFile)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_logFile)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_logFile)
    
    #if ENABLE_PROFILER
    if (!GetBuildSettings().hasPROVersion)
    {
    ErrorString("Profiler is only supported in Unity Pro.");
    return;
    }
    UnityProfiler::Get().SetLogPath(value);
    #else
    ErrorString("Profiler is not supported in this build");
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Profiler_Get_Custom_PropEnableBinaryLog()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Get_Custom_PropEnableBinaryLog)
    SCRIPTINGAPI_STACK_CHECK(get_enableBinaryLog)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enableBinaryLog)
    #if ENABLE_PROFILER
    return UnityProfiler::Get().BinaryLogEnabled();
    #else
    return false;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_Set_Custom_PropEnableBinaryLog(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Set_Custom_PropEnableBinaryLog)
    SCRIPTINGAPI_STACK_CHECK(set_enableBinaryLog)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enableBinaryLog)
    
    #if ENABLE_PROFILER
    if (!GetBuildSettings().hasPROVersion)
    {
    ErrorString("Profiler is only supported in Unity Pro.");
    return;
    }
    UnityProfiler::Get().EnableBinaryLog(value);
    #else
    ErrorString("Profiler is not supported in this build");
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Profiler_Get_Custom_PropEnabled()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Get_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(get_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_enabled)
    #if ENABLE_PROFILER
    return UnityProfiler::Get().GetEnabled();
    #else
    return false;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_Set_Custom_PropEnabled(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Set_Custom_PropEnabled)
    SCRIPTINGAPI_STACK_CHECK(set_enabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_enabled)
    
    #if ENABLE_PROFILER
    if (!GetBuildSettings().hasPROVersion)
    {
    ErrorString("Profiler is only supported in Unity Pro.");
    return;
    }
    return UnityProfiler::Get().SetEnabled(value);
    #else
    ErrorString("Profiler is not supported in this build");
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_CUSTOM_AddFramesFromFile(ICallType_String_Argument file_)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_AddFramesFromFile)
    ICallType_String_Local file(file_);
    UNUSED(file);
    SCRIPTINGAPI_STACK_CHECK(AddFramesFromFile)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(AddFramesFromFile)
    
    		#if ENABLE_PROFILER
    		if(file.Length() == 0)
    		{
    			ErrorString ("AddFramesFromFile: Invalid empty path");
    			return;
    		}
    		UnityProfiler::Get().AddFramesFromFile(file);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_CUSTOM_BeginSample(ICallType_String_Argument name_, ICallType_ReadOnlyUnityEngineObject_Argument targetObject_)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_BeginSample)
    ICallType_String_Local name(name_);
    UNUSED(name);
    ReadOnlyScriptingObjectOfType<Object> targetObject(targetObject_);
    UNUSED(targetObject);
    SCRIPTINGAPI_STACK_CHECK(BeginSample)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BeginSample)
    
    		#if ENABLE_PROFILER
    		UnityProfiler& profiler = UnityProfiler::Get();
    		if (profiler.GetIsActive ())
    			UnityProfiler::Get().BeginSampleDynamic(name, targetObject);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_CUSTOM_BeginSampleOnly(ICallType_String_Argument name_)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_BeginSampleOnly)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(BeginSampleOnly)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BeginSampleOnly)
    
    		#if ENABLE_PROFILER
    		UnityProfiler& profiler = UnityProfiler::Get();
    		if (profiler.GetIsActive ())
    			UnityProfiler::Get().BeginSampleDynamic(name, NULL);
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Profiler_CUSTOM_EndSample()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_EndSample)
    SCRIPTINGAPI_STACK_CHECK(EndSample)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(EndSample)
    
    		#if ENABLE_PROFILER
    		UnityProfiler::Get().EndSampleDynamic();
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION Profiler_Get_Custom_PropUsedHeapSize()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_Get_Custom_PropUsedHeapSize)
    SCRIPTINGAPI_STACK_CHECK(get_usedHeapSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_usedHeapSize)
    #if ENABLE_PROFILER
    return GetUsedHeapSize();
    #else
    return 0;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Profiler_CUSTOM_GetRuntimeMemorySize(ICallType_ReadOnlyUnityEngineObject_Argument o_)
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_GetRuntimeMemorySize)
    ReadOnlyScriptingObjectOfType<Object> o(o_);
    UNUSED(o);
    SCRIPTINGAPI_STACK_CHECK(GetRuntimeMemorySize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetRuntimeMemorySize)
    
    		#if ENABLE_PROFILER
    			return o->GetRuntimeMemorySize();
    		#else
    			return 0;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION Profiler_CUSTOM_GetMonoHeapSize()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_GetMonoHeapSize)
    SCRIPTINGAPI_STACK_CHECK(GetMonoHeapSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMonoHeapSize)
    
    		#if ENABLE_PROFILER && ENABLE_MONO
    			return mono_gc_get_heap_size ();
    		#else
    			return 0;
    		#endif
    	 
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION Profiler_CUSTOM_GetMonoUsedSize()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_GetMonoUsedSize)
    SCRIPTINGAPI_STACK_CHECK(GetMonoUsedSize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMonoUsedSize)
    
    		#if ENABLE_PROFILER && ENABLE_MONO
    			return mono_gc_get_used_size ();
    		#else
    			return 0;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION Profiler_CUSTOM_GetTotalAllocatedMemory()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_GetTotalAllocatedMemory)
    SCRIPTINGAPI_STACK_CHECK(GetTotalAllocatedMemory)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTotalAllocatedMemory)
    
    		#if ENABLE_MEMORY_MANAGER
    			return GetMemoryManager().GetTotalAllocatedMemory();
    		#else
    			return 0;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION Profiler_CUSTOM_GetTotalUnusedReservedMemory()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_GetTotalUnusedReservedMemory)
    SCRIPTINGAPI_STACK_CHECK(GetTotalUnusedReservedMemory)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTotalUnusedReservedMemory)
    
    		#if ENABLE_MEMORY_MANAGER
    			return GetMemoryManager().GetTotalUnusedReservedMemory();
    		#else
    			return 0;
    		#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt32 SCRIPT_CALL_CONVENTION Profiler_CUSTOM_GetTotalReservedMemory()
{
    SCRIPTINGAPI_ETW_ENTRY(Profiler_CUSTOM_GetTotalReservedMemory)
    SCRIPTINGAPI_STACK_CHECK(GetTotalReservedMemory)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetTotalReservedMemory)
    
    		#if ENABLE_MEMORY_MANAGER
    			return GetMemoryManager().GetTotalReservedMemory();
    		#else
    			return 0;
    		#endif
    	
}

#if ENABLE_MONO
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Reproduction_CUSTOM_CaptureScreenshot()
{
    SCRIPTINGAPI_ETW_ENTRY(Reproduction_CUSTOM_CaptureScreenshot)
    SCRIPTINGAPI_STACK_CHECK(CaptureScreenshot)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(CaptureScreenshot)
    
    #if SUPPORT_REPRODUCE_LOG
    		CaptureScreenshotReproduction(true);
    #else
    		RaiseMonoException("This method only works with internal development builds.");
    #endif
    	
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_LayerMask_LayerToName()
{
    scripting_add_internal_call( "UnityEngine.LayerMask::LayerToName" , (gpointer)& LayerMask_CUSTOM_LayerToName );
}

void Register_UnityEngine_LayerMask_NameToLayer()
{
    scripting_add_internal_call( "UnityEngine.LayerMask::NameToLayer" , (gpointer)& LayerMask_CUSTOM_NameToLayer );
}

#if ENABLE_AUDIO_FMOD
void Register_UnityEngine_AudioSettings_get_driverCaps()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::get_driverCaps" , (gpointer)& AudioSettings_Get_Custom_PropDriverCaps );
}

void Register_UnityEngine_AudioSettings_get_speakerMode()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::get_speakerMode" , (gpointer)& AudioSettings_Get_Custom_PropSpeakerMode );
}

void Register_UnityEngine_AudioSettings_set_speakerMode()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::set_speakerMode" , (gpointer)& AudioSettings_Set_Custom_PropSpeakerMode );
}

void Register_UnityEngine_AudioSettings_get_dspTime()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::get_dspTime" , (gpointer)& AudioSettings_Get_Custom_PropDspTime );
}

void Register_UnityEngine_AudioSettings_get_outputSampleRate()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::get_outputSampleRate" , (gpointer)& AudioSettings_Get_Custom_PropOutputSampleRate );
}

void Register_UnityEngine_AudioSettings_set_outputSampleRate()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::set_outputSampleRate" , (gpointer)& AudioSettings_Set_Custom_PropOutputSampleRate );
}

void Register_UnityEngine_AudioSettings_SetDSPBufferSize()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::SetDSPBufferSize" , (gpointer)& AudioSettings_CUSTOM_SetDSPBufferSize );
}

void Register_UnityEngine_AudioSettings_GetDSPBufferSize()
{
    scripting_add_internal_call( "UnityEngine.AudioSettings::GetDSPBufferSize" , (gpointer)& AudioSettings_CUSTOM_GetDSPBufferSize );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioClip_get_length()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::get_length" , (gpointer)& AudioClip_Get_Custom_PropLength );
}

void Register_UnityEngine_AudioClip_get_samples()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::get_samples" , (gpointer)& AudioClip_Get_Custom_PropSamples );
}

void Register_UnityEngine_AudioClip_get_channels()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::get_channels" , (gpointer)& AudioClip_Get_Custom_PropChannels );
}

void Register_UnityEngine_AudioClip_get_frequency()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::get_frequency" , (gpointer)& AudioClip_Get_Custom_PropFrequency );
}

void Register_UnityEngine_AudioClip_get_isReadyToPlay()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::get_isReadyToPlay" , (gpointer)& AudioClip_Get_Custom_PropIsReadyToPlay );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
void Register_UnityEngine_AudioClip_GetData()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::GetData" , (gpointer)& AudioClip_CUSTOM_GetData );
}

void Register_UnityEngine_AudioClip_SetData()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::SetData" , (gpointer)& AudioClip_CUSTOM_SetData );
}

void Register_UnityEngine_AudioClip_Construct_Internal()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::Construct_Internal" , (gpointer)& AudioClip_CUSTOM_Construct_Internal );
}

void Register_UnityEngine_AudioClip_Init_Internal()
{
    scripting_add_internal_call( "UnityEngine.AudioClip::Init_Internal" , (gpointer)& AudioClip_CUSTOM_Init_Internal );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioListener_get_volume()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::get_volume" , (gpointer)& AudioListener_Get_Custom_PropVolume );
}

void Register_UnityEngine_AudioListener_set_volume()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::set_volume" , (gpointer)& AudioListener_Set_Custom_PropVolume );
}

void Register_UnityEngine_AudioListener_get_pause()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::get_pause" , (gpointer)& AudioListener_Get_Custom_PropPause );
}

void Register_UnityEngine_AudioListener_set_pause()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::set_pause" , (gpointer)& AudioListener_Set_Custom_PropPause );
}

void Register_UnityEngine_AudioListener_get_velocityUpdateMode()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::get_velocityUpdateMode" , (gpointer)& AudioListener_Get_Custom_PropVelocityUpdateMode );
}

void Register_UnityEngine_AudioListener_set_velocityUpdateMode()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::set_velocityUpdateMode" , (gpointer)& AudioListener_Set_Custom_PropVelocityUpdateMode );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
void Register_UnityEngine_AudioListener_GetOutputDataHelper()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::GetOutputDataHelper" , (gpointer)& AudioListener_CUSTOM_GetOutputDataHelper );
}

void Register_UnityEngine_AudioListener_GetSpectrumDataHelper()
{
    scripting_add_internal_call( "UnityEngine.AudioListener::GetSpectrumDataHelper" , (gpointer)& AudioListener_CUSTOM_GetSpectrumDataHelper );
}

#endif
#if ENABLE_MICROPHONE
void Register_UnityEngine_Microphone_Start()
{
    scripting_add_internal_call( "UnityEngine.Microphone::Start" , (gpointer)& Microphone_CUSTOM_Start );
}

void Register_UnityEngine_Microphone_End()
{
    scripting_add_internal_call( "UnityEngine.Microphone::End" , (gpointer)& Microphone_CUSTOM_End );
}

void Register_UnityEngine_Microphone_get_devices()
{
    scripting_add_internal_call( "UnityEngine.Microphone::get_devices" , (gpointer)& Microphone_Get_Custom_PropDevices );
}

void Register_UnityEngine_Microphone_IsRecording()
{
    scripting_add_internal_call( "UnityEngine.Microphone::IsRecording" , (gpointer)& Microphone_CUSTOM_IsRecording );
}

void Register_UnityEngine_Microphone_GetPosition()
{
    scripting_add_internal_call( "UnityEngine.Microphone::GetPosition" , (gpointer)& Microphone_CUSTOM_GetPosition );
}

void Register_UnityEngine_Microphone_GetDeviceCaps()
{
    scripting_add_internal_call( "UnityEngine.Microphone::GetDeviceCaps" , (gpointer)& Microphone_CUSTOM_GetDeviceCaps );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioSource_get_volume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_volume" , (gpointer)& AudioSource_Get_Custom_PropVolume );
}

void Register_UnityEngine_AudioSource_set_volume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_volume" , (gpointer)& AudioSource_Set_Custom_PropVolume );
}

void Register_UnityEngine_AudioSource_get_pitch()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_pitch" , (gpointer)& AudioSource_Get_Custom_PropPitch );
}

void Register_UnityEngine_AudioSource_set_pitch()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_pitch" , (gpointer)& AudioSource_Set_Custom_PropPitch );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
void Register_UnityEngine_AudioSource_get_time()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_time" , (gpointer)& AudioSource_Get_Custom_PropTime );
}

void Register_UnityEngine_AudioSource_set_time()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_time" , (gpointer)& AudioSource_Set_Custom_PropTime );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
void Register_UnityEngine_AudioSource_get_timeSamples()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_timeSamples" , (gpointer)& AudioSource_Get_Custom_PropTimeSamples );
}

void Register_UnityEngine_AudioSource_set_timeSamples()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_timeSamples" , (gpointer)& AudioSource_Set_Custom_PropTimeSamples );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioSource_get_clip()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_clip" , (gpointer)& AudioSource_Get_Custom_PropClip );
}

void Register_UnityEngine_AudioSource_set_clip()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_clip" , (gpointer)& AudioSource_Set_Custom_PropClip );
}

void Register_UnityEngine_AudioSource_Play()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::Play" , (gpointer)& AudioSource_CUSTOM_Play );
}

void Register_UnityEngine_AudioSource_PlayDelayed()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::PlayDelayed" , (gpointer)& AudioSource_CUSTOM_PlayDelayed );
}

void Register_UnityEngine_AudioSource_PlayScheduled()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::PlayScheduled" , (gpointer)& AudioSource_CUSTOM_PlayScheduled );
}

void Register_UnityEngine_AudioSource_SetScheduledStartTime()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::SetScheduledStartTime" , (gpointer)& AudioSource_CUSTOM_SetScheduledStartTime );
}

void Register_UnityEngine_AudioSource_SetScheduledEndTime()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::SetScheduledEndTime" , (gpointer)& AudioSource_CUSTOM_SetScheduledEndTime );
}

void Register_UnityEngine_AudioSource_Stop()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::Stop" , (gpointer)& AudioSource_CUSTOM_Stop );
}

void Register_UnityEngine_AudioSource_INTERNAL_CALL_Pause()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::INTERNAL_CALL_Pause" , (gpointer)& AudioSource_CUSTOM_INTERNAL_CALL_Pause );
}

void Register_UnityEngine_AudioSource_get_isPlaying()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_isPlaying" , (gpointer)& AudioSource_Get_Custom_PropIsPlaying );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
void Register_UnityEngine_AudioSource_PlayOneShot()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::PlayOneShot" , (gpointer)& AudioSource_CUSTOM_PlayOneShot );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioSource_get_loop()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_loop" , (gpointer)& AudioSource_Get_Custom_PropLoop );
}

void Register_UnityEngine_AudioSource_set_loop()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_loop" , (gpointer)& AudioSource_Set_Custom_PropLoop );
}

void Register_UnityEngine_AudioSource_get_ignoreListenerVolume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_ignoreListenerVolume" , (gpointer)& AudioSource_Get_Custom_PropIgnoreListenerVolume );
}

void Register_UnityEngine_AudioSource_set_ignoreListenerVolume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_ignoreListenerVolume" , (gpointer)& AudioSource_Set_Custom_PropIgnoreListenerVolume );
}

void Register_UnityEngine_AudioSource_get_playOnAwake()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_playOnAwake" , (gpointer)& AudioSource_Get_Custom_PropPlayOnAwake );
}

void Register_UnityEngine_AudioSource_set_playOnAwake()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_playOnAwake" , (gpointer)& AudioSource_Set_Custom_PropPlayOnAwake );
}

void Register_UnityEngine_AudioSource_get_ignoreListenerPause()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_ignoreListenerPause" , (gpointer)& AudioSource_Get_Custom_PropIgnoreListenerPause );
}

void Register_UnityEngine_AudioSource_set_ignoreListenerPause()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_ignoreListenerPause" , (gpointer)& AudioSource_Set_Custom_PropIgnoreListenerPause );
}

void Register_UnityEngine_AudioSource_get_velocityUpdateMode()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_velocityUpdateMode" , (gpointer)& AudioSource_Get_Custom_PropVelocityUpdateMode );
}

void Register_UnityEngine_AudioSource_set_velocityUpdateMode()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_velocityUpdateMode" , (gpointer)& AudioSource_Set_Custom_PropVelocityUpdateMode );
}

void Register_UnityEngine_AudioSource_get_panLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_panLevel" , (gpointer)& AudioSource_Get_Custom_PropPanLevel );
}

void Register_UnityEngine_AudioSource_set_panLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_panLevel" , (gpointer)& AudioSource_Set_Custom_PropPanLevel );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
void Register_UnityEngine_AudioSource_get_bypassEffects()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_bypassEffects" , (gpointer)& AudioSource_Get_Custom_PropBypassEffects );
}

void Register_UnityEngine_AudioSource_set_bypassEffects()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_bypassEffects" , (gpointer)& AudioSource_Set_Custom_PropBypassEffects );
}

void Register_UnityEngine_AudioSource_get_bypassListenerEffects()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_bypassListenerEffects" , (gpointer)& AudioSource_Get_Custom_PropBypassListenerEffects );
}

void Register_UnityEngine_AudioSource_set_bypassListenerEffects()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_bypassListenerEffects" , (gpointer)& AudioSource_Set_Custom_PropBypassListenerEffects );
}

void Register_UnityEngine_AudioSource_get_bypassReverbZones()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_bypassReverbZones" , (gpointer)& AudioSource_Get_Custom_PropBypassReverbZones );
}

void Register_UnityEngine_AudioSource_set_bypassReverbZones()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_bypassReverbZones" , (gpointer)& AudioSource_Set_Custom_PropBypassReverbZones );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioSource_get_dopplerLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_dopplerLevel" , (gpointer)& AudioSource_Get_Custom_PropDopplerLevel );
}

void Register_UnityEngine_AudioSource_set_dopplerLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_dopplerLevel" , (gpointer)& AudioSource_Set_Custom_PropDopplerLevel );
}

void Register_UnityEngine_AudioSource_get_spread()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_spread" , (gpointer)& AudioSource_Get_Custom_PropSpread );
}

void Register_UnityEngine_AudioSource_set_spread()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_spread" , (gpointer)& AudioSource_Set_Custom_PropSpread );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
void Register_UnityEngine_AudioSource_get_priority()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_priority" , (gpointer)& AudioSource_Get_Custom_PropPriority );
}

void Register_UnityEngine_AudioSource_set_priority()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_priority" , (gpointer)& AudioSource_Set_Custom_PropPriority );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioSource_get_mute()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_mute" , (gpointer)& AudioSource_Get_Custom_PropMute );
}

void Register_UnityEngine_AudioSource_set_mute()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_mute" , (gpointer)& AudioSource_Set_Custom_PropMute );
}

void Register_UnityEngine_AudioSource_get_minDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_minDistance" , (gpointer)& AudioSource_Get_Custom_PropMinDistance );
}

void Register_UnityEngine_AudioSource_set_minDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_minDistance" , (gpointer)& AudioSource_Set_Custom_PropMinDistance );
}

void Register_UnityEngine_AudioSource_get_maxDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_maxDistance" , (gpointer)& AudioSource_Get_Custom_PropMaxDistance );
}

void Register_UnityEngine_AudioSource_set_maxDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_maxDistance" , (gpointer)& AudioSource_Set_Custom_PropMaxDistance );
}

void Register_UnityEngine_AudioSource_get_pan()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_pan" , (gpointer)& AudioSource_Get_Custom_PropPan );
}

void Register_UnityEngine_AudioSource_set_pan()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_pan" , (gpointer)& AudioSource_Set_Custom_PropPan );
}

void Register_UnityEngine_AudioSource_get_rolloffMode()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_rolloffMode" , (gpointer)& AudioSource_Get_Custom_PropRolloffMode );
}

void Register_UnityEngine_AudioSource_set_rolloffMode()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_rolloffMode" , (gpointer)& AudioSource_Set_Custom_PropRolloffMode );
}

void Register_UnityEngine_AudioSource_GetOutputDataHelper()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::GetOutputDataHelper" , (gpointer)& AudioSource_CUSTOM_GetOutputDataHelper );
}

#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
void Register_UnityEngine_AudioSource_GetSpectrumDataHelper()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::GetSpectrumDataHelper" , (gpointer)& AudioSource_CUSTOM_GetSpectrumDataHelper );
}

#endif
#if ENABLE_AUDIO
void Register_UnityEngine_AudioSource_get_minVolume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_minVolume" , (gpointer)& AudioSource_Get_Custom_PropMinVolume );
}

void Register_UnityEngine_AudioSource_set_minVolume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_minVolume" , (gpointer)& AudioSource_Set_Custom_PropMinVolume );
}

void Register_UnityEngine_AudioSource_get_maxVolume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_maxVolume" , (gpointer)& AudioSource_Get_Custom_PropMaxVolume );
}

void Register_UnityEngine_AudioSource_set_maxVolume()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_maxVolume" , (gpointer)& AudioSource_Set_Custom_PropMaxVolume );
}

void Register_UnityEngine_AudioSource_get_rolloffFactor()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::get_rolloffFactor" , (gpointer)& AudioSource_Get_Custom_PropRolloffFactor );
}

void Register_UnityEngine_AudioSource_set_rolloffFactor()
{
    scripting_add_internal_call( "UnityEngine.AudioSource::set_rolloffFactor" , (gpointer)& AudioSource_Set_Custom_PropRolloffFactor );
}

#endif
#if ENABLE_AUDIO_FMOD
void Register_UnityEngine_AudioReverbZone_get_minDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_minDistance" , (gpointer)& AudioReverbZone_Get_Custom_PropMinDistance );
}

void Register_UnityEngine_AudioReverbZone_set_minDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_minDistance" , (gpointer)& AudioReverbZone_Set_Custom_PropMinDistance );
}

void Register_UnityEngine_AudioReverbZone_get_maxDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_maxDistance" , (gpointer)& AudioReverbZone_Get_Custom_PropMaxDistance );
}

void Register_UnityEngine_AudioReverbZone_set_maxDistance()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_maxDistance" , (gpointer)& AudioReverbZone_Set_Custom_PropMaxDistance );
}

void Register_UnityEngine_AudioReverbZone_get_reverbPreset()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_reverbPreset" , (gpointer)& AudioReverbZone_Get_Custom_PropReverbPreset );
}

void Register_UnityEngine_AudioReverbZone_set_reverbPreset()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_reverbPreset" , (gpointer)& AudioReverbZone_Set_Custom_PropReverbPreset );
}

void Register_UnityEngine_AudioReverbZone_get_room()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_room" , (gpointer)& AudioReverbZone_Get_Custom_PropRoom );
}

void Register_UnityEngine_AudioReverbZone_set_room()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_room" , (gpointer)& AudioReverbZone_Set_Custom_PropRoom );
}

void Register_UnityEngine_AudioReverbZone_get_roomHF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_roomHF" , (gpointer)& AudioReverbZone_Get_Custom_PropRoomHF );
}

void Register_UnityEngine_AudioReverbZone_set_roomHF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_roomHF" , (gpointer)& AudioReverbZone_Set_Custom_PropRoomHF );
}

void Register_UnityEngine_AudioReverbZone_get_roomLF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_roomLF" , (gpointer)& AudioReverbZone_Get_Custom_PropRoomLF );
}

void Register_UnityEngine_AudioReverbZone_set_roomLF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_roomLF" , (gpointer)& AudioReverbZone_Set_Custom_PropRoomLF );
}

void Register_UnityEngine_AudioReverbZone_get_decayTime()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_decayTime" , (gpointer)& AudioReverbZone_Get_Custom_PropDecayTime );
}

void Register_UnityEngine_AudioReverbZone_set_decayTime()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_decayTime" , (gpointer)& AudioReverbZone_Set_Custom_PropDecayTime );
}

void Register_UnityEngine_AudioReverbZone_get_decayHFRatio()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_decayHFRatio" , (gpointer)& AudioReverbZone_Get_Custom_PropDecayHFRatio );
}

void Register_UnityEngine_AudioReverbZone_set_decayHFRatio()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_decayHFRatio" , (gpointer)& AudioReverbZone_Set_Custom_PropDecayHFRatio );
}

void Register_UnityEngine_AudioReverbZone_get_reflections()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_reflections" , (gpointer)& AudioReverbZone_Get_Custom_PropReflections );
}

void Register_UnityEngine_AudioReverbZone_set_reflections()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_reflections" , (gpointer)& AudioReverbZone_Set_Custom_PropReflections );
}

void Register_UnityEngine_AudioReverbZone_get_reflectionsDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_reflectionsDelay" , (gpointer)& AudioReverbZone_Get_Custom_PropReflectionsDelay );
}

void Register_UnityEngine_AudioReverbZone_set_reflectionsDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_reflectionsDelay" , (gpointer)& AudioReverbZone_Set_Custom_PropReflectionsDelay );
}

void Register_UnityEngine_AudioReverbZone_get_reverb()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_reverb" , (gpointer)& AudioReverbZone_Get_Custom_PropReverb );
}

void Register_UnityEngine_AudioReverbZone_set_reverb()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_reverb" , (gpointer)& AudioReverbZone_Set_Custom_PropReverb );
}

void Register_UnityEngine_AudioReverbZone_get_reverbDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_reverbDelay" , (gpointer)& AudioReverbZone_Get_Custom_PropReverbDelay );
}

void Register_UnityEngine_AudioReverbZone_set_reverbDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_reverbDelay" , (gpointer)& AudioReverbZone_Set_Custom_PropReverbDelay );
}

void Register_UnityEngine_AudioReverbZone_get_HFReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_HFReference" , (gpointer)& AudioReverbZone_Get_Custom_PropHFReference );
}

void Register_UnityEngine_AudioReverbZone_set_HFReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_HFReference" , (gpointer)& AudioReverbZone_Set_Custom_PropHFReference );
}

void Register_UnityEngine_AudioReverbZone_get_LFReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_LFReference" , (gpointer)& AudioReverbZone_Get_Custom_PropLFReference );
}

void Register_UnityEngine_AudioReverbZone_set_LFReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_LFReference" , (gpointer)& AudioReverbZone_Set_Custom_PropLFReference );
}

void Register_UnityEngine_AudioReverbZone_get_roomRolloffFactor()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_roomRolloffFactor" , (gpointer)& AudioReverbZone_Get_Custom_PropRoomRolloffFactor );
}

void Register_UnityEngine_AudioReverbZone_set_roomRolloffFactor()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_roomRolloffFactor" , (gpointer)& AudioReverbZone_Set_Custom_PropRoomRolloffFactor );
}

void Register_UnityEngine_AudioReverbZone_get_diffusion()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_diffusion" , (gpointer)& AudioReverbZone_Get_Custom_PropDiffusion );
}

void Register_UnityEngine_AudioReverbZone_set_diffusion()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_diffusion" , (gpointer)& AudioReverbZone_Set_Custom_PropDiffusion );
}

void Register_UnityEngine_AudioReverbZone_get_density()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::get_density" , (gpointer)& AudioReverbZone_Get_Custom_PropDensity );
}

void Register_UnityEngine_AudioReverbZone_set_density()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbZone::set_density" , (gpointer)& AudioReverbZone_Set_Custom_PropDensity );
}

void Register_UnityEngine_AudioLowPassFilter_get_cutoffFrequency()
{
    scripting_add_internal_call( "UnityEngine.AudioLowPassFilter::get_cutoffFrequency" , (gpointer)& AudioLowPassFilter_Get_Custom_PropCutoffFrequency );
}

void Register_UnityEngine_AudioLowPassFilter_set_cutoffFrequency()
{
    scripting_add_internal_call( "UnityEngine.AudioLowPassFilter::set_cutoffFrequency" , (gpointer)& AudioLowPassFilter_Set_Custom_PropCutoffFrequency );
}

void Register_UnityEngine_AudioLowPassFilter_get_lowpassResonaceQ()
{
    scripting_add_internal_call( "UnityEngine.AudioLowPassFilter::get_lowpassResonaceQ" , (gpointer)& AudioLowPassFilter_Get_Custom_PropLowpassResonaceQ );
}

void Register_UnityEngine_AudioLowPassFilter_set_lowpassResonaceQ()
{
    scripting_add_internal_call( "UnityEngine.AudioLowPassFilter::set_lowpassResonaceQ" , (gpointer)& AudioLowPassFilter_Set_Custom_PropLowpassResonaceQ );
}

void Register_UnityEngine_AudioHighPassFilter_get_cutoffFrequency()
{
    scripting_add_internal_call( "UnityEngine.AudioHighPassFilter::get_cutoffFrequency" , (gpointer)& AudioHighPassFilter_Get_Custom_PropCutoffFrequency );
}

void Register_UnityEngine_AudioHighPassFilter_set_cutoffFrequency()
{
    scripting_add_internal_call( "UnityEngine.AudioHighPassFilter::set_cutoffFrequency" , (gpointer)& AudioHighPassFilter_Set_Custom_PropCutoffFrequency );
}

void Register_UnityEngine_AudioHighPassFilter_get_highpassResonaceQ()
{
    scripting_add_internal_call( "UnityEngine.AudioHighPassFilter::get_highpassResonaceQ" , (gpointer)& AudioHighPassFilter_Get_Custom_PropHighpassResonaceQ );
}

void Register_UnityEngine_AudioHighPassFilter_set_highpassResonaceQ()
{
    scripting_add_internal_call( "UnityEngine.AudioHighPassFilter::set_highpassResonaceQ" , (gpointer)& AudioHighPassFilter_Set_Custom_PropHighpassResonaceQ );
}

void Register_UnityEngine_AudioDistortionFilter_get_distortionLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioDistortionFilter::get_distortionLevel" , (gpointer)& AudioDistortionFilter_Get_Custom_PropDistortionLevel );
}

void Register_UnityEngine_AudioDistortionFilter_set_distortionLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioDistortionFilter::set_distortionLevel" , (gpointer)& AudioDistortionFilter_Set_Custom_PropDistortionLevel );
}

void Register_UnityEngine_AudioEchoFilter_get_delay()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::get_delay" , (gpointer)& AudioEchoFilter_Get_Custom_PropDelay );
}

void Register_UnityEngine_AudioEchoFilter_set_delay()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::set_delay" , (gpointer)& AudioEchoFilter_Set_Custom_PropDelay );
}

void Register_UnityEngine_AudioEchoFilter_get_decayRatio()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::get_decayRatio" , (gpointer)& AudioEchoFilter_Get_Custom_PropDecayRatio );
}

void Register_UnityEngine_AudioEchoFilter_set_decayRatio()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::set_decayRatio" , (gpointer)& AudioEchoFilter_Set_Custom_PropDecayRatio );
}

void Register_UnityEngine_AudioEchoFilter_get_dryMix()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::get_dryMix" , (gpointer)& AudioEchoFilter_Get_Custom_PropDryMix );
}

void Register_UnityEngine_AudioEchoFilter_set_dryMix()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::set_dryMix" , (gpointer)& AudioEchoFilter_Set_Custom_PropDryMix );
}

void Register_UnityEngine_AudioEchoFilter_get_wetMix()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::get_wetMix" , (gpointer)& AudioEchoFilter_Get_Custom_PropWetMix );
}

void Register_UnityEngine_AudioEchoFilter_set_wetMix()
{
    scripting_add_internal_call( "UnityEngine.AudioEchoFilter::set_wetMix" , (gpointer)& AudioEchoFilter_Set_Custom_PropWetMix );
}

void Register_UnityEngine_AudioChorusFilter_get_dryMix()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_dryMix" , (gpointer)& AudioChorusFilter_Get_Custom_PropDryMix );
}

void Register_UnityEngine_AudioChorusFilter_set_dryMix()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_dryMix" , (gpointer)& AudioChorusFilter_Set_Custom_PropDryMix );
}

void Register_UnityEngine_AudioChorusFilter_get_wetMix1()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_wetMix1" , (gpointer)& AudioChorusFilter_Get_Custom_PropWetMix1 );
}

void Register_UnityEngine_AudioChorusFilter_set_wetMix1()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_wetMix1" , (gpointer)& AudioChorusFilter_Set_Custom_PropWetMix1 );
}

void Register_UnityEngine_AudioChorusFilter_get_wetMix2()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_wetMix2" , (gpointer)& AudioChorusFilter_Get_Custom_PropWetMix2 );
}

void Register_UnityEngine_AudioChorusFilter_set_wetMix2()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_wetMix2" , (gpointer)& AudioChorusFilter_Set_Custom_PropWetMix2 );
}

void Register_UnityEngine_AudioChorusFilter_get_wetMix3()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_wetMix3" , (gpointer)& AudioChorusFilter_Get_Custom_PropWetMix3 );
}

void Register_UnityEngine_AudioChorusFilter_set_wetMix3()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_wetMix3" , (gpointer)& AudioChorusFilter_Set_Custom_PropWetMix3 );
}

void Register_UnityEngine_AudioChorusFilter_get_delay()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_delay" , (gpointer)& AudioChorusFilter_Get_Custom_PropDelay );
}

void Register_UnityEngine_AudioChorusFilter_set_delay()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_delay" , (gpointer)& AudioChorusFilter_Set_Custom_PropDelay );
}

void Register_UnityEngine_AudioChorusFilter_get_rate()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_rate" , (gpointer)& AudioChorusFilter_Get_Custom_PropRate );
}

void Register_UnityEngine_AudioChorusFilter_set_rate()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_rate" , (gpointer)& AudioChorusFilter_Set_Custom_PropRate );
}

void Register_UnityEngine_AudioChorusFilter_get_depth()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_depth" , (gpointer)& AudioChorusFilter_Get_Custom_PropDepth );
}

void Register_UnityEngine_AudioChorusFilter_set_depth()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_depth" , (gpointer)& AudioChorusFilter_Set_Custom_PropDepth );
}

void Register_UnityEngine_AudioChorusFilter_get_feedback()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::get_feedback" , (gpointer)& AudioChorusFilter_Get_Custom_PropFeedback );
}

void Register_UnityEngine_AudioChorusFilter_set_feedback()
{
    scripting_add_internal_call( "UnityEngine.AudioChorusFilter::set_feedback" , (gpointer)& AudioChorusFilter_Set_Custom_PropFeedback );
}

void Register_UnityEngine_AudioReverbFilter_get_reverbPreset()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_reverbPreset" , (gpointer)& AudioReverbFilter_Get_Custom_PropReverbPreset );
}

void Register_UnityEngine_AudioReverbFilter_set_reverbPreset()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_reverbPreset" , (gpointer)& AudioReverbFilter_Set_Custom_PropReverbPreset );
}

void Register_UnityEngine_AudioReverbFilter_get_dryLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_dryLevel" , (gpointer)& AudioReverbFilter_Get_Custom_PropDryLevel );
}

void Register_UnityEngine_AudioReverbFilter_set_dryLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_dryLevel" , (gpointer)& AudioReverbFilter_Set_Custom_PropDryLevel );
}

void Register_UnityEngine_AudioReverbFilter_get_room()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_room" , (gpointer)& AudioReverbFilter_Get_Custom_PropRoom );
}

void Register_UnityEngine_AudioReverbFilter_set_room()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_room" , (gpointer)& AudioReverbFilter_Set_Custom_PropRoom );
}

void Register_UnityEngine_AudioReverbFilter_get_roomHF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_roomHF" , (gpointer)& AudioReverbFilter_Get_Custom_PropRoomHF );
}

void Register_UnityEngine_AudioReverbFilter_set_roomHF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_roomHF" , (gpointer)& AudioReverbFilter_Set_Custom_PropRoomHF );
}

void Register_UnityEngine_AudioReverbFilter_get_roomRolloff()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_roomRolloff" , (gpointer)& AudioReverbFilter_Get_Custom_PropRoomRolloff );
}

void Register_UnityEngine_AudioReverbFilter_set_roomRolloff()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_roomRolloff" , (gpointer)& AudioReverbFilter_Set_Custom_PropRoomRolloff );
}

void Register_UnityEngine_AudioReverbFilter_get_decayTime()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_decayTime" , (gpointer)& AudioReverbFilter_Get_Custom_PropDecayTime );
}

void Register_UnityEngine_AudioReverbFilter_set_decayTime()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_decayTime" , (gpointer)& AudioReverbFilter_Set_Custom_PropDecayTime );
}

void Register_UnityEngine_AudioReverbFilter_get_decayHFRatio()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_decayHFRatio" , (gpointer)& AudioReverbFilter_Get_Custom_PropDecayHFRatio );
}

void Register_UnityEngine_AudioReverbFilter_set_decayHFRatio()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_decayHFRatio" , (gpointer)& AudioReverbFilter_Set_Custom_PropDecayHFRatio );
}

void Register_UnityEngine_AudioReverbFilter_get_reflectionsLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_reflectionsLevel" , (gpointer)& AudioReverbFilter_Get_Custom_PropReflectionsLevel );
}

void Register_UnityEngine_AudioReverbFilter_set_reflectionsLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_reflectionsLevel" , (gpointer)& AudioReverbFilter_Set_Custom_PropReflectionsLevel );
}

void Register_UnityEngine_AudioReverbFilter_get_reflectionsDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_reflectionsDelay" , (gpointer)& AudioReverbFilter_Get_Custom_PropReflectionsDelay );
}

void Register_UnityEngine_AudioReverbFilter_set_reflectionsDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_reflectionsDelay" , (gpointer)& AudioReverbFilter_Set_Custom_PropReflectionsDelay );
}

void Register_UnityEngine_AudioReverbFilter_get_reverbLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_reverbLevel" , (gpointer)& AudioReverbFilter_Get_Custom_PropReverbLevel );
}

void Register_UnityEngine_AudioReverbFilter_set_reverbLevel()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_reverbLevel" , (gpointer)& AudioReverbFilter_Set_Custom_PropReverbLevel );
}

void Register_UnityEngine_AudioReverbFilter_get_reverbDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_reverbDelay" , (gpointer)& AudioReverbFilter_Get_Custom_PropReverbDelay );
}

void Register_UnityEngine_AudioReverbFilter_set_reverbDelay()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_reverbDelay" , (gpointer)& AudioReverbFilter_Set_Custom_PropReverbDelay );
}

void Register_UnityEngine_AudioReverbFilter_get_diffusion()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_diffusion" , (gpointer)& AudioReverbFilter_Get_Custom_PropDiffusion );
}

void Register_UnityEngine_AudioReverbFilter_set_diffusion()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_diffusion" , (gpointer)& AudioReverbFilter_Set_Custom_PropDiffusion );
}

void Register_UnityEngine_AudioReverbFilter_get_density()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_density" , (gpointer)& AudioReverbFilter_Get_Custom_PropDensity );
}

void Register_UnityEngine_AudioReverbFilter_set_density()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_density" , (gpointer)& AudioReverbFilter_Set_Custom_PropDensity );
}

void Register_UnityEngine_AudioReverbFilter_get_hfReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_hfReference" , (gpointer)& AudioReverbFilter_Get_Custom_PropHfReference );
}

void Register_UnityEngine_AudioReverbFilter_set_hfReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_hfReference" , (gpointer)& AudioReverbFilter_Set_Custom_PropHfReference );
}

void Register_UnityEngine_AudioReverbFilter_get_roomLF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_roomLF" , (gpointer)& AudioReverbFilter_Get_Custom_PropRoomLF );
}

void Register_UnityEngine_AudioReverbFilter_set_roomLF()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_roomLF" , (gpointer)& AudioReverbFilter_Set_Custom_PropRoomLF );
}

void Register_UnityEngine_AudioReverbFilter_get_lFReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::get_lFReference" , (gpointer)& AudioReverbFilter_Get_Custom_PropLFReference );
}

void Register_UnityEngine_AudioReverbFilter_set_lFReference()
{
    scripting_add_internal_call( "UnityEngine.AudioReverbFilter::set_lFReference" , (gpointer)& AudioReverbFilter_Set_Custom_PropLFReference );
}

#endif
void Register_UnityEngine_PlayerPrefs_TrySetInt()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::TrySetInt" , (gpointer)& PlayerPrefs_CUSTOM_TrySetInt );
}

void Register_UnityEngine_PlayerPrefs_TrySetFloat()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::TrySetFloat" , (gpointer)& PlayerPrefs_CUSTOM_TrySetFloat );
}

void Register_UnityEngine_PlayerPrefs_TrySetSetString()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::TrySetSetString" , (gpointer)& PlayerPrefs_CUSTOM_TrySetSetString );
}

void Register_UnityEngine_PlayerPrefs_GetInt()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::GetInt" , (gpointer)& PlayerPrefs_CUSTOM_GetInt );
}

void Register_UnityEngine_PlayerPrefs_GetFloat()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::GetFloat" , (gpointer)& PlayerPrefs_CUSTOM_GetFloat );
}

void Register_UnityEngine_PlayerPrefs_GetString()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::GetString" , (gpointer)& PlayerPrefs_CUSTOM_GetString );
}

void Register_UnityEngine_PlayerPrefs_HasKey()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::HasKey" , (gpointer)& PlayerPrefs_CUSTOM_HasKey );
}

void Register_UnityEngine_PlayerPrefs_DeleteKey()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::DeleteKey" , (gpointer)& PlayerPrefs_CUSTOM_DeleteKey );
}

void Register_UnityEngine_PlayerPrefs_DeleteAll()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::DeleteAll" , (gpointer)& PlayerPrefs_CUSTOM_DeleteAll );
}

void Register_UnityEngine_PlayerPrefs_Save()
{
    scripting_add_internal_call( "UnityEngine.PlayerPrefs::Save" , (gpointer)& PlayerPrefs_CUSTOM_Save );
}

void Register_UnityEngine_SystemInfo_get_operatingSystem()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_operatingSystem" , (gpointer)& SystemInfo_Get_Custom_PropOperatingSystem );
}

void Register_UnityEngine_SystemInfo_get_processorType()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_processorType" , (gpointer)& SystemInfo_Get_Custom_PropProcessorType );
}

void Register_UnityEngine_SystemInfo_get_processorCount()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_processorCount" , (gpointer)& SystemInfo_Get_Custom_PropProcessorCount );
}

void Register_UnityEngine_SystemInfo_get_systemMemorySize()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_systemMemorySize" , (gpointer)& SystemInfo_Get_Custom_PropSystemMemorySize );
}

void Register_UnityEngine_SystemInfo_get_graphicsMemorySize()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsMemorySize" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsMemorySize );
}

void Register_UnityEngine_SystemInfo_get_graphicsDeviceName()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsDeviceName" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsDeviceName );
}

void Register_UnityEngine_SystemInfo_get_graphicsDeviceVendor()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsDeviceVendor" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsDeviceVendor );
}

void Register_UnityEngine_SystemInfo_get_graphicsDeviceID()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsDeviceID" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsDeviceID );
}

void Register_UnityEngine_SystemInfo_get_graphicsDeviceVendorID()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsDeviceVendorID" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsDeviceVendorID );
}

void Register_UnityEngine_SystemInfo_get_graphicsDeviceVersion()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsDeviceVersion" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsDeviceVersion );
}

void Register_UnityEngine_SystemInfo_get_graphicsShaderLevel()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsShaderLevel" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsShaderLevel );
}

void Register_UnityEngine_SystemInfo_get_graphicsPixelFillrate()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_graphicsPixelFillrate" , (gpointer)& SystemInfo_Get_Custom_PropGraphicsPixelFillrate );
}

void Register_UnityEngine_SystemInfo_get_supportsShadows()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsShadows" , (gpointer)& SystemInfo_Get_Custom_PropSupportsShadows );
}

void Register_UnityEngine_SystemInfo_get_supportsRenderTextures()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsRenderTextures" , (gpointer)& SystemInfo_Get_Custom_PropSupportsRenderTextures );
}

void Register_UnityEngine_SystemInfo_get_supportsImageEffects()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsImageEffects" , (gpointer)& SystemInfo_Get_Custom_PropSupportsImageEffects );
}

void Register_UnityEngine_SystemInfo_get_supports3DTextures()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supports3DTextures" , (gpointer)& SystemInfo_Get_Custom_PropSupports3DTextures );
}

void Register_UnityEngine_SystemInfo_get_supportsComputeShaders()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsComputeShaders" , (gpointer)& SystemInfo_Get_Custom_PropSupportsComputeShaders );
}

void Register_UnityEngine_SystemInfo_get_supportsInstancing()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsInstancing" , (gpointer)& SystemInfo_Get_Custom_PropSupportsInstancing );
}

void Register_UnityEngine_SystemInfo_get_supportedRenderTargetCount()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportedRenderTargetCount" , (gpointer)& SystemInfo_Get_Custom_PropSupportedRenderTargetCount );
}

void Register_UnityEngine_SystemInfo_get_supportsStencil()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsStencil" , (gpointer)& SystemInfo_Get_Custom_PropSupportsStencil );
}

void Register_UnityEngine_SystemInfo_get_supportsVertexPrograms()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsVertexPrograms" , (gpointer)& SystemInfo_Get_Custom_PropSupportsVertexPrograms );
}

void Register_UnityEngine_SystemInfo_SupportsRenderTextureFormat()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::SupportsRenderTextureFormat" , (gpointer)& SystemInfo_CUSTOM_SupportsRenderTextureFormat );
}

void Register_UnityEngine_SystemInfo_get_npotSupport()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_npotSupport" , (gpointer)& SystemInfo_Get_Custom_PropNpotSupport );
}

#if !UNITY_FLASH && !PLATFORM_WEBGL
void Register_UnityEngine_SystemInfo_get_deviceUniqueIdentifier()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_deviceUniqueIdentifier" , (gpointer)& SystemInfo_Get_Custom_PropDeviceUniqueIdentifier );
}

void Register_UnityEngine_SystemInfo_get_deviceName()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_deviceName" , (gpointer)& SystemInfo_Get_Custom_PropDeviceName );
}

void Register_UnityEngine_SystemInfo_get_deviceModel()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_deviceModel" , (gpointer)& SystemInfo_Get_Custom_PropDeviceModel );
}

#endif
void Register_UnityEngine_SystemInfo_get_supportsAccelerometer()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsAccelerometer" , (gpointer)& SystemInfo_Get_Custom_PropSupportsAccelerometer );
}

void Register_UnityEngine_SystemInfo_get_supportsGyroscope()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsGyroscope" , (gpointer)& SystemInfo_Get_Custom_PropSupportsGyroscope );
}

void Register_UnityEngine_SystemInfo_get_supportsLocationService()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsLocationService" , (gpointer)& SystemInfo_Get_Custom_PropSupportsLocationService );
}

void Register_UnityEngine_SystemInfo_get_supportsVibration()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_supportsVibration" , (gpointer)& SystemInfo_Get_Custom_PropSupportsVibration );
}

void Register_UnityEngine_SystemInfo_get_deviceType()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_deviceType" , (gpointer)& SystemInfo_Get_Custom_PropDeviceType );
}

void Register_UnityEngine_SystemInfo_get_maxTextureSize()
{
    scripting_add_internal_call( "UnityEngine.SystemInfo::get_maxTextureSize" , (gpointer)& SystemInfo_Get_Custom_PropMaxTextureSize );
}

void Register_UnityEngine_Coroutine_ReleaseCoroutine()
{
    scripting_add_internal_call( "UnityEngine.Coroutine::ReleaseCoroutine" , (gpointer)& Coroutine_CUSTOM_ReleaseCoroutine );
}

void Register_UnityEngine_ScriptableObject_Internal_CreateScriptableObject()
{
    scripting_add_internal_call( "UnityEngine.ScriptableObject::Internal_CreateScriptableObject" , (gpointer)& ScriptableObject_CUSTOM_Internal_CreateScriptableObject );
}

#if ENABLE_MONO
void Register_UnityEngine_ScriptableObject_INTERNAL_CALL_SetDirty()
{
    scripting_add_internal_call( "UnityEngine.ScriptableObject::INTERNAL_CALL_SetDirty" , (gpointer)& ScriptableObject_CUSTOM_INTERNAL_CALL_SetDirty );
}

#endif
void Register_UnityEngine_ScriptableObject_CreateInstance()
{
    scripting_add_internal_call( "UnityEngine.ScriptableObject::CreateInstance" , (gpointer)& ScriptableObject_CUSTOM_CreateInstance );
}

void Register_UnityEngine_ScriptableObject_CreateInstanceFromType()
{
    scripting_add_internal_call( "UnityEngine.ScriptableObject::CreateInstanceFromType" , (gpointer)& ScriptableObject_CUSTOM_CreateInstanceFromType );
}

void Register_UnityEngine_Resources_FindObjectsOfTypeAll()
{
    scripting_add_internal_call( "UnityEngine.Resources::FindObjectsOfTypeAll" , (gpointer)& Resources_CUSTOM_FindObjectsOfTypeAll );
}

void Register_UnityEngine_Resources_Load()
{
    scripting_add_internal_call( "UnityEngine.Resources::Load" , (gpointer)& Resources_CUSTOM_Load );
}

void Register_UnityEngine_Resources_LoadAll()
{
    scripting_add_internal_call( "UnityEngine.Resources::LoadAll" , (gpointer)& Resources_CUSTOM_LoadAll );
}

void Register_UnityEngine_Resources_GetBuiltinResource()
{
    scripting_add_internal_call( "UnityEngine.Resources::GetBuiltinResource" , (gpointer)& Resources_CUSTOM_GetBuiltinResource );
}

void Register_UnityEngine_Resources_LoadAssetAtPath()
{
    scripting_add_internal_call( "UnityEngine.Resources::LoadAssetAtPath" , (gpointer)& Resources_CUSTOM_LoadAssetAtPath );
}

void Register_UnityEngine_Resources_UnloadAsset()
{
    scripting_add_internal_call( "UnityEngine.Resources::UnloadAsset" , (gpointer)& Resources_CUSTOM_UnloadAsset );
}

void Register_UnityEngine_Resources_UnloadUnusedAssets()
{
    scripting_add_internal_call( "UnityEngine.Resources::UnloadUnusedAssets" , (gpointer)& Resources_CUSTOM_UnloadUnusedAssets );
}

#if ENABLE_WWW
void Register_UnityEngine_AssetBundleCreateRequest_get_assetBundle()
{
    scripting_add_internal_call( "UnityEngine.AssetBundleCreateRequest::get_assetBundle" , (gpointer)& AssetBundleCreateRequest_Get_Custom_PropAssetBundle );
}

void Register_UnityEngine_AssetBundle_CreateFromMemory()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::CreateFromMemory" , (gpointer)& AssetBundle_CUSTOM_CreateFromMemory );
}

#endif
void Register_UnityEngine_AssetBundle_CreateFromFile()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::CreateFromFile" , (gpointer)& AssetBundle_CUSTOM_CreateFromFile );
}

void Register_UnityEngine_AssetBundle_get_mainAsset()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::get_mainAsset" , (gpointer)& AssetBundle_Get_Custom_PropMainAsset );
}

void Register_UnityEngine_AssetBundle_Contains()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::Contains" , (gpointer)& AssetBundle_CUSTOM_Contains );
}

void Register_UnityEngine_AssetBundle_Load()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::Load" , (gpointer)& AssetBundle_CUSTOM_Load );
}

void Register_UnityEngine_AssetBundle_LoadAsync()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::LoadAsync" , (gpointer)& AssetBundle_CUSTOM_LoadAsync );
}

void Register_UnityEngine_AssetBundle_LoadAll()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::LoadAll" , (gpointer)& AssetBundle_CUSTOM_LoadAll );
}

void Register_UnityEngine_AssetBundle_Unload()
{
    scripting_add_internal_call( "UnityEngine.AssetBundle::Unload" , (gpointer)& AssetBundle_CUSTOM_Unload );
}

void Register_UnityEngine_Profiler_get_supported()
{
    scripting_add_internal_call( "UnityEngine.Profiler::get_supported" , (gpointer)& Profiler_Get_Custom_PropSupported );
}

void Register_UnityEngine_Profiler_get_logFile()
{
    scripting_add_internal_call( "UnityEngine.Profiler::get_logFile" , (gpointer)& Profiler_Get_Custom_PropLogFile );
}

void Register_UnityEngine_Profiler_set_logFile()
{
    scripting_add_internal_call( "UnityEngine.Profiler::set_logFile" , (gpointer)& Profiler_Set_Custom_PropLogFile );
}

void Register_UnityEngine_Profiler_get_enableBinaryLog()
{
    scripting_add_internal_call( "UnityEngine.Profiler::get_enableBinaryLog" , (gpointer)& Profiler_Get_Custom_PropEnableBinaryLog );
}

void Register_UnityEngine_Profiler_set_enableBinaryLog()
{
    scripting_add_internal_call( "UnityEngine.Profiler::set_enableBinaryLog" , (gpointer)& Profiler_Set_Custom_PropEnableBinaryLog );
}

void Register_UnityEngine_Profiler_get_enabled()
{
    scripting_add_internal_call( "UnityEngine.Profiler::get_enabled" , (gpointer)& Profiler_Get_Custom_PropEnabled );
}

void Register_UnityEngine_Profiler_set_enabled()
{
    scripting_add_internal_call( "UnityEngine.Profiler::set_enabled" , (gpointer)& Profiler_Set_Custom_PropEnabled );
}

void Register_UnityEngine_Profiler_AddFramesFromFile()
{
    scripting_add_internal_call( "UnityEngine.Profiler::AddFramesFromFile" , (gpointer)& Profiler_CUSTOM_AddFramesFromFile );
}

void Register_UnityEngine_Profiler_BeginSample()
{
    scripting_add_internal_call( "UnityEngine.Profiler::BeginSample" , (gpointer)& Profiler_CUSTOM_BeginSample );
}

void Register_UnityEngine_Profiler_BeginSampleOnly()
{
    scripting_add_internal_call( "UnityEngine.Profiler::BeginSampleOnly" , (gpointer)& Profiler_CUSTOM_BeginSampleOnly );
}

void Register_UnityEngine_Profiler_EndSample()
{
    scripting_add_internal_call( "UnityEngine.Profiler::EndSample" , (gpointer)& Profiler_CUSTOM_EndSample );
}

void Register_UnityEngine_Profiler_get_usedHeapSize()
{
    scripting_add_internal_call( "UnityEngine.Profiler::get_usedHeapSize" , (gpointer)& Profiler_Get_Custom_PropUsedHeapSize );
}

void Register_UnityEngine_Profiler_GetRuntimeMemorySize()
{
    scripting_add_internal_call( "UnityEngine.Profiler::GetRuntimeMemorySize" , (gpointer)& Profiler_CUSTOM_GetRuntimeMemorySize );
}

void Register_UnityEngine_Profiler_GetMonoHeapSize()
{
    scripting_add_internal_call( "UnityEngine.Profiler::GetMonoHeapSize" , (gpointer)& Profiler_CUSTOM_GetMonoHeapSize );
}

void Register_UnityEngine_Profiler_GetMonoUsedSize()
{
    scripting_add_internal_call( "UnityEngine.Profiler::GetMonoUsedSize" , (gpointer)& Profiler_CUSTOM_GetMonoUsedSize );
}

void Register_UnityEngine_Profiler_GetTotalAllocatedMemory()
{
    scripting_add_internal_call( "UnityEngine.Profiler::GetTotalAllocatedMemory" , (gpointer)& Profiler_CUSTOM_GetTotalAllocatedMemory );
}

void Register_UnityEngine_Profiler_GetTotalUnusedReservedMemory()
{
    scripting_add_internal_call( "UnityEngine.Profiler::GetTotalUnusedReservedMemory" , (gpointer)& Profiler_CUSTOM_GetTotalUnusedReservedMemory );
}

void Register_UnityEngine_Profiler_GetTotalReservedMemory()
{
    scripting_add_internal_call( "UnityEngine.Profiler::GetTotalReservedMemory" , (gpointer)& Profiler_CUSTOM_GetTotalReservedMemory );
}

#if ENABLE_MONO
void Register_UnityEngineInternal_Reproduction_CaptureScreenshot()
{
    scripting_add_internal_call( "UnityEngineInternal.Reproduction::CaptureScreenshot" , (gpointer)& Reproduction_CUSTOM_CaptureScreenshot );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_BaseClass_IcallNames [] =
{
    "UnityEngine.LayerMask::LayerToName"    ,    // -> LayerMask_CUSTOM_LayerToName
    "UnityEngine.LayerMask::NameToLayer"    ,    // -> LayerMask_CUSTOM_NameToLayer
#if ENABLE_AUDIO_FMOD
    "UnityEngine.AudioSettings::get_driverCaps",    // -> AudioSettings_Get_Custom_PropDriverCaps
    "UnityEngine.AudioSettings::get_speakerMode",    // -> AudioSettings_Get_Custom_PropSpeakerMode
    "UnityEngine.AudioSettings::set_speakerMode",    // -> AudioSettings_Set_Custom_PropSpeakerMode
    "UnityEngine.AudioSettings::get_dspTime",    // -> AudioSettings_Get_Custom_PropDspTime
    "UnityEngine.AudioSettings::get_outputSampleRate",    // -> AudioSettings_Get_Custom_PropOutputSampleRate
    "UnityEngine.AudioSettings::set_outputSampleRate",    // -> AudioSettings_Set_Custom_PropOutputSampleRate
    "UnityEngine.AudioSettings::SetDSPBufferSize",    // -> AudioSettings_CUSTOM_SetDSPBufferSize
    "UnityEngine.AudioSettings::GetDSPBufferSize",    // -> AudioSettings_CUSTOM_GetDSPBufferSize
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioClip::get_length"     ,    // -> AudioClip_Get_Custom_PropLength
    "UnityEngine.AudioClip::get_samples"    ,    // -> AudioClip_Get_Custom_PropSamples
    "UnityEngine.AudioClip::get_channels"   ,    // -> AudioClip_Get_Custom_PropChannels
    "UnityEngine.AudioClip::get_frequency"  ,    // -> AudioClip_Get_Custom_PropFrequency
    "UnityEngine.AudioClip::get_isReadyToPlay",    // -> AudioClip_Get_Custom_PropIsReadyToPlay
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    "UnityEngine.AudioClip::GetData"        ,    // -> AudioClip_CUSTOM_GetData
    "UnityEngine.AudioClip::SetData"        ,    // -> AudioClip_CUSTOM_SetData
    "UnityEngine.AudioClip::Construct_Internal",    // -> AudioClip_CUSTOM_Construct_Internal
    "UnityEngine.AudioClip::Init_Internal"  ,    // -> AudioClip_CUSTOM_Init_Internal
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioListener::get_volume" ,    // -> AudioListener_Get_Custom_PropVolume
    "UnityEngine.AudioListener::set_volume" ,    // -> AudioListener_Set_Custom_PropVolume
    "UnityEngine.AudioListener::get_pause"  ,    // -> AudioListener_Get_Custom_PropPause
    "UnityEngine.AudioListener::set_pause"  ,    // -> AudioListener_Set_Custom_PropPause
    "UnityEngine.AudioListener::get_velocityUpdateMode",    // -> AudioListener_Get_Custom_PropVelocityUpdateMode
    "UnityEngine.AudioListener::set_velocityUpdateMode",    // -> AudioListener_Set_Custom_PropVelocityUpdateMode
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    "UnityEngine.AudioListener::GetOutputDataHelper",    // -> AudioListener_CUSTOM_GetOutputDataHelper
    "UnityEngine.AudioListener::GetSpectrumDataHelper",    // -> AudioListener_CUSTOM_GetSpectrumDataHelper
#endif
#if ENABLE_MICROPHONE
    "UnityEngine.Microphone::Start"         ,    // -> Microphone_CUSTOM_Start
    "UnityEngine.Microphone::End"           ,    // -> Microphone_CUSTOM_End
    "UnityEngine.Microphone::get_devices"   ,    // -> Microphone_Get_Custom_PropDevices
    "UnityEngine.Microphone::IsRecording"   ,    // -> Microphone_CUSTOM_IsRecording
    "UnityEngine.Microphone::GetPosition"   ,    // -> Microphone_CUSTOM_GetPosition
    "UnityEngine.Microphone::GetDeviceCaps" ,    // -> Microphone_CUSTOM_GetDeviceCaps
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioSource::get_volume"   ,    // -> AudioSource_Get_Custom_PropVolume
    "UnityEngine.AudioSource::set_volume"   ,    // -> AudioSource_Set_Custom_PropVolume
    "UnityEngine.AudioSource::get_pitch"    ,    // -> AudioSource_Get_Custom_PropPitch
    "UnityEngine.AudioSource::set_pitch"    ,    // -> AudioSource_Set_Custom_PropPitch
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
    "UnityEngine.AudioSource::get_time"     ,    // -> AudioSource_Get_Custom_PropTime
    "UnityEngine.AudioSource::set_time"     ,    // -> AudioSource_Set_Custom_PropTime
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    "UnityEngine.AudioSource::get_timeSamples",    // -> AudioSource_Get_Custom_PropTimeSamples
    "UnityEngine.AudioSource::set_timeSamples",    // -> AudioSource_Set_Custom_PropTimeSamples
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioSource::get_clip"     ,    // -> AudioSource_Get_Custom_PropClip
    "UnityEngine.AudioSource::set_clip"     ,    // -> AudioSource_Set_Custom_PropClip
    "UnityEngine.AudioSource::Play"         ,    // -> AudioSource_CUSTOM_Play
    "UnityEngine.AudioSource::PlayDelayed"  ,    // -> AudioSource_CUSTOM_PlayDelayed
    "UnityEngine.AudioSource::PlayScheduled",    // -> AudioSource_CUSTOM_PlayScheduled
    "UnityEngine.AudioSource::SetScheduledStartTime",    // -> AudioSource_CUSTOM_SetScheduledStartTime
    "UnityEngine.AudioSource::SetScheduledEndTime",    // -> AudioSource_CUSTOM_SetScheduledEndTime
    "UnityEngine.AudioSource::Stop"         ,    // -> AudioSource_CUSTOM_Stop
    "UnityEngine.AudioSource::INTERNAL_CALL_Pause",    // -> AudioSource_CUSTOM_INTERNAL_CALL_Pause
    "UnityEngine.AudioSource::get_isPlaying",    // -> AudioSource_Get_Custom_PropIsPlaying
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
    "UnityEngine.AudioSource::PlayOneShot"  ,    // -> AudioSource_CUSTOM_PlayOneShot
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioSource::get_loop"     ,    // -> AudioSource_Get_Custom_PropLoop
    "UnityEngine.AudioSource::set_loop"     ,    // -> AudioSource_Set_Custom_PropLoop
    "UnityEngine.AudioSource::get_ignoreListenerVolume",    // -> AudioSource_Get_Custom_PropIgnoreListenerVolume
    "UnityEngine.AudioSource::set_ignoreListenerVolume",    // -> AudioSource_Set_Custom_PropIgnoreListenerVolume
    "UnityEngine.AudioSource::get_playOnAwake",    // -> AudioSource_Get_Custom_PropPlayOnAwake
    "UnityEngine.AudioSource::set_playOnAwake",    // -> AudioSource_Set_Custom_PropPlayOnAwake
    "UnityEngine.AudioSource::get_ignoreListenerPause",    // -> AudioSource_Get_Custom_PropIgnoreListenerPause
    "UnityEngine.AudioSource::set_ignoreListenerPause",    // -> AudioSource_Set_Custom_PropIgnoreListenerPause
    "UnityEngine.AudioSource::get_velocityUpdateMode",    // -> AudioSource_Get_Custom_PropVelocityUpdateMode
    "UnityEngine.AudioSource::set_velocityUpdateMode",    // -> AudioSource_Set_Custom_PropVelocityUpdateMode
    "UnityEngine.AudioSource::get_panLevel" ,    // -> AudioSource_Get_Custom_PropPanLevel
    "UnityEngine.AudioSource::set_panLevel" ,    // -> AudioSource_Set_Custom_PropPanLevel
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    "UnityEngine.AudioSource::get_bypassEffects",    // -> AudioSource_Get_Custom_PropBypassEffects
    "UnityEngine.AudioSource::set_bypassEffects",    // -> AudioSource_Set_Custom_PropBypassEffects
    "UnityEngine.AudioSource::get_bypassListenerEffects",    // -> AudioSource_Get_Custom_PropBypassListenerEffects
    "UnityEngine.AudioSource::set_bypassListenerEffects",    // -> AudioSource_Set_Custom_PropBypassListenerEffects
    "UnityEngine.AudioSource::get_bypassReverbZones",    // -> AudioSource_Get_Custom_PropBypassReverbZones
    "UnityEngine.AudioSource::set_bypassReverbZones",    // -> AudioSource_Set_Custom_PropBypassReverbZones
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioSource::get_dopplerLevel",    // -> AudioSource_Get_Custom_PropDopplerLevel
    "UnityEngine.AudioSource::set_dopplerLevel",    // -> AudioSource_Set_Custom_PropDopplerLevel
    "UnityEngine.AudioSource::get_spread"   ,    // -> AudioSource_Get_Custom_PropSpread
    "UnityEngine.AudioSource::set_spread"   ,    // -> AudioSource_Set_Custom_PropSpread
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    "UnityEngine.AudioSource::get_priority" ,    // -> AudioSource_Get_Custom_PropPriority
    "UnityEngine.AudioSource::set_priority" ,    // -> AudioSource_Set_Custom_PropPriority
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioSource::get_mute"     ,    // -> AudioSource_Get_Custom_PropMute
    "UnityEngine.AudioSource::set_mute"     ,    // -> AudioSource_Set_Custom_PropMute
    "UnityEngine.AudioSource::get_minDistance",    // -> AudioSource_Get_Custom_PropMinDistance
    "UnityEngine.AudioSource::set_minDistance",    // -> AudioSource_Set_Custom_PropMinDistance
    "UnityEngine.AudioSource::get_maxDistance",    // -> AudioSource_Get_Custom_PropMaxDistance
    "UnityEngine.AudioSource::set_maxDistance",    // -> AudioSource_Set_Custom_PropMaxDistance
    "UnityEngine.AudioSource::get_pan"      ,    // -> AudioSource_Get_Custom_PropPan
    "UnityEngine.AudioSource::set_pan"      ,    // -> AudioSource_Set_Custom_PropPan
    "UnityEngine.AudioSource::get_rolloffMode",    // -> AudioSource_Get_Custom_PropRolloffMode
    "UnityEngine.AudioSource::set_rolloffMode",    // -> AudioSource_Set_Custom_PropRolloffMode
    "UnityEngine.AudioSource::GetOutputDataHelper",    // -> AudioSource_CUSTOM_GetOutputDataHelper
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    "UnityEngine.AudioSource::GetSpectrumDataHelper",    // -> AudioSource_CUSTOM_GetSpectrumDataHelper
#endif
#if ENABLE_AUDIO
    "UnityEngine.AudioSource::get_minVolume",    // -> AudioSource_Get_Custom_PropMinVolume
    "UnityEngine.AudioSource::set_minVolume",    // -> AudioSource_Set_Custom_PropMinVolume
    "UnityEngine.AudioSource::get_maxVolume",    // -> AudioSource_Get_Custom_PropMaxVolume
    "UnityEngine.AudioSource::set_maxVolume",    // -> AudioSource_Set_Custom_PropMaxVolume
    "UnityEngine.AudioSource::get_rolloffFactor",    // -> AudioSource_Get_Custom_PropRolloffFactor
    "UnityEngine.AudioSource::set_rolloffFactor",    // -> AudioSource_Set_Custom_PropRolloffFactor
#endif
#if ENABLE_AUDIO_FMOD
    "UnityEngine.AudioReverbZone::get_minDistance",    // -> AudioReverbZone_Get_Custom_PropMinDistance
    "UnityEngine.AudioReverbZone::set_minDistance",    // -> AudioReverbZone_Set_Custom_PropMinDistance
    "UnityEngine.AudioReverbZone::get_maxDistance",    // -> AudioReverbZone_Get_Custom_PropMaxDistance
    "UnityEngine.AudioReverbZone::set_maxDistance",    // -> AudioReverbZone_Set_Custom_PropMaxDistance
    "UnityEngine.AudioReverbZone::get_reverbPreset",    // -> AudioReverbZone_Get_Custom_PropReverbPreset
    "UnityEngine.AudioReverbZone::set_reverbPreset",    // -> AudioReverbZone_Set_Custom_PropReverbPreset
    "UnityEngine.AudioReverbZone::get_room" ,    // -> AudioReverbZone_Get_Custom_PropRoom
    "UnityEngine.AudioReverbZone::set_room" ,    // -> AudioReverbZone_Set_Custom_PropRoom
    "UnityEngine.AudioReverbZone::get_roomHF",    // -> AudioReverbZone_Get_Custom_PropRoomHF
    "UnityEngine.AudioReverbZone::set_roomHF",    // -> AudioReverbZone_Set_Custom_PropRoomHF
    "UnityEngine.AudioReverbZone::get_roomLF",    // -> AudioReverbZone_Get_Custom_PropRoomLF
    "UnityEngine.AudioReverbZone::set_roomLF",    // -> AudioReverbZone_Set_Custom_PropRoomLF
    "UnityEngine.AudioReverbZone::get_decayTime",    // -> AudioReverbZone_Get_Custom_PropDecayTime
    "UnityEngine.AudioReverbZone::set_decayTime",    // -> AudioReverbZone_Set_Custom_PropDecayTime
    "UnityEngine.AudioReverbZone::get_decayHFRatio",    // -> AudioReverbZone_Get_Custom_PropDecayHFRatio
    "UnityEngine.AudioReverbZone::set_decayHFRatio",    // -> AudioReverbZone_Set_Custom_PropDecayHFRatio
    "UnityEngine.AudioReverbZone::get_reflections",    // -> AudioReverbZone_Get_Custom_PropReflections
    "UnityEngine.AudioReverbZone::set_reflections",    // -> AudioReverbZone_Set_Custom_PropReflections
    "UnityEngine.AudioReverbZone::get_reflectionsDelay",    // -> AudioReverbZone_Get_Custom_PropReflectionsDelay
    "UnityEngine.AudioReverbZone::set_reflectionsDelay",    // -> AudioReverbZone_Set_Custom_PropReflectionsDelay
    "UnityEngine.AudioReverbZone::get_reverb",    // -> AudioReverbZone_Get_Custom_PropReverb
    "UnityEngine.AudioReverbZone::set_reverb",    // -> AudioReverbZone_Set_Custom_PropReverb
    "UnityEngine.AudioReverbZone::get_reverbDelay",    // -> AudioReverbZone_Get_Custom_PropReverbDelay
    "UnityEngine.AudioReverbZone::set_reverbDelay",    // -> AudioReverbZone_Set_Custom_PropReverbDelay
    "UnityEngine.AudioReverbZone::get_HFReference",    // -> AudioReverbZone_Get_Custom_PropHFReference
    "UnityEngine.AudioReverbZone::set_HFReference",    // -> AudioReverbZone_Set_Custom_PropHFReference
    "UnityEngine.AudioReverbZone::get_LFReference",    // -> AudioReverbZone_Get_Custom_PropLFReference
    "UnityEngine.AudioReverbZone::set_LFReference",    // -> AudioReverbZone_Set_Custom_PropLFReference
    "UnityEngine.AudioReverbZone::get_roomRolloffFactor",    // -> AudioReverbZone_Get_Custom_PropRoomRolloffFactor
    "UnityEngine.AudioReverbZone::set_roomRolloffFactor",    // -> AudioReverbZone_Set_Custom_PropRoomRolloffFactor
    "UnityEngine.AudioReverbZone::get_diffusion",    // -> AudioReverbZone_Get_Custom_PropDiffusion
    "UnityEngine.AudioReverbZone::set_diffusion",    // -> AudioReverbZone_Set_Custom_PropDiffusion
    "UnityEngine.AudioReverbZone::get_density",    // -> AudioReverbZone_Get_Custom_PropDensity
    "UnityEngine.AudioReverbZone::set_density",    // -> AudioReverbZone_Set_Custom_PropDensity
    "UnityEngine.AudioLowPassFilter::get_cutoffFrequency",    // -> AudioLowPassFilter_Get_Custom_PropCutoffFrequency
    "UnityEngine.AudioLowPassFilter::set_cutoffFrequency",    // -> AudioLowPassFilter_Set_Custom_PropCutoffFrequency
    "UnityEngine.AudioLowPassFilter::get_lowpassResonaceQ",    // -> AudioLowPassFilter_Get_Custom_PropLowpassResonaceQ
    "UnityEngine.AudioLowPassFilter::set_lowpassResonaceQ",    // -> AudioLowPassFilter_Set_Custom_PropLowpassResonaceQ
    "UnityEngine.AudioHighPassFilter::get_cutoffFrequency",    // -> AudioHighPassFilter_Get_Custom_PropCutoffFrequency
    "UnityEngine.AudioHighPassFilter::set_cutoffFrequency",    // -> AudioHighPassFilter_Set_Custom_PropCutoffFrequency
    "UnityEngine.AudioHighPassFilter::get_highpassResonaceQ",    // -> AudioHighPassFilter_Get_Custom_PropHighpassResonaceQ
    "UnityEngine.AudioHighPassFilter::set_highpassResonaceQ",    // -> AudioHighPassFilter_Set_Custom_PropHighpassResonaceQ
    "UnityEngine.AudioDistortionFilter::get_distortionLevel",    // -> AudioDistortionFilter_Get_Custom_PropDistortionLevel
    "UnityEngine.AudioDistortionFilter::set_distortionLevel",    // -> AudioDistortionFilter_Set_Custom_PropDistortionLevel
    "UnityEngine.AudioEchoFilter::get_delay",    // -> AudioEchoFilter_Get_Custom_PropDelay
    "UnityEngine.AudioEchoFilter::set_delay",    // -> AudioEchoFilter_Set_Custom_PropDelay
    "UnityEngine.AudioEchoFilter::get_decayRatio",    // -> AudioEchoFilter_Get_Custom_PropDecayRatio
    "UnityEngine.AudioEchoFilter::set_decayRatio",    // -> AudioEchoFilter_Set_Custom_PropDecayRatio
    "UnityEngine.AudioEchoFilter::get_dryMix",    // -> AudioEchoFilter_Get_Custom_PropDryMix
    "UnityEngine.AudioEchoFilter::set_dryMix",    // -> AudioEchoFilter_Set_Custom_PropDryMix
    "UnityEngine.AudioEchoFilter::get_wetMix",    // -> AudioEchoFilter_Get_Custom_PropWetMix
    "UnityEngine.AudioEchoFilter::set_wetMix",    // -> AudioEchoFilter_Set_Custom_PropWetMix
    "UnityEngine.AudioChorusFilter::get_dryMix",    // -> AudioChorusFilter_Get_Custom_PropDryMix
    "UnityEngine.AudioChorusFilter::set_dryMix",    // -> AudioChorusFilter_Set_Custom_PropDryMix
    "UnityEngine.AudioChorusFilter::get_wetMix1",    // -> AudioChorusFilter_Get_Custom_PropWetMix1
    "UnityEngine.AudioChorusFilter::set_wetMix1",    // -> AudioChorusFilter_Set_Custom_PropWetMix1
    "UnityEngine.AudioChorusFilter::get_wetMix2",    // -> AudioChorusFilter_Get_Custom_PropWetMix2
    "UnityEngine.AudioChorusFilter::set_wetMix2",    // -> AudioChorusFilter_Set_Custom_PropWetMix2
    "UnityEngine.AudioChorusFilter::get_wetMix3",    // -> AudioChorusFilter_Get_Custom_PropWetMix3
    "UnityEngine.AudioChorusFilter::set_wetMix3",    // -> AudioChorusFilter_Set_Custom_PropWetMix3
    "UnityEngine.AudioChorusFilter::get_delay",    // -> AudioChorusFilter_Get_Custom_PropDelay
    "UnityEngine.AudioChorusFilter::set_delay",    // -> AudioChorusFilter_Set_Custom_PropDelay
    "UnityEngine.AudioChorusFilter::get_rate",    // -> AudioChorusFilter_Get_Custom_PropRate
    "UnityEngine.AudioChorusFilter::set_rate",    // -> AudioChorusFilter_Set_Custom_PropRate
    "UnityEngine.AudioChorusFilter::get_depth",    // -> AudioChorusFilter_Get_Custom_PropDepth
    "UnityEngine.AudioChorusFilter::set_depth",    // -> AudioChorusFilter_Set_Custom_PropDepth
    "UnityEngine.AudioChorusFilter::get_feedback",    // -> AudioChorusFilter_Get_Custom_PropFeedback
    "UnityEngine.AudioChorusFilter::set_feedback",    // -> AudioChorusFilter_Set_Custom_PropFeedback
    "UnityEngine.AudioReverbFilter::get_reverbPreset",    // -> AudioReverbFilter_Get_Custom_PropReverbPreset
    "UnityEngine.AudioReverbFilter::set_reverbPreset",    // -> AudioReverbFilter_Set_Custom_PropReverbPreset
    "UnityEngine.AudioReverbFilter::get_dryLevel",    // -> AudioReverbFilter_Get_Custom_PropDryLevel
    "UnityEngine.AudioReverbFilter::set_dryLevel",    // -> AudioReverbFilter_Set_Custom_PropDryLevel
    "UnityEngine.AudioReverbFilter::get_room",    // -> AudioReverbFilter_Get_Custom_PropRoom
    "UnityEngine.AudioReverbFilter::set_room",    // -> AudioReverbFilter_Set_Custom_PropRoom
    "UnityEngine.AudioReverbFilter::get_roomHF",    // -> AudioReverbFilter_Get_Custom_PropRoomHF
    "UnityEngine.AudioReverbFilter::set_roomHF",    // -> AudioReverbFilter_Set_Custom_PropRoomHF
    "UnityEngine.AudioReverbFilter::get_roomRolloff",    // -> AudioReverbFilter_Get_Custom_PropRoomRolloff
    "UnityEngine.AudioReverbFilter::set_roomRolloff",    // -> AudioReverbFilter_Set_Custom_PropRoomRolloff
    "UnityEngine.AudioReverbFilter::get_decayTime",    // -> AudioReverbFilter_Get_Custom_PropDecayTime
    "UnityEngine.AudioReverbFilter::set_decayTime",    // -> AudioReverbFilter_Set_Custom_PropDecayTime
    "UnityEngine.AudioReverbFilter::get_decayHFRatio",    // -> AudioReverbFilter_Get_Custom_PropDecayHFRatio
    "UnityEngine.AudioReverbFilter::set_decayHFRatio",    // -> AudioReverbFilter_Set_Custom_PropDecayHFRatio
    "UnityEngine.AudioReverbFilter::get_reflectionsLevel",    // -> AudioReverbFilter_Get_Custom_PropReflectionsLevel
    "UnityEngine.AudioReverbFilter::set_reflectionsLevel",    // -> AudioReverbFilter_Set_Custom_PropReflectionsLevel
    "UnityEngine.AudioReverbFilter::get_reflectionsDelay",    // -> AudioReverbFilter_Get_Custom_PropReflectionsDelay
    "UnityEngine.AudioReverbFilter::set_reflectionsDelay",    // -> AudioReverbFilter_Set_Custom_PropReflectionsDelay
    "UnityEngine.AudioReverbFilter::get_reverbLevel",    // -> AudioReverbFilter_Get_Custom_PropReverbLevel
    "UnityEngine.AudioReverbFilter::set_reverbLevel",    // -> AudioReverbFilter_Set_Custom_PropReverbLevel
    "UnityEngine.AudioReverbFilter::get_reverbDelay",    // -> AudioReverbFilter_Get_Custom_PropReverbDelay
    "UnityEngine.AudioReverbFilter::set_reverbDelay",    // -> AudioReverbFilter_Set_Custom_PropReverbDelay
    "UnityEngine.AudioReverbFilter::get_diffusion",    // -> AudioReverbFilter_Get_Custom_PropDiffusion
    "UnityEngine.AudioReverbFilter::set_diffusion",    // -> AudioReverbFilter_Set_Custom_PropDiffusion
    "UnityEngine.AudioReverbFilter::get_density",    // -> AudioReverbFilter_Get_Custom_PropDensity
    "UnityEngine.AudioReverbFilter::set_density",    // -> AudioReverbFilter_Set_Custom_PropDensity
    "UnityEngine.AudioReverbFilter::get_hfReference",    // -> AudioReverbFilter_Get_Custom_PropHfReference
    "UnityEngine.AudioReverbFilter::set_hfReference",    // -> AudioReverbFilter_Set_Custom_PropHfReference
    "UnityEngine.AudioReverbFilter::get_roomLF",    // -> AudioReverbFilter_Get_Custom_PropRoomLF
    "UnityEngine.AudioReverbFilter::set_roomLF",    // -> AudioReverbFilter_Set_Custom_PropRoomLF
    "UnityEngine.AudioReverbFilter::get_lFReference",    // -> AudioReverbFilter_Get_Custom_PropLFReference
    "UnityEngine.AudioReverbFilter::set_lFReference",    // -> AudioReverbFilter_Set_Custom_PropLFReference
#endif
    "UnityEngine.PlayerPrefs::TrySetInt"    ,    // -> PlayerPrefs_CUSTOM_TrySetInt
    "UnityEngine.PlayerPrefs::TrySetFloat"  ,    // -> PlayerPrefs_CUSTOM_TrySetFloat
    "UnityEngine.PlayerPrefs::TrySetSetString",    // -> PlayerPrefs_CUSTOM_TrySetSetString
    "UnityEngine.PlayerPrefs::GetInt"       ,    // -> PlayerPrefs_CUSTOM_GetInt
    "UnityEngine.PlayerPrefs::GetFloat"     ,    // -> PlayerPrefs_CUSTOM_GetFloat
    "UnityEngine.PlayerPrefs::GetString"    ,    // -> PlayerPrefs_CUSTOM_GetString
    "UnityEngine.PlayerPrefs::HasKey"       ,    // -> PlayerPrefs_CUSTOM_HasKey
    "UnityEngine.PlayerPrefs::DeleteKey"    ,    // -> PlayerPrefs_CUSTOM_DeleteKey
    "UnityEngine.PlayerPrefs::DeleteAll"    ,    // -> PlayerPrefs_CUSTOM_DeleteAll
    "UnityEngine.PlayerPrefs::Save"         ,    // -> PlayerPrefs_CUSTOM_Save
    "UnityEngine.SystemInfo::get_operatingSystem",    // -> SystemInfo_Get_Custom_PropOperatingSystem
    "UnityEngine.SystemInfo::get_processorType",    // -> SystemInfo_Get_Custom_PropProcessorType
    "UnityEngine.SystemInfo::get_processorCount",    // -> SystemInfo_Get_Custom_PropProcessorCount
    "UnityEngine.SystemInfo::get_systemMemorySize",    // -> SystemInfo_Get_Custom_PropSystemMemorySize
    "UnityEngine.SystemInfo::get_graphicsMemorySize",    // -> SystemInfo_Get_Custom_PropGraphicsMemorySize
    "UnityEngine.SystemInfo::get_graphicsDeviceName",    // -> SystemInfo_Get_Custom_PropGraphicsDeviceName
    "UnityEngine.SystemInfo::get_graphicsDeviceVendor",    // -> SystemInfo_Get_Custom_PropGraphicsDeviceVendor
    "UnityEngine.SystemInfo::get_graphicsDeviceID",    // -> SystemInfo_Get_Custom_PropGraphicsDeviceID
    "UnityEngine.SystemInfo::get_graphicsDeviceVendorID",    // -> SystemInfo_Get_Custom_PropGraphicsDeviceVendorID
    "UnityEngine.SystemInfo::get_graphicsDeviceVersion",    // -> SystemInfo_Get_Custom_PropGraphicsDeviceVersion
    "UnityEngine.SystemInfo::get_graphicsShaderLevel",    // -> SystemInfo_Get_Custom_PropGraphicsShaderLevel
    "UnityEngine.SystemInfo::get_graphicsPixelFillrate",    // -> SystemInfo_Get_Custom_PropGraphicsPixelFillrate
    "UnityEngine.SystemInfo::get_supportsShadows",    // -> SystemInfo_Get_Custom_PropSupportsShadows
    "UnityEngine.SystemInfo::get_supportsRenderTextures",    // -> SystemInfo_Get_Custom_PropSupportsRenderTextures
    "UnityEngine.SystemInfo::get_supportsImageEffects",    // -> SystemInfo_Get_Custom_PropSupportsImageEffects
    "UnityEngine.SystemInfo::get_supports3DTextures",    // -> SystemInfo_Get_Custom_PropSupports3DTextures
    "UnityEngine.SystemInfo::get_supportsComputeShaders",    // -> SystemInfo_Get_Custom_PropSupportsComputeShaders
    "UnityEngine.SystemInfo::get_supportsInstancing",    // -> SystemInfo_Get_Custom_PropSupportsInstancing
    "UnityEngine.SystemInfo::get_supportedRenderTargetCount",    // -> SystemInfo_Get_Custom_PropSupportedRenderTargetCount
    "UnityEngine.SystemInfo::get_supportsStencil",    // -> SystemInfo_Get_Custom_PropSupportsStencil
    "UnityEngine.SystemInfo::get_supportsVertexPrograms",    // -> SystemInfo_Get_Custom_PropSupportsVertexPrograms
    "UnityEngine.SystemInfo::SupportsRenderTextureFormat",    // -> SystemInfo_CUSTOM_SupportsRenderTextureFormat
    "UnityEngine.SystemInfo::get_npotSupport",    // -> SystemInfo_Get_Custom_PropNpotSupport
#if !UNITY_FLASH && !PLATFORM_WEBGL
    "UnityEngine.SystemInfo::get_deviceUniqueIdentifier",    // -> SystemInfo_Get_Custom_PropDeviceUniqueIdentifier
    "UnityEngine.SystemInfo::get_deviceName",    // -> SystemInfo_Get_Custom_PropDeviceName
    "UnityEngine.SystemInfo::get_deviceModel",    // -> SystemInfo_Get_Custom_PropDeviceModel
#endif
    "UnityEngine.SystemInfo::get_supportsAccelerometer",    // -> SystemInfo_Get_Custom_PropSupportsAccelerometer
    "UnityEngine.SystemInfo::get_supportsGyroscope",    // -> SystemInfo_Get_Custom_PropSupportsGyroscope
    "UnityEngine.SystemInfo::get_supportsLocationService",    // -> SystemInfo_Get_Custom_PropSupportsLocationService
    "UnityEngine.SystemInfo::get_supportsVibration",    // -> SystemInfo_Get_Custom_PropSupportsVibration
    "UnityEngine.SystemInfo::get_deviceType",    // -> SystemInfo_Get_Custom_PropDeviceType
    "UnityEngine.SystemInfo::get_maxTextureSize",    // -> SystemInfo_Get_Custom_PropMaxTextureSize
    "UnityEngine.Coroutine::ReleaseCoroutine",    // -> Coroutine_CUSTOM_ReleaseCoroutine
    "UnityEngine.ScriptableObject::Internal_CreateScriptableObject",    // -> ScriptableObject_CUSTOM_Internal_CreateScriptableObject
#if ENABLE_MONO
    "UnityEngine.ScriptableObject::INTERNAL_CALL_SetDirty",    // -> ScriptableObject_CUSTOM_INTERNAL_CALL_SetDirty
#endif
    "UnityEngine.ScriptableObject::CreateInstance",    // -> ScriptableObject_CUSTOM_CreateInstance
    "UnityEngine.ScriptableObject::CreateInstanceFromType",    // -> ScriptableObject_CUSTOM_CreateInstanceFromType
    "UnityEngine.Resources::FindObjectsOfTypeAll",    // -> Resources_CUSTOM_FindObjectsOfTypeAll
    "UnityEngine.Resources::Load"           ,    // -> Resources_CUSTOM_Load
    "UnityEngine.Resources::LoadAll"        ,    // -> Resources_CUSTOM_LoadAll
    "UnityEngine.Resources::GetBuiltinResource",    // -> Resources_CUSTOM_GetBuiltinResource
    "UnityEngine.Resources::LoadAssetAtPath",    // -> Resources_CUSTOM_LoadAssetAtPath
    "UnityEngine.Resources::UnloadAsset"    ,    // -> Resources_CUSTOM_UnloadAsset
    "UnityEngine.Resources::UnloadUnusedAssets",    // -> Resources_CUSTOM_UnloadUnusedAssets
#if ENABLE_WWW
    "UnityEngine.AssetBundleCreateRequest::get_assetBundle",    // -> AssetBundleCreateRequest_Get_Custom_PropAssetBundle
    "UnityEngine.AssetBundle::CreateFromMemory",    // -> AssetBundle_CUSTOM_CreateFromMemory
#endif
    "UnityEngine.AssetBundle::CreateFromFile",    // -> AssetBundle_CUSTOM_CreateFromFile
    "UnityEngine.AssetBundle::get_mainAsset",    // -> AssetBundle_Get_Custom_PropMainAsset
    "UnityEngine.AssetBundle::Contains"     ,    // -> AssetBundle_CUSTOM_Contains
    "UnityEngine.AssetBundle::Load"         ,    // -> AssetBundle_CUSTOM_Load
    "UnityEngine.AssetBundle::LoadAsync"    ,    // -> AssetBundle_CUSTOM_LoadAsync
    "UnityEngine.AssetBundle::LoadAll"      ,    // -> AssetBundle_CUSTOM_LoadAll
    "UnityEngine.AssetBundle::Unload"       ,    // -> AssetBundle_CUSTOM_Unload
    "UnityEngine.Profiler::get_supported"   ,    // -> Profiler_Get_Custom_PropSupported
    "UnityEngine.Profiler::get_logFile"     ,    // -> Profiler_Get_Custom_PropLogFile
    "UnityEngine.Profiler::set_logFile"     ,    // -> Profiler_Set_Custom_PropLogFile
    "UnityEngine.Profiler::get_enableBinaryLog",    // -> Profiler_Get_Custom_PropEnableBinaryLog
    "UnityEngine.Profiler::set_enableBinaryLog",    // -> Profiler_Set_Custom_PropEnableBinaryLog
    "UnityEngine.Profiler::get_enabled"     ,    // -> Profiler_Get_Custom_PropEnabled
    "UnityEngine.Profiler::set_enabled"     ,    // -> Profiler_Set_Custom_PropEnabled
    "UnityEngine.Profiler::AddFramesFromFile",    // -> Profiler_CUSTOM_AddFramesFromFile
    "UnityEngine.Profiler::BeginSample"     ,    // -> Profiler_CUSTOM_BeginSample
    "UnityEngine.Profiler::BeginSampleOnly" ,    // -> Profiler_CUSTOM_BeginSampleOnly
    "UnityEngine.Profiler::EndSample"       ,    // -> Profiler_CUSTOM_EndSample
    "UnityEngine.Profiler::get_usedHeapSize",    // -> Profiler_Get_Custom_PropUsedHeapSize
    "UnityEngine.Profiler::GetRuntimeMemorySize",    // -> Profiler_CUSTOM_GetRuntimeMemorySize
    "UnityEngine.Profiler::GetMonoHeapSize" ,    // -> Profiler_CUSTOM_GetMonoHeapSize
    "UnityEngine.Profiler::GetMonoUsedSize" ,    // -> Profiler_CUSTOM_GetMonoUsedSize
    "UnityEngine.Profiler::GetTotalAllocatedMemory",    // -> Profiler_CUSTOM_GetTotalAllocatedMemory
    "UnityEngine.Profiler::GetTotalUnusedReservedMemory",    // -> Profiler_CUSTOM_GetTotalUnusedReservedMemory
    "UnityEngine.Profiler::GetTotalReservedMemory",    // -> Profiler_CUSTOM_GetTotalReservedMemory
#if ENABLE_MONO
    "UnityEngineInternal.Reproduction::CaptureScreenshot",    // -> Reproduction_CUSTOM_CaptureScreenshot
#endif
    NULL
};

static const void* s_BaseClass_IcallFuncs [] =
{
    (const void*)&LayerMask_CUSTOM_LayerToName            ,  //  <- UnityEngine.LayerMask::LayerToName
    (const void*)&LayerMask_CUSTOM_NameToLayer            ,  //  <- UnityEngine.LayerMask::NameToLayer
#if ENABLE_AUDIO_FMOD
    (const void*)&AudioSettings_Get_Custom_PropDriverCaps ,  //  <- UnityEngine.AudioSettings::get_driverCaps
    (const void*)&AudioSettings_Get_Custom_PropSpeakerMode,  //  <- UnityEngine.AudioSettings::get_speakerMode
    (const void*)&AudioSettings_Set_Custom_PropSpeakerMode,  //  <- UnityEngine.AudioSettings::set_speakerMode
    (const void*)&AudioSettings_Get_Custom_PropDspTime    ,  //  <- UnityEngine.AudioSettings::get_dspTime
    (const void*)&AudioSettings_Get_Custom_PropOutputSampleRate,  //  <- UnityEngine.AudioSettings::get_outputSampleRate
    (const void*)&AudioSettings_Set_Custom_PropOutputSampleRate,  //  <- UnityEngine.AudioSettings::set_outputSampleRate
    (const void*)&AudioSettings_CUSTOM_SetDSPBufferSize   ,  //  <- UnityEngine.AudioSettings::SetDSPBufferSize
    (const void*)&AudioSettings_CUSTOM_GetDSPBufferSize   ,  //  <- UnityEngine.AudioSettings::GetDSPBufferSize
#endif
#if ENABLE_AUDIO
    (const void*)&AudioClip_Get_Custom_PropLength         ,  //  <- UnityEngine.AudioClip::get_length
    (const void*)&AudioClip_Get_Custom_PropSamples        ,  //  <- UnityEngine.AudioClip::get_samples
    (const void*)&AudioClip_Get_Custom_PropChannels       ,  //  <- UnityEngine.AudioClip::get_channels
    (const void*)&AudioClip_Get_Custom_PropFrequency      ,  //  <- UnityEngine.AudioClip::get_frequency
    (const void*)&AudioClip_Get_Custom_PropIsReadyToPlay  ,  //  <- UnityEngine.AudioClip::get_isReadyToPlay
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    (const void*)&AudioClip_CUSTOM_GetData                ,  //  <- UnityEngine.AudioClip::GetData
    (const void*)&AudioClip_CUSTOM_SetData                ,  //  <- UnityEngine.AudioClip::SetData
    (const void*)&AudioClip_CUSTOM_Construct_Internal     ,  //  <- UnityEngine.AudioClip::Construct_Internal
    (const void*)&AudioClip_CUSTOM_Init_Internal          ,  //  <- UnityEngine.AudioClip::Init_Internal
#endif
#if ENABLE_AUDIO
    (const void*)&AudioListener_Get_Custom_PropVolume     ,  //  <- UnityEngine.AudioListener::get_volume
    (const void*)&AudioListener_Set_Custom_PropVolume     ,  //  <- UnityEngine.AudioListener::set_volume
    (const void*)&AudioListener_Get_Custom_PropPause      ,  //  <- UnityEngine.AudioListener::get_pause
    (const void*)&AudioListener_Set_Custom_PropPause      ,  //  <- UnityEngine.AudioListener::set_pause
    (const void*)&AudioListener_Get_Custom_PropVelocityUpdateMode,  //  <- UnityEngine.AudioListener::get_velocityUpdateMode
    (const void*)&AudioListener_Set_Custom_PropVelocityUpdateMode,  //  <- UnityEngine.AudioListener::set_velocityUpdateMode
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    (const void*)&AudioListener_CUSTOM_GetOutputDataHelper,  //  <- UnityEngine.AudioListener::GetOutputDataHelper
    (const void*)&AudioListener_CUSTOM_GetSpectrumDataHelper,  //  <- UnityEngine.AudioListener::GetSpectrumDataHelper
#endif
#if ENABLE_MICROPHONE
    (const void*)&Microphone_CUSTOM_Start                 ,  //  <- UnityEngine.Microphone::Start
    (const void*)&Microphone_CUSTOM_End                   ,  //  <- UnityEngine.Microphone::End
    (const void*)&Microphone_Get_Custom_PropDevices       ,  //  <- UnityEngine.Microphone::get_devices
    (const void*)&Microphone_CUSTOM_IsRecording           ,  //  <- UnityEngine.Microphone::IsRecording
    (const void*)&Microphone_CUSTOM_GetPosition           ,  //  <- UnityEngine.Microphone::GetPosition
    (const void*)&Microphone_CUSTOM_GetDeviceCaps         ,  //  <- UnityEngine.Microphone::GetDeviceCaps
#endif
#if ENABLE_AUDIO
    (const void*)&AudioSource_Get_Custom_PropVolume       ,  //  <- UnityEngine.AudioSource::get_volume
    (const void*)&AudioSource_Set_Custom_PropVolume       ,  //  <- UnityEngine.AudioSource::set_volume
    (const void*)&AudioSource_Get_Custom_PropPitch        ,  //  <- UnityEngine.AudioSource::get_pitch
    (const void*)&AudioSource_Set_Custom_PropPitch        ,  //  <- UnityEngine.AudioSource::set_pitch
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
    (const void*)&AudioSource_Get_Custom_PropTime         ,  //  <- UnityEngine.AudioSource::get_time
    (const void*)&AudioSource_Set_Custom_PropTime         ,  //  <- UnityEngine.AudioSource::set_time
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    (const void*)&AudioSource_Get_Custom_PropTimeSamples  ,  //  <- UnityEngine.AudioSource::get_timeSamples
    (const void*)&AudioSource_Set_Custom_PropTimeSamples  ,  //  <- UnityEngine.AudioSource::set_timeSamples
#endif
#if ENABLE_AUDIO
    (const void*)&AudioSource_Get_Custom_PropClip         ,  //  <- UnityEngine.AudioSource::get_clip
    (const void*)&AudioSource_Set_Custom_PropClip         ,  //  <- UnityEngine.AudioSource::set_clip
    (const void*)&AudioSource_CUSTOM_Play                 ,  //  <- UnityEngine.AudioSource::Play
    (const void*)&AudioSource_CUSTOM_PlayDelayed          ,  //  <- UnityEngine.AudioSource::PlayDelayed
    (const void*)&AudioSource_CUSTOM_PlayScheduled        ,  //  <- UnityEngine.AudioSource::PlayScheduled
    (const void*)&AudioSource_CUSTOM_SetScheduledStartTime,  //  <- UnityEngine.AudioSource::SetScheduledStartTime
    (const void*)&AudioSource_CUSTOM_SetScheduledEndTime  ,  //  <- UnityEngine.AudioSource::SetScheduledEndTime
    (const void*)&AudioSource_CUSTOM_Stop                 ,  //  <- UnityEngine.AudioSource::Stop
    (const void*)&AudioSource_CUSTOM_INTERNAL_CALL_Pause  ,  //  <- UnityEngine.AudioSource::INTERNAL_CALL_Pause
    (const void*)&AudioSource_Get_Custom_PropIsPlaying    ,  //  <- UnityEngine.AudioSource::get_isPlaying
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
    (const void*)&AudioSource_CUSTOM_PlayOneShot          ,  //  <- UnityEngine.AudioSource::PlayOneShot
#endif
#if ENABLE_AUDIO
    (const void*)&AudioSource_Get_Custom_PropLoop         ,  //  <- UnityEngine.AudioSource::get_loop
    (const void*)&AudioSource_Set_Custom_PropLoop         ,  //  <- UnityEngine.AudioSource::set_loop
    (const void*)&AudioSource_Get_Custom_PropIgnoreListenerVolume,  //  <- UnityEngine.AudioSource::get_ignoreListenerVolume
    (const void*)&AudioSource_Set_Custom_PropIgnoreListenerVolume,  //  <- UnityEngine.AudioSource::set_ignoreListenerVolume
    (const void*)&AudioSource_Get_Custom_PropPlayOnAwake  ,  //  <- UnityEngine.AudioSource::get_playOnAwake
    (const void*)&AudioSource_Set_Custom_PropPlayOnAwake  ,  //  <- UnityEngine.AudioSource::set_playOnAwake
    (const void*)&AudioSource_Get_Custom_PropIgnoreListenerPause,  //  <- UnityEngine.AudioSource::get_ignoreListenerPause
    (const void*)&AudioSource_Set_Custom_PropIgnoreListenerPause,  //  <- UnityEngine.AudioSource::set_ignoreListenerPause
    (const void*)&AudioSource_Get_Custom_PropVelocityUpdateMode,  //  <- UnityEngine.AudioSource::get_velocityUpdateMode
    (const void*)&AudioSource_Set_Custom_PropVelocityUpdateMode,  //  <- UnityEngine.AudioSource::set_velocityUpdateMode
    (const void*)&AudioSource_Get_Custom_PropPanLevel     ,  //  <- UnityEngine.AudioSource::get_panLevel
    (const void*)&AudioSource_Set_Custom_PropPanLevel     ,  //  <- UnityEngine.AudioSource::set_panLevel
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    (const void*)&AudioSource_Get_Custom_PropBypassEffects,  //  <- UnityEngine.AudioSource::get_bypassEffects
    (const void*)&AudioSource_Set_Custom_PropBypassEffects,  //  <- UnityEngine.AudioSource::set_bypassEffects
    (const void*)&AudioSource_Get_Custom_PropBypassListenerEffects,  //  <- UnityEngine.AudioSource::get_bypassListenerEffects
    (const void*)&AudioSource_Set_Custom_PropBypassListenerEffects,  //  <- UnityEngine.AudioSource::set_bypassListenerEffects
    (const void*)&AudioSource_Get_Custom_PropBypassReverbZones,  //  <- UnityEngine.AudioSource::get_bypassReverbZones
    (const void*)&AudioSource_Set_Custom_PropBypassReverbZones,  //  <- UnityEngine.AudioSource::set_bypassReverbZones
#endif
#if ENABLE_AUDIO
    (const void*)&AudioSource_Get_Custom_PropDopplerLevel ,  //  <- UnityEngine.AudioSource::get_dopplerLevel
    (const void*)&AudioSource_Set_Custom_PropDopplerLevel ,  //  <- UnityEngine.AudioSource::set_dopplerLevel
    (const void*)&AudioSource_Get_Custom_PropSpread       ,  //  <- UnityEngine.AudioSource::get_spread
    (const void*)&AudioSource_Set_Custom_PropSpread       ,  //  <- UnityEngine.AudioSource::set_spread
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    (const void*)&AudioSource_Get_Custom_PropPriority     ,  //  <- UnityEngine.AudioSource::get_priority
    (const void*)&AudioSource_Set_Custom_PropPriority     ,  //  <- UnityEngine.AudioSource::set_priority
#endif
#if ENABLE_AUDIO
    (const void*)&AudioSource_Get_Custom_PropMute         ,  //  <- UnityEngine.AudioSource::get_mute
    (const void*)&AudioSource_Set_Custom_PropMute         ,  //  <- UnityEngine.AudioSource::set_mute
    (const void*)&AudioSource_Get_Custom_PropMinDistance  ,  //  <- UnityEngine.AudioSource::get_minDistance
    (const void*)&AudioSource_Set_Custom_PropMinDistance  ,  //  <- UnityEngine.AudioSource::set_minDistance
    (const void*)&AudioSource_Get_Custom_PropMaxDistance  ,  //  <- UnityEngine.AudioSource::get_maxDistance
    (const void*)&AudioSource_Set_Custom_PropMaxDistance  ,  //  <- UnityEngine.AudioSource::set_maxDistance
    (const void*)&AudioSource_Get_Custom_PropPan          ,  //  <- UnityEngine.AudioSource::get_pan
    (const void*)&AudioSource_Set_Custom_PropPan          ,  //  <- UnityEngine.AudioSource::set_pan
    (const void*)&AudioSource_Get_Custom_PropRolloffMode  ,  //  <- UnityEngine.AudioSource::get_rolloffMode
    (const void*)&AudioSource_Set_Custom_PropRolloffMode  ,  //  <- UnityEngine.AudioSource::set_rolloffMode
    (const void*)&AudioSource_CUSTOM_GetOutputDataHelper  ,  //  <- UnityEngine.AudioSource::GetOutputDataHelper
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    (const void*)&AudioSource_CUSTOM_GetSpectrumDataHelper,  //  <- UnityEngine.AudioSource::GetSpectrumDataHelper
#endif
#if ENABLE_AUDIO
    (const void*)&AudioSource_Get_Custom_PropMinVolume    ,  //  <- UnityEngine.AudioSource::get_minVolume
    (const void*)&AudioSource_Set_Custom_PropMinVolume    ,  //  <- UnityEngine.AudioSource::set_minVolume
    (const void*)&AudioSource_Get_Custom_PropMaxVolume    ,  //  <- UnityEngine.AudioSource::get_maxVolume
    (const void*)&AudioSource_Set_Custom_PropMaxVolume    ,  //  <- UnityEngine.AudioSource::set_maxVolume
    (const void*)&AudioSource_Get_Custom_PropRolloffFactor,  //  <- UnityEngine.AudioSource::get_rolloffFactor
    (const void*)&AudioSource_Set_Custom_PropRolloffFactor,  //  <- UnityEngine.AudioSource::set_rolloffFactor
#endif
#if ENABLE_AUDIO_FMOD
    (const void*)&AudioReverbZone_Get_Custom_PropMinDistance,  //  <- UnityEngine.AudioReverbZone::get_minDistance
    (const void*)&AudioReverbZone_Set_Custom_PropMinDistance,  //  <- UnityEngine.AudioReverbZone::set_minDistance
    (const void*)&AudioReverbZone_Get_Custom_PropMaxDistance,  //  <- UnityEngine.AudioReverbZone::get_maxDistance
    (const void*)&AudioReverbZone_Set_Custom_PropMaxDistance,  //  <- UnityEngine.AudioReverbZone::set_maxDistance
    (const void*)&AudioReverbZone_Get_Custom_PropReverbPreset,  //  <- UnityEngine.AudioReverbZone::get_reverbPreset
    (const void*)&AudioReverbZone_Set_Custom_PropReverbPreset,  //  <- UnityEngine.AudioReverbZone::set_reverbPreset
    (const void*)&AudioReverbZone_Get_Custom_PropRoom     ,  //  <- UnityEngine.AudioReverbZone::get_room
    (const void*)&AudioReverbZone_Set_Custom_PropRoom     ,  //  <- UnityEngine.AudioReverbZone::set_room
    (const void*)&AudioReverbZone_Get_Custom_PropRoomHF   ,  //  <- UnityEngine.AudioReverbZone::get_roomHF
    (const void*)&AudioReverbZone_Set_Custom_PropRoomHF   ,  //  <- UnityEngine.AudioReverbZone::set_roomHF
    (const void*)&AudioReverbZone_Get_Custom_PropRoomLF   ,  //  <- UnityEngine.AudioReverbZone::get_roomLF
    (const void*)&AudioReverbZone_Set_Custom_PropRoomLF   ,  //  <- UnityEngine.AudioReverbZone::set_roomLF
    (const void*)&AudioReverbZone_Get_Custom_PropDecayTime,  //  <- UnityEngine.AudioReverbZone::get_decayTime
    (const void*)&AudioReverbZone_Set_Custom_PropDecayTime,  //  <- UnityEngine.AudioReverbZone::set_decayTime
    (const void*)&AudioReverbZone_Get_Custom_PropDecayHFRatio,  //  <- UnityEngine.AudioReverbZone::get_decayHFRatio
    (const void*)&AudioReverbZone_Set_Custom_PropDecayHFRatio,  //  <- UnityEngine.AudioReverbZone::set_decayHFRatio
    (const void*)&AudioReverbZone_Get_Custom_PropReflections,  //  <- UnityEngine.AudioReverbZone::get_reflections
    (const void*)&AudioReverbZone_Set_Custom_PropReflections,  //  <- UnityEngine.AudioReverbZone::set_reflections
    (const void*)&AudioReverbZone_Get_Custom_PropReflectionsDelay,  //  <- UnityEngine.AudioReverbZone::get_reflectionsDelay
    (const void*)&AudioReverbZone_Set_Custom_PropReflectionsDelay,  //  <- UnityEngine.AudioReverbZone::set_reflectionsDelay
    (const void*)&AudioReverbZone_Get_Custom_PropReverb   ,  //  <- UnityEngine.AudioReverbZone::get_reverb
    (const void*)&AudioReverbZone_Set_Custom_PropReverb   ,  //  <- UnityEngine.AudioReverbZone::set_reverb
    (const void*)&AudioReverbZone_Get_Custom_PropReverbDelay,  //  <- UnityEngine.AudioReverbZone::get_reverbDelay
    (const void*)&AudioReverbZone_Set_Custom_PropReverbDelay,  //  <- UnityEngine.AudioReverbZone::set_reverbDelay
    (const void*)&AudioReverbZone_Get_Custom_PropHFReference,  //  <- UnityEngine.AudioReverbZone::get_HFReference
    (const void*)&AudioReverbZone_Set_Custom_PropHFReference,  //  <- UnityEngine.AudioReverbZone::set_HFReference
    (const void*)&AudioReverbZone_Get_Custom_PropLFReference,  //  <- UnityEngine.AudioReverbZone::get_LFReference
    (const void*)&AudioReverbZone_Set_Custom_PropLFReference,  //  <- UnityEngine.AudioReverbZone::set_LFReference
    (const void*)&AudioReverbZone_Get_Custom_PropRoomRolloffFactor,  //  <- UnityEngine.AudioReverbZone::get_roomRolloffFactor
    (const void*)&AudioReverbZone_Set_Custom_PropRoomRolloffFactor,  //  <- UnityEngine.AudioReverbZone::set_roomRolloffFactor
    (const void*)&AudioReverbZone_Get_Custom_PropDiffusion,  //  <- UnityEngine.AudioReverbZone::get_diffusion
    (const void*)&AudioReverbZone_Set_Custom_PropDiffusion,  //  <- UnityEngine.AudioReverbZone::set_diffusion
    (const void*)&AudioReverbZone_Get_Custom_PropDensity  ,  //  <- UnityEngine.AudioReverbZone::get_density
    (const void*)&AudioReverbZone_Set_Custom_PropDensity  ,  //  <- UnityEngine.AudioReverbZone::set_density
    (const void*)&AudioLowPassFilter_Get_Custom_PropCutoffFrequency,  //  <- UnityEngine.AudioLowPassFilter::get_cutoffFrequency
    (const void*)&AudioLowPassFilter_Set_Custom_PropCutoffFrequency,  //  <- UnityEngine.AudioLowPassFilter::set_cutoffFrequency
    (const void*)&AudioLowPassFilter_Get_Custom_PropLowpassResonaceQ,  //  <- UnityEngine.AudioLowPassFilter::get_lowpassResonaceQ
    (const void*)&AudioLowPassFilter_Set_Custom_PropLowpassResonaceQ,  //  <- UnityEngine.AudioLowPassFilter::set_lowpassResonaceQ
    (const void*)&AudioHighPassFilter_Get_Custom_PropCutoffFrequency,  //  <- UnityEngine.AudioHighPassFilter::get_cutoffFrequency
    (const void*)&AudioHighPassFilter_Set_Custom_PropCutoffFrequency,  //  <- UnityEngine.AudioHighPassFilter::set_cutoffFrequency
    (const void*)&AudioHighPassFilter_Get_Custom_PropHighpassResonaceQ,  //  <- UnityEngine.AudioHighPassFilter::get_highpassResonaceQ
    (const void*)&AudioHighPassFilter_Set_Custom_PropHighpassResonaceQ,  //  <- UnityEngine.AudioHighPassFilter::set_highpassResonaceQ
    (const void*)&AudioDistortionFilter_Get_Custom_PropDistortionLevel,  //  <- UnityEngine.AudioDistortionFilter::get_distortionLevel
    (const void*)&AudioDistortionFilter_Set_Custom_PropDistortionLevel,  //  <- UnityEngine.AudioDistortionFilter::set_distortionLevel
    (const void*)&AudioEchoFilter_Get_Custom_PropDelay    ,  //  <- UnityEngine.AudioEchoFilter::get_delay
    (const void*)&AudioEchoFilter_Set_Custom_PropDelay    ,  //  <- UnityEngine.AudioEchoFilter::set_delay
    (const void*)&AudioEchoFilter_Get_Custom_PropDecayRatio,  //  <- UnityEngine.AudioEchoFilter::get_decayRatio
    (const void*)&AudioEchoFilter_Set_Custom_PropDecayRatio,  //  <- UnityEngine.AudioEchoFilter::set_decayRatio
    (const void*)&AudioEchoFilter_Get_Custom_PropDryMix   ,  //  <- UnityEngine.AudioEchoFilter::get_dryMix
    (const void*)&AudioEchoFilter_Set_Custom_PropDryMix   ,  //  <- UnityEngine.AudioEchoFilter::set_dryMix
    (const void*)&AudioEchoFilter_Get_Custom_PropWetMix   ,  //  <- UnityEngine.AudioEchoFilter::get_wetMix
    (const void*)&AudioEchoFilter_Set_Custom_PropWetMix   ,  //  <- UnityEngine.AudioEchoFilter::set_wetMix
    (const void*)&AudioChorusFilter_Get_Custom_PropDryMix ,  //  <- UnityEngine.AudioChorusFilter::get_dryMix
    (const void*)&AudioChorusFilter_Set_Custom_PropDryMix ,  //  <- UnityEngine.AudioChorusFilter::set_dryMix
    (const void*)&AudioChorusFilter_Get_Custom_PropWetMix1,  //  <- UnityEngine.AudioChorusFilter::get_wetMix1
    (const void*)&AudioChorusFilter_Set_Custom_PropWetMix1,  //  <- UnityEngine.AudioChorusFilter::set_wetMix1
    (const void*)&AudioChorusFilter_Get_Custom_PropWetMix2,  //  <- UnityEngine.AudioChorusFilter::get_wetMix2
    (const void*)&AudioChorusFilter_Set_Custom_PropWetMix2,  //  <- UnityEngine.AudioChorusFilter::set_wetMix2
    (const void*)&AudioChorusFilter_Get_Custom_PropWetMix3,  //  <- UnityEngine.AudioChorusFilter::get_wetMix3
    (const void*)&AudioChorusFilter_Set_Custom_PropWetMix3,  //  <- UnityEngine.AudioChorusFilter::set_wetMix3
    (const void*)&AudioChorusFilter_Get_Custom_PropDelay  ,  //  <- UnityEngine.AudioChorusFilter::get_delay
    (const void*)&AudioChorusFilter_Set_Custom_PropDelay  ,  //  <- UnityEngine.AudioChorusFilter::set_delay
    (const void*)&AudioChorusFilter_Get_Custom_PropRate   ,  //  <- UnityEngine.AudioChorusFilter::get_rate
    (const void*)&AudioChorusFilter_Set_Custom_PropRate   ,  //  <- UnityEngine.AudioChorusFilter::set_rate
    (const void*)&AudioChorusFilter_Get_Custom_PropDepth  ,  //  <- UnityEngine.AudioChorusFilter::get_depth
    (const void*)&AudioChorusFilter_Set_Custom_PropDepth  ,  //  <- UnityEngine.AudioChorusFilter::set_depth
    (const void*)&AudioChorusFilter_Get_Custom_PropFeedback,  //  <- UnityEngine.AudioChorusFilter::get_feedback
    (const void*)&AudioChorusFilter_Set_Custom_PropFeedback,  //  <- UnityEngine.AudioChorusFilter::set_feedback
    (const void*)&AudioReverbFilter_Get_Custom_PropReverbPreset,  //  <- UnityEngine.AudioReverbFilter::get_reverbPreset
    (const void*)&AudioReverbFilter_Set_Custom_PropReverbPreset,  //  <- UnityEngine.AudioReverbFilter::set_reverbPreset
    (const void*)&AudioReverbFilter_Get_Custom_PropDryLevel,  //  <- UnityEngine.AudioReverbFilter::get_dryLevel
    (const void*)&AudioReverbFilter_Set_Custom_PropDryLevel,  //  <- UnityEngine.AudioReverbFilter::set_dryLevel
    (const void*)&AudioReverbFilter_Get_Custom_PropRoom   ,  //  <- UnityEngine.AudioReverbFilter::get_room
    (const void*)&AudioReverbFilter_Set_Custom_PropRoom   ,  //  <- UnityEngine.AudioReverbFilter::set_room
    (const void*)&AudioReverbFilter_Get_Custom_PropRoomHF ,  //  <- UnityEngine.AudioReverbFilter::get_roomHF
    (const void*)&AudioReverbFilter_Set_Custom_PropRoomHF ,  //  <- UnityEngine.AudioReverbFilter::set_roomHF
    (const void*)&AudioReverbFilter_Get_Custom_PropRoomRolloff,  //  <- UnityEngine.AudioReverbFilter::get_roomRolloff
    (const void*)&AudioReverbFilter_Set_Custom_PropRoomRolloff,  //  <- UnityEngine.AudioReverbFilter::set_roomRolloff
    (const void*)&AudioReverbFilter_Get_Custom_PropDecayTime,  //  <- UnityEngine.AudioReverbFilter::get_decayTime
    (const void*)&AudioReverbFilter_Set_Custom_PropDecayTime,  //  <- UnityEngine.AudioReverbFilter::set_decayTime
    (const void*)&AudioReverbFilter_Get_Custom_PropDecayHFRatio,  //  <- UnityEngine.AudioReverbFilter::get_decayHFRatio
    (const void*)&AudioReverbFilter_Set_Custom_PropDecayHFRatio,  //  <- UnityEngine.AudioReverbFilter::set_decayHFRatio
    (const void*)&AudioReverbFilter_Get_Custom_PropReflectionsLevel,  //  <- UnityEngine.AudioReverbFilter::get_reflectionsLevel
    (const void*)&AudioReverbFilter_Set_Custom_PropReflectionsLevel,  //  <- UnityEngine.AudioReverbFilter::set_reflectionsLevel
    (const void*)&AudioReverbFilter_Get_Custom_PropReflectionsDelay,  //  <- UnityEngine.AudioReverbFilter::get_reflectionsDelay
    (const void*)&AudioReverbFilter_Set_Custom_PropReflectionsDelay,  //  <- UnityEngine.AudioReverbFilter::set_reflectionsDelay
    (const void*)&AudioReverbFilter_Get_Custom_PropReverbLevel,  //  <- UnityEngine.AudioReverbFilter::get_reverbLevel
    (const void*)&AudioReverbFilter_Set_Custom_PropReverbLevel,  //  <- UnityEngine.AudioReverbFilter::set_reverbLevel
    (const void*)&AudioReverbFilter_Get_Custom_PropReverbDelay,  //  <- UnityEngine.AudioReverbFilter::get_reverbDelay
    (const void*)&AudioReverbFilter_Set_Custom_PropReverbDelay,  //  <- UnityEngine.AudioReverbFilter::set_reverbDelay
    (const void*)&AudioReverbFilter_Get_Custom_PropDiffusion,  //  <- UnityEngine.AudioReverbFilter::get_diffusion
    (const void*)&AudioReverbFilter_Set_Custom_PropDiffusion,  //  <- UnityEngine.AudioReverbFilter::set_diffusion
    (const void*)&AudioReverbFilter_Get_Custom_PropDensity,  //  <- UnityEngine.AudioReverbFilter::get_density
    (const void*)&AudioReverbFilter_Set_Custom_PropDensity,  //  <- UnityEngine.AudioReverbFilter::set_density
    (const void*)&AudioReverbFilter_Get_Custom_PropHfReference,  //  <- UnityEngine.AudioReverbFilter::get_hfReference
    (const void*)&AudioReverbFilter_Set_Custom_PropHfReference,  //  <- UnityEngine.AudioReverbFilter::set_hfReference
    (const void*)&AudioReverbFilter_Get_Custom_PropRoomLF ,  //  <- UnityEngine.AudioReverbFilter::get_roomLF
    (const void*)&AudioReverbFilter_Set_Custom_PropRoomLF ,  //  <- UnityEngine.AudioReverbFilter::set_roomLF
    (const void*)&AudioReverbFilter_Get_Custom_PropLFReference,  //  <- UnityEngine.AudioReverbFilter::get_lFReference
    (const void*)&AudioReverbFilter_Set_Custom_PropLFReference,  //  <- UnityEngine.AudioReverbFilter::set_lFReference
#endif
    (const void*)&PlayerPrefs_CUSTOM_TrySetInt            ,  //  <- UnityEngine.PlayerPrefs::TrySetInt
    (const void*)&PlayerPrefs_CUSTOM_TrySetFloat          ,  //  <- UnityEngine.PlayerPrefs::TrySetFloat
    (const void*)&PlayerPrefs_CUSTOM_TrySetSetString      ,  //  <- UnityEngine.PlayerPrefs::TrySetSetString
    (const void*)&PlayerPrefs_CUSTOM_GetInt               ,  //  <- UnityEngine.PlayerPrefs::GetInt
    (const void*)&PlayerPrefs_CUSTOM_GetFloat             ,  //  <- UnityEngine.PlayerPrefs::GetFloat
    (const void*)&PlayerPrefs_CUSTOM_GetString            ,  //  <- UnityEngine.PlayerPrefs::GetString
    (const void*)&PlayerPrefs_CUSTOM_HasKey               ,  //  <- UnityEngine.PlayerPrefs::HasKey
    (const void*)&PlayerPrefs_CUSTOM_DeleteKey            ,  //  <- UnityEngine.PlayerPrefs::DeleteKey
    (const void*)&PlayerPrefs_CUSTOM_DeleteAll            ,  //  <- UnityEngine.PlayerPrefs::DeleteAll
    (const void*)&PlayerPrefs_CUSTOM_Save                 ,  //  <- UnityEngine.PlayerPrefs::Save
    (const void*)&SystemInfo_Get_Custom_PropOperatingSystem,  //  <- UnityEngine.SystemInfo::get_operatingSystem
    (const void*)&SystemInfo_Get_Custom_PropProcessorType ,  //  <- UnityEngine.SystemInfo::get_processorType
    (const void*)&SystemInfo_Get_Custom_PropProcessorCount,  //  <- UnityEngine.SystemInfo::get_processorCount
    (const void*)&SystemInfo_Get_Custom_PropSystemMemorySize,  //  <- UnityEngine.SystemInfo::get_systemMemorySize
    (const void*)&SystemInfo_Get_Custom_PropGraphicsMemorySize,  //  <- UnityEngine.SystemInfo::get_graphicsMemorySize
    (const void*)&SystemInfo_Get_Custom_PropGraphicsDeviceName,  //  <- UnityEngine.SystemInfo::get_graphicsDeviceName
    (const void*)&SystemInfo_Get_Custom_PropGraphicsDeviceVendor,  //  <- UnityEngine.SystemInfo::get_graphicsDeviceVendor
    (const void*)&SystemInfo_Get_Custom_PropGraphicsDeviceID,  //  <- UnityEngine.SystemInfo::get_graphicsDeviceID
    (const void*)&SystemInfo_Get_Custom_PropGraphicsDeviceVendorID,  //  <- UnityEngine.SystemInfo::get_graphicsDeviceVendorID
    (const void*)&SystemInfo_Get_Custom_PropGraphicsDeviceVersion,  //  <- UnityEngine.SystemInfo::get_graphicsDeviceVersion
    (const void*)&SystemInfo_Get_Custom_PropGraphicsShaderLevel,  //  <- UnityEngine.SystemInfo::get_graphicsShaderLevel
    (const void*)&SystemInfo_Get_Custom_PropGraphicsPixelFillrate,  //  <- UnityEngine.SystemInfo::get_graphicsPixelFillrate
    (const void*)&SystemInfo_Get_Custom_PropSupportsShadows,  //  <- UnityEngine.SystemInfo::get_supportsShadows
    (const void*)&SystemInfo_Get_Custom_PropSupportsRenderTextures,  //  <- UnityEngine.SystemInfo::get_supportsRenderTextures
    (const void*)&SystemInfo_Get_Custom_PropSupportsImageEffects,  //  <- UnityEngine.SystemInfo::get_supportsImageEffects
    (const void*)&SystemInfo_Get_Custom_PropSupports3DTextures,  //  <- UnityEngine.SystemInfo::get_supports3DTextures
    (const void*)&SystemInfo_Get_Custom_PropSupportsComputeShaders,  //  <- UnityEngine.SystemInfo::get_supportsComputeShaders
    (const void*)&SystemInfo_Get_Custom_PropSupportsInstancing,  //  <- UnityEngine.SystemInfo::get_supportsInstancing
    (const void*)&SystemInfo_Get_Custom_PropSupportedRenderTargetCount,  //  <- UnityEngine.SystemInfo::get_supportedRenderTargetCount
    (const void*)&SystemInfo_Get_Custom_PropSupportsStencil,  //  <- UnityEngine.SystemInfo::get_supportsStencil
    (const void*)&SystemInfo_Get_Custom_PropSupportsVertexPrograms,  //  <- UnityEngine.SystemInfo::get_supportsVertexPrograms
    (const void*)&SystemInfo_CUSTOM_SupportsRenderTextureFormat,  //  <- UnityEngine.SystemInfo::SupportsRenderTextureFormat
    (const void*)&SystemInfo_Get_Custom_PropNpotSupport   ,  //  <- UnityEngine.SystemInfo::get_npotSupport
#if !UNITY_FLASH && !PLATFORM_WEBGL
    (const void*)&SystemInfo_Get_Custom_PropDeviceUniqueIdentifier,  //  <- UnityEngine.SystemInfo::get_deviceUniqueIdentifier
    (const void*)&SystemInfo_Get_Custom_PropDeviceName    ,  //  <- UnityEngine.SystemInfo::get_deviceName
    (const void*)&SystemInfo_Get_Custom_PropDeviceModel   ,  //  <- UnityEngine.SystemInfo::get_deviceModel
#endif
    (const void*)&SystemInfo_Get_Custom_PropSupportsAccelerometer,  //  <- UnityEngine.SystemInfo::get_supportsAccelerometer
    (const void*)&SystemInfo_Get_Custom_PropSupportsGyroscope,  //  <- UnityEngine.SystemInfo::get_supportsGyroscope
    (const void*)&SystemInfo_Get_Custom_PropSupportsLocationService,  //  <- UnityEngine.SystemInfo::get_supportsLocationService
    (const void*)&SystemInfo_Get_Custom_PropSupportsVibration,  //  <- UnityEngine.SystemInfo::get_supportsVibration
    (const void*)&SystemInfo_Get_Custom_PropDeviceType    ,  //  <- UnityEngine.SystemInfo::get_deviceType
    (const void*)&SystemInfo_Get_Custom_PropMaxTextureSize,  //  <- UnityEngine.SystemInfo::get_maxTextureSize
    (const void*)&Coroutine_CUSTOM_ReleaseCoroutine       ,  //  <- UnityEngine.Coroutine::ReleaseCoroutine
    (const void*)&ScriptableObject_CUSTOM_Internal_CreateScriptableObject,  //  <- UnityEngine.ScriptableObject::Internal_CreateScriptableObject
#if ENABLE_MONO
    (const void*)&ScriptableObject_CUSTOM_INTERNAL_CALL_SetDirty,  //  <- UnityEngine.ScriptableObject::INTERNAL_CALL_SetDirty
#endif
    (const void*)&ScriptableObject_CUSTOM_CreateInstance  ,  //  <- UnityEngine.ScriptableObject::CreateInstance
    (const void*)&ScriptableObject_CUSTOM_CreateInstanceFromType,  //  <- UnityEngine.ScriptableObject::CreateInstanceFromType
    (const void*)&Resources_CUSTOM_FindObjectsOfTypeAll   ,  //  <- UnityEngine.Resources::FindObjectsOfTypeAll
    (const void*)&Resources_CUSTOM_Load                   ,  //  <- UnityEngine.Resources::Load
    (const void*)&Resources_CUSTOM_LoadAll                ,  //  <- UnityEngine.Resources::LoadAll
    (const void*)&Resources_CUSTOM_GetBuiltinResource     ,  //  <- UnityEngine.Resources::GetBuiltinResource
    (const void*)&Resources_CUSTOM_LoadAssetAtPath        ,  //  <- UnityEngine.Resources::LoadAssetAtPath
    (const void*)&Resources_CUSTOM_UnloadAsset            ,  //  <- UnityEngine.Resources::UnloadAsset
    (const void*)&Resources_CUSTOM_UnloadUnusedAssets     ,  //  <- UnityEngine.Resources::UnloadUnusedAssets
#if ENABLE_WWW
    (const void*)&AssetBundleCreateRequest_Get_Custom_PropAssetBundle,  //  <- UnityEngine.AssetBundleCreateRequest::get_assetBundle
    (const void*)&AssetBundle_CUSTOM_CreateFromMemory     ,  //  <- UnityEngine.AssetBundle::CreateFromMemory
#endif
    (const void*)&AssetBundle_CUSTOM_CreateFromFile       ,  //  <- UnityEngine.AssetBundle::CreateFromFile
    (const void*)&AssetBundle_Get_Custom_PropMainAsset    ,  //  <- UnityEngine.AssetBundle::get_mainAsset
    (const void*)&AssetBundle_CUSTOM_Contains             ,  //  <- UnityEngine.AssetBundle::Contains
    (const void*)&AssetBundle_CUSTOM_Load                 ,  //  <- UnityEngine.AssetBundle::Load
    (const void*)&AssetBundle_CUSTOM_LoadAsync            ,  //  <- UnityEngine.AssetBundle::LoadAsync
    (const void*)&AssetBundle_CUSTOM_LoadAll              ,  //  <- UnityEngine.AssetBundle::LoadAll
    (const void*)&AssetBundle_CUSTOM_Unload               ,  //  <- UnityEngine.AssetBundle::Unload
    (const void*)&Profiler_Get_Custom_PropSupported       ,  //  <- UnityEngine.Profiler::get_supported
    (const void*)&Profiler_Get_Custom_PropLogFile         ,  //  <- UnityEngine.Profiler::get_logFile
    (const void*)&Profiler_Set_Custom_PropLogFile         ,  //  <- UnityEngine.Profiler::set_logFile
    (const void*)&Profiler_Get_Custom_PropEnableBinaryLog ,  //  <- UnityEngine.Profiler::get_enableBinaryLog
    (const void*)&Profiler_Set_Custom_PropEnableBinaryLog ,  //  <- UnityEngine.Profiler::set_enableBinaryLog
    (const void*)&Profiler_Get_Custom_PropEnabled         ,  //  <- UnityEngine.Profiler::get_enabled
    (const void*)&Profiler_Set_Custom_PropEnabled         ,  //  <- UnityEngine.Profiler::set_enabled
    (const void*)&Profiler_CUSTOM_AddFramesFromFile       ,  //  <- UnityEngine.Profiler::AddFramesFromFile
    (const void*)&Profiler_CUSTOM_BeginSample             ,  //  <- UnityEngine.Profiler::BeginSample
    (const void*)&Profiler_CUSTOM_BeginSampleOnly         ,  //  <- UnityEngine.Profiler::BeginSampleOnly
    (const void*)&Profiler_CUSTOM_EndSample               ,  //  <- UnityEngine.Profiler::EndSample
    (const void*)&Profiler_Get_Custom_PropUsedHeapSize    ,  //  <- UnityEngine.Profiler::get_usedHeapSize
    (const void*)&Profiler_CUSTOM_GetRuntimeMemorySize    ,  //  <- UnityEngine.Profiler::GetRuntimeMemorySize
    (const void*)&Profiler_CUSTOM_GetMonoHeapSize         ,  //  <- UnityEngine.Profiler::GetMonoHeapSize
    (const void*)&Profiler_CUSTOM_GetMonoUsedSize         ,  //  <- UnityEngine.Profiler::GetMonoUsedSize
    (const void*)&Profiler_CUSTOM_GetTotalAllocatedMemory ,  //  <- UnityEngine.Profiler::GetTotalAllocatedMemory
    (const void*)&Profiler_CUSTOM_GetTotalUnusedReservedMemory,  //  <- UnityEngine.Profiler::GetTotalUnusedReservedMemory
    (const void*)&Profiler_CUSTOM_GetTotalReservedMemory  ,  //  <- UnityEngine.Profiler::GetTotalReservedMemory
#if ENABLE_MONO
    (const void*)&Reproduction_CUSTOM_CaptureScreenshot   ,  //  <- UnityEngineInternal.Reproduction::CaptureScreenshot
#endif
    NULL
};

void ExportBaseClassBindings();
void ExportBaseClassBindings()
{
    for (int i = 0; s_BaseClass_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_BaseClass_IcallNames [i], s_BaseClass_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportBaseClassBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(LayerMask_CUSTOM_LayerToName); //  <- UnityEngine.LayerMask::LayerToName
    SET_METRO_BINDING(LayerMask_CUSTOM_NameToLayer); //  <- UnityEngine.LayerMask::NameToLayer
#if ENABLE_AUDIO_FMOD
    SET_METRO_BINDING(AudioSettings_Get_Custom_PropDriverCaps); //  <- UnityEngine.AudioSettings::get_driverCaps
    SET_METRO_BINDING(AudioSettings_Get_Custom_PropSpeakerMode); //  <- UnityEngine.AudioSettings::get_speakerMode
    SET_METRO_BINDING(AudioSettings_Set_Custom_PropSpeakerMode); //  <- UnityEngine.AudioSettings::set_speakerMode
    SET_METRO_BINDING(AudioSettings_Get_Custom_PropDspTime); //  <- UnityEngine.AudioSettings::get_dspTime
    SET_METRO_BINDING(AudioSettings_Get_Custom_PropOutputSampleRate); //  <- UnityEngine.AudioSettings::get_outputSampleRate
    SET_METRO_BINDING(AudioSettings_Set_Custom_PropOutputSampleRate); //  <- UnityEngine.AudioSettings::set_outputSampleRate
    SET_METRO_BINDING(AudioSettings_CUSTOM_SetDSPBufferSize); //  <- UnityEngine.AudioSettings::SetDSPBufferSize
    SET_METRO_BINDING(AudioSettings_CUSTOM_GetDSPBufferSize); //  <- UnityEngine.AudioSettings::GetDSPBufferSize
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioClip_Get_Custom_PropLength); //  <- UnityEngine.AudioClip::get_length
    SET_METRO_BINDING(AudioClip_Get_Custom_PropSamples); //  <- UnityEngine.AudioClip::get_samples
    SET_METRO_BINDING(AudioClip_Get_Custom_PropChannels); //  <- UnityEngine.AudioClip::get_channels
    SET_METRO_BINDING(AudioClip_Get_Custom_PropFrequency); //  <- UnityEngine.AudioClip::get_frequency
    SET_METRO_BINDING(AudioClip_Get_Custom_PropIsReadyToPlay); //  <- UnityEngine.AudioClip::get_isReadyToPlay
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    SET_METRO_BINDING(AudioClip_CUSTOM_GetData); //  <- UnityEngine.AudioClip::GetData
    SET_METRO_BINDING(AudioClip_CUSTOM_SetData); //  <- UnityEngine.AudioClip::SetData
    SET_METRO_BINDING(AudioClip_CUSTOM_Construct_Internal); //  <- UnityEngine.AudioClip::Construct_Internal
    SET_METRO_BINDING(AudioClip_CUSTOM_Init_Internal); //  <- UnityEngine.AudioClip::Init_Internal
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioListener_Get_Custom_PropVolume); //  <- UnityEngine.AudioListener::get_volume
    SET_METRO_BINDING(AudioListener_Set_Custom_PropVolume); //  <- UnityEngine.AudioListener::set_volume
    SET_METRO_BINDING(AudioListener_Get_Custom_PropPause); //  <- UnityEngine.AudioListener::get_pause
    SET_METRO_BINDING(AudioListener_Set_Custom_PropPause); //  <- UnityEngine.AudioListener::set_pause
    SET_METRO_BINDING(AudioListener_Get_Custom_PropVelocityUpdateMode); //  <- UnityEngine.AudioListener::get_velocityUpdateMode
    SET_METRO_BINDING(AudioListener_Set_Custom_PropVelocityUpdateMode); //  <- UnityEngine.AudioListener::set_velocityUpdateMode
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    SET_METRO_BINDING(AudioListener_CUSTOM_GetOutputDataHelper); //  <- UnityEngine.AudioListener::GetOutputDataHelper
    SET_METRO_BINDING(AudioListener_CUSTOM_GetSpectrumDataHelper); //  <- UnityEngine.AudioListener::GetSpectrumDataHelper
#endif
#if ENABLE_MICROPHONE
    SET_METRO_BINDING(Microphone_CUSTOM_Start); //  <- UnityEngine.Microphone::Start
    SET_METRO_BINDING(Microphone_CUSTOM_End); //  <- UnityEngine.Microphone::End
    SET_METRO_BINDING(Microphone_Get_Custom_PropDevices); //  <- UnityEngine.Microphone::get_devices
    SET_METRO_BINDING(Microphone_CUSTOM_IsRecording); //  <- UnityEngine.Microphone::IsRecording
    SET_METRO_BINDING(Microphone_CUSTOM_GetPosition); //  <- UnityEngine.Microphone::GetPosition
    SET_METRO_BINDING(Microphone_CUSTOM_GetDeviceCaps); //  <- UnityEngine.Microphone::GetDeviceCaps
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioSource_Get_Custom_PropVolume); //  <- UnityEngine.AudioSource::get_volume
    SET_METRO_BINDING(AudioSource_Set_Custom_PropVolume); //  <- UnityEngine.AudioSource::set_volume
    SET_METRO_BINDING(AudioSource_Get_Custom_PropPitch); //  <- UnityEngine.AudioSource::get_pitch
    SET_METRO_BINDING(AudioSource_Set_Custom_PropPitch); //  <- UnityEngine.AudioSource::set_pitch
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
    SET_METRO_BINDING(AudioSource_Get_Custom_PropTime); //  <- UnityEngine.AudioSource::get_time
    SET_METRO_BINDING(AudioSource_Set_Custom_PropTime); //  <- UnityEngine.AudioSource::set_time
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    SET_METRO_BINDING(AudioSource_Get_Custom_PropTimeSamples); //  <- UnityEngine.AudioSource::get_timeSamples
    SET_METRO_BINDING(AudioSource_Set_Custom_PropTimeSamples); //  <- UnityEngine.AudioSource::set_timeSamples
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioSource_Get_Custom_PropClip); //  <- UnityEngine.AudioSource::get_clip
    SET_METRO_BINDING(AudioSource_Set_Custom_PropClip); //  <- UnityEngine.AudioSource::set_clip
    SET_METRO_BINDING(AudioSource_CUSTOM_Play); //  <- UnityEngine.AudioSource::Play
    SET_METRO_BINDING(AudioSource_CUSTOM_PlayDelayed); //  <- UnityEngine.AudioSource::PlayDelayed
    SET_METRO_BINDING(AudioSource_CUSTOM_PlayScheduled); //  <- UnityEngine.AudioSource::PlayScheduled
    SET_METRO_BINDING(AudioSource_CUSTOM_SetScheduledStartTime); //  <- UnityEngine.AudioSource::SetScheduledStartTime
    SET_METRO_BINDING(AudioSource_CUSTOM_SetScheduledEndTime); //  <- UnityEngine.AudioSource::SetScheduledEndTime
    SET_METRO_BINDING(AudioSource_CUSTOM_Stop); //  <- UnityEngine.AudioSource::Stop
    SET_METRO_BINDING(AudioSource_CUSTOM_INTERNAL_CALL_Pause); //  <- UnityEngine.AudioSource::INTERNAL_CALL_Pause
    SET_METRO_BINDING(AudioSource_Get_Custom_PropIsPlaying); //  <- UnityEngine.AudioSource::get_isPlaying
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO)
    SET_METRO_BINDING(AudioSource_CUSTOM_PlayOneShot); //  <- UnityEngine.AudioSource::PlayOneShot
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioSource_Get_Custom_PropLoop); //  <- UnityEngine.AudioSource::get_loop
    SET_METRO_BINDING(AudioSource_Set_Custom_PropLoop); //  <- UnityEngine.AudioSource::set_loop
    SET_METRO_BINDING(AudioSource_Get_Custom_PropIgnoreListenerVolume); //  <- UnityEngine.AudioSource::get_ignoreListenerVolume
    SET_METRO_BINDING(AudioSource_Set_Custom_PropIgnoreListenerVolume); //  <- UnityEngine.AudioSource::set_ignoreListenerVolume
    SET_METRO_BINDING(AudioSource_Get_Custom_PropPlayOnAwake); //  <- UnityEngine.AudioSource::get_playOnAwake
    SET_METRO_BINDING(AudioSource_Set_Custom_PropPlayOnAwake); //  <- UnityEngine.AudioSource::set_playOnAwake
    SET_METRO_BINDING(AudioSource_Get_Custom_PropIgnoreListenerPause); //  <- UnityEngine.AudioSource::get_ignoreListenerPause
    SET_METRO_BINDING(AudioSource_Set_Custom_PropIgnoreListenerPause); //  <- UnityEngine.AudioSource::set_ignoreListenerPause
    SET_METRO_BINDING(AudioSource_Get_Custom_PropVelocityUpdateMode); //  <- UnityEngine.AudioSource::get_velocityUpdateMode
    SET_METRO_BINDING(AudioSource_Set_Custom_PropVelocityUpdateMode); //  <- UnityEngine.AudioSource::set_velocityUpdateMode
    SET_METRO_BINDING(AudioSource_Get_Custom_PropPanLevel); //  <- UnityEngine.AudioSource::get_panLevel
    SET_METRO_BINDING(AudioSource_Set_Custom_PropPanLevel); //  <- UnityEngine.AudioSource::set_panLevel
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    SET_METRO_BINDING(AudioSource_Get_Custom_PropBypassEffects); //  <- UnityEngine.AudioSource::get_bypassEffects
    SET_METRO_BINDING(AudioSource_Set_Custom_PropBypassEffects); //  <- UnityEngine.AudioSource::set_bypassEffects
    SET_METRO_BINDING(AudioSource_Get_Custom_PropBypassListenerEffects); //  <- UnityEngine.AudioSource::get_bypassListenerEffects
    SET_METRO_BINDING(AudioSource_Set_Custom_PropBypassListenerEffects); //  <- UnityEngine.AudioSource::set_bypassListenerEffects
    SET_METRO_BINDING(AudioSource_Get_Custom_PropBypassReverbZones); //  <- UnityEngine.AudioSource::get_bypassReverbZones
    SET_METRO_BINDING(AudioSource_Set_Custom_PropBypassReverbZones); //  <- UnityEngine.AudioSource::set_bypassReverbZones
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioSource_Get_Custom_PropDopplerLevel); //  <- UnityEngine.AudioSource::get_dopplerLevel
    SET_METRO_BINDING(AudioSource_Set_Custom_PropDopplerLevel); //  <- UnityEngine.AudioSource::set_dopplerLevel
    SET_METRO_BINDING(AudioSource_Get_Custom_PropSpread); //  <- UnityEngine.AudioSource::get_spread
    SET_METRO_BINDING(AudioSource_Set_Custom_PropSpread); //  <- UnityEngine.AudioSource::set_spread
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    SET_METRO_BINDING(AudioSource_Get_Custom_PropPriority); //  <- UnityEngine.AudioSource::get_priority
    SET_METRO_BINDING(AudioSource_Set_Custom_PropPriority); //  <- UnityEngine.AudioSource::set_priority
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioSource_Get_Custom_PropMute); //  <- UnityEngine.AudioSource::get_mute
    SET_METRO_BINDING(AudioSource_Set_Custom_PropMute); //  <- UnityEngine.AudioSource::set_mute
    SET_METRO_BINDING(AudioSource_Get_Custom_PropMinDistance); //  <- UnityEngine.AudioSource::get_minDistance
    SET_METRO_BINDING(AudioSource_Set_Custom_PropMinDistance); //  <- UnityEngine.AudioSource::set_minDistance
    SET_METRO_BINDING(AudioSource_Get_Custom_PropMaxDistance); //  <- UnityEngine.AudioSource::get_maxDistance
    SET_METRO_BINDING(AudioSource_Set_Custom_PropMaxDistance); //  <- UnityEngine.AudioSource::set_maxDistance
    SET_METRO_BINDING(AudioSource_Get_Custom_PropPan); //  <- UnityEngine.AudioSource::get_pan
    SET_METRO_BINDING(AudioSource_Set_Custom_PropPan); //  <- UnityEngine.AudioSource::set_pan
    SET_METRO_BINDING(AudioSource_Get_Custom_PropRolloffMode); //  <- UnityEngine.AudioSource::get_rolloffMode
    SET_METRO_BINDING(AudioSource_Set_Custom_PropRolloffMode); //  <- UnityEngine.AudioSource::set_rolloffMode
    SET_METRO_BINDING(AudioSource_CUSTOM_GetOutputDataHelper); //  <- UnityEngine.AudioSource::GetOutputDataHelper
#endif
#if (ENABLE_AUDIO) && (ENABLE_AUDIO_FMOD)
    SET_METRO_BINDING(AudioSource_CUSTOM_GetSpectrumDataHelper); //  <- UnityEngine.AudioSource::GetSpectrumDataHelper
#endif
#if ENABLE_AUDIO
    SET_METRO_BINDING(AudioSource_Get_Custom_PropMinVolume); //  <- UnityEngine.AudioSource::get_minVolume
    SET_METRO_BINDING(AudioSource_Set_Custom_PropMinVolume); //  <- UnityEngine.AudioSource::set_minVolume
    SET_METRO_BINDING(AudioSource_Get_Custom_PropMaxVolume); //  <- UnityEngine.AudioSource::get_maxVolume
    SET_METRO_BINDING(AudioSource_Set_Custom_PropMaxVolume); //  <- UnityEngine.AudioSource::set_maxVolume
    SET_METRO_BINDING(AudioSource_Get_Custom_PropRolloffFactor); //  <- UnityEngine.AudioSource::get_rolloffFactor
    SET_METRO_BINDING(AudioSource_Set_Custom_PropRolloffFactor); //  <- UnityEngine.AudioSource::set_rolloffFactor
#endif
#if ENABLE_AUDIO_FMOD
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropMinDistance); //  <- UnityEngine.AudioReverbZone::get_minDistance
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropMinDistance); //  <- UnityEngine.AudioReverbZone::set_minDistance
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropMaxDistance); //  <- UnityEngine.AudioReverbZone::get_maxDistance
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropMaxDistance); //  <- UnityEngine.AudioReverbZone::set_maxDistance
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropReverbPreset); //  <- UnityEngine.AudioReverbZone::get_reverbPreset
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropReverbPreset); //  <- UnityEngine.AudioReverbZone::set_reverbPreset
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropRoom); //  <- UnityEngine.AudioReverbZone::get_room
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropRoom); //  <- UnityEngine.AudioReverbZone::set_room
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropRoomHF); //  <- UnityEngine.AudioReverbZone::get_roomHF
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropRoomHF); //  <- UnityEngine.AudioReverbZone::set_roomHF
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropRoomLF); //  <- UnityEngine.AudioReverbZone::get_roomLF
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropRoomLF); //  <- UnityEngine.AudioReverbZone::set_roomLF
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropDecayTime); //  <- UnityEngine.AudioReverbZone::get_decayTime
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropDecayTime); //  <- UnityEngine.AudioReverbZone::set_decayTime
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropDecayHFRatio); //  <- UnityEngine.AudioReverbZone::get_decayHFRatio
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropDecayHFRatio); //  <- UnityEngine.AudioReverbZone::set_decayHFRatio
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropReflections); //  <- UnityEngine.AudioReverbZone::get_reflections
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropReflections); //  <- UnityEngine.AudioReverbZone::set_reflections
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropReflectionsDelay); //  <- UnityEngine.AudioReverbZone::get_reflectionsDelay
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropReflectionsDelay); //  <- UnityEngine.AudioReverbZone::set_reflectionsDelay
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropReverb); //  <- UnityEngine.AudioReverbZone::get_reverb
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropReverb); //  <- UnityEngine.AudioReverbZone::set_reverb
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropReverbDelay); //  <- UnityEngine.AudioReverbZone::get_reverbDelay
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropReverbDelay); //  <- UnityEngine.AudioReverbZone::set_reverbDelay
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropHFReference); //  <- UnityEngine.AudioReverbZone::get_HFReference
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropHFReference); //  <- UnityEngine.AudioReverbZone::set_HFReference
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropLFReference); //  <- UnityEngine.AudioReverbZone::get_LFReference
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropLFReference); //  <- UnityEngine.AudioReverbZone::set_LFReference
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropRoomRolloffFactor); //  <- UnityEngine.AudioReverbZone::get_roomRolloffFactor
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropRoomRolloffFactor); //  <- UnityEngine.AudioReverbZone::set_roomRolloffFactor
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropDiffusion); //  <- UnityEngine.AudioReverbZone::get_diffusion
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropDiffusion); //  <- UnityEngine.AudioReverbZone::set_diffusion
    SET_METRO_BINDING(AudioReverbZone_Get_Custom_PropDensity); //  <- UnityEngine.AudioReverbZone::get_density
    SET_METRO_BINDING(AudioReverbZone_Set_Custom_PropDensity); //  <- UnityEngine.AudioReverbZone::set_density
    SET_METRO_BINDING(AudioLowPassFilter_Get_Custom_PropCutoffFrequency); //  <- UnityEngine.AudioLowPassFilter::get_cutoffFrequency
    SET_METRO_BINDING(AudioLowPassFilter_Set_Custom_PropCutoffFrequency); //  <- UnityEngine.AudioLowPassFilter::set_cutoffFrequency
    SET_METRO_BINDING(AudioLowPassFilter_Get_Custom_PropLowpassResonaceQ); //  <- UnityEngine.AudioLowPassFilter::get_lowpassResonaceQ
    SET_METRO_BINDING(AudioLowPassFilter_Set_Custom_PropLowpassResonaceQ); //  <- UnityEngine.AudioLowPassFilter::set_lowpassResonaceQ
    SET_METRO_BINDING(AudioHighPassFilter_Get_Custom_PropCutoffFrequency); //  <- UnityEngine.AudioHighPassFilter::get_cutoffFrequency
    SET_METRO_BINDING(AudioHighPassFilter_Set_Custom_PropCutoffFrequency); //  <- UnityEngine.AudioHighPassFilter::set_cutoffFrequency
    SET_METRO_BINDING(AudioHighPassFilter_Get_Custom_PropHighpassResonaceQ); //  <- UnityEngine.AudioHighPassFilter::get_highpassResonaceQ
    SET_METRO_BINDING(AudioHighPassFilter_Set_Custom_PropHighpassResonaceQ); //  <- UnityEngine.AudioHighPassFilter::set_highpassResonaceQ
    SET_METRO_BINDING(AudioDistortionFilter_Get_Custom_PropDistortionLevel); //  <- UnityEngine.AudioDistortionFilter::get_distortionLevel
    SET_METRO_BINDING(AudioDistortionFilter_Set_Custom_PropDistortionLevel); //  <- UnityEngine.AudioDistortionFilter::set_distortionLevel
    SET_METRO_BINDING(AudioEchoFilter_Get_Custom_PropDelay); //  <- UnityEngine.AudioEchoFilter::get_delay
    SET_METRO_BINDING(AudioEchoFilter_Set_Custom_PropDelay); //  <- UnityEngine.AudioEchoFilter::set_delay
    SET_METRO_BINDING(AudioEchoFilter_Get_Custom_PropDecayRatio); //  <- UnityEngine.AudioEchoFilter::get_decayRatio
    SET_METRO_BINDING(AudioEchoFilter_Set_Custom_PropDecayRatio); //  <- UnityEngine.AudioEchoFilter::set_decayRatio
    SET_METRO_BINDING(AudioEchoFilter_Get_Custom_PropDryMix); //  <- UnityEngine.AudioEchoFilter::get_dryMix
    SET_METRO_BINDING(AudioEchoFilter_Set_Custom_PropDryMix); //  <- UnityEngine.AudioEchoFilter::set_dryMix
    SET_METRO_BINDING(AudioEchoFilter_Get_Custom_PropWetMix); //  <- UnityEngine.AudioEchoFilter::get_wetMix
    SET_METRO_BINDING(AudioEchoFilter_Set_Custom_PropWetMix); //  <- UnityEngine.AudioEchoFilter::set_wetMix
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropDryMix); //  <- UnityEngine.AudioChorusFilter::get_dryMix
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropDryMix); //  <- UnityEngine.AudioChorusFilter::set_dryMix
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropWetMix1); //  <- UnityEngine.AudioChorusFilter::get_wetMix1
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropWetMix1); //  <- UnityEngine.AudioChorusFilter::set_wetMix1
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropWetMix2); //  <- UnityEngine.AudioChorusFilter::get_wetMix2
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropWetMix2); //  <- UnityEngine.AudioChorusFilter::set_wetMix2
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropWetMix3); //  <- UnityEngine.AudioChorusFilter::get_wetMix3
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropWetMix3); //  <- UnityEngine.AudioChorusFilter::set_wetMix3
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropDelay); //  <- UnityEngine.AudioChorusFilter::get_delay
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropDelay); //  <- UnityEngine.AudioChorusFilter::set_delay
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropRate); //  <- UnityEngine.AudioChorusFilter::get_rate
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropRate); //  <- UnityEngine.AudioChorusFilter::set_rate
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropDepth); //  <- UnityEngine.AudioChorusFilter::get_depth
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropDepth); //  <- UnityEngine.AudioChorusFilter::set_depth
    SET_METRO_BINDING(AudioChorusFilter_Get_Custom_PropFeedback); //  <- UnityEngine.AudioChorusFilter::get_feedback
    SET_METRO_BINDING(AudioChorusFilter_Set_Custom_PropFeedback); //  <- UnityEngine.AudioChorusFilter::set_feedback
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropReverbPreset); //  <- UnityEngine.AudioReverbFilter::get_reverbPreset
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropReverbPreset); //  <- UnityEngine.AudioReverbFilter::set_reverbPreset
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropDryLevel); //  <- UnityEngine.AudioReverbFilter::get_dryLevel
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropDryLevel); //  <- UnityEngine.AudioReverbFilter::set_dryLevel
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropRoom); //  <- UnityEngine.AudioReverbFilter::get_room
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropRoom); //  <- UnityEngine.AudioReverbFilter::set_room
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropRoomHF); //  <- UnityEngine.AudioReverbFilter::get_roomHF
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropRoomHF); //  <- UnityEngine.AudioReverbFilter::set_roomHF
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropRoomRolloff); //  <- UnityEngine.AudioReverbFilter::get_roomRolloff
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropRoomRolloff); //  <- UnityEngine.AudioReverbFilter::set_roomRolloff
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropDecayTime); //  <- UnityEngine.AudioReverbFilter::get_decayTime
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropDecayTime); //  <- UnityEngine.AudioReverbFilter::set_decayTime
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropDecayHFRatio); //  <- UnityEngine.AudioReverbFilter::get_decayHFRatio
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropDecayHFRatio); //  <- UnityEngine.AudioReverbFilter::set_decayHFRatio
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropReflectionsLevel); //  <- UnityEngine.AudioReverbFilter::get_reflectionsLevel
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropReflectionsLevel); //  <- UnityEngine.AudioReverbFilter::set_reflectionsLevel
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropReflectionsDelay); //  <- UnityEngine.AudioReverbFilter::get_reflectionsDelay
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropReflectionsDelay); //  <- UnityEngine.AudioReverbFilter::set_reflectionsDelay
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropReverbLevel); //  <- UnityEngine.AudioReverbFilter::get_reverbLevel
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropReverbLevel); //  <- UnityEngine.AudioReverbFilter::set_reverbLevel
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropReverbDelay); //  <- UnityEngine.AudioReverbFilter::get_reverbDelay
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropReverbDelay); //  <- UnityEngine.AudioReverbFilter::set_reverbDelay
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropDiffusion); //  <- UnityEngine.AudioReverbFilter::get_diffusion
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropDiffusion); //  <- UnityEngine.AudioReverbFilter::set_diffusion
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropDensity); //  <- UnityEngine.AudioReverbFilter::get_density
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropDensity); //  <- UnityEngine.AudioReverbFilter::set_density
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropHfReference); //  <- UnityEngine.AudioReverbFilter::get_hfReference
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropHfReference); //  <- UnityEngine.AudioReverbFilter::set_hfReference
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropRoomLF); //  <- UnityEngine.AudioReverbFilter::get_roomLF
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropRoomLF); //  <- UnityEngine.AudioReverbFilter::set_roomLF
    SET_METRO_BINDING(AudioReverbFilter_Get_Custom_PropLFReference); //  <- UnityEngine.AudioReverbFilter::get_lFReference
    SET_METRO_BINDING(AudioReverbFilter_Set_Custom_PropLFReference); //  <- UnityEngine.AudioReverbFilter::set_lFReference
#endif
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_TrySetInt); //  <- UnityEngine.PlayerPrefs::TrySetInt
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_TrySetFloat); //  <- UnityEngine.PlayerPrefs::TrySetFloat
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_TrySetSetString); //  <- UnityEngine.PlayerPrefs::TrySetSetString
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_GetInt); //  <- UnityEngine.PlayerPrefs::GetInt
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_GetFloat); //  <- UnityEngine.PlayerPrefs::GetFloat
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_GetString); //  <- UnityEngine.PlayerPrefs::GetString
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_HasKey); //  <- UnityEngine.PlayerPrefs::HasKey
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_DeleteKey); //  <- UnityEngine.PlayerPrefs::DeleteKey
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_DeleteAll); //  <- UnityEngine.PlayerPrefs::DeleteAll
    SET_METRO_BINDING(PlayerPrefs_CUSTOM_Save); //  <- UnityEngine.PlayerPrefs::Save
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropOperatingSystem); //  <- UnityEngine.SystemInfo::get_operatingSystem
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropProcessorType); //  <- UnityEngine.SystemInfo::get_processorType
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropProcessorCount); //  <- UnityEngine.SystemInfo::get_processorCount
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSystemMemorySize); //  <- UnityEngine.SystemInfo::get_systemMemorySize
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsMemorySize); //  <- UnityEngine.SystemInfo::get_graphicsMemorySize
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsDeviceName); //  <- UnityEngine.SystemInfo::get_graphicsDeviceName
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsDeviceVendor); //  <- UnityEngine.SystemInfo::get_graphicsDeviceVendor
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsDeviceID); //  <- UnityEngine.SystemInfo::get_graphicsDeviceID
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsDeviceVendorID); //  <- UnityEngine.SystemInfo::get_graphicsDeviceVendorID
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsDeviceVersion); //  <- UnityEngine.SystemInfo::get_graphicsDeviceVersion
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsShaderLevel); //  <- UnityEngine.SystemInfo::get_graphicsShaderLevel
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropGraphicsPixelFillrate); //  <- UnityEngine.SystemInfo::get_graphicsPixelFillrate
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsShadows); //  <- UnityEngine.SystemInfo::get_supportsShadows
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsRenderTextures); //  <- UnityEngine.SystemInfo::get_supportsRenderTextures
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsImageEffects); //  <- UnityEngine.SystemInfo::get_supportsImageEffects
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupports3DTextures); //  <- UnityEngine.SystemInfo::get_supports3DTextures
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsComputeShaders); //  <- UnityEngine.SystemInfo::get_supportsComputeShaders
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsInstancing); //  <- UnityEngine.SystemInfo::get_supportsInstancing
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportedRenderTargetCount); //  <- UnityEngine.SystemInfo::get_supportedRenderTargetCount
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsStencil); //  <- UnityEngine.SystemInfo::get_supportsStencil
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsVertexPrograms); //  <- UnityEngine.SystemInfo::get_supportsVertexPrograms
    SET_METRO_BINDING(SystemInfo_CUSTOM_SupportsRenderTextureFormat); //  <- UnityEngine.SystemInfo::SupportsRenderTextureFormat
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropNpotSupport); //  <- UnityEngine.SystemInfo::get_npotSupport
#if !UNITY_FLASH && !PLATFORM_WEBGL
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropDeviceUniqueIdentifier); //  <- UnityEngine.SystemInfo::get_deviceUniqueIdentifier
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropDeviceName); //  <- UnityEngine.SystemInfo::get_deviceName
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropDeviceModel); //  <- UnityEngine.SystemInfo::get_deviceModel
#endif
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsAccelerometer); //  <- UnityEngine.SystemInfo::get_supportsAccelerometer
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsGyroscope); //  <- UnityEngine.SystemInfo::get_supportsGyroscope
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsLocationService); //  <- UnityEngine.SystemInfo::get_supportsLocationService
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropSupportsVibration); //  <- UnityEngine.SystemInfo::get_supportsVibration
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropDeviceType); //  <- UnityEngine.SystemInfo::get_deviceType
    SET_METRO_BINDING(SystemInfo_Get_Custom_PropMaxTextureSize); //  <- UnityEngine.SystemInfo::get_maxTextureSize
    SET_METRO_BINDING(Coroutine_CUSTOM_ReleaseCoroutine); //  <- UnityEngine.Coroutine::ReleaseCoroutine
    SET_METRO_BINDING(ScriptableObject_CUSTOM_Internal_CreateScriptableObject); //  <- UnityEngine.ScriptableObject::Internal_CreateScriptableObject
#if ENABLE_MONO
    SET_METRO_BINDING(ScriptableObject_CUSTOM_INTERNAL_CALL_SetDirty); //  <- UnityEngine.ScriptableObject::INTERNAL_CALL_SetDirty
#endif
    SET_METRO_BINDING(ScriptableObject_CUSTOM_CreateInstance); //  <- UnityEngine.ScriptableObject::CreateInstance
    SET_METRO_BINDING(ScriptableObject_CUSTOM_CreateInstanceFromType); //  <- UnityEngine.ScriptableObject::CreateInstanceFromType
    SET_METRO_BINDING(Resources_CUSTOM_FindObjectsOfTypeAll); //  <- UnityEngine.Resources::FindObjectsOfTypeAll
    SET_METRO_BINDING(Resources_CUSTOM_Load); //  <- UnityEngine.Resources::Load
    SET_METRO_BINDING(Resources_CUSTOM_LoadAll); //  <- UnityEngine.Resources::LoadAll
    SET_METRO_BINDING(Resources_CUSTOM_GetBuiltinResource); //  <- UnityEngine.Resources::GetBuiltinResource
    SET_METRO_BINDING(Resources_CUSTOM_LoadAssetAtPath); //  <- UnityEngine.Resources::LoadAssetAtPath
    SET_METRO_BINDING(Resources_CUSTOM_UnloadAsset); //  <- UnityEngine.Resources::UnloadAsset
    SET_METRO_BINDING(Resources_CUSTOM_UnloadUnusedAssets); //  <- UnityEngine.Resources::UnloadUnusedAssets
#if ENABLE_WWW
    SET_METRO_BINDING(AssetBundleCreateRequest_Get_Custom_PropAssetBundle); //  <- UnityEngine.AssetBundleCreateRequest::get_assetBundle
    SET_METRO_BINDING(AssetBundle_CUSTOM_CreateFromMemory); //  <- UnityEngine.AssetBundle::CreateFromMemory
#endif
    SET_METRO_BINDING(AssetBundle_CUSTOM_CreateFromFile); //  <- UnityEngine.AssetBundle::CreateFromFile
    SET_METRO_BINDING(AssetBundle_Get_Custom_PropMainAsset); //  <- UnityEngine.AssetBundle::get_mainAsset
    SET_METRO_BINDING(AssetBundle_CUSTOM_Contains); //  <- UnityEngine.AssetBundle::Contains
    SET_METRO_BINDING(AssetBundle_CUSTOM_Load); //  <- UnityEngine.AssetBundle::Load
    SET_METRO_BINDING(AssetBundle_CUSTOM_LoadAsync); //  <- UnityEngine.AssetBundle::LoadAsync
    SET_METRO_BINDING(AssetBundle_CUSTOM_LoadAll); //  <- UnityEngine.AssetBundle::LoadAll
    SET_METRO_BINDING(AssetBundle_CUSTOM_Unload); //  <- UnityEngine.AssetBundle::Unload
    SET_METRO_BINDING(Profiler_Get_Custom_PropSupported); //  <- UnityEngine.Profiler::get_supported
    SET_METRO_BINDING(Profiler_Get_Custom_PropLogFile); //  <- UnityEngine.Profiler::get_logFile
    SET_METRO_BINDING(Profiler_Set_Custom_PropLogFile); //  <- UnityEngine.Profiler::set_logFile
    SET_METRO_BINDING(Profiler_Get_Custom_PropEnableBinaryLog); //  <- UnityEngine.Profiler::get_enableBinaryLog
    SET_METRO_BINDING(Profiler_Set_Custom_PropEnableBinaryLog); //  <- UnityEngine.Profiler::set_enableBinaryLog
    SET_METRO_BINDING(Profiler_Get_Custom_PropEnabled); //  <- UnityEngine.Profiler::get_enabled
    SET_METRO_BINDING(Profiler_Set_Custom_PropEnabled); //  <- UnityEngine.Profiler::set_enabled
    SET_METRO_BINDING(Profiler_CUSTOM_AddFramesFromFile); //  <- UnityEngine.Profiler::AddFramesFromFile
    SET_METRO_BINDING(Profiler_CUSTOM_BeginSample); //  <- UnityEngine.Profiler::BeginSample
    SET_METRO_BINDING(Profiler_CUSTOM_BeginSampleOnly); //  <- UnityEngine.Profiler::BeginSampleOnly
    SET_METRO_BINDING(Profiler_CUSTOM_EndSample); //  <- UnityEngine.Profiler::EndSample
    SET_METRO_BINDING(Profiler_Get_Custom_PropUsedHeapSize); //  <- UnityEngine.Profiler::get_usedHeapSize
    SET_METRO_BINDING(Profiler_CUSTOM_GetRuntimeMemorySize); //  <- UnityEngine.Profiler::GetRuntimeMemorySize
    SET_METRO_BINDING(Profiler_CUSTOM_GetMonoHeapSize); //  <- UnityEngine.Profiler::GetMonoHeapSize
    SET_METRO_BINDING(Profiler_CUSTOM_GetMonoUsedSize); //  <- UnityEngine.Profiler::GetMonoUsedSize
    SET_METRO_BINDING(Profiler_CUSTOM_GetTotalAllocatedMemory); //  <- UnityEngine.Profiler::GetTotalAllocatedMemory
    SET_METRO_BINDING(Profiler_CUSTOM_GetTotalUnusedReservedMemory); //  <- UnityEngine.Profiler::GetTotalUnusedReservedMemory
    SET_METRO_BINDING(Profiler_CUSTOM_GetTotalReservedMemory); //  <- UnityEngine.Profiler::GetTotalReservedMemory
#if ENABLE_MONO
    SET_METRO_BINDING(Reproduction_CUSTOM_CaptureScreenshot); //  <- UnityEngineInternal.Reproduction::CaptureScreenshot
#endif
}

#endif
