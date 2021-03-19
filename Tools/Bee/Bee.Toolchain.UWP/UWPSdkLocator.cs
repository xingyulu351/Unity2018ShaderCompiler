using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.NativeProgramSupport;
using Bee.Stevedore;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;

namespace Bee.Toolchain.UWP
{
    public class UWPSdk : VisualStudioSdk
    {
        public static UWPSdkLocator Locatorx64 => new UWPSdkLocator(Architecture.x64);
        public static UWPSdkLocator Locatorx86 => new UWPSdkLocator(Architecture.x86);
        public static UWPSdkLocator LocatorARMv7 => new UWPSdkLocator(Architecture.Armv7);

        public static UWPSdkLocator LocatorFor(Architecture architecture)
        {
            if (architecture is x64Architecture)   return Locatorx64;
            if (architecture is x86Architecture)   return Locatorx86;
            if (architecture is ARMv7Architecture) return LocatorARMv7;
            throw new Exception("unsupported architecture");
        }

        public NPath WinRTPlatformMetadata => Installation.WinRTPlatformMetadata;

        internal UWPSdk(MsvcInstallation installation, Architecture architecture, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(installation, new UniversalWindowsPlatform(), architecture, "store", backendRegistrables)
        {
        }

        public override bool SupportedOnHostPlatform => HostPlatform.IsWindows;
    }

    public class DownloadableUWPSdk : UWPSdk
    {
        public static DownloadableUWPSdk FromVs2015(Architecture architecture, StevedoreArtifact vs2015, StevedoreArtifact win10Sdk)
            => new DownloadableUWPSdk(
            new Msvc14Installation(vs2015.Path, new Windows10Sdk(win10Sdk.Id.Version.VersionString, win10Sdk.Path), dotNetFrameworkSdkDirectory: null),
            architecture, vs2015, win10Sdk);

        public static DownloadableUWPSdk FromVs2017(Architecture architecture, StevedoreArtifact vs2017, StevedoreArtifact win10Sdk)
            => new DownloadableUWPSdk(
            new Msvc15Installation(vs2017.Path, new Windows10Sdk(win10Sdk.Id.Version.VersionString, win10Sdk.Path)),
            architecture, vs2017, win10Sdk);

        readonly StevedoreArtifact m_Win10Sdk;

        protected DownloadableUWPSdk(MsvcInstallation installation, Architecture architecture, StevedoreArtifact visualStudio, StevedoreArtifact win10Sdk)
            : base(installation, architecture, new[] { visualStudio, win10Sdk })
        {
            m_Win10Sdk = win10Sdk;
            IsDownloadable = true;
        }

        // HACK: We need to depend on a file in the win10Sdk to trigger Stevedore
        // downloads. Since there's no well-defined convenient file to depend
        // on (and we can't depend on directories), delegate that to the code
        // constructing the Sdk.
        public override IEnumerable<NPath> TundraInputs => base.TundraInputs.Append(m_Win10Sdk.ArtifactVersionFile);
    }

    public class UWPSdkLocator : SdkLocator<UWPSdk>
    {
        public UWPSdkLocator(Architecture architecture)
        {
            Architecture = architecture;
        }

        protected override IEnumerable<UWPSdk> ScanForSystemInstalledSdks()
        {
            foreach (var sdkVersion in new[] {14, 15})
            {
                foreach (var msvcInstallation in MsvcInstallation.InstallationsFor(new Version(sdkVersion, 0)))
                {
                    yield return new UWPSdk(msvcInstallation, Architecture);
                }
            }
        }

        public override UWPSdk Dummy => new UWPDummySdk(Architecture);
        protected override Platform Platform { get; } = new UniversalWindowsPlatform();

        protected override UWPSdk NewSdkInstance(NPath path, Version version, bool isDownloadable)
        {
            throw new NotImplementedException();
        }

        public override UWPSdk FromPath(NPath path)
        {
            throw new NotImplementedException();
        }

        public override UWPSdk UserDefault => new UWPSdk(MsvcInstallation.GetLatestInstalled(), Architecture);

        private Architecture Architecture { get; }
    }

    internal sealed class UWPDummySdk : UWPSdk
    {
        public UWPDummySdk(Architecture architecture) : base(null, architecture) {}
    }
}
