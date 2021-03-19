using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using Bee.TundraBackend.Tests;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.ToolchainTestInfrastructure
{
    public abstract class SingleToolchainTests : NativeProgramAndToolchainTestBase
    {
        private BeeAPITestBackend_Tundra<ToolChain> BeeApiTestBackendTundra;
        protected abstract ToolChain ProduceToolchainToTest();

        [SetUp]
        public void SetUp()
        {
            BeeApiTestBackendTundra = new BeeAPITestBackend_Tundra<ToolChain>();
            var toolchain = ProduceToolchainToTest();
            if (toolchain.CanBuild)
                ManualFixtureSetup(toolchain, BeeApiTestBackendTundra);
            else
                Assert.Ignore($"Toolchain {toolchain} not supported on host platform");
        }
    }
}
