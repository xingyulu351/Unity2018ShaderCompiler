using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.VisualStudio.MsvcVersions
{
    // MSVC14 aka Visual Studio 2015
    public class Msvc14Installation : MsvcInstallation
    {
        public static NPath PackagePath { get; } = $"{Paths.UnityRoot}/PlatformDependent/Win/External/NonRedistributable/vs2015/builds.7z";

        public override string HumanName { get; } = "VS2015";

        private readonly NPath _netfxsdkDir;
        private readonly List<NPath> _sdkBinDirectories;

        public Msvc14Installation(NPath visualStudioDir, Windows10Sdk windows10Sdk, NPath dotNetFrameworkSdkDirectory) :
            base(new Version(14, 0), visualStudioDir, windows10Sdk)
        {
            _sdkBinDirectories = Windows10Sdk?.GetBinDirectories() ?? new List<NPath>();
            _netfxsdkDir = dotNetFrameworkSdkDirectory;
        }

        protected override IEnumerable<NPath> GetSDKBinDirectories()
        {
            return _sdkBinDirectories;
        }

        public override NPath[] GetIncludeDirectories(Architecture architecture)
        {
            var includeDirectory = Windows10Sdk.IncludeDirectory;
            return new NPath[]
            {
                VisualStudioDirectory.Combine("include"),
                includeDirectory.Combine("shared"),
                includeDirectory.Combine("um"),
                includeDirectory.Combine("winrt"),
                includeDirectory.Combine("ucrt"),
            };
        }

        public override IEnumerable<NPath> GetLibDirectories(Architecture architecture, string sdkSubset = null)
        {
            var libDirectory = Windows10Sdk.LibDirectory;
            var vcLibPath = VisualStudioDirectory.Combine("lib");

            if (sdkSubset != null)
                vcLibPath = vcLibPath.Combine(sdkSubset);

            if (architecture is x86Architecture)
            {
                yield return vcLibPath;
                yield return libDirectory.Combine("um", "x86");
                yield return libDirectory.Combine("ucrt", "x86");

                if (_netfxsdkDir != null)
                    yield return _netfxsdkDir.Combine("lib", "um", "x86");
            }
            else if (architecture is x64Architecture)
            {
                yield return vcLibPath.Combine("amd64");
                yield return libDirectory.Combine("um", "x64");
                yield return libDirectory.Combine("ucrt", "x64");

                if (_netfxsdkDir != null)
                    yield return _netfxsdkDir.Combine("lib", "um", "x64");
            }
            else if (architecture is ARMv7Architecture)
            {
                yield return vcLibPath.Combine("arm");
                yield return libDirectory.Combine("um", "arm");
                yield return libDirectory.Combine("ucrt", "arm");

                if (_netfxsdkDir != null)
                    yield return _netfxsdkDir.Combine("lib", "um", "arm");
            }
            else
            {
                throw new NotSupportedException(string.Format("Architecture {0} is not supported by MsvcToolChain!", architecture));
            }
        }

        public override NPath WinRTPlatformMetadata =>
            new NPath($"{VisualStudioDirectory}/vcpackages/platform.winmd").ToString(SlashMode.Native);
    }
}
