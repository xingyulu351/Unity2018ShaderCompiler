using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class IncludeDirectory : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Test(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            TestRoot.Combine("subfolder").EnsureDirectoryExists().Combine("myheader.h").WriteAllText("#define VALUE \"14\"");
            MainCpp.WriteAllText(@"
#include ""myheader.h""
int main()
{
    #pragma message(""Test message: "" VALUE)
}");
            var result = Build(toolChain => SetupNativeProgram(toolChain, np => np.IncludeDirectories.Add("subfolder")));
            Assert.That(result.StdOut, Does.Contain("Test message: 14"));
        }
    }
}
