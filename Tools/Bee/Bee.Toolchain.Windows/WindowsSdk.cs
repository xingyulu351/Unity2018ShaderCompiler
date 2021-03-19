using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.Stevedore;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;
using Unity.BuildTools;

namespace Bee.Toolchain.Windows
{
    public class WindowsSdk : VisualStudioSdk
    {
        public static WindowsSdkLocator Locatorx64 => new WindowsSdkLocator(new x64Architecture());
        public static WindowsSdkLocator Locatorx86 => new WindowsSdkLocator(new x86Architecture());

        public static WindowsSdkLocator LocatorFor(Architecture architecture)
        {
            if (architecture is x64Architecture) return Locatorx64;
            if (architecture is x86Architecture) return Locatorx86;
            throw new Exception("Unsupported architecture");
        }

        public override IEnumerable<string> Defines => new string[] { "WIN32", "WINDOWS", "_WINDOWS" };

        internal WindowsSdk(MsvcInstallation installation, Architecture architecture, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(installation, new WindowsPlatform(), architecture, null, backendRegistrables)
        {
        }
    }

    public class DownloadableWindowsSdk : WindowsSdk
    {
        public static DownloadableWindowsSdk FromVs2015(Architecture architecture, StevedoreArtifact vs2015, StevedoreArtifact win10Sdk)
            => new DownloadableWindowsSdk(
            new Msvc14Installation(vs2015.Path, new Windows10Sdk(win10Sdk.Id.Version.VersionString, win10Sdk.Path), dotNetFrameworkSdkDirectory: null),
            architecture, vs2015, win10Sdk);

        public static DownloadableWindowsSdk FromVs2017(Architecture architecture, StevedoreArtifact vs2017, StevedoreArtifact win10Sdk)
            => new DownloadableWindowsSdk(
            new Msvc15Installation(vs2017.Path, new Windows10Sdk(win10Sdk.Id.Version.VersionString, win10Sdk.Path)),
            architecture, vs2017, win10Sdk);

        readonly StevedoreArtifact m_Win10Sdk;

        protected DownloadableWindowsSdk(MsvcInstallation installation, Architecture architecture, StevedoreArtifact visualStudio, StevedoreArtifact win10Sdk)
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

    internal class WindowsDummySdk : WindowsSdk
    {
        internal WindowsDummySdk(Architecture architecture) : base(null, architecture) {}
    }
}
