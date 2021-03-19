using System.Collections.Generic;
using System.Linq;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Linux.Unity
{
    public class UnityLinuxToolchainProvider
    {
        class LinuxToolchainSort : Comparer<ToolChain>
        {
            bool IsToolchainValid(ToolChain toolchain)
            {
                if (toolchain.Sdk is LinuxSdk linuxSdk)
                    return linuxSdk.IsValid;
                return true;
            }

            public override int Compare(ToolChain a, ToolChain b) => IsToolchainValid(a).CompareTo(IsToolchainValid(b));
        }

        public static UnityLinuxClangSdkLocator<x86Architecture> x86ClangLocator => new UnityLinuxClangSdkLocator<x86Architecture>();
        public static UnityLinuxClangSdkLocator<x64Architecture> x64ClangLocator => new UnityLinuxClangSdkLocator<x64Architecture>();
        public static UnityLinuxGccSdkLocator<x86Architecture> x86GccLocator => new UnityLinuxGccSdkLocator<x86Architecture>();
        public static UnityLinuxGccSdkLocator<x64Architecture> x64GccLocator => new UnityLinuxGccSdkLocator<x64Architecture>();

        static ToolChain CreateToolChain<TSdk>(SdkLocator<TSdk> locator, IntelArchitecture arch, System.Func<ToolChain> creationFunc, bool enableWSL) where TSdk : LinuxSdk
            => new UnityLinuxToolchain(locator.UserDefaultOrLatest, arch, creationFunc, enableWSLSupport : enableWSL);

        static ToolChain ClangLinux_x64() => CreateToolChain(x64ClangLocator, new x64Architecture(), ClangLinux_x64, false);
        static ToolChain ClangLinux_x86() => CreateToolChain(x86ClangLocator, new x86Architecture(), ClangLinux_x86, false);
        static ToolChain ClangWSL_x64() => CreateToolChain(x64ClangLocator, new x64Architecture(), ClangWSL_x64, true);
        static ToolChain ClangWSL_x86() => CreateToolChain(x86ClangLocator, new x86Architecture(), ClangWSL_x86, true);
        static ToolChain GccLinux_x64() => CreateToolChain(x64GccLocator, new x64Architecture(), GccLinux_x64, false);
        static ToolChain GccLinux_x86() => CreateToolChain(x86GccLocator, new x86Architecture(), GccLinux_x86, false);
        static ToolChain GccWSL_x64() => CreateToolChain(x64GccLocator, new x64Architecture(), GccWSL_x64, true);
        static ToolChain GccWSL_x86() => CreateToolChain(x86GccLocator, new x86Architecture(), GccWSL_x86, true);

        // Returns all toolchains
        public IEnumerable<ToolChain> Provide() => new[]
        {
            ClangLinux_x64(),
            ClangLinux_x86(),
            GccLinux_x64(),
            GccLinux_x86(),
            ClangWSL_x64(),
            ClangWSL_x86(),
            GccWSL_x64(),
            GccWSL_x86()
        }
            .OrderByDescending(toolchain => toolchain, new LinuxToolchainSort());

        // Returns all toolchains for a given architecture
        public IEnumerable<ToolChain> Provide(Architecture arch) => Provide()
        .Where(toolchain => toolchain.Architecture.GetType() == arch.GetType());

        // Returns the first *valid* toolchain for an architecture
        public ToolChain ProvideOne(Architecture arch) => Provide(arch)
        .FirstOrDefault();
    }
}
