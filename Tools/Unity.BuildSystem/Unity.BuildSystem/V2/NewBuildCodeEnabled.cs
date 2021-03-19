using System.Collections.Generic;
using System.Linq;
using JamSharp.Runtime;
using Unity.BuildTools;

namespace Unity.BuildSystem.V2
{
    public static class NewBuildCodeEnabled
    {
        public static bool ForMacEditor => ForPlatform(nameof(ForMacEditor));
        public static bool ForPS4 => ForPlatform(nameof(ForPS4));
        public static bool ForUWP => ForPlatform(nameof(ForUWP));

        private static bool ForPlatform(string propertyName)
        {
            var platformName = propertyName.Substring(3);
            var jamVariableName = $"USENEWBUILDCODE_{platformName.ToUpperInvariant()}";
            if (_platformV2ActivationCache.TryGetValue(jamVariableName, out var enabledFromCache))
            {
                return enabledFromCache;
            }

            var jamVar = GlobalVariables.Singleton[jamVariableName];

            var enabledByDefault = Paths.UnityRoot.Combine(
                $"Tools/Unity.BuildSystem/Unity.BuildSystem/enable_v2_{platformName.ToLowerInvariant()}_by_default").FileExists();
            var enabled = jamVar.Any() ? jamVar[0] == "1" : enabledByDefault;

            GlobalVariables.Singleton[jamVariableName].Assign(enabled ? "1" : "0");

            _platformV2ActivationCache[jamVariableName] = enabled;
            return enabled;
        }

        private static readonly Dictionary<string, bool> _platformV2ActivationCache = new Dictionary<string, bool>();
    }
}
