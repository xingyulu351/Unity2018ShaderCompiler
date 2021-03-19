#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Input/TimeManager.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include <ctime>

using namespace Unity;
using namespace std;
SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropTime()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropTime)
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time)
    return GetTimeManager ().GetCurTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropTimeSinceLevelLoad()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropTimeSinceLevelLoad)
    SCRIPTINGAPI_STACK_CHECK(get_timeSinceLevelLoad)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_timeSinceLevelLoad)
    return GetTimeManager ().GetTimeSinceSceneLoad ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropDeltaTime()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropDeltaTime)
    SCRIPTINGAPI_STACK_CHECK(get_deltaTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_deltaTime)
    return GetTimeManager ().GetDeltaTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropFixedTime()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropFixedTime)
    SCRIPTINGAPI_STACK_CHECK(get_fixedTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fixedTime)
    return GetTimeManager ().GetFixedTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropFixedDeltaTime()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropFixedDeltaTime)
    SCRIPTINGAPI_STACK_CHECK(get_fixedDeltaTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_fixedDeltaTime)
    return GetTimeManager ().GetFixedDeltaTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Time_Set_Custom_PropFixedDeltaTime(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Set_Custom_PropFixedDeltaTime)
    SCRIPTINGAPI_STACK_CHECK(set_fixedDeltaTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_fixedDeltaTime)
     GetTimeManager ().SetFixedDeltaTime (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropMaximumDeltaTime()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropMaximumDeltaTime)
    SCRIPTINGAPI_STACK_CHECK(get_maximumDeltaTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maximumDeltaTime)
    return GetTimeManager ().GetMaximumDeltaTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Time_Set_Custom_PropMaximumDeltaTime(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Set_Custom_PropMaximumDeltaTime)
    SCRIPTINGAPI_STACK_CHECK(set_maximumDeltaTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maximumDeltaTime)
     GetTimeManager ().SetMaximumDeltaTime (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropSmoothDeltaTime()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropSmoothDeltaTime)
    SCRIPTINGAPI_STACK_CHECK(get_smoothDeltaTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_smoothDeltaTime)
    return GetTimeManager ().GetSmoothDeltaTime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropTimeScale()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropTimeScale)
    SCRIPTINGAPI_STACK_CHECK(get_timeScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_timeScale)
    return GetTimeManager ().GetTimeScale ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Time_Set_Custom_PropTimeScale(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Set_Custom_PropTimeScale)
    SCRIPTINGAPI_STACK_CHECK(set_timeScale)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_timeScale)
     GetTimeManager ().SetTimeScale (value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Time_Get_Custom_PropFrameCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropFrameCount)
    SCRIPTINGAPI_STACK_CHECK(get_frameCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_frameCount)
    return GetTimeManager ().GetFrameCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Time_Get_Custom_PropRenderedFrameCount()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropRenderedFrameCount)
    SCRIPTINGAPI_STACK_CHECK(get_renderedFrameCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_renderedFrameCount)
    return GetTimeManager ().GetRenderFrameCount ();
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Time_Get_Custom_PropRealtimeSinceStartup()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropRealtimeSinceStartup)
    SCRIPTINGAPI_STACK_CHECK(get_realtimeSinceStartup)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_realtimeSinceStartup)
    return GetTimeManager().GetRealtime ();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Time_Get_Custom_PropCaptureFramerate()
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Get_Custom_PropCaptureFramerate)
    SCRIPTINGAPI_STACK_CHECK(get_captureFramerate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_captureFramerate)
    return GetTimeManager().GetCaptureFramerate();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Time_Set_Custom_PropCaptureFramerate(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Time_Set_Custom_PropCaptureFramerate)
    SCRIPTINGAPI_STACK_CHECK(set_captureFramerate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_captureFramerate)
     GetTimeManager().SetCaptureFramerate (value); 
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Time_get_time()
{
    scripting_add_internal_call( "UnityEngine.Time::get_time" , (gpointer)& Time_Get_Custom_PropTime );
}

void Register_UnityEngine_Time_get_timeSinceLevelLoad()
{
    scripting_add_internal_call( "UnityEngine.Time::get_timeSinceLevelLoad" , (gpointer)& Time_Get_Custom_PropTimeSinceLevelLoad );
}

void Register_UnityEngine_Time_get_deltaTime()
{
    scripting_add_internal_call( "UnityEngine.Time::get_deltaTime" , (gpointer)& Time_Get_Custom_PropDeltaTime );
}

void Register_UnityEngine_Time_get_fixedTime()
{
    scripting_add_internal_call( "UnityEngine.Time::get_fixedTime" , (gpointer)& Time_Get_Custom_PropFixedTime );
}

