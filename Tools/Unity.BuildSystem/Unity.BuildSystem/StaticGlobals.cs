using System.Diagnostics;
using JamSharp.Runtime;
using Unity.BuildSystem;

internal class ConvertedJamFile : ConvertedJamFileBase
{
    public static StaticGlobals Vars = new StaticGlobals();
    public static readonly SubIncludeRunner SubIncludeRunner = new SubIncludeRunner();
}

[DebuggerStepThrough]
class StaticGlobals
{
    JamList UNITY_API_DEFINES_store = GlobalVariables.Singleton["UNITY_API_DEFINES"];

    public JamList UNITY_API_DEFINES
    {
        get
        {
            return UNITY_API_DEFINES_store;
        }
    }

    JamList ENABLE_WWW_store = GlobalVariables.Singleton["ENABLE_WWW"];

    public JamList ENABLE_WWW
    {
        get
        {
            return ENABLE_WWW_store;
        }
    }

    JamList CONFIG_store = GlobalVariables.Singleton["CONFIG"];

    public JamList CONFIG
    {
        get
        {
            return CONFIG_store;
        }
    }

    JamList TAB_store = GlobalVariables.Singleton["TAB"];

    public JamList TAB
    {
        get
        {
            return TAB_store;
        }
    }

    JamList GLOBAL_DEFINES_store = GlobalVariables.Singleton["GLOBAL_DEFINES"];

    public JamList GLOBAL_DEFINES
    {
        get
        {
            return GLOBAL_DEFINES_store;
        }
    }

    JamList UNITY_TARGET_DEFINES_PLATFORM_store = GlobalVariables.Singleton["UNITY_TARGET_DEFINES_PLATFORM"];

    public JamList UNITY_TARGET_DEFINES_PLATFORM
    {
        get
        {
            return UNITY_TARGET_DEFINES_PLATFORM_store;
        }
    }

    JamList NEWLINE_store = GlobalVariables.Singleton["NEWLINE"];

    public JamList NEWLINE
    {
        get
        {
            return NEWLINE_store;
        }
    }

    JamList TOP_store = GlobalVariables.Singleton["TOP"];

    public JamList TOP
    {
        get
        {
            return TOP_store;
        }
    }

    JamList PLATFORM_store = GlobalVariables.Singleton["PLATFORM"];

    public JamList PLATFORM
    {
        get
        {
            return PLATFORM_store;
        }
    }

    // Helper properties for checking PLATFORM variable values
    public bool TargetPlatformIsWindows32 => PLATFORM == "win32";
    public bool TargetPlatformIsWindows64 => PLATFORM == "win64";
    public bool TargetPlatformIsWindows => TargetPlatformIsWindows32 || TargetPlatformIsWindows64;
    public bool TargetPlatformIsMac32 => PLATFORM == "macosx32";
    public bool TargetPlatformIsMac64 => PLATFORM == "macosx64";
    public bool TargetPlatformIsMac => TargetPlatformIsMac32 || TargetPlatformIsMac64;
    public bool TargetPlatformIsLinux32 => PLATFORM == "linux32";
    public bool TargetPlatformIsLinux64 => PLATFORM == "linux64";
    public bool TargetPlatformIsLinux => TargetPlatformIsLinux32 || TargetPlatformIsLinux64;
    public bool TargetPlatformIsMetro => PLATFORM.IsIn("uap_arm", "uap_arm64", "uap_x64", "uap_x86");
    public bool TargetPlatformIsAndroid => PLATFORM == "androidndk";
    public bool TargetPlatformIsPS4 => PLATFORM == "orbis";
    public bool TargetPlatformIsXboxOne => PLATFORM == "durango";
    public bool TargetPlatformIsWebGL => PLATFORM == "webgl";

    public bool BuildingWindowsEditor => JAM_COMMAND_LINE_TARGETS.IsIn("Editor", "EditorApp");
    public bool BuildingMacEditor => JAM_COMMAND_LINE_TARGETS.IsIn("MacEditor", "MacEditorApp");
    public bool BuildingLinuxEditor => JAM_COMMAND_LINE_TARGETS.IsIn("LinuxEditor", "LinuxEditorApp");
    public bool BuildingAndroidPlayer => new JamList("AndroidSupport").IsIn(JAM_COMMAND_LINE_TARGETS);
    public bool BuildingAppleTV => new JamList("AppleTVSupportMac").IsIn(JAM_COMMAND_LINE_TARGETS);
    public bool BuildingIOSPlayer => new JamList("iOSPlayer").IsIn(JAM_COMMAND_LINE_TARGETS);
    public bool BuildingPS4Player => new JamList("PS4Player").IsIn(JAM_COMMAND_LINE_TARGETS);
    public bool BuildingXboxOnePlayer => new JamList("XboxOnePlayer").IsIn(JAM_COMMAND_LINE_TARGETS);

    public string PlatformForFmodModules
    {
        get
        {
            var p = PLATFORM.ToString();
            if (p == "androidndk")
                p = "android";
            if (p == "iphonesimulator")
                p = "iphone";
            return p;
        }
    }

    JamList IS_PLATFORM_AUTODETECTED_store = GlobalVariables.Singleton["IS_PLATFORM_AUTODETECTED"];

    public JamList IS_PLATFORM_AUTODETECTED
    {
        get
        {
            return IS_PLATFORM_AUTODETECTED_store;
        }
    }

    JamList GLOBAL_CSHARP_DEFINES_store = GlobalVariables.Singleton["GLOBAL_CSHARP_DEFINES"];

    public JamList GLOBAL_CSHARP_DEFINES
    {
        get
        {
            return GLOBAL_CSHARP_DEFINES_store;
        }
    }

    JamList SUBDIR_store = GlobalVariables.Singleton["SUBDIR"];

    public JamList SUBDIR
    {
        get
        {
            return SUBDIR_store;
        }
    }

    JamList SRCS_store = GlobalVariables.Singleton["SRCS"];

    public JamList SRCS
    {
        get
        {
            return SRCS_store;
        }
    }

    JamList LOCATE_TARGET_store = GlobalVariables.Singleton["LOCATE_TARGET"];

    public JamList LOCATE_TARGET
    {
        get
        {
            return LOCATE_TARGET_store;
        }
    }

    JamList OS_store = GlobalVariables.Singleton["OS"];

    public JamList OS
    {
        get
        {
            return OS_store;
        }
    }

    JamList UNITY_BUILDING_FOR_INSTALLER_store = GlobalVariables.Singleton["UNITY_BUILDING_FOR_INSTALLER"];

    public JamList UNITY_BUILDING_FOR_INSTALLER
    {
        get
        {
            return UNITY_BUILDING_FOR_INSTALLER_store;
        }
    }

    JamList implicitArgument1_store = GlobalVariables.Singleton["implicitArgument1"];

    public JamList implicitArgument1
    {
        get
        {
            return implicitArgument1_store;
        }
    }

    JamList SPACE_store = GlobalVariables.Singleton["SPACE"];

    public JamList SPACE
    {
        get
        {
            return SPACE_store;
        }
    }

    JamList NEWLINE_SET_store = GlobalVariables.Singleton["NEWLINE_SET"];

    public JamList NEWLINE_SET
    {
        get
        {
            return NEWLINE_SET_store;
        }
    }

    JamList NT_store = GlobalVariables.Singleton["NT"];

    public JamList NT
    {
        get
        {
            return NT_store;
        }
    }

    JamList MV_store = GlobalVariables.Singleton["MV"];

    public JamList MV
    {
        get
        {
            return MV_store;
        }
    }

    JamList RM_store = GlobalVariables.Singleton["RM"];

    public JamList RM
    {
        get
        {
            return RM_store;
        }
    }

    JamList RMDIR_store = GlobalVariables.Singleton["RMDIR"];

    public JamList RMDIR
    {
        get
        {
            return RMDIR_store;
        }
    }

    JamList SLASH_store = GlobalVariables.Singleton["SLASH"];

    public JamList SLASH
    {
        get
        {
            return SLASH_store;
        }
    }

    JamList UNIX_store = GlobalVariables.Singleton["UNIX"];

    public JamList UNIX
    {
        get
        {
            return UNIX_store;
        }
    }

    JamList JAMSHELL_store = GlobalVariables.Singleton["JAMSHELL"];

    public JamList JAMSHELL
    {
        get
        {
            return JAMSHELL_store;
        }
    }

    JamList DOT_store = GlobalVariables.Singleton["DOT"];

    public JamList DOT
    {
        get
        {
            return DOT_store;
        }
    }

    JamList LN_store = GlobalVariables.Singleton["LN"];

    public JamList LN
    {
        get
        {
            return LN_store;
        }
    }

    JamList SUBDIRRULES_store = GlobalVariables.Singleton["SUBDIRRULES"];

    public JamList SUBDIRRULES
    {
        get
        {
            return SUBDIRRULES_store;
        }
    }

