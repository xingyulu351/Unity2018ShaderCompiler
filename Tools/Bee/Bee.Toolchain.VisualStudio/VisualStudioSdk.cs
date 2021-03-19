using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;

namespace Bee.Toolchain.VisualStudio
{
    public class VisualStudioSdk : Sdk
    {
        public override Version    Version => Installation?.Version ?? new Version(0, 0);
        public MsvcInstallation Installation { get; }
        public Architecture        Architecture { get; }
        private string             SdkSubsetForLibraries { get; }

        public override bool SupportedOnHostPlatform => HostPlatform.IsWindows;

        protected VisualStudioSdk(MsvcInstallation installation, Platform platform, Architecture architecture, string sdkSubsetForLibraries = null, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(backendRegistrables)
        {
            Installation = installation;
            Platform     = platform;
            Architecture = architecture;
            SdkSubsetForLibraries = sdkSubsetForLibraries;
        }

        private string CommandString(string exe)
        {
            if (Installation == null)
                return Path.ToString();

            var pathEnv = Installation.GetPathEnvVariable(Architecture);
            var exePath = Installation.GetVSToolPath(Architecture, exe);
            return $"(set \"PATH={pathEnv}\") && {exePath.InQuotes()}";
        }

        public override Platform Platform { get; }
        public override NPath Path => Installation?.Windows10Sdk?.Path ?? "__visualstudio_sdk_missing__";

        public override NPath CppCompilerPath => Installation.GetVSToolPath(Architecture, "cl");
        public override NPath StaticLinkerPath => Installation.GetVSToolPath(Architecture, "lib");
        public override NPath DynamicLinkerPath => Installation.GetVSToolPath(Architecture, "link");

        public override string CppCompilerCommand => CommandString("cl");
        public override string StaticLinkerCommand => CommandString("lib");
        public override string DynamicLinkerCommand => CommandString("link");


        public override IEnumerable<NPath> IncludePaths => Installation?.GetIncludeDirectories(Architecture) ?? Array.Empty<NPath>();
        public override IEnumerable<NPath> LibraryPaths => Installation?.GetLibDirectories(Architecture, SdkSubsetForLibraries) ?? Array.Empty<NPath>();

        public NPath ToolPath(string command) => Installation?.GetSDKToolPath(command);
    }
}
