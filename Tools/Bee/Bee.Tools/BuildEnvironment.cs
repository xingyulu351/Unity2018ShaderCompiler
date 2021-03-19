using System;

namespace Unity.BuildTools
{
    public static class BuildEnvironment
    {
        private static bool? s_IsRunningOnBuildMachineCached;

        public static bool IsRunningOnBuildMachine()
        {
            if (!s_IsRunningOnBuildMachineCached.HasValue) s_IsRunningOnBuildMachineCached = Environment.GetEnvironmentVariable("UNITY_THISISABUILDMACHINE") == "1";

            return s_IsRunningOnBuildMachineCached.Value;
        }
    }
}
