using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    [TestFixture]
    public class Defines : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Test(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(@"
int main() {
#if MYDEFINE
    // All is well
#else
    #error ""Missed '#if MYDEFINE' condition""
#endif
}
");
            Build(toolChain => SetupNativeProgram(toolChain, np => np.Defines.Add("MYDEFINE")));
        }
    }
}
