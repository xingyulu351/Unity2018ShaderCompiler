using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.Platforms.PS4
{
    class NativeProgramConfigurationExtensions
    {
        public static bool IsPS4(NativeProgramConfiguration c)
        {
            return c.Platform is PS4Platform;
        }
    }
}
