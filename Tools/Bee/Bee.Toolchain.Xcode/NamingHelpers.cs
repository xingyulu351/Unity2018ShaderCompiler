using System;
using System.Linq;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Xcode
{
    public static class NamingHelpers
    {
        public static string AppleNameForArch(Architecture architecture)
        {
            if (architecture is ARMv7Architecture)
                return $"armv7";
            if (architecture is Arm64Architecture)
                return $"arm64";
            if (architecture is x64Architecture)
                return $"x86_64";
            if (architecture is x86Architecture)
                return $"i386";

            throw new NotSupportedException("Unsupported architecture: " + architecture);
        }
    }
}
