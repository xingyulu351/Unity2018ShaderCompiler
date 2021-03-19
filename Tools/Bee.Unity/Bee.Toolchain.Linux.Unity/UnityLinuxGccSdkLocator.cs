using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux.Unity
{
    public class UnityLinuxGccSdkLocator<T> : SdkLocator<LinuxGccSdk> where T : Architecture, new()
    {
        public static readonly T TargetArchitecture = new T();
        public static string TargetArchitectureName => LinuxSdk.DefaultArchitectureNameFor(TargetArchitecture);

        // Sysroot version is targeted specifically at a distro, but can contain multiple archictectures. Use the Target* properties here.
        private static readonly string DefaultTargetDistribution = "ubuntu14.04-linux-gnu";

        // The Unity Linux SDK is a functional chroot converted to a sysroot
        private static readonly string DefaultUnitySdkVersion = "LinuxBuildEnvironment-20180406";

        // Users can override the target architecture
        public static string UserDefinedTarget => Environment.GetEnvironmentVariable("UNITY_LINUX_TARGET") ?? DefaultTargetDistribution;

        static class SdkPaths
        {
            // This is the root location of the default build tools
            public static readonly NPath BuildToolsRoot = Paths.UnityRoot.Combine("PlatformDependent/Linux/External/NonRedistributable/");

            // This is the location of Unity's default GCC cross compiler
            public static readonly NPath DefaultGccPath = BuildToolsRoot.Combine($"cross-compilers/{DefaultTargetDistribution}/builds");

            // This is the location of Unity's default sysroot
            public static readonly NPath DefaultSysRootPath = BuildToolsRoot.Combine($"sysroots/{DefaultUnitySdkVersion}/builds");

            private static NPath PathOrNull(string path) => path != null ? new NPath(path) : null;

            // User overrides for all these paths
            public static NPath UserDefinedGccPath => PathOrNull(Environment.GetEnvironmentVariable("UNITY_LINUX_GCC_PATH"));
            public static NPath UserDefinedSysRootPath => PathOrNull(Environment.GetEnvironmentVariable("UNITY_LINUX_SYSROOT_PATH"));
        }

        protected override Platform Platform { get; } = new LinuxPlatform();

        // Returns all locations that might contain a Linux gcc toolchain
        protected static IEnumerable<NPath> ScanGccLocationsFor()
        {
            return new NPath[]
            {
                SdkPaths.UserDefinedGccPath,
                SdkPaths.DefaultGccPath
            }
                .Where(path => path != null);
        }

        // Returns all locations that might contain a Linux chroot that we'll use as a sysroot
        protected static IEnumerable<Tuple<NPath, string>> ScanSysrootLocationsFor(NPath gccRoot)
        {
            return new Tuple<NPath, string>[]
            {
                new Tuple<NPath, string>(SdkPaths.UserDefinedSysRootPath, UserDefinedTarget),
                new Tuple<NPath, string>(SdkPaths.UserDefinedSysRootPath, null),
                new Tuple<NPath, string>(SdkPaths.DefaultSysRootPath, DefaultTargetDistribution)
            }
                .Where(sysroot => sysroot.Item1 != null);
        }

        protected IEnumerable<LinuxGccSdk> ScanForAllSdkCombos()
        {
            var gccPaths = ScanGccLocationsFor();
            foreach (var gccPath in gccPaths)
            {
                var sysRoots = ScanSysrootLocationsFor(gccPath);
                foreach (var sysRoot in sysRoots)
                    yield return Combine(gccPath, sysRoot.Item1, sysRoot.Item2);
            }

            // Return the system GCC with no specific target or sysroot
            yield return new LinuxSystemGccSdk(TargetArchitecture);
        }

        // returns all valid SDK combinations
        protected override IEnumerable<LinuxGccSdk> ScanForSystemInstalledSdks()
        {
            return ScanForAllSdkCombos().Where(l => l != null && l.IsValid);
        }

        protected override LinuxGccSdk NewSdkInstance(NPath path, Version version, bool isDownloadable)
        {
            throw new NotImplementedException();
        }

        public override LinuxGccSdk FromPath(NPath path)
        {
            throw new NotImplementedException();
        }

        public override LinuxGccSdk Dummy => Combine(new NPath("/_invalid_binary_path_"), null, null);
        public override LinuxGccSdk UserDefault => SdkPaths.UserDefinedGccPath != null ? Combine(SdkPaths.UserDefinedGccPath, SdkPaths.UserDefinedSysRootPath, UserDefinedTarget) : null;

        private static LinuxGccSdk Combine(string binaryPath, string sysrootPath, string target) => Combine(
            binaryPath != null ? new NPath(binaryPath) : null,
            sysrootPath != null ? new NPath(sysrootPath) : null,
            target);

        private static LinuxGccSdk Combine(NPath binaryPath, NPath sysrootPath, string target)
        {
            return new LinuxGccSdk(TargetArchitecture, sysrootPath, binaryPath, target);
        }
    }
}