void Register_UnityEngine_Time_get_fixedDeltaTime()
{
    scripting_add_internal_call( "UnityEngine.Time::get_fixedDeltaTime" , (gpointer)& Time_Get_Custom_PropFixedDeltaTime );
}

void Register_UnityEngine_Time_set_fixedDeltaTime()
{
    scripting_add_internal_call( "UnityEngine.Time::set_fixedDeltaTime" , (gpointer)& Time_Set_Custom_PropFixedDeltaTime );
}

void Register_UnityEngine_Time_get_maximumDeltaTime()
{
    scripting_add_internal_call( "UnityEngine.Time::get_maximumDeltaTime" , (gpointer)& Time_Get_Custom_PropMaximumDeltaTime );
}

void Register_UnityEngine_Time_set_maximumDeltaTime()
{
    scripting_add_internal_call( "UnityEngine.Time::set_maximumDeltaTime" , (gpointer)& Time_Set_Custom_PropMaximumDeltaTime );
}

void Register_UnityEngine_Time_get_smoothDeltaTime()
{
    scripting_add_internal_call( "UnityEngine.Time::get_smoothDeltaTime" , (gpointer)& Time_Get_Custom_PropSmoothDeltaTime );
}

void Register_UnityEngine_Time_get_timeScale()
{
    scripting_add_internal_call( "UnityEngine.Time::get_timeScale" , (gpointer)& Time_Get_Custom_PropTimeScale );
}

void Register_UnityEngine_Time_set_timeScale()
{
    scripting_add_internal_call( "UnityEngine.Time::set_timeScale" , (gpointer)& Time_Set_Custom_PropTimeScale );
}

void Register_UnityEngine_Time_get_frameCount()
{
    scripting_add_internal_call( "UnityEngine.Time::get_frameCount" , (gpointer)& Time_Get_Custom_PropFrameCount );
}

void Register_UnityEngine_Time_get_renderedFrameCount()
{
    scripting_add_internal_call( "UnityEngine.Time::get_renderedFrameCount" , (gpointer)& Time_Get_Custom_PropRenderedFrameCount );
}

void Register_UnityEngine_Time_get_realtimeSinceStartup()
{
    scripting_add_internal_call( "UnityEngine.Time::get_realtimeSinceStartup" , (gpointer)& Time_Get_Custom_PropRealtimeSinceStartup );
}

void Register_UnityEngine_Time_get_captureFramerate()
{
    scripting_add_internal_call( "UnityEngine.Time::get_captureFramerate" , (gpointer)& Time_Get_Custom_PropCaptureFramerate );
}

