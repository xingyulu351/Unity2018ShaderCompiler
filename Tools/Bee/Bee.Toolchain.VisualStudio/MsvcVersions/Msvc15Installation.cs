using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.VisualStudio.MsvcVersions
{
    // MSVC15 aka Visual Studio 2017
    public class Msvc15Installation : MsvcInstallation
    {
        public static NPath PackagePath { get; } = $"{Paths.UnityRoot}/PlatformDependent/Win/External/NonRedistributable/vs2017/builds.7z";

        public override string HumanName { get; } = "VS2017";

        private readonly List<NPath> _sdkBinDirectories;

        public Msvc15Installation(NPath visualStudioDir, Windows10Sdk windows10Sdk) :
            base(new Version(15, 0), visualStudioDir, windows10Sdk)
        {
            _sdkBinDirectories = Windows10Sdk?.GetBinDirectories() ?? new List<NPath>();
        }

        protected override IEnumerable<NPath> GetSDKBinDirectories()
        {
            return _sdkBinDirectories;
        }

        public override string GetPathEnvVariable(Architecture architecture)
        {
            if (Windows10Sdk == null)
                return "";

            var paths = new List<NPath>();

            if (architecture is x86Architecture)
            {
                paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x86")));
            }
            else if (architecture is ARMv7Architecture)
            {
                paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x86")));
                // mspdbsrv libraries are in x64 folder; need to setup path to that
                paths.Add(VisualStudioDirectory.Combine("bin/Hostx64/x64"));
            }
            else if (architecture.Bits == 64)
            {
                paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x64")));
                paths.AddRange(GetSDKBinDirectories().Select(d => d.Combine("x86")));
                paths.Add(VisualStudioDirectory.Combine("bin/Hostx64/x64"));
            }
            else
            {
                throw new NotSupportedException($"'{architecture.Name}' architecture is not supported.");
            }

            return paths.Select(p => p.ToString(SlashMode.Native)).SeparateWith(";");
        }

        public override NPath GetVSToolPath(Architecture architecture, string toolName)
        {
            var binFolder = VisualStudioDirectory.Combine("bin");

            if (architecture is x86Architecture)
                return binFolder.Combine("Hostx86", "x86", toolName);

            if (architecture is x64Architecture)
                return binFolder.Combine("Hostx64", "x64", toolName);

            if (architecture is ARMv7Architecture)
                return binFolder.Combine("Hostx64", "arm", toolName);

            if (architecture is Arm64Architecture)
                return binFolder.Combine("Hostx64", "arm64", toolName);

            throw new NotSupportedException("Can't find MSVC15 tool for " + architecture);
        }

        public override NPath[] GetIncludeDirectories(Architecture architecture)
        {
            var sdkIncludes = Windows10Sdk.IncludeDirectory;
            return new NPath[]
            {
                VisualStudioDirectory.Combine("include"),
                sdkIncludes.Combine("shared"),
                sdkIncludes.Combine("um"),
                sdkIncludes.Combine("winrt"),
                sdkIncludes.Combine("ucrt"),
            };
        }

        public override IEnumerable<NPath> GetLibDirectories(Architecture architecture, string sdkSubset = null)
        {
            var sdkLibs = Windows10Sdk.LibDirectory;
            var vcLibPath = VisualStudioDirectory.Combine("lib");

            if (architecture is x86Architecture)
            {
                vcLibPath = vcLibPath.Combine("x86");
                yield return sdkLibs.Combine("um", "x86");
                yield return sdkLibs.Combine("ucrt", "x86");
            }
            else if (architecture is x64Architecture)
            {
                vcLibPath = vcLibPath.Combine("x64");
                yield return sdkLibs.Combine("um", "x64");
                yield return sdkLibs.Combine("ucrt", "x64");
            }
            else if (architecture is ARMv7Architecture)
            {
                vcLibPath = vcLibPath.Combine("arm");
                yield return sdkLibs.Combine("um", "arm");
                yield return sdkLibs.Combine("ucrt", "arm");
            }
            else if (architecture is Arm64Architecture)
            {
                vcLibPath = vcLibPath.Combine("arm64");
                yield return sdkLibs.Combine("um", "arm64");
                yield return sdkLibs.Combine("ucrt", "arm64");
            }
            else
            {
                throw new NotSupportedException($"Architecture {architecture} is not supported by Msvc15Installation!");
            }

            if (sdkSubset != null)
                vcLibPath = vcLibPath.Combine(sdkSubset);

            yield return vcLibPath;
        }

        public override NPath WinRTPlatformMetadata =>
            new NPath($"{VisualStudioDirectory}/lib/x86/store/references/platform.winmd").ToString(SlashMode.Native);
    }
}
