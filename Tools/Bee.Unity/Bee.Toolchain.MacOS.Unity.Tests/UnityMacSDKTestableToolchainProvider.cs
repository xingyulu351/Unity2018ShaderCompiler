using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.MacSDKSupport
{
    class UnityMacSDKTestableToolchainProvider : TestableToolchainProvider
    {
        private static readonly UnityMacSDKToolChain x86 = new UnityMacSDKToolChain(new x86Architecture()) {CreatingFunc = () => x86};
        private static readonly UnityMacSDKToolChain x64 = new UnityMacSDKToolChain(new x64Architecture()) {CreatingFunc = () => x64};

        public override IEnumerable<ToolChain> Provide() => new[] {x86, x64};
    }
}
