using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux.Unity
{
    public class UnityLinuxClangSdkLocator<T> : SdkLocator<LinuxClangSdk> where T : Architecture, new()
    {
        // clang is targeted at the _build machine_, not the build target. Do _not_ use Target* properties here.
        private static readonly string DefaultClangVersion = "clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04";

        static class SdkPaths
        {
            // This is the root location of the default build tools
            public static readonly NPath BuildToolsRoot = Paths.UnityRoot.Combine("PlatformDependent/Linux/External/NonRedistributable/");

            // This is the location of Unity's default Clang compiler
            public static readonly NPath DefaultClangPath = BuildToolsRoot.Combine($"clang/{DefaultClangVersion}/builds");

            private static NPath PathOrNull(string path) => path != null ? new NPath(path) : null;

            // User overrides for all these paths
            public static NPath UserDefinedClangPath => PathOrNull(Environment.GetEnvironmentVariable("UNITY_LINUX_CLANG_PATH"));
        }

        private static UnityLinuxGccSdkLocator<T> GccLocator = new UnityLinuxGccSdkLocator<T>();

        protected override Platform Platform { get; } = new LinuxPlatform();

        // Returns all valid locations where clang might be found
        protected static IEnumerable<NPath> ScanClangLocationsFor()
        {
            return new NPath[]
            {
                SdkPaths.UserDefinedClangPath,
                SdkPaths.DefaultClangPath
            }
                .Where(path => path != null);
        }

        protected IEnumerable<LinuxClangSdk> ScanForAllSdkCombos()
        {
            var gccSdk = GccLocator.Latest;
            if (gccSdk == null)
                yield break;

            var clangPaths = ScanClangLocationsFor();
            foreach (var clangPath in clangPaths)
                yield return Combine(gccSdk, clangPath);
        }

        // returns all valid SDK combinations
        protected override IEnumerable<LinuxClangSdk> ScanForSystemInstalledSdks()
        {
            return ScanForAllSdkCombos().Where(l => l != null && l.IsValid);
        }

        protected override LinuxClangSdk NewSdkInstance(NPath path, Version version, bool isDownloadable)
        {
            throw new NotImplementedException();
        }

        public override LinuxClangSdk FromPath(NPath path)
        {
            throw new NotImplementedException();
        }

        public override LinuxClangSdk Dummy => Combine(GccLocator.Dummy, new NPath("/_invalid_clang_path_"));
        public override LinuxClangSdk UserDefault => SdkPaths.UserDefinedClangPath != null ? Combine(GccLocator.UserDefaultOrLatest, SdkPaths.UserDefinedClangPath) : null;

        private static LinuxClangSdk Combine(LinuxGccSdk gccSdk, string clangPath) => Combine(
            gccSdk, clangPath != null ? new NPath(clangPath) : null);

        public static LinuxClangSdk Combine(LinuxGccSdk gccSdk, NPath clangPath)
        {
            if (clangPath == null || gccSdk == null)
                return null;

            return new LinuxClangSdk(gccSdk, clangPath);
        }
    }
}
