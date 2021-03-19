using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Linq;
using System.Text.RegularExpressions;
using NiceIO;

namespace Unity.BuildSystem
{
    class PlatformSupportUtils
    {
        // Path to folder where standalone support things should be built, based on variation of the player
        internal static NPath GetStandaloneConfigBaseFolder(NPath baseFolder, string platform, string scriptingBackend, bool developmentPlayer)
        {
            var postfix = developmentPlayer ? "development" : "nondevelopment";
            if (ConvertedJamFile.Vars.TargetPlatformIsMac && ConvertedJamFile.Vars.USE_EXPERIMENTAL_MODULARITY == "1")
            {
                postfix = "modular";
            }

            if (ConvertedJamFile.Vars.TargetPlatformIsLinux)
            {
                var head = ConvertedJamFile.Vars.HEADLESS == "1" ? "headless" : "withgfx";
                postfix = $"{head}_{postfix}";
            }

            return $"{baseFolder}/Variations/{platform}_{postfix}_{scriptingBackend.ToLowerInvariant()}";
        }

        internal static NPath GetStandaloneConfigBaseFolder(NPath baseFolder)
        {
            var developmentPlayer = ConvertedJamFile.Vars.DEVELOPMENT_PLAYER == "1";
            var platform = ConvertedJamFile.Vars.PLATFORM.ToString();
            if (platform == "*") // happens during Jam workspace generation; default to win32 there
                platform = "win32";
            return GetStandaloneConfigBaseFolder(baseFolder, platform, ConvertedJamFile.Vars.SCRIPTING_BACKEND.ToString(), developmentPlayer);
        }

        internal static string WindowsSupportFolder => $"{ConvertedJamFile.Vars.TOP}/build/WindowsStandaloneSupport";
        internal static string MacSupportFolder => "build/MacStandaloneSupport";
        internal static string LinuxSupportFolder => "build/LinuxStandaloneSupport";

        [Pure]
        internal static IEnumerable<string> ExtractEnabledDefines(IEnumerable<string> defines)
        {
            foreach (var define in defines.Where(d => d.StartsWith("ENABLE_")))
            {
                var m = Regex.Match(define, @"^(?<name>ENABLE_\w+)(=(?<enabled>[01]))?$");
                if (!m.Success)
                    throw new Exception($"Unexpected ENABLE_ macro format `{define}`");

                if (m.Groups["enabled"].Value != "0")
                    yield return m.Groups["name"].Value;
            }
        }
    }
}