    JamList SUBDIRRESET_store = GlobalVariables.Singleton["SUBDIRRESET"];

    public JamList SUBDIRRESET
    {
        get
        {
            return SUBDIRRESET_store;
        }
    }

    JamList CWD_store = GlobalVariables.Singleton["CWD"];

    public JamList CWD
    {
        get
        {
            return CWD_store;
        }
    }

    JamList implicitArgument2_store = GlobalVariables.Singleton["implicitArgument2"];

    public JamList implicitArgument2
    {
        get
        {
            return implicitArgument2_store;
        }
    }

    JamList SUBDIR_PATH_store = GlobalVariables.Singleton["SUBDIR_PATH"];

    public JamList SUBDIR_PATH
    {
        get
        {
            return SUBDIR_PATH_store;
        }
    }

    JamList TARGET_DIRECTORY_store = GlobalVariables.Singleton["TARGET_DIRECTORY"];

    public JamList TARGET_DIRECTORY
    {
        get
        {
            return TARGET_DIRECTORY_store;
        }
    }

    JamList ALL_SUBDIR_TOKENS_store = GlobalVariables.Singleton["ALL_SUBDIR_TOKENS"];

    public JamList ALL_SUBDIR_TOKENS
    {
        get
        {
            return ALL_SUBDIR_TOKENS_store;
        }
    }

    JamList _top_store = GlobalVariables.Singleton["_top"];

    public JamList _top
    {
        get
        {
            return _top_store;
        }
    }

    JamList _tokens_store = GlobalVariables.Singleton["_tokens"];

    public JamList _tokens
    {
        get
        {
            return _tokens_store;
        }
    }

    JamList SUBDIR_DOWN_store = GlobalVariables.Singleton["SUBDIR_DOWN"];

    public JamList SUBDIR_DOWN
    {
        get
        {
            return SUBDIR_DOWN_store;
        }
    }

    JamList SUBDIR_UP_store = GlobalVariables.Singleton["SUBDIR_UP"];

    public JamList SUBDIR_UP
    {
        get
        {
            return SUBDIR_UP_store;
        }
    }

    JamList SUBDIR_ROOT_store = GlobalVariables.Singleton["SUBDIR_ROOT"];

    public JamList SUBDIR_ROOT
    {
        get
        {
            return SUBDIR_ROOT_store;
        }
    }

    JamList SUBDIR_TOKENS_store = GlobalVariables.Singleton["SUBDIR_TOKENS"];

    public JamList SUBDIR_TOKENS
    {
        get
        {
            return SUBDIR_TOKENS_store;
        }
    }

    JamList target_store = GlobalVariables.Singleton["target"];

    public JamList target
    {
        get
        {
            return target_store;
        }
    }

    JamList ALL_LOCATE_TARGET_store = GlobalVariables.Singleton["ALL_LOCATE_TARGET"];

    public JamList ALL_LOCATE_TARGET
    {
        get
        {
            return ALL_LOCATE_TARGET_store;
        }
    }

    JamList SEARCH_SOURCE_store = GlobalVariables.Singleton["SEARCH_SOURCE"];

    public JamList SEARCH_SOURCE
    {
        get
        {
            return SEARCH_SOURCE_store;
        }
    }

    JamList LOCATE_SOURCE_store = GlobalVariables.Singleton["LOCATE_SOURCE"];

    public JamList LOCATE_SOURCE
    {
        get
        {
            return LOCATE_SOURCE_store;
        }
    }

    JamList SOURCE_GRIST_store = GlobalVariables.Singleton["SOURCE_GRIST"];

    public JamList SOURCE_GRIST
    {
        get
        {
            return SOURCE_GRIST_store;
        }
    }

    JamList HDRGRIST_store = GlobalVariables.Singleton["HDRGRIST"];

    public JamList HDRGRIST
    {
        get
        {
            return HDRGRIST_store;
        }
    }

    JamList _r_store = GlobalVariables.Singleton["_r"];

    public JamList _r
    {
        get
        {
            return _r_store;
        }
    }

    JamList _l_store = GlobalVariables.Singleton["_l"];

    public JamList _l
    {
        get
        {
            return _l_store;
        }
    }

    JamList _s_store = GlobalVariables.Singleton["_s"];

    public JamList _s
    {
        get
        {
            return _s_store;
        }
    }

    JamList MSVCNT_store = GlobalVariables.Singleton["MSVCNT"];

    public JamList MSVCNT
    {
        get
        {
            return MSVCNT_store;
        }
    }

    JamList TARGET_store = GlobalVariables.Singleton["TARGET"];

    public JamList TARGET
    {
        get
        {
            return TARGET_store;
        }
    }

    JamList ACTIVE_PROJECT_store = GlobalVariables.Singleton["ACTIVE_PROJECT"];

    public JamList ACTIVE_PROJECT
    {
        get
        {
            return ACTIVE_PROJECT_store;
        }
    }

    JamList SOURCES_store = GlobalVariables.Singleton["SOURCES"];

    public JamList SOURCES
    {
        get
        {
            return SOURCES_store;
        }
    }

    JamList THE_CONFIG_store = GlobalVariables.Singleton["THE_CONFIG"];

    public JamList THE_CONFIG
    {
        get
        {
            return THE_CONFIG_store;
        }
    }

    JamList THE_PLATFORM_store = GlobalVariables.Singleton["THE_PLATFORM"];

    public JamList THE_PLATFORM
    {
        get
        {
            return THE_PLATFORM_store;
        }
    }

    JamList OPTIONS_store = GlobalVariables.Singleton["OPTIONS"];

    public JamList OPTIONS
    {
        get
        {
            return OPTIONS_store;
        }
    }

    JamList TYPE_store = GlobalVariables.Singleton["TYPE"];

    public JamList TYPE
    {
        get
        {
            return TYPE_store;
        }
    }

    JamList EXTS_store = GlobalVariables.Singleton["EXTS"];

    public JamList EXTS
    {
        get
        {
            return EXTS_store;
        }
    }

    JamList RULE_store = GlobalVariables.Singleton["RULE"];

    public JamList RULE
    {
        get
        {
            return RULE_store;
        }
    }

    JamList SUFOBJ_store = GlobalVariables.Singleton["SUFOBJ"];

    public JamList SUFOBJ
    {
        get
        {
            return SUFOBJ_store;
        }
    }

    JamList ADD_TO_LINK_store = GlobalVariables.Singleton["ADD_TO_LINK"];

    public JamList ADD_TO_LINK
    {
        get
        {
            return ADD_TO_LINK_store;
        }
    }

    JamList C_BUILD_EXTENSIONS_store = GlobalVariables.Singleton["C.BUILD_EXTENSIONS"];

    public JamList C_BUILD_EXTENSIONS
    {
        get
        {
            return C_BUILD_EXTENSIONS_store;
        }
    }

    JamList C_C_STYLE_BUILD_EXTENSIONS_store = GlobalVariables.Singleton["C.C_STYLE_BUILD_EXTENSIONS"];

    public JamList C_C_STYLE_BUILD_EXTENSIONS
    {
        get
        {
            return C_C_STYLE_BUILD_EXTENSIONS_store;
        }
    }

    JamList C_CPP_STYLE_BUILD_EXTENSIONS_store = GlobalVariables.Singleton["C.CPP_STYLE_BUILD_EXTENSIONS"];

    public JamList C_CPP_STYLE_BUILD_EXTENSIONS
    {
        get
        {
            return C_CPP_STYLE_BUILD_EXTENSIONS_store;
        }
    }

    JamList _obj_store = GlobalVariables.Singleton["_obj"];

    public JamList _obj
    {
        get
        {
            return _obj_store;
        }
    }

    JamList _postmflags_store = GlobalVariables.Singleton["_postmflags"];

    public JamList _postmflags
    {
        get
        {
            return _postmflags_store;
        }
    }

    JamList _VCPDB_store = GlobalVariables.Singleton["_VCPDB"];

    public JamList _VCPDB
    {
        get
        {
            return _VCPDB_store;
        }
    }

    JamList TARGET__store = GlobalVariables.Singleton["TARGET_"];

    public JamList TARGET_
    {
        get
        {
            return TARGET__store;
        }
    }

    JamList SOURCES__store = GlobalVariables.Singleton["SOURCES_"];

    public JamList SOURCES_
    {
        get
        {
            return SOURCES__store;
        }
    }

    JamList grist_store = GlobalVariables.Singleton["grist"];

    public JamList grist
    {
        get
        {
            return grist_store;
        }
    }

    JamList FLAGS_ALL_TYPES_store = GlobalVariables.Singleton["FLAGS_ALL_TYPES"];

    public JamList FLAGS_ALL_TYPES
    {
        get
        {
            return FLAGS_ALL_TYPES_store;
        }
    }

    JamList INCLUDES_ALL_TYPES_store = GlobalVariables.Singleton["INCLUDES_ALL_TYPES"];

