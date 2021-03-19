using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.Stevedore;
using Bee.Toolchain.Xcode;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.IOS
{
    internal static class EmbeddedXcode
    {
        public static readonly NPath Path = Paths.UnityRoot.Combine("PlatformDependent/iPhonePlayer/External/NonRedistributable/BuildEnvironment/builds/Xcode.app");
    }

    public class IOSSdkLocator : XcodeSdkLocator<IOSSdk>
    {
        protected override Platform Platform { get; } = new IosPlatform();
        protected override string XcodePlatformName { get; } = "iPhoneOS";

        public override IOSSdk UserDefault => UnityEmbedded ?? base.UserDefault;
        public IOSSdk UnityEmbedded => DefaultSdkFromXcodeApp(EmbeddedXcode.Path);

        protected override IOSSdk NewSdkInstance(NPath path, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            => new IOSSdk(path, version, xcodePath, isDownloadable, backendRegistrables);

        protected override IEnumerable<IOSSdk> ScanForDownloadableSdks()
        {
            var sdk10_1 = StevedoreArtifact.UnityInternal("ios-tvos-toolchain/8.0.0-8.1-8B62-10.1-10.0_06b8196fda0ad9cdd39fba0ed45a6e30ce5fe1c85c7992a42cc068a67b45baca.7z");
            yield return NewDefaultSdkInstanceFromXcodeApp(sdk10_1.Path.Combine("Xcode.app"), new Version(10, 1), true, new[] { sdk10_1 });
        }
    }

    public class IOSSimulatorSdkLocator : XcodeSdkLocator<IOSSimulatorSdk>
    {
        protected override Platform Platform { get; } = new IosPlatform();
        protected override string XcodePlatformName { get; } = "iPhoneSimulator";

        public override IOSSimulatorSdk UserDefault => UnityEmbedded ?? base.UserDefault;
        public IOSSimulatorSdk UnityEmbedded => DefaultSdkFromXcodeApp(EmbeddedXcode.Path);

        protected override IOSSimulatorSdk NewSdkInstance(NPath path, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            => new IOSSimulatorSdk(path, version, xcodePath, isDownloadable, backendRegistrables);

        protected override IEnumerable<IOSSimulatorSdk> ScanForDownloadableSdks()
        {
            var sdk10_1 = StevedoreArtifact.UnityInternal("ios-tvos-toolchain/8.0.0-8.1-8B62-10.1-10.0_06b8196fda0ad9cdd39fba0ed45a6e30ce5fe1c85c7992a42cc068a67b45baca.7z");
            yield return NewDefaultSdkInstanceFromXcodeApp(sdk10_1.Path.Combine("Xcode.app"), new Version(10, 1), true, new[] { sdk10_1 });
        }
    }

    public class IOSSdk : XcodeSdk
    {
        public static IOSSdkLocator Locator { get; } = new IOSSdkLocator();

        public override Platform Platform => new IosPlatform();
        public IOSSdk(NPath sdkPath, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(sdkPath, version, xcodePath, isDownloadable, backendRegistrables) {}
    }

    public class IOSSimulatorSdk : XcodeSdk
    {
        public static IOSSimulatorSdkLocator Locator { get; } = new IOSSimulatorSdkLocator();

        public override Platform Platform => new IosPlatform();
        public IOSSimulatorSdk(NPath sdkPath, Version version, NPath xcodePath, bool isDownloadable, IEnumerable<IBackendRegistrable> backendRegistrables = null)
            : base(sdkPath, version, xcodePath, isDownloadable, backendRegistrables) {}
    }

    public class IOSToolchain : XcodeToolchain
    {
        public IOSToolchain(IOSSdk sdk, ArmArchitecture architecture, string minOSVersion = "8.0", StdLib stdlib = StdLib.libcpp)
            : base(sdk, architecture, minOSVersion, stdlib)
        {
        }

        public IOSToolchain(IOSSimulatorSdk sdk, IntelArchitecture architecture, string minOSVersion = "8.0", StdLib stdlib = StdLib.libcpp)
            : base(sdk, architecture, minOSVersion, stdlib)
        {
        }
    }
}
