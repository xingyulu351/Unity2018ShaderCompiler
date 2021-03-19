#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingUtility.h"
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION CrashReport_CUSTOM_GetReports()
{
    SCRIPTINGAPI_ETW_ENTRY(CrashReport_CUSTOM_GetReports)
    SCRIPTINGAPI_STACK_CHECK(GetReports)
    
    #if PLATFORM_IPHONE
    		extern ScriptingArrayPtr GetCrashReports();
    		return GetCrashReports();
    #else
    		std::vector<std::string> reports;
    		return StringVectorToScripting(reports);
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION CrashReport_CUSTOM_GetReportData(ICallType_String_Argument id_, double* secondsSinceUnixEpoch, ICallType_String_Argument_Out text_)
{
    SCRIPTINGAPI_ETW_ENTRY(CrashReport_CUSTOM_GetReportData)
    ICallType_String_Local id(id_);
    UNUSED(id);
    ICallType_String_Local_Out text(text_);
    UNUSED(text);
    SCRIPTINGAPI_STACK_CHECK(GetReportData)
    
    #if PLATFORM_IPHONE
    		extern void GetCrashReportData(ICallString id, double* secondsSinceUnixEpoch, ICallString* text);
    		GetCrashReportData(id, secondsSinceUnixEpoch, text);
    #else
    		*secondsSinceUnixEpoch = 0.0;
    #if !PLATFORM_METRO && !UNITY_FLASH
    		text->str = CreateScriptingString("");
    #endif
    #endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION CrashReport_CUSTOM_RemoveReport(ICallType_String_Argument id_)
{
    SCRIPTINGAPI_ETW_ENTRY(CrashReport_CUSTOM_RemoveReport)
    ICallType_String_Local id(id_);
    UNUSED(id);
    SCRIPTINGAPI_STACK_CHECK(RemoveReport)
    
    #if PLATFORM_IPHONE
    		extern bool RemoveCrashReport(ICallString id);
    		return RemoveCrashReport(id);
    #else
    		return false;
    #endif
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_CrashReport_GetReports()
{
    scripting_add_internal_call( "UnityEngine.CrashReport::GetReports" , (gpointer)& CrashReport_CUSTOM_GetReports );
}

void Register_UnityEngine_CrashReport_GetReportData()
{
    scripting_add_internal_call( "UnityEngine.CrashReport::GetReportData" , (gpointer)& CrashReport_CUSTOM_GetReportData );
}

void Register_UnityEngine_CrashReport_RemoveReport()
{
    scripting_add_internal_call( "UnityEngine.CrashReport::RemoveReport" , (gpointer)& CrashReport_CUSTOM_RemoveReport );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_CrashReporter_IcallNames [] =
{
    "UnityEngine.CrashReport::GetReports"   ,    // -> CrashReport_CUSTOM_GetReports
    "UnityEngine.CrashReport::GetReportData",    // -> CrashReport_CUSTOM_GetReportData
    "UnityEngine.CrashReport::RemoveReport" ,    // -> CrashReport_CUSTOM_RemoveReport
    NULL
};

static const void* s_CrashReporter_IcallFuncs [] =
{
    (const void*)&CrashReport_CUSTOM_GetReports           ,  //  <- UnityEngine.CrashReport::GetReports
    (const void*)&CrashReport_CUSTOM_GetReportData        ,  //  <- UnityEngine.CrashReport::GetReportData
    (const void*)&CrashReport_CUSTOM_RemoveReport         ,  //  <- UnityEngine.CrashReport::RemoveReport
    NULL
};

void ExportCrashReporterBindings();
void ExportCrashReporterBindings()
{
    for (int i = 0; s_CrashReporter_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_CrashReporter_IcallNames [i], s_CrashReporter_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportCrashReporterBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(CrashReport_CUSTOM_GetReports); //  <- UnityEngine.CrashReport::GetReports
    SET_METRO_BINDING(CrashReport_CUSTOM_GetReportData); //  <- UnityEngine.CrashReport::GetReportData
    SET_METRO_BINDING(CrashReport_CUSTOM_RemoveReport); //  <- UnityEngine.CrashReport::RemoveReport
}

#endif
