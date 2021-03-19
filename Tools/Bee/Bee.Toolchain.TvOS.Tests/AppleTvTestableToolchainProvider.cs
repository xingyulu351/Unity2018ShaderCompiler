using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.TvOS.Tests
{
    class AppleTvTestableToolchainProvider : TestableToolchainProvider
    {
        static readonly ToolChain _regular = new AppleTvToolchain(AppleTvSdk.Locator.UserDefaultOrDummy) { CreatingFunc = () => _regular};
        static readonly ToolChain _simulator = new AppleTvToolchain(AppleTvSimulatorSdk.Locator.UserDefaultOrDummy)  { CreatingFunc = () => _simulator};

        public override IEnumerable<ToolChain> Provide() => new[] {_regular, _simulator};
    }
}
