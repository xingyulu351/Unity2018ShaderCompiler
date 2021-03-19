using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.Stevedore;
using Bee.Toolchain.Xcode;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.TvOS
{
    internal static class EmbeddedXcode
    {
        public static readonly NPath Path = Paths.UnityRoot.Combine("PlatformDependent/iPhonePlayer/External/NonRedistributable/BuildEnvironment/builds/Xcode.app");
    }

    public class AppleTvSdkLocator : XcodeSdkLocator<AppleTvSdk>
    {
        protected override Platform Platform { get; } = new IosPlatform();
        protected override string XcodePlatformName { get; } = "AppleTVOS";

        public override AppleTvSdk UserDefault => UnityEmbedded ?? base.UserDefault;
        public AppleTvSdk UnityEmbedded => DefaultSdkFromXcodeApp(EmbeddedXcode.Path);

        protected override AppleTvSdk NewSdkInstance(NPath path, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            => new AppleTvSdk(path, version, xcodePath, isDownloadable, backendRegistrables);

        protected override IEnumerable<AppleTvSdk> ScanForDownloadableSdks()
        {
            var sdk10_0 = StevedoreArtifact.UnityInternal("ios-tvos-toolchain/8.0.0-8.1-8B62-10.1-10.0_06b8196fda0ad9cdd39fba0ed45a6e30ce5fe1c85c7992a42cc068a67b45baca.7z");
            yield return NewDefaultSdkInstanceFromXcodeApp(sdk10_0.Path.Combine("Xcode.app"), new Version(10, 0), true, new[] { sdk10_0 });
        }
    }

    public class AppleTvSimulatorSdkLocator : XcodeSdkLocator<AppleTvSimulatorSdk>
    {
        protected override Platform Platform { get; } = new IosPlatform();
        protected override string XcodePlatformName { get; } = "AppleTVSimulator";

        public override AppleTvSimulatorSdk UserDefault => UnityEmbedded ?? base.UserDefault;
        public AppleTvSimulatorSdk UnityEmbedded => DefaultSdkFromXcodeApp(EmbeddedXcode.Path);

        protected override AppleTvSimulatorSdk NewSdkInstance(NPath path, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            => new AppleTvSimulatorSdk(path, version, xcodePath, isDownloadable, backendRegistrables);

        protected override IEnumerable<AppleTvSimulatorSdk> ScanForDownloadableSdks()
        {
            var sdk10_0 = StevedoreArtifact.UnityInternal("ios-tvos-toolchain/8.0.0-8.1-8B62-10.1-10.0_06b8196fda0ad9cdd39fba0ed45a6e30ce5fe1c85c7992a42cc068a67b45baca.7z");
            yield return NewDefaultSdkInstanceFromXcodeApp(sdk10_0.Path.Combine("Xcode.app"), new Version(10, 0), true, new[] { sdk10_0 });
        }
    }

    public class AppleTvSdk : XcodeSdk
    {
        public static AppleTvSdkLocator Locator { get; } = new AppleTvSdkLocator();

        public override Platform Platform => new TvosPlatform();
        public AppleTvSdk(NPath sdkPath, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(sdkPath, version, xcodePath, isDownloadable, backendRegistrables) {}
    }

    public class AppleTvSimulatorSdk : XcodeSdk
    {
        public static AppleTvSimulatorSdkLocator Locator { get; } = new AppleTvSimulatorSdkLocator();

        public override Platform Platform => new TvosPlatform();
        public AppleTvSimulatorSdk(NPath sdkPath, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(sdkPath, version, xcodePath, isDownloadable, backendRegistrables) {}
    }

    public class AppleTvToolchain : XcodeToolchain
    {
        public AppleTvToolchain(AppleTvSdk sdk, string minOSVersion = "8.0", StdLib stdlib = StdLib.libcpp)
            : base(sdk, new Arm64Architecture(), minOSVersion, stdlib)
        {
        }

        public AppleTvToolchain(AppleTvSimulatorSdk sdk, string minOSVersion = "8.0", StdLib stdlib = StdLib.libcpp)
            : base(sdk, new x64Architecture(), minOSVersion, stdlib)
        {
        }
    }
}