    public JamList INCLUDES_ALL_TYPES
    {
        get
        {
            return INCLUDES_ALL_TYPES_store;
        }
    }

    JamList postHDRS_store = GlobalVariables.Singleton["postHDRS"];

    public JamList postHDRS
    {
        get
        {
            return postHDRS_store;
        }
    }

    JamList postREFERENCEDIRECTORIES_store = GlobalVariables.Singleton["postREFERENCEDIRECTORIES"];

    public JamList postREFERENCEDIRECTORIES
    {
        get
        {
            return postREFERENCEDIRECTORIES_store;
        }
    }

    JamList preDEFINES_store = GlobalVariables.Singleton["preDEFINES"];

    public JamList preDEFINES
    {
        get
        {
            return preDEFINES_store;
        }
    }

    JamList preCCFLAGS_store = GlobalVariables.Singleton["preCCFLAGS"];

    public JamList preCCFLAGS
    {
        get
        {
            return preCCFLAGS_store;
        }
    }

    JamList preCPlusPlusFLAGS_store = GlobalVariables.Singleton["preC++FLAGS"];

    public JamList preCPlusPlusFLAGS
    {
        get
        {
            return preCPlusPlusFLAGS_store;
        }
    }

    JamList preMFLAGS_store = GlobalVariables.Singleton["preMFLAGS"];

    public JamList preMFLAGS
    {
        get
        {
            return preMFLAGS_store;
        }
    }

    JamList preMMFLAGS_store = GlobalVariables.Singleton["preMMFLAGS"];

    public JamList preMMFLAGS
    {
        get
        {
            return preMMFLAGS_store;
        }
    }

    JamList sysHDRS_store = GlobalVariables.Singleton["sysHDRS"];

    public JamList sysHDRS
    {
        get
        {
            return sysHDRS_store;
        }
    }

    JamList flags_store = GlobalVariables.Singleton["flags"];

    public JamList flags
    {
        get
        {
            return flags_store;
        }
    }

    JamList forceIncludes_store = GlobalVariables.Singleton["forceIncludes"];

    public JamList forceIncludes
    {
        get
        {
            return forceIncludes_store;
        }
    }

    JamList FORCE_INCLUDES_store = GlobalVariables.Singleton["FORCE_INCLUDES"];

    public JamList FORCE_INCLUDES
    {
        get
        {
            return FORCE_INCLUDES_store;
        }
    }

    JamList _allobjs_store = GlobalVariables.Singleton["_allobjs"];

    public JamList _allobjs
    {
        get
        {
            return _allobjs_store;
        }
    }

    JamList sources_store = GlobalVariables.Singleton["sources"];

    public JamList sources
    {
        get
        {
            return sources_store;
        }
    }

    JamList newsrcs_store = GlobalVariables.Singleton["newsrcs"];

    public JamList newsrcs
    {
        get
        {
            return newsrcs_store;
        }
    }

    JamList _src_store = GlobalVariables.Singleton["_src"];

    public JamList _src
    {
        get
        {
            return _src_store;
        }
    }

    JamList _nopch_store = GlobalVariables.Singleton["_nopch"];

    public JamList _nopch
    {
        get
        {
            return _nopch_store;
        }
    }

    JamList _pch_store = GlobalVariables.Singleton["_pch"];

    public JamList _pch
    {
        get
        {
            return _pch_store;
        }
    }

    JamList _flags_store = GlobalVariables.Singleton["_flags"];

    public JamList _flags
    {
        get
        {
            return _flags_store;
        }
    }

    JamList EXCLUDED_FROM_BUILD_store = GlobalVariables.Singleton["EXCLUDED_FROM_BUILD"];

    public JamList EXCLUDED_FROM_BUILD
    {
        get
        {
            return EXCLUDED_FROM_BUILD_store;
        }
    }

    JamList FORCE_FILE_TYPE_store = GlobalVariables.Singleton["FORCE_FILE_TYPE"];

    public JamList FORCE_FILE_TYPE
    {
        get
        {
            return FORCE_FILE_TYPE_store;
        }
    }

    JamList NOPCH_store = GlobalVariables.Singleton["NOPCH"];

    public JamList NOPCH
    {
        get
        {
            return NOPCH_store;
        }
    }

    JamList PCH_store = GlobalVariables.Singleton["PCH"];

    public JamList PCH
    {
        get
        {
            return PCH_store;
        }
    }

    JamList ext_store = GlobalVariables.Singleton["ext"];

    public JamList ext
    {
        get
        {
            return ext_store;
        }
    }

    JamList BUILD_EXT_store = GlobalVariables.Singleton["BUILD_EXT"];

    public JamList BUILD_EXT
    {
        get
        {
            return BUILD_EXT_store;
        }
    }

    JamList sufobj_store = GlobalVariables.Singleton["sufobj"];

    public JamList sufobj
    {
        get
        {
            return sufobj_store;
        }
    }

    JamList _hdrs_store = GlobalVariables.Singleton["_hdrs"];

    public JamList _hdrs
    {
        get
        {
            return _hdrs_store;
        }
    }

    JamList objectForceIncludes_store = GlobalVariables.Singleton["objectForceIncludes"];

    public JamList objectForceIncludes
    {
        get
        {
            return objectForceIncludes_store;
        }
    }

    JamList C_STDHDRS_store = GlobalVariables.Singleton["C.STDHDRS"];

    public JamList C_STDHDRS
    {
        get
        {
            return C_STDHDRS_store;
        }
    }

    JamList _pch1_store = GlobalVariables.Singleton["_pch1"];

    public JamList _pch1
    {
        get
        {
            return _pch1_store;
        }
    }

    JamList compileRule_store = GlobalVariables.Singleton["compileRule"];

    public JamList compileRule
    {
        get
        {
            return compileRule_store;
        }
    }

    JamList MFLAGS_store = GlobalVariables.Singleton["MFLAGS"];

    public JamList MFLAGS
    {
        get
        {
            return MFLAGS_store;
        }
    }

    JamList objects_store = GlobalVariables.Singleton["objects"];

    public JamList objects
    {
        get
        {
            return objects_store;
        }
    }

    JamList contents_store = GlobalVariables.Singleton["contents"];

    public JamList contents
    {
        get
        {
            return contents_store;
        }
    }

    JamList _t_store = GlobalVariables.Singleton["_t"];

    public JamList _t
    {
        get
        {
            return _t_store;
        }
    }

    JamList outputPath_store = GlobalVariables.Singleton["outputPath"];

    public JamList outputPath
    {
        get
        {
            return outputPath_store;
        }
    }

    JamList OBJECTS_store = GlobalVariables.Singleton["OBJECTS"];

    public JamList OBJECTS
    {
        get
        {
            return OBJECTS_store;
        }
    }

    JamList _linkTargets_store = GlobalVariables.Singleton["_linkTargets"];

    public JamList _linkTargets
    {
        get
        {
            return _linkTargets_store;
        }
    }

    JamList linkDirectories_store = GlobalVariables.Singleton["linkDirectories"];

    public JamList linkDirectories
    {
        get
        {
            return linkDirectories_store;
        }
    }

    JamList linkFlags_store = GlobalVariables.Singleton["linkFlags"];

    public JamList linkFlags
    {
        get
        {
            return linkFlags_store;
        }
    }

    JamList C_STDLINKDIRECTORIES_store = GlobalVariables.Singleton["C.STDLINKDIRECTORIES"];

    public JamList C_STDLINKDIRECTORIES
    {
        get
        {
            return C_STDLINKDIRECTORIES_store;
        }
    }

    JamList linkRule_store = GlobalVariables.Singleton["linkRule"];

    public JamList linkRule
    {
        get
        {
            return linkRule_store;
        }
    }

    JamList linkLibs_store = GlobalVariables.Singleton["linkLibs"];

    public JamList linkLibs
    {
        get
        {
            return linkLibs_store;
        }
    }

    JamList needLibs_store = GlobalVariables.Singleton["needLibs"];

    public JamList needLibs
    {
        get
        {
            return needLibs_store;
        }
    }

    JamList C_LIBRARY_SHARED_MODULE_store = GlobalVariables.Singleton["C_LIBRARY_SHARED_MODULE"];

    public JamList C_LIBRARY_SHARED_MODULE
    {
        get
        {
            return C_LIBRARY_SHARED_MODULE_store;
        }
    }

    JamList SUFLIB_store = GlobalVariables.Singleton["SUFLIB"];

    public JamList SUFLIB
    {
        get
        {
            return SUFLIB_store;
        }
    }

    JamList _lBINDING_store = GlobalVariables.Singleton["_lBINDING"];

    public JamList _lBINDING
    {
        get
        {
            return _lBINDING_store;
        }
    }

    JamList KEEPOBJS_store = GlobalVariables.Singleton["KEEPOBJS"];

    public JamList KEEPOBJS
    {
        get
        {
            return KEEPOBJS_store;
        }
    }

