using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.MacSDKSupport
{
    class MacTestableToolchainProvider : TestableToolchainProvider
    {
        private static readonly MacToolchain x86 = new MacToolchain(MacSdk.Locator.UserDefaultOrLatest, new x86Architecture()) {CreatingFunc = () => x86};
        private static readonly MacToolchain x64 = new MacToolchain(MacSdk.Locator.UserDefaultOrLatest, new x64Architecture()) {CreatingFunc = () => x64};

        public override IEnumerable<ToolChain> Provide() => new[] {x86, x64};
    }
}
