using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin;
using NiceIO;

namespace Tools.BugReporterWin.lib
{
    class CrashHandlerLib : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            Jambase.SubDir(JamList("TOP", "Tools", "BugReporterWin", "lib"));
            Jamrules.UnitySetupDefaults("CrashHandlerLib");
            c_autodetect.ActiveProject("CrashHandlerLib");
            C.C_IncludeDirectories(JamList(), JamList("../../../", "../../../Configuration", "include"));
            C.C_Defines(JamList(), JamList("_SECURE_SCL=0", "_HAS_ITERATOR_DEBUGGING=0"), JamList(), JamList("win32", "win64"));
            Jamrules.C_SetOptimization(JamList(), Jamrules.OptimizationMode.Size, "release"); // want to keep in small
            Jamrules.C_SetOptimization(JamList(), Jamrules.OptimizationMode.None, "debug");
            C.C_Library(
                "CrashHandlerLib",
                JamList(
                    "PlatformDependent/Win/WinUnicode.cpp",
                    "PlatformDependent/Win/WinUnicode.h",
                    "PlatformDependent/Win/PathUnicodeConversion.cpp",
                    "PlatformDependent/Win/PathUnicodeConversion.h",
                    "PlatformDependent/Win/FileInformation.cpp",
                    "PlatformDependent/Win/FileInformation.h",
                    "Tools/BugReporterWin/shared/FileUtilities.cpp",
                    "Tools/BugReporterWin/shared/FileUtilities.h",
                    new NPath("Tools/BugReporterWin/lib").Files("*.cpp")));
        }
    }
}
