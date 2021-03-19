using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Lumin
{
    public class LuminSdk : Sdk
    {
        private const string GccSupportLibraryPath = "tools/toolchains/lib/gcc/aarch64-linux-android/4.9.x";

        public static LuminSdkLocator Locator => new LuminSdkLocator();

        public override NPath Path { get; }
        public override Platform Platform => new LuminPlatform();
        public override Version Version { get; }

        public override NPath CppCompilerPath => new NPath($"{ToolchainBinaries}/clang");
        public override NPath StaticLinkerPath => PathForTool("ar");
        public override NPath DynamicLinkerPath => CppCompilerPath;
        public override bool SupportedOnHostPlatform => HostPlatform.IsWindows || HostPlatform.IsOSX;

        public override NPath SysRoot { get; }
        public override IEnumerable<NPath> IncludePaths => new[]
        {
            Path.Combine("include"),
            Path.Combine("lumin/stl/libc++/include"),
            Path.Combine("lumin/usr/include"),
        };

        public override IEnumerable<NPath> LibraryPaths => new[]
        {
            Path.Combine("lumin/stl/libc++/lib"),
            Path.Combine(GccSupportLibraryPath),
            Path.Combine("lumin/usr/lib"),
        };

        public Architecture Architecture => new Arm64Architecture();
        public string Target => "aarch64-linux-android";

        public NPath ToolchainRoot { get; }
        public NPath ToolchainBinaries { get; }

        internal StaticLibrary GccSupportLibrary => Path.Combine(GccSupportLibraryPath).Combine("libgcc.a");

        public bool Exists => Path.DirectoryExists() && ToolchainBinaries.DirectoryExists();

        public LuminSdk(NPath path, Version version, bool isDownloadable)
        {
            Path = path;
            Version = version;
            SysRoot = $"{Path}/lumin";
            ToolchainRoot = $"{Path}/tools/toolchains";
            ToolchainBinaries = $"{ToolchainRoot}/bin";
            IsDownloadable = isDownloadable;
        }

        public void CheckExists()
        {
            if (!Path.DirectoryExists())
                Errors.Exit($"Lumin build; Lumin SDK missing: '{Path}'");
            if (!ToolchainBinaries.DirectoryExists())
                Errors.Exit($"Lumin build; Toolchain binaries not found at '{ToolchainBinaries}'");
        }

        public NPath PathForTool(string name) => new NPath($"{ToolchainBinaries}/{Target}-{name}");
    }
}