    JamList objectsInArchive_store = GlobalVariables.Singleton["objectsInArchive"];

    public JamList objectsInArchive
    {
        get
        {
            return objectsInArchive_store;
        }
    }

    JamList NOARSCAN_store = GlobalVariables.Singleton["NOARSCAN"];

    public JamList NOARSCAN
    {
        get
        {
            return NOARSCAN_store;
        }
    }

    JamList CRELIB_store = GlobalVariables.Singleton["CRELIB"];

    public JamList CRELIB
    {
        get
        {
            return CRELIB_store;
        }
    }

    JamList libFlags_store = GlobalVariables.Singleton["libFlags"];

    public JamList libFlags
    {
        get
        {
            return libFlags_store;
        }
    }

    JamList C_RANLIB_store = GlobalVariables.Singleton["C.RANLIB"];

    public JamList C_RANLIB
    {
        get
        {
            return C_RANLIB_store;
        }
    }

    JamList NOARUPDATE_store = GlobalVariables.Singleton["NOARUPDATE"];

    public JamList NOARUPDATE
    {
        get
        {
            return NOARUPDATE_store;
        }
    }

    JamList suffixName_store = GlobalVariables.Singleton["suffixName"];

    public JamList suffixName
    {
        get
        {
            return suffixName_store;
        }
    }

    JamList _tlib_store = GlobalVariables.Singleton["_tlib"];

    public JamList _tlib
    {
        get
        {
            return _tlib_store;
        }
    }

    JamList EMPTY_store = GlobalVariables.Singleton["EMPTY"];

    public JamList EMPTY
    {
        get
        {
            return EMPTY_store;
        }
    }

    JamList LIBRARIES_store = GlobalVariables.Singleton["LIBRARIES"];

    public JamList LIBRARIES
    {
        get
        {
            return LIBRARIES_store;
        }
    }

    JamList gristedLibraries_store = GlobalVariables.Singleton["gristedLibraries"];

    public JamList gristedLibraries
    {
        get
        {
            return gristedLibraries_store;
        }
    }

    JamList SUFLINKLIBS_store = GlobalVariables.Singleton["SUFLINKLIBS"];

    public JamList SUFLINKLIBS
    {
        get
        {
            return SUFLINKLIBS_store;
        }
    }

    JamList PATH_store = GlobalVariables.Singleton["PATH"];

    public JamList PATH
    {
        get
        {
            return PATH_store;
        }
    }

    JamList COMPILER_store = GlobalVariables.Singleton["COMPILER"];

    public JamList COMPILER
    {
        get
        {
            return COMPILER_store;
        }
    }

    JamList C_CC_store = GlobalVariables.Singleton["C.CC"];

    public JamList C_CC
    {
        get
        {
            return C_CC_store;
        }
    }

    JamList C_CPlusPlus_store = GlobalVariables.Singleton["C.C++"];

    public JamList C_CPlusPlus
    {
        get
        {
            return C_CPlusPlus_store;
        }
    }

    JamList C_LINK_store = GlobalVariables.Singleton["C.LINK"];

    public JamList C_LINK
    {
        get
        {
            return C_LINK_store;
        }
    }

    JamList C_AR_store = GlobalVariables.Singleton["C.AR"];

    public JamList C_AR
    {
        get
        {
            return C_AR_store;
        }
    }

    JamList SUFMODULE_store = GlobalVariables.Singleton["SUFMODULE"];

    public JamList SUFMODULE
    {
        get
        {
            return SUFMODULE_store;
        }
    }

    JamList SUFSHAREDLIB_store = GlobalVariables.Singleton["SUFSHAREDLIB"];

    public JamList SUFSHAREDLIB
    {
        get
        {
            return SUFSHAREDLIB_store;
        }
    }

    JamList SUFEXE_store = GlobalVariables.Singleton["SUFEXE"];

    public JamList SUFEXE
    {
        get
        {
            return SUFEXE_store;
        }
    }

    JamList PCHCCFLAGS_store = GlobalVariables.Singleton["PCHCCFLAGS"];

    public JamList PCHCCFLAGS
    {
        get
        {
            return PCHCCFLAGS_store;
        }
    }

    JamList PCHCPlusPlusFLAGS_store = GlobalVariables.Singleton["PCHC++FLAGS"];

    public JamList PCHCPlusPlusFLAGS
    {
        get
        {
            return PCHCPlusPlusFLAGS_store;
        }
    }

    JamList PCHMFLAGS_store = GlobalVariables.Singleton["PCHMFLAGS"];

    public JamList PCHMFLAGS
    {
        get
        {
            return PCHMFLAGS_store;
        }
    }

    JamList PCHMMFLAGS_store = GlobalVariables.Singleton["PCHMMFLAGS"];

    public JamList PCHMMFLAGS
    {
        get
        {
            return PCHMMFLAGS_store;
        }
    }

    JamList BINDIR_store = GlobalVariables.Singleton["BINDIR"];

    public JamList BINDIR
    {
        get
        {
            return BINDIR_store;
        }
    }

    JamList LIBDIR_store = GlobalVariables.Singleton["LIBDIR"];

    public JamList LIBDIR
    {
        get
        {
            return LIBDIR_store;
        }
    }

    JamList STDHDRS_store = GlobalVariables.Singleton["STDHDRS"];

    public JamList STDHDRS
    {
        get
        {
            return STDHDRS_store;
        }
    }

    JamList UNDEFFLAG_store = GlobalVariables.Singleton["UNDEFFLAG"];

    public JamList UNDEFFLAG
    {
        get
        {
            return UNDEFFLAG_store;
        }
    }

    JamList _pchName_store = GlobalVariables.Singleton["_pchName"];

    public JamList _pchName
    {
        get
        {
            return _pchName_store;
        }
    }

    JamList _pchFilename_store = GlobalVariables.Singleton["_pchFilename"];

    public JamList _pchFilename
    {
        get
        {
            return _pchFilename_store;
        }
    }

    JamList pchext_store = GlobalVariables.Singleton["pchext"];

    public JamList pchext
    {
        get
        {
            return pchext_store;
        }
    }

    JamList pchBUILD_EXT_store = GlobalVariables.Singleton["pchBUILD_EXT"];

    public JamList pchBUILD_EXT
    {
        get
        {
            return pchBUILD_EXT_store;
        }
    }

    JamList GCC_ROOT_store = GlobalVariables.Singleton["GCC_ROOT"];

    public JamList GCC_ROOT
    {
        get
        {
            return GCC_ROOT_store;
        }
    }

    JamList SUFRES_store = GlobalVariables.Singleton["SUFRES"];

    public JamList SUFRES
    {
        get
        {
            return SUFRES_store;
        }
    }

    JamList SNBINDIR_store = GlobalVariables.Singleton["SNBINDIR"];

    public JamList SNBINDIR
    {
        get
        {
            return SNBINDIR_store;
        }
    }

    JamList MRI_RESPONSE_FILE_store = GlobalVariables.Singleton["MRI_RESPONSE_FILE"];

    public JamList MRI_RESPONSE_FILE
    {
        get
        {
            return MRI_RESPONSE_FILE_store;
        }
    }

    JamList MSVC_SUBSYSTEM_WINDOWS_store = GlobalVariables.Singleton["MSVC_SUBSYSTEM_WINDOWS"];

    public JamList MSVC_SUBSYSTEM_WINDOWS
    {
        get
        {
            return MSVC_SUBSYSTEM_WINDOWS_store;
        }
    }

    JamList MSVC_SUBSYSTEM_CONSOLE_store = GlobalVariables.Singleton["MSVC_SUBSYSTEM_CONSOLE"];

    public JamList MSVC_SUBSYSTEM_CONSOLE
    {
        get
        {
            return MSVC_SUBSYSTEM_CONSOLE_store;
        }
    }

    JamList VS140COMNTOOLS_store = GlobalVariables.Singleton["VS140COMNTOOLS"];

    public JamList VS140COMNTOOLS
    {
        get
        {
            return VS140COMNTOOLS_store;
        }
    }

    JamList MSVCNT_BIN_PATH_store = GlobalVariables.Singleton["MSVCNT_BIN_PATH"];

    public JamList MSVCNT_BIN_PATH
    {
        get
        {
            return MSVCNT_BIN_PATH_store;
        }
    }

    JamList IPHONEOS_SDK_VERSION_store = GlobalVariables.Singleton["IPHONEOS_SDK_VERSION"];

    public JamList IPHONEOS_SDK_VERSION
    {
        get
        {
            return IPHONEOS_SDK_VERSION_store;
        }
    }

    JamList IOS_SDK_VERSION_MIN_store = GlobalVariables.Singleton["IOS_SDK_VERSION_MIN"];

    public JamList IOS_SDK_VERSION_MIN
    {
        get
        {
            return IOS_SDK_VERSION_MIN_store;
        }
    }

