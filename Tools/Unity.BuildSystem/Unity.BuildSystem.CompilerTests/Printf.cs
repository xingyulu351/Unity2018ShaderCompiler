using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class PrintfTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName("printf prints to stdout")]
        public void Printf(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
#include <cstdio>
int main()
{
    printf(""hello"");
}";

            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(
                toolChain => SetupNativeProgramAndRun(toolChain),
                expectedOutput: "hello");
        }
    }
}
