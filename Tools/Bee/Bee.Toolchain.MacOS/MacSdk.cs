using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.NativeProgramSupport;
using Bee.Stevedore;
using Bee.Toolchain.Xcode;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.MacSDKSupport
{
    public class MacSdkLocator : XcodeSdkLocator<MacSdk>
    {
        protected override Platform Platform { get; } = new MacOSXPlatform();
        protected override string XcodePlatformName { get; } = "MacOSX";

        protected override MacSdk NewSdkInstance(NPath path, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            => new MacSdk(path, version, xcodePath, isDownloadable, backendRegistrables);

        protected override IEnumerable<MacSdk> ScanForDownloadableSdks()
        {
            var sdk = StevedoreArtifact.UnityInternal("mac-toolchain-10_12/8.0.0-8.1-8B62_e4686b94eaf1a3c1d19bb5fd451944754f62dbbd477b8303cbbae0cbfffb696f.7z");
            yield return new UnityMacSdk(sdk.Path, true, new[] { sdk });
        }
    }

    public sealed class UnityMacSdk : MacSdk
    {
        public override NPath BinPath { get;  }

        public UnityMacSdk(NPath buildsPath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(buildsPath.Combine("MacOSX10.12.sdk"), new Version(10, 12), "/Applications/Xcode.app", isDownloadable, backendRegistrables)
            => BinPath = buildsPath.Combine("usr/bin");
    }

    public class MacSdk : XcodeSdk
    {
        public static MacSdkLocator Locator { get; } = new MacSdkLocator();

        public override Platform Platform => new MacOSXPlatform();
        public MacSdk(NPath sdkPath, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(sdkPath, version, xcodePath, isDownloadable, backendRegistrables) {}
    }
}