    JamList INFO_PLIST_store = GlobalVariables.Singleton["INFO_PLIST"];

    public JamList INFO_PLIST
    {
        get
        {
            return INFO_PLIST_store;
        }
    }

    JamList LINK_TARGET_store = GlobalVariables.Singleton["LINK_TARGET"];

    public JamList LINK_TARGET
    {
        get
        {
            return LINK_TARGET_store;
        }
    }

    JamList BUNDLE_PATH_store = GlobalVariables.Singleton["BUNDLE_PATH"];

    public JamList BUNDLE_PATH
    {
        get
        {
            return BUNDLE_PATH_store;
        }
    }

    JamList C___DOLLAR_store = GlobalVariables.Singleton["C.__DOLLAR"];

    public JamList C___DOLLAR
    {
        get
        {
            return C___DOLLAR_store;
        }
    }

    JamList ISYSROOT_store = GlobalVariables.Singleton["ISYSROOT"];

    public JamList ISYSROOT
    {
        get
        {
            return ISYSROOT_store;
        }
    }

    JamList BUNDLE_TARGET_store = GlobalVariables.Singleton["BUNDLE_TARGET"];

    public JamList BUNDLE_TARGET
    {
        get
        {
            return BUNDLE_TARGET_store;
        }
    }

    JamList ENTITLEMENTS_store = GlobalVariables.Singleton["ENTITLEMENTS"];

    public JamList ENTITLEMENTS
    {
        get
        {
            return ENTITLEMENTS_store;
        }
    }

    JamList MACOSX_SDK_DEVELOPER_ROOT_store = GlobalVariables.Singleton["MACOSX_SDK_DEVELOPER_ROOT"];

    public JamList MACOSX_SDK_DEVELOPER_ROOT
    {
        get
        {
            return MACOSX_SDK_DEVELOPER_ROOT_store;
        }
    }

    JamList VALUE_store = GlobalVariables.Singleton["VALUE"];

    public JamList VALUE
    {
        get
        {
            return VALUE_store;
        }
    }

    JamList MSSDK_store = GlobalVariables.Singleton["MSSDK"];

    public JamList MSSDK
    {
        get
        {
            return MSSDK_store;
        }
    }

    JamList MSSDK_STDHDRS_store = GlobalVariables.Singleton["MSSDK_STDHDRS"];

    public JamList MSSDK_STDHDRS
    {
        get
        {
            return MSSDK_STDHDRS_store;
        }
    }

    JamList MSSDK_LINKDIRECTORIES_store = GlobalVariables.Singleton["MSSDK_LINKDIRECTORIES"];

    public JamList MSSDK_LINKDIRECTORIES
    {
        get
        {
            return MSSDK_LINKDIRECTORIES_store;
        }
    }

    JamList MSVCBIN_store = GlobalVariables.Singleton["MSVCBIN"];

    public JamList MSVCBIN
    {
        get
        {
            return MSVCBIN_store;
        }
    }

    JamList C_MT_store = GlobalVariables.Singleton["C.MT"];

    public JamList C_MT
    {
        get
        {
            return C_MT_store;
        }
    }

    JamList C_RC_store = GlobalVariables.Singleton["C.RC"];

    public JamList C_RC
    {
        get
        {
            return C_RC_store;
        }
    }

    JamList VSIBIN_store = GlobalVariables.Singleton["VSIBIN"];

    public JamList VSIBIN
    {
        get
        {
            return VSIBIN_store;
        }
    }

    JamList SCE_ROOT_DIR_store = GlobalVariables.Singleton["SCE_ROOT_DIR"];

    public JamList SCE_ROOT_DIR
    {
        get
        {
            return SCE_ROOT_DIR_store;
        }
    }

    JamList SDKVERSION_store = GlobalVariables.Singleton["SDKVERSION"];

    public JamList SDKVERSION
    {
        get
        {
            return SDKVERSION_store;
        }
    }

    JamList BULLSEYEBIN_store = GlobalVariables.Singleton["BULLSEYEBIN"];

    public JamList BULLSEYEBIN
    {
        get
        {
            return BULLSEYEBIN_store;
        }
    }

    JamList COVC_store = GlobalVariables.Singleton["COVC"];

    public JamList COVC
    {
        get
        {
            return COVC_store;
        }
    }

    JamList COVLINK_store = GlobalVariables.Singleton["COVLINK"];

    public JamList COVLINK
    {
        get
        {
            return COVLINK_store;
        }
    }

    JamList LOCATE_store = GlobalVariables.Singleton["LOCATE"];

    public JamList LOCATE
    {
        get
        {
            return LOCATE_store;
        }
    }

    JamList BINDING_store = GlobalVariables.Singleton["BINDING"];

    public JamList BINDING
    {
        get
        {
            return BINDING_store;
        }
    }

    JamList DEFS_store = GlobalVariables.Singleton["DEFS"];

    public JamList DEFS
    {
        get
        {
            return DEFS_store;
        }
    }

    JamList UNITY_ACTIVE_PROJECTS_store = GlobalVariables.Singleton["UNITY_ACTIVE_PROJECTS"];

    public JamList UNITY_ACTIVE_PROJECTS
    {
        get
        {
            return UNITY_ACTIVE_PROJECTS_store;
        }
    }

    JamList PARENT_store = GlobalVariables.Singleton["PARENT"];

    public JamList PARENT
    {
        get
        {
            return PARENT_store;
        }
    }

    JamList USE_EXPERIMENTAL_MODULARITY_store = GlobalVariables.Singleton["USE_EXPERIMENTAL_MODULARITY"];

    public JamList USE_EXPERIMENTAL_MODULARITY
    {
        get
        {
            return USE_EXPERIMENTAL_MODULARITY_store;
        }
    }

    JamList CombinedAssembliesAPIs_store = GlobalVariables.Singleton["CombinedAssembliesAPIs"];

    public JamList CombinedAssembliesAPIs
    {
        get
        {
            return CombinedAssembliesAPIs_store;
        }
    }

    JamList JAM_COMMAND_LINE_TARGETS_store = GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"];

    public JamList JAM_COMMAND_LINE_TARGETS
    {
        get
        {
            return JAM_COMMAND_LINE_TARGETS_store;
        }
    }

    JamList ENABLE_UNET_store = GlobalVariables.Singleton["ENABLE_UNET"];

    public JamList ENABLE_UNET
    {
        get
        {
            return ENABLE_UNET_store;
        }
    }

    JamList ENABLE_CLOUD_SERVICES_store = GlobalVariables.Singleton["ENABLE_CLOUD_SERVICES"];

    public JamList ENABLE_CLOUD_SERVICES
    {
        get
        {
            return ENABLE_CLOUD_SERVICES_store;
        }
    }

    JamList ENABLE_CLOUD_SERVICES_ADS_store = GlobalVariables.Singleton["ENABLE_CLOUD_SERVICES_ADS"];

    public JamList ENABLE_CLOUD_SERVICES_ADS
    {
        get
        {
            return ENABLE_CLOUD_SERVICES_ADS_store;
        }
    }

    JamList UNITY_TARGET_PLATFORMS_store = GlobalVariables.Singleton["UNITY_TARGET_PLATFORMS"];

    public JamList UNITY_TARGET_PLATFORMS
    {
        get
        {
            return UNITY_TARGET_PLATFORMS_store;
        }
    }

    JamList INCLUDES_store = GlobalVariables.Singleton["INCLUDES"];

    public JamList INCLUDES
    {
        get
        {
            return INCLUDES_store;
        }
    }

    JamList DEVELOPMENT_PLAYER_store = GlobalVariables.Singleton["DEVELOPMENT_PLAYER"];

    public JamList DEVELOPMENT_PLAYER
    {
        get
        {
            return DEVELOPMENT_PLAYER_store;
        }
    }

    JamList SCRIPTING_BACKEND_store = GlobalVariables.Singleton["SCRIPTING_BACKEND"];

    public JamList SCRIPTING_BACKEND
    {
        get
        {
            return SCRIPTING_BACKEND_store;
        }
    }

    JamList MODULE_BUILDMODE_store = GlobalVariables.Singleton["MODULE_BUILDMODE"];

    public JamList MODULE_BUILDMODE
    {
        get
        {
            return MODULE_BUILDMODE_store;
        }
    }

    JamList DEFINES_Star_Star_store = GlobalVariables.Singleton["DEFINES.*.*"];

    public JamList DEFINES_Star_Star
    {
        get
        {
            return DEFINES_Star_Star_store;
        }
    }

    JamList RUN_UNIT_TESTS_DURING_BUILD_store = GlobalVariables.Singleton["RUN_UNIT_TESTS_DURING_BUILD"];

    public JamList RUN_UNIT_TESTS_DURING_BUILD
    {
        get
        {
            return RUN_UNIT_TESTS_DURING_BUILD_store;
        }
    }

