using System.Collections.Generic;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.UWP.Tests
{
    class UWPTestableToolchainProvider : TestableToolchainProvider
    {
        static ToolChain x64_vs15() => new UWPToolchain(UWPSdk.Locatorx64.All.LatestMatching(15, 0) ?? UWPSdk.Locatorx64.Dummy) {CreatingFunc = x64_vs15};
        static ToolChain x86_vs15() => new UWPToolchain(UWPSdk.Locatorx86.All.LatestMatching(15, 0) ?? UWPSdk.Locatorx86.Dummy) {CreatingFunc = x86_vs15};

        public override IEnumerable<ToolChain> Provide() => new[] {x64_vs15(), x86_vs15()};
    }
}
