using System;
using System.Collections.Generic;
using System.Linq;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
{
    public class AndroidNdk : Sdk
    {
        public static AndroidNdkLocator LocatorArmv7 => new AndroidNdkLocator(new ARMv7Architecture());
        public static AndroidNdkLocator LocatorArm64 => new AndroidNdkLocator(new Arm64Architecture());
        public static AndroidNdkLocator Locatorx86 => new AndroidNdkLocator(new x86Architecture());

        public static AndroidNdkLocator Locator(Architecture architecture)
        {
            if (architecture is ARMv7Architecture) return LocatorArmv7;
            if (architecture is Arm64Architecture) return LocatorArm64;
            if (architecture is x86Architecture) return Locatorx86;
            throw new Exception("Unsupported architecture");
        }

        NPath BinPath => $"{Path}/toolchains/llvm/prebuilt/{HostPrefix}/bin";
        NPath GNUToolchain => $"{Path}/toolchains/{ToolchainArchPrefix}-4.9/prebuilt/{HostPrefix}";
        public NPath GNUTools => $"{GNUToolchain}/{Triple}/bin";

        public override NPath CppCompilerPath => BinPath.Combine("clang++");
        public override NPath StaticLinkerPath => GNUTools.Combine("ar");
        public override NPath DynamicLinkerPath => CppCompilerPath;

        public override bool SupportedOnHostPlatform => HostPlatform.IsWindows || HostPlatform.IsOSX || HostPlatform.IsLinux;
        public override string DynamicLinkerCommand => CppCompilerCommand + $" --gcc-toolchain={GNUToolchain.InQuotes()}";
        public override NPath SysRoot => Path.Combine("sysroot");
        public NPath LinkerSysRoot => Path.Combine($"platforms/android-{ApiLevel}/{Arch}");
        public int ApiLevel => Architecture is Arm64Architecture ? 21 : 16;

        public override IEnumerable<NPath> IncludePaths => new[] { SysRoot.Combine($"usr/include/{Triple}") }.Concat(GnuStlIncludes);
        public override IEnumerable<NPath> LibraryPaths => GnuStlLibraryPaths;
        public override Platform Platform { get; } = new AndroidPlatform();
        public Architecture Architecture { get; }
        public override NPath Path { get; }
        public override Version Version { get; }

        public AndroidNdk(Architecture architecture, NPath path, Version version, bool isDownloadable)
        {
            Architecture = architecture;
            Path = path;
            Version = version;
            IsDownloadable = isDownloadable;
        }

        string HostPrefix { get; } =
            HostPlatform.IsWindows ? "windows-x86_64" :
            HostPlatform.IsLinux   ? "linux-x86_64" :
            HostPlatform.IsOSX     ? "darwin-x86_64" :
            "<unsupported host platform>";

        string Abi => AbiFromArch(Architecture);

        string Arch =>
            Architecture is ARMv7Architecture ? "arch-arm" :
            Architecture is Arm64Architecture ? "arch-arm64" :
            Architecture is x86Architecture   ? "arch-x86" :
            Architecture is x64Architecture   ? "arch-x86_64" :
            "<unknown architecture>";

        string ToolchainArchPrefix =>
            Architecture is ARMv7Architecture ? "arm-linux-androideabi" :
            Architecture is Arm64Architecture ? "aarch64-linux-android" :
            Architecture is x86Architecture   ? "x86" :
            Architecture is x64Architecture   ? "x86_64" :
            "<unknown architecture>";

        string Triple =>
            Architecture is ARMv7Architecture ? "arm-linux-androideabi" :
            Architecture is Arm64Architecture ? "aarch64-linux-android" :
            Architecture is x86Architecture   ? "i686-linux-android" :
            Architecture is x64Architecture   ? "x86_64-linux-android" :
            "<unknown architecture>";

        public void CheckExists()
        {
            if (!Path.DirectoryExists())
                Errors.Exit($"Android build; Android NDK missing: '{Path}'");
            if (!BinPath.DirectoryExists())
                Errors.Exit($"Android build; NDK LLVM bin utils not found at '{BinPath}'");
            if (!GNUTools.DirectoryExists())
                Errors.Exit($"Android build; NDK GNU tools not found at '{GNUTools}'");
            if (!GnuStlRootPath.DirectoryExists())
                Errors.Exit($"Android build; NDK C++ STL folder not found at '{GnuStlRootPath}'");
        }

        // GNU STL
        NPath GnuStlRootPath => Path.Combine("sources/cxx-stl/gnu-libstdc++/4.9");
        IEnumerable<NPath> GnuStlLibraryPaths => GnuStlRootPath.Combine("libs", Abi).AsEnumerable();
        IEnumerable<NPath> GnuStlIncludes => new NPath[]
        {
            $"{GnuStlRootPath}/include",
            $"{GnuStlRootPath}/include/backward",
            $"{GnuStlRootPath}/libs/{Abi}/include"
        };

        public static string AbiFromArch(Architecture architecture) =>
            architecture is ARMv7Architecture ? "armeabi-v7a" :
            architecture is Arm64Architecture ? "arm64-v8a" :
            architecture is x86Architecture   ? "x86" :
            architecture is x64Architecture   ? "x86_64" :
            "<unknown architecture>";
    }
}
