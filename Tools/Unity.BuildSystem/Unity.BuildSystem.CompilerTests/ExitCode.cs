using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class ExitCodeTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName("int main() exit code is the process exit code")]
        public void ProgramReturnsSpecifiedExitCode(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText("int main() { return 3; }");
            Build(toolChain => SetupNativeProgramAndRun(toolChain, ignoreProgramExitCode: true));
            Assert.AreEqual("3", TestRoot.Combine("program.result").ReadAllText().Trim());
        }
    }
}
