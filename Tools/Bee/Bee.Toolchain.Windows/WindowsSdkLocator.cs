using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;

namespace Bee.Toolchain.Windows
{
    public class WindowsSdkLocator : SdkLocator<WindowsSdk>
    {
        public WindowsSdkLocator(Architecture architecture) => Architecture = architecture;

        protected override IEnumerable<WindowsSdk> ScanForSystemInstalledSdks()
        {
            foreach (var sdkVersion in new[] {14, 15})
            {
                foreach (var msvcInstallation in MsvcInstallation.InstallationsFor(new Version(sdkVersion, 0)))
                {
                    yield return new WindowsSdk(msvcInstallation, Architecture);
                }
            }
        }

        public override WindowsSdk Dummy => new WindowsDummySdk(Architecture);
        protected override Platform Platform { get; } = new WindowsPlatform();

        protected override WindowsSdk NewSdkInstance(NPath path, Version version, bool isDownloadable)
        {
            throw new NotImplementedException();
        }

        public override WindowsSdk FromPath(NPath path)
        {
            throw new NotImplementedException();
        }

        public override WindowsSdk UserDefault => new WindowsSdk(MsvcInstallation.GetLatestInstalled(), Architecture);

        private Architecture Architecture { get; }
    }
}
