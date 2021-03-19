using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.Platforms.XboxOne
{
    internal static class NativeProgramConfigurationExtensions
    {
        public static bool IsXboxOne(NativeProgramConfiguration c)
        {
            return c.Platform is XboxOnePlatform;
        }
    }
}