    JamList UNITY_RUN_NATIVE_TESTS_DURING_BUILD_store = GlobalVariables.Singleton["UNITY_RUN_NATIVE_TESTS_DURING_BUILD"];

    public JamList UNITY_RUN_NATIVE_TESTS_DURING_BUILD
    {
        get
        {
            return UNITY_RUN_NATIVE_TESTS_DURING_BUILD_store;
        }
    }

    JamList UNITY_MAC_BUILD_ENVIRONMENT_PATH_store = GlobalVariables.Singleton["UNITY_MAC_BUILD_ENVIRONMENT_PATH"];

    public JamList UNITY_MAC_BUILD_ENVIRONMENT_PATH
    {
        get
        {
            return UNITY_MAC_BUILD_ENVIRONMENT_PATH_store;
        }
    }

    JamList UNITY_MAC_BUILD_ENVIRONMENT_ZIP_store = GlobalVariables.Singleton["UNITY_MAC_BUILD_ENVIRONMENT_ZIP"];

    public JamList UNITY_MAC_BUILD_ENVIRONMENT_ZIP
    {
        get
        {
            return UNITY_MAC_BUILD_ENVIRONMENT_ZIP_store;
        }
    }

    JamList USE_UNITY_MAC_BUILD_ENVIRONMENT_ZIP_store = GlobalVariables.Singleton["USE_UNITY_MAC_BUILD_ENVIRONMENT_ZIP"];

    public JamList USE_UNITY_MAC_BUILD_ENVIRONMENT_ZIP
    {
        get
        {
            return USE_UNITY_MAC_BUILD_ENVIRONMENT_ZIP_store;
        }
    }

    JamList MACOSX_MIN_VERSION_store = GlobalVariables.Singleton["MACOSX_MIN_VERSION"];

    public JamList MACOSX_MIN_VERSION
    {
        get
        {
            return MACOSX_MIN_VERSION_store;
        }
    }

    JamList MACOSX_SDK_VERSION_store = GlobalVariables.Singleton["MACOSX_SDK_VERSION"];

    public JamList MACOSX_SDK_VERSION
    {
        get
        {
            return MACOSX_SDK_VERSION_store;
        }
    }

    JamList UNITY_THISISABUILDMACHINE_store = GlobalVariables.Singleton["UNITY_THISISABUILDMACHINE"];

    public JamList UNITY_THISISABUILDMACHINE
    {
        get
        {
            return UNITY_THISISABUILDMACHINE_store;
        }
    }

    JamList WINDIR_store = GlobalVariables.Singleton["WINDIR"];

    public JamList WINDIR
    {
        get
        {
            return WINDIR_store;
        }
    }

    JamList HDRS_Star_Star_store = GlobalVariables.Singleton["HDRS.*.*"];

    public JamList HDRS_Star_Star
    {
        get
        {
            return HDRS_Star_Star_store;
        }
    }

    JamList RUNTIME_EXPORT_SRCS_store = GlobalVariables.Singleton["RUNTIME_EXPORT_SRCS"];

    public JamList RUNTIME_EXPORT_SRCS
    {
        get
        {
            return RUNTIME_EXPORT_SRCS_store;
        }
    }

    JamList references_store = GlobalVariables.Singleton["references"];

    public JamList references
    {
        get
        {
            return references_store;
        }
    }

    JamList XBuildCmd_store = GlobalVariables.Singleton["XBuildCmd"];

    public JamList XBuildCmd
    {
        get
        {
            return XBuildCmd_store;
        }
    }

    JamList PDB_OUTPUT_PATH_store = GlobalVariables.Singleton["PDB_OUTPUT_PATH"];

    public JamList PDB_OUTPUT_PATH
    {
        get
        {
            return PDB_OUTPUT_PATH_store;
        }
    }

    JamList GLOBAL_PDB_NAME_CHECK_store = GlobalVariables.Singleton["GLOBAL_PDB_NAME_CHECK"];

    public JamList GLOBAL_PDB_NAME_CHECK
    {
        get
        {
            return GLOBAL_PDB_NAME_CHECK_store;
        }
    }

    JamList EDITOR_modules_store = GlobalVariables.Singleton["EDITOR_modules"];

    public JamList EDITOR_modules
    {
        get
        {
            return EDITOR_modules_store;
        }
    }

    JamList RUNTIME_modules_store = GlobalVariables.Singleton["RUNTIME_modules"];

    public JamList RUNTIME_modules
    {
        get
        {
            return RUNTIME_modules_store;
        }
    }

    JamList currentTarget_store = GlobalVariables.Singleton["currentTarget"];

    public JamList currentTarget
    {
        get
        {
            return currentTarget_store;
        }
    }

    public bool CurrentTargetIsEditor
    {
        get
        {
            var t = currentTarget;
            return t.IsIn("EditorApp", "MacEditorApp", "LinuxEditorApp");
        }
    }

    JamList Android_ABI_store = GlobalVariables.Singleton["Android.ABI"];

    public JamList Android_ABI
    {
        get
        {
            return Android_ABI_store;
        }
    }

    JamList Android_StaticLib_store = GlobalVariables.Singleton["Android.StaticLib"];

    public JamList Android_StaticLib
    {
        get
        {
            return Android_StaticLib_store;
        }
    }

    JamList Android_EnableBatching_store = GlobalVariables.Singleton["Android.EnableBatching"];

    public JamList Android_EnableBatching
    {
        get
        {
            return Android_EnableBatching_store;
        }
    }

    JamList OPENJDK_PATH_store = GlobalVariables.Singleton["OPENJDK_PATH"];

    public JamList OPENJDK_PATH
    {
        get
        {
            return OPENJDK_PATH_store;
        }
    }

    JamList ENABLE_MONO_SGEN_store = GlobalVariables.Singleton["ENABLE_MONO_SGEN"];

    public JamList ENABLE_MONO_SGEN
    {
        get
        {
            return ENABLE_MONO_SGEN_store;
        }
    }

    JamList ENABLE_MONO_BDWGC_store = GlobalVariables.Singleton["ENABLE_MONO_BDWGC"];

    public JamList ENABLE_MONO_BDWGC
    {
        get
        {
            return ENABLE_MONO_BDWGC_store;
        }
    }

    JamList PHYSX_CONFIG_store = GlobalVariables.Singleton["PHYSX_CONFIG"];

    public JamList PHYSX_CONFIG
    {
        get
        {
            return PHYSX_CONFIG_store;
        }
    }

    // Note: link order matters on gcc platforms (e.g. Linux)
    // The general rule is: if libA depends on libB, then it has to go first in the list of libraries.
    public JamList PhysX3Libraries { get; } = new JamList(
        "PhysX3CharacterKinematic",
        "PhysX3Extensions",
        "PhysX3",
        "PhysX3Cooking",
        "SimulationController",
        "LowLevelDynamics",
        "LowLevelAABB",
        "LowLevel",
        "PhysX3Common",
        "LowLevelCloth",
        "LowLevelParticles",
        "PhysX3Vehicle",
        "PsFastXml",
        "PxPvdSDK",
        "PxFoundation",
        "PxTask",
        "SceneQuery"
    );

    JamList INCLUDE_DYNAMIC_GI_store = GlobalVariables.Singleton["INCLUDE_DYNAMIC_GI"];

    public JamList INCLUDE_DYNAMIC_GI
    {
        get
        {
            return INCLUDE_DYNAMIC_GI_store;
        }
    }

    JamList RUNTIME_INCLUDES_store = GlobalVariables.Singleton["RUNTIME_INCLUDES"];

    public JamList RUNTIME_INCLUDES
    {
        get
        {
            return RUNTIME_INCLUDES_store;
        }
    }

    JamList project_store = GlobalVariables.Singleton["project"];

    public JamList project
    {
        get
        {
            return project_store;
        }
    }

    JamList HEADLESS_store = GlobalVariables.Singleton["HEADLESS"];

    public JamList HEADLESS
    {
        get
        {
            return HEADLESS_store;
        }
    }

    JamList INCLUDE_GI_store = GlobalVariables.Singleton["INCLUDE_GI"];

    public JamList INCLUDE_GI
    {
        get
        {
            return INCLUDE_GI_store;
        }
    }

    JamList ASSERTS_ENABLED_store = GlobalVariables.Singleton["ASSERTS_ENABLED"];

    public JamList ASSERTS_ENABLED
    {
        get
        {
            return ASSERTS_ENABLED_store;
        }
    }

    JamList bdwgc_include_files_store = GlobalVariables.Singleton["bdwgc_include_files"];

    public JamList bdwgc_include_files
    {
        get
        {
            return bdwgc_include_files_store;
        }
    }

    JamList bdwgc_library_store = GlobalVariables.Singleton["bdwgc_library"];

    public JamList bdwgc_library
    {
        get
        {
            return bdwgc_library_store;
        }
    }

    JamList libil2cpp_include_folder_store = GlobalVariables.Singleton["libil2cpp_include_folder"];

