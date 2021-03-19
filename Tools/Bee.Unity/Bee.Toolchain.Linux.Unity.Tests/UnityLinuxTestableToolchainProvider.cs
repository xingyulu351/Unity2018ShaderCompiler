using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;
using Bee.Toolchain.Linux.Unity;

namespace Bee.Toolchain.Linux.Unity.Tests
{
    //linux doesn't follow regular pattern yet, as there is some actual buildcode in the unity buildprogram that uses their toolchain provider
    public class UnityLinuxTestableToolchainProvider : TestableToolchainProvider
    {
        readonly UnityLinuxToolchainProvider _nestedProvider = new UnityLinuxToolchainProvider();

        public override IEnumerable<ToolChain> Provide() => _nestedProvider.Provide();
    }
}
