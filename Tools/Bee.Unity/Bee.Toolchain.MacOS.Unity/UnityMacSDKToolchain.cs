using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.NativeProgramSupport;
using Bee.NativeProgramSupport.Building.FluentSyntaxHelpers;
using Bee.Stevedore;
using Bee.Toolchain.Xcode;
using NiceIO;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.MacSDKSupport
{
    public sealed class UnityMacSDKToolChain : MacToolchain
    {
        public static NPath EmbeddedSdkPath { get; } = Paths.UnityRoot.Combine("PlatformDependent/OSX/External/NonRedistributable/BuildEnvironment/builds/");

        public override string LegacyPlatformIdentifier => $"macosx{Architecture.Bits}";
        public override string ActionName => $"{Platform.DisplayName}{Architecture.Bits}";

        public UnityMacSDKToolChain(IntelArchitecture architecture, string minOSVersion = null, StdLib stdlib = StdLib.libcpp)
            : base(new UnityMacSdk(EmbeddedSdkPath, false), architecture, minOSVersion ?? "10.9", stdlib)
        {
        }
    }

    public class UnityDownloadableMacSdkProvider : DownloadableSdkProvider
    {
        public static readonly Lazy<MacSdk> MacSdk_10_12 = new Lazy<MacSdk>(() =>
        {
            // TODO: this appears redundant - but inconsistent - with MacSdkLocator?!
            var sdk = StevedoreArtifact.UnityInternal("mac-toolchain-10_12/8.0.0-8.1-8B62_e4686b94eaf1a3c1d19bb5fd451944754f62dbbd477b8303cbbae0cbfffb696f.7z");
            //todo: figure out why we need xcode.app path here.
            return new MacSdk(sdk.Path, new Version(10, 12), "/Applications/Xcode.app", true, new[] { sdk });
        });

        public override IEnumerable<Sdk> Provide()
        {
            if (!HostPlatform.IsOSX || Backend.Current is IJamBackend)
                yield break;
            yield return MacSdk_10_12.Value;
        }
    }
}