    public JamList libil2cpp_include_folder
    {
        get
        {
            return libil2cpp_include_folder_store;
        }
    }

    JamList libil2cpp_include_files_store = GlobalVariables.Singleton["libil2cpp_include_files"];

    public JamList libil2cpp_include_files
    {
        get
        {
            return libil2cpp_include_files_store;
        }
    }

    JamList NuGetPath_store = GlobalVariables.Singleton["NuGetPath"];

    public JamList NuGetPath
    {
        get
        {
            return NuGetPath_store;
        }
    }

    JamList NuGetPlPath_store = GlobalVariables.Singleton["NuGetPlPath"];

    public JamList NuGetPlPath
    {
        get
        {
            return NuGetPlPath_store;
        }
    }

    JamList EXTERNAL_SOURCES_NOPCH_store = GlobalVariables.Singleton["EXTERNAL_SOURCES_NOPCH"];

    public JamList EXTERNAL_SOURCES_NOPCH
    {
        get
        {
            return EXTERNAL_SOURCES_NOPCH_store;
        }
    }

    JamList sharedDefines_store = GlobalVariables.Singleton["sharedDefines"];

    public JamList sharedDefines
    {
        get
        {
            return sharedDefines_store;
        }
    }

    JamList SCEDIR_store = GlobalVariables.Singleton["SCEDIR"];

    public JamList SCEDIR
    {
        get
        {
            return SCEDIR_store;
        }
    }

    JamList SCE_ORBIS_SDK_DIR_store = GlobalVariables.Singleton["SCE_ORBIS_SDK_DIR"];

    public JamList SCE_ORBIS_SDK_DIR
    {
        get
        {
            return SCE_ORBIS_SDK_DIR_store;
        }
    }

    JamList ORBISBIN_store = GlobalVariables.Singleton["ORBISBIN"];

    public JamList ORBISBIN
    {
        get
        {
            return ORBISBIN_store;
        }
    }

    JamList C_orbis_bin_store = GlobalVariables.Singleton["C.orbis-bin"];

    public JamList C_orbis_bin
    {
        get
        {
            return C_orbis_bin_store;
        }
    }

    JamList ORBISSDK_STDHDRS_store = GlobalVariables.Singleton["ORBISSDK_STDHDRS"];

    public JamList ORBISSDK_STDHDRS
    {
        get
        {
            return ORBISSDK_STDHDRS_store;
        }
    }

    JamList ORBISSDK_LINKDIRECTORIES_store = GlobalVariables.Singleton["ORBISSDK_LINKDIRECTORIES"];

    public JamList ORBISSDK_LINKDIRECTORIES
    {
        get
        {
            return ORBISSDK_LINKDIRECTORIES_store;
        }
    }

    JamList TMAPI_store = GlobalVariables.Singleton["TMAPI"];

    public JamList TMAPI
    {
        get
        {
            return TMAPI_store;
        }
    }

    JamList NINTENDO_SDK_ROOT_store = GlobalVariables.Singleton["NINTENDO_SDK_ROOT"];

    public JamList NINTENDO_SDK_ROOT
    {
        get
        {
            return NINTENDO_SDK_ROOT_store;
        }
    }

    JamList RYNDA_SDK_store = GlobalVariables.Singleton["RYNDA_SDK"];

    public JamList RYNDA_SDK
    {
        get
        {
            return RYNDA_SDK_store;
        }
    }

    JamList SWITCH_CLANG_BIN_store = GlobalVariables.Singleton["SWITCH_CLANG_BIN"];

    public JamList SWITCH_CLANG_BIN
    {
        get
        {
            return SWITCH_CLANG_BIN_store;
        }
    }

    JamList ENABLE_UNIT_TESTS_store = GlobalVariables.Singleton["ENABLE_UNIT_TESTS"];

    public JamList ENABLE_UNIT_TESTS
    {
        get
        {
            return ENABLE_UNIT_TESTS_store;
        }
    }

    JamList ENABLE_VIDEO_store = GlobalVariables.Singleton["ENABLE_VIDEO"];

    public JamList ENABLE_VIDEO
    {
        get
        {
            return ENABLE_VIDEO_store;
        }
    }

    JamList ENABLE_RUNTIME_PERMISSIONS_store = GlobalVariables.Singleton["ENABLE_RUNTIME_PERMISSIONS"];

    public JamList ENABLE_RUNTIME_PERMISSIONS
    {
        get
        {
            return ENABLE_RUNTIME_PERMISSIONS_store;
        }
    }

    JamList RUNTIME_SOURCES_MS_COMMON_store = GlobalVariables.Singleton["RUNTIME_SOURCES_MS_COMMON"];

    public JamList RUNTIME_SOURCES_MS_COMMON
    {
        get
        {
            return RUNTIME_SOURCES_MS_COMMON_store;
        }
    }

    JamList XBOXONE_XDK_ROOT_store = GlobalVariables.Singleton["XBOXONE_XDK_ROOT"];

    public JamList XBOXONE_XDK_ROOT
    {
        get
        {
            return XBOXONE_XDK_ROOT_store;
        }
    }

    JamList DURANGOXDK_store = GlobalVariables.Singleton["DURANGOXDK"];

    public JamList DURANGOXDK
    {
        get
        {
            return DURANGOXDK_store;
        }
    }

    JamList XBOXONE_XDK_ROOT_LATEST_store = GlobalVariables.Singleton["XBOXONE_XDK_ROOT_LATEST"];

    public JamList XBOXONE_XDK_ROOT_LATEST
    {
        get
        {
            return XBOXONE_XDK_ROOT_LATEST_store;
        }
    }

    JamList TARGET_XDK_store = GlobalVariables.Singleton["TARGET_XDK"];

    public JamList TARGET_XDK
    {
        get
        {
            return TARGET_XDK_store;
        }
    }

    JamList MSSDK_LATEST_store = GlobalVariables.Singleton["MSSDK_LATEST"];

    public JamList MSSDK_LATEST
    {
        get
        {
            return MSSDK_LATEST_store;
        }
    }

    JamList XBOXONE_EXTENSION_SDK_store = GlobalVariables.Singleton["XBOXONE_EXTENSION_SDK"];

    public JamList XBOXONE_EXTENSION_SDK
    {
        get
        {
            return XBOXONE_EXTENSION_SDK_store;
        }
    }

    JamList XBOXONE_EXTENSION_SDK_EXT_store = GlobalVariables.Singleton["XBOXONE_EXTENSION_SDK_EXT"];

    public JamList XBOXONE_EXTENSION_SDK_EXT
    {
        get
        {
            return XBOXONE_EXTENSION_SDK_EXT_store;
        }
    }

    JamList EDITOR_BINDINGS_SRCS_store = GlobalVariables.Singleton["EDITOR_BINDINGS_SRCS"];

    public JamList EDITOR_BINDINGS_SRCS
    {
        get
        {
            return EDITOR_BINDINGS_SRCS_store;
        }
    }

    JamList EDITOR_EXPORT_SRCS_store = GlobalVariables.Singleton["EDITOR_EXPORT_SRCS"];

    public JamList EDITOR_EXPORT_SRCS
    {
        get
        {
            return EDITOR_EXPORT_SRCS_store;
        }
    }

    JamList CS_FILES_store = GlobalVariables.Singleton["CS_FILES"];

    public JamList CS_FILES
    {
        get
        {
            return CS_FILES_store;
        }
    }

    JamList CS_DEFINES_store = GlobalVariables.Singleton["CS_DEFINES"];

    public JamList CS_DEFINES
    {
        get
        {
            return CS_DEFINES_store;
        }
    }

    JamList EDITOR_SOURCES_store = GlobalVariables.Singleton["EDITOR_SOURCES"];

    public JamList EDITOR_SOURCES
    {
        get
        {
            return EDITOR_SOURCES_store;
        }
    }

    JamList INCLUDE_PUBNUB_store = GlobalVariables.Singleton["INCLUDE_PUBNUB"];

    public JamList INCLUDE_PUBNUB
    {
        get
        {
            return INCLUDE_PUBNUB_store;
        }
    }

    JamList ENABLE_CRUNCH_TEXTURE_COMPRESSION_store = GlobalVariables.Singleton["ENABLE_CRUNCH_TEXTURE_COMPRESSION"];

    public JamList ENABLE_CRUNCH_TEXTURE_COMPRESSION
    {
        get
        {
            return ENABLE_CRUNCH_TEXTURE_COMPRESSION_store;
        }
    }

    JamList EDITOR_BINDINGS_store = GlobalVariables.Singleton["EDITOR_BINDINGS"];

    public JamList EDITOR_BINDINGS
    {
        get
        {
            return EDITOR_BINDINGS_store;
        }
    }

    JamList ENABLE_EDITOR_HUB_store = GlobalVariables.Singleton["ENABLE_EDITOR_HUB"];

