using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Lumin.Tests
{
    public class LuminTestableToolchainProvider : TestableToolchainProvider
    {
        static ToolChain AArch64() => new LuminToolchain(LuminSdk.Locator.Latest) {CreatingFunc = AArch64};

        public override IEnumerable<ToolChain> Provide() => new[] {AArch64()};
    }
}
