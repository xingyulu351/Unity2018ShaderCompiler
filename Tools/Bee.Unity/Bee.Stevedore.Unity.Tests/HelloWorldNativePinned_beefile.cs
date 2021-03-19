// This is supposed to represent a "best practice" example of creating
// toolchains using specific ("pinned") Stevedore artifacts (rather than
// Bee defaults).
// TODO: Make less awful.

using System;
using Bee.Stevedore;
using Bee.Toolchain.Windows;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

class HelloWorldNativePinned
{
    static void Main()
    {
        var np = new NativeProgram("HelloWorld");
        np.Sources.Add("src");

        ToolChain toolchain;
        if (HostPlatform.IsWindows)
        {
            var win10Sdk = StevedoreArtifact.UnityInternal("win10sdk/10.0.10586.0_913ab7b550148fb64e28886ff7a607d32449213a702c46c536c972dc00a688ec.7z");
            var vs2017 = StevedoreArtifact.UnityInternal("vs2017-toolchain/14.13.26128.0_767d8a2f0e7a5bb2f7962dcb27e45f0d252133a4aa2d4a8b8b3b1a35af2a9029.7z");
            toolchain = new WindowsToolchain(DownloadableWindowsSdk.FromVs2017(Architecture.x64, vs2017, win10Sdk));
        }
        else if (HostPlatform.IsOSX)
        {
            var macToolchain10_12 = StevedoreArtifact.UnityInternal("mac-toolchain-10_12/8.0.0-8.1-8B62_e4686b94eaf1a3c1d19bb5fd451944754f62dbbd477b8303cbbae0cbfffb696f.7z");
            toolchain = new MacToolchain(new UnityMacSdk(macToolchain10_12.Path, true, new[] { macToolchain10_12 }), Architecture.x64);
        }
        else if (HostPlatform.IsLinux)
        {
            throw new NotImplementedException("Steveification of Linux toolchains pending ongoing reworking of Linux build process");
        }
        else throw new InvalidOperationException("This case should not be possible");

        var nativeProgramConfiguration = new NativeProgramConfiguration(CodeGen.Debug, toolchain, lump: true);
        np.SetupSpecificConfiguration(nativeProgramConfiguration, toolchain.ExecutableFormat, targetDirectory: "build");
    }
}
