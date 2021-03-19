using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Building.FluentSyntaxHelpers;
using Bee.Toolchain.Xcode;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.MacSDKSupport
{
    public class MacToolchain : XcodeToolchain
    {
        public override string LegacyPlatformIdentifier => $"macosx{Architecture.Bits}";
        public override string ActionName => $"{Platform.DisplayName}{Architecture.Bits}";

        public MacToolchain(MacSdk sdk, Architecture architecture, string minOSVersion = "10.8", StdLib stdlib = StdLib.libcpp)
            : base(sdk, architecture, minOSVersion, stdlib)
        {
        }

        protected override IEnumerable<ObjectFileProducer> ProvideObjectFileProducers()
        {
            foreach (var s in base.ProvideObjectFileProducers())
                yield return s;
            yield return new YasmAssembler(this);
            yield return new NasmAssembler(this);
        }
    }

    public sealed class FluentMacSdk
    {
        private MacSdk Sdk { get; }
        internal FluentMacSdk(MacSdk sdk) => Sdk = sdk;

        public MacToolchain x86(string minOSVersion) => new MacToolchain(Sdk, Architecture.x86, minOSVersion: minOSVersion);
        public MacToolchain x64(string minOSVersion) => new MacToolchain(Sdk, Architecture.x64, minOSVersion: minOSVersion);
    }
}

public static class FluentMacExtensions
{
    public static FluentMacSdk Sdk_10_12(this FluentPlatform<MacOSXPlatform> _) => Find(new Version(10, 12));
    public static FluentMacSdk Sdk_10_13(this FluentPlatform<MacOSXPlatform> _) => Find(new Version(10, 13));
    public static FluentMacSdk Sdk_10_14(this FluentPlatform<MacOSXPlatform> _) => Find(new Version(10, 14));

    private static FluentMacSdk Find(Version version) => new FluentMacSdk(MacSdk.Locator.FindSdkInSystemOrDownloads(version));
}
