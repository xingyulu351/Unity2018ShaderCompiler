using System.Collections.Generic;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Windows.Tests
{
    class WindowsTestableToolchainProvider : TestableToolchainProvider
    {
        static ToolChain x64_vs14() => new WindowsToolchain(WindowsSdk.Locatorx64.All.LatestMatching(14, 0) ?? WindowsSdk.Locatorx64.Dummy) {CreatingFunc = x64_vs14};
        static ToolChain x86_vs14() => new WindowsToolchain(WindowsSdk.Locatorx86.All.LatestMatching(14, 0) ?? WindowsSdk.Locatorx86.Dummy) {CreatingFunc = x86_vs14};
        static ToolChain x64_vs15() => new WindowsToolchain(WindowsSdk.Locatorx64.All.LatestMatching(15, 0) ?? WindowsSdk.Locatorx64.Dummy) {CreatingFunc = x64_vs15};
        static ToolChain x86_vs15() => new WindowsToolchain(WindowsSdk.Locatorx86.All.LatestMatching(15, 0) ?? WindowsSdk.Locatorx86.Dummy) {CreatingFunc = x86_vs15};

        public override IEnumerable<ToolChain> Provide() => new[] {x64_vs14(), x86_vs14(), x64_vs15(), x86_vs15()};
    }
}