    public JamList ENABLE_EDITOR_HUB
    {
        get
        {
            return ENABLE_EDITOR_HUB_store;
        }
    }

    JamList ENABLE_EDITOR_HUB_LICENSE_store = GlobalVariables.Singleton["ENABLE_EDITOR_HUB_LICENSE"];

    public JamList ENABLE_EDITOR_HUB_LICENSE
    {
        get
        {
            return ENABLE_EDITOR_HUB_LICENSE_store;
        }
    }

    JamList ENABLE_WEBSOCKET_CLIENT_store = GlobalVariables.Singleton["ENABLE_WEBSOCKET_CLIENT"];

    public JamList ENABLE_WEBSOCKET_CLIENT
    {
        get
        {
            return ENABLE_WEBSOCKET_CLIENT_store;
        }
    }

    JamList ENABLE_CLOUD_SERVICES_COLLAB_store = GlobalVariables.Singleton["ENABLE_CLOUD_SERVICES_COLLAB"];

    public JamList ENABLE_CLOUD_SERVICES_COLLAB
    {
        get
        {
            return ENABLE_CLOUD_SERVICES_COLLAB_store;
        }
    }

    JamList RUNTIME_BINDINGS_store = GlobalVariables.Singleton["RUNTIME_BINDINGS"];

    public JamList RUNTIME_BINDINGS
    {
        get
        {
            return RUNTIME_BINDINGS_store;
        }
    }

    JamList IVY_store = GlobalVariables.Singleton["IVY"];

    public JamList IVY
    {
        get
        {
            return IVY_store;
        }
    }

    JamList IVY_OPTIONS_store = GlobalVariables.Singleton["IVY_OPTIONS"];

    public JamList IVY_OPTIONS
    {
        get
        {
            return IVY_OPTIONS_store;
        }
    }

    JamList ARGS_store = GlobalVariables.Singleton["ARGS"];

    public JamList ARGS
    {
        get
        {
            return ARGS_store;
        }
    }

    JamList ENABLE_MARSHALLING_TESTS_store = GlobalVariables.Singleton["ENABLE_MARSHALLING_TESTS"];

    public JamList ENABLE_MARSHALLING_TESTS
    {
        get
        {
            return ENABLE_MARSHALLING_TESTS_store;
        }
    }

    JamList RUNTIME_SOURCES_BASE_store = GlobalVariables.Singleton["RUNTIME_SOURCES_BASE"];

    public JamList RUNTIME_SOURCES_BASE
    {
        get
        {
            return RUNTIME_SOURCES_BASE_store;
        }
    }

    JamList runtimesources_store = GlobalVariables.Singleton["runtimesources"];

    public JamList runtimesources
    {
        get
        {
            return runtimesources_store;
        }
    }

    JamList ENABLE_2D_COMMON_store = GlobalVariables.Singleton["ENABLE_2D_COMMON"];

    public JamList ENABLE_2D_COMMON
    {
        get
        {
            return ENABLE_2D_COMMON_store;
        }
    }

    JamList generatedCpp_store = GlobalVariables.Singleton["generatedCpp"];

    public JamList generatedCpp
    {
        get
        {
            return generatedCpp_store;
        }
    }

    JamList generatedCs_store = GlobalVariables.Singleton["generatedCs"];

    public JamList generatedCs
    {
        get
        {
            return generatedCs_store;
        }
    }

    JamList TMP_store = GlobalVariables.Singleton["TMP"];

    public JamList TMP
    {
        get
        {
            return TMP_store;
        }
    }

    JamList ENABLE_CLOUD_SERVICES_CRASH_REPORTING_store = GlobalVariables.Singleton["ENABLE_CLOUD_SERVICES_CRASH_REPORTING"];

    public JamList ENABLE_CLOUD_SERVICES_CRASH_REPORTING
    {
        get
        {
            return ENABLE_CLOUD_SERVICES_CRASH_REPORTING_store;
        }
    }

    JamList ENABLE_VR_store = GlobalVariables.Singleton["ENABLE_VR"];

    public JamList ENABLE_VR
    {
        get
        {
            return ENABLE_VR_store;
        }
    }
    JamList ENABLE_SPATIALTRACKING_store = GlobalVariables.Singleton["ENABLE_SPATIALTRACKING"];
    public JamList ENABLE_SPATIALTRACKING
    {
        get
        {
            return ENABLE_SPATIALTRACKING_store;
        }
    }

    JamList ENABLE_DIRECTOR_store = GlobalVariables.Singleton["ENABLE_DIRECTOR"];

    public JamList ENABLE_DIRECTOR
    {
        get
        {
            return ENABLE_DIRECTOR_store;
        }
    }

    JamList ALL_modules_store = GlobalVariables.Singleton["ALL_modules"];

    public JamList ALL_modules
    {
        get
        {
            return ALL_modules_store;
        }
    }

    JamList ENABLE_TIMELINE_store = GlobalVariables.Singleton["ENABLE_TIMELINE"];

    public JamList ENABLE_TIMELINE
    {
        get
        {
            return ENABLE_TIMELINE_store;
        }
    }

    JamList ENABLE_TIMELINE_VIDEO_TRACK_store = GlobalVariables.Singleton["ENABLE_TIMELINE_VIDEO_TRACK"];

    public JamList ENABLE_TIMELINE_VIDEO_TRACK
    {
        get
        {
            return ENABLE_TIMELINE_VIDEO_TRACK_store;
        }
    }

    JamList ENABLE_HOLOLENS_MODULE_store = GlobalVariables.Singleton["ENABLE_HOLOLENS_MODULE"];

    public JamList ENABLE_HOLOLENS_MODULE
    {
        get
        {
            return ENABLE_HOLOLENS_MODULE_store;
        }
    }

    JamList ENABLE_HOLOLENS_MODULE_API_store = GlobalVariables.Singleton["ENABLE_HOLOLENS_MODULE_API"];

    public JamList ENABLE_HOLOLENS_MODULE_API
    {
        get
        {
            return ENABLE_HOLOLENS_MODULE_API_store;
        }
    }

    JamList wsa_patched_unityscript_store = GlobalVariables.Singleton["wsa_patched_unityscript"];

    public JamList wsa_patched_unityscript
    {
        get
        {
            return wsa_patched_unityscript_store;
        }
    }

    JamList uap_patched_unityscript_store = GlobalVariables.Singleton["uap_patched_unityscript"];

    public JamList uap_patched_unityscript
    {
        get
        {
            return uap_patched_unityscript_store;
        }
    }

    JamList assembly_patcher_targetdir_store = GlobalVariables.Singleton["assembly_patcher_targetdir"];

    public JamList assembly_patcher_targetdir
    {
        get
        {
            return assembly_patcher_targetdir_store;
        }
    }

    JamList assembly_patcher_deps_store = GlobalVariables.Singleton["assembly_patcher_deps"];

    public JamList assembly_patcher_deps
    {
        get
        {
            return assembly_patcher_deps_store;
        }
    }

    JamList assembly_patcher_config_store = GlobalVariables.Singleton["assembly_patcher_config"];

    public JamList assembly_patcher_config
    {
        get
        {
            return assembly_patcher_config_store;
        }
    }

    JamList targetdll_store = GlobalVariables.Singleton["targetdll"];

    public JamList targetdll
    {
        get
        {
            return targetdll_store;
        }
    }

    JamList assemblyReferences_store = GlobalVariables.Singleton["assemblyReferences"];

    public JamList assemblyReferences
    {
        get
        {
            return assemblyReferences_store;
        }
    }

    JamList winRTLegacyDll_store = GlobalVariables.Singleton["winRTLegacyDll"];

    public JamList winRTLegacyDll
    {
        get
        {
            return winRTLegacyDll_store;
        }
    }

    JamList mergeNativeArray_store = GlobalVariables.Singleton["mergeNativeArray"];

    public JamList mergeNativeArray
    {
        get
        {
            return mergeNativeArray_store;
        }
    }

    JamList IL2CPP_PACKAGE_VERSION_store = GlobalVariables.Singleton["IL2CPP_PACKAGE_VERSION"];

    public JamList IL2CPP_PACKAGE_VERSION
    {
        get
        {
            return IL2CPP_PACKAGE_VERSION_store;
        }
    }

    JamList ENABLE_AR_store = GlobalVariables.Singleton["ENABLE_AR"];

    public JamList ENABLE_AR
    {
        get
        {
            return ENABLE_AR_store;
        }
    }

    JamList ASAN_store = GlobalVariables.Singleton["ASAN"];

    public JamList ASAN
    {
        get
        {
            return ASAN_store;
        }
    }

    JamList UBSAN_store = GlobalVariables.Singleton["UBSAN"];

    public JamList UBSAN
    {
        get
        {
            return UBSAN_store;
        }
    }

    JamList TSAN_store = GlobalVariables.Singleton["TSAN"];

    public JamList TSAN
    {
        get
        {
            return TSAN_store;
        }
    }
}