void Register_UnityEngine_Time_set_captureFramerate()
{
    scripting_add_internal_call( "UnityEngine.Time::set_captureFramerate" , (gpointer)& Time_Set_Custom_PropCaptureFramerate );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineTime_IcallNames [] =
{
    "UnityEngine.Time::get_time"            ,    // -> Time_Get_Custom_PropTime
    "UnityEngine.Time::get_timeSinceLevelLoad",    // -> Time_Get_Custom_PropTimeSinceLevelLoad
    "UnityEngine.Time::get_deltaTime"       ,    // -> Time_Get_Custom_PropDeltaTime
    "UnityEngine.Time::get_fixedTime"       ,    // -> Time_Get_Custom_PropFixedTime
    "UnityEngine.Time::get_fixedDeltaTime"  ,    // -> Time_Get_Custom_PropFixedDeltaTime
    "UnityEngine.Time::set_fixedDeltaTime"  ,    // -> Time_Set_Custom_PropFixedDeltaTime
    "UnityEngine.Time::get_maximumDeltaTime",    // -> Time_Get_Custom_PropMaximumDeltaTime
    "UnityEngine.Time::set_maximumDeltaTime",    // -> Time_Set_Custom_PropMaximumDeltaTime
    "UnityEngine.Time::get_smoothDeltaTime" ,    // -> Time_Get_Custom_PropSmoothDeltaTime
    "UnityEngine.Time::get_timeScale"       ,    // -> Time_Get_Custom_PropTimeScale
    "UnityEngine.Time::set_timeScale"       ,    // -> Time_Set_Custom_PropTimeScale
    "UnityEngine.Time::get_frameCount"      ,    // -> Time_Get_Custom_PropFrameCount
    "UnityEngine.Time::get_renderedFrameCount",    // -> Time_Get_Custom_PropRenderedFrameCount
    "UnityEngine.Time::get_realtimeSinceStartup",    // -> Time_Get_Custom_PropRealtimeSinceStartup
    "UnityEngine.Time::get_captureFramerate",    // -> Time_Get_Custom_PropCaptureFramerate
    "UnityEngine.Time::set_captureFramerate",    // -> Time_Set_Custom_PropCaptureFramerate
    NULL
};

static const void* s_UnityEngineTime_IcallFuncs [] =
{
    (const void*)&Time_Get_Custom_PropTime                ,  //  <- UnityEngine.Time::get_time
    (const void*)&Time_Get_Custom_PropTimeSinceLevelLoad  ,  //  <- UnityEngine.Time::get_timeSinceLevelLoad
    (const void*)&Time_Get_Custom_PropDeltaTime           ,  //  <- UnityEngine.Time::get_deltaTime
    (const void*)&Time_Get_Custom_PropFixedTime           ,  //  <- UnityEngine.Time::get_fixedTime
    (const void*)&Time_Get_Custom_PropFixedDeltaTime      ,  //  <- UnityEngine.Time::get_fixedDeltaTime
    (const void*)&Time_Set_Custom_PropFixedDeltaTime      ,  //  <- UnityEngine.Time::set_fixedDeltaTime
    (const void*)&Time_Get_Custom_PropMaximumDeltaTime    ,  //  <- UnityEngine.Time::get_maximumDeltaTime
    (const void*)&Time_Set_Custom_PropMaximumDeltaTime    ,  //  <- UnityEngine.Time::set_maximumDeltaTime
    (const void*)&Time_Get_Custom_PropSmoothDeltaTime     ,  //  <- UnityEngine.Time::get_smoothDeltaTime
    (const void*)&Time_Get_Custom_PropTimeScale           ,  //  <- UnityEngine.Time::get_timeScale
    (const void*)&Time_Set_Custom_PropTimeScale           ,  //  <- UnityEngine.Time::set_timeScale
    (const void*)&Time_Get_Custom_PropFrameCount          ,  //  <- UnityEngine.Time::get_frameCount
    (const void*)&Time_Get_Custom_PropRenderedFrameCount  ,  //  <- UnityEngine.Time::get_renderedFrameCount
    (const void*)&Time_Get_Custom_PropRealtimeSinceStartup,  //  <- UnityEngine.Time::get_realtimeSinceStartup
    (const void*)&Time_Get_Custom_PropCaptureFramerate    ,  //  <- UnityEngine.Time::get_captureFramerate
    (const void*)&Time_Set_Custom_PropCaptureFramerate    ,  //  <- UnityEngine.Time::set_captureFramerate
    NULL
};

void ExportUnityEngineTimeBindings();
void ExportUnityEngineTimeBindings()
{
    for (int i = 0; s_UnityEngineTime_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineTime_IcallNames [i], s_UnityEngineTime_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineTimeBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Time_Get_Custom_PropTime); //  <- UnityEngine.Time::get_time
    SET_METRO_BINDING(Time_Get_Custom_PropTimeSinceLevelLoad); //  <- UnityEngine.Time::get_timeSinceLevelLoad
    SET_METRO_BINDING(Time_Get_Custom_PropDeltaTime); //  <- UnityEngine.Time::get_deltaTime
    SET_METRO_BINDING(Time_Get_Custom_PropFixedTime); //  <- UnityEngine.Time::get_fixedTime
    SET_METRO_BINDING(Time_Get_Custom_PropFixedDeltaTime); //  <- UnityEngine.Time::get_fixedDeltaTime
    SET_METRO_BINDING(Time_Set_Custom_PropFixedDeltaTime); //  <- UnityEngine.Time::set_fixedDeltaTime
    SET_METRO_BINDING(Time_Get_Custom_PropMaximumDeltaTime); //  <- UnityEngine.Time::get_maximumDeltaTime
    SET_METRO_BINDING(Time_Set_Custom_PropMaximumDeltaTime); //  <- UnityEngine.Time::set_maximumDeltaTime
    SET_METRO_BINDING(Time_Get_Custom_PropSmoothDeltaTime); //  <- UnityEngine.Time::get_smoothDeltaTime
    SET_METRO_BINDING(Time_Get_Custom_PropTimeScale); //  <- UnityEngine.Time::get_timeScale
    SET_METRO_BINDING(Time_Set_Custom_PropTimeScale); //  <- UnityEngine.Time::set_timeScale
    SET_METRO_BINDING(Time_Get_Custom_PropFrameCount); //  <- UnityEngine.Time::get_frameCount
    SET_METRO_BINDING(Time_Get_Custom_PropRenderedFrameCount); //  <- UnityEngine.Time::get_renderedFrameCount
    SET_METRO_BINDING(Time_Get_Custom_PropRealtimeSinceStartup); //  <- UnityEngine.Time::get_realtimeSinceStartup
    SET_METRO_BINDING(Time_Get_Custom_PropCaptureFramerate); //  <- UnityEngine.Time::get_captureFramerate
    SET_METRO_BINDING(Time_Set_Custom_PropCaptureFramerate); //  <- UnityEngine.Time::set_captureFramerate
}

#endif
